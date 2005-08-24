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
#include "ntp_stdlib.h"
#include "ntp_request.h"
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
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H	/* UXPV: SIOC* #defines (Frank Vance <fvance@waii.com>) */
# include <sys/sockio.h>
#endif

/* 
 * Set up some macros to look for IPv6 and IPv6 multicast
 */

#if defined(ISC_PLATFORM_HAVEIPV6) && !defined(DISABLE_IPV6)

#define INCLUDE_IPV6_SUPPORT

#if defined(INCLUDE_IPV6_SUPPORT) && defined(IPV6_JOIN_GROUP) && defined(IPV6_LEAVE_GROUP)
#define INCLUDE_IPV6_MULTICAST_SUPPORT

#endif	/* IPV6 Multicast Support */
#endif  /* IPv6 Support */

extern int listen_to_virtual_ips;
extern const char *specific_interface;

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
struct interface *any_interface;	/* default ipv4 interface */
struct interface *any6_interface;	/* default ipv6 interface */
struct interface *loopback_interface;	/* loopback ipv4 interface */
struct interface *loopback6_interface;	/* loopback ipv6 interface */

int ninterfaces;			/* Total number of interfaces */

#ifdef REFCLOCK
/*
 * Refclock stuff.	We keep a chain of structures with data concerning
 * the guys we are doing I/O for.
 */
static	struct refclockio *refio;
#endif /* REFCLOCK */


/*
 * Define what the possible "soft" errors can be.  These are non-fatal returns
 * of various network related functions, like recv() and so on.
 *
 * For some reason, BSDI (and perhaps others) will sometimes return <0
 * from recv() but will have errno==0.  This is broken, but we have to
 * work around it here.
 */
#define SOFT_ERROR(e)	((e) == EAGAIN || \
			 (e) == EWOULDBLOCK || \
			 (e) == EINTR || \
			 (e) == 0)

/*
 * File descriptor masks etc. for call to select
 * Not needed for I/O Completion Ports
 */
fd_set activefds;
int maxactivefd;
/*
 * bit alternating value to detect verified interfaces during an update cycle
 */
static  u_char          sys_interphase = 0;

static  struct interface *new_interface P((struct interface *));
static  void add_interface P((struct interface *));
static  void update_interfaces P((u_short, interface_receiver_t, void *));
static  void remove_interface P((struct interface *));
static  struct interface *find_iface P((struct sockaddr_storage *saddr, char *));
static  struct interface *create_interface P((u_short, struct interface *));


static	int create_sockets	P((u_short));
static	SOCKET	open_socket	P((struct sockaddr_storage *, int, int, struct interface *));
static	void	close_socket	P((SOCKET));
#ifdef REFCLOCK
static	void	close_file	P((SOCKET));
#endif
static	char *	fdbits		P((int, fd_set *));
static	void	set_reuseaddr	P((int));
static	isc_boolean_t	socket_broadcast_enable	 P((struct interface *, SOCKET, struct sockaddr_storage *));
static	isc_boolean_t	socket_broadcast_disable P((struct interface *, struct sockaddr_storage *));

/*
 * Multicast functions
 */
static	isc_boolean_t	addr_ismulticast	 P((struct sockaddr_storage *));
/*
 * Not all platforms support multicast
 */
#ifdef MCAST
static	isc_boolean_t	socket_multicast_enable	 P((struct interface *, int, struct sockaddr_storage *));
static	isc_boolean_t	socket_multicast_disable P((struct interface *, struct sockaddr_storage *));
#endif

#ifdef DEBUG
static void print_interface	P((struct interface *, char *, char *));
#endif

typedef struct vsock vsock_t;

struct vsock {
	SOCKET				fd;
	ISC_LINK(vsock_t)		link;
};

ISC_LIST(vsock_t)	sockets_list;

typedef struct remaddr remaddr_t;

struct remaddr {
      struct sockaddr_storage	 addr;
      struct interface               *interface;
      ISC_LINK(remaddr_t)	 link;
};

ISC_LIST(remaddr_t)       remoteaddr_list;

ISC_LIST(struct interface)     inter_list;

static struct interface *wildipv4 = NULL;
static struct interface *wildipv6 = NULL;

void	add_socket_to_list	P((SOCKET));
void	delete_socket_from_list	P((SOCKET));
void	add_addr_to_list	P((struct sockaddr_storage *, struct interface *));
void	delete_addr_from_list	P((struct sockaddr_storage *));
struct interface *find_addr_in_list	P((struct sockaddr_storage *));
struct interface *find_flagged_addr_in_list P((struct sockaddr_storage *, int));
static void create_wildcards	P((u_short));
isc_boolean_t address_okay	P((isc_interface_t *));
void	convert_isc_if		P((isc_interface_t *, struct interface *, u_short));
static struct interface *findlocalinterface	P((struct sockaddr_storage *));
static struct interface *findlocalcastinterface	P((struct sockaddr_storage *, int));

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
 * About interfaces, sockets, reception and more...
 *
 * the code solves following tasks:
 *
 *   - keep a current list of active interfaces in order
 *     to bind to to the interface address on NTP_PORT so that
 *     all wild and specific bindings for NTP_PORT are taken by ntpd
 *     to avoid other daemons messing with the time or sockets.
 *   - all interfaces keep a list of peers that are referencing 
 *     the interface in order to quickly re-assign the peers to
 *     new interface in case an interface is deleted (=> gone from system or
 *     down)
 *   - have a preconfigured socket ready with the right local address
 *     for transmission and reception
 *   - have an address list for all destination addresses used within ntpd
 *     to find the "right" preconfigured socket.
 *   - facilitate updating the internal interface list with respect to
 *     the current kernel state
 *
 * special issues:
 *
 *   - mapping of multicast addresses to the interface affected is not always
 *     one to one - exspecially on hosts with multiple interfaces
 *     the code here currently allocates a separate interface entry for those
 *     multicast addresses
 *     iff it is able to bind to a *new* socket with the multicast address (flags |= MCASTIF)
 *     in case of failure the multicast address of bound to an existing interface.
 *   - on some systems it is perfectly legal to assign the same address to
 *     multiple interfaces. Therefore this code does not really keep a list of interfaces
 *     but a list of interfaces that represent a unique address as determined by the kernel
 *     by the procedure in findlocalinterface. Thus it is perfectly legal to see only
 *     one representavive of a group of real interfaces if they share the same address.
 *    
 */

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
	any_interface = NULL;
	any6_interface = NULL;

#ifdef REFCLOCK
	refio = 0;
#endif

#if defined(HAVE_SIGNALED_IO)
	(void) set_signal();
#endif

	ISC_LIST_INIT(sockets_list);

        ISC_LIST_INIT(remoteaddr_list);

	ISC_LIST_INIT(inter_list);

	/*
	 * Create the sockets
	 */
	BLOCKIO();
	(void) create_sockets(htons(NTP_PORT));
	UNBLOCKIO();

	DPRINTF(1, ("init_io: maxactivefd %d\n", maxactivefd));
}

#ifdef DEBUG
/*
 * function to dump the contents of the interface structure
 * for debugging use only.
 */
void
interface_dump(struct interface *itf)
{
	u_char* cp;
	int i;
	/* Limit the size of the sockaddr_storage hex dump */
	int maxsize = min(32, sizeof(struct sockaddr_storage));

	printf("Dumping interface: %p\n", itf);
	printf("fd = %d\n", itf->fd);
	printf("bfd = %d\n", itf->bfd);
	printf("sin = %s,\n", stoa(&(itf->sin)));
	cp = (u_char*) &(itf->sin);
	for(i = 0; i < maxsize; i++)
	{
		printf("%02x", *cp++);
		if((i+1)%4 == 0)
			printf(" ");
	}
	printf("\n");
	printf("bcast = %s,\n", stoa(&(itf->bcast)));
	cp = (u_char*) &(itf->bcast);
	for(i = 0; i < maxsize; i++)
	{
		printf("%02x", *cp++);
		if((i+1)%4 == 0)
			printf(" ");
	}
	printf("\n");
	printf("mask = %s,\n", stoa(&(itf->mask)));
	cp = (u_char*) &(itf->mask);
	for(i = 0; i < maxsize; i++)
	{
		printf("%02x", *cp++);
		if((i+1)%4 == 0)
			printf(" ");
	}
	printf("\n");
	printf("name = %s\n", itf->name);
	printf("flags = 0x%08x\n", itf->flags);
	printf("last_ttl = %d\n", itf->last_ttl);
	printf("addr_refid = %08x\n", itf->addr_refid);
	printf("num_mcast = %d\n", itf->num_mcast);
	printf("received = %ld\n", itf->received);
	printf("sent = %ld\n", itf->sent);
	printf("notsent = %ld\n", itf->notsent);
	printf("ifindex = %u\n", itf->ifindex);
	printf("scopeid = %u\n", itf->scopeid);
	printf("peercnt = %u\n", itf->peercnt);
	printf("phase = %u\n", itf->phase);
}

