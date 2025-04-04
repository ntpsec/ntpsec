/*
 * ntp_packetstamp.c - grubby platform-dependent details of packet timestamps
 *
 * One of our serious platform dependencies (things POSIX doesn't
 * specify a facility for) is isolated here.
 */
#include "config.h"

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#include "ntpd.h"
#include "ntp_stdlib.h"
#include "timespecops.h"

/* We handle 3 flavors of timestamp:
 * SO_TIMESTAMPNS/SCM_TIMESTAMPNS  Linux (maybe others)
 * SO_TS_CLOCK/SCM_REALTIME        FreeBSD
 * SO_TIMESTAMP/SCM_TIMESTAMP      Everybody else
 *
 * Linux supports both SO_TIMESTAMP and SO_TIMESTAMPNS so it's
 * important to check for SO_TIMESTAMPNS first to get the better accuracy.
 *
 * FreeBSD needs 2 setsockopt, SO_TIMESTAMP and SO_TS_CLOCK
 *
 * Note that the if/elif tests are done in several places.
 * It's important that they all check in the same order to
 * be consistent in case some systems support more than one.
 *
 * If SO_xxx exists, we assume that SCM_xxx does too.
 * All flavors assume the CMSG_xxx macros exist.
 *
 */


/*
 * Packet timestamps are based on the kernel's notion of the timestamp format,
 * which may not always agree with userspace expectations.  Some documentation
 * makes this clear, and some doesn't.  To get around this potential problem,
 * we provide two functions for optionally performing a conversion from
 * kernel format to userspace format.
 *
 * The format variations relate to whether the tv_sec field is 32 or 64 bits
 * (usually based on the time_t definition in the relevant context), whether
 * the tv_usec/tv_nsec field is 32 or 64 bits, and whether the structure
 * includes extra padding.  Although there's no reason for the tv_Xsec field
 * to be 64 bits to hold its value, in some cases it's extended to 64 bits
 * to incorporate padding that would otherwise be added, anyway.  This only
 * happens with a 64-bit tv_sec, which may trigger padding the entire structure
 * to a multiple of 64 bits if needed.
 *
 * The userspace format is always clearly defined by the structure definitions,
 * but the only information straightforwardly available about the kernel's
 * format is the cmsg payload length, which may have one of three values:
 *
 * 8 bytes:  This clearly indicates a 32+32 tv_sec/tv_Xsec format, with no
 * ambiguity.
 *
 * 12 bytes:  This could theoretically indicate either 64+32 or 32+64, but
 * since the only reason for a 64-bit tv_Xsec field is to subsume the padding
 * caused by a 64-bit tv_sec, in reality this can only mean 64+32.
 *
 * 16 bytes:  This could mean either 64+64 or 64+32 with 4 padding bytes.
 * It's not possible to tell which from the length alone.  On a little-endian
 * machine, the significant portion of the tv_Xsec field is always the first
 * longword, with the second being either a guaranteed all-zero upper half,
 * or 4 bytes of possibly uninitialized data.  In either case, merely using
 * the first longword and discarding the second is appropriate.
 *
 * In the 16-byte big-endian case, however, there's no way to determine which
 * longword is significant without examining the data.  In the 64+64 case,
 * the first tv_Xsec longword is guaranteed to be zero, while the second is
 * significant (and should be in the allowable range).  In the padded 64+32
 * case, the first longword is the significant part (and should be in range),
 * while the second is possibly uninitialized padding.  Hence, if the first
 * longword is nonzero and in range, then it's clearly the significant part;
 * if the first longword is zero and the second is in range, either the
 * second part is the significant part, or the value is exactly zero while
 * accompanied by padding that happens to be in range; and if the first
 * longword is out of range the timestamp is invalid.  In the second of the
 * cases mentioned, we assume the more likely possibility that the second
 * longword is the significant part.
 *
 * The code is currently written to assume that translation is only necessary
 * when the lengths are mismatched between the kernel and user timestamps,
 * though (as noted above) it's possible to have a format mismatch in the
 * 16-byte matching length case.  At present, there are no known cases of
 * this type of mismatch, so we assume that the current approach is valid.
 * If that assumption ever proves to be incorrect, the code could be adjusted
 * to treat the 16-byte case as always mismatching.
 *
 * The endian conditions here are coded as runtime tests, but since they're
 * based on compile-time constants, compiler optimization will normally
 * remove whichever case is unreachable, making them effectively compile-time
 * tests.
 *
 * Based on current usage of the timestamp features, only one of the two
 * functions defined here will be used in any given build, but we define
 * both to avoid additional conditionals that need to be kept in sync with
 * other parts of the code.  The void casts within the definitions suppress
 * the usual "unused function" warnings, and the "unemitted inline" warning
 * from clang is disabled explicitly.
 */

