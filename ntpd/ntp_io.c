/*
 * ntp_io.c - input/output routines for ntpd.	The socket-opening code
 *		   was shamelessly stolen from ntpd.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "iosignal.h"
#include "ntp_refclock.h"
#include "ntp_if.h"
#include "ntp_stdlib.h"
#include "ntp.h"

/* Don't include ISC's version of IPv6 variables and structures */
#define ISC_IPV6_H 1
#include <isc/interfaceiter.h>
#include <isc/list.h>
#include <isc/result.h>

#ifdef SIM
#include "ntpsim.h"
#endif

#include <stdio.h>
#include <signal.h>
#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif /* HAVE_SYS_PARAM_H */
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN_SYSTM_H
# include <netinet/in_systm.h>
#else /* Some old linux systems at least have in_system.h instead. */
# ifdef HAVE_NETINET_IN_SYSTEM_H
#  include <netinet/in_system.h>
# endif
#endif /* HAVE_NETINET_IN_SYSTM_H */
#ifdef HAVE_NETINET_IP_H
# include <netinet/ip.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H	/* UXPV: SIOC* #defines (Frank Vance <fvance@waii.com>) */
# include <sys/sockio.h>
#endif
#include <arpa/inet.h>

extern int listen_to_virtual_ips;

#if defined(SYS_WINNT)
#include <transmitbuff.h>
#include <isc/win32os.h>
/*
 * Define this macro to control the behavior of connection
 * resets on UDP sockets.  See Microsoft KnowledgeBase Article Q263823
 * for details.
 * NOTE: This requires that Windows 2000 systems install Service Pack 2
 * or later.
 */
#ifndef SIO_UDP_CONNRESET 
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12) 
#endif

#endif

#undef UDP_WILDCARD_DELIVERY
/*
#define ISC_PLATFORM_HAVEIPV6
 struct ipv6_mreq {  struct in6_addr ipv6mr_multiaddr;
				  unsigned int ipv6mr_interface;
} ipv6_mreq;
*/

/*
 * We do asynchronous input using the SIGIO facility.  A number of
 * recvbuf buffers are preallocated for input.	In the signal
 * handler we poll to see which sockets are ready and read the
 * packets from them into the recvbuf's along with a time stamp and
 * an indication of the source host and the interface it was received
 * through.  This allows us to get as accurate receive time stamps
 * as possible independent of other processing going on.
 *
 * We watch the number of recvbufs available to the signal handler
 * and allocate more when this number drops below the low water
 * mark.  If the signal handler should run out of buffers in the
 * interim it will drop incoming frames, the idea being that it is
 * better to drop a packet than to be inaccurate.
 */


/*
 * Other statistics of possible interest
 */
volatile u_long packets_dropped;	/* total number of packets dropped on reception */
volatile u_long packets_ignored;	/* packets received on wild card interface */
volatile u_long packets_received;	/* total number of packets received */
u_long packets_sent;	/* total number of packets sent */
u_long packets_notsent; /* total number of packets which couldn't be sent */

volatile u_long handler_calls;	/* number of calls to interrupt handler */
volatile u_long handler_pkts;	/* number of pkts received by handler */
u_long io_timereset;		/* time counters were reset */

/*
 * Interface stuff
 */
struct interface *any_interface = NULL;	/* default ipv4 interface */
struct interface *any6_interface = NULL;/* default ipv6 interface */
struct interface *loopback_interface;	/* loopback ipv4 interface */
struct interface *loopback6_interface;	/* loopback ipv6 interface */
struct interface inter_list[MAXINTERFACES]; /* Interface list */
int ninterfaces;			/* Total number of interfaces */
int nwilds;				/* Total number of wildcard intefaces */
int wildipv4 = -1;			/* Index into inter_list for IPv4 wildcard */
int wildipv6 = -1;			/* Index into inter_list for IPv6 wildcard */

/*
 * These should be set if there is only one */
int outifaceipv4 = -1;		/* The only IPv4 outgoing interface */
int outifaceipv6 = -1;		/* The only IPv6 outgoing interface */

#ifdef REFCLOCK
/*
 * Refclock stuff.	We keep a chain of structures with data concerning
 * the guys we are doing I/O for.
 */
static	struct refclockio *refio;
#endif /* REFCLOCK */

/*
 * File descriptor masks etc. for call to select
 * Not needed for I/O Completion Ports
 */
fd_set activefds;
int maxactivefd;

static	int create_sockets	P((u_short));
static	SOCKET	open_socket	P((struct sockaddr_storage *, int, int, struct interface *, int));
static	void	close_socket	P((SOCKET));
#ifdef REFCLOCK
static	void	close_file	P((SOCKET));
#endif
static	char *	fdbits		P((int, fd_set *));
static	void	set_reuseaddr	P((int));
static	int find_interface_index P((struct sockaddr_storage *, int, int));
static	isc_boolean_t	socket_multicast_enable	 P((struct interface *, int, struct sockaddr_storage *));
static	isc_boolean_t	socket_multicast_disable P((struct interface *, int, struct sockaddr_storage *));
static	isc_boolean_t	socket_broadcast_enable	 P((struct interface *, int, struct sockaddr_storage *));
static	isc_boolean_t	socket_broadcast_disable P((struct interface *, int, struct sockaddr_storage *));


typedef struct vsock vsock_t;

struct vsock {
	SOCKET				fd;
	ISC_LINK(vsock_t)		link;
};

ISC_LIST(vsock_t)	sockets_list;

typedef struct remaddr remaddr_t;

struct remaddr {
      struct sockaddr_storage	addr;
      int			if_index;
      int			flags;
      ISC_LINK(remaddr_t)	link;
};

ISC_LIST(remaddr_t)       remoteaddr_list;

void	add_socket_to_list	P((SOCKET));
void	delete_socket_from_list	P((SOCKET));
int	find_socket_in_if	P((SOCKET ));
void	add_addr_to_list	P((struct sockaddr_storage *, int, int));
int     modify_addr_in_list	P((struct sockaddr_storage *, int));
void	delete_addr_from_list	P((struct sockaddr_storage *));
int     find_addr_in_list	P((struct sockaddr_storage *));
int     find_flagged_addr_in_list P((struct sockaddr_storage *, int));
int	create_wildcards	P((u_short));
isc_boolean_t address_okay	P((isc_interface_t *));
void	convert_isc_if		P((isc_interface_t *, struct interface *, u_short));

#ifdef SYS_WINNT
/*
 * Windows 2000 systems incorrectly cause UDP sockets using WASRecvFrom
 * to not work correctly, returning a WSACONNRESET error when a WSASendTo
 * fails with an "ICMP port unreachable" response and preventing the
 * socket from using the WSARecvFrom in subsequent operations.
 * The function below fixes this, but requires that Windows 2000
 * Service Pack 2 or later be installed on the system.  NT 4.0
 * systems are not affected by this and work correctly.
 * See Microsoft Knowledge Base Article Q263823 for details of this.
 */
isc_result_t
connection_reset_fix(SOCKET fd) {
	DWORD dwBytesReturned = 0;
	BOOL  bNewBehavior = FALSE;
	DWORD status;

	if(isc_win32os_majorversion() < 5)
		return (ISC_R_SUCCESS); /*  NT 4.0 has no problem */

	/* disable bad behavior using IOCTL: SIO_UDP_CONNRESET */
	status = WSAIoctl(fd, SIO_UDP_CONNRESET, &bNewBehavior,
			  sizeof(bNewBehavior), NULL, 0,
			  &dwBytesReturned, NULL, NULL);
	if (status != SOCKET_ERROR)
		return (ISC_R_SUCCESS);
	else
		return (ISC_R_UNEXPECTED);
}
#endif
/*
 * init_io - initialize I/O data structures and call socket creation routine
 */
void
init_io(void)
{
#ifdef SYS_WINNT
	if (!Win32InitSockets())
	{
		netsyslog(LOG_ERR, "No useable winsock.dll: %m");
		exit(1);
	}
	init_transmitbuff();
#endif /* SYS_WINNT */

	/*
	 * Init buffer free list and stat counters
	 */
	init_recvbuff(RECV_INIT);

	packets_dropped = packets_received = 0;
	packets_ignored = 0;
	packets_sent = packets_notsent = 0;
	handler_calls = handler_pkts = 0;
	io_timereset = 0;
	loopback_interface = NULL;
	loopback6_interface = NULL;

#ifdef REFCLOCK
	refio = 0;
#endif

#if defined(HAVE_SIGNALED_IO)
	(void) set_signal();
#endif

	ISC_LIST_INIT(sockets_list);

        ISC_LIST_INIT(remoteaddr_list);

	/*
	 * Create the sockets
	 */
	BLOCKIO();
	(void) create_sockets(htons(NTP_PORT));
	UNBLOCKIO();

#ifdef DEBUG
	if (debug)
	    printf("init_io: maxactivefd %d\n", maxactivefd);
#endif
}

#ifdef UDP_WILDCARD_DELIVERY
int
create_wildcards(u_short port) {

	int idx = 0;
	isc_boolean_t okipv4 = ISC_TRUE;
	/*
	 * create pseudo-interface with wildcard IPv4 address
	 */
#ifdef IPV6_V6ONLY
	if(isc_net_probeipv4() != ISC_R_SUCCESS)
		okipv4 = ISC_FALSE;
#endif

	if(okipv4 == ISC_TRUE) {
		inter_list[idx].family = AF_INET;
		inter_list[idx].sin.ss_family = AF_INET;
		((struct sockaddr_in*)&inter_list[idx].sin)->sin_addr.s_addr = htonl(INADDR_ANY);
		((struct sockaddr_in*)&inter_list[idx].sin)->sin_port = port;
		(void) strncpy(inter_list[idx].name, "wildcard", sizeof(inter_list[idx].name));
		inter_list[idx].mask.ss_family = AF_INET;
		((struct sockaddr_in*)&inter_list[idx].mask)->sin_addr.s_addr = htonl(~(u_int32)0);
		inter_list[idx].bfd = INVALID_SOCKET;
		inter_list[idx].num_mcast = 0;
		inter_list[idx].received = 0;
		inter_list[idx].sent = 0;
		inter_list[idx].notsent = 0;
		inter_list[idx].flags = INT_BROADCAST;
		any_interface = &inter_list[idx];
#if defined(MCAST)
	/*
	 * enable possible multicast reception on the broadcast socket
	 */
		inter_list[idx].bcast.ss_family = AF_INET;
		((struct sockaddr_in*)&inter_list[idx].bcast)->sin_port = port;
		((struct sockaddr_in*)&inter_list[idx].bcast)->sin_addr.s_addr = htonl(INADDR_ANY);
#endif /* MCAST */
		wildipv4 = idx;
		idx++;
	}

#ifdef ISC_PLATFORM_HAVEIPV6
	/*
	 * create pseudo-interface with wildcard IPv6 address
	 */
	if (isc_net_probeipv6() == ISC_R_SUCCESS) {
		inter_list[idx].family = AF_INET6;
		inter_list[idx].sin.ss_family = AF_INET6;
		((struct sockaddr_in6*)&inter_list[idx].sin)->sin6_addr = in6addr_any;
		((struct sockaddr_in6*)&inter_list[idx].sin)->sin6_port = port;
		(void) strncpy(inter_list[idx].name, "wildcard", sizeof(inter_list[idx].name));
		inter_list[idx].mask.ss_family = AF_INET6;
		memset(&((struct sockaddr_in6*)&inter_list[idx].mask)->sin6_addr.s6_addr, 0xff, sizeof(struct in6_addr));
		inter_list[idx].bfd = INVALID_SOCKET;
		inter_list[idx].num_mcast = 0;
		inter_list[idx].received = 0;
		inter_list[idx].sent = 0;
		inter_list[idx].notsent = 0;
		inter_list[idx].flags = 0;
		any6_interface = &inter_list[idx];
		wildipv6 = idx;
		idx++;
	}
#endif
	return (idx);
}
#endif /* UDP_WILDCARD_DELIVERY */