/*
 * print_interface - helper to output debug information
 */
static void
print_interface(struct interface *iface, char *pfx, char *sfx)
{
	printf("%sinterface #%d: fd=%d, bfd=%d, name=%s, flags=0x%x, scope=%d",
	       pfx,
	       iface->ifindex,
	       iface->fd,
	       iface->bfd,
	       iface->name,
	       iface->flags,
	       iface->scopeid);
	/* Leave these as three printf calls. */
	printf(", sin=%s",
	       stoa((&iface->sin)));
	if (iface->flags & INT_BROADCAST)
		printf(", bcast=%s,",
		       stoa((&iface->bcast)));
	if (iface->family == AF_INET)
	  printf(", mask=%s",
		 stoa((&iface->mask)));
	printf(", %s:%s", iface->ignore_packets == ISC_FALSE ? "Enabled" : "Disabled", sfx);
	if (debug > 4)	/* in-depth debugging only */
		interface_dump(iface);
}

#endif

/*
 * interface list enumerator - visitor pattern
 */
void
interface_enumerate(interface_receiver_t receiver, void *data)
{
	interface_info_t ifi;
        struct interface *interf;

	ifi.action = IFS_EXISTS;
	
	for (interf = ISC_LIST_HEAD(inter_list);
	     interf != NULL;
	     interf = ISC_LIST_NEXT(interf, link)) {
		ifi.interface = interf;
		receiver(data, &ifi);
	}
}

/*
 * do standard initialization of interface structure
 */
static void
init_interface(struct interface *interface)
{
	memset((char *)interface, 0, sizeof(struct interface));
	ISC_LINK_INIT(interface, link);
	ISC_LIST_INIT(interface->peers);
	interface->fd = INVALID_SOCKET;
	interface->bfd = INVALID_SOCKET;
	interface->num_mcast = 0;
	interface->received = 0;
	interface->sent = 0;
	interface->notsent = 0;
	interface->peercnt = 0;
	interface->phase = sys_interphase;
}

/*
 * create new interface structure initialize from
 * template structure or via standard initialization
 * function
 */
static struct interface *
new_interface(struct interface *interface)
{
	static u_int sys_ifnum = 0;

	struct interface *iface = (struct interface *)emalloc(sizeof(struct interface));

	if (interface != NULL)
	{
		memcpy((char*)iface, (char*)interface, sizeof(*interface));
	}
	else
	{
		init_interface(iface);
	}

	iface->ifnum = sys_ifnum++;  /* count every new instance of an interface in the system */

	return iface;
}

/*
 * return interface storage into free memory pool
 */
static void
delete_interface(struct interface *interface)
{
	free(interface);
}

/*
 * link interface into list of known interfaces
 */
static void
add_interface(struct interface *interface)
{
	/*
	 * Calculate the address hash
	 */
	interface->addr_refid = addr2refid(&interface->sin);
	
	ISC_LIST_APPEND(inter_list, interface, link);
	ninterfaces++;
}

/*
 * remove interface from knoen interface list and clean up
 * associated resources
 */
static void
remove_interface(struct interface *interface)
{
	struct sockaddr_storage resmask;

	ISC_LIST_UNLINK_TYPE(inter_list, interface, link, struct interface);

	delete_addr_from_list(&interface->sin);
  
	if (interface->fd != INVALID_SOCKET) 
	{
		msyslog(LOG_INFO, "Deleting interface #%d %s, %s#%d",
			interface->ifindex,
			interface->name,
			stoa((&interface->sin)),
			NTP_PORT);  /* XXX should extract port from sin structure */
		close_socket(interface->fd);
	}
  
	if (interface->bfd != INVALID_SOCKET) 
	{
		msyslog(LOG_INFO, "Deleting interface #%d %s, broadcast address %s#%d",
			interface->ifindex,
			interface->name,
			stoa((&interface->bcast)),
			(u_short) NTP_PORT);  /* XXX extract port from sin structure */
		close_socket(interface->bfd);
	}

	ninterfaces--;
	ntp_monclearinterface(interface);

	/* remove restrict interface entry */

	/*
	 * Blacklist bound interface address
	 */
	SET_HOSTMASK(&resmask, interface->sin.ss_family);
	hack_restrict(RESTRICT_REMOVEIF, &interface->sin, &resmask,
		      RESM_NTPONLY|RESM_INTERFACE, RES_IGNORE);
}

void
create_wildcards(u_short port) {
	isc_boolean_t okipv4 = ISC_TRUE;
	/*
	 * create pseudo-interface with wildcard IPv4 address
	 */
#ifdef IPV6_V6ONLY
	if(isc_net_probeipv4() != ISC_R_SUCCESS)
		okipv4 = ISC_FALSE;
#endif

	if(okipv4 == ISC_TRUE) {
	        struct interface *interface = new_interface(NULL);

		interface->family = AF_INET;
		interface->sin.ss_family = AF_INET;
		((struct sockaddr_in*)&interface->sin)->sin_addr.s_addr = htonl(INADDR_ANY);
		((struct sockaddr_in*)&interface->sin)->sin_port = port;
		(void) strncpy(interface->name, "wildcard", sizeof(interface->name));
		interface->mask.ss_family = AF_INET;
		((struct sockaddr_in*)&interface->mask)->sin_addr.s_addr = htonl(~(u_int32)0);
		interface->flags = INT_BROADCAST | INT_UP | INT_WILDCARD;
		interface->ignore_packets = ISC_TRUE;
#if defined(MCAST)
	/*
	 * enable possible multicast reception on the broadcast socket
	 */
		interface->bcast.ss_family = AF_INET;
		((struct sockaddr_in*)&interface->bcast)->sin_port = port;
		((struct sockaddr_in*)&interface->bcast)->sin_addr.s_addr = htonl(INADDR_ANY);
#endif /* MCAST */
		interface->fd = open_socket(&interface->sin,
				 interface->flags, 0, interface);

		wildipv4 = interface;
		add_interface(interface);
	}

#ifdef INCLUDE_IPV6_SUPPORT
	/*
	 * create pseudo-interface with wildcard IPv6 address
	 */
	if (isc_net_probeipv6() == ISC_R_SUCCESS) {
	        struct interface *interface = new_interface(NULL);

		interface->family = AF_INET6;
		interface->sin.ss_family = AF_INET6;
		((struct sockaddr_in6*)&interface->sin)->sin6_addr = in6addr_any;
 		((struct sockaddr_in6*)&interface->sin)->sin6_port = port;
 		((struct sockaddr_in6*)&interface->sin)->sin6_scope_id = 0;
		(void) strncpy(interface->name, "wildcard", sizeof(interface->name));
		interface->mask.ss_family = AF_INET6;
		memset(&((struct sockaddr_in6*)&interface->mask)->sin6_addr.s6_addr, 0xff, sizeof(struct in6_addr));
		interface->flags = INT_UP | INT_WILDCARD;
		interface->ignore_packets = ISC_TRUE;

		interface->fd = open_socket(&interface->sin,
				 interface->flags, 0, interface);

		wildipv6 = interface;
		add_interface(interface);
	}
#endif
}


isc_boolean_t
address_okay(isc_interface_t *isc_if) {

	DPRINTF(3, ("address_okay: listen Virtual: %d, IF name: %s, Up Flag: %d\n", 
		    listen_to_virtual_ips, isc_if->name, (isc_if->flags & INTERFACE_F_UP)));

	/*
	 * Always allow the loopback
	 */
	if((isc_if->flags & INTERFACE_F_LOOPBACK) != 0)
		return (ISC_TRUE);

	/*
	 * Check if the interface is specified
	 */
	if (specific_interface != NULL) {
		if (strcasecmp(isc_if->name, specific_interface) == 0)
			return (ISC_TRUE);
		else
			return (ISC_FALSE);
	}
	else {
		if (listen_to_virtual_ips == 0  && 
		   (strchr(isc_if->name, (int)':') != NULL))
			return (ISC_FALSE);
	}

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
	itf->scopeid = 0;
	itf->family = (short) isc_if->af;
	if(isc_if->af == AF_INET) {
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
	}
#ifdef INCLUDE_IPV6_SUPPORT
	else if (isc_if->af == AF_INET6) {
		itf->sin.ss_family = (u_short) isc_if->af;
		strcpy(itf->name, isc_if->name);
		memcpy(&(((struct sockaddr_in6 *)&itf->sin)->sin6_addr),
		       &(isc_if->address.type.in6),
		       sizeof(((struct sockaddr_in6 *)&itf->sin)->sin6_addr));
		((struct sockaddr_in6 *)&itf->sin)->sin6_port = port;

#ifdef ISC_PLATFORM_HAVESCOPEID
		((struct sockaddr_in6 *)&itf->sin)->sin6_scope_id = isc_netaddr_getzone(&isc_if->address);
		itf->scopeid = isc_netaddr_getzone(&isc_if->address);
#endif
		itf->mask.ss_family = itf->sin.ss_family;
		memcpy(&(((struct sockaddr_in6 *)&itf->mask)->sin6_addr),
		       &(isc_if->netmask.type.in6),
		       sizeof(struct in6_addr));
		((struct sockaddr_in6 *)&itf->mask)->sin6_port = port;
		/* Copy the interface index */
		itf->ifindex = isc_if->ifindex;
	}
#endif /* INCLUDE_IPV6_SUPPORT */


		/* Process the rest of the flags */

	if((isc_if->flags & INTERFACE_F_UP) != 0)
		itf->flags |= INT_UP;
	if((isc_if->flags & INTERFACE_F_LOOPBACK) != 0)
		itf->flags |= INT_LOOPBACK;
	if((isc_if->flags & INTERFACE_F_POINTTOPOINT) != 0)
		itf->flags |= INT_PPP;
	if((isc_if->flags & INTERFACE_F_MULTICAST) != 0)
		itf->flags |= INT_MULTICAST;

}

