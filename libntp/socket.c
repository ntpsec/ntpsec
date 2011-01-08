/*
 * socket.c - low-level socket operations
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#include "ntp.h"
// #include "ntp_assert.h"
#include "ntp_types.h"
#include "ntp_net.h"
#include "ntp_io.h"

#include <isc/util.h>

/*
 * Windows C runtime ioctl() can't deal properly with sockets, 
 * map to ioctlsocket for this source file.
 */
#ifdef SYS_WINNT
#define ioctl(fd, opt, val)  ioctlsocket(fd, opt, (u_long *)(val))
#endif

/* HMS: Is this the best place to put the qos global? */

#if defined(HAVE_IPTOS_SUPPORT)
/* set IP_TOS to minimize packet delay */
unsigned int qos =
# if defined(IPTOS_PREC_INTERNETCONTROL)
			IPTOS_PREC_INTERNETCONTROL
# else
			IPTOS_LOWDELAY;
# endif
			;
#endif

/*
 * on Unix systems the stdio library typically
 * makes use of file descriptors in the lower
 * integer range.  stdio usually will make use
 * of the file descriptors in the range of
 * [0..FOPEN_MAX)
 * in order to keep this range clean, for socket
 * file descriptors we attempt to move them above
 * FOPEN_MAX. This is not as easy as it sounds as
 * FOPEN_MAX changes from implementation to implementation
 * and may exceed to current file decriptor limits.
 * We are using following strategy:
 * - keep a current socket fd boundary initialized with
 *   max(0, min(GETDTABLESIZE() - FD_CHUNK, FOPEN_MAX))
 * - attempt to move the descriptor to the boundary or
 *   above.
 *   - if that fails and boundary > 0 set boundary
 *     to min(0, socket_fd_boundary - FD_CHUNK)
 *     -> retry
 *     if failure and boundary == 0 return old fd
 *   - on success close old fd return new fd
 *
 * effects:
 *   - fds will be moved above the socket fd boundary
 *     if at all possible.
 *   - the socket boundary will be reduced until
 *     allocation is possible or 0 is reached - at this
 *     point the algrithm will be disabled
 */
SOCKET
move_fd(
	SOCKET fd
	)
{
#if !defined(SYS_WINNT) && defined(F_DUPFD)
#ifndef FD_CHUNK
#define FD_CHUNK	10
#endif
#ifndef FOPEN_MAX
#define FOPEN_MAX	20
#endif
/*
 * number of fds we would like to have for
 * stdio FILE* available.
 * we can pick a "low" number as our use of
 * FILE* is limited to log files and temporarily
 * to data and config files. Except for log files
 * we don't keep the other FILE* open beyond the
 * scope of the function that opened it.
 */
#ifndef FD_PREFERRED_SOCKBOUNDARY
#define FD_PREFERRED_SOCKBOUNDARY 48
#endif

	static SOCKET socket_boundary = -1;
	SOCKET newfd;

	NTP_REQUIRE((int)fd >= 0);

	/*
	 * check whether boundary has be set up
	 * already
	 */
	if (socket_boundary == -1) {
		socket_boundary = max(0, min(GETDTABLESIZE() - FD_CHUNK,
					     min(FOPEN_MAX, FD_PREFERRED_SOCKBOUNDARY)));
#ifdef DEBUG
		msyslog(LOG_DEBUG,
			"ntp_io: estimated max descriptors: %d, initial socket boundary: %d",
			GETDTABLESIZE(), socket_boundary);
#endif
	}

	/*
	 * Leave a space for stdio to work in. potentially moving the
	 * socket_boundary lower until allocation succeeds.
	 */
	do {
		if (fd >= 0 && fd < socket_boundary) {
			/* inside reserved range: attempt to move fd */
			newfd = fcntl(fd, F_DUPFD, socket_boundary);

			if (newfd != -1) {
				/* success: drop the old one - return the new one */
				close(fd);
				return newfd;
			}
		} else {
			/* outside reserved range: no work - return the original one */
			return fd;
		}
		socket_boundary = max(0, socket_boundary - FD_CHUNK);
#ifdef DEBUG
		msyslog(LOG_DEBUG,
			"ntp_io: selecting new socket boundary: %d",
			socket_boundary);
#endif
	} while (socket_boundary > 0);
#else
	NTP_REQUIRE((int)fd >= 0);
#endif /* !defined(SYS_WINNT) && defined(F_DUPFD) */
	return fd;
}


/*
 * make_socket_nonblocking() - set up descriptor to be non blocking
 */
