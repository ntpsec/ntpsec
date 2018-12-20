/*
 * clockwork.c - the interface to the hardare clock
 */

#include "config.h"

#include <unistd.h>
#include <sys/time.h>	/* prerequisite on NetBSD */
#include <sys/timex.h>

#include "ntp.h"
#include "ntp_machine.h"
#include "ntp_syslog.h"
#include "ntp_stdlib.h"
#include "lib_strbuf.h"
#include "ntp_debug.h"
#include "ntp_syscall.h"

#ifdef HAVE_SYS_TIMEX_H
/*
 * ntp_adjtime at nanosecond precision.  Hiding the units difference
 * here helps prevent loss-of-precision bugs elsewhere.  We
 * deliberately don't merge STA_NANO into the status flags if it's
 * absent, however, this way callers can tell what accuracy they're
 * actually getting.
 *
 * Some versions of ntp_adjtime(2), notably the Linux one which is
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

int
ntp_set_tod(
	struct timespec *tvs
	)
{
	int		rc;
	int		saved_errno;

	TPRINT(1, ("In ntp_set_tod\n"));
	errno = 0;
	rc = clock_settime(CLOCK_REALTIME, tvs);
	saved_errno = errno;
	TPRINT(1, ("ntp_set_tod: clock_settime: %d %m\n", rc));
	errno = saved_errno;	/* for %m below */
	TPRINT(1, ("ntp_set_tod: Final result: clock_settime: %d %m\n", rc));

	if (rc)
		errno = saved_errno;

	return rc;
}

