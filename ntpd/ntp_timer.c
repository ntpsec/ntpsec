/*
 * ntp_timer.c - event timer support routines
 */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_stdlib.h"

#include <stdio.h>
#include <signal.h>
#ifdef HAVE_SYS_SIGNAL_H
# include <sys/signal.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#if defined(HAVE_IO_COMPLETION_PORT)
# include "ntp_iocompletionport.h"
# include "ntp_timer.h"
#endif

#ifdef KERNEL_PLL
#include "ntp_syscall.h"
#endif /* KERNEL_PLL */

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
volatile int interface_interval = 300;     /* update interface every 5 minutes as default */
	  
/*
 * Alarm flag. The mainline code imports this.
 */
volatile int alarm_flag;

/*
 * The counters and timeouts
 */
static	u_long adjust_timer;	/* second timer */
static	u_long stats_timer;	/* stats timer */
static	u_long huffpuff_timer;	/* huff-n'-puff timer */
static  u_long interface_timer;	/* interface update timer */
#ifdef OPENSSL
static	u_long revoke_timer;	/* keys revoke timer */
u_long	sys_revoke = KEY_REVOKE; /* keys revoke timeout */
static	u_long keys_timer;	/* session key timer */
u_long	sys_automax = NTP_AUTOMAX; /* session key timeout */
#endif /* OPENSSL */

/*
 * Statistics counter for the interested.
 */
volatile u_long alarm_overflow;

#define MINUTE	60
#define HOUR	(60 * MINUTE)
#define	DAY	(24 * HOUR)

u_long current_time;		/* seconds since startup */

/*
 * Stats.  Number of overflows and number of calls to transmit().
 */
u_long timer_timereset;
u_long timer_overflows;
u_long timer_xmtcalls;

#if defined(VMS)
static int vmstimer[2]; 	/* time for next timer AST */
static int vmsinc[2];		/* timer increment */
#endif /* VMS */

#if defined SYS_WINNT
static HANDLE WaitableTimerHandle = NULL;
#else
static	RETSIGTYPE alarming (int);
#endif /* SYS_WINNT */

#if !defined(VMS)
# if !defined SYS_WINNT || defined(SYS_CYGWIN32)
#  ifndef HAVE_TIMER_SETTIME
	struct itimerval itimer;
#  else 
	static timer_t ntpd_timerid;
	struct itimerspec itimer;
#  endif /* HAVE_TIMER_SETTIME */
# endif /* SYS_WINNT */
#endif /* VMS */

/*
 * reinit_timer - reinitialize interval timer.
 */
void 
reinit_timer(void)
{
#if !defined(SYS_WINNT) && !defined(VMS)
#  if defined(HAVE_TIMER_CREATE) && defined(HAVE_TIMER_SETTIME)
	timer_gettime(ntpd_timerid, &itimer);
	if (itimer.it_value.tv_sec < 0 || itimer.it_value.tv_sec > (1<<EVENT_TIMEOUT)) {
		itimer.it_value.tv_sec = (1<<EVENT_TIMEOUT);
	}
	if (itimer.it_value.tv_nsec < 0 ) {
		itimer.it_value.tv_nsec = 0;
	}
	if (itimer.it_value.tv_sec == 0 && itimer.it_value.tv_nsec == 0) {
		itimer.it_value.tv_sec = (1<<EVENT_TIMEOUT);
		itimer.it_value.tv_nsec = 0;
	}
	itimer.it_interval.tv_sec = (1<<EVENT_TIMEOUT);
	itimer.it_interval.tv_nsec = 0;
	timer_settime(ntpd_timerid, 0 /*!TIMER_ABSTIME*/, &itimer, NULL);
#  else
	getitimer(ITIMER_REAL, &itimer);
	if (itimer.it_value.tv_sec < 0 || itimer.it_value.tv_sec > (1<<EVENT_TIMEOUT)) {
		itimer.it_value.tv_sec = (1<<EVENT_TIMEOUT);
	}
	if (itimer.it_value.tv_usec < 0 ) {
		itimer.it_value.tv_usec = 0;
	}
	if (itimer.it_value.tv_sec == 0 && itimer.it_value.tv_usec == 0) {
		itimer.it_value.tv_sec = (1<<EVENT_TIMEOUT);
		itimer.it_value.tv_usec = 0;
	}
	itimer.it_interval.tv_sec = (1<<EVENT_TIMEOUT);
	itimer.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &itimer, (struct itimerval *)0);
#  endif
# endif /* VMS */
}

/*
 * init_timer - initialize the timer data structures
 */
