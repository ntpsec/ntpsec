/*
 * clockwork.c - the interface to the hardare clock
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
#else
#error POSIX clock_gettime(2) is required
#endif
    return 0;
}
#endif /* HAVE_CLOCK_GETTIME */

#ifdef HAVE_SYS_TIMEX_H
/*
 * ntp_adjtime at nanosecond precision.  Hiding the units difference
 * here helps prevent loss-of-precision bugs elsewhere.  We
 * deliberately don't merge STA_NANO into the status flags if it's
 * absent, however, this way callers can tell what accuracy they're
 * actually getting.
 *
 * Some versions of ntp_adtime(2), notably the Linux one which is
 * implemented in terms of a local, unstandardized adjtimex(2), have a
 * time member that can be used to retrieve and increment
 * (ADJ_SETOFFSET) system time.  If this were portable there would be
 * scaling of ntx->time.tv_usec in here for non-STA_NANO systems.  It
 * isn't; NetBSD and FreeBSD don't have that time member.
 *
 * Problem: the Linux manual page for adjtimex(2) says the precision
 * member is microseconds and doesn't mention STA_NANO, but the legacy
 * ntptime code has a scaling expression in it that implies
 * nanoseconds if that flash bit is on. It is unknown under what
 * circumstances, if any, this was ever correct.
 */
int ntp_adjtime_ns(struct timex *ntx)
{
#ifdef STA_NANO
    static bool nanoseconds = false;
    static int callcount = 0;
    if (callcount++ == 0){
	struct timex ztx;
	memset(&ztx, '\0', sizeof(ztx));
	ntp_adjtime(&ztx);
	nanoseconds = (STA_NANO & ztx.status) != 0;
    }
#endif

#ifdef STA_NANO
    if (!nanoseconds)
#endif
	ntx->offset /= 1000;
    int errval = ntp_adjtime(ntx);
#ifdef STA_NANO
    nanoseconds = (STA_NANO & ntx->status) != 0;
    if (!nanoseconds)
#endif
    {
	ntx->offset *= 1000;
	//ntx->precision *= 1000;
	ntx->jitter *= 1000;
    }
    return errval;
}
#endif /* HAVE_SYS_TIMEX_H */

#if !defined(HAVE_NTP_GETTIME) && defined(HAVE_NTP_ADJTIME)
int ntp_gettime(struct ntptimeval *ntv)
{
	struct timex tntx;
	int result;

	ZERO(tntx);
	result = ntp_adjtime(&tntx);
	ntv->time = tntx.time;
	ntv->maxerror = tntx.maxerror;
	ntv->esterror = tntx.esterror;
#if defined(HAVE_STRUCT_NTPTIMEVAL_TAI)
	ntv->tai = tntx.tai;
#endif
	return result;
}
#endif	/* !HAVE_NTP_GETTIME */

int
ntp_set_tod(
	struct timespec *tvs
	)
{
	int		rc;
	int		saved_errno;

	TRACE(1, ("In ntp_set_tod\n"));
#ifdef HAVE_CLOCK_SETTIME
	errno = 0;
	rc = clock_settime(CLOCK_REALTIME, tvs);
	saved_errno = errno;
	TRACE(1, ("ntp_set_tod: clock_settime: %d %m\n", rc));
#else
#error POSIX clock_gettime(2) is required
#endif /* HAVE_CLOCK_SETTIME */
	errno = saved_errno;	/* for %m below */
	TRACE(1, ("ntp_set_tod: Final result: clock_settime: %d %m\n", rc));

	if (rc)
		errno = saved_errno;

	return rc;
}

