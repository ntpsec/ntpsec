/*
 * systime -- routines to fiddle a UNIX clock.
 */
#include "ntp_machine.h"
#include "ntp_fp.h"
#include "ntp_syslog.h"
#include "ntp_unixtime.h"
#include "ntp_stdlib.h"

#ifdef SIM
#include "ntpsim.h"
#endif /*SIM */

#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
#ifdef HAVE_UTMP_H
# include <utmp.h>
#endif /* HAVE_UTMP_H */
#ifdef HAVE_UTMPX_H
# include <utmpx.h>
#endif /* HAVE_UTMPX_H */

int	systime_10ms_ticks = 0;	/* adj sysclock in 10ms increments */

/*
 * These routines (init_systime, get_systime, step_systime, adj_systime)
 * implement an interface between the (more or less) system independent
 * bits of NTP and the peculiarities of dealing with the Unix system
 * clock.
 */
double sys_residual = 0;	/* residual from previous adjustment */

#ifndef SIM
/*
 * get_systime - return the system time in timestamp format biased by
 * the current time offset.
 */
void
get_systime(
	l_fp *now
	)
{
#if defined(HAVE_CLOCK_GETTIME) || defined(HAVE_GETCLOCK)
	struct timespec ts;
#else
	struct timeval tv;
#endif
	double dtemp;

	/*
	 * We use nanosecond time if we can get it. Watch out for
	 * rounding wiggles, which may overflow the fraction.
	 */
#if defined(HAVE_CLOCK_GETTIME) || defined(HAVE_GETCLOCK)
# ifdef HAVE_CLOCK_GETTIME
	(void) clock_gettime(CLOCK_REALTIME, &ts);
# else
	(void) getclock(TIMEOFDAY, &ts);
# endif
	now->l_i = ts.tv_sec + JAN_1970;
	dtemp = ts.tv_nsec * FRAC / 1e9;
	if (dtemp >= FRAC)
		now->l_i++;
	now->l_uf = (u_int32)dtemp;
#else /* HAVE_CLOCK_GETTIME */
	(void) GETTIMEOFDAY(&tv, (struct timezone *)0);
	now->l_i = tv.tv_sec + JAN_1970;

#if defined RELIANTUNIX_CLOCK || defined SCO5_CLOCK
	if (systime_10ms_ticks) {
		/* fake better than 10ms resolution by interpolating 
	   	accumulated residual (in adj_systime(), see below) */
		dtemp = tv.tv_usec / 1e6;
		if (sys_residual < 5000e-6 && sys_residual > -5000e-6) {
			dtemp += sys_residual;
			if (dtemp < 0) {
				now->l_i--;
				dtemp++;
			}
		}
		dtemp *= FRAC;
	} else
#endif

	dtemp = tv.tv_usec * FRAC / 1e6;

	if (dtemp >= FRAC)
		now->l_i++;
	now->l_uf = (u_int32)dtemp;
#endif /* HAVE_CLOCK_GETTIME */

}


/*
 * adj_systime - called once every second to make system time
 * adjustments. Returns 1 if okay, 0 if trouble.
 */
