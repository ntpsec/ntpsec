/*
 * ntp_timer.c - event timer support routines
 */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_stdlib.h"

#if defined(HAVE_IO_COMPLETION_PORT)
# include "ntp_iocompletionport.h"
# include "ntp_timer.h"
#endif

#include <stdio.h>
#include <signal.h>
#ifdef HAVE_SYS_SIGNAL_H
# include <sys/signal.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef KERNEL_PLL
#include "ntp_syscall.h"
#endif /* KERNEL_PLL */

#ifdef AUTOKEY
#include <openssl/rand.h>
#endif	/* AUTOKEY */


/* TC_ERR represents the timer_create() error return value. */
#ifdef SYS_VXWORKS
#define	TC_ERR	ERROR
#else
#define	TC_ERR	(-1)
#endif

/*
 * These routines provide support for the event timer.  The timer is
 * implemented by an interrupt routine which sets a flag once every
 * second, and a timer routine which is called when the mainline code
 * gets around to seeing the flag.  The timer routine dispatches the
 * clock adjustment code if its time has come, then searches the timer
 * queue for expiries which are dispatched to the transmit procedure.
 * Finally, we call the hourly procedure to do cleanup and print a
 * message.
 */
volatile int interface_interval;     /* init_io() sets def. 300s */

/*
 * Alarm flag. The mainline code imports this.
 */
volatile int alarm_flag;

/*
 * The counters and timeouts
 */
static  u_long interface_timer;	/* interface update timer */
static	u_long adjust_timer;	/* second timer */
static	u_long stats_timer;	/* stats timer */
static	u_long huffpuff_timer;	/* huff-n'-puff timer */
static	u_long worker_idle_timer;/* next check for idle intres */
u_long	leapsec;		/* leapseconds countdown */
u_long	orphwait; 		/* orphan wait time */
#ifdef AUTOKEY
static	u_long revoke_timer;	/* keys revoke timer */
static	u_long keys_timer;	/* session key timer */
u_long	sys_revoke = KEY_REVOKE; /* keys revoke timeout (log2 s) */
u_long	sys_automax = NTP_AUTOMAX; /* key list timeout (log2 s) */
#endif	/* AUTOKEY */

/*
 * Statistics counter for the interested.
 */
volatile u_long alarm_overflow;

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

#ifdef SYS_WINNT
HANDLE WaitableTimerHandle;
#else
static	RETSIGTYPE alarming (int);
#endif /* SYS_WINNT */

#if !defined(VMS)
# if !defined SYS_WINNT || defined(SYS_CYGWIN32)
#  ifdef HAVE_TIMER_CREATE
static timer_t timer_id;
typedef struct itimerspec intervaltimer;
#   define	itv_frac	tv_nsec
#  else 
typedef struct itimerval intervaltimer;
#   define	itv_frac	tv_usec
#  endif
intervaltimer itimer;
# endif
#endif

#if !defined(SYS_WINNT) && !defined(VMS)
void	set_timer_or_die(const intervaltimer *);
#endif


#if !defined(SYS_WINNT) && !defined(VMS)
void
set_timer_or_die(
	const intervaltimer *	ptimer
	)
{
	const char *	setfunc;
	int		rc;

# ifdef HAVE_TIMER_CREATE
	setfunc = "timer_settime";
	rc = timer_settime(timer_id, 0, &itimer, NULL);
# else
	setfunc = "setitimer";
	rc = setitimer(ITIMER_REAL, &itimer, NULL);
# endif
	if (-1 == rc) {
		msyslog(LOG_ERR, "interval timer %s failed, %m\n",
			setfunc);
		exit(1);
	}
}
#endif	/* !SYS_WINNT && !VMS */


/*
 * reinit_timer - reinitialize interval timer after a clock step.
 */
void 
reinit_timer(void)
{
#if !defined(SYS_WINNT) && !defined(VMS)
	ZERO(itimer);
# ifdef HAVE_TIMER_CREATE
	timer_gettime(timer_id, &itimer);
# else
	getitimer(ITIMER_REAL, &itimer);
# endif
	if (itimer.it_value.tv_sec < 0 ||
	    itimer.it_value.tv_sec > (1 << EVENT_TIMEOUT))
		itimer.it_value.tv_sec = (1 << EVENT_TIMEOUT);
	if (itimer.it_value.itv_frac < 0)
		itimer.it_value.itv_frac = 0;
	if (0 == itimer.it_value.tv_sec &&
	    0 == itimer.it_value.itv_frac)
		itimer.it_value.tv_sec = (1 << EVENT_TIMEOUT);
	itimer.it_interval.tv_sec = (1 << EVENT_TIMEOUT);
	itimer.it_interval.itv_frac = 0;
	set_timer_or_die(&itimer);
# endif /* VMS */
}


