/*
 * systime -- routines to fiddle a UNIX clock.
 *
 * ATTENTION: Get approval from Dave Mills on all changes to this file!
 *
 */
#include "config.h"

#include "ntp.h"
#include "ntp_syslog.h"
#include "ntp_stdlib.h"
#include "timespecops.h"
#include "ntp_calendar.h"

#ifndef USE_COMPILETIME_PIVOT
# define USE_COMPILETIME_PIVOT 1
#endif

/*
 * These routines (get_systime, step_systime, adj_systime) implement an
 * interface between the system independent NTP clock and the Unix
 * system clock in various architectures and operating systems. Time is
 * a precious quantity in these routines and every effort is made to
 * minimize errors by unbiased rounding and amortizing adjustment
 * residues.
 *
 * In order to improve the apparent resolution, provide unbiased
 * rounding and most importantly ensure that the readings cannot be
 * predicted, the low-order unused portion of the time below the minimum
 * time to read the clock is filled with an unbiased random fuzz.
 *
 * The sys_tick variable specifies the system clock tick interval in
 * seconds, for stepping clocks, defined as those which return times
 * less than MINSTEP greater than the previous reading. For systems that
 * use a high-resolution counter such that each clock reading is always
 * at least MINSTEP greater than the prior, sys_tick is the time to read
 * the system clock.
 *
 * The sys_fuzz variable measures the minimum time to read the system
 * clock, regardless of its precision.  When reading the system clock
 * using get_systime() after sys_tick and sys_fuzz have been determined,
 * ntpd ensures each unprocessed clock reading is no less than sys_fuzz
 * later than the prior unprocessed reading, and then fuzzes the bits
 * below sys_fuzz in the timestamp returned, ensuring each of its
 * resulting readings is strictly later than the previous.
 *
 * When slewing the system clock using adj_systime() (with the kernel
 * loop discipline unavailable or disabled), adjtime() offsets are
 * quantized to sys_tick, if sys_tick is greater than sys_fuzz, which
 * is to say if the OS presents a stepping clock.  Otherwise, offsets
 * are quantized to the microsecond resolution of adjtime()'s timeval
 * input.  The remaining correction sys_residual is carried into the
 * next adjtime() and meanwhile is also factored into get_systime()
 * readings.
 *
 * adj_systime() and step_systime() will behave sanely with these
 * variables not set, but the adjustments may be in larger steps.
 */
time_stepped_callback	step_callback;

static doubletime_t  sys_residual = 0;	/* adjustment residue (s) */

static  void	get_ostime	(struct timespec *tsp);


static void
get_ostime(
	struct timespec *	tsp
	)
{
	int	rc;

	rc = clock_gettime(CLOCK_REALTIME, tsp);
	if (rc < 0) {
#ifndef __COVERITY__
		msyslog(LOG_ERR, "TIME: read system clock failed: %s (%d)",
			strerror(errno), errno);
#endif /* __COVERITY__ */
		exit(1);
	}

}


/*
 * get_systime - return system time in NTP timestamp format.
 */
void
get_systime(
	l_fp *now		/* system time */
	)
{
	struct timespec ts;	/* seconds and nanoseconds */
	get_ostime(&ts);
	*now = tspec_stamp_to_lfp(ts);
}


/*
 * adj_systime - adjust system time by the argument.
 */
bool				/* true on okay, false on error */
adj_systime(
	double now,		/* adjustment (s) */
	int (*ladjtime)(const struct timeval *, struct timeval *)
	)
{
	struct timeval adjtv;	/* new adjustment */
	struct timeval oadjtv;	/* residual adjustment */
	double	quant;		/* quantize to multiples of */
	doubletime_t	dtemp;
	long	ticks;
	bool	isneg = false;

	/*
	 * FIXME: With the legacy Windows port gone, this might be removable.
	 * See also the related FIXME comment in ntpd/ntp_loopfilter.c.
	 *
	 * The Windows port adj_systime() depended on being called each
	 * second even when there's no additional correction, to allow
	 * emulation of adjtime() behavior on top of an API that simply
	 * sets the current rate.  This POSIX implementation needs to
	 * ignore invocations with zero correction, otherwise ongoing
	 * EVNT_NSET adjtime() can be aborted by a tiny adjtime()
	 * triggered by sys_residual.
	 */
	if ( D_ISZERO_NS(now))
		return true;

	/*
	 * Most Unix adjtime() implementations adjust the system clock
	 * in microsecond quanta, but some adjust in 10-ms quanta. We
	 * carefully round the adjustment to the nearest quantum, then
	 * adjust in quanta and keep the residue for later.
	 */
	dtemp = now + sys_residual;
	if (dtemp < 0) {
		isneg = true;
		dtemp = -dtemp;
	}
	adjtv.tv_sec = (long)dtemp;
	dtemp -= adjtv.tv_sec;
	quant = S_PER_US;
	ticks = (long)(dtemp / quant + .5);
	adjtv.tv_usec = (long)(ticks * quant * US_PER_S + .5);
	/* The rounding in the conversions could push us over the
	 * limits: make sure the result is properly normalised!
	 * note: sign comes later, all numbers non-negative here.
	 */
	if (adjtv.tv_usec >= US_PER_S) {
		adjtv.tv_sec  += 1;
		adjtv.tv_usec -= US_PER_S;
		dtemp         -= 1.;
	}
	/* set the new residual with leftover from correction */
	sys_residual = dtemp - adjtv.tv_usec * S_PER_US;

	/*
	 * Convert to signed seconds and microseconds for the Unix
	 * adjtime() system call. Note we purposely lose the adjtime()
	 * leftover.
	 */
	if (isneg) {
		adjtv.tv_sec = -adjtv.tv_sec;
		adjtv.tv_usec = -adjtv.tv_usec;
		sys_residual = -sys_residual;
	}
	if (adjtv.tv_sec != 0 || adjtv.tv_usec != 0) {
		if (ladjtime(&adjtv, &oadjtv) < 0) {
			msyslog(LOG_ERR, "CLOCK: adj_systime: %s", strerror(errno));
			return false;
		}
	}
	return true;
}


