/*
 * ntp_timer.c - event timer support routines
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/signal.h>

#include "ntp_machine.h"                                                                                                /*  98/06/03  */
#include "ntpd.h"
#include "ntp_stdlib.h"

/*
 * These routines provide support for the event timer.	The timer is
 * implemented by an interrupt routine which sets a flag once every
 * 2**EVENT_TIMEOUT seconds (currently 4), and a timer routine which
 * is called when the mainline code gets around to seeing the flag.
 * The timer routine dispatches the clock adjustment code if its time
 * has come, then searches the timer queue for expiries which are
 * dispatched to the transmit procedure.  Finally, we call the hourly
 * procedure to do cleanup and print a message.
 */

/*
 * Alarm flag.	The mainline code imports this.
 */
volatile int alarm_flag;

/*
 * The counters
 */
static	u_long adjust_timer;		/* second timer */
static	u_long keys_timer;		/* minute timer */
static	u_long hourly_timer;		/* hour timer */
static	u_long revoke_timer;		/* keys revoke timer */
u_long	sys_revoke = KEY_REVOKE;	/* keys revoke timeout */

/*
 * Imported from ntp_peer
 */
extern struct peer *peer_hash[];

/*
 * Statistics counter for the interested.
 */
volatile u_long alarm_overflow;

#define MINUTE	60
#define HOUR	(60*60)

u_long current_time;

/*
 * Stats.  Number of overflows and number of calls to transmit().
 */
u_long timer_timereset;
u_long timer_overflows;
u_long timer_xmtcalls;

#ifndef SYS_WINNT
static	RETSIGTYPE alarming P((int));
#endif /* SYS_WINNT */

#if defined(VMS)
static int vmstimer[2]; 	/* time for next timer AST */
static int vmsinc[2];		/* timer increment */
#endif /* VMS */

/*
 * init_timer - initialize the timer data structures
 */
void
init_timer(void)
{
#if !defined(VMS)
# ifndef SYS_WINNT
#  ifndef HAVE_TIMER_SETTIME
	struct itimerval itimer;
#  else
	static timer_t ntpd_timerid;	/* should be global if we ever want */
					/* to kill timer without rebooting ... */
	struct itimerspec itimer;
#  endif
# else /* SYS_WINNT */
	TIMECAPS tc;
	extern HANDLE hMutex;
	UINT wTimerRes, wTimerID;
# endif /* SYS_WINNT */
#if defined(SYS_CYGWIN32) || defined(SYS_WINNT) 																		/*	98/06/03  */
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
#endif
#endif /* VMS */

	/*
	 * Initialize...
	 */
	alarm_flag = 0;
	alarm_overflow = 0;
	adjust_timer = 1;
	hourly_timer = HOUR;
	current_time = 0;
	timer_overflows = 0;
	timer_xmtcalls = 0;
	timer_timereset = 0;

#ifndef SYS_WINNT
	/*
	 * Set up the alarm interrupt.	The first comes 2**EVENT_TIMEOUT
	 * seconds from now and they continue on every 2**EVENT_TIMEOUT
	 * seconds.
	 */
# if !defined(VMS)
#  if defined(HAVE_TIMER_CREATE) && defined(HAVE_TIMER_SETTIME)
	if (timer_create (CLOCK_REALTIME, NULL, &ntpd_timerid) ==
#	ifdef SYS_VXWORKS
		ERROR
#	else
		-1
#	endif
	   )
	{
		fprintf (stderr, "timer create FAILED\n");
		exit (0);
	}
	(void) signal_no_reset(SIGALRM, alarming);
	itimer.it_interval.tv_sec = itimer.it_value.tv_sec = (1<<EVENT_TIMEOUT);
	itimer.it_interval.tv_nsec = itimer.it_value.tv_nsec = 0;
	timer_settime(ntpd_timerid, 0 /*!TIMER_ABSTIME*/, &itimer, NULL);
#  else
	(void) signal_no_reset(SIGALRM, alarming);
	itimer.it_interval.tv_sec = itimer.it_value.tv_sec = (1<<EVENT_TIMEOUT);
	itimer.it_interval.tv_usec = itimer.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itimer, (struct itimerval *)0);
#  endif
# else /* VMS */
	vmsinc[0] = 10000000;		/* 1 sec */
	vmsinc[1] = 0;
	lib$emul(&(1<<EVENT_TIMEOUT), &vmsinc, &0, &vmsinc);

	sys$gettim(&vmstimer);	/* that's "now" as abstime */

	lib$addx(&vmsinc, &vmstimer, &vmstimer);
	sys$setimr(0, &vmstimer, alarming, alarming, 0);
# endif /* VMS */
#ifdef SYS_CYGWIN32
	/*
	 * Get privileges needed for fiddling with the clock
	 */

	/* get the current process token handle */
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		msyslog(LOG_ERR, "OpenProcessToken failed: %m");
		exit(1);
	}
	/* get the LUID for system-time privilege. */
	LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;  /* one privilege to set */
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	/* get set-time privilege for this process. */
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);
	/* cannot test return value of AdjustTokenPrivileges. */
	if (GetLastError() != ERROR_SUCCESS)
		msyslog(LOG_ERR, "AdjustTokenPrivileges failed: %m");