#define CMSG_DATALEN(cmsg) ((uint8_t *) (cmsg) + (cmsg)->cmsg_len \
	                    - (uint8_t *) CMSG_DATA(cmsg))

static const union {
	uint8_t c[8];
	uint64_t i;
} endian_test = {{1, 2, 3, 4, 5, 6, 7, 8}};
#define ENDIAN_LITTLE 0x0807060504030201ULL
#define IS_LITTLE_ENDIAN (endian_test.i == ENDIAN_LITTLE)

#define MAX_TV_USEC 1000000
#define MAX_TV_NSEC 1000000000

#if defined (__clang_major__) && __clang_major__ >= 3
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#endif

static inline struct timespec *
access_cmsg_timespec(struct cmsghdr *cmsghdr, struct timespec *temp)
{
	struct timespec_3232 {
		uint32_t	tv_sec;  /* Unsigned to get past 2038 */
		int32_t		tv_nsec;
	} *ts3232p;
	struct timespec_6432 {
		int64_t		tv_sec;
		int32_t		tv_nsec;
	} *ts6432p;
	#define SIZEOF_PACKED_TIMESPEC6432 (sizeof(int64_t) + sizeof(int32_t))
	struct timespec_6464 {
		int64_t		tv_sec;
		uint32_t	tv_nsec[2];  /* Unsigned for compares */
	} *ts6464p;
	int datalen = CMSG_DATALEN(cmsghdr);

	(void) access_cmsg_timespec;

	if (datalen == sizeof(struct timespec)) {
		return (struct timespec *) CMSG_DATA(cmsghdr);
	}

	switch (datalen) {

	case sizeof(struct timespec_3232):
		ts3232p = (struct timespec_3232 *) CMSG_DATA(cmsghdr);
		temp->tv_sec = ts3232p->tv_sec;
		temp->tv_nsec = ts3232p->tv_nsec;
		break;

	case SIZEOF_PACKED_TIMESPEC6432:
		ts6432p = (struct timespec_6432 *) CMSG_DATA(cmsghdr);
		temp->tv_sec = ts6432p->tv_sec;
		temp->tv_nsec = ts6432p->tv_nsec;
		break;

	case sizeof(struct timespec_6464):
		ts6464p = (struct timespec_6464 *) CMSG_DATA(cmsghdr);
		temp->tv_sec = ts6464p->tv_sec;
		if (IS_LITTLE_ENDIAN) {
			temp->tv_nsec = ts6464p->tv_nsec[0];
			break;
		} else if (ts6464p->tv_nsec[0] == 0) {
			if (ts6464p->tv_nsec[1] < MAX_TV_NSEC) {
				temp->tv_nsec = ts6464p->tv_nsec[1];
			} else {
				temp->tv_nsec = ts6464p->tv_nsec[0];
			}
			break;
		} else if (ts6464p->tv_nsec[0] < MAX_TV_NSEC) {
			temp->tv_nsec = ts6464p->tv_nsec[0];
			break;
		}
		/* FALLTHRU to default (invalid timestamp) */
		  FALLTHRU

	default:
		memset(temp, 0, sizeof(struct timespec));
	}

	return temp;
}