isc_boolean_t
address_okay(isc_interface_t *isc_if) {

#ifdef DEBUG
	if (debug > 2)
	    printf("address_okay: listen Virtual: %d, IF name: %s, Up Flag: %d\n", 
		    listen_to_virtual_ips, isc_if->name, (isc_if->flags & INTERFACE_F_UP));
#endif

	if (listen_to_virtual_ips == 0  && (strchr(isc_if->name, (int)':') != NULL))
		return (ISC_FALSE);

	/* XXXPDM This should be fixed later, but since we may not have set
	 * the UP flag, we at least get to use the interface.
	 * The UP flag is not always set so we don't do this right now.
	 */
/*	if ((isc_if->flags & INTERFACE_F_UP) == 0)
		return (ISC_FALSE);
*/
	return (ISC_TRUE);
}
void
convert_isc_if(isc_interface_t *isc_if, struct interface *itf, u_short port) {

	if(isc_if->af == AF_INET) {
		itf->family = AF_INET;
		itf->sin.ss_family = (u_short) isc_if->af;
		strcpy(itf->name, isc_if->name);
		memcpy(&(((struct sockaddr_in*)&itf->sin)->sin_addr),
		       &(isc_if->address.type.in),
		       sizeof(struct in_addr));
		((struct sockaddr_in*)&itf->sin)->sin_port = port;

		if((isc_if->flags & INTERFACE_F_BROADCAST) != 0) {
			itf->flags |= INT_BROADCAST;
			itf->bcast.ss_family = itf->sin.ss_family;
			memcpy(&(((struct sockaddr_in*)&itf->bcast)->sin_addr),
			       &(isc_if->broadcast.type.in),
				 sizeof(struct in_addr));
			((struct sockaddr_in*)&itf->bcast)->sin_port = port;
		}

		itf->mask.ss_family = itf->sin.ss_family;
		memcpy(&(((struct sockaddr_in*)&itf->mask)->sin_addr),
		       &(isc_if->netmask.type.in),
		       sizeof(struct in_addr));
		((struct sockaddr_in*)&itf->mask)->sin_port = port;

		if (((isc_if->flags & INTERFACE_F_LOOPBACK) != 0) && (loopback_interface == NULL))
		{
			loopback_interface = itf;
		}
	}
#ifdef ISC_PLATFORM_HAVEIPV6
	else if (isc_if->af == AF_INET6) {
		itf->family = AF_INET6;
		itf->sin.ss_family = (u_short) isc_if->af;
		strcpy(itf->name, isc_if->name);
		memcpy(&(((struct sockaddr_in6 *)&itf->sin)->sin6_addr),
		       &(isc_if->address.type.in6),
		       sizeof(struct in6_addr));
		((struct sockaddr_in6 *)&itf->sin)->sin6_port = port;
		((struct sockaddr_in6 *)&itf->sin)->sin6_scope_id = isc_if->scopeid;

		itf->mask.ss_family = itf->sin.ss_family;
		memcpy(&(((struct sockaddr_in6 *)&itf->mask)->sin6_addr),
		       &(isc_if->netmask.type.in6),
		       sizeof(struct in6_addr));
		((struct sockaddr_in6 *)&itf->mask)->sin6_port = port;

		if (((isc_if->flags & INTERFACE_F_LOOPBACK) != 0) && (loopback6_interface == NULL))
		{
			loopback6_interface = itf;
		}
	}
#endif /* ISC_PLATFORM_HAVEIPV6 */

		/* Process the rest of the flags */

	if((isc_if->flags & INTERFACE_F_UP) != 0)
		itf->flags |= INT_UP;
	if((isc_if->flags & INTERFACE_F_LOOPBACK) != 0)
		itf->flags |= INT_LOOPBACK;
	if((isc_if->flags & INTERFACE_F_POINTTOPOINT) != 0)
		itf->flags |= INT_PPP;
	if((isc_if->flags & INTERFACE_F_MULTICAST) != 0)
		itf->flags |= INT_MULTICAST;

	/* Copy the scopeid and the interface index */
	itf->ifindex = isc_if->ifindex;
	itf->scopeid = isc_if->scopeid;
}
/*
 * create_sockets - create a socket for each interface plus a default
 *			socket for when we don't know where to send
 */
static int
create_sockets(
	u_short port
	)
{
	struct sockaddr_storage resmask;
	int i;
	isc_boolean_t ofacesetipv4;
	isc_boolean_t ofacesetipv6;
	isc_mem_t *mctx = NULL;
	isc_interfaceiter_t *iter = NULL;
	isc_boolean_t scan_ipv4 = ISC_FALSE;
	isc_boolean_t scan_ipv6 = ISC_FALSE;
	isc_result_t result;
	int idx = 0;

#ifndef HAVE_IO_COMPLETION_PORT
	/*
	 * I/O Completion Ports don't care about the select and FD_SET
	 */
	maxactivefd = 0;
	FD_ZERO(&activefds);
#endif

#ifdef DEBUG
	if (debug)
	    printf("create_sockets(%d)\n", ntohs( (u_short) port));
#endif

	if (isc_net_probeipv6() == ISC_R_SUCCESS)
		scan_ipv6 = ISC_TRUE;
#if defined(ISC_PLATFORM_HAVEIPV6) && defined(DEBUG)
	else
		if(debug)
			netsyslog(LOG_ERR, "no IPv6 interfaces found");
#endif

	if (isc_net_probeipv4() == ISC_R_SUCCESS)
		scan_ipv4 = ISC_TRUE;
#ifdef DEBUG
	else
		if(debug)
			netsyslog(LOG_ERR, "no IPv4 interfaces found");
#endif
#ifdef UDP_WILDCARD_DELIVERY
	nwilds = create_wildcards(port);
	idx = nwilds;
#endif

	result = isc_interfaceiter_create(mctx, &iter);
	if (result != ISC_R_SUCCESS)
		return (result);

	for (result = isc_interfaceiter_first(iter);
	     result == ISC_R_SUCCESS;
	     result = isc_interfaceiter_next(iter))
	{
		isc_interface_t isc_if;
		unsigned int family; 

		result = isc_interfaceiter_current(iter, &isc_if);
		if (result != ISC_R_SUCCESS)
			break;

		/* See if we have a valid family to use */
		family = isc_if.address.family;
		if (family != AF_INET && family != AF_INET6)
			continue;
		if (scan_ipv4 == ISC_FALSE && family == AF_INET)
			continue;
		if (scan_ipv6 == ISC_FALSE && family == AF_INET6)
			continue;

		/* Check to see if we are going to use the interface */
		if (address_okay(&isc_if) == ISC_TRUE) {
			convert_isc_if(&isc_if, &inter_list[idx], port);
			inter_list[idx].fd = INVALID_SOCKET;
			inter_list[idx].bfd = INVALID_SOCKET;
			inter_list[idx].num_mcast = 0;
			inter_list[idx].received = 0;
			inter_list[idx].sent = 0;
			inter_list[idx].notsent = 0;
			idx++;
		}
	}
	isc_interfaceiter_destroy(&iter);

	ninterfaces = idx;
	/*
	 * Create the sockets
	 */
	for (i = 0; i < ninterfaces; i++) {
		inter_list[i].fd = open_socket(&inter_list[i].sin,
		    inter_list[i].flags & INT_BROADCAST, 0, &inter_list[i], i);
		if (inter_list[i].fd != INVALID_SOCKET)
			msyslog(LOG_INFO, "Listening on interface %s, %s#%d",
				inter_list[i].name,
				stoa((&inter_list[i].sin)),
				NTP_PORT);
		if ((inter_list[i].flags & INT_BROADCAST) &&
		     inter_list[i].bfd != INVALID_SOCKET)
			msyslog(LOG_INFO, "Listening on broadcast address %s#%d",
				stoa((&inter_list[i].bcast)),
				NTP_PORT);
	}

	/*
	 * Now that we have opened all the sockets, turn off the reuse
	 * flag for security.
	 */
	set_reuseaddr(0);

	/*
	 * Blacklist all bound interface addresses
	 * Wildcard interfaces, if any, are ignored.
	 */

	ofacesetipv4 = ISC_FALSE;
	ofacesetipv6 = ISC_FALSE;
	for (i = nwilds; i < ninterfaces; i++) {
		SET_HOSTMASK(&resmask, inter_list[i].sin.ss_family);
		hack_restrict(RESTRICT_FLAGS, &inter_list[i].sin, &resmask,
		    RESM_NTPONLY|RESM_INTERFACE, RES_IGNORE);

		/*
		 * We set the outgoing interface number ONLY if there is just one
		 */
		if (inter_list[i].family == AF_INET) {
			if((outifaceipv4 == -1) && !(inter_list[i].flags & INT_LOOPBACK) && 
				ofacesetipv4 == ISC_FALSE) {
				outifaceipv4 = i;
				ofacesetipv4 = ISC_TRUE;
			}
			else if ((outifaceipv4 != -1) && !(inter_list[i].flags & INT_LOOPBACK))
				outifaceipv4 = -1;
		}
		if (inter_list[i].family == AF_INET6) {
			if((outifaceipv6 == -1) && !(inter_list[i].flags & INT_LOOPBACK) && 
				ofacesetipv6 == ISC_FALSE) {
				outifaceipv6 = i;
				ofacesetipv6 = ISC_TRUE;
			}
			else if ((outifaceipv6 != -1) && !(inter_list[i].flags & INT_LOOPBACK))
				outifaceipv6 = -1;
		}
	}

	/*
	 * Calculate the address hash for each interface address.
	 */
	for (i = 0; i < ninterfaces; i++) {
		inter_list[i].addr_refid = addr2refid(&inter_list[i].sin);
	}


#ifdef DEBUG
	if (debug > 1) {
		printf("create_sockets: Total interfaces = %d\n", ninterfaces);
		for (i = 0; i < ninterfaces; i++) {
			printf("interface %d:  fd=%d,  bfd=%d,  name=%.8s,  flags=0x%x,  scope=%d\n",
			       i,
			       inter_list[i].fd,
			       inter_list[i].bfd,
			       inter_list[i].name,
			       inter_list[i].flags,
			       inter_list[i].scopeid);
			/* Leave these as three printf calls. */
			printf("              sin=%s",
			       stoa((&inter_list[i].sin)));
			if (inter_list[i].flags & INT_BROADCAST)
			    printf("  bcast=%s,",
				   stoa((&inter_list[i].bcast)));
			printf("  mask=%s\n",
			       stoa((&inter_list[i].mask)));
		}
	}
#endif
	return ninterfaces;
}