/*
 * interface_update - externally callable update function
 */
void
interface_update(interface_receiver_t receiver, void *data)
{
  /* XXX verify BLOCK_IO(); */
	update_interfaces(htons(NTP_PORT), receiver, data);
  /* XXX verify UNBLOCK_IO(); */
}

/*
 * update_interfaces
 * strategy
 * toggle configuration phase
 * Phase 1:
 * forall currently existing interfaces
 *   if address is known: copy configuration phase into interface (make it seen)
 *   if addres is NOT known: attempt to create a new interface entry
 * Phase 2:
 * forall currently known interfaces
 *   if interface does not match configuration phase (not seen in phase 1):
 *     remove interface from known interface list
 *     forall peers associated with this interface
 *       disconnect peer from this interface
 *       attempt to re-assign interface to peer (will unpeer() unconfigured peers)
 *
 * done
 */

static void
update_interfaces(
	u_short port,
	interface_receiver_t receiver,
	void *data
	)
{
	interface_info_t ifi;
	int need_refresh = 0;
	isc_mem_t *mctx = NULL;
	isc_interfaceiter_t *iter = NULL;
	isc_boolean_t scan_ipv4 = ISC_FALSE;
	isc_boolean_t scan_ipv6 = ISC_FALSE;
	isc_result_t result;

	DPRINTF(1, ("update_interfaces(%d)\n", ntohs( (u_short) port)));

#ifdef INCLUDE_IPV6_SUPPORT
	if (isc_net_probeipv6() == ISC_R_SUCCESS)
		scan_ipv6 = ISC_TRUE;
#if defined(DEBUG)
	else
		if(debug)
			netsyslog(LOG_ERR, "no IPv6 interfaces found");
#endif
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
	/*
	 * phase one - scan interfaces
	 * - create those that are not found
	 * - update those that are found
	 */

	sys_interphase ^= 0x1;	/* toggle system phase for finding untouched (to be deleted) interfaces */
	
	result = isc_interfaceiter_create(mctx, &iter);

	if (result != ISC_R_SUCCESS)
		return;

	for (result = isc_interfaceiter_first(iter);
	     result == ISC_R_SUCCESS;
	     result = isc_interfaceiter_next(iter))
	{
		isc_interface_t isc_if;
		unsigned int family;
		struct interface interface;
		struct interface *iface;
		
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

		/*
		 * create prototype
		 */
		init_interface(&interface);

		convert_isc_if(&isc_if, &interface, port);

		if (!(interface.flags & INT_UP))  /* interfaces must be UP to be usable */
			continue;

		/*
		 * map to local *address* in order
		 * to map all duplicate interfaces to an interface structure
		 * with the appropriate socket (our name space is
		 * (ip-address) - NOT (interface name, ip-address))
		 */
		iface = findlocalinterface(&interface.sin);
		
		if (iface) 
		{
			/*
			 * found existing interface - mark as verified
			 * XXX - should compare central parameters...
			 */
			iface->phase = sys_interphase;
#ifdef DEBUG
			if (debug > 1)
				print_interface(iface, "updating ", " present - updated\n");
#endif
			ifi.action = IFS_EXISTS;
			ifi.interface = iface;
			if (receiver)
				receiver(data, &ifi);
		}
		else
		{
			/*
			 * this is new - add to out interface list
			 */
			iface = create_interface(port, &interface);

			need_refresh = 1;
			
			ifi.action = IFS_CREATED;
			ifi.interface = iface;
			if (receiver && iface)
				receiver(data, &ifi);
#ifdef DEBUG
			if (debug > 1) {
				print_interface(iface ? iface : &interface, "updating ", iface ? " new - created\n" : " new - creation FAILED");
#endif
		}
	}

	isc_interfaceiter_destroy(&iter);

	/*
	 * phase 2 - delete gone interfaces - reassigning peers to other interfaces
	 */
	{
		struct interface *interf = ISC_LIST_HEAD(inter_list);

		while (interf != NULL)
		{
			struct interface *next = ISC_LIST_NEXT(interf, link);
			  
			if (!(interf->flags & (INT_WILDCARD|INT_MCASTIF))) {
				/*
				 * if phase does not match sys_phase this interface was not
				 * enumerated during interface scan - so it is gone and
				 * will be deleted here unless it is solely an MCAST interface
				 */
				if (interf->phase != sys_interphase) {
					struct peer *peer;
#ifdef DEBUG
					if (debug > 1)
						print_interface(interf, "updating ", "GONE - deleting\n");
#endif
					remove_interface(interf);

					ifi.action = IFS_DELETED;
					ifi.interface = interf;
					if (receiver)
						receiver(data, &ifi);

					peer = ISC_LIST_HEAD(interf->peers);
					/*
					 * disconnect peer from deleted interface
					 */
					while (peer != NULL) {
						struct peer *npeer = ISC_LIST_NEXT(peer, ilink);
						
						/*
						 * this one just lost it's interface
						 */
						need_refresh = 1;
						set_peerdstadr(peer, NULL);
	
						peer = npeer;
					}
					delete_interface(interf);
				}
			}
			interf = next;
		}
	}

	/*
	 * now re-configure as the world has changed
	 */
	if (need_refresh) 
		refresh_all_peerinterfaces();
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
#ifndef HAVE_IO_COMPLETION_PORT
	/*
	 * I/O Completion Ports don't care about the select and FD_SET
	 */
	maxactivefd = 0;
	FD_ZERO(&activefds);
#endif

	DPRINTF(2, ("create_sockets(%d)\n", ntohs( (u_short) port)));

	create_wildcards(port);

	update_interfaces(port, NULL, NULL);
	
	/*
	 * Now that we have opened all the sockets, turn off the reuse
	 * flag for security.
	 */
	set_reuseaddr(0);

	DPRINTF(2, ("create_sockets: Total interfaces = %d\n", ninterfaces));

	return ninterfaces;
}

/*
 * create_sockets - create a socket for each interface plus a default
 *			socket for when we don't know where to send
 */
static struct interface *
create_interface(
		 u_short port,
		 struct interface *iface
		 )
{
	struct sockaddr_storage resmask;
	struct interface *interface;
	isc_interface_t isc_if;

	DPRINTF(2, ("create_interface(%s#%d)\n", stoa(&iface->sin), ntohs( (u_short) port)));

	/* build an interface */
	interface = new_interface(iface);
	
	/* 
	 * Check to see if we are going to use the interface
	 * If we don't use it we mark it to drop any packet
	 * received but we still must create the socket and
	 * bind to it. This prevents other apps binding to it
	 * and potentially causing problems with more than one
	 * process fiddling with the clock
	 */
	if (address_okay(&isc_if) == ISC_TRUE) {
	  interface->ignore_packets = ISC_FALSE;
	}
	else {
	  interface->ignore_packets = ISC_TRUE;
	}

	/*
	 * create socket
	 */
	interface->fd = open_socket(&interface->sin,
				 interface->flags, 0, interface);

	if (interface->fd != INVALID_SOCKET)
	  msyslog(LOG_INFO, "Listening on interface #%d %s, %s#%d %s",
		  interface->ifnum,
		  interface->name,
		  stoa((&interface->sin)),
		  ntohs( (u_short) port),
		  (interface->ignore_packets == ISC_FALSE) ?
		  "Enabled": "Disabled");

	if ((interface->flags & INT_BROADCAST) &&
	    interface->bfd != INVALID_SOCKET)
	  msyslog(LOG_INFO, "Listening on broadcast address %s#%d",
		  stoa((&interface->bcast)),
		  ntohs( (u_short) port));

	if (interface->fd == INVALID_SOCKET &&
	    interface->bfd == INVALID_SOCKET) {
		msyslog(LOG_ERR, "unable to create socket on %s (%d) for %s#%d",
			interface->name,
			interface->ifnum,
			stoa((&interface->sin)),
			ntohs( (u_short) port));
		delete_interface(interface);
		return NULL;
	}
	
        /*
	 * Blacklist bound interface address
	 */
	
	SET_HOSTMASK(&resmask, interface->sin.ss_family);
	hack_restrict(RESTRICT_FLAGS, &interface->sin, &resmask,
		      RESM_NTPONLY|RESM_INTERFACE, RES_IGNORE);
	  
	/*
	 * set globals with the first found
	 * loopback interface of the appropriate class
	 */
	if ((loopback_interface == NULL) &&
	    (interface->family == AF_INET) &&
	    ((interface->flags & INT_LOOPBACK) != 0))
	{
		loopback_interface = interface;
	}

	if ((loopback6_interface == NULL) &&
	    (interface->family == AF_INET6) &&
	    ((interface->flags & INT_LOOPBACK) != 0))
	{
		loopback6_interface = interface;
	}

	/*
	 * put into our interface list
	 */
	add_interface(interface);

#ifdef DEBUG
	if (debug > 1) {
	  print_interface(interface, "created ", "\n");
	}
#endif
	return interface;
}

/*
 * set_reuseaddr() - set/clear REUSEADDR on all sockets
 *			NB possible hole - should we be doing this on broadcast
 *			fd's also?
 */
static void
set_reuseaddr(int flag) {
        struct interface *interf;

	for (interf = ISC_LIST_HEAD(inter_list);
	     interf != NULL;
	     interf = ISC_LIST_NEXT(interf, link)) {
	        if (interf->flags & INT_WILDCARD)
		        continue;
	  
		/*
		 * if interf->fd  is INVALID_SOCKET, we might have a adapter
		 * configured but not present
		 */
		DPRINTF(2, ("setting SO_REUSEADDR on %.16s@%s to %s\n", interf->name, stoa(&interf->sin), flag ? "on" : "off"));
		
		if (interf->fd != INVALID_SOCKET) {
			if (setsockopt(interf->fd, SOL_SOCKET,
					SO_REUSEADDR, (char *)&flag,
					sizeof(flag))) {
				netsyslog(LOG_ERR, "set_reuseaddr: setsockopt(SO_REUSEADDR, %s) failed: %m", flag ? "on" : "off");
			}
		}
	}
}

/*
 * This is just a wrapper around an internal function so we can
 * make other changes as necessary later on
 */
void
enable_broadcast(struct interface *iface, struct sockaddr_storage *baddr)
{
#ifdef SO_BROADCAST
	socket_broadcast_enable(iface, iface->fd, baddr);
#endif
}

#ifdef OPEN_BCAST_SOCKET 
/*
 * Enable a broadcast address to a given socket
 * The socket is in the inter_list all we need to do is enable
 * broadcasting. It is not this function's job to select the socket
 */
static isc_boolean_t
socket_broadcast_enable(struct interface *iface, SOCKET fd, struct sockaddr_storage *maddr)
{
#ifdef SO_BROADCAST
	int on = 1;

	if (maddr->ss_family == AF_INET)
	{
		/* if this interface can support broadcast, set SO_BROADCAST */
		if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST,
			       (char *)&on, sizeof(on)))
		{
			netsyslog(LOG_ERR, "setsockopt(SO_BROADCAST) enable failure on address %s: %m",
				stoa(maddr));
		}
#ifdef DEBUG
		else if (debug > 1) {
			printf("Broadcast enabled on socket %d for address %s\n",
				fd, stoa(maddr));
		}
#endif
	}
	iface->flags |= INT_BCASTOPEN;
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
socket_broadcast_disable(struct interface *iface, struct sockaddr_storage *maddr)
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
	return ISC_TRUE;
