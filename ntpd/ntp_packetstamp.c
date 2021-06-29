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
	struct timespec *	tsp;
#elif defined(SO_TIMESTAMP)
	struct timeval *	tvp;
#endif
#ifdef ENABLE_FUZZ
	unsigned long		ticks;
	double			fuzz;
	l_fp			lfpfuzz;
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
		DPRINT(4,
                        ("fetch_timestamp: strange control message 0x%x\n",
			     (unsigned)cmsghdr->cmsg_type));
		msyslog(LOG_ERR,
			"ERR: fetch_timestamp: strange control message 0x%x",
                             (unsigned)cmsghdr->cmsg_type);
		exit(2);
		/* Could loop and skip strange types. */
		/* cmsghdr = CMSG_NXTHDR(msghdr, cmsghdr); */
	}

/* cmsghdr now points to a timestamp slot */

#if defined(SO_TIMESTAMPNS) || defined(SO_TS_CLOCK)
	tsp = (struct timespec *)CMSG_DATA(cmsghdr);
#ifdef ENABLE_FUZZ
	if (sys_tick > measured_tick && sys_tick > S_PER_NS) {
	    ticks = (unsigned long) ((tsp->tv_nsec * S_PER_NS) / sys_tick);
	    tsp->tv_nsec = (long) (ticks * NS_PER_S * sys_tick);
	}
#endif
	DPRINT(4, ("fetch_timestamp: system nsec network time stamp: %ld.%09ld\n",
		(long)tsp->tv_sec, tsp->tv_nsec));
	nts = tspec_stamp_to_lfp(*tsp);
#elif defined(SO_TIMESTAMP)
	tvp = (struct timeval *)CMSG_DATA(cmsghdr);
#ifdef ENABLE_FUZZ
	if (sys_tick > measured_tick && sys_tick > S_PER_NS) {
	    ticks = (unsigned long) ((tvp->tv_usec * S_PER_NS) / sys_tick);
	    tvp->tv_usec = (long)(ticks * US_PER_S * sys_tick);
	}
#endif
	DPRINT(4, ("fetch_timestamp: system usec network time stamp: %jd.%06ld\n",
		(intmax_t)tvp->tv_sec, (long)tvp->tv_usec));
	nts = tspec_stamp_to_lfp(tval_to_tspec(*tvp));
#else
# error "Can't get packet timestamp"
#endif
#ifdef ENABLE_FUZZ
/*	fuzz = ntp_random() * 2. / FRAC * sys_fuzz; */
	fuzz = random() * 2. / FRAC * sys_fuzz;
	lfpfuzz = dtolfp(fuzz);
	nts += lfpfuzz;
#endif
	return nts;
}

// end