/*
 * set_reuseaddr() - set/clear REUSEADDR on all sockets
 *			NB possible hole - should we be doing this on broadcast
 *			fd's also?
 */
static void
set_reuseaddr(int flag) {
	int i;

	for (i=0; i < ninterfaces; i++) {
		/*
		 * if inter_list[ n ].fd  is -1, we might have a adapter
		 * configured but not present
		 */
		if (inter_list[i].fd != INVALID_SOCKET) {
			if (setsockopt(inter_list[i].fd, SOL_SOCKET,
					SO_REUSEADDR, (char *)&flag,
					sizeof(flag))) {
				netsyslog(LOG_ERR, "set_reuseaddr: setsockopt(SO_REUSEADDR, %s) failed: %m", flag ? "on" : "off");
			}
		}
	}
}

#ifdef OPEN_BCAST_SOCKET 
/*
 * Enable a broadcast address to a given socket
 * The socket is in the inter_list all we need to do is enable
 * broadcasting. It is not this function's job to select the socket
 */
static isc_boolean_t
socket_broadcast_enable(struct interface *iface, int ind, struct sockaddr_storage *maddr)
{
#ifdef SO_BROADCAST
	int on = 1;

	if (maddr->ss_family == AF_INET)
	{
		/* if this interface can support broadcast, set SO_BROADCAST */
		if (setsockopt(iface->fd, SOL_SOCKET, SO_BROADCAST,
			       (char *)&on, sizeof(on)))
		{
			netsyslog(LOG_ERR, "setsockopt(SO_BROADCAST) enable failure on address %s: %m",
				stoa(maddr));
		}
	}
	iface->flags |= INT_BCASTOPEN;
	modify_addr_in_list(maddr, iface->flags);
	return ISC_TRUE;
#else
	return ISC_FALSE;
#endif /* SO_BROADCAST */
}

/*
 * Remove a broadcast address from a given socket
 * The socket is in the inter_list all we need to do is disable
 * broadcasting. It is not this function's job to select the socket
 */
static isc_boolean_t
socket_broadcast_disable(struct interface *iface, int ind, struct sockaddr_storage *maddr)
{
#ifdef SO_BROADCAST
	int off = 0;

	if (maddr->ss_family == AF_INET)
	{
		if (setsockopt(iface->fd, SOL_SOCKET, SO_BROADCAST,
			       (char *)&off, sizeof(off)))
		{
			netsyslog(LOG_ERR, "setsockopt(SO_BROADCAST) disable failure on address %s: %m",
				stoa(maddr));
		}
	}
	iface->flags &= ~INT_BCASTOPEN;
	modify_addr_in_list(maddr, iface->flags);
	return ISC_TRUE;
#else
	return ISC_FALSE;
#endif /* SO_BROADCAST */
}

#endif /* OPEN_BCAST_SOCKET */
/*
 * NOTE: Not all platforms support multicast
 */
#ifdef MCAST
/*
 * Add a multicast address to a given socket
 * The socket is in the inter_list all we need to do is enable
 * multicasting. It is not this function's job to select the socket
 */
static isc_boolean_t
socket_multicast_enable(struct interface *iface, int ind, struct sockaddr_storage *maddr)
{
#ifdef ISC_PLATFORM_HAVEIPV6
	struct ipv6_mreq mreq6;
	struct in6_addr iaddr6;
#endif /* ISC_PLATFORM_HAVEIPV6 */
	struct ip_mreq mreq;
	memset((char *)&mreq, 0, sizeof(mreq));

	switch (maddr->ss_family)
	{
	case AF_INET:
		mreq.imr_multiaddr = (((struct sockaddr_in*)maddr)->sin_addr);
		mreq.imr_interface.s_addr = ((struct sockaddr_in*)&iface->sin)->sin_addr.s_addr;
/*		mreq.imr_interface.s_addr = htonl(INADDR_ANY); */
		if (setsockopt(iface->fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			(char *)&mreq, sizeof(mreq)) == -1) {
			netsyslog(LOG_ERR,
			"setsockopt IP_ADD_MEMBERSHIP failure: %m for %x / %x (%s)",
			mreq.imr_multiaddr.s_addr,
			mreq.imr_interface.s_addr, stoa(maddr));
			return ISC_FALSE;
		}
		break;
	case AF_INET6:
#if defined(ISC_PLATFORM_HAVEIPV6) && defined(IPV6_JOIN_GROUP) && defined(IPV6_LEAVE_GROUP)

		/*
		 * Enable reception of multicast packets
		 * If the address is link-local we can get the interface index
		 * from the scope id. Don't do this for other types of multicast
		 * addresses. For now let the kernel figure it out.
		 */
		iaddr6 = ((struct sockaddr_in6*)maddr)->sin6_addr;
		mreq6.ipv6mr_multiaddr = iaddr6;
#ifdef HAVE_STRUCT_SOCKADDR_IN6_SIN6_SCOPE_ID
		if (IN6_IS_ADDR_LINKLOCAL(&iaddr6))
			mreq6.ipv6mr_interface = ((struct sockaddr_in6*)maddr)->sin6_scope_id;
		else
#endif
			mreq6.ipv6mr_interface = 0;

		if (setsockopt(iface->fd, IPPROTO_IPV6, IPV6_JOIN_GROUP,
			(char *)&mreq6, sizeof(mreq6)) == -1) {
			netsyslog(LOG_ERR,
			 "setsockopt IPV6_JOIN_GROUP failure: %m on interface %d(%s)",
			 ind, stoa(maddr));
			return ISC_FALSE;
		}
		break;
#else
		return ISC_FALSE;
#endif	/* ISC_PLATFORM_HAVEIPV6 */
	}
	iface->flags |= INT_MCASTOPEN;
	iface->num_mcast++;
	add_addr_to_list(maddr, ind, iface->flags);
	return ISC_TRUE;
}

/*
 * Remove a multicast address from a given socket
 * The socket is in the inter_list all we need to do is disable
 * multicasting. It is not this function's job to select the socket
 */
static isc_boolean_t
socket_multicast_disable(struct interface *iface, int ind, struct sockaddr_storage *maddr)
{
#ifdef ISC_PLATFORM_HAVEIPV6
	struct ipv6_mreq mreq6;
	struct in6_addr iaddr6;
#endif /* ISC_PLATFORM_HAVEIPV6 */
	struct ip_mreq mreq;
	memset((char *)&mreq, 0, sizeof(mreq));

	switch (maddr->ss_family)
	{
	case AF_INET:
		mreq.imr_multiaddr = (((struct sockaddr_in*)&maddr)->sin_addr);
		mreq.imr_interface.s_addr = ((struct sockaddr_in*)&iface->sin)->sin_addr.s_addr;
		if (setsockopt(iface->fd, IPPROTO_IP, IP_DROP_MEMBERSHIP,
			(char *)&mreq, sizeof(mreq)) == -1) {
			netsyslog(LOG_ERR,
			"setsockopt IP_DROP_MEMBERSHIP failure: %m for %x / %x (%s)",
			mreq.imr_multiaddr.s_addr,
			mreq.imr_interface.s_addr, stoa(maddr));
			return ISC_FALSE;
		}
		break;
	case AF_INET6:
#if defined(ISC_PLATFORM_HAVEIPV6) && defined(IPV6_JOIN_GROUP) && defined(IPV6_LEAVE_GROUP)

		/*
		 * Disable reception of multicast packets
		 * If the address is link-local we can get the interface index
		 * from the scope id. Don't do this for other types of multicast
		 * addresses. For now let the kernel figure it out.
		 */
		iaddr6 = ((struct sockaddr_in6*)&maddr)->sin6_addr;
		mreq6.ipv6mr_multiaddr = iaddr6;
#ifdef HAVE_STRUCT_SOCKADDR_IN6_SIN6_SCOPE_ID
		if (IN6_IS_ADDR_LINKLOCAL(&iaddr6))
			mreq6.ipv6mr_interface = ((struct sockaddr_in6*)maddr)->sin6_scope_id;
		else
#endif
			mreq6.ipv6mr_interface = 0;

		if (setsockopt(iface->fd, IPPROTO_IPV6, IPV6_LEAVE_GROUP,
			(char *)&mreq6, sizeof(mreq6)) == -1) {
			netsyslog(LOG_ERR,
			 "setsockopt IPV6_LEAVE_GROUP failure: %m on interface %d(%s)",
			 mreq6.ipv6mr_interface, stoa(maddr));
			return ISC_FALSE;
		}
		break;
#else
		return ISC_FALSE;
#endif	/* ISC_PLATFORM_HAVEIPV6 */
	}
	iface->num_mcast--;
	if (iface->num_mcast <= 0) {
		iface->flags &= ~INT_MCASTOPEN;
		modify_addr_in_list(maddr, iface->flags);
	}
	return ISC_TRUE;
}
#endif	/* MCAST */

/*
 * io_setbclient - open the broadcast client sockets
 */
void
io_setbclient(void)
{
#ifdef OPEN_BCAST_SOCKET 
	int i;
	int nif = 0;
	isc_boolean_t jstatus; 

	set_reuseaddr(1);

	for (i = nwilds; i < ninterfaces; i++) {
		/* Only IPv4 addresses are valid for broadcast */
		if (inter_list[i].family != AF_INET)
			continue;

		/* Is this a broadcast address? */
		if (!(inter_list[i].flags & INT_BROADCAST))
			continue;

		/* Skip the loopback addresses */
		if (inter_list[i].flags & INT_LOOPBACK)
			continue;

		/* Do we already have the broadcast address open? */
		if (inter_list[i].flags & INT_BCASTOPEN)
			continue;

		/* Enable Broadcast on socket */
		jstatus = socket_broadcast_enable(&inter_list[i], i, &inter_list[i].sin);
		if (jstatus == ISC_TRUE)
			nif++;
#ifdef DEBUG
		if (debug) {
			if (jstatus == ISC_TRUE)
				printf("io_setbclient: Opened broadcast client on interface %d, socket: %d\n",
				i, inter_list[i].fd);
			else
				printf("io_setbclient: Unable to Open broadcast client on interface %d\n",
				i);
		}
#endif
	}
	set_reuseaddr(0);
#ifdef DEBUG
	if (debug)
		if (nif > 0)
			printf("io_setbclient: Opened broadcast clients\n");
#endif
		if (nif == 0)
			netsyslog(LOG_ERR, "Unable to listen for broadcasts, no broadcast interfaces available");
#else
	netsyslog(LOG_ERR, "io_setbclient: Broadcast Client disabled by build");
#endif
}