static inline struct timeval *
access_cmsg_timeval(struct cmsghdr *cmsghdr, struct timeval *temp)
{
	struct timeval_3232 {
		uint32_t	tv_sec;  /* Unsigned to get past 2038 */
		int32_t		tv_usec;
	} *tv3232p;
	struct timeval_6432 {
		int64_t		tv_sec;
		int32_t		tv_usec;
	} *tv6432p;
	#define SIZEOF_PACKED_TIMEVAL6432 (sizeof(int64_t) + sizeof(int32_t))
	struct timeval_6464 {
		int64_t		tv_sec;
		uint32_t	tv_usec[2];  /* Unsigned for compares */
	} *tv6464p;
	int datalen = CMSG_DATALEN(cmsghdr);

	(void) access_cmsg_timeval;

	if (datalen == sizeof(struct timeval)) {
		return (struct timeval *) CMSG_DATA(cmsghdr);
	}

	switch (datalen) {

	case sizeof(struct timeval_3232):
		tv3232p = (struct timeval_3232 *) CMSG_DATA(cmsghdr);
		temp->tv_sec = tv3232p->tv_sec;
		temp->tv_usec = tv3232p->tv_usec;
		break;

	case SIZEOF_PACKED_TIMEVAL6432:
		tv6432p = (struct timeval_6432 *) CMSG_DATA(cmsghdr);
		temp->tv_sec = tv6432p->tv_sec;
		temp->tv_usec = tv6432p->tv_usec;
		break;

	case sizeof(struct timeval_6464):
		tv6464p = (struct timeval_6464 *) CMSG_DATA(cmsghdr);
		temp->tv_sec = tv6464p->tv_sec;
		if (IS_LITTLE_ENDIAN) {
			temp->tv_usec = tv6464p->tv_usec[0];
			break;
		} else if (tv6464p->tv_usec[0] == 0) {
			if (tv6464p->tv_usec[1] < MAX_TV_USEC) {
				temp->tv_usec = tv6464p->tv_usec[1];
			} else {
				temp->tv_usec = tv6464p->tv_usec[0];
			}
			break;
		} else if (tv6464p->tv_usec[0] < MAX_TV_USEC) {
			temp->tv_usec = tv6464p->tv_usec[0];
			break;
		}
		/* FALLTHRU to default (invalid timestamp) */
		  FALLTHRU

	default:
		memset(temp, 0, sizeof(struct timeval));
	}

	return temp;
}

void
enable_packetstamps(
    int fd,
    sockaddr_u *	addr
    )
{
	const int	on = 1;
	static bool	once = false;
#if defined(SO_TS_CLOCK)
	const int	ts_type = SO_TS_REALTIME;
#endif

#if defined (SO_TIMESTAMPNS)
	if (!once) {
		once = true;
		msyslog(LOG_INFO, "INIT: Using SO_TIMESTAMPNS(ns)");
	}
	if (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMPNS,
			       (const void *)&on, sizeof(on)))
		msyslog(LOG_DEBUG,
			"ERR: setsockopt SO_TIMESTAMPNS on fails on address %s: %s",
				socktoa(addr), strerror(errno));
	else
		DPRINT(4, ("ERR: setsockopt SO_TIMESTAMPNS enabled on fd %d address %s\n",
				    fd, socktoa(addr)));
#elif defined(SO_TIMESTAMP)
#if !defined(SO_TS_CLOCK)
	if (!once) {
		once = true;
		msyslog(LOG_INFO, "INIT: Using SO_TIMESTAMP(us)");
	}
#endif
	if (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP,
			       (const void*)&on, sizeof(on)))
		msyslog(LOG_DEBUG,
			"ERR: setsockopt SO_TIMESTAMP on fails on address %s: %s",
			socktoa(addr), strerror(errno));
	else
		DPRINT(4, ("setsockopt SO_TIMESTAMP enabled on fd %d address %s\n",
			    fd, socktoa(addr)));