#else
	return ISC_FALSE;
#endif /* SO_BROADCAST */
}

#endif /* OPEN_BCAST_SOCKET */
/*
 * Check to see if the address is a multicast address
 */
static isc_boolean_t
addr_ismulticast(struct sockaddr_storage *maddr)
{
	switch (maddr->ss_family)
	{
	case AF_INET :
		if (!IN_CLASSD(ntohl(((struct sockaddr_in*)maddr)->sin_addr.s_addr))) {
			DPRINTF(1, ("multicast address %s not class D\n", stoa(maddr)));
			return (ISC_FALSE);
		}
		else
		{
			return (ISC_TRUE);
		}

	case AF_INET6 :
#ifdef INCLUDE_IPV6_MULTICAST_SUPPORT
		if (!IN6_IS_ADDR_MULTICAST(&((struct sockaddr_in6*)maddr)->sin6_addr)) {
			DPRINTF(1, ("address %s not IPv6 multicast address\n", stoa(maddr)));
			return (ISC_FALSE);
		}
		else
		{
			return (ISC_TRUE);
		}

/*
 * If we don't have IPV6 support any IPV6 address is not multicast
 */
#else
		return (ISC_FALSE);
#endif
	/*
	 * Never valid
	 */
	default:
		return (ISC_FALSE);
	}
}

/*
 * Multicast servers need to set the appropriate Multicast interface
 * socket option in order for it to know which interface to use for
 * send the multicast packet.
 */
void
enable_multicast_if(struct interface *iface, struct sockaddr_storage *maddr)
{
#ifdef MCAST
	switch (maddr->ss_family)
	{
	case AF_INET:
		if (setsockopt(iface->fd, IPPROTO_IP, IP_MULTICAST_IF,
		   (char *)&(((struct sockaddr_in*)&iface->sin)->sin_addr.s_addr),
		    sizeof(struct sockaddr_in*)) == -1) {
			netsyslog(LOG_ERR,
			"setsockopt IP_MULTICAST_IF failure: %m on socket %d, addr %s for multicast address %s",
			iface->fd, stoa(&iface->sin), stoa(maddr));
			return;
		}
		DPRINTF(1, ("Added IPv4 multicast interface on socket %d, addr %s for multicast address %s\n",
			    iface->fd, stoa(&iface->sin),
			    stoa(maddr)));
		break;

	case AF_INET6:
#ifdef INCLUDE_IPV6_MULTICAST_SUPPORT
		if (setsockopt(iface->fd, IPPROTO_IPV6, IPV6_MULTICAST_IF,
		    &iface->scopeid, sizeof(iface->scopeid)) == -1) {
			netsyslog(LOG_ERR,
			"setsockopt IPV6_MULTICAST_IF failure: %m on socket %d, addr %s, scope %d for multicast address %s",
			iface->fd, stoa(&iface->sin), iface->scopeid,
			stoa(maddr));
			return;
		}
		DPRINTF(1, ("Added IPv6 multicast interface on socket %d, addr %s, scope %d for multicast address %s\n",
			    iface->fd,  stoa(&iface->sin), iface->scopeid,
			    stoa(maddr)));
		break;
#else
		return;
#endif	/* INCLUDE_IPV6_MULTICAST_SUPPORT */
	}
	return;
#endif
}

/*
 * Add a multicast address to a given socket
 * The socket is in the inter_list all we need to do is enable
 * multicasting. It is not this function's job to select the socket
 */
static isc_boolean_t
socket_multicast_enable(struct interface *iface, int lscope, struct sockaddr_storage *maddr)
{
#ifdef INCLUDE_IPV6_MULTICAST_SUPPORT
	struct ipv6_mreq mreq6;
	struct in6_addr iaddr6;
#endif /* INCLUDE_IPV6_MULTICAST_SUPPORT */

	struct ip_mreq mreq;
	memset((char *)&mreq, 0, sizeof(mreq));

	switch (maddr->ss_family)
	{
	case AF_INET:
		mreq.imr_multiaddr = (((struct sockaddr_in*)maddr)->sin_addr);
/*
 * Temporarily just use INADDR_ANY for now
 * We should be checking if they're the same address
 * PDMXXX
 */
/*		mreq.imr_interface.s_addr = ((struct sockaddr_in*)&iface->sin)->sin_addr.s_addr; */
		mreq.imr_interface.s_addr = ((struct sockaddr_in*)&iface->sin)->sin_addr.s_addr;
		mreq.imr_interface.s_addr = htonl(INADDR_ANY); 
		if (setsockopt(iface->fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			(char *)&mreq, sizeof(mreq)) == -1) {
			netsyslog(LOG_ERR,
			"setsockopt IP_ADD_MEMBERSHIP failure: %m on socket %d, addr %s for %x / %x (%s)",
			iface->fd, stoa(&iface->sin),
			mreq.imr_multiaddr.s_addr,
			mreq.imr_interface.s_addr, stoa(maddr));
			return ISC_FALSE;
		}
		DPRINTF(1, ("Added IPv4 multicast membership on socket %d, addr %s for %x / %x (%s)\n",
			    iface->fd, stoa(&iface->sin),
			    mreq.imr_multiaddr.s_addr,
			    mreq.imr_interface.s_addr, stoa(maddr)));
		break;

	case AF_INET6:
#ifdef INCLUDE_IPV6_MULTICAST_SUPPORT
		/*
		 * Enable reception of multicast packets
		 * If the address is link-local we can get the interface index
		 * from the scope id. Don't do this for other types of multicast
		 * addresses. For now let the kernel figure it out.
		 */
		iaddr6 = ((struct sockaddr_in6*)maddr)->sin6_addr;
		mreq6.ipv6mr_multiaddr = iaddr6;
		mreq6.ipv6mr_interface = lscope;

		if (setsockopt(iface->fd, IPPROTO_IPV6, IPV6_JOIN_GROUP,
			(char *)&mreq6, sizeof(mreq6)) == -1) {
			netsyslog(LOG_ERR,
			 "setsockopt IPV6_JOIN_GROUP failure: %m on socket %d, addr %s for interface %d(%s)",
			iface->fd, stoa(&iface->sin),
			mreq6.ipv6mr_interface, stoa(maddr));
			return ISC_FALSE;
		}
		DPRINTF(1, ("Added IPv6 multicast group on socket %d, addr %s for interface %d(%s)\n",
			    iface->fd, stoa(&iface->sin),
			    mreq6.ipv6mr_interface, stoa(maddr)));
		break;
#else
		return ISC_FALSE;
#endif	/* INCLUDE_IPV6_MULTICAST_SUPPORT */
	}
	iface->flags |= INT_MCASTOPEN;
	iface->num_mcast++;
	add_addr_to_list(maddr, iface);
	return ISC_TRUE;
}