/*
 * io_unsetbclient - close the broadcast client sockets
 */
void
io_unsetbclient(void)
{
	int i;
	isc_boolean_t lstatus;

	for (i = nwilds; i < ninterfaces; i++)
	{
		if (!(inter_list[i].flags & INT_BCASTOPEN))
		    continue;
		lstatus = socket_broadcast_disable(&inter_list[i], i, &inter_list[i].sin);
	}
}

/*
 * io_multicast_add() - add multicast group address
 */
void
io_multicast_add(
	struct sockaddr_storage addr
	)
{
#ifdef MCAST
	u_int32 haddr = ntohl(((struct sockaddr_in*)&addr)->sin_addr.s_addr);
	struct in_addr iaddr;
	isc_boolean_t jstatus;
	int ind;

#ifdef ISC_PLATFORM_HAVEIPV6
	struct in6_addr iaddr6;
#endif /* ISC_PLATFORM_HAVEIPV6 */

	/* If we already have it we can just return */
	if (find_flagged_addr_in_list(&addr, INT_MCASTOPEN) >= 0)
	{
		netsyslog(LOG_INFO, "Duplicate request found for multicast address %s",
			stoa(&addr));
		return;
	}

	switch (addr.ss_family)
	{
	case AF_INET :
		iaddr = (((struct sockaddr_in*)&addr)->sin_addr);
		if (!IN_CLASSD(haddr)) {
			netsyslog(LOG_ERR,
			"multicast address %s not class D",
				inet_ntoa(iaddr));
#ifdef DEBUG
			if (debug >= 4)
				printf("haddr value = %x\n", haddr);
#endif
			return;
		}
		break;

#if defined(ISC_PLATFORM_HAVEIPV6) && defined(IPV6_JOIN_GROUP) && defined(IPV6_LEAVE_GROUP)
	case AF_INET6 :

		iaddr6 = ((struct sockaddr_in6*)&addr)->sin6_addr;
		if (!IN6_IS_ADDR_MULTICAST(&iaddr6)) {
			netsyslog(LOG_ERR,
			    "address %s not IPv6 multicast address",
				stoa(&addr));
			return;
		}
		break;
#endif /* ISC_PLATFORM_HAVEIPV6 */
	}

	ind = find_interface_index(&addr, INT_MULTICAST, nwilds);
	if (ind < 0) {
		netsyslog(LOG_ERR,
		"No wildcard socket available to use for address %s",
		stoa(&addr));
		return;
	}
	jstatus = socket_multicast_enable(&inter_list[ind], ind, &addr);

#ifdef DEBUG
	if (debug)
		printf("io_multicast_add %s\n", stoa(&addr));
#endif
#else /* MCAST */
	netsyslog(LOG_ERR,
	    "cannot add multicast address %s: no Multicast support",
	    stoa(&addr));
#endif /* MCAST */
}

/*
 * io_multicast_del() - delete multicast group address
 */
void
io_multicast_del(
	struct sockaddr_storage addr
	)
{
#ifdef MCAST
	int i;
	u_int32 haddr;
	isc_boolean_t lstatus;

#ifdef ISC_PLATFORM_HAVEIPV6
	struct in6_addr haddr6;
#endif /* ISC_PLATFORM_HAVEIPV6 */

	switch (addr.ss_family)
	{
	case AF_INET :

		haddr = ntohl(((struct sockaddr_in*)&addr)->sin_addr.s_addr);

		if (!IN_CLASSD(haddr))
		{
			netsyslog(LOG_ERR,
				 "invalid multicast address %s", stoa(&addr));
			return;
		}

		/*
		 * Disable reception of multicast packets
		 */
		i = find_flagged_addr_in_list(&addr, INT_MCASTOPEN);
		while ( i > 0) {
			lstatus = socket_multicast_disable(&inter_list[i], i, &addr);
			i = find_flagged_addr_in_list(&addr, INT_MCASTOPEN);
		}
		break;

#if defined(ISC_PLATFORM_HAVEIPV6) && defined(IPV6_JOIN_GROUP) && defined(IPV6_LEAVE_GROUP)
	case AF_INET6 :
		haddr6 = ((struct sockaddr_in6*)&addr)->sin6_addr;

		if (!IN6_IS_ADDR_MULTICAST(&haddr6))
		{
			netsyslog(LOG_ERR,
				"invalid multicast address %s", stoa(&addr));
			return;
		}

		/*
		* Disable reception of multicast packets
		*/
		for (i = 0; i < ninterfaces; i++)
		{
			/* Be sure it's the correct family */
			if (inter_list[i].family != AF_INET6)
				continue;
			if (!(inter_list[i].flags & INT_MCASTOPEN))
				continue;
			if (!(inter_list[i].fd < 0))
				continue;
			if (!SOCKCMP(&addr, &inter_list[i].sin))
				continue;
			lstatus = socket_multicast_disable(&inter_list[i], i, &addr);
		}
		break;
#endif /* ISC_PLATFORM_HAVEIPV6 */
	}/* switch */
        delete_addr_from_list(&addr);

#else /* not MCAST */
	netsyslog(LOG_ERR, "this function requires multicast kernel");
#endif /* not MCAST */
}


/*
 * open_socket - open a socket, returning the file descriptor
 */

static SOCKET
open_socket(
	struct sockaddr_storage *addr,
	int flags,
	int turn_off_reuse,
	struct interface *interf,
	int ind
	)
{
	int errval;
	SOCKET fd;
	int on = 1, off = 0;
#if defined(IPTOS_LOWDELAY) && defined(IPPROTO_IP) && defined(IP_TOS)
	int tos;
#endif /* IPTOS_LOWDELAY && IPPROTO_IP && IP_TOS */

	if ((addr->ss_family == AF_INET6) && (isc_net_probeipv6() != ISC_R_SUCCESS))
		return (INVALID_SOCKET);

	/* create a datagram (UDP) socket */
#ifndef SYS_WINNT
	if (  (fd = socket(addr->ss_family, SOCK_DGRAM, 0)) < 0) {
		errval = errno;
#else
	if (  (fd = socket(addr->ss_family, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		errval = WSAGetLastError();
#endif
		if(addr->ss_family == AF_INET)
			netsyslog(LOG_ERR, "socket(AF_INET, SOCK_DGRAM, 0) failed on address %s: %m",
				stoa(addr));
		else if(addr->ss_family == AF_INET6)
			netsyslog(LOG_ERR, "socket(AF_INET6, SOCK_DGRAM, 0) failed on address %s: %m",
				stoa(addr));
#ifndef SYS_WINNT
		if (errval == EPROTONOSUPPORT || errval == EAFNOSUPPORT ||
		    errval == EPFNOSUPPORT)
#else
		if (errval == WSAEPROTONOSUPPORT || errval == WSAEAFNOSUPPORT ||
		    errval == WSAEPFNOSUPPORT)
#endif
			return (INVALID_SOCKET);
		exit(1);
		/*NOTREACHED*/
	}
#ifdef SYS_WINNT
	if (connection_reset_fix(fd) != ISC_R_SUCCESS) {
		netsyslog(LOG_ERR, "connection_reset_fix(fd) failed on address %s: %m",
			stoa(addr));
	}
#endif /* SYS_WINNT */

	/* set SO_REUSEADDR since we will be binding the same port
	   number on each interface */
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
		       (char *)&on, sizeof(on)))
	{
		netsyslog(LOG_ERR, "setsockopt SO_REUSEADDR on fails on address %s: %m",
			stoa(addr));
	}

	/*
	 * IPv4 specific options go here
	 */
	if (addr->ss_family == AF_INET) {
#if defined(IPTOS_LOWDELAY) && defined(IPPROTO_IP) && defined(IP_TOS)
	/* set IP_TOS to minimize packet delay */
		tos = IPTOS_LOWDELAY;
		if (setsockopt(fd, IPPROTO_IP, IP_TOS, (char *) &tos, sizeof(tos)) < 0)
		{
			netsyslog(LOG_ERR, "setsockopt IPTOS_LOWDELAY on fails on address %s: %m",
				stoa(addr));
		}
#endif /* IPTOS_LOWDELAY && IPPROTO_IP && IP_TOS */
	}

	/*
	 * IPv6 specific options go here
	 */
        if (addr->ss_family == AF_INET6) {
#if defined(IPV6_V6ONLY)
                if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY,
                	(char*)&on, sizeof(on)))
                {
                	netsyslog(LOG_ERR, "setsockopt IPV6_V6ONLY on fails on address %s: %m",
				stoa(addr));
		}
#endif /* IPV6_V6ONLY */
#if defined(IPV6_BINDV6ONLY)
                if (setsockopt(fd, IPPROTO_IPV6, IPV6_BINDV6ONLY,
                	(char*)&on, sizeof(on)))
                {
                	netsyslog(LOG_ERR,
			    "setsockopt IPV6_BINDV6ONLY on fails on address %s: %m",
			    stoa(addr));
		}
#endif /* IPV6_BINDV6ONLY */
	}

	/*
	 * bind the local address.
	 */
	if (bind(fd, (struct sockaddr *)addr, SOCKLEN(addr)) < 0) {
		char buff[160];

		if(addr->ss_family == AF_INET)
			sprintf(buff,
				"bind() fd %d, family %d, port %d, addr %s, in_classd=%d flags=%d fails: %%m",
				fd, addr->ss_family, (int)ntohs(((struct sockaddr_in*)addr)->sin_port),
				stoa(addr),
				IN_CLASSD(ntohl(((struct sockaddr_in*)addr)->sin_addr.s_addr)), flags);
		else if(addr->ss_family == AF_INET6)
		                sprintf(buff,
                                "bind() fd %d, family %d, port %d, addr %s, in6_is_addr_multicast=%d flags=%d fails: %%m",
                                fd, addr->ss_family, (int)ntohs(((struct sockaddr_in6*)addr)->sin6_port),
                                stoa(addr),
                                IN6_IS_ADDR_MULTICAST(&((struct sockaddr_in6*)addr)->sin6_addr), flags);
		else return INVALID_SOCKET;

		netsyslog(LOG_ERR, buff);
		closesocket(fd);

		/*
		 * soft fail if opening a multicast address
		 */
 		if(addr->ss_family == AF_INET){
			if(IN_CLASSD(ntohl(((struct sockaddr_in*)addr)->sin_addr.s_addr)))
				return (INVALID_SOCKET);
		}
		else {
			if(IN6_IS_ADDR_MULTICAST(&((struct sockaddr_in6*)addr)->sin6_addr))
				return (INVALID_SOCKET);
		}
#if 0
		exit(1);
#else
		return INVALID_SOCKET;
#endif
	}