#if !defined SYS_WINNT
int
adj_systime(
	double now
	)
{
	double dtemp;
	struct timeval adjtv;
	u_char isneg = 0;
	struct timeval oadjtv;

	/*
	 * Add the residual from the previous adjustment to the new
	 * adjustment, bound and round.
	 */
	dtemp = sys_residual + now;
	sys_residual = 0;
	if (dtemp < 0) {
		isneg = 1;
		dtemp = -dtemp;
	}

#if defined RELIANTUNIX_CLOCK || defined SCO5_CLOCK
	if (systime_10ms_ticks) {
		/*
		 * accumulate changes until we have enough to adjust a
		 * tick
		 */
		if (dtemp < 5000e-6) {
			if (isneg) sys_residual = -dtemp;
			else sys_residual = dtemp;
			dtemp = 0;
		} else {
			if (isneg) sys_residual = 10000e-6 - dtemp;
			else sys_residual = dtemp - 10000e-6;
			dtemp = 10000e-6;
		}
	}
#endif
	adjtv.tv_sec = (long)dtemp;
	adjtv.tv_usec = (long)((dtemp - adjtv.tv_sec) * 1e6 + .5);
	if (isneg) {
		adjtv.tv_sec = -adjtv.tv_sec;
		adjtv.tv_usec = -adjtv.tv_usec;
	}

	/*
	 * Here we do the actual adjustment. If for some reason the
	 * adjtime() call fails, like it is not implemented or something
	 * like that, we honk to the log. If the previous adjustment did
	 * not complete, we correct the residual offset.
	 */
	/* casey - we need a posix type thang here */
	if (adjtime(&adjtv, &oadjtv) < 0)
	{
		msyslog(LOG_ERR,
		    "Can't adjust time (%ld sec, %ld usec): %m",
		    (long)adjtv.tv_sec, (long)adjtv.tv_usec);
		return (0);
	} 
	else {
	sys_residual += oadjtv.tv_usec / 1e6;
	}
#ifdef DEBUG
	if (debug && sys_residual != 0)
		printf("adj_systime: adj %.9f -> remaining residual %.9f\n", now, sys_residual);
#endif
	return (1);
}
#endif


/*
 * step_systime - step the system clock.
 */