void
make_socket_nonblocking(
	SOCKET fd
	)
{
	/*
	 * set non-blocking,
	 */

#ifdef USE_FIONBIO
	/* in vxWorks we use FIONBIO, but the others are defined for old
	 * systems, so all hell breaks loose if we leave them defined
	 */
#undef O_NONBLOCK
#undef FNDELAY
#undef O_NDELAY
#endif

#if defined(O_NONBLOCK) /* POSIX */
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		msyslog(LOG_ERR,
			"fcntl(O_NONBLOCK) fails on fd #%d: %m", fd);
		exit(1);
	}
#elif defined(FNDELAY)
	if (fcntl(fd, F_SETFL, FNDELAY) < 0) {
		msyslog(LOG_ERR, "fcntl(FNDELAY) fails on fd #%d: %m",
			fd);
		exit(1);
	}
#elif defined(O_NDELAY) /* generally the same as FNDELAY */
	if (fcntl(fd, F_SETFL, O_NDELAY) < 0) {
		msyslog(LOG_ERR, "fcntl(O_NDELAY) fails on fd #%d: %m",
			fd);
		exit(1);
	}
#elif defined(FIONBIO)
	{
		int on = 1;

		if (ioctl(fd, FIONBIO, &on) < 0) {
			msyslog(LOG_ERR,
				"ioctl(FIONBIO) fails on fd #%d: %m",
				fd);
			exit(1);
		}
	}
#elif defined(FIOSNBIO)
	if (ioctl(fd, FIOSNBIO, &on) < 0) {
		msyslog(LOG_ERR,
			"ioctl(FIOSNBIO) fails on fd #%d: %m", fd);
		exit(1);
	}
#else
# include "Bletch: Need non-blocking I/O!"
#endif
}

#if 0
/*
 * open_socket - open a socket, returning the file descriptor
 */

static SOCKET
open_socket(
	sockaddr_u *	addr,
	int		bcast,
	int		turn_off_reuse,
	endpt *		interf
	)
{
	SOCKET	fd;
	int	errval;
	char	scopetext[16];
	/*
	 * int is OK for REUSEADR per
	 * http://www.kohala.com/start/mcast.api.txt
	 */
	int	on = 1;
	int	off = 0;

	if (IS_IPV6(addr) && !ipv6_works)
		return INVALID_SOCKET;

	/* create a datagram (UDP) socket */
	fd = socket(AF(addr), SOCK_DGRAM, 0);
	if (INVALID_SOCKET == fd) {
		errval = socket_errno();
		msyslog(LOG_ERR,
			"socket(AF_INET%s, SOCK_DGRAM, 0) failed on address %s: %m",
			IS_IPV6(addr) ? "6" : "", stoa(addr));

		if (errval == EPROTONOSUPPORT ||
		    errval == EAFNOSUPPORT ||
		    errval == EPFNOSUPPORT)
			return (INVALID_SOCKET);

		errno = errval;
		msyslog(LOG_ERR,
			"unexpected socket() error %m code %d (not EPROTONOSUPPORT nor EAFNOSUPPORT nor EPFNOSUPPORT) - exiting",
			errno);
		exit(1);
	}

#ifdef SYS_WINNT
	connection_reset_fix(fd, addr);
#endif
	/*
	 * Fixup the file descriptor for some systems
	 * See bug #530 for details of the issue.
	 */
	fd = move_fd(fd);

	/*
	 * set SO_REUSEADDR since we will be binding the same port
	 * number on each interface according to turn_off_reuse.
	 * This is undesirable on Windows versions starting with
	 * Windows XP (numeric version 5.1).
	 */
#ifdef SYS_WINNT
	if (isc_win32os_versioncheck(5, 1, 0, 0) < 0)  /* before 5.1 */
#endif
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
			       (char *)((turn_off_reuse)
					    ? &off
					    : &on),
			       sizeof(on))) {

			msyslog(LOG_ERR,
				"setsockopt SO_REUSEADDR %s fails for address %s: %m",
				(turn_off_reuse)
				    ? "off"
				    : "on",
				stoa(addr));
			closesocket(fd);
			return INVALID_SOCKET;
		}
#ifdef SO_EXCLUSIVEADDRUSE
	/*
	 * setting SO_EXCLUSIVEADDRUSE on the wildcard we open
	 * first will cause more specific binds to fail.
	 */
	if (!(interf->flags & INT_WILDCARD))
		set_excladdruse(fd);