void
init_timer(void)
{
# if defined SYS_WINNT & !defined(SYS_CYGWIN32)
	HANDLE hToken = INVALID_HANDLE_VALUE;
	TOKEN_PRIVILEGES tkp;
# endif /* SYS_WINNT */

	/*
	 * Initialize...
	 */
	alarm_flag = 0;
	alarm_overflow = 0;
	adjust_timer = 1;
	stats_timer = 0;
	huffpuff_timer = 0;
	interface_timer = 0;
	current_time = 0;
	timer_overflows = 0;
	timer_xmtcalls = 0;
	timer_timereset = 0;

#if !defined(SYS_WINNT)
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
	if (GetLastError() != ERROR_SUCCESS) {
		msyslog(LOG_ERR, "AdjustTokenPrivileges failed: %m");
	}

	/*
	 * Set up timer interrupts for every 2**EVENT_TIMEOUT seconds
	 * Under Windows/NT, 
	 */

	WaitableTimerHandle = CreateWaitableTimer(NULL, FALSE, NULL);
	if (WaitableTimerHandle == NULL) {
		msyslog(LOG_ERR, "CreateWaitableTimer failed: %m");
		exit(1);
	}
	else {
		DWORD Period = (1<<EVENT_TIMEOUT) * 1000;
		LARGE_INTEGER DueTime;
		DueTime.QuadPart = Period * 10000i64;
		if (!SetWaitableTimer(WaitableTimerHandle, &DueTime, Period, NULL, NULL, FALSE) != NO_ERROR) {
			msyslog(LOG_ERR, "SetWaitableTimer failed: %m");
			exit(1);
		}
	}

#endif /* SYS_WINNT */
}

#if defined(SYS_WINNT)
extern HANDLE 
get_timer_handle(void)
{
	return WaitableTimerHandle;
}
#endif

/*
 * timer - event timer
 */
void
timer(void)
{
	register struct peer *peer, *next_peer;
	u_int	n;
	l_fp	now;

	/*
	 * The basic timerevent is one second. This is used to adjust
	 * the system clock in time and frequency, implement the
	 * kiss-o'-deatch function and implement the association
	 * polling function..
	 */
	current_time += (1<<EVENT_TIMEOUT);
	if (adjust_timer <= current_time) {
		adjust_timer += 1;
		adj_host_clock();
		kod_proto();
#ifdef REFCLOCK
		for (n = 0; n < NTP_HASH_SIZE; n++) {
			for (peer = peer_hash[n]; peer != 0; peer = next_peer) {
				next_peer = peer->next;
				if (peer->flags & FLAG_REFCLOCK)
					refclock_timer(peer);
			}
		}
#endif /* REFCLOCK */
	}

	/*
	 * Now dispatch any peers whose event timer has expired. Be
	 * careful here, since the peer structure might go away as the
	 * result of the call.
	 */
	for (n = 0; n < NTP_HASH_SIZE; n++) {
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
	 * Leapseconds. When the time remaining decrements to zero,
	 * increment the TAI offset. If the kernel code is not available
	 * or disabled, Do the leap crudely. There are of course races
	 * here, cheerfully ignored.
	 */
	if (leap_sec > 0) {
		get_systime(&now);
		if (now.l_ui > leap_sec) {
			sys_leap = LEAP_NOWARNING;
			sys_tai = leap_tai;
		} else if (now.l_ui - leap_sec < 28 * 86400) {
			sys_leap = LEAP_ADDSECOND;
			if (leap_tai > 0)
				sys_tai = leap_tai - 1;
		}
#ifdef KERNEL_PLL
		if (now.l_ui - leap_sec == 600) {
			if (pll_control && kern_enable)
				loop_config(LOOP_LEAP, 0);
 		}
#endif /* KERNEL_PLL */
		if (leap_sec == 0) {
#ifdef KERNEL_PLL
			if (!(pll_control && kern_enable))
				step_systime(-1.0);
#else /* KERNEL_PLL */
				step_systime(-1.0);
#endif /* KERNEL_PLL */
			msyslog(LOG_NOTICE,
			    "timer: leap second %lu TAI %d",
				leap_sec, sys_tai);
		}
	}

	/*
	 * Update huff-n'-puff filter.
	 */
	if (huffpuff_timer <= current_time) {
		huffpuff_timer += HUFFPUFF;
		huffpuff();
	}

#ifdef OPENSSL

	/*
	 * Garbage collect expired keys.
	 */
	if (keys_timer <= current_time) {
		keys_timer += sys_automax;
		auth_agekeys();
	}

	/*
	 * Garbage collect key list and generate new private value. The
	 * timer runs only after initial synchronization and fires about
	 * once per day.
	 */
	if (revoke_timer <= current_time && sys_leap !=
	    LEAP_NOTINSYNC) {
		revoke_timer += sys_revoke;
		expire_all();
	}
#endif /* OPENSSL */

	/*
	 * Interface update timer
	 */
	if (interface_interval && interface_timer <= current_time) {
		timer_interfacetimeout(current_time +
		    interface_interval);
#ifdef DEBUG
	  	if (debug > 1)
	 	 	printf("timer: interface update\n");
#endif
		interface_update(NULL, NULL);
	}
	
	/*
	 * Finally, write stats once per hour.
	 */
	if (stats_timer <= current_time) {
		stats_timer += HOUR;
		write_stats();
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

void
timer_interfacetimeout(u_long timeout)
{
	interface_timer = timeout;
}


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