/*
 * step_systime - step the system clock.
 *
 * if your timespec has a 64 bit time_t then you are 2038 ready.
 * if your timespec has a 32 bit time_t, be sure to duck in 2038
 */

bool
step_systime(
	doubletime_t step
	)
{
	time_t pivot; /* for ntp era unfolding */
	struct timespec timets;
	struct timespec old, new;
	struct calendar jd;
	l_fp fp_ofs, fp_sys; /* offset and target system time in FP */

	/*
	 * Get pivot time for NTP era unfolding. Since we don't step
	 * very often, we can afford to do the whole calculation from
	 * scratch. And we're not in the time-critical path yet.
	 */
        if (4 < sizeof(time_t)) {
            // 64-bit time_t
            /*
             * This code makes sure the resulting time stamp for the new
             * system time is in the 2^32 seconds starting at 1970-01-01,
             * 00:00:00 UTC.
             */
            pivot = 0x80000000;
#if USE_COMPILETIME_PIVOT
            /*
             * Add the compile time minus 10 years to get a possible target
             * area of (compile time - 10 years) to (compile time + 126
             * years).  This should be sufficient for a given binary of
             * NTPD.
             */
            if (ntpcal_get_build_date(&jd)) {
                    jd.year -= 10;
                    pivot += ntpcal_date_to_time(&jd);
            } else {
                    msyslog(LOG_ERR,
                       "CLOCK: step_systime: assume 1970-01-01 as build date");
            }
#else
            UNUSED_LOCAL(jd);
#endif  // USE_COMPILETIME_PIVOT
    } else {
            // 32-bit time_t
            UNUSED_LOCAL(jd);
            /* This makes sure the resulting time stamp is on or after
             * 1969-12-31/23:59:59 UTC and gives us additional two years,
             * from the change of NTP era in 2036 to the UNIX rollover in
             * 2038. (Minus one second, but that won't hurt.) We *really*
             * need a longer 'time_t' after that!  Or a different baseline,
             * but that would cause other serious trouble, too.
             */
            pivot = 0x7FFFFFFF;
    }

	// get the complete jump distance as l_fp
	fp_sys = dtolfp(sys_residual);
	fp_ofs = dtolfp(step);
	fp_ofs += fp_sys;

	/* ---> time-critical path starts ---> */

	/* get the current time as l_fp (without fuzz) and as struct timespec */
	get_ostime(&timets);
	old = timets;
	fp_sys = tspec_stamp_to_lfp(timets);

	/* get the target time as l_fp */
	fp_sys += fp_ofs;

	/* unfold the new system time */
	timets = lfp_stamp_to_tspec(fp_sys, pivot);
	new = timets;

	/* now set new system time */
	if (ntp_set_tod(&timets) != 0) {
		msyslog(LOG_ERR, "CLOCK: step_systime: %s", strerror(errno));
		return false;
	}

	/* <--- time-critical path ended with call to the settime hook <--- */

	msyslog(LOG_WARNING, "CLOCK: time stepped by %Lf", step);
	if (fabsl(step) > 86400) {
	    /* Get the full year (both old and new) into the log file.
	     * Issue #474 */
	    struct tm oldtm, newtm;
	    char oldbuf[100], newbuf[100];
	    if (!localtime_r(&old.tv_sec, &oldtm)) {
		oldtm.tm_year = 9999-1900;
		oldtm.tm_mon = 98;
		oldtm.tm_mday = 99;
	    }
	    snprintf(oldbuf, sizeof(oldbuf), "%04d-%02d-%02d",
		oldtm.tm_year+1900, oldtm.tm_mon+1, oldtm.tm_mday);
	    if (!localtime_r(&new.tv_sec, &newtm)) {
		newtm.tm_year = 9999-1900;
		newtm.tm_mon = 98;
		newtm.tm_mday = 99;
	    }
	    snprintf(newbuf, sizeof(newbuf), "%04d-%02d-%02d",
		newtm.tm_year+1900, newtm.tm_mon+1, newtm.tm_mday);
	    msyslog(LOG_WARNING, "CLOCK: time changed from %s to %s",
		oldbuf, newbuf);
	}

	sys_residual = 0;
	if (step_callback)
		(*step_callback)();
	return true;
}
