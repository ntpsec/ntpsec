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

#if defined(SO_BINTIME) && defined(SCM_BINTIME) && defined(CMSG_FIRSTHDR)
#  define USE_PACKET_TIMESTAMP
#  define USE_SCM_BINTIME
/* UNUSED
 * #  ifdef OVERRIDE_BINTIME_CTLMSGBUF_SIZE
 * #   define CMSG_BUFSIZE OVERRIDE_BINTIME_CTLMSGBUF_SIZE
 * #  else
 * #   define CMSG_BUFSIZE  1536 * moderate default *
 * #  endif
 */
#elif defined(SO_TIMESTAMPNS) && defined(SCM_TIMESTAMPNS) && defined(CMSG_FIRSTHDR)
#  define USE_PACKET_TIMESTAMP
#  define USE_SCM_TIMESTAMPNS
/* UNUSED
 * #  ifdef OVERRIDE_TIMESTAMPNS_CTLMSGBUF_SIZE
 * #   define CMSG_BUFSIZE OVERRIDE_TIMESTAMPNS_CTLMSGBUF_SIZE
 * #  else
 * #   define CMSG_BUFSIZE  1536 * moderate default *
 * #  endif
 */
#elif defined(SO_TIMESTAMP) && defined(SCM_TIMESTAMP) && defined(CMSG_FIRSTHDR)
#  define USE_PACKET_TIMESTAMP
#  define USE_SCM_TIMESTAMP
/* UNUSED
 * #  ifdef OVERRIDE_TIMESTAMP_CTLMSGBUF_SIZE
 * #   define CMSG_BUFSIZE OVERRIDE_TIMESTAMP_CTLMSGBUF_SIZE
 * #  else
 * #   define CMSG_BUFSIZE  1536 * moderate default *
 * #  endif
 */
#else
/* fill in for old/other timestamp interfaces */
#endif

void
enable_packetstamps(
    int fd,
    sockaddr_u *	addr
    )
{

#ifdef USE_SCM_TIMESTAMP
	{
		const int	on = 1;
		if (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP,
			       (const char*)&on, sizeof(on)))
			msyslog(LOG_DEBUG,
				"setsockopt SO_TIMESTAMP on fails on address %s: %m",
				socktoa(addr));
		else
			DPRINTF(4, ("setsockopt SO_TIMESTAMP enabled on fd %d address %s\n",
				    fd, socktoa(addr)));
	}
#endif
#ifdef USE_SCM_TIMESTAMPNS
	{
		const int	on = 1;
		if (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMPNS,
			       (char*)&on, sizeof(on)))
			msyslog(LOG_DEBUG,
				"setsockopt SO_TIMESTAMPNS on fails on address %s: %m",
				socktoa(addr));
		else
			DPRINTF(4, ("setsockopt SO_TIMESTAMPNS enabled on fd %d address %s\n",
				    fd, socktoa(addr)));
	}
#endif
#ifdef USE_SCM_BINTIME
	{
		const int	on = 1;
		if (setsockopt(fd, SOL_SOCKET, SO_BINTIME,
			       (char*)&on, sizeof(on)))
			msyslog(LOG_DEBUG,
				"setsockopt SO_BINTIME on fails on address %s: %m",
				socktoa(addr));
		else
			DPRINTF(4, ("setsockopt SO_BINTIME enabled on fd %d address %s\n",
				    fd, socktoa(addr)));
	}
#endif
}


#ifdef USE_PACKET_TIMESTAMP
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
#ifdef USE_SCM_BINTIME
	struct bintime *	btp;
#endif
#ifdef USE_SCM_TIMESTAMPNS
	struct timespec *	tsp;
#endif
#ifdef USE_SCM_TIMESTAMP
	struct timeval *	tvp;
#endif
	unsigned long		ticks;
	double			fuzz;
	l_fp			lfpfuzz;
	l_fp			nts = 0;
#ifdef ENABLE_DEBUG_TIMING
	l_fp			dts;
#endif

#ifndef ENABLE_DEBUG_TIMING
	UNUSED_ARG(rb);