#ifdef DEBUG
	if (debug)
	    printf("bind() fd %d, family %d, port %d, addr %s, flags=%d\n",
		   fd,
		   addr->ss_family,
		   (int)ntohs(((struct sockaddr_in*)addr)->sin_port),
		   stoa(addr),
		   flags);
#endif

	/*
	 * I/O Completion Ports don't care about the select and FD_SET
	 */
#ifndef HAVE_IO_COMPLETION_PORT
	if (fd > maxactivefd)
	    maxactivefd = fd;
	FD_SET(fd, &activefds);
#endif
	add_socket_to_list(fd);
	add_addr_to_list(addr, ind, flags);
	/*
	 * set non-blocking,
	 */

#ifdef USE_FIONBIO
	/* in vxWorks we use FIONBIO, but the others are defined for old systems, so
	 * all hell breaks loose if we leave them defined
	 */
#undef O_NONBLOCK
#undef FNDELAY
#undef O_NDELAY
#endif

#if defined(O_NONBLOCK) /* POSIX */
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		netsyslog(LOG_ERR, "fcntl(O_NONBLOCK) fails on address %s: %m",
			stoa(addr));
		exit(1);
		/*NOTREACHED*/
	}
#elif defined(FNDELAY)
	if (fcntl(fd, F_SETFL, FNDELAY) < 0)
	{
		netsyslog(LOG_ERR, "fcntl(FNDELAY) fails on address %s: %m",
			stoa(addr));
		exit(1);
		/*NOTREACHED*/
	}
#elif defined(O_NDELAY) /* generally the same as FNDELAY */
	if (fcntl(fd, F_SETFL, O_NDELAY) < 0)
	{
		netsyslog(LOG_ERR, "fcntl(O_NDELAY) fails on address %s: %m",
			stoa(addr));
		exit(1);
		/*NOTREACHED*/
	}
#elif defined(FIONBIO)
# if defined(SYS_WINNT)
		if (ioctlsocket(fd,FIONBIO,(u_long *) &on) == SOCKET_ERROR)
# else
		if (ioctl(fd,FIONBIO,&on) < 0)
# endif
	{
		netsyslog(LOG_ERR, "ioctl(FIONBIO) fails on address %s: %m",
			stoa(addr));
		exit(1);
		/*NOTREACHED*/
	}
#elif defined(FIOSNBIO)
	if (ioctl(fd,FIOSNBIO,&on) < 0)
	{
		netsyslog(LOG_ERR, "ioctl(FIOSNBIO) fails on address %s: %m",
			stoa(addr));
		exit(1);
		/*NOTREACHED*/
	}
#else
# include "Bletch: Need non-blocking I/O!"
#endif

#ifdef HAVE_SIGNALED_IO
	init_socket_sig(fd);
#endif /* not HAVE_SIGNALED_IO */

	/*
	 *	Turn off the SO_REUSEADDR socket option.  It apparently
	 *	causes heartburn on systems with multicast IP installed.
	 *	On normal systems it only gets looked at when the address
	 *	is being bound anyway..
	 */
	if (turn_off_reuse)
	    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
			   (char *)&off, sizeof(off)))
	    {
		    netsyslog(LOG_ERR, "setsockopt SO_REUSEADDR off fails on address %s: %m",
			    stoa(addr));
	    }

#if !defined(SYS_WINNT) && !defined(VMS)
# ifdef DEBUG
	if (debug > 1)
	    printf("flags for fd %d: 0%o\n", fd,
		   fcntl(fd, F_GETFL, 0));
# endif
#endif /* SYS_WINNT || VMS */


#if defined (HAVE_IO_COMPLETION_PORT)
/*
 * Add the socket to the completion port
 */
	io_completion_port_add_socket(fd, interf);
#endif
	return fd;
}


/*
 * close_socket - close a socket and remove from the activefd list
 */
static void
close_socket(
	     SOCKET fd
	)
{
	int ind;
	SOCKET i, newmax;

	(void) closesocket(fd);

	/*
	 * I/O Completion Ports don't care about select and fd_set
	 */
#ifndef HAVE_IO_COMPLETION_PORT
	FD_CLR( (u_int) fd, &activefds);

	if (fd == maxactivefd) {
		newmax = 0;
		for (i = 0; i < maxactivefd; i++)
			if (FD_ISSET(i, &activefds))
				newmax = i;
		maxactivefd = newmax;
	}
#endif
	/*
	 * Remove the socket from the socket list
	 * and remove the address from the address list
	 */
	ind = find_socket_in_if(fd);
	delete_socket_from_list(fd);
	if (ind >= 0)
		delete_addr_from_list(&inter_list[ind].sin);
}


/*
 * close_file - close a file and remove from the activefd list
 * added 1/31/1997 Greg Schueman for Windows NT portability
 */
#ifdef REFCLOCK
static void
close_file(
	SOCKET fd
	)
{
	int i, newmax;

	(void) close(fd);

#ifndef HAVE_IO_COMPLETION_PORT
	/*
	 * I/O Completion Ports don't care about select and fd_set
	 */
	FD_CLR( (u_int) fd, &activefds);

	if (fd == maxactivefd) {
		newmax = 0;
		for (i = 0; i < maxactivefd; i++)
			if (FD_ISSET(i, &activefds))
				newmax = i;
		maxactivefd = newmax;
	}
#endif
	delete_socket_from_list(fd);
}
#endif


/* XXX ELIMINATE sendpkt similar in ntpq.c, ntpdc.c, ntp_io.c, ntptrace.c */
/*
 * sendpkt - send a packet to the specified destination. Maintain a
 * send error cache so that only the first consecutive error for a
 * destination is logged.
 */
void
sendpkt(
	struct sockaddr_storage *dest,
	struct interface *inter,
	int ttl,
	struct pkt *pkt,
	int len
	)
{
	int cc, slot;
#ifdef SYS_WINNT
	DWORD err;
#endif /* SYS_WINNT */

	/*
	 * Send error caches. Empty slots have port == 0
	 * Set ERRORCACHESIZE to 0 to disable
	 */
	struct cache {
		u_short port;
		struct	in_addr addr;
	};

#ifdef ISC_PLATFORM_HAVEIPV6
	struct cache6 {
		u_short port;
		struct in6_addr addr;
	};
#endif /* ISC_PLATFORM_HAVEIPV6 */

#ifndef ERRORCACHESIZE
#define ERRORCACHESIZE 8
#endif
#if ERRORCACHESIZE > 0
	static struct cache badaddrs[ERRORCACHESIZE];
#ifdef ISC_PLATFORM_HAVEIPV6
	static struct cache6 badaddrs6[ERRORCACHESIZE];
#endif /* ISC_PLATFORM_HAVEIPV6 */
#else
#define badaddrs ((struct cache *)0)		/* Only used in empty loops! */
#ifdef ISC_PLATFORM_HAVEIPV6
#define badaddrs6 ((struct cache6 *)0)		/* Only used in empty loops! */
#endif /* ISC_PLATFORM_HAVEIPV6 */
#endif
#ifdef DEBUG
	if (debug > 1)
	    printf("%ssendpkt(fd=%d dst=%s, src=%s, ttl=%d, len=%d)\n",
		   (ttl >= 0) ? "\tMCAST\t*****" : "",
		   inter->fd, stoa(dest),
		   stoa(&inter->sin), ttl, len);
#endif

#ifdef MCAST

	switch (inter->family) {

	case AF_INET :

		/*
		* for the moment we use the bcast option to set multicast ttl
		*/
		if (ttl > 0 && ttl != inter->last_ttl) {

			/*
			* set the multicast ttl for outgoing packets
			*/
			u_char mttl = (u_char) ttl;
			if (setsockopt(inter->fd, IPPROTO_IP, IP_MULTICAST_TTL,
				(const void *) &mttl, sizeof(mttl)) != 0) {
				netsyslog(LOG_ERR, "setsockopt IP_MULTICAST_TTL fails on address %s: %m",
					stoa(&inter->sin));
			}
			else
   				inter->last_ttl = ttl;
		}
		break;

#ifdef ISC_PLATFORM_HAVEIPV6
	case AF_INET6 :

	 	/*
		 * for the moment we use the bcast option to set
		 * multicast max hops
		 */
        	if (ttl > 0 && ttl != inter->last_ttl) {

                	/*
                 	* set the multicast ttl for outgoing packets
                 	*/
			u_int ittl = (u_int) ttl;
                	if (setsockopt(inter->fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
                    	(const void *) &ittl, sizeof(ittl)) == -1)
	                        netsyslog(LOG_ERR, "setsockopt IP_MULTICAST_TTL fails on address %s: %m",
					stoa(&inter->sin));
                	else
	                        inter->last_ttl = ttl;
	        }
	        break;
#endif /* ISC_PLATFORM_HAVEIPV6 */

	default :
		exit(1);

	}


#endif /* MCAST */

	for (slot = ERRORCACHESIZE; --slot >= 0; )
		if(dest->ss_family == AF_INET) {
			if (badaddrs[slot].port == ((struct sockaddr_in*)dest)->sin_port &&
				badaddrs[slot].addr.s_addr == ((struct sockaddr_in*)dest)->sin_addr.s_addr)
			break;
		}
#ifdef ISC_PLATFORM_HAVEIPV6
		else if (dest->ss_family == AF_INET6) {
			if (badaddrs6[slot].port == ((struct sockaddr_in6*)dest)->sin6_port &&
				badaddrs6[slot].addr.s6_addr == ((struct sockaddr_in6*)dest)->sin6_addr.s6_addr)
			break;
		}
#endif /* ISC_PLATFORM_HAVEIPV6 */
		else exit(1);  /* address family not supported yet */

#if defined(HAVE_IO_COMPLETION_PORT)
        err = io_completion_port_sendto(inter, pkt, len, dest);
	if (err != ERROR_SUCCESS)
#else
#ifdef SIM
        cc = srvr_rply(&ntp_node,  dest, inter, pkt);
#else /* SIM */
	cc = sendto(inter->fd, (char *)pkt, (unsigned int)len, 0, (struct sockaddr *)dest,
		    SOCKLEN(dest));
#endif /* SIM */
	if (cc == -1)
#endif
	{
		inter->notsent++;
		packets_notsent++;
#if defined(HAVE_IO_COMPLETION_PORT)
		err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK && err != WSAENOBUFS && slot < 0)
#else
		if (errno != EWOULDBLOCK && errno != ENOBUFS && slot < 0)
#endif
		{
			/*
			 * Remember this, if there's an empty slot
			 */
			switch (dest->ss_family) {

			case AF_INET :

				for (slot = ERRORCACHESIZE; --slot >= 0; )
					if (badaddrs[slot].port == 0)
					{
						badaddrs[slot].port = SRCPORT(dest);
						badaddrs[slot].addr = ((struct sockaddr_in*)dest)->sin_addr;
						break;
					}
				break;

#ifdef ISC_PLATFORM_HAVEIPV6
			case AF_INET6 :

				for (slot = ERRORCACHESIZE; --slot >= 0; )
        				if (badaddrs6[slot].port == 0)
            				{
                                    		badaddrs6[slot].port = SRCPORT(dest);
                                    		badaddrs6[slot].addr = ((struct sockaddr_in6*)dest)->sin6_addr;
                                    		break;
                            		}
                		break;
#endif /* ISC_PLATFORM_HAVEIPV6 */

			default :
				exit(1);
			}

			netsyslog(LOG_ERR, "sendto(%s): %m", stoa(dest));
		}
	}
	else
	{
		inter->sent++;
		packets_sent++;
		/*
		 * He's not bad any more
		 */
		if (slot >= 0)
		{
			netsyslog(LOG_INFO, "Connection re-established to %s", stoa(dest));
			switch (dest->ss_family) {
			case AF_INET :
				badaddrs[slot].port = 0;
				break;
#ifdef ISC_PLATFORM_HAVEIPV6
			case AF_INET6 :
				badaddrs6[slot].port = 0;
				break;
#endif /* ISC_PLATFORM_HAVEIPV6 */
			}
		}
	}
}

