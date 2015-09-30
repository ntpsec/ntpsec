/*
 * ntp_syscall.h - various ways to perform the ntp_adjtime() and ntp_gettime()
 * 		   system calls.
 */

#ifndef GUARD_NTP_SYSCALL_H
#define GUARD_NTP_SYSCALL_H

#ifdef HAVE_SYS_TIMEX_H
# include <sys/time.h>	/* prerequisite on NetBSD */
# include <sys/timex.h>
#endif

#ifdef HAVE_NTP_ADJTIME
extern	int	ntp_adjtime	(struct timex *);

struct ntptimeval
{
	struct timeval	time;		/* current time (ro) */
	long int	maxerror;	/* maximum error (us) (ro) */
	long int	esterror;	/* estimated error (us) (ro) */
};

# ifndef HAVE_NTP_GETTIME
static inline int
ntp_gettime(
	struct ntptimeval *ntv
	)
{
	struct timex tntx;
	int result;

	ZERO(tntx);
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
# endif	/* !HAVE_NTP_GETTIME */
#endif	/* !HAVE_NTP_ADJTIME */

#endif	/* GUARD_NTP_SYSCALL_H */
