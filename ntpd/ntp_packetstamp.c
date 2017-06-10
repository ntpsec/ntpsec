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
#ifdef HAVE_SYS_SOCKIO_H	/* UXPV: SIOC* #defines (Frank Vance <fvance@waii.com>) */
# include <sys/sockio.h>
#endif

#include "ntpd.h"
#include "ntp_stdlib.h"
#include "timespecops.h"

/* We handle 2 flavors of timestamp:
 * SO_TIMESTAMPNS/SCM_TIMESTAMPNS  Linux
 * SO_TIMESTAMP/SCM_TIMESTAMP      FreeBSD, NetBSD, OpenBSD, Linux, macOS,
 *                                 Solaris
 *
 * Linux supports both SO_TIMESTAMP and SO_TIMESTAMPNS so it's
 * important to check for SO_TIMESTAMPNS first to get the better accuracy.
 *
 * Note that the if/elif tests are done in several places.
 * It's important that they all check in the same order to
 * be consistent in case some systems support more than one.
 *
 * If SO_xxx exists, we assume that SCM_xxx does too.
 * All flavors assume the CMSG_xxx macros exist.
 *
 * FreeBSD has SO_BINTIME/SCM_BINTIME
 *   It has better resolution, but it doesn't work for IPv6
 *   bintime documentation is at
 *   http://phk.freebsd.dk/pubs/timecounter.pdf
 */


void
enable_packetstamps(
    int fd,
    sockaddr_u *	addr
    )
{
	const int	on = 1;
	static bool	once = false;

#if defined (SO_TIMESTAMPNS)
	if (!once) {
		once = true;
		msyslog(LOG_INFO, "Using SO_TIMESTAMPNS");
	}
	if (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMPNS,
			       (const void *)&on, sizeof(on)))
		msyslog(LOG_DEBUG,
			"setsockopt SO_TIMESTAMPNS on fails on address %s: %m",
				socktoa(addr));
	else
		DPRINT(4, ("setsockopt SO_TIMESTAMPNS enabled on fd %d address %s\n",
				    fd, socktoa(addr)));
#elif defined(SO_TIMESTAMP)
	if (!once) {
		once = true;
		msyslog(LOG_INFO, "Using SO_TIMESTAMP");
	}
	if (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP,
			       (const void*)&on, sizeof(on)))
		msyslog(LOG_DEBUG,
			"setsockopt SO_TIMESTAMP on fails on address %s: %m",
			socktoa(addr));
	else
		DPRINT(4, ("setsockopt SO_TIMESTAMP enabled on fd %d address %s\n",
			    fd, socktoa(addr)));
#else
# error "Can't get packet timestamp"
#endif
}


/*
 * extract timestamps from control message buffer
 */
l_fp
fetch_packetstamp(
	struct recvbuf *	rb,
	struct msghdr *		msghdr,
	l_fp			ts
	)
{
	struct cmsghdr *	cmsghdr;
#if defined(SO_TIMESTAMPNS)
	struct timespec *	tsp;
#elif defined(SO_TIMESTAMP)
	struct timeval *	tvp;
#endif
	unsigned long		ticks;
	double			fuzz;
	l_fp			lfpfuzz;
	l_fp			nts = 0;  /* network time stamp */
#ifdef ENABLE_DEBUG_TIMING
	l_fp			dts;
#endif

#ifndef ENABLE_DEBUG_TIMING
	UNUSED_ARG(rb);
#endif

/* There should be only one cmsg. */
	cmsghdr = CMSG_FIRSTHDR(msghdr);
	if (NULL == cmsghdr) {
		DPRINT(4, ("fetch_timestamp: can't find timestamp\n"));
		msyslog(LOG_ERR,
			"fetch_timestamp: no msghdrs, %s",
			socktoa(&rb->recv_srcadr));
		exit(2);
		/* return ts;	** Kludge to use time from select. */
	}
#if defined(SO_TIMESTAMPNS)
	if (SCM_TIMESTAMPNS != cmsghdr->cmsg_type) {
#elif defined(SO_TIMESTAMP)
	if (SCM_TIMESTAMP != cmsghdr->cmsg_type) {
#else
# error "Can't get packet timestamp"
#endif
		DPRINT(4,	
                        ("fetch_timestamp: strange control message 0x%x\n",
			     (unsigned)cmsghdr->cmsg_type));
		msyslog(LOG_ERR,
			"fetch_timestamp: strange control message 0x%x",
                             (unsigned)cmsghdr->cmsg_type);
		exit(2);
		/* Could loop and skip strange types. */
		/* cmsghdr = CMSG_NXTHDR(msghdr, cmsghdr); */
	}

/* cmsghdr now points to a timestamp slot */

#if defined(SO_TIMESTAMPNS)
	tsp = (struct timespec *)CMSG_DATA(cmsghdr);
	if (sys_tick > measured_tick && sys_tick > S_PER_NS) {
	    ticks = (unsigned long) ((tsp->tv_nsec * S_PER_NS) / sys_tick);
	    tsp->tv_nsec = (long) (ticks * NS_PER_S * sys_tick);
	}
	DPRINT(4, ("fetch_timestamp: system nsec network time stamp: %ld.%09ld\n",
		tsp->tv_sec, tsp->tv_nsec));
	nts = tspec_stamp_to_lfp(*tsp);
#elif defined(SO_TIMESTAMP)
	tvp = (struct timeval *)CMSG_DATA(cmsghdr);
	if (sys_tick > measured_tick && sys_tick > S_PER_NS) {
	    ticks = (unsigned long) ((tvp->tv_usec * S_PER_NS) / sys_tick);
	    tvp->tv_usec = (long)(ticks * US_PER_S * sys_tick);
	}
	DPRINT(4, ("fetch_timestamp: system usec network time stamp: %jd.%06ld\n",
		(intmax_t)tvp->tv_sec, (long)tvp->tv_usec));
	nts = tspec_stamp_to_lfp(tval_to_tspec(*tvp));
#else
# error "Can't get packet timestamp"
#endif
	fuzz = ntp_random() * 2. / FRAC * sys_fuzz;
	lfpfuzz = dtolfp(fuzz);
	nts += lfpfuzz;
#ifdef ENABLE_DEBUG_TIMING
	dts = ts;
	dts -= nts;
	collect_timing(rb, "input processing delay", 1, dts);
	DPRINT(4, ("fetch_timestamp: timestamp delta: %s (incl. fuzz)\n",
		lfptoa(dts, 9)));
#endif	/* ENABLE_DEBUG_TIMING */
	ts = nts;

	return ts;
}

// end