#if !defined(HAVE_IO_COMPLETION_PORT)
/*
 * fdbits - generate ascii representation of fd_set (FAU debug support)
 * HFDF format - highest fd first.
 */
static char *
fdbits(
	int count,
	fd_set *set
	)
{
	static char buffer[256];
	char * buf = buffer;

	count = (count < 256) ? count : 255;

	while (count >= 0)
	{
		*buf++ = FD_ISSET(count, set) ? '#' : '-';
		count--;
	}
	*buf = '\0';

	return buffer;
}

/*
 * input_handler - receive packets asynchronously
 */
void
input_handler(
	l_fp *cts
	)
{
	register int i, n;
	register struct recvbuf *rb;
	register int doing;
	register SOCKET fd;
	struct timeval tvzero;
	int fromlen;
	l_fp ts;			/* Timestamp at BOselect() gob */
	l_fp ts_e;			/* Timestamp at EOselect() gob */
	fd_set fds;
	int select_count = 0;
	static int handler_count = 0;

	++handler_count;
	if (handler_count != 1)
	    msyslog(LOG_ERR, "input_handler: handler_count is %d!", handler_count);
	handler_calls++;
	ts = *cts;

	for (;;)
	{
		/*
		 * Do a poll to see who has data
		 */

		fds = activefds;
		tvzero.tv_sec = tvzero.tv_usec = 0;

		/*
		 * If we have something to do, freeze a timestamp.
		 * See below for the other cases (nothing (left) to do or error)
		 */
		while (0 < (n = select(maxactivefd+1, &fds, (fd_set *)0, (fd_set *)0, &tvzero)))
		{
			++select_count;
			++handler_pkts;

#ifdef REFCLOCK
			/*
			 * Check out the reference clocks first, if any
			 */
			if (refio != 0)
			{
				register struct refclockio *rp;

				for (rp = refio; rp != 0 && n > 0; rp = rp->next)
				{
					fd = rp->fd;
					if (FD_ISSET(fd, &fds))
					{
						n--;
						if (free_recvbuffs() == 0)
						{
							char buf[RX_BUFF_SIZE];

							(void) read(fd, buf, sizeof buf);
							packets_dropped++;
							goto select_again;
						}

						rb = get_free_recv_buffer();

						i = (rp->datalen == 0
						     || rp->datalen > sizeof(rb->recv_space))
						    ? sizeof(rb->recv_space) : rp->datalen;
						rb->recv_length =
						    read(fd, (char *)&rb->recv_space, (unsigned)i);

						if (rb->recv_length == -1)
						{
							netsyslog(LOG_ERR, "clock read fd %d: %m", fd);
							freerecvbuf(rb);
							goto select_again;
						}

						/*
						 * Got one.  Mark how
						 * and when it got here,
						 * put it on the full
						 * list and do
						 * bookkeeping.
						 */
						rb->recv_srcclock = rp->srcclock;
						rb->dstadr = 0;
						rb->fd = fd;
						rb->recv_time = ts;
						rb->receiver = rp->clock_recv;

						if (rp->io_input)
						{
							/*
							 * have direct
							 * input routine
							 * for refclocks
							 */
							if (rp->io_input(rb) == 0)
							{
								/*
								 * data
								 * was
								 * consumed
								 * -
								 * nothing
								 * to
								 * pass
								 * up
								 * into
								 * block
								 * input
								 * machine
								 */
								freerecvbuf(rb);
#if 1
								goto select_again;
#else
								continue;
#endif
							}
						}

						add_full_recv_buffer(rb);

						rp->recvcount++;
						packets_received++;
					}
				}
			}
#endif /* REFCLOCK */

			/*
			 * Loop through the interfaces looking for data
			 * to read.
			 */
			for (i = ninterfaces - 1; (i >= 0) && (n > 0); i--)
			{
				for (doing = 0; (doing < 2) && (n > 0); doing++)
				{
					if (doing == 0)
					{
						fd = inter_list[i].fd;
					}
					else
					{
						if (!(inter_list[i].flags & INT_BCASTOPEN))
						    break;
						fd = inter_list[i].bfd;
					}
					if (fd < 0) continue;
					if (FD_ISSET(fd, &fds))
					{
						n--;

						/*
						 * Get a buffer and read
						 * the frame.  If we
						 * haven't got a buffer,
						 * or this is received
						 * on the wild card
						 * socket, just dump the
						 * packet.
						 */
#ifdef UDP_WILDCARD_DELIVERY
				/*
				 * these guys manage to put properly addressed
				 * packets into the wildcard queue
				 */
						if (free_recvbuffs() == 0)
#else
						if((i == wildipv4) || (i == wildipv6)||
						   (free_recvbuffs() == 0))
#endif
	{
		char buf[RX_BUFF_SIZE];
		struct sockaddr_storage from;

		fromlen = sizeof from;
		(void) recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&from, &fromlen);
#ifdef DEBUG
		if (debug)
		    printf("%s on %d(%lu) fd=%d from %s\n",
			   (i) ? "drop" : "ignore",
			   i, free_recvbuffs(), fd,
			   stoa(&from));
#endif
		if (i == wildipv4 || i == wildipv6)
		    packets_ignored++;
		else
		    packets_dropped++;
		goto select_again;
	}

	rb = get_free_recv_buffer();

	fromlen = sizeof(struct sockaddr_storage);
	rb->recv_length = recvfrom(fd,
				   (char *)&rb->recv_space,
				   sizeof(rb->recv_space), 0,
				   (struct sockaddr *)&rb->recv_srcadr,
				   &fromlen);
	if (rb->recv_length == 0
#ifdef EWOULDBLOCK
		 || errno==EWOULDBLOCK
#endif
#ifdef EAGAIN
		 || errno==EAGAIN
#endif
		 ) {
		freerecvbuf(rb);
	    continue;
	}
	else if (rb->recv_length < 0)
	{
		netsyslog(LOG_ERR, "recvfrom(%s) fd=%d: %m",
 			stoa(&rb->recv_srcadr), fd);
#ifdef DEBUG
		if (debug)
		    printf("input_handler: fd=%d dropped (bad recvfrom)\n", fd);
#endif
		freerecvbuf(rb);
		continue;
	}
#ifdef DEBUG
	if (debug > 2) {
		if(rb->recv_srcadr.ss_family == AF_INET)
			printf("input_handler: if=%d fd=%d length %d from %08lx %s\n",
		   		i, fd, rb->recv_length,
				(u_long)ntohl(((struct sockaddr_in*)&rb->recv_srcadr)->sin_addr.s_addr) &
				0x00000000ffffffff,
			   	stoa(&rb->recv_srcadr));
		else
			printf("input_handler: if=%d fd=%d length %d from %s\n",
				i, fd, rb->recv_length,
				stoa(&rb->recv_srcadr));
        }
#endif

	/*
	 * Got one.  Mark how and when it got here,
	 * put it on the full list and do bookkeeping.
	 */
	rb->dstadr = &inter_list[i];
	rb->fd = fd;
	rb->recv_time = ts;
	rb->receiver = receive;

	add_full_recv_buffer(rb);

	inter_list[i].received++;
	packets_received++;
	goto select_again;
					}
					/* Check more interfaces */
				}
			}
		select_again:;
			/*
			 * Done everything from that select.  Poll again.
			 */
		}

		/*
		 * If nothing more to do, try again.
		 * If nothing to do, just return.
		 * If an error occurred, complain and return.
		 */
		if (n == 0)
		{
			if (select_count == 0) /* We really had nothing to do */
			{
				if (debug)
				    netsyslog(LOG_DEBUG, "input_handler: select() returned 0");
				--handler_count;
				return;
			}
			/* We've done our work */
			get_systime(&ts_e);
			/*
			 * (ts_e - ts) is the amount of time we spent
			 * processing this gob of file descriptors.  Log
			 * it.
			 */
			L_SUB(&ts_e, &ts);
			if (debug > 3)
			    netsyslog(LOG_INFO, "input_handler: Processed a gob of fd's in %s msec", lfptoms(&ts_e, 6));

			/* just bail. */
			--handler_count;
			return;
		}
		else if (n == -1)
		{
			int err = errno;

			/*
			 * extended FAU debugging output
			 */
			if (err != EINTR)
			    netsyslog(LOG_ERR,
				      "select(%d, %s, 0L, 0L, &0.0) error: %m",
				      maxactivefd+1,
				      fdbits(maxactivefd, &activefds));
			if (err == EBADF) {
				int j, b;

				fds = activefds;
				for (j = 0; j <= maxactivefd; j++)
				    if (
					    (FD_ISSET(j, &fds) && (read(j, &b, 0) == -1))
					    )
					netsyslog(LOG_ERR, "Bad file descriptor %d", j);
			}
			--handler_count;
			return;
		}
	}
	msyslog(LOG_ERR, "input_handler: fell out of infinite for(;;) loop!");
	--handler_count;
	return;
}