#endif

	/*
	 * IPv4 specific options go here
	 */
	if (IS_IPV4(addr)) {
#if defined(HAVE_IPTOS_SUPPORT)
		if (setsockopt(fd, IPPROTO_IP, IP_TOS, (char *)&qos,
			       sizeof(qos)))
			msyslog(LOG_ERR,
				"setsockopt IP_TOS (%02x) fails on address %s: %m",
				qos, stoa(addr));
#endif /* HAVE_IPTOS_SUPPORT */
		if (bcast)
			socket_broadcast_enable(interf, fd, addr);
	}

	/*
	 * IPv6 specific options go here
	 */
	if (IS_IPV6(addr)) {
#ifdef IPV6_V6ONLY
		if (isc_net_probe_ipv6only() == ISC_R_SUCCESS
		    && setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY,
		    (char*)&on, sizeof(on)))
			msyslog(LOG_ERR,
				"setsockopt IPV6_V6ONLY on fails on address %s: %m",
				stoa(addr));
#endif
#ifdef IPV6_BINDV6ONLY
		if (setsockopt(fd, IPPROTO_IPV6, IPV6_BINDV6ONLY,
		    (char*)&on, sizeof(on)))
			msyslog(LOG_ERR,
				"setsockopt IPV6_BINDV6ONLY on fails on address %s: %m",
				stoa(addr));
#endif
	}

#ifdef OS_NEEDS_REUSEADDR_FOR_IFADDRBIND
	/*
	 * some OSes don't allow binding to more specific
	 * addresses if a wildcard address already bound
	 * to the port and SO_REUSEADDR is not set
	 */
	if (!is_wildcard_addr(addr))
		set_wildcard_reuse(AF(addr), 1);
#endif

	/*
	 * bind the local address.
	 */
	errval = bind(fd, &addr->sa, SOCKLEN(addr));

#ifdef OS_NEEDS_REUSEADDR_FOR_IFADDRBIND
	if (!is_wildcard_addr(addr))
		set_wildcard_reuse(AF(addr), 0);
#endif

	if (errval < 0) {
		/*
		 * Don't log this under all conditions
		 */
		if (turn_off_reuse == 0
#ifdef DEBUG
		    || debug > 1
#endif
		    ) {
			if (SCOPE(addr))
				snprintf(scopetext, sizeof(scopetext),
					 "%%%d", SCOPE(addr));
			else
				scopetext[0] = 0;

			msyslog(LOG_ERR,
				"bind(%d) AF_INET%s %s%s#%d%s flags 0x%x failed: %m",
				fd, IS_IPV6(addr) ? "6" : "",
				stoa(addr), scopetext, SRCPORT(addr),
				IS_MCAST(addr) ? " (multicast)" : "",
				interf->flags);
		}

		closesocket(fd);

		return INVALID_SOCKET;
	}

#ifdef HAVE_TIMESTAMP
	{
		if (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP,
			       (char*)&on, sizeof(on)))
			msyslog(LOG_DEBUG,
				"setsockopt SO_TIMESTAMP on fails on address %s: %m",
				stoa(addr));
		else
			DPRINTF(4, ("setsockopt SO_TIMESTAMP enabled on fd %d address %s\n",
				    fd, stoa(addr)));
	}
#endif
	DPRINTF(4, ("bind(%d) AF_INET%s, addr %s%%%d#%d, flags 0x%x\n",
		   fd, IS_IPV6(addr) ? "6" : "", stoa(addr),
		   SCOPE(addr), SRCPORT(addr), interf->flags));

	init_nonblocking_io(fd);

#ifdef HAVE_SIGNALED_IO
	init_socket_sig(fd);
#endif /* not HAVE_SIGNALED_IO */

	add_fd_to_list(fd, FD_TYPE_SOCKET);

#if !defined(SYS_WINNT) && !defined(VMS)
	DPRINTF(4, ("flags for fd %d: 0x%x\n", fd,
		    fcntl(fd, F_GETFL, 0)));
#endif /* SYS_WINNT || VMS */

#if defined (HAVE_IO_COMPLETION_PORT)
/*
 * Add the socket to the completion port
 */
	if (io_completion_port_add_socket(fd, interf)) {
		msyslog(LOG_ERR, "unable to set up io completion port - EXITING");
		exit(1);
	}
#endif
	return fd;
}


#ifdef SYS_WINNT
#define sendto(fd, buf, len, flags, dest, destsz)	\
	io_completion_port_sendto(fd, buf, len, (sockaddr_u *)(dest))
#endif

/* XXX ELIMINATE sendpkt similar in ntpq.c, ntpdc.c, ntp_io.c, ntptrace.c */
/*
 * sendpkt - send a packet to the specified destination. Maintain a
 * send error cache so that only the first consecutive error for a
 * destination is logged.
 */