/*
 * Remove a multicast address from a given socket
 * The socket is in the inter_list all we need to do is disable
 * multicasting. It is not this function's job to select the socket
 */
static isc_boolean_t
socket_multicast_disable(struct interface *iface, struct sockaddr_storage *maddr)
{
#ifdef INCLUDE_IPV6_MULTICAST_SUPPORT
	struct ipv6_mreq mreq6;
	struct in6_addr iaddr6;
#endif /* INCLUDE_IPV6_MULTICAST_SUPPORT */

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
			"setsockopt IP_DROP_MEMBERSHIP failure: %m on socket %d, addr %s for %x / %x (%s)",
			iface->fd, stoa(&iface->sin),
			mreq.imr_multiaddr.s_addr,
			mreq.imr_interface.s_addr, stoa(maddr));
			return ISC_FALSE;
		}
		break;
	case AF_INET6:
#ifdef INCLUDE_IPV6_MULTICAST_SUPPORT
		/*
		 * Disable reception of multicast packets
		 * If the address is link-local we can get the interface index
		 * from the scope id. Don't do this for other types of multicast
		 * addresses. For now let the kernel figure it out.
		 */
		iaddr6 = ((struct sockaddr_in6*)&maddr)->sin6_addr;
		mreq6.ipv6mr_multiaddr = iaddr6;
		mreq6.ipv6mr_interface = iface->scopeid;

		if (setsockopt(iface->fd, IPPROTO_IPV6, IPV6_LEAVE_GROUP,
			(char *)&mreq6, sizeof(mreq6)) == -1) {
			netsyslog(LOG_ERR,
			"setsockopt IPV6_LEAVE_GROUP failure: %m on socket %d, addr %s for %d(%s)",
			iface->fd, stoa(&iface->sin),
			mreq6.ipv6mr_interface, stoa(maddr));
			return ISC_FALSE;
		}
		break;
#else
		return ISC_FALSE;
#endif	/* INCLUDE_IPV6_MULTICAST_SUPPORT */

	}
	iface->num_mcast--;
	if (iface->num_mcast <= 0) {
		iface->flags &= ~INT_MCASTOPEN;
	}
	return ISC_TRUE;
}

/*
 * io_setbclient - open the broadcast client sockets
 */