#endif
#else /* SYS_WINNT */
	_tzset();

	/*
	 * Get privileges needed for fiddling with the clock
	 */

	/* get the current process token handle */
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		msyslog(LOG_ERR, "OpenProcessToken failed: %m");
		exit(1);
	}
	/* get the LUID for system-time privilege. */
	LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;  /* one privilege to set */
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	/* get set-time privilege for this process. */
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);
	/* cannot test return value of AdjustTokenPrivileges. */
	if (GetLastError() != ERROR_SUCCESS)
		msyslog(LOG_ERR, "AdjustTokenPrivileges failed: %m");

	/*
	 * Set up timer interrupts for every 2**EVENT_TIMEOUT seconds
	 * Under Windows/NT, expiry of timer interval leads to invocation
	 * of a callback function (on a different thread) rather than
	 * generating an alarm signal
	 */

	/* determine max and min resolution supported */
	if(timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
		msyslog(LOG_ERR, "timeGetDevCaps failed: %m");
		exit(1);
	}
	wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
	/* establish the minimum timer resolution that we'll use */
	timeBeginPeriod(wTimerRes);

	hMutex = CreateMutex(
		NULL,				/* no security attributes */
		FALSE,				/* initially not owned */
		"MutexForNTP");     /* name of mutex */
	if (hMutex == NULL) {
		msyslog(LOG_ERR, "cannot create a mutex: %m\n");
		exit(1);
	}

	/* start the timer event */
	wTimerID = timeSetEvent(
		(1<<EVENT_TIMEOUT) * 1000,	 /* Delay in ms */
		wTimerRes,					 /* Resolution */
		(LPTIMECALLBACK) alarming,	 /* Callback function */
		(DWORD) 0,					 /* User data */
		TIME_PERIODIC); 		  /* Event type (periodic) */
	if (wTimerID == 0) {
		msyslog(LOG_ERR, "timeSetEvent failed: %m");
		exit(1);
	}
#endif /* SYS_WINNT */
}


/*
 * timer - dispatch anyone who needs to be
 */
void
timer(void)
{
	register struct peer *peer, *next_peer;
	int n;
#ifdef SYS_WINNT																										/*	98/05/29  */
	extern HANDLE hMutex;																								/*	98/06/03  */
	DWORD dwWaitResult; 																								/*	98/06/03  */
																														/*	98/06/03  */
	dwWaitResult = WaitForSingleObject( 																				/*	98/06/03  */
		hMutex, 	/* handle of mutex */																				/*	98/06/03  */
		5000L); /* five-second time-out interval */ 																	/*	98/06/03  */
																														/*	98/06/03  */
	switch (dwWaitResult) { 																							/*	98/06/03  */
		case WAIT_OBJECT_0: 																							/*	98/06/03  */
			/* The thread got mutex ownership. */																		/*	98/06/03  */
			break;																										/*	98/06/03  */
		default:																										/*	98/06/03  */
			/* Cannot get mutex ownership due to time-out. */															/*	98/06/03  */
			msyslog(LOG_ERR, "timer() cannot obtain mutex: %m\n");                                                      /*  98/06/03  */
			exit(1);																									/*	98/06/03  */
	}																													/*	98/06/03  */
#endif																													/*	98/05/29  */

	current_time += (1<<EVENT_TIMEOUT);

	/*
	 * Adjustment timeout first.
	 */
	if (adjust_timer <= current_time) {
		adjust_timer += 1;
		adj_host_clock();
	}
																														/*	98/05/29  */
#ifdef SYS_WINNT																										/*	98/05/29  */
	if (!ReleaseMutex(hMutex)) {																						/*	98/05/29  */
		msyslog(LOG_ERR, "timer() cannot release mutex: %m\n");                                                         /*  98/05/29  *//*  98/06/03  */
		exit(1);																										/*	98/05/29  */
	}																													/*	98/05/29  */
#endif /* SYS_WINNT */

	/*
	 * Now dispatch any peers whose event timer has expired. Be careful
	 * here, since the peer structure might go away as the result of
	 * the call.
	 */
	for (n = 0; n < HASH_SIZE; n++) {
		for (peer = peer_hash[n]; peer != 0; peer = next_peer) {
			next_peer = peer->next;
			if (peer->action && peer->nextaction <= current_time)
	  			peer->action(peer);
			if (peer->nextdate <= current_time) {
#ifdef REFCLOCK
				if (peer->flags & FLAG_REFCLOCK)
					refclock_transmit(peer);
				else
					transmit(peer);
#else /* REFCLOCK */
				transmit(peer);
#endif /* REFCLOCK */
			}
		}
	}

	/*
	 * Garbage collect expired keys.
	 */
	if (keys_timer <= current_time) {
		keys_timer += MINUTE;
		auth_agekeys();
	}

	/*
	 * Garbage collect revoked keys
	 */
	if (revoke_timer <= current_time) {
		revoke_timer += RANDPOLL(sys_revoke);
		key_expire_all();
	}

	/*
	 * Finally, call the hourly routine.
	 */
	if (hourly_timer <= current_time) {
		hourly_timer += HOUR;
		hourly_stats();
	}
}


#ifndef SYS_WINNT
/*
 * alarming - tell the world we've been alarmed
 */
static RETSIGTYPE
alarming(
	int sig
	)
{
	extern int initializing;	/* from main line code */

#if !defined(VMS)
	if (initializing)
		return;
	if (alarm_flag)
		alarm_overflow++;
	else
		alarm_flag++;
#else /* VMS AST routine */
	if (!initializing) {
		if (alarm_flag) alarm_overflow++;
		else alarm_flag = 1;	/* increment is no good */
	}
	lib$addx(&vmsinc,&vmstimer,&vmstimer);
	sys$setimr(0,&vmstimer,alarming,alarming,0);
#endif /* VMS */
}
#endif /* SYS_WINNT */


/*
 * timer_clr_stats - clear timer module stat counters
 */
void
timer_clr_stats(void)
{
	timer_overflows = 0;
	timer_xmtcalls = 0;
	timer_timereset = current_time;
}
