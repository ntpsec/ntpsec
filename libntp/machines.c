/* machines.c - provide special support for peculiar architectures
 *
 * Real bummers unite !
 *
 */

#include <unistd.h>

#include "config.h"

#ifdef HAVE_SYS_TIMEX_H
# include <sys/time.h>	/* prerequisite on NetBSD */
# include <sys/timex.h>
#endif

#include "ntp.h"
#include "ntp_machine.h"
#include "ntp_syslog.h"
#include "ntp_stdlib.h"
#include "ntp_unixtime.h"
#include "lib_strbuf.h"
#include "ntp_debug.h"
#include "ntp_syscall.h"

/*
 * Simulate ANSI/POSIX conformance on platforms that don't have it
 */
#ifndef HAVE_CLOCK_GETTIME
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#elif HAVE_GETCLOCK
#include <sys/timers.h>
#endif

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    int mode;
    switch (clk_id) {
    case CLOCK_REALTIME:
	mode = CALENDAR_CLOCK;
	break;
    case CLOCK_MONOTONIC:
	/* http://stackoverflow.com/questions/11680461/monotonic-clock-on-osx */
	mode = SYSTEM_CLOCK;
	break;
    default:
	return -1;
    }
    host_get_clock_service(mach_host_self(), mode, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    tp->tv_sec = mts.tv_sec;
    tp->tv_nsec = mts.tv_nsec;
#elif HAVE_GETCLOCK
    (void) getclock(TIMEOFDAY, &tp);
#else
#error Either POSIX clock_gettime(2) or Tru64/HP-UX getclock(2) is required
#endif
    return 0;
}
#endif /* HAVE_CLOCK_GETTIME */

#if !defined(HAVE_NTP_GETTIME) && defined(HAVE_NTP_ADJTIME)
int ntp_gettime(struct ntptimeval *ntv)
{
	struct timex tntx;
	int result;

	ZERO(tntx);
	/* not intercepted because ntpd doesn't use this */
	result = ntp_adjtime(&tntx);
	ntv->time = tntx.time;
	ntv->maxerror = tntx.maxerror;
	ntv->esterror = tntx.esterror;
#  ifdef NTP_API
#   if NTP_API > 3
	ntv->tai = tntx.tai;
#   endif
#  endif
	return result;
}
#endif	/* !HAVE_NTP_GETTIME */

#define SET_TOD_UNDETERMINED	0
#define SET_TOD_CLOCK_SETTIME	1
#define SET_TOD_SETTIMEOFDAY	2
#define SET_TOD_STIME		3

const char * const set_tod_used[] = {
	"undetermined",
	"clock_settime",
	"settimeofday",
	"stime"
};

pset_tod_using	set_tod_using = NULL;


int
ntp_set_tod(
	struct timespec *tvs
	)
{
	static int	tod;
	int		rc;
	int		saved_errno;

	TRACE(1, ("In ntp_set_tod\n"));
	rc = -1;
	saved_errno = 0;

#ifdef HAVE_CLOCK_SETTIME
	if (rc && (SET_TOD_CLOCK_SETTIME == tod || !tod)) {
		errno = 0;
		rc = clock_settime(CLOCK_REALTIME, tvs);
		saved_errno = errno;
		TRACE(1, ("ntp_set_tod: clock_settime: %d %m\n", rc));
		if (!tod && !rc)
			tod = SET_TOD_CLOCK_SETTIME;

	}
#endif /* HAVE_CLOCK_SETTIME */
#ifdef HAVE_SETTIMEOFDAY
	if (rc && (SET_TOD_SETTIMEOFDAY == tod || !tod)) {
		struct timeval adjtv;

		/*
		 * Some broken systems don't reset adjtime() when the
		 * clock is stepped.
		 */
		adjtv.tv_sec = adjtv.tv_usec = 0;
		adjtime(&adjtv, NULL);
		errno = 0;

		adjtv.tv_sec = tvs->tv_sec;
		adjtv.tv_usec = (tvs->tv_nsec + 500) / 1000;
		rc = settimeofday(&adjtv, NULL);
		saved_errno = errno;
		TRACE(1, ("ntp_set_tod: settimeofday: %d %m\n", rc));
		if (!tod && !rc)
			tod = SET_TOD_SETTIMEOFDAY;
	}
#endif /* HAVE_SETTIMEOFDAY */
	errno = saved_errno;	/* for %m below */
	TRACE(1, ("ntp_set_tod: Final result: %s: %d %m\n",
		  set_tod_used[tod], rc));
	/*
	 * Say how we're setting the time of day
	 */
	if (!rc && NULL != set_tod_using) {
		(*set_tod_using)(set_tod_used[tod]);
		set_tod_using = NULL;
	}

	if (rc)
		errno = saved_errno;

	return rc;
}