#endif
/*
 * findinterface - find interface corresponding to address
 */
struct interface *
findinterface(
	struct sockaddr_storage *addr
	)
{
	int i;
	SOCKET s;
	int rtn;
	struct sockaddr_storage saddr;
	int saddrlen;
	u_int32 amask, imask;

#ifdef DEBUG
	if (debug > 2)
	    printf("Finding interface for address: %s\n", stoa(addr));
#endif
	/*
	 * If there is only one outgoing interface we already know the interface
	 */
	if (addr->ss_family == AF_INET && outifaceipv4 != -1) {
#ifdef DEBUG
	if (debug > 2)
	    printf("Found only interface %d for address: %s\n", outifaceipv4, stoa(addr));
#endif
		return (&inter_list[outifaceipv4]);
	}
	if (addr->ss_family == AF_INET6 && outifaceipv6 != -1) {
#ifdef DEBUG
	if (debug > 2)
	    printf("Found only interface %d for address: %s\n", outifaceipv6, stoa(addr));
#endif
		return (&inter_list[outifaceipv6]);
	}

	/*
	 * If we got this far we need to try and match the
	 * network part of the address
	 */
	for (i= nwilds; i < ninterfaces; i++)
	{
		/*
		 * Skip the loopback. It can't act as an outgoing interface
		 */
		if (inter_list[i].flags & INT_LOOPBACK)
			continue;
		/*
		 * For IPv4 we can check the network mask to see if
		 * we have a match on the outgoing interface
		 */
		if (addr->ss_family == AF_INET && inter_list[i].family == AF_INET) {
			amask = (((struct sockaddr_in*)addr)->sin_addr.s_addr &
			    ((struct sockaddr_in*)&inter_list[i].mask)->sin_addr.s_addr);
			imask = (((struct sockaddr_in*)&inter_list[i].sin)->sin_addr.s_addr &
			    ((struct sockaddr_in*)&inter_list[i].mask)->sin_addr.s_addr);

			if (amask == imask) {
#ifdef DEBUG
				if (debug > 2)
				    printf("Found network local interface %d for address: %s\n", i, stoa(addr));
#endif
			     return (&inter_list[i]);
			}
		}

		/*
		 * See if the IPv6 address is Link-Local or Site Local
		 */
		if (addr->ss_family == AF_INET6 && inter_list[i].family == AF_INET6) {
			if (IN6_IS_ADDR_LINKLOCAL(&((struct sockaddr_in6*)addr)->sin6_addr) &&
			    IN6_IS_ADDR_LINKLOCAL(&((struct sockaddr_in6*)&inter_list[i].sin)->sin6_addr))
			{
#ifdef DEBUG
				if (debug > 2)
				    printf("Found Link-Local interface %d for address: %s\n", i, stoa(addr));
#endif
				return (&inter_list[i]);
			}

			if (IN6_IS_ADDR_SITELOCAL(&((struct sockaddr_in6*)addr)->sin6_addr) &&
			    IN6_IS_ADDR_SITELOCAL(&((struct sockaddr_in6*)&inter_list[i].sin)->sin6_addr))
			{
#ifdef DEBUG
				if (debug > 2)
				    printf("Found Site-Local interface %d for address: %s\n", i, stoa(addr));
#endif
				return (&inter_list[i]);
			}
		}
	}

	/*
	 * If we got here and failed because it was not a local network
	 * address, see if we have a PPP interface and use that
	 */
	for (i = nwilds; i < ninterfaces; i++)
	{
		if (addr->ss_family == AF_INET && inter_list[i].family == AF_INET &&
			inter_list[i].flags & INT_PPP)
		{
#ifdef DEBUG
			if (debug > 2)
			    printf("Found PPP interface %d for address: %s\n", i, stoa(addr));
#endif
			return (&inter_list[i]);
		}
		else if (addr->ss_family == AF_INET6 && inter_list[i].family == AF_INET6 &&
			 inter_list[i].flags & INT_PPP)
		{
#ifdef DEBUG
			if (debug > 2)
				printf("Found PPP interface %d for address: %s\n", i, stoa(addr));
#endif
			return (&inter_list[i]);
		}
	}

	/*
	 * If we got this far, we still don't have an interface.
	 * We pick the first one which is not loopback or link/site local
	 */

	for (i = nwilds; i < ninterfaces; i++)
	{
		if (inter_list[i].flags & INT_LOOPBACK)
			continue;

		if (addr->ss_family == AF_INET && inter_list[i].family == AF_INET)
		{
#ifdef DEBUG
			if (debug > 2)
			    printf("Found other interface %d for address: %s\n", i, stoa(addr));
#endif
			return (&inter_list[i]);
		}

		/*
		 * Skip if it's an IPv6 address and is Link-Local or Site Local
		 */
		if (addr->ss_family == AF_INET6 && inter_list[i].family == AF_INET6) {
			if (IN6_IS_ADDR_LINKLOCAL(&((struct sockaddr_in6*)&inter_list[i].sin)->sin6_addr))
				continue;

			if (IN6_IS_ADDR_SITELOCAL(&((struct sockaddr_in6*)&inter_list[i].sin)->sin6_addr))
				continue;
#ifdef DEBUG
			if (debug > 2)
			    printf("Found other interface %d for address: %s\n", i, stoa(addr));
#endif
			return (&inter_list[i]);
		}

	}
	/*
	 * If we got here then we are really in trouble
	 */

#ifdef DEBUG
	if (debug > 1)
	    printf("Having trouble finding interface for address: %s\n", stoa(addr));
#endif


	saddrlen = SOCKLEN(addr);

	/*
	 * This is considerably hoke. We open a socket, connect to it
	 * and slap a getsockname() on it. If anything breaks, as it
	 * probably will in some j-random knockoff, we just return the
	 * wildcard interface.
	 */
	memset(&saddr, 0, sizeof(saddr));
	saddr.ss_family = addr->ss_family;
	if(addr->ss_family == AF_INET)
		memcpy(&((struct sockaddr_in*)&saddr)->sin_addr, &((struct sockaddr_in*)addr)->sin_addr, sizeof(struct in_addr));
	else if(addr->ss_family == AF_INET6)
		memcpy(&((struct sockaddr_in6*)&saddr)->sin6_addr, &((struct sockaddr_in6*)addr)->sin6_addr, sizeof(struct in6_addr));
	((struct sockaddr_in*)&saddr)->sin_port = htons(2000);
	s = socket(addr->ss_family, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET)
		return ANY_INTERFACE_CHOOSE(addr);

	rtn = connect(s, (struct sockaddr *)&saddr, SOCKLEN(&saddr));
#ifndef SYS_WINNT
	if (rtn < 0)
#else
	if (rtn == SOCKET_ERROR)
#endif
	{
		closesocket(s);
		return ANY_INTERFACE_CHOOSE(addr);
	}

	rtn = getsockname(s, (struct sockaddr *)&saddr, &saddrlen);
	closesocket(s);
#ifndef SYS_WINNT
	if (rtn < 0)
#else
	if (rtn == SOCKET_ERROR)
#endif
		return ANY_INTERFACE_CHOOSE(addr);

	for (i = 0; i < ninterfaces; i++) {
		/*
		* First look if is the the correct family
		*/
		if(inter_list[i].sin.ss_family != saddr.ss_family)
	  		continue;
		/*
		 * We match the unicast address only.
		 */
		if (SOCKCMP(&inter_list[i].sin, &saddr))
			return (&inter_list[i]);
	}
	return ANY_INTERFACE_CHOOSE(addr);
}

/*
 * findbcastinter - find broadcast interface corresponding to address
 */
struct interface *
findbcastinter(
	struct sockaddr_storage *addr
	)
{	
	int i;
	int flagtype = INT_BROADCAST;

	i = find_interface_index(addr, flagtype, nwilds);
	if (i < 0 || i >= ninterfaces)
		return (NULL);
	else
		return (&inter_list[i]);
}

/*
 * find_interface_index - find the index of the interface given
 * the external address, a flag and a start index
 */

static int
find_interface_index(
		     struct sockaddr_storage *addr,
		     int flagtype,
		     int startind)
{
	int i;
	u_int32 amask, imask;
#ifdef DEBUG
	if (debug > 2)
	    printf("Finding *cast interface for address: %s\n", stoa(addr));
#endif
	/*
	 * If we got this far we need to try and match the
	 * network part of the address
	 */
	for (i= startind; i < ninterfaces; i++)
	{
		/*
		 * Skip the loopback. It can't act as an outgoing interface
		 * If it doesn't have the requested flag, skipp it too
		 */
		if (inter_list[i].flags & INT_LOOPBACK  || !(inter_list[i].flags & flagtype))
			continue;
		/*
		 * For IPv4 we can check the network mask to see if
		 * we have a match on the outgoing interface
		 */
		if (addr->ss_family == AF_INET && inter_list[i].family == AF_INET) {
			amask = (((struct sockaddr_in*)addr)->sin_addr.s_addr &
			    ((struct sockaddr_in*)&inter_list[i].mask)->sin_addr.s_addr);
			imask = (((struct sockaddr_in*)&inter_list[i].sin)->sin_addr.s_addr &
			    ((struct sockaddr_in*)&inter_list[i].mask)->sin_addr.s_addr);

			if (amask == imask) {
#ifdef DEBUG
				if (debug > 2)
				    printf("Found network local *cast interface %d for address: %s\n", i, stoa(addr));
#endif
			     return (i);
			}
		}

		/*
		 * See if the IPv6 address is Link-Local or Site Local
		 */
		if (addr->ss_family == AF_INET6 && inter_list[i].family == AF_INET6) {
			if (IN6_IS_ADDR_LINKLOCAL(&((struct sockaddr_in6*)addr)->sin6_addr) &&
			    IN6_IS_ADDR_LINKLOCAL(&((struct sockaddr_in6*)&inter_list[i].sin)->sin6_addr))
			{
#ifdef DEBUG
				if (debug > 2)
				    printf("Found Link-Local *cast interface %d for address: %s\n", i, stoa(addr));
#endif
				return (i);
			}

			if (IN6_IS_ADDR_SITELOCAL(&((struct sockaddr_in6*)addr)->sin6_addr) &&
			    IN6_IS_ADDR_SITELOCAL(&((struct sockaddr_in6*)&inter_list[i].sin)->sin6_addr))
			{
#ifdef DEBUG
				if (debug > 2)
				    printf("Found Site-Local *cast interface %d for address: %s\n", i, stoa(addr));
#endif
				return (i);
			}
		}
	}

	/*
	 * If we got here and failed because it was not a local network
	 * address, see if we have a PPP interface and use that
	 */
	for (i = startind; i < ninterfaces; i++)
	{
		/*
		 * Skip the loopback. It can't act as an outgoing interface
		 * If it doesn't have the requested flag, skipp it too
		 */
		if (inter_list[i].flags & INT_LOOPBACK  || !(inter_list[i].flags & flagtype))
			continue;
		if (addr->ss_family == AF_INET && inter_list[i].family == AF_INET &&
			inter_list[i].flags & INT_PPP)
		{
#ifdef DEBUG
			if (debug > 2)
			    printf("Found PPP *cast interface %d for address: %s\n", i, stoa(addr));
#endif
			return (i);
		}
		else if (addr->ss_family == AF_INET6 && inter_list[i].family == AF_INET6 &&
			 inter_list[i].flags & INT_PPP)
		{
#ifdef DEBUG
			if (debug > 2)
				printf("Found PPP *cast interface %d for address: %s\n", i, stoa(addr));
#endif
			return (i);
		}
	}

	/*
	 * If we got this far, we still don't have an interface.
	 * We pick the first one which is not loopback or link/site local
	 */

	for (i = startind; i < ninterfaces; i++)
	{
		/*
		 * Skip the loopback. It can't act as an outgoing interface
		 * If it doesn't have the requested flag, skipp it too
		 */
		if (inter_list[i].flags & INT_LOOPBACK  || !(inter_list[i].flags & flagtype))
			continue;

		if (addr->ss_family == AF_INET && inter_list[i].family == AF_INET)
		{
#ifdef DEBUG
			if (debug > 2)
			    printf("Found non-local *cast interface %d for address: %s\n", i, stoa(addr));
#endif
			return (i);
		}

		/*
		 * Skip if it's an IPv6 address and is Link-Local or Site Local
		 */
		if (addr->ss_family == AF_INET6 && inter_list[i].family == AF_INET6) {
			if (IN6_IS_ADDR_LINKLOCAL(&((struct sockaddr_in6*)&inter_list[i].sin)->sin6_addr))
				continue;

			if (IN6_IS_ADDR_SITELOCAL(&((struct sockaddr_in6*)&inter_list[i].sin)->sin6_addr))
				continue;
#ifdef DEBUG
			if (debug > 2)
			    printf("Found non-local *cast interface %d for address: %s\n", i, stoa(addr));
#endif
			return (i);
		}

	}
	/*
	 * If we got here then we are really in trouble
	 */

#ifdef DEBUG
	if (debug > 1)
	    printf("Having trouble finding *cast flag: %d interface for address: %s\n", flagtype, stoa(addr));
#endif

	return (-1);

}