#if defined(SO_TS_CLOCK)
	/* FreeBSD */
	if (!once) {
		once = true;
		msyslog(LOG_INFO, "INIT: Using SO_TS_CLOCK(ns)");
	}
	if (setsockopt(fd, SOL_SOCKET, SO_TS_CLOCK,
			       (const void*)&ts_type, sizeof(ts_type)))
		msyslog(LOG_DEBUG,
			"ERR: setsockop SO_TS_CLOCK on fails on address %s: %s",
			socktoa(addr), strerror(errno));
	else
		DPRINT(4, ("setsockopt SO_TS_CLOCK enabled on fd %d address %s\n",
			    fd, socktoa(addr)));
#endif
#else
# error "Can't get packet timestamp"
#endif
}


/*
 * extract timestamps from control message buffer
 */
l_fp
fetch_packetstamp(
	struct msghdr *		msghdr
	)
{
	struct cmsghdr *	cmsghdr;
#if defined(SO_TIMESTAMPNS) || defined(SO_TS_CLOCK)
	struct timespec		*tsp, tstemp;
#elif defined(SO_TIMESTAMP)
	struct timeval		*tvp, tvtemp;
#endif
	l_fp			nts = 0;  /* network time stamp */

/* There should be only one cmsg. */
	cmsghdr = CMSG_FIRSTHDR(msghdr);
	if (NULL == cmsghdr) {
		DPRINT(4, ("fetch_timestamp: can't find timestamp\n"));
		msyslog(LOG_ERR, "ERR: fetch_timestamp: no msghdrs");
		exit(2);
		/* return ts;	** Kludge to use time from select. */
	}
#if defined(SO_TIMESTAMPNS)
	/* Linux and ?? */
	if (SCM_TIMESTAMPNS != cmsghdr->cmsg_type) {
#elif defined(SO_TS_CLOCK)
	/* FreeBSD */
	if (SCM_REALTIME != cmsghdr->cmsg_type) {
#elif defined(SO_TIMESTAMP)
	if (SCM_TIMESTAMP != cmsghdr->cmsg_type) {
#else
# error "Can't get packet timestamp"
#endif
                char errbuf[128];

                snprintf(errbuf, sizeof(errbuf),
                        "fetch_timestamp: strange control message "
                        "len x%x, level x%x type x%x\n",
		       (unsigned)cmsghdr->cmsg_len,
		       (unsigned)cmsghdr->cmsg_level,
		       (unsigned)cmsghdr->cmsg_type);
		DPRINT(4, ("%s", errbuf));
		msyslog(LOG_ERR, "ERR: %s", errbuf);

                // debug
                snprintf(errbuf, sizeof(errbuf),
                        "fetch_timestamp: sizeof(size_t) %lu, sizeof(int) %lu "
                        "sizeof(cmsghdr) %lu'n",
                        sizeof(size_t), sizeof(int), sizeof(struct cmsghdr));
		DPRINT(4, ("%s", errbuf));
		msyslog(LOG_ERR, "ERR: %s", errbuf);
                // debug

		exit(2);
		/* Could loop and skip strange types. */
		/* cmsghdr = CMSG_NXTHDR(msghdr, cmsghdr); */
	}

/* cmsghdr now points to a timestamp slot */

#if defined(SO_TIMESTAMPNS) || defined(SO_TS_CLOCK)
	tsp = access_cmsg_timespec(cmsghdr, &tstemp);
	DPRINT(4, ("fetch_timestamp: system nsec network time stamp: %ld.%09ld\n",
		(long)tsp->tv_sec, tsp->tv_nsec));
	nts = tspec_stamp_to_lfp(*tsp);
#elif defined(SO_TIMESTAMP)
	tvp = access_cmsg_timeval(cmsghdr, &tvtemp);
	DPRINT(4, ("fetch_timestamp: system usec network time stamp: %jd.%06ld\n",
		(intmax_t)tvp->tv_sec, (long)tvp->tv_usec));
	nts = tspec_stamp_to_lfp(tval_to_tspec(*tvp));
#else
# error "Can't get packet timestamp"
#endif
	return nts;
}

// end