/*
 * init_timer - initialize the timer data structures
 */
void
init_timer(void)
{
	/*
	 * Initialize...
	 */
	alarm_flag = FALSE;
	alarm_overflow = 0;
	adjust_timer = 1;
	stats_timer = HOUR;
	huffpuff_timer = 0;
	interface_timer = 0;
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
# ifndef VMS
#  ifdef HAVE_TIMER_CREATE
	if (TC_ERR == timer_create(CLOCK_REALTIME, NULL, &timer_id)) {
		msyslog(LOG_ERR, "timer_create failed, %m\n");
		exit(1);
	}
#  endif
	signal_no_reset(SIGALRM, alarming);
	itimer.it_interval.tv_sec = 
		itimer.it_value.tv_sec = (1 << EVENT_TIMEOUT);
	itimer.it_interval.itv_frac = itimer.it_value.itv_frac = 0;
	set_timer_or_die(&itimer);
# else	/* VMS follows */
	vmsinc[0] = 10000000;		/* 1 sec */
	vmsinc[1] = 0;
	lib$emul(&(1<<EVENT_TIMEOUT), &vmsinc, &0, &vmsinc);

	sys$gettim(&vmstimer);	/* that's "now" as abstime */

	lib$addx(&vmsinc, &vmstimer, &vmstimer);
	sys$setimr(0, &vmstimer, alarming, alarming, 0);
# endif	/* VMS */
#else	/* SYS_WINNT follows */
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
		DWORD		Period;
		LARGE_INTEGER	DueTime;
		BOOL		rc;

		Period = (1 << EVENT_TIMEOUT) * 1000;
		DueTime.QuadPart = Period * 10000i64;
		rc = SetWaitableTimer(WaitableTimerHandle, &DueTime,
				      Period, NULL, NULL, FALSE);
		if (!rc) {
			msyslog(LOG_ERR, "SetWaitableTimer failed: %m");
			exit(1);
		}
	}

#endif	/* SYS_WINNT */
}


/*
 * intres_timeout_req(s) is invoked in the parent to schedule an idle
 * timeout to fire in s seconds, if not reset earlier by a call to
 * intres_timeout_req(0), which clears any pending timeout.  When the
 * timeout expires, worker_idle_timer_fired() is invoked (again, in the
 * parent).
 *
 * sntp and ntpd each provide implementations adapted to their timers.
 */
void
intres_timeout_req(
	u_int	seconds		/* 0 cancels */
	)
{
	if (0 == seconds) {
		worker_idle_timer = 0;
		return;
	}
	worker_idle_timer = current_time + seconds;
}


/*
 * timer - event timer
 */