/*
 * io_clr_stats - clear I/O module statistics
 */
void
io_clr_stats(void)
{
	packets_dropped = 0;
	packets_ignored = 0;
	packets_received = 0;
	packets_sent = 0;
	packets_notsent = 0;

	handler_calls = 0;
	handler_pkts = 0;
	io_timereset = current_time;
}


#ifdef REFCLOCK
/*
 * This is a hack so that I don't have to fool with these ioctls in the
 * pps driver ... we are already non-blocking and turn on SIGIO thru
 * another mechanisim
 */
int
io_addclock_simple(
	struct refclockio *rio
	)
{
	BLOCKIO();
	/*
	 * Stuff the I/O structure in the list and mark the descriptor
	 * in use.	There is a harmless (I hope) race condition here.
	 */
	rio->next = refio;
	refio = rio;

	/*
	 * I/O Completion Ports don't care about select and fd_set
	 */
#ifndef HAVE_IO_COMPLETION_PORT
	if (rio->fd > maxactivefd)
	    maxactivefd = rio->fd;
	FD_SET(rio->fd, &activefds);
#endif
	UNBLOCKIO();
	return 1;
}

/*
 * io_addclock - add a reference clock to the list and arrange that we
 *				 get SIGIO interrupts from it.
 */
int
io_addclock(
	struct refclockio *rio
	)
{
	BLOCKIO();
	/*
	 * Stuff the I/O structure in the list and mark the descriptor
	 * in use.	There is a harmless (I hope) race condition here.
	 */
	rio->next = refio;
	refio = rio;

# ifdef HAVE_SIGNALED_IO
	if (init_clock_sig(rio))
	{
		refio = rio->next;
		UNBLOCKIO();
		return 0;
	}
# elif defined(HAVE_IO_COMPLETION_PORT)
	if (io_completion_port_add_clock_io(rio))
	{
		add_socket_to_list(rio->fd);
		refio = rio->next;
		UNBLOCKIO();
		return 0;
	}
# endif

	/*
	 * I/O Completion Ports don't care about select and fd_set
	 */
#ifndef HAVE_IO_COMPLETION_PORT
	if (rio->fd > maxactivefd)
	    maxactivefd = rio->fd;
	FD_SET(rio->fd, &activefds);
#endif
	UNBLOCKIO();
	return 1;
}

/*
 * io_closeclock - close the clock in the I/O structure given
 */
void
io_closeclock(
	struct refclockio *rio
	)
{
	/*
	 * Remove structure from the list
	 */
	if (refio == rio)
	{
		refio = rio->next;
	}
	else
	{
		register struct refclockio *rp;

		for (rp = refio; rp != 0; rp = rp->next)
		    if (rp->next == rio)
		    {
			    rp->next = rio->next;
			    break;
		    }

		if (rp == 0)
		{
			/*
			 * Internal error.	Report it.
			 */
			msyslog(LOG_ERR,
				"internal error: refclockio structure not found");
			return;
		}
	}

	/*
	 * Close the descriptor.
	 */
	close_file(rio->fd);
}
#endif	/* REFCLOCK */

	/*
	 * I/O Completion Ports don't care about select and fd_set
	 */
#ifndef HAVE_IO_COMPLETION_PORT
void
kill_asyncio(
	int startfd
	)
{
	SOCKET i;

	BLOCKIO();
	for (i = startfd; i <= maxactivefd; i++)
	    (void)close_socket(i);
}
#else
/*
 * On NT a SOCKET is an unsigned int so we cannot possibly keep it in
 * an array. So we use one of the ISC_LIST functions to hold the
 * socket value and use that when we want to enumerate it.
 */
void
kill_asyncio(int startfd)
{
	vsock_t *lsock;
	vsock_t *next;

	BLOCKIO();

	lsock = ISC_LIST_HEAD(sockets_list);
	while (lsock != NULL) {
		next = ISC_LIST_NEXT(lsock, link);
		close_socket(lsock->fd);
		lsock = next;
	}

}
#endif
/*
 * Add and delete functions for the list of open sockets
 */
void
add_socket_to_list(SOCKET fd){
	vsock_t *lsock = (vsock_t *)malloc(sizeof(vsock_t));
	lsock->fd = fd;

	ISC_LIST_APPEND(sockets_list, lsock, link);
}
void
delete_socket_from_list(SOCKET fd) {

	vsock_t *next;
	vsock_t *lsock = ISC_LIST_HEAD(sockets_list);

	while(lsock != NULL) {
		next = ISC_LIST_NEXT(lsock, link);
		if(lsock->fd == fd) {
			ISC_LIST_DEQUEUE_TYPE(sockets_list, lsock, link, vsock_t);
			free(lsock);
			break;
		}
		else
			lsock = next;
	}
}

/*
 * Scan through the inter_list of interfaces looking for the socket
 */
int
find_socket_in_if(SOCKET fd) {
	int i;
#ifdef DEBUG
	if (debug)
	    printf("Finding socket %d in list of interfaces\n", fd);
#endif

	for (i = 0; i < ninterfaces; i++)
	{
		if(inter_list[i].fd == fd)
			return (i);
	}
	return (-1); /* Not found */
}

void
add_addr_to_list(struct sockaddr_storage *addr, int if_index, int flags){
	remaddr_t *laddr = (remaddr_t *)malloc(sizeof(remaddr_t));
	memcpy(&laddr->addr, addr, sizeof(struct sockaddr_storage));
	laddr->if_index = if_index;
	laddr->flags = flags;

	ISC_LIST_APPEND(remoteaddr_list, laddr, link);
#ifdef DEBUG
	if (debug)
	    printf("Added addr %s to list of addresses\n",
		   stoa(addr));
#endif


}
/*
 * Find the given address and modify the associated flags
 */
int
modify_addr_in_list(struct sockaddr_storage *addr, int flag) {

	int ind;
	remaddr_t *next;
	remaddr_t *laddr = ISC_LIST_HEAD(remoteaddr_list);
#ifdef DEBUG
	if (debug)
	    printf("Modifying addr %s in list of addresses\n",
		   stoa(addr));
#endif

	ind = -1;
	while(laddr != NULL) {
		next = ISC_LIST_NEXT(laddr, link);
		if(SOCKCMP(&laddr->addr, addr)) {
			laddr->flags = flag;
			ind = laddr->if_index;
			break;
		}
		else
			laddr = next;
	}
	return (ind); /* Not found */
}

void
delete_addr_from_list(struct sockaddr_storage *addr) {

	remaddr_t *next;
	remaddr_t *laddr = ISC_LIST_HEAD(remoteaddr_list);

	while(laddr != NULL) {
		next = ISC_LIST_NEXT(laddr, link);
		if(SOCKCMP(&laddr->addr, addr)) {
			ISC_LIST_DEQUEUE_TYPE(remoteaddr_list, laddr, link, remaddr_t);
			free(laddr);
			break;
		}
		else
			laddr = next;
	}
#ifdef DEBUG
	if (debug)
	    printf("Deleted addr %s from list of addresses\n",
		   stoa(addr));
#endif
}
int
find_addr_in_list(struct sockaddr_storage *addr) {

	remaddr_t *next;
	remaddr_t *laddr = ISC_LIST_HEAD(remoteaddr_list);
#ifdef DEBUG
	if (debug)
	    printf("Finding addr %s in list of addresses\n",
		   stoa(addr));
#endif

	while(laddr != NULL) {
		next = ISC_LIST_NEXT(laddr, link);
		if(SOCKCMP(&laddr->addr, addr)) {
			return (laddr->if_index);
			break;
		}
		else
			laddr = next;
	}
	return (-1); /* Not found */
}

/*
 * Find the given address with the associated flag in the list
 */
int
find_flagged_addr_in_list(struct sockaddr_storage *addr, int flag) {

	remaddr_t *next;
	remaddr_t *laddr = ISC_LIST_HEAD(remoteaddr_list);
#ifdef DEBUG
	if (debug)
	    printf("Finding addr %s in list of addresses\n",
		   stoa(addr));
#endif

	while(laddr != NULL) {
		next = ISC_LIST_NEXT(laddr, link);
		if(SOCKCMP(&laddr->addr, addr) && (laddr->flags & flag)) {
			return (laddr->if_index);
			break;
		}
		else
			laddr = next;
	}
	return (-1); /* Not found */
}