void
io_setbclient(void)
{
#ifdef OPEN_BCAST_SOCKET 
        struct interface *interf;
	int nif = 0;
	isc_boolean_t jstatus; 
	SOCKET fd;

	set_reuseaddr(1);

	for (interf = ISC_LIST_HEAD(inter_list);
	     interf != NULL;
	     interf = ISC_LIST_NEXT(interf, link)) {
	        if (interf->flags & INT_WILDCARD)
		        continue;
	  
		/* use only allowed addresses */
		if (interf->ignore_packets == ISC_TRUE)
			continue;
		/* Only IPv4 addresses are valid for broadcast */
		if (interf->sin.ss_family != AF_INET)
			continue;

		/* Is this a broadcast address? */
		if (!(interf->flags & INT_BROADCAST))
			continue;

		/* Skip the loopback addresses */
		if (interf->flags & INT_LOOPBACK)
			continue;

		/* Do we already have the broadcast address open? */
		if (interf->flags & INT_BCASTOPEN)
			continue;

		/*
		 * Try to open the broadcast address
		 */
		interf->family = AF_INET;
		interf->bfd = open_socket(&interf->bcast,
				    INT_BROADCAST, 1, interf);

		 /*
		 * If we succeeded then we use it otherwise
		 * enable the underlying address
		 */
		if (interf->bfd == INVALID_SOCKET) {
			fd = interf->fd;
		}
		else {
			fd = interf->bfd;
		}

		/* Enable Broadcast on socket */
		jstatus = socket_broadcast_enable(interf, fd, &interf->sin);
		if (jstatus == ISC_TRUE)
		{
			nif++;
			netsyslog(LOG_INFO,"io_setbclient: Opened broadcast client on interface #%d %s, socket: %d",
				  interf->ifnum, interf->name, fd);
		}
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
        struct interface *interf;
	isc_boolean_t lstatus;

	for (interf = ISC_LIST_HEAD(inter_list);
	     interf != NULL;
	     interf = ISC_LIST_NEXT(interf, link))
	{
	        if (interf->flags & INT_WILDCARD)
		    continue;
	  
		if (!(interf->flags & INT_BCASTOPEN))
		    continue;
		lstatus = socket_broadcast_disable(interf, &interf->sin);
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
        struct interface *interface, *iface;
	isc_boolean_t jstatus;
	int lscope = 0;
	
	/*
	 * Check to see if this is a multicast address
	 */
	if (addr_ismulticast(&addr) == ISC_FALSE)
		return;

	/* If we already have it we can just return */
	if (find_flagged_addr_in_list(&addr, INT_MCASTOPEN) != NULL)
	{
		netsyslog(LOG_INFO, "Duplicate request found for multicast address %s",
			stoa(&addr));
		return;
	}

#ifndef MULTICAST_NONEWSOCKET
	interface = new_interface(NULL);
	
	/*
	 * Open a new socket for the multicast address
	 */
	interface->sin.ss_family = addr.ss_family;
	interface->family = addr.ss_family;

	switch(addr.ss_family) {
	case AF_INET:
		memcpy(&(((struct sockaddr_in *)&interface->sin)->sin_addr),
		       &(((struct sockaddr_in*)&addr)->sin_addr),
		       sizeof(struct in_addr));
		((struct sockaddr_in*)&interface->sin)->sin_port = htons(NTP_PORT);
		memset(&((struct sockaddr_in*)&interface->mask)->sin_addr.s_addr, 0xff, sizeof(struct in_addr));
		break;
	case AF_INET6:
#ifdef INCLUDE_IPV6_MULTICAST_SUPPORT
		memcpy(&(((struct sockaddr_in6 *)&interface->sin)->sin6_addr),
		       &((struct sockaddr_in6*)&addr)->sin6_addr,
		       sizeof(struct in6_addr));
		((struct sockaddr_in6*)&interface->sin)->sin6_port = htons(NTP_PORT);
#ifdef ISC_PLATFORM_HAVESCOPEID
		((struct sockaddr_in6*)&interface->sin)->sin6_scope_id = ((struct sockaddr_in6*)&addr)->sin6_scope_id;
#endif
		memset(&((struct sockaddr_in6*)&interface->mask)->sin6_addr.s6_addr, 0xff, sizeof(struct in6_addr));
#endif
		iface = findlocalcastinterface(&addr, INT_MULTICAST);
		if (iface) {
			lscope = ((struct sockaddr_in6*)&iface->sin)->sin6_scope_id;
			DPRINTF(1, ("Found interface #%d %s, scope: %d for address %s\n", iface->ifnum, iface->name, lscope, stoa(&addr)));
		}
		break;
	}
		
	interface->bfd = INVALID_SOCKET;
	interface->fd = open_socket(&interface->sin,
			    INT_MULTICAST, 1, interface);

	if (interface->fd != INVALID_SOCKET)
	{
		interface->bfd = INVALID_SOCKET;
		interface->flags |= INT_MCASTIF;
		
		(void) strncpy(interface->name, "multicast",
			sizeof(interface->name));
		((struct sockaddr_in*)&interface->mask)->sin_addr.s_addr =
						htonl(~(u_int32)0);
#ifdef DEBUG
		if(debug > 1)
		  print_interface(interface, "multicast add ", "\n");
#endif
		add_interface(interface);
	}
	else
	{
	        delete_interface(interface);  /* re-use existing interface */
		interface = NULL;
		if (addr.ss_family == AF_INET)
			interface = wildipv4;
		else if (addr.ss_family == AF_INET6)
			interface = wildipv6;

		if (interface != NULL) {
			/* HACK ! -- stuff in an address */
			interface->bcast = addr;
			netsyslog(LOG_ERR,
			 "...multicast address %s using wildcard interface #%d %s",
				  stoa(&addr), interface->ifnum, interface->name);
		} else {
			netsyslog(LOG_ERR,
			"No multicast socket available to use for address %s",
			stoa(&addr));
			return;
		}
	}
#else
	/*
	 * For the case where we can't use a separate socket
	 */
	interface = findlocalcastinterface(&addr, INT_MULTICAST);
#endif
	/*
	 * If we don't have a valid socket, just return
	 */
	if (!interface)
	{
		netsyslog(LOG_ERR,
		"Cannot add multicast address %s: Cannot find slot",
		stoa(&addr));
		return;
	}

	jstatus = socket_multicast_enable(interface, lscope, &addr);

	if (jstatus == ISC_TRUE)
		netsyslog(LOG_INFO, "Added Multicast Listener %s on interface #%d %s\n", stoa(&addr), interface->ifnum, interface->name);
	else
		netsyslog(LOG_ERR, "Failed to add Multicast Listener %s\n", stoa(&addr));
#else /* MCAST */
	netsyslog(LOG_ERR,
		  "Cannot add multicast address %s: no Multicast support",
		  stoa(&addr));
#endif /* MCAST */
	return;
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
        struct interface *interface;
	isc_boolean_t lstatus;

	/*
	 * Check to see if this is a multicast address
	 */
	if (addr_ismulticast(&addr) == ISC_FALSE)
	{
		netsyslog(LOG_ERR,
			 "invalid multicast address %s", stoa(&addr));
		return;
	}

	switch (addr.ss_family)
	{
	case AF_INET :
		/*
		 * Disable reception of multicast packets
		 */
		interface = find_flagged_addr_in_list(&addr, INT_MCASTOPEN);
		while ( interface != NULL) {
			lstatus = socket_multicast_disable(interface, &addr);
			interface = find_flagged_addr_in_list(&addr, INT_MCASTOPEN);
		}
		break;

#ifdef INCLUDE_IPV6_MULTICAST_SUPPORT
	case AF_INET6 :
		/*
		 * Disable reception of multicast packets
		 */
		for (interface = ISC_LIST_HEAD(inter_list);
		     interface != NULL;
		     interface = ISC_LIST_NEXT(interface, link))
		{
                        if (interface->flags & INT_WILDCARD)
			        continue;
	  
			/* Be sure it's the correct family */
			if (interface->sin.ss_family != AF_INET6)
				continue;
			if (!(interface->flags & INT_MCASTOPEN))
				continue;
			if (!(interface->fd < 0))
				continue;
			if (!SOCKCMP(&addr, &interface->sin))
				continue;
			lstatus = socket_multicast_disable(interface, &addr);
		}
		break;
#endif /* INCLUDE_IPV6_MULTICAST_SUPPORT */

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
	struct interface *interf
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
				"bind() fd %d, family %d, port %d, addr %s, in_classd=%d flags=0x%x fails: %%m",
				fd, addr->ss_family, (int)ntohs(((struct sockaddr_in*)addr)->sin_port),
				stoa(addr),
				IN_CLASSD(ntohl(((struct sockaddr_in*)addr)->sin_addr.s_addr)), flags);
#ifdef INCLUDE_IPV6_SUPPORT
		else if(addr->ss_family == AF_INET6)
		                sprintf(buff,
                                "bind() fd %d, family %d, port %d, scope %d, addr %s, in6_is_addr_multicast=%d flags=%d fails: %%m",
                                fd, addr->ss_family, (int)ntohs(((struct sockaddr_in6*)addr)->sin6_port),
                                ((struct sockaddr_in6*)addr)->sin6_scope_id, stoa(addr),
                                IN6_IS_ADDR_MULTICAST(&((struct sockaddr_in6*)addr)->sin6_addr), flags);
#endif
		else
			return INVALID_SOCKET;

		/*
		 * Don't log this under all conditions
		 */
		if (turn_off_reuse == 0 || debug > 1)
			netsyslog(LOG_ERR, buff);

		closesocket(fd);

		return (INVALID_SOCKET);
	}
	DPRINTF(1, ("bind() fd %d, family %d, port %d, addr %s, flags=0x%x\n",
		   fd,
		   addr->ss_family,
		   (int)ntohs(((struct sockaddr_in*)addr)->sin_port),
		   stoa(addr),
		    flags));

	/*
	 * I/O Completion Ports don't care about the select and FD_SET
	 */
#ifndef HAVE_IO_COMPLETION_PORT
	if (fd > maxactivefd)
	    maxactivefd = fd;
	FD_SET(fd, &activefds);
#endif
	add_socket_to_list(fd);
	add_addr_to_list(addr, interf);
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
	DPRINTF(1, ("flags for fd %d: 0%o\n", fd,
		    fcntl(fd, F_GETFL, 0)));
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
	SOCKET i, newmax;

	if (fd < 0)
		return;
	
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
	delete_socket_from_list(fd);

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

	if (fd < 0)
		return;
	
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

#ifdef INCLUDE_IPV6_SUPPORT
	struct cache6 {
		u_short port;
		struct in6_addr addr;
	};
#endif /* INCLUDE_IPV6_SUPPORT */


#ifndef ERRORCACHESIZE
#define ERRORCACHESIZE 8
#endif
#if ERRORCACHESIZE > 0
	static struct cache badaddrs[ERRORCACHESIZE];
#ifdef INCLUDE_IPV6_SUPPORT
	static struct cache6 badaddrs6[ERRORCACHESIZE];
#endif /* INCLUDE_IPV6_SUPPORT */
#else
#define badaddrs ((struct cache *)0)		/* Only used in empty loops! */
#ifdef INCLUDE_IPV6_SUPPORT
#define badaddrs6 ((struct cache6 *)0)		/* Only used in empty loops! */
#endif /* INCLUDE_IPV6_SUPPORT */
#endif
#ifdef DEBUG
	if (debug > 1) 
	  {
	    if (inter != NULL) 
	      {
		printf("%ssendpkt(fd=%d dst=%s, src=%s, ttl=%d, len=%d)\n",
		       (ttl > 0) ? "\tMCAST\t***** " : "",
		       inter->fd, stoa(dest),
		       stoa(&inter->sin), ttl, len);
	      }
	    else
	      {
		printf("%ssendpkt(dst=%s, ttl=%d, len=%d): no interface - IGNORED\n",
		       (ttl > 0) ? "\tMCAST\t***** " : "",
		       stoa(dest),
		       ttl, len);
	      }
	  }
#endif

	if (inter == NULL)	/* unbound peer - drop request and wait for better network conditions */
	  return;
	
#ifdef MCAST

	switch (inter->sin.ss_family) {

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

#ifdef INCLUDE_IPV6_SUPPORT
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
#endif /* INCLUDE_IPV6_SUPPORT */

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
#ifdef INCLUDE_IPV6_SUPPORT
		else if (dest->ss_family == AF_INET6) {
			if (badaddrs6[slot].port == ((struct sockaddr_in6*)dest)->sin6_port &&
				badaddrs6[slot].addr.s6_addr == ((struct sockaddr_in6*)dest)->sin6_addr.s6_addr)
			break;
		}
#endif /* INCLUDE_IPV6_SUPPORT */
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

#ifdef INCLUDE_IPV6_SUPPORT
			case AF_INET6 :

				for (slot = ERRORCACHESIZE; --slot >= 0; )
        				if (badaddrs6[slot].port == 0)
            				{
                                    		badaddrs6[slot].port = SRCPORT(dest);
                                    		badaddrs6[slot].addr = ((struct sockaddr_in6*)dest)->sin6_addr;
                                    		break;
                            		}
                		break;
#endif /* INCLUDE_IPV6_SUPPORT */

			default :
				exit(1);
			}

			netsyslog(LOG_ERR, "sendto(%s) (fd=%d): %m",
				  stoa(dest), inter->fd);
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
#ifdef INCLUDE_IPV6_SUPPORT
			case AF_INET6 :
				badaddrs6[slot].port = 0;
				break;
#endif /* INCLUDE_IPV6_SUPPORT */
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

	/*
	 * List of fd's to skip the read
	 * We don't really expect to have this many
	 * refclocks on a system
	 */
#define MAXSKIPS  20
	int skiplist[MAXSKIPS];
	int totskips;
	isc_boolean_t skip;
	int nonzeroreads;
	int buflen;
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
	struct interface *interface;

	/*
	 * Initialize the skip list
	 */
	for (i = 0; i<MAXSKIPS; i++)
	{
		skiplist[i] = 0;
	}
	totskips = 0;

	handler_calls++;

	/*
	 * If we have something to do, freeze a timestamp.
	 * See below for the other cases (nothing (left) to do or error)
	 */
	ts = *cts;

	/*
	 * Do a poll to see who has data
	 */

	fds = activefds;
	tvzero.tv_sec = tvzero.tv_usec = 0;

#ifdef REFCLOCK
	/*
	 * Check out the reference clocks first, if any
	 */

	nonzeroreads = 1;
	while (nonzeroreads > 0)
	{
		nonzeroreads = 0;
		n = select(maxactivefd+1, &fds, (fd_set *)0, (fd_set *)0, &tvzero);

		/*
		 * If there are no packets waiting just return
		 */
		if (n <= 0)
			return;

		++select_count;
		++handler_pkts;

		if (refio != 0)
		{
			register struct refclockio *rp;

			for (rp = refio; rp != 0; rp = rp->next)
			{
				fd = rp->fd;
				skip = ISC_FALSE;
				/* Check for skips */
				for (i = 0; i < totskips; i++)
				{
					if (fd == skiplist[i])
					{
						skip = ISC_TRUE;
						break;
					}
				}
				/* fd was on skip list */
				if (skip == ISC_TRUE)
					continue;

				if (FD_ISSET(fd, &fds))
				{
					n--;
					if (free_recvbuffs() == 0)
					{
						char buf[RX_BUFF_SIZE];

						buflen = read(fd, buf, sizeof buf);
						packets_dropped++;
						if (buflen > 0)
							nonzeroreads++;
						else if (totskips < MAXSKIPS)
						{
							skiplist[totskips] = fd;
							totskips++;
						}
						continue;	/* Keep reading until drained */
					}

					rb = get_free_recv_buffer();

					i = (rp->datalen == 0
					    || rp->datalen > sizeof(rb->recv_space))
					    ? sizeof(rb->recv_space) : rp->datalen;
					buflen =
					    read(fd, (char *)&rb->recv_space, (unsigned)i);

					if (buflen < 0)
					{
						if (errno != EINTR && errno != EAGAIN) {
							netsyslog(LOG_ERR, "clock read fd %d: %m", fd);
						}
						freerecvbuf(rb);
						if (totskips < MAXSKIPS)
						{
							skiplist[totskips] = fd;
							totskips++;
						}
						continue;
					}
					if(buflen > 0)
						nonzeroreads++;
					else if (totskips < MAXSKIPS)
					{
						skiplist[totskips] = fd;
						totskips++;
					}
					/*
					 * Got one.  Mark how and when it got here,
					 * put it on the full list and do
					 * bookkeeping.
					 */
					rb->recv_length = buflen;
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
							 * data was consumed -
							 * nothing to pass up
							 * into block input
							 * machine
							 */
							freerecvbuf(rb);
							continue;
						}
					}
	
					add_full_recv_buffer(rb);

					rp->recvcount++;
					packets_received++;
				} /* End if (FD_ISSET(fd, &fds)) */
			} /* End for (rp = refio; rp != 0 && n > 0; rp = rp->next) */
		} /* End if (refio != 0) */
	} /* End while (nonzeroreads > 0) */