int
step_systime(
	double now
	)
{
	struct timeval timetv, adjtv, oldtimetv;
	int isneg = 0;
	double dtemp;
#if defined(HAVE_CLOCK_GETTIME) || defined(HAVE_GETCLOCK)
	struct timespec ts;
#endif

	dtemp = sys_residual + now;
	if (dtemp < 0) {
		isneg = 1;
		dtemp = - dtemp;
		adjtv.tv_sec = (int32)dtemp;
		adjtv.tv_usec = (u_int32)((dtemp -
		    (double)adjtv.tv_sec) * 1e6 + .5);
	} else {
		adjtv.tv_sec = (int32)dtemp;
		adjtv.tv_usec = (u_int32)((dtemp -
		    (double)adjtv.tv_sec) * 1e6 + .5);
	}
#if defined(HAVE_CLOCK_GETTIME) || defined(HAVE_GETCLOCK)
#ifdef HAVE_CLOCK_GETTIME
	(void) clock_gettime(CLOCK_REALTIME, &ts);
#else
	(void) getclock(TIMEOFDAY, &ts);
#endif
	timetv.tv_sec = ts.tv_sec;
	timetv.tv_usec = ts.tv_nsec / 1000;
#else /*  not HAVE_GETCLOCK */
	(void) GETTIMEOFDAY(&timetv, (struct timezone *)0);
#endif /* not HAVE_GETCLOCK */

	oldtimetv = timetv;

#ifdef DEBUG
	if (debug)
		printf("step_systime: step %.6f residual %.6f\n", now, sys_residual);
#endif
	if (isneg) {
		timetv.tv_sec -= adjtv.tv_sec;
		timetv.tv_usec -= adjtv.tv_usec;
		if (timetv.tv_usec < 0) {
			timetv.tv_sec--;
			timetv.tv_usec += 1000000;
		}
	} else {
		timetv.tv_sec += adjtv.tv_sec;
		timetv.tv_usec += adjtv.tv_usec;
		if (timetv.tv_usec >= 1000000) {
			timetv.tv_sec++;
			timetv.tv_usec -= 1000000;
		}
	}
	if (ntp_set_tod(&timetv, NULL) != 0) {
		msyslog(LOG_ERR, "Can't set time of day: %m");
		return (0);
	}
	sys_residual = 0;

#ifdef NEED_HPUX_ADJTIME
	/*
	 * CHECKME: is this correct when called by ntpdate?????
	 */
	_clear_adjtime();
#endif

	/*
	 * FreeBSD, for example, has:
	 * struct utmp {
	 *	   char    ut_line[UT_LINESIZE];
	 *	   char    ut_name[UT_NAMESIZE];
	 *	   char    ut_host[UT_HOSTSIZE];
	 *	   long    ut_time;
	 * };
	 * and appends line="|", name="date", host="", time for the OLD
	 * and appends line="{", name="date", host="", time for the NEW
	 * to _PATH_WTMP .
	 *
	 * Some OSes have utmp, some have utmpx.
	 */

	/*
	 * Write old and new time entries in utmp and wtmp if step
	 * adjustment is greater than one second.
	 *
	 * This might become even Uglier...
	 */
	if (oldtimetv.tv_sec != timetv.tv_sec)
	{
#ifdef HAVE_UTMP_H
		struct utmp ut;
#endif
#ifdef HAVE_UTMPX_H
		struct utmpx utx;
#endif

#ifdef HAVE_UTMP_H
		memset((char *)&ut, 0, sizeof(ut));
#endif
#ifdef HAVE_UTMPX_H
		memset((char *)&utx, 0, sizeof(utx));
#endif

		/* UTMP */

#ifdef UPDATE_UTMP
# ifdef HAVE_PUTUTLINE
		ut.ut_type = OLD_TIME;
		(void)strcpy(ut.ut_line, OTIME_MSG);
		ut.ut_time = oldtimetv.tv_sec;
		pututline(&ut);
		setutent();
		ut.ut_type = NEW_TIME;
		(void)strcpy(ut.ut_line, NTIME_MSG);
		ut.ut_time = timetv.tv_sec;
		pututline(&ut);
		endutent();
# else /* not HAVE_PUTUTLINE */
# endif /* not HAVE_PUTUTLINE */
#endif /* UPDATE_UTMP */

		/* UTMPX */

#ifdef UPDATE_UTMPX
# ifdef HAVE_PUTUTXLINE
		utx.ut_type = OLD_TIME;
		(void)strcpy(utx.ut_line, OTIME_MSG);
		utx.ut_tv = oldtimetv;
		pututxline(&utx);
		setutxent();
		utx.ut_type = NEW_TIME;
		(void)strcpy(utx.ut_line, NTIME_MSG);
		utx.ut_tv = timetv;
		pututxline(&utx);
		endutxent();
# else /* not HAVE_PUTUTXLINE */
# endif /* not HAVE_PUTUTXLINE */
#endif /* UPDATE_UTMPX */

		/* WTMP */

#ifdef UPDATE_WTMP
# ifdef HAVE_PUTUTLINE
		utmpname(WTMP_FILE);
		ut.ut_type = OLD_TIME;
		(void)strcpy(ut.ut_line, OTIME_MSG);
		ut.ut_time = oldtimetv.tv_sec;
		pututline(&ut);
		ut.ut_type = NEW_TIME;
		(void)strcpy(ut.ut_line, NTIME_MSG);
		ut.ut_time = timetv.tv_sec;
		pututline(&ut);
		endutent();
# else /* not HAVE_PUTUTLINE */
# endif /* not HAVE_PUTUTLINE */
#endif /* UPDATE_WTMP */

		/* WTMPX */

#ifdef UPDATE_WTMPX
# ifdef HAVE_PUTUTXLINE
		utx.ut_type = OLD_TIME;
		utx.ut_tv = oldtimetv;
		(void)strcpy(utx.ut_line, OTIME_MSG);
#  ifdef HAVE_UPDWTMPX
		updwtmpx(WTMPX_FILE, &utx);
#  else /* not HAVE_UPDWTMPX */
#  endif /* not HAVE_UPDWTMPX */
# else /* not HAVE_PUTUTXLINE */
# endif /* not HAVE_PUTUTXLINE */
# ifdef HAVE_PUTUTXLINE
		utx.ut_type = NEW_TIME;
		utx.ut_tv = timetv;
		(void)strcpy(utx.ut_line, NTIME_MSG);
#  ifdef HAVE_UPDWTMPX
		updwtmpx(WTMPX_FILE, &utx);
#  else /* not HAVE_UPDWTMPX */
#  endif /* not HAVE_UPDWTMPX */
# else /* not HAVE_PUTUTXLINE */
# endif /* not HAVE_PUTUTXLINE */
#endif /* UPDATE_WTMPX */

	}
	return (1);
}

#else /* SIM */
/*
 * Clock routines for the simulator - Harish Nair, with help
 */