void
timer(void)
{
	struct peer *	p;
	struct peer *	next_peer;
	l_fp		now;
	static int	leap_warn_log = FALSE;

	/*
	 * The basic timerevent is one second.  This is used to adjust the
	 * system clock in time and frequency, implement the kiss-o'-death
	 * function and the association polling function.
	 */
	current_time++;
	if (adjust_timer <= current_time) {
		adjust_timer += 1;
		adj_host_clock();
#ifdef REFCLOCK
		for (p = peer_list; p != NULL; p = next_peer) {
			next_peer = p->p_link;
			if (FLAG_REFCLOCK & p->flags)
				refclock_timer(p);
		}
#endif /* REFCLOCK */
	}

	/*
	 * Now dispatch any peers whose event timer has expired. Be
	 * careful here, since the peer structure might go away as the
	 * result of the call.
	 */
	for (p = peer_list; p != NULL; p = next_peer) {
		next_peer = p->p_link;

		/*
		 * Restrain the non-burst packet rate not more
		 * than one packet every 16 seconds. This is
		 * usually tripped using iburst and minpoll of
		 * 128 s or less.
		 */
		if (p->throttle > 0)
			p->throttle--;
		if (p->nextdate <= current_time) {
#ifdef REFCLOCK
			if (FLAG_REFCLOCK & p->flags)
				refclock_transmit(p);
			else
#endif	/* REFCLOCK */
				transmit(p);
		}
	}

	/*
	 * Orphan mode is active when enabled and when no servers less
	 * than the orphan stratum are available. A server with no other
	 * synchronization source is an orphan. It shows offset zero and
	 * reference ID the loopback address.
	 */
	if (sys_orphan < STRATUM_UNSPEC && sys_peer == NULL &&
	    current_time > orphwait) {
		if (sys_leap == LEAP_NOTINSYNC) {
			sys_leap = LEAP_NOWARNING;
#ifdef AUTOKEY
			if (crypto_flags)	
				crypto_update();
#endif	/* AUTOKEY */
		}
		sys_stratum = (u_char)sys_orphan;
		if (sys_stratum > 1)
			sys_refid = htonl(LOOPBACKADR);
		else
			memcpy(&sys_refid, "LOOP", 4);
		sys_offset = 0;
		sys_rootdelay = 0;
		sys_rootdisp = 0;
	}

	/*
	 * Leapseconds. If a leap is pending, decrement the time
	 * remaining. If less than one day remains, set the leap bits.
	 * When no time remains, clear the leap bits and increment the
	 * TAI. If kernel suppport is not available, do the leap
	 * crudely. Note a leap cannot be pending unless the clock is
	 * set.
	 */
	if (leapsec > 0) {
		leapsec--;
		if (leapsec == 0) {
			sys_leap = LEAP_NOWARNING;
			sys_tai = leap_tai;
#ifndef SYS_WINNT /* WinNT port has its own leap second handling */
# ifdef KERNEL_PLL
			if (!(pll_control && kern_enable))
# endif /* KERNEL_PLL */
			{
				step_systime(-1.0);
				msyslog(LOG_NOTICE, "Inserting positive leap second.");
			}
#endif /* SYS_WINNT */
			report_event(EVNT_LEAP, NULL, NULL);
		} else {
			if (leapsec < DAY)
				sys_leap = LEAP_ADDSECOND;
			if (leap_tai > 0)
				sys_tai = leap_tai - 1;
		}
	}

	/*
	 * Update huff-n'-puff filter.
	 */
	if (huffpuff_timer <= current_time) {
		huffpuff_timer += HUFFPUFF;
		huffpuff();
	}

#ifdef AUTOKEY
	/*
	 * Garbage collect expired keys.
	 */
	if (keys_timer <= current_time) {
		keys_timer += 1 << sys_automax;
		auth_agekeys();
	}

	/*
	 * Generate new private value. This causes all associations
	 * to regenerate cookies.
	 */
	if (revoke_timer && revoke_timer <= current_time) {
		revoke_timer += 1 << sys_revoke;
		RAND_bytes((u_char *)&sys_private, 4);
	}
#endif	/* AUTOKEY */

	/*
	 * Interface update timer
	 */
	if (interface_interval && interface_timer <= current_time) {
		timer_interfacetimeout(current_time +
		    interface_interval);
		DPRINTF(2, ("timer: interface update\n"));
		interface_update(NULL, NULL);
	}

	if (worker_idle_timer && worker_idle_timer <= current_time)
		worker_idle_timer_fired();

	/*
	 * Finally, write hourly stats.
	 */
	if (stats_timer <= current_time) {
		stats_timer += HOUR;
		write_stats();
		if (sys_tai != 0) {
			get_systime(&now);
			if (now.l_ui > leap_expire) {
				report_event(EVNT_LEAPVAL, NULL, NULL);
				if (leap_warn_log == FALSE) {
					msyslog(LOG_WARNING,
						"leapseconds data file has expired.");
					leap_warn_log = TRUE;
				}
				/* If a new file was installed between
				 * the previous 24 hour check and the
				 * expiration of this one, we'll squawk
				 * once.  Better than checking for a
				 * new file every hour...
				 */
				check_leap_file();
			} else
				leap_warn_log = FALSE;
		}
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
# ifdef DEBUG
	const char *msg = "alarming: initializing TRUE\n";
# endif

	if (!initializing) {
		if (alarm_flag) {
			alarm_overflow++;
# ifdef DEBUG
			msg = "alarming: overflow\n";
# endif
		} else {
# ifndef VMS
			alarm_flag++;
# else
			/* VMS AST routine, increment is no good */
			alarm_flag = 1;
# endif
# ifdef DEBUG
			msg = "alarming: normal\n";
# endif
		}
	}
# ifdef VMS
	lib$addx(&vmsinc, &vmstimer, &vmstimer);
	sys$setimr(0, &vmstimer, alarming, alarming, 0);
# endif
# ifdef DEBUG
	if (debug >= 4)
		write(1, msg, strlen(msg));
# endif
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