#endif /* REFCLOCK */

	/*
	 * Loop through the interfaces looking for data
	 * to read.
	 */
	for (interface = ISC_LIST_TAIL(inter_list);
	     interface != NULL;
	     interface = ISC_LIST_PREV(interface, link))
	{
		for (doing = 0; (doing < 2); doing++)
		{
			if (doing == 0)
			{
				fd = interface->fd;
			}
			else
			{
				if (!(interface->flags & INT_BCASTOPEN))
				    break;
				fd = interface->bfd;
			}
			if (fd < 0) continue;
			if (FD_ISSET(fd, &fds))
			{
				n--;

				/*
				 * Get a buffer and read the frame.  If we
				 * haven't got a buffer, or this is received
				 * on a disallowed socket, just dump the
				 * packet.
				 */

				if (free_recvbuffs() == 0 ||
				    interface->ignore_packets == ISC_TRUE)
				{
					char buf[RX_BUFF_SIZE];
					struct sockaddr_storage from;

					fromlen = sizeof from;
					(void) recvfrom(fd, buf, sizeof(buf), 0,
							(struct sockaddr*)&from, &fromlen);
					DPRINTF(4, ("%s on %d(%lu) fd=%d from %s\n",
						    (interface->ignore_packets == ISC_TRUE) ? "ignore" : "drop",
						    i, free_recvbuffs(), fd,
						    stoa(&from)));

					if (interface->ignore_packets == ISC_TRUE)
					    packets_ignored++;
					else
					    packets_dropped++;
					continue;
				}

				rb = get_free_recv_buffer();

				fromlen = sizeof(struct sockaddr_storage);
				rb->recv_length = recvfrom(fd,
						  (char *)&rb->recv_space,
						   sizeof(rb->recv_space), 0,
						   (struct sockaddr *)&rb->recv_srcadr,
						   &fromlen);
				if (rb->recv_length == 0
				 || (rb->recv_length == -1 && 
				    (errno==EWOULDBLOCK
#ifdef EAGAIN
				     || errno==EAGAIN
#endif
					    ))) {
					freerecvbuf(rb);
					continue;
				}
				else if (rb->recv_length < 0)
				{
					netsyslog(LOG_ERR, "recvfrom(%s) fd=%d: %m",
 					stoa(&rb->recv_srcadr), fd);

					DPRINTF(0, ("input_handler: fd=%d dropped (bad recvfrom)\n", fd));

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
				rb->dstadr = interface;
				rb->fd = fd;
				rb->recv_time = ts;
				rb->receiver = receive;

				add_full_recv_buffer(rb);

				interface->received++;
				packets_received++;
				continue;
			}
		/* Check more interfaces */
		}
	}

	/*
	 * Done everything from that select.
	 */

	/*
	 * If nothing to do, just return.
	 * If an error occurred, complain and return.
	 */
	if (n == 0)
	{
		if (select_count == 0) /* We really had nothing to do */
		{
#ifdef DEBUG
			if (debug)
			    netsyslog(LOG_DEBUG, "input_handler: select() returned 0");
#endif
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
#ifdef DEBUG
		if (debug > 3)
		    netsyslog(LOG_INFO, "input_handler: Processed a gob of fd's in %s msec", lfptoms(&ts_e, 6));
#endif

		/* just bail. */
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
			    if ((FD_ISSET(j, &fds) && (read(j, &b, 0) == -1)))
				netsyslog(LOG_ERR, "Bad file descriptor %d", j);
		}
		return;
	}
	return;
}

#endif

/*
 * findinterface - find local interface corresponding to address
 */
struct interface *
findinterface(
	struct sockaddr_storage *addr
	)
{
	struct interface *interface;
	
	interface = findlocalinterface(addr);

	if (interface == NULL)
	{
		DPRINTF(2, ("Found no interface for address %s - returning wildcard\n",
			    stoa(addr)));

		return (ANY_INTERFACE_CHOOSE(addr));
	}
	else
	{
		DPRINTF(2, ("Found interface #%d %s for address %s\n",
			    interface->ifnum, interface->name, stoa(addr)));

		return (interface);
	}
}

/*
 * findlocalinterface - find local interface index corresponding to address
 */
struct interface *
findlocalinterface(
	struct sockaddr_storage *addr
	)
{
	SOCKET s;
	int rtn;
	struct sockaddr_storage saddr;
	int saddrlen = SOCKLEN(addr);

	DPRINTF(2, ("Finding interface for addr %s in list of addresses\n",
		    stoa(addr));)

	/*
	 * This is considerably hoke. We open a socket, connect to it
	 * and slap a getsockname() on it. If anything breaks, as it
	 * probably will in some j-random knockoff, we just return the
	 * NULL
	 */
	memset(&saddr, 0, sizeof(saddr));
	saddr.ss_family = addr->ss_family;
	if(addr->ss_family == AF_INET) {
		memcpy(&((struct sockaddr_in*)&saddr)->sin_addr, &((struct sockaddr_in*)addr)->sin_addr, sizeof(struct in_addr));
		((struct sockaddr_in*)&saddr)->sin_port = htons(2000);
	}
#ifdef INCLUDE_IPV6_SUPPORT
	else if(addr->ss_family == AF_INET6) {
 		memcpy(&((struct sockaddr_in6*)&saddr)->sin6_addr, &((struct sockaddr_in6*)addr)->sin6_addr, sizeof(struct in6_addr));
		((struct sockaddr_in6*)&saddr)->sin6_port = htons(2000);
		((struct sockaddr_in6*)&saddr)->sin6_scope_id = ((struct sockaddr_in6*)addr)->sin6_scope_id;
	}
#endif
	
	((struct sockaddr_in*)&saddr)->sin_port = htons(2000);
	s = socket(addr->ss_family, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET)
		return NULL;

	rtn = connect(s, (struct sockaddr *)&saddr, SOCKLEN(&saddr));
#ifndef SYS_WINNT
	if (rtn < 0)
#else
	if (rtn == SOCKET_ERROR)
#endif
	{
		closesocket(s);
		return NULL;
	}

	rtn = getsockname(s, (struct sockaddr *)&saddr, &saddrlen);
	closesocket(s);
#ifndef SYS_WINNT
	if (rtn < 0)
#else
	if (rtn == SOCKET_ERROR)
#endif
		return NULL;

	DPRINTF(2, ("findlocalinterface: kernel maps %s to %s\n", stoa(addr), stoa(&saddr)));
	
	return find_iface(&saddr, NULL);
}