/*
 * get_systime - return the system time in NTP timestamp format 
 */
void
get_systime(
        l_fp *now		/* current system time in l_fp */        )
{
	/*
	 * To fool the code that determines the local clock precision,
	 * we advance the clock a minimum of 200 nanoseconds on every
	 * clock read. This is appropriate for a typical modern machine
	 * with nanosecond clocks. Note we make no attempt here to
	 * simulate reading error, since the error is so small. This may
	 * change when the need comes to implement picosecond clocks.
	 */
	if (ntp_node.ntp_time == ntp_node.last_time)
		ntp_node.ntp_time += 200e-9;
	ntp_node.last_time = ntp_node.ntp_time;
	DTOLFP(ntp_node.ntp_time, now);
}
 
 
/*
 * adj_systime - advance or retard the system clock
 */
int				/* always succeeds */
adj_systime(
        double now		/* time adjustment (s) */
        )
{
	/*
	 * Just like the tickadj() function, this code replaces the
	 * current adjustment amount. It's important that the code not
	 * restrict the adjustment range, since with ntpdate emulation
	 * the range can be much larger than 500 PPM. 
	 */
	ntp_node.adj = now;
        return (1);
}
 
 
/*
 * step_systime - step the system clock
 */
int				/* always succeeds */
step_systime(
        double now		/* step adjustment (s) */
        )
{
	/*
	 * This is pretty brutal and assumes the system clock code was
	 * written by other than amateurs. Good clock code is a black
	 * art anyway.
	 */
	ntp_node.clk_time -= now;
        return (1);
}

/*
 * node_clock - update the clocks
 */
int				/* always succeeds */
node_clock(
	Node *n,		/* global node pointer */
	double t		/* node time */
	)
{
	double	dtemp;

	/*
	 * Advance client clock (ntp_time). Advance server clock
	 * (clk_time) adjusted for systematic and random frequency
	 * errors. The random error is a random walk computed as the
	 * integral of samples from a Gaussian distribution.
	 */
	dtemp = t - n->ntp_time;
	n->time = t;
	n->ntp_time += dtemp;
/*
	n->ferr += gauss(0, dtemp * n->fnse * 1e-6);
*/
	n->clk_time += dtemp * (1 + n->ferr * 1e-6);

	/*
	 * Perform the adjtime() function. If the adjustment completed
	 * in the previous interval, amortize the entire amount; if not,
	 * carry the leftover to the next interval.
	 */
	dtemp *= n->slew;
	if (dtemp < fabs(n->adj)) {
		if (n->adj < 0) {
			n->adj += dtemp;
			n->ntp_time -= dtemp;
		} else {
			n->adj -= dtemp;
			n->ntp_time += dtemp;
		}
	} else {
		n->ntp_time += n->adj;
		n->adj = 0;
	}
        return (0);
}

 
/*
 * gauss() - returns samples from a gaussion distribution
 */
double				/* Gaussian sample */
gauss(
	double m,		/* sample mean */
	double s		/* sample standard deviation (sigma) */
	)
{
        double q1, q2;

	/*
	 * Roll a sample from a Gaussian distribution with mean m and
	 * standard deviation s. For m = 0, s = 1, mean(y) = 0,
	 * std(y) = 1.
	 */
	if (s == 0)
		return (m);
        while ((q1 = drand48()) == 0);
        q2 = drand48();
        return (m + s * sqrt(-2. * log(q1)) * cos(2. * PI * q2));
}

 
/*
 * poisson() - returns samples from a network delay distribution
 */
double				/* delay sample (s) */
poisson(
	double m,		/* fixed propagation delay (s) */
	double s		/* exponential parameter (mu) */
	)
{
        double q1;

	/*
	 * Roll a sample from a composite distribution with propagation
	 * delay m and exponential distribution time with parameter s.
	 * For m = 0, s = 1, mean(y) = std(y) = 1.
	 */
	if (s == 0)
		return (m);
        while ((q1 = drand48()) == 0);
        return (m - s * log(q1 * s));
}
#endif /* SIM */