void
sendpkt(
	sockaddr_u *		dest,
	struct interface *	ep,
	int			ttl,
	struct pkt *		pkt,
	int			len
	)
{
	endpt *	src;
	int	ismcast;
	int	cc;
	int	rc;
	u_char	cttl;

	ismcast = IS_MCAST(dest);
	if (!ismcast)
		src = ep;
	else
		src = (IS_IPV4(dest))
			  ? mc4_list
			  : mc6_list;

	if (NULL == src) {
		/*
		 * unbound peer - drop request and wait for better
		 * network conditions
		 */
		DPRINTF(2, ("%ssendpkt(dst=%s, ttl=%d, len=%d): no interface - IGNORED\n",
			    ismcast ? "\tMCAST\t***** " : "",
			    stoa(dest), ttl, len));
		return;
	}

	do {
		DPRINTF(2, ("%ssendpkt(%d, dst=%s, src=%s, ttl=%d, len=%d)\n",
			    ismcast ? "\tMCAST\t***** " : "", src->fd,
			    stoa(dest), stoa(&src->sin), ttl, len));
#ifdef MCAST
		/*
		 * for the moment we use the bcast option to set multicast ttl
		 */
		if (ismcast && ttl > 0 && ttl != src->last_ttl) {
			/*
			 * set the multicast ttl for outgoing packets
			 */
			switch (AF(&src->sin)) {

			case AF_INET :
				cttl = (u_char)ttl;
				rc = setsockopt(src->fd, IPPROTO_IP,
						IP_MULTICAST_TTL,
						(void *)&cttl,
						sizeof(cttl));
				break;

# ifdef INCLUDE_IPV6_SUPPORT
			case AF_INET6 :
				rc = setsockopt(src->fd, IPPROTO_IPV6,
						 IPV6_MULTICAST_HOPS,
						 (void *)&ttl,
						 sizeof(ttl));
				break;
# endif	/* INCLUDE_IPV6_SUPPORT */

			default:
				rc = 0;
			}

			if (!rc)
				src->last_ttl = ttl;
			else
				msyslog(LOG_ERR,
					"setsockopt IP_MULTICAST_TTL/IPV6_MULTICAST_HOPS fails on address %s: %m",
					stoa(&src->sin));
		}
#endif	/* MCAST */

#ifdef SIM
		cc = simulate_server(dest, src, pkt);
#else
		cc = sendto(src->fd, (char *)pkt, (u_int)len, 0,
			    &dest->sa, SOCKLEN(dest));
#endif
		if (cc == -1) {
			src->notsent++;
			packets_notsent++;
		} else	{
			src->sent++;
			packets_sent++;
		}
		if (ismcast)
			src = src->mclink;
	} while (ismcast && src != NULL);
}

/*
 * Routine to read the refclock packets for a specific interface
 * Return the number of bytes read. That way we know if we should
 * read it again or go on to the next one if no bytes returned
 */
static inline int
read_refclock_packet(SOCKET fd, struct refclockio *rp, l_fp ts)
{
	int i;
	int buflen;
	register struct recvbuf *rb;

	rb = get_free_recv_buffer();

	if (NULL == rb) {
		/*
		 * No buffer space available - just drop the packet
		 */
		char buf[RX_BUFF_SIZE];

		buflen = read(fd, buf, sizeof buf);
		packets_dropped++;
		return (buflen);
	}

	i = (rp->datalen == 0
	     || rp->datalen > sizeof(rb->recv_space))
		? sizeof(rb->recv_space)
		: rp->datalen;
	buflen = read(fd, (char *)&rb->recv_space, (unsigned)i);

	if (buflen < 0) {
		if (errno != EINTR && errno != EAGAIN)
			msyslog(LOG_ERR, "clock read fd %d: %m", fd);
		freerecvbuf(rb);
		return (buflen);
	}

	/*
	 * Got one. Mark how and when it got here,
	 * put it on the full list and do bookkeeping.
	 */
	rb->recv_length = buflen;
	rb->recv_srcclock = rp->srcclock;
	rb->dstadr = 0;
	rb->fd = fd;
	rb->recv_time = ts;
	rb->receiver = rp->clock_recv;

	if (rp->io_input) {
		/*
		 * have direct input routine for refclocks
		 */
		if (rp->io_input(rb) == 0) {
			/*
			 * data was consumed - nothing to pass up
			 * into block input machine
			 */
			freerecvbuf(rb);
			return (buflen);
		}
	}

	add_full_recv_buffer(rb);

	rp->recvcount++;
	packets_received++;
	return (buflen);
}


