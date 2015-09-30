/*
 * ntp_syscall.h - various ways to perform the ntp_adjtime() and ntp_gettime()
 * 		   system calls.
 *
 * On most systems including <sys/timex.h> will bring in declarations
 * for the BSD functions ntp_gettime(2) and ntp_adjtime(2). (Linux
 * using glibc has these, though they're not visible in the manual
 * pages.)
 */

#ifndef GUARD_NTP_SYSCALL_H
#define GUARD_NTP_SYSCALL_H

#ifdef HAVE_SYS_TIMEX_H
# include <sys/time.h>	/* prerequisite on NetBSD */
# include <sys/timex.h>
#endif

#ifndef HAVE_NTP_GETTIME
struct ntptimeval
{
	struct timeval	time;		/* current time (ro) */
	long int	maxerror;	/* maximum error (us) (ro) */
	long int	esterror;	/* estimated error (us) (ro) */
};

int ntp_gettime(struct ntptimeval *);
#endif	/* !HAVE_NTP_GETTIME */

#endif	/* GUARD_NTP_SYSCALL_H */