static struct interface *
find_iface(struct sockaddr_storage *saddr, char *ifname)
{
        struct interface *interface;
    
	for (interface = ISC_LIST_HEAD(inter_list);
	     interface != NULL;
	     interface = ISC_LIST_NEXT(interface, link)) 
	  {
	        if (interface->flags & INT_WILDCARD)
		        continue;
	  
		/* Don't both with ignore interfaces */
		if (interface->ignore_packets == ISC_TRUE)
			continue;
		/*
		 * First look if is the the correct family
		 */
		if(interface->sin.ss_family != saddr->ss_family)
	  		continue;
		/*
		 * We match the unicast address only.
		 */
		if (SOCKCMP(&interface->sin, saddr) &&
		    ((ifname != NULL) ? (0 == strcmp(ifname, interface->name)) : 1))
		{
			break;
		}
	  }
	return interface;
}
 
/*
 * findlocalcastinterface - find local *cast interface index corresponding to address
 * depending on the flags passed
 */
static struct interface *
findlocalcastinterface(
	struct sockaddr_storage *addr, int flags
	)
{
        struct interface *interface;
	struct interface *nif = NULL;

#ifdef INCLUDE_IPV6_MULTICAST_SUPPORT
	isc_boolean_t want_linklocal = ISC_FALSE; 
	if (addr_ismulticast(addr) && flags == INT_MULTICAST)
	{
		if (IN6_IS_ADDR_MC_LINKLOCAL(&((struct sockaddr_in6*)addr)->sin6_addr))
		{
			want_linklocal = ISC_TRUE;
		}
		else if (IN6_IS_ADDR_MC_SITELOCAL(&((struct sockaddr_in6*)addr)->sin6_addr))
		{
			want_linklocal = ISC_TRUE;
		}
	}
#endif

	for (interface = ISC_LIST_HEAD(inter_list);
	     interface != NULL;
	     interface = ISC_LIST_NEXT(interface, link)) 
	  {
		/* use only allowed addresses */
		if (interface->ignore_packets == ISC_TRUE)
			continue;

		/* Skip the loopback and wildcard addresses */
		if (interface->flags & (INT_LOOPBACK|INT_WILDCARD))
			continue;

		/* Skip if different family */
		if(interface->sin.ss_family != addr->ss_family)
			continue;

		/* Is this it one of these based on flags? */
		if (!(interface->flags & flags))
			continue;

		/* for IPv6 multicast check the address for linklocal */
#ifdef INCLUDE_IPV6_MULTICAST_SUPPORT
		if (flags == INT_MULTICAST && interface->sin.ss_family == AF_INET6 &&
		   (IN6_IS_ADDR_LINKLOCAL(&((struct sockaddr_in6*)&interface->sin)->sin6_addr))
		   && want_linklocal == ISC_TRUE)
		{
			nif = interface;
			break;
		}
		/* If we want a linklocal address and this isn't it, skip */\
		if (want_linklocal == ISC_TRUE)
			continue;
#endif
		/* Otherwise just look for the flag */
		if((interface->flags & flags))
		{
			nif = interface;
			break;
		}
	}
#ifdef DEBUG
	if (debug > 1) 
	{
		if (nif)
			printf("findlocalcastinterface: found interface #%d %s\n", nif->ifnum, nif->name);
		else
			printf("findlocalcastinterface: no interface found for %s flags 0x%x\n", stoa(addr), flags);
	}
#endif
	return (nif);
}

/*
 * findbcastinter - find broadcast interface corresponding to address
 */
struct interface *
findbcastinter(
	struct sockaddr_storage *addr
	)
{
#if !defined(MPE) && (defined(SIOCGIFCONF) || defined(SYS_WINNT))
        struct interface *interface;
	
	
	DPRINTF(3, ("Finding broadcast interface for addr %s in list of addresses\n",
		    stoa(addr)));

	interface = find_flagged_addr_in_list(addr, INT_BCASTOPEN|INT_MCASTOPEN);
	
#ifdef DEBUG
	if (interface) {
		DPRINTF(2, ("Found bcastinter index #%d %s\n", interface->ifnum, interface->name));
	} else {
		DPRINTF(2, ("No bcast interface found for %s\n", stoa(addr)));
	}
#endif
	/*
	 * Do nothing right now
	 * Eventually we will find the interface this
	 * way, but until it works properly we just see
	 * which one we got
	 */
/*	if (interface != NULL)
	{
		return (interface);
	}
*/

	for (interface = ISC_LIST_HEAD(inter_list);
	     interface != NULL;
	     interface = ISC_LIST_NEXT(interface, link)) 
	  {
	        if (interface->flags & INT_WILDCARD)
		        continue;
	  
		/* Don't bother with ignored interfaces */
		if (interface->ignore_packets == ISC_TRUE)
			continue;

		/*
		 * First look if this is the correct family
		 */
		if(interface->sin.ss_family != addr->ss_family)
	  		continue;

		/* Skip the loopback addresses */
		if (interface->flags & INT_LOOPBACK)
			continue;

		/* for IPv6 multicast check the address for linklocal */
#ifdef INCLUDE_IPV6_SUPPORT
		if (interface->sin.ss_family == AF_INET6 &&
		   (IN6_IS_ADDR_LINKLOCAL(&((struct sockaddr_in6*)&interface->sin)->sin6_addr)))
		{
/*			continue; */
		}
#endif
		/*
		 * If we are looking to match a multicast address grab it.
		 * We must not do this before we have eliminated any linklocal
		 * addresses
		 */
		if (addr_ismulticast(addr) == ISC_TRUE && interface->flags & INT_MULTICAST)
		{
			return (interface);
		}

		/*
		 * We match only those interfaces marked as
		 * broadcastable and either the explicit broadcast
		 * address or the network portion of the IP address.
		 * Sloppy.
		 */
		if(addr->ss_family == AF_INET) {
			if (SOCKCMP(&interface->bcast, addr))
				return interface;
			if ((NSRCADR(&interface->sin) &
				NSRCADR(&interface->mask)) == (NSRCADR(addr) &
			    	NSRCADR(&interface->mask)))
				return interface;
		}
#ifdef INCLUDE_IPV6_SUPPORT
		else if(addr->ss_family == AF_INET6) {
			if (SOCKCMP(&interface->bcast, addr))
				return interface;
			if (SOCKCMP(netof(&interface->sin), netof(addr)))
				return interface;
		}
#endif
	}
#endif /* SIOCGIFCONF */
 	return ANY_INTERFACE_CHOOSE(addr);
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
			return;
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
void
add_addr_to_list(struct sockaddr_storage *addr, struct interface *interface){
	remaddr_t *laddr = (remaddr_t *)emalloc(sizeof(remaddr_t));
	memcpy(&laddr->addr, addr, sizeof(struct sockaddr_storage));
	laddr->interface = interface;

	ISC_LIST_APPEND(remoteaddr_list, laddr, link);

	DPRINTF(1, ("Added addr %s to list of addresses\n",
		    stoa(addr)));
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
	DPRINTF(1, ("Deleted addr %s from list of addresses\n",
		    stoa(addr)));
}

struct interface *
find_addr_in_list(struct sockaddr_storage *addr) {

	remaddr_t *next;
	remaddr_t *laddr = ISC_LIST_HEAD(remoteaddr_list);
	DPRINTF(1, ("Finding addr %s in list of addresses\n",
		    stoa(addr)));

	while(laddr != NULL) {
		next = ISC_LIST_NEXT(laddr, link);
		if(SOCKCMP(&laddr->addr, addr)) {
			return laddr->interface;
			break;
		}
		else
			laddr = next;
	}
	return NULL; /* Not found */
}

/*
 * Find the given address with the associated flag in the list
 */
struct interface *
find_flagged_addr_in_list(struct sockaddr_storage *addr, int flag) {

	remaddr_t *next;
	remaddr_t *laddr = ISC_LIST_HEAD(remoteaddr_list);
	DPRINTF(1, ("Finding addr %s in list of addresses\n",
		    stoa(addr)));

	while(laddr != NULL) {
		next = ISC_LIST_NEXT(laddr, link);
		if(SOCKCMP(&laddr->addr, addr) && (laddr->interface->flags & flag)) {
			return laddr->interface;
			break;
		}
		else
			laddr = next;
	}
	return NULL; /* Not found */
}