/*
 * Routine to read the network NTP packets for a specific interface
 * Return the number of bytes read. That way we know if we should
 * read it again or go on to the next one if no bytes returned
 */
static inline int
read_network_packet(
	SOCKET			fd,
	struct interface *	itf,
	l_fp			ts
	)
{
	GETSOCKNAME_SOCKLEN_TYPE fromlen;
	int buflen;
	register struct recvbuf *rb;
#ifdef HAVE_TIMESTAMP
	struct msghdr msghdr;
	struct iovec iovec;
	char control[TIMESTAMP_CTLMSGBUF_SIZE];
#endif

	/*
	 * Get a buffer and read the frame.  If we
	 * haven't got a buffer, or this is received
	 * on a disallowed socket, just dump the
	 * packet.
	 */

	rb = get_free_recv_buffer();
	if (NULL == rb || itf->ignore_packets) {
		char buf[RX_BUFF_SIZE];
		sockaddr_u from;

		if (rb != NULL)
			freerecvbuf(rb);

		fromlen = sizeof(from);
		buflen = recvfrom(fd, buf, sizeof(buf), 0,
				  &from.sa, &fromlen);
		DPRINTF(4, ("%s on (%lu) fd=%d from %s\n",
			(itf->ignore_packets)
			    ? "ignore"
			    : "drop",
			free_recvbuffs(), fd, stoa(&from)));
		if (itf->ignore_packets)
			packets_ignored++;
		else
			packets_dropped++;
		return (buflen);
	}

	fromlen = sizeof(rb->recv_srcadr);

#ifndef HAVE_TIMESTAMP
	rb->recv_length = recvfrom(fd, (char *)&rb->recv_space,
				   sizeof(rb->recv_space), 0,
				   &rb->recv_srcadr.sa, &fromlen);
#else
	iovec.iov_base        = &rb->recv_space;
	iovec.iov_len         = sizeof(rb->recv_space);
	msghdr.msg_name       = &rb->recv_srcadr;
	msghdr.msg_namelen    = fromlen;
	msghdr.msg_iov        = &iovec;
	msghdr.msg_iovlen     = 1;
	msghdr.msg_control    = (void *)&control;
	msghdr.msg_controllen = sizeof(control);
	msghdr.msg_flags      = 0;
	rb->recv_length       = recvmsg(fd, &msghdr, 0);
#endif

	buflen = rb->recv_length;

	if (buflen == 0 || (buflen == -1 &&
	    (EWOULDBLOCK == errno
#ifdef EAGAIN
	     || EAGAIN == errno
#endif
	     ))) {
		freerecvbuf(rb);
		return (buflen);
	} else if (buflen < 0) {
		msyslog(LOG_ERR, "recvfrom(%s) fd=%d: %m",
			stoa(&rb->recv_srcadr), fd);
		DPRINTF(5, ("read_network_packet: fd=%d dropped (bad recvfrom)\n",
			    fd));
		freerecvbuf(rb);
		return (buflen);
	}

	DPRINTF(3, ("read_network_packet: fd=%d length %d from %s\n",
		    fd, buflen, stoa(&rb->recv_srcadr)));

	/*
	 * Got one.  Mark how and when it got here,
	 * put it on the full list and do bookkeeping.
	 */
	rb->dstadr = itf;
	rb->fd = fd;
#ifdef HAVE_TIMESTAMP
	/* pick up a network time stamp if possible */
	ts = fetch_timestamp(rb, &msghdr, ts);
#endif
	rb->recv_time = ts;
	rb->receiver = receive;

	add_full_recv_buffer(rb);

	itf->received++;
	packets_received++;
	return (buflen);
}


/*
 * On NT a SOCKET is an unsigned int so we cannot possibly keep it in
 * an array. So we use one of the ISC_LIST functions to hold the
 * socket value and use that when we want to enumerate it.
 *
 * This routine is called by the forked intres child process to close
 * all open sockets.  On Windows there's no need as intres runs in
 * the same process as a thread.
 */
#ifndef SYS_WINNT
void
kill_asyncio(int startfd)
{
	BLOCKIO();

	/*
	 * In the child process we do not maintain activefds and
	 * maxactivefd.  Zeroing maxactivefd disables code which
	 * maintains it in close_and_delete_fd_from_list().
	 */
	maxactivefd = 0;

	while (fd_list != NULL)
		close_and_delete_fd_from_list(fd_list->fd);

	UNBLOCKIO();
}
#endif	/* !SYS_WINNT */

#endif /* 0 */
