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
extern int ntp_adjtime_ns(struct timex *);
#endif

#ifndef HAVE_STRUCT_NTPTIMEVAL
struct ntptimeval
{
	struct timeval	time;		/* current time (ro) */
	long int	maxerror;	/* maximum error (us) (ro) */
	long int	esterror;	/* estimated error (us) (ro) */
};

int ntp_gettime(struct ntptimeval *);
#endif	/* !HAVE_STRUCT_NTPTIMEVAL */

/*
 * The units of the maxerror and esterror fields vary by platform.  If
 * STA_NANO is defined, they're in nanoseconds; otherwise in
 * microseconds. Hide the difference by normalizing everything to
 * float seconds.
 */
# ifdef STA_NANO
#define ntp_error_in_seconds(n)	((n)/1.0e9)
# else
#define ntp_error_in_seconds(n)	((n)/1.0e6)
# endif

/* MUSL port shim */
#if !defined(HAVE_NTP_ADJTIME) && defined(HAVE_ADJTIMEX)
#define ntp_adjtime adjtimex
#endif

#endif	/* GUARD_NTP_SYSCALL_H */