#endif

	cmsghdr = CMSG_FIRSTHDR(msghdr);
	while (cmsghdr != NULL) {
		switch (cmsghdr->cmsg_type)
		{
#ifdef USE_SCM_BINTIME
		case SCM_BINTIME:
#endif  /* USE_SCM_BINTIME */
#ifdef USE_SCM_TIMESTAMPNS
		case SCM_TIMESTAMPNS:
#endif	/* USE_SCM_TIMESTAMPNS */
#ifdef USE_SCM_TIMESTAMP
		case SCM_TIMESTAMP:
#endif	/* USE_SCM_TIMESTAMP */
#if defined(USE_SCM_BINTIME) || defined (USE_SCM_TIMESTAMPNS) || defined(USE_SCM_TIMESTAMP)
			switch (cmsghdr->cmsg_type)
			{
#ifdef USE_SCM_BINTIME
			case SCM_BINTIME:
				btp = (struct bintime *)CMSG_DATA(cmsghdr);
				/*
				 * bintime documentation is at http://phk.freebsd.dk/pubs/timecounter.pdf
				 */
				setlfpuint(nts, btp->sec + JAN_1970);
				setlfpfrac(nts, (uint32_t)(btp->frac >> 32));
				if (sys_tick > measured_tick &&
				    sys_tick > S_PER_NS) {
				    ticks = (unsigned long)(lfpfrac(nts) / (unsigned long)(sys_tick * FRAC));
				    setlfpfrac(nts, (unsigned long)(ticks * (unsigned long)(sys_tick * FRAC)));
				}
                                DPRINTF(4, ("fetch_timestamp: system bintime network time stamp: %ld.%09lu\n",
                                            (long)btp->sec, (unsigned long)((lfpfrac(nts) / FRAC) * 1e9)));
				break;
#endif  /* USE_SCM_BINTIME */
#ifdef USE_SCM_TIMESTAMPNS
			case SCM_TIMESTAMPNS:
				tsp = (struct timespec *)CMSG_DATA(cmsghdr);
				if (sys_tick > measured_tick &&
				    sys_tick > S_PER_NS) {
					ticks = (unsigned long)
                                            ((tsp->tv_nsec * S_PER_NS) /
						       sys_tick);
					tsp->tv_nsec = (long)(ticks * NS_PER_S *
							      sys_tick);
				}
				DPRINTF(4, ("fetch_timestamp: system nsec network time stamp: %ld.%09ld\n",
					    tsp->tv_sec, tsp->tv_nsec));
				nts = tspec_stamp_to_lfp(*tsp);
				break;
#endif	/* USE_SCM_TIMESTAMPNS */
#ifdef USE_SCM_TIMESTAMP
			case SCM_TIMESTAMP:
				tvp = (struct timeval *)CMSG_DATA(cmsghdr);
				if (sys_tick > measured_tick &&
				    sys_tick > 1e-6) {
					ticks = (unsigned long)((tvp->tv_usec * 1e-6) /
						       sys_tick);
					tvp->tv_usec = (long)(ticks * US_PER_S *
							      sys_tick);
				}
				DPRINTF(4, ("fetch_timestamp: system usec network time stamp: %jd.%06ld\n",
					    (intmax_t)tvp->tv_sec, (long)tvp->tv_usec));
				nts = tspec_stamp_to_lfp(tval_to_tspec(*tvp));
				break;
#endif  /* USE_SCM_TIMESTAMP */
			}
			fuzz = ntp_random() * 2. / FRAC * sys_fuzz;
			lfpfuzz = dtolfp(fuzz);
			nts += lfpfuzz;
#ifdef ENABLE_DEBUG_TIMING
			dts = ts;
			dts -= nts;
			collect_timing(rb, "input processing delay", 1, dts);
			DPRINTF(4, ("fetch_timestamp: timestamp delta: %s (incl. fuzz)\n",
				    lfptoa(dts, 9)));
#endif	/* ENABLE_DEBUG_TIMING */
			ts = nts;  /* network time stamp */
			break;
#endif	/* USE_SCM_BINTIME || USE_SCM_TIMESTAMPNS || USE_SCM_TIMESTAMP */

		default:
			DPRINTF(4,
                            ("fetch_timestamp: skipping control message 0x%x\n",
			     (unsigned)cmsghdr->cmsg_type));
		}
		cmsghdr = CMSG_NXTHDR(msghdr, cmsghdr);
	}
	return ts;
}
#endif	/* USE_PACKET_TIMESTAMP */

// end
