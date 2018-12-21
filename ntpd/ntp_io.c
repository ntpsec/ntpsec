/*
 * ntp_io.c - input/output routines for ntpd.
 */

#include "config.h"

#include <stdio.h>
#include <signal.h>
#include <fnmatch.h>
#if !defined(FNM_CASEFOLD) && defined(FNM_IGNORECASE)
# define FNM_CASEFOLD FNM_IGNORECASE
#endif
#include <sys/uio.h>

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_lists.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"
#include "ntp_assert.h"
#include "ntp_dns.h"
#include "timespecops.h"

#include "isc_interfaceiter.h"
#include "isc_netaddr.h"

#ifdef HAVE_NET_ROUTE_H
# define USE_ROUTING_SOCKET
# include <net/route.h>
# ifdef HAVE_LINUX_RTNETLINK_H
#  include <linux/rtnetlink.h>
# endif
#endif

/* From ntp_request.h - after nuking ntpdc */
#define IFS_EXISTS      1       /* just exists */
#define IFS_CREATED     2       /* was just created */
#define IFS_DELETED     3       /* was just delete */

#ifndef IPTOS_DSCP_EF
#define IPTOS_DSCP_EF 0xb8
#endif
int qos = IPTOS_DSCP_EF;	/* QoS RFC 3246 */


/*
 * NIC rule entry
 */
typedef struct nic_rule_tag nic_rule;

struct nic_rule_tag {
	nic_rule *	next;
	nic_rule_action	action;
	nic_rule_match	match_type;
	char *		if_name;
	sockaddr_u	addr;
	int		prefixlen;
};

/*
 * NIC rule listhead.  Entries are added at the head so that the first
 * match in the list is the last matching rule specified.
 */
static nic_rule *nic_rule_list;

/*
 * This code is a remnant from when ntpd did asynchronous input using
 * the SIGIO facility.  Most of that complexity is gone now.
 *
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
volatile struct packet_counters pkt_count;

/*
 * Interface stuff
 */
endpt *	any_interface;		/* wildcard ipv4 interface */
endpt *	any6_interface;		/* wildcard ipv6 interface */
endpt *	loopback_interface;	/* loopback ipv4 interface */

unsigned int sys_ifnum;			/* next .ifnum to assign */
static int ninterfaces;			/* total # of interfaces */

bool disable_dynamic_updates;	/* if true, scan interfaces once only */
extern  SOCKET  open_socket     (sockaddr_u *, bool, endpt *);

static bool
netaddr_eqprefix(const isc_netaddr_t *, const isc_netaddr_t *,
                    unsigned int) __attribute__((pure));

/* Socket Address */
typedef struct isc_sockaddr {
	union {
		struct sockaddr		sa;
		struct sockaddr_in	sin;
		struct sockaddr_in6	sin6;
	}				type;
	unsigned int			length;		/* XXXRTH beginning? */
        struct { struct isc_sockaddr *prev, *next; } link;
} isc_sockaddr_t;

static void
netaddr_fromsockaddr(isc_netaddr_t *netaddr, const isc_sockaddr_t *source);


#ifdef REFCLOCK
/*
 * Refclock stuff.	We keep a chain of structures with data concerning
 * the guys we are doing I/O for.
 */
static	struct refclockio *refio;
#endif /* REFCLOCK */

/*
 * File descriptor masks etc. for call to select
 * Not needed for I/O Completion Ports or anything outside this file
 */
static fd_set activefds;
static int maxactivefd;

/*
 * bit alternating value to detect verified interfaces during an update cycle
 */
static unsigned short		sys_interphase = 0;

static void	add_interface(endpt *);
static bool	update_interfaces(unsigned short, interface_receiver_t,
				  void *);
static void	remove_interface(endpt *);
static endpt *	create_interface(unsigned short, endpt *);

static bool	is_wildcard_addr	(const sockaddr_u *);

/*
 * Multicast functions
 */
static	bool	is_anycast		(sockaddr_u *,
						 const char *);

#ifdef DEBUG
static void interface_dump	(const endpt *);
static void sockaddr_dump	(const sockaddr_u *);
static void print_interface	(const endpt *, const char *, const char *);
#define DPRINT_INTERFACE(level, args) do { if (debug >= (level)) { print_interface args; } } while (0)
#else
#define DPRINT_INTERFACE(level, args) do {} while (0)
#endif

typedef struct vsock vsock_t;
enum desc_type { FD_TYPE_SOCKET, FD_TYPE_FILE };

struct vsock {
	vsock_t	*	link;
	SOCKET		fd;
	enum desc_type	type;
};

static vsock_t	*fd_list;

#if defined(USE_ROUTING_SOCKET)
/*
 * async notification processing (e. g. routing sockets)
 */
/*
 * support for receiving data on fd that is not a refclock or a socket
 * like e. g. routing sockets
 */
struct asyncio_reader {
	struct asyncio_reader *link;		    /* the list this is being kept in */
	SOCKET fd;				    /* fd to be read */
	void  *data;				    /* possibly local data */
	void (*receiver)(struct asyncio_reader *);  /* input handler */
};

static struct asyncio_reader *asyncio_reader_list;

static void delete_asyncio_reader (struct asyncio_reader *);
static struct asyncio_reader *new_asyncio_reader (void);
static void add_asyncio_reader (struct asyncio_reader *, enum desc_type);
static void remove_asyncio_reader (struct asyncio_reader *);

#endif /* defined(USE_ROUTING_SOCKET) */

static void init_async_notifications (void);

static	bool	addr_eqprefix	(const sockaddr_u *, const sockaddr_u *,
				 int);
static	int	create_sockets	(unsigned short);
static	void	set_reuseaddr	(int);

typedef struct remaddr remaddr_t;

struct remaddr {
	remaddr_t *		link;
	sockaddr_u		addr;
	endpt *			ep;
};

static remaddr_t * remoteaddr_list;
endpt *		ep_list;	/* complete endpt list */

static endpt *	wildipv4;
static endpt *	wildipv6;

static const int accept_wildcard_if_for_winnt = false;

static void	add_fd_to_list		(SOCKET, enum desc_type);
static endpt *	find_addr_in_list	(sockaddr_u *);
static void	delete_interface_from_list(endpt *);
static void	close_and_delete_fd_from_list(SOCKET);
static void	add_addr_to_list	(sockaddr_u *, endpt *);
static void	create_wildcards	(unsigned short);
static endpt *	findlocalinterface	(sockaddr_u *, int, int);
static endpt *	findclosestinterface	(sockaddr_u *, int);

#ifdef DEBUG
static const char *	action_text	(nic_rule_action);
#endif
static nic_rule_action	interface_action(char *, sockaddr_u *, uint32_t);
static void		convert_isc_if	(isc_interface_t *,
					 endpt *, unsigned short);
static void		calc_addr_distance(sockaddr_u *,
					   const sockaddr_u *,
					   const sockaddr_u *);
static int		cmp_addr_distance(const sockaddr_u *,
					  const sockaddr_u *);
static void		maintain_activefds(int fd, bool closing);

/*
 * Routines to read the ntp packets
 */
static int	read_network_packet	(SOCKET, endpt *);
static void input_handler (fd_set *);
#ifdef REFCLOCK
static int	read_refclock_packet	(SOCKET, struct refclockio *);
#endif

/*
 * Flags from signal handlers
 */
volatile struct signals_detected sig_flags = {
    .sawALRM = false,
    .sawHUP = false,
    .sawDNS = false,
    .sawQuit = false  /* SIGQUIT, SIGINT, SIGTERM */
};
static sigset_t blockMask;

void
maintain_activefds(
	int fd,
	bool closing
	)
{
	int i;

	if (fd < 0 || fd >= (int)FD_SETSIZE) {
		msyslog(LOG_ERR,
			"IO: Too many sockets in use, FD_SETSIZE %d exceeded by fd %d",
			FD_SETSIZE, fd);
		exit(1);
	}

	if (!closing) {
		FD_SET(fd, &activefds);
		maxactivefd = max(fd, maxactivefd);
	} else {
		FD_CLR(fd, &activefds);
		if (maxactivefd && fd == maxactivefd) {
			for (i = maxactivefd - 1; i >= 0; i--)
				if (FD_ISSET(i, &activefds)) {
					maxactivefd = i;
					break;
				}
			INSIST(fd != maxactivefd);
		}
	}
}


/*
 * About dynamic interfaces, sockets, reception and more...
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
 *   - on some systems it is perfectly legal to assign the same
 *     address to multiple interfaces. Therefore this code does not
 *     keep a list of interfaces but a list of interfaces that
 *     represent a unique address as determined by the kernel by the
 *     procedure in findlocalinterface. Thus it is perfectly legal to
 *     see only one representative of a group of real interfaces if
 *     they share the same address.
 *
 * Frank Kardel 20050910
 */

/*
 * init_io - initialize I/O module.
 */
void
init_io(void)
{
	/* Init buffer free list and stat counters */
	init_recvbuff(RECV_INIT);
	/* update interface every 5 minutes as default */
	interface_interval = 300;

	sigemptyset(&blockMask);
	sigaddset(&blockMask, SIGALRM);
	sigaddset(&blockMask, MOREDEBUGSIG);
	sigaddset(&blockMask, LESSDEBUGSIG);
	sigaddset(&blockMask, SIGINT);
	sigaddset(&blockMask, SIGQUIT);
	sigaddset(&blockMask, SIGTERM);
	sigaddset(&blockMask, SIGHUP);

}


/*
 * io_open_sockets - call socket creation routine
 */
void
io_open_sockets(void)
{
	static bool already_opened = false;

	if (already_opened)
		return;

	already_opened = true;

	/*
	 * Create the sockets
	 */
	create_sockets(NTP_PORT);

	init_async_notifications();

	DPRINT(3, ("io_open_sockets: maxactivefd %d\n", maxactivefd));
}


#ifdef DEBUG
/*
 * function to dump the contents of the interface structure
 * for debugging use only.
 */
void
interface_dump(const endpt *itf)
{
	printf("Dumping interface: %p\n", itf);
	printf("fd = %d\n", itf->fd);
	printf("sin = %s,\n", socktoa(&itf->sin));
	sockaddr_dump(&itf->sin);
	printf("bcast = %s,\n", socktoa(&itf->bcast));
	sockaddr_dump(&itf->bcast);
	printf("mask = %s,\n", socktoa(&itf->mask));
	sockaddr_dump(&itf->mask);
	printf("name = %s\n", itf->name);
	printf("flags = 0x%08x\n", itf->flags);
	printf("addr_refid = %08x\n", itf->addr_refid);
	printf("received = %ld\n", itf->received);
	printf("sent = %ld\n", itf->sent);
	printf("notsent = %ld\n", itf->notsent);
	printf("ifindex = %u\n", itf->ifindex);
	printf("peercnt = %u\n", itf->peercnt);
	printf("phase = %u\n", itf->phase);
}

/*
 * sockaddr_dump - hex dump the start of a sockaddr_u
 */
static void
sockaddr_dump(const sockaddr_u *psau)
{
	/* Limit the size of the sockaddr_in6 hex dump */
	const int maxsize = min(32, sizeof(psau->sa6));
	const uint8_t *	cp;

	/* XXX: Should we limit maxsize based on psau->saX.sin_family? */
	cp = (const void *)&psau->sa6;

	for(int i = 0; i < maxsize; i++) {
		printf("%02x", *cp++);
		if (!((i + 1) % 4))
			printf(" ");
	}
	printf("\n");
}

/*
 * print_interface - helper to output debug information
 */
static void
print_interface(const endpt *iface, const char *pfx, const char *sfx)
{
	printf("%sinterface #%u: fd=%d, name=%s, "
               "flags=0x%x, ifindex=%u, sin=%s",
	       pfx,
	       iface->ifnum,
	       iface->fd,
	       iface->name,
	       iface->flags,
	       iface->ifindex,
	       socktoa(&iface->sin));
	if (AF_INET == iface->family) {
		if (iface->flags & INT_BROADCAST)
			printf(", bcast=%s", socktoa(&iface->bcast));
		printf(", mask=%s", socktoa(&iface->mask));
	}
	printf(", %s:%s",
	       (iface->ignore_packets)
		   ? "Disabled"
		   : "Enabled",
	       sfx);
	if (debug > 4)	/* in-depth debugging only */ /* SPECIAL DEBUG */
		interface_dump(iface);
}
#endif

#if defined(USE_ROUTING_SOCKET)
/*
 * create an asyncio_reader structure
 */
static struct asyncio_reader *
new_asyncio_reader(void)
{
	struct asyncio_reader *reader;

	reader = emalloc_zero(sizeof(*reader));
	reader->fd = INVALID_SOCKET;

	return reader;
}

/*
 * delete a reader
 */
static void
delete_asyncio_reader(
	struct asyncio_reader *reader
	)
{
	free(reader);
}

/*
 * add asynchio_reader
 */
static void
add_asyncio_reader(
	struct asyncio_reader *	reader,
	enum desc_type		type)
{
	LINK_SLIST(asyncio_reader_list, reader, link);
	add_fd_to_list(reader->fd, type);
}

/*
 * remove asynchio_reader
 */
static void
remove_asyncio_reader(
	struct asyncio_reader *reader
	)
{
	struct asyncio_reader *unlinked;

	UNLINK_SLIST(unlinked, asyncio_reader_list, reader, link,
	    struct asyncio_reader);

	if (reader->fd != INVALID_SOCKET)
		close_and_delete_fd_from_list(reader->fd);

	reader->fd = INVALID_SOCKET;
}
#endif /* defined(USE_ROUTING_SOCKET) */

static void
netaddr_fromsockaddr(isc_netaddr_t *t, const isc_sockaddr_t *s) {
	int family = s->type.sa.sa_family;
	t->family = (unsigned int)family;
	switch (family) {
	case AF_INET:
		t->type.in = s->type.sin.sin_addr;
		t->zone = 0;
		break;
	case AF_INET6:
		memcpy(&t->type.in6, &s->type.sin6.sin6_addr, 16);
		t->zone = s->type.sin6.sin6_scope_id;
		break;
	default:
		INSIST(0);
	}
}

static bool
netaddr_eqprefix(const isc_netaddr_t *a, const isc_netaddr_t *b,
		     unsigned int prefixlen)
{
	const unsigned char *pa = NULL, *pb = NULL;
	unsigned int ipabytes = 0; /* Length of whole IP address in bytes */
	unsigned int nbytes;       /* Number of significant whole bytes */
	unsigned int nbits;        /* Number of significant leftover bits */

	REQUIRE(a != NULL && b != NULL);

	if (a->family != b->family)
		return (false);

	if (a->zone != b->zone && b->zone != 0)
		return (false);

	switch (a->family) {
	case AF_INET:
		pa = (const unsigned char *) &a->type.in;
		pb = (const unsigned char *) &b->type.in;
		ipabytes = 4;
		break;
	case AF_INET6:
		pa = (const unsigned char *) &a->type.in6;
		pb = (const unsigned char *) &b->type.in6;
		ipabytes = 16;
		break;
	default:
		return (false);
	}

	/*
	 * Don't crash if we get a pattern like 10.0.0.1/9999999.
	 */
	if (prefixlen > ipabytes * 8)
		prefixlen = ipabytes * 8;

	nbytes = prefixlen / 8;
	nbits = prefixlen % 8;

	if (nbytes > 0) {
		if (memcmp(pa, pb, nbytes) != 0)
			return (false);
	}
	if (nbits > 0) {
		unsigned int bytea, byteb, mask;
		INSIST(nbytes < ipabytes);
		INSIST(nbits < 8);
		bytea = pa[nbytes];
		byteb = pb[nbytes];
		mask = (0xFF << (8-nbits)) & 0xFF;
		if ((bytea & mask) != (byteb & mask))
			return (false);
	}
	return (true);
}


/* compare two sockaddr prefixes */
static bool
addr_eqprefix(
	const sockaddr_u *	a,
	const sockaddr_u *	b,
	int			prefixlen
	)
{
	isc_netaddr_t		isc_a;
	isc_netaddr_t		isc_b;
	isc_sockaddr_t		isc_sa;

	ZERO(isc_sa);
	memcpy(&isc_sa.type, a, min(sizeof(isc_sa.type), sizeof(*a)));
	netaddr_fromsockaddr(&isc_a, &isc_sa);

	ZERO(isc_sa);
	memcpy(&isc_sa.type, b, min(sizeof(isc_sa.type), sizeof(*b)));
	netaddr_fromsockaddr(&isc_b, &isc_sa);

	return netaddr_eqprefix(&isc_a, &isc_b,
					 (unsigned int)prefixlen);
}


/*
 * Code to tell if we have an IP address
 * If we have then return the sockaddr structure
 * and set the return value
 * see the bind9/getaddresses.c for details
 */
bool
is_ip_address(
	const char *	host,
	unsigned short	af,
	sockaddr_u *	addr
	)
{
	struct in_addr in4;
	struct addrinfo hints;
	struct addrinfo *result;
	struct sockaddr_in6 *resaddr6;
	char tmpbuf[128];
	char *pch;

	REQUIRE(host != NULL);
	REQUIRE(addr != NULL);

	ZERO_SOCK(addr);

	/*
	 * Try IPv4, then IPv6.  In order to handle the extended format
	 * for IPv6 scoped addresses (address%scope_ID), we'll use a local
	 * working buffer of 128 bytes.  The length is an ad-hoc value, but
	 * should be enough for this purpose; the buffer can contain a string
	 * of at least 80 bytes for scope_ID in addition to any IPv6 numeric
	 * addresses (up to 46 bytes), the delimiter character and the
	 * terminating NULL character.
	 */
	if (AF_UNSPEC == af || AF_INET == af)
		if (inet_pton(AF_INET, host, &in4) == 1) {
			AF(addr) = AF_INET;
			SET_ADDR4N(addr, in4.s_addr);

			return true;
		}

	if (AF_UNSPEC == af || AF_INET6 == af)
		if (sizeof(tmpbuf) > strlen(host)) {
			if ('[' == host[0]) {
				strlcpy(tmpbuf, &host[1], sizeof(tmpbuf));
				pch = strchr(tmpbuf, ']');
				if (pch != NULL)
					*pch = '\0';
			} else {
				strlcpy(tmpbuf, host, sizeof(tmpbuf));
			}
			ZERO(hints);
			hints.ai_family = AF_INET6;
			hints.ai_flags |= AI_NUMERICHOST;
			if (getaddrinfo(tmpbuf, NULL, &hints, &result) == 0) {
				AF(addr) = AF_INET6;
				resaddr6 = (struct sockaddr_in6 *)result->ai_addr;
				SET_ADDR6N(addr, resaddr6->sin6_addr);
				SET_SCOPE(addr, resaddr6->sin6_scope_id);

				freeaddrinfo(result);
				return true;
			}
		}
	/*
	 * If we got here it was not an IP address
	 */
	return false;
}


/*
 * do standard initialization of interface structure
 */
static void
init_interface(
	endpt *ep
	)
{
	ZERO(*ep);
	ep->fd = INVALID_SOCKET;
	ep->phase = sys_interphase;
}


/*
 * create new interface structure initialize from
 * template structure or via standard initialization
 * function
 */
static endpt *
new_interface(
	endpt *interface
	)
{
	endpt *	iface;

	iface = emalloc(sizeof(*iface));

	if (NULL == interface)
		init_interface(iface);
	else				/* use the template */
		memcpy(iface, interface, sizeof(*iface));

	/* count every new instance of an interface in the system */
	iface->ifnum = sys_ifnum++;
	iface->starttime = current_time;

	return iface;
}


/*
 * return interface storage into free memory pool
 */
static inline void
delete_interface(
	endpt *ep
	)
{
	free(ep);
}


/*
 * link interface into list of known interfaces
 */
static void
add_interface(
	endpt *	ep
	)
{
	/* Calculate the refid */
	ep->addr_refid = addr2refid(&ep->sin);
	/* link at tail so ntpq -c ifstats index increases each row */
	LINK_TAIL_SLIST(ep_list, ep, elink, endpt);
	ninterfaces++;
}


/*
 * remove interface from known interface list and clean up
 * associated resources
 */
static void
remove_interface(
	endpt *	ep
	)
{
	endpt *		unlinked;
	sockaddr_u	resmask;

	UNLINK_SLIST(unlinked, ep_list, ep, elink, endpt);
	delete_interface_from_list(ep);

	if (ep->fd != INVALID_SOCKET) {
		msyslog(LOG_INFO,
			"IO: Deleting interface #%u %s, %s#%d, interface stats: "
                        "received=%ld, sent=%ld, dropped=%ld, "
                        "active_time=%lu secs",
			ep->ifnum,
			ep->name,
			socktoa(&ep->sin),
			SRCPORT(&ep->sin),
			ep->received,
			ep->sent,
			ep->notsent,
			current_time - ep->starttime);
		close_and_delete_fd_from_list(ep->fd);
		ep->fd = INVALID_SOCKET;
	}

	ninterfaces--;
	mon_clearinterface(ep);

	/* remove restrict interface entry */
	SET_HOSTMASK(&resmask, AF(&ep->sin));
	hack_restrict(RESTRICT_REMOVEIF, &ep->sin, &resmask,
		      RESM_NTPONLY | RESM_INTERFACE, RES_IGNORE, 0);
}


static void
log_listen_address(
	endpt *	ep
	)
{
	msyslog(LOG_INFO, "IO: %s on %u %s %s",
			(ep->ignore_packets)
			    ? "Listen and drop"
			    : "Listen normally",
			ep->ifnum,
			ep->name,
			sockporttoa(&ep->sin));
}


static void
create_wildcards(
	unsigned short	port
	)
{
	bool			v4wild;
	bool			v6wild;
	sockaddr_u		wildaddr;
	nic_rule_action		action;
	endpt *	wildif;

	/*
	 * silence "potentially uninitialized" warnings from VC9
	 * failing to follow the logic.  Ideally action could remain
	 * uninitialized, and the memset be the first statement under
	 * the first if (v4wild).
	 */
	action = ACTION_LISTEN;
	ZERO(wildaddr);

	/*
	 * create pseudo-interface with wildcard IPv6 address
	 */
	v6wild = ipv6_works;
	if (v6wild) {
		/* set wildaddr to the v6 wildcard address :: */
		ZERO(wildaddr);
		AF(&wildaddr) = AF_INET6;
		SET_ADDR6N(&wildaddr, in6addr_any);
		SET_PORT(&wildaddr, port);
		SET_SCOPE(&wildaddr, 0);

		/* check for interface/nic rules affecting the wildcard */
		action = interface_action(NULL, &wildaddr, 0);
		v6wild = (ACTION_IGNORE != action);
	}
	if (v6wild) {
		wildif = new_interface(NULL);

		strlcpy(wildif->name, "v6wildcard", sizeof(wildif->name));
		memcpy(&wildif->sin, &wildaddr, sizeof(wildif->sin));
		wildif->family = AF_INET6;
		AF(&wildif->mask) = AF_INET6;
		SET_ONESMASK(&wildif->mask);

		wildif->flags = INT_UP | INT_WILDCARD;
		wildif->ignore_packets = (ACTION_DROP == action);

		wildif->fd = open_socket(&wildif->sin, true, wildif);

		if (wildif->fd != INVALID_SOCKET) {
			wildipv6 = wildif;
			any6_interface = wildif;
			add_addr_to_list(&wildif->sin, wildif);
			add_interface(wildif);
			log_listen_address(wildif);
		} else {
			msyslog(LOG_ERR,
				"IO: unable to bind to wildcard address %s - another process may be running: %m; EXITING",
				socktoa(&wildif->sin));
			exit(1);
		}
		DPRINT_INTERFACE(2, (wildif, "created ", "\n"));
	}

	/*
	 * create pseudo-interface with wildcard IPv4 address
	 */
	v4wild = ipv4_works;
	if (v4wild) {
		/* set wildaddr to the v4 wildcard address 0.0.0.0 */
		AF(&wildaddr) = AF_INET;
		SET_ADDR4N(&wildaddr, INADDR_ANY);
		SET_PORT(&wildaddr, port);

		/* check for interface/nic rules affecting the wildcard */
		action = interface_action(NULL, &wildaddr, 0);
		v4wild = (ACTION_IGNORE != action);
	}
	if (v4wild) {
		wildif = new_interface(NULL);

		strlcpy(wildif->name, "v4wildcard", sizeof(wildif->name));
		memcpy(&wildif->sin, &wildaddr, sizeof(wildif->sin));
		wildif->family = AF_INET;
		AF(&wildif->mask) = AF_INET;
		SET_ONESMASK(&wildif->mask);

		wildif->flags = INT_BROADCAST | INT_UP | INT_WILDCARD;
		wildif->ignore_packets = (ACTION_DROP == action);
		wildif->fd = open_socket(&wildif->sin, true, wildif);

		if (wildif->fd != INVALID_SOCKET) {
			wildipv4 = wildif;
			any_interface = wildif;

			add_addr_to_list(&wildif->sin, wildif);
			add_interface(wildif);
			log_listen_address(wildif);
		} else {
			msyslog(LOG_ERR,
				"IO: unable to bind to wildcard address %s - another process may be running: %m; EXITING",
				socktoa(&wildif->sin));
			exit(1);
		}
		DPRINT_INTERFACE(2, (wildif, "created ", "\n"));
	}
}


/*
 * add_nic_rule() -- insert a rule entry at the head of nic_rule_list.
 */
void
add_nic_rule(
	nic_rule_match	match_type,
	const char *	if_name,	/* interface name or numeric address */
	int		prefixlen,
	nic_rule_action	action
	)
{
	nic_rule *	rule;
	bool	is_ip;

	rule = emalloc_zero(sizeof(*rule));
	rule->match_type = match_type;
	rule->prefixlen = prefixlen;
	rule->action = action;

	if (MATCH_IFNAME == match_type) {
		REQUIRE(NULL != if_name);
		rule->if_name = estrdup(if_name);
	} else if (MATCH_IFADDR == match_type) {
		REQUIRE(NULL != if_name);
		/* set rule->addr */
		is_ip = is_ip_address(if_name, AF_UNSPEC, &rule->addr);
		REQUIRE(is_ip);
	} else
		REQUIRE(NULL == if_name);

	LINK_SLIST(nic_rule_list, rule, next);
}


#ifdef DEBUG
static const char *
action_text(
	nic_rule_action	action
	)
{
	const char *t;

	switch (action) {

	default:
		t = "ERROR";	/* quiet uninit warning */
		DPRINT(1, ("fatal: unknown nic_rule_action %u\n",
			   action));
		ENSURE(0);
		break;

	case ACTION_LISTEN:
		t = "listen";
		break;

	case ACTION_IGNORE:
		t = "ignore";
		break;

	case ACTION_DROP:
		t = "drop";
		break;
	}

	return t;
}
#endif	/* DEBUG */


static nic_rule_action
interface_action(
	char *		if_name,
	sockaddr_u *	if_addr,
	uint32_t		if_flags
	)
{
	nic_rule *	rule;
	int		isloopback;
	int		iswildcard;

	DPRINT(4, ("interface_action: interface %s ",
		   (if_name != NULL) ? if_name : "wildcard"));

	iswildcard = is_wildcard_addr(if_addr);
	isloopback = !!(INT_LOOPBACK & if_flags);

	/*
	 * Find any matching NIC rule from --interface / -I or ntp.conf
	 * interface/nic rules.
	 */
	for (rule = nic_rule_list; rule != NULL; rule = rule->next) {

		switch (rule->match_type) {
                default:
                        /* huh? */
		case MATCH_ALL:
			/* loopback and wildcard excluded from "all" */
			if (isloopback || iswildcard)
				break;
			DPRINT(4, ("nic all %s\n",
				   action_text(rule->action)));
			return rule->action;

		case MATCH_IPV4:
			if (IS_IPV4(if_addr)) {
				DPRINT(4, ("nic ipv4 %s\n",
					   action_text(rule->action)));
				return rule->action;
			}
			break;

		case MATCH_IPV6:
			if (IS_IPV6(if_addr)) {
				DPRINT(4, ("nic ipv6 %s\n",
					   action_text(rule->action)));
				return rule->action;
			}
			break;

		case MATCH_WILDCARD:
			if (iswildcard) {
				DPRINT(4, ("nic wildcard %s\n",
					   action_text(rule->action)));
				return rule->action;
			}
			break;

		case MATCH_IFADDR:
			if (rule->prefixlen != -1) {
				if (addr_eqprefix(if_addr, &rule->addr,
						  rule->prefixlen)) {

					DPRINT(4, ("subnet address match - %s\n",
						   action_text(rule->action)));
					return rule->action;
				}
			} else
				if (SOCK_EQ(if_addr, &rule->addr)) {

					DPRINT(4, ("address match - %s\n",
						   action_text(rule->action)));
					return rule->action;
				}
			break;

		case MATCH_IFNAME:
			if (if_name != NULL
#if defined(FNM_CASEFOLD)
			    && !fnmatch(rule->if_name, if_name, FNM_CASEFOLD)
#else
			    && !strcasecmp(if_name, rule->if_name)
#endif
			    ) {

				DPRINT(4, ("interface name match - %s\n",
					   action_text(rule->action)));
				return rule->action;
			}
			break;
		}
	}

	/*
	 * Unless explicitly disabled such as with "nic ignore ::1"
	 * listen on loopback addresses.  Since ntpq query
	 * "localhost" by default, which typically resolves to ::1 and
	 * 127.0.0.1, it's useful to default to listening on both.
	 */
	if (isloopback) {
		DPRINT(4, ("default loopback listen\n"));
		return ACTION_LISTEN;
	}

	/*
	 * Treat wildcard addresses specially.  If there is no explicit
	 * "nic ... wildcard" or "nic ... 0.0.0.0" or "nic ... ::" rule
	 * default to drop.
	 */
	if (iswildcard) {
		DPRINT(4, ("default wildcard drop\n"));
		return ACTION_DROP;
	}

	/*
	 * Check for "virtual IP" (colon in the interface name) after
	 * the rules so that "ntpd --interface eth0:1 -novirtualips"
	 * does indeed listen on eth0:1's addresses.
	 */
	if (!listen_to_virtual_ips && if_name != NULL
	    && (strchr(if_name, ':') != NULL)) {

		DPRINT(4, ("virtual ip - ignore\n"));
		return ACTION_IGNORE;
	}

	/*
	 * If there are no --interface/-I command-line options and no
	 * interface/nic rules in ntp.conf, the default action is to
	 * listen.  In the presence of rules from either, the default
	 * is to ignore.  This implements ntpd's traditional listen-
	 * every default with no interface listen configuration, and
	 * ensures a single -I eth0 or "nic listen eth0" means do not
	 * listen on any other addresses.
	 */
	if (NULL == nic_rule_list) {
		DPRINT(4, ("default listen\n"));
		return ACTION_LISTEN;
	}

	DPRINT(4, ("implicit ignore\n"));
	return ACTION_IGNORE;
}


static void
convert_isc_if(
	isc_interface_t *isc_if,
	endpt *itf,
	unsigned short port
	)
{
	const uint8_t v6loop[16] = {0, 0, 0, 0, 0, 0, 0, 0,
				   0, 0, 0, 0, 0, 0, 0, 1};

	strlcpy(itf->name, isc_if->name, sizeof(itf->name));
	itf->ifindex = isc_if->ifindex;
	itf->family = (unsigned short)isc_if->af;
	AF(&itf->sin) = (sa_family_t)itf->family;
	AF(&itf->mask) = (sa_family_t)itf->family;
	AF(&itf->bcast) = (sa_family_t)itf->family;
	SET_PORT(&itf->sin, port);
	SET_PORT(&itf->mask, port);
	SET_PORT(&itf->bcast, port);

	if (IS_IPV4(&itf->sin)) {
		NSRCADR(&itf->sin) = isc_if->address.type.in.s_addr;
		NSRCADR(&itf->mask) = isc_if->netmask.type.in.s_addr;

		if (isc_if->flags & INTERFACE_F_BROADCAST) {
			itf->flags |= INT_BROADCAST;
			NSRCADR(&itf->bcast) =
			    isc_if->broadcast.type.in.s_addr;
		}
	}
	else if (IS_IPV6(&itf->sin)) {
		SET_ADDR6N(&itf->sin, isc_if->address.type.in6);
		SET_ADDR6N(&itf->mask, isc_if->netmask.type.in6);

		SET_SCOPE(&itf->sin, isc_if->address.zone);
	}

	/* Process the rest of the flags */

	itf->flags |=
		  ((INTERFACE_F_UP & isc_if->flags)
			? INT_UP : 0)
		| ((INTERFACE_F_LOOPBACK & isc_if->flags)
			? INT_LOOPBACK : 0)
		| ((INTERFACE_F_POINTTOPOINT & isc_if->flags)
			? INT_PPP : 0)
		| ((INTERFACE_F_PRIVACY & isc_if->flags)
			? INT_PRIVACY : 0)
		;

	/*
	 * Clear the loopback flag if the address is not localhost.
	 * http://bugs.ntp.org/1683
	 */
	if (INT_LOOPBACK & itf->flags) {
		if (AF_INET == itf->family) {
			if (127 != (SRCADR(&itf->sin) >> 24))
				itf->flags &= ~INT_LOOPBACK;
		} else {
			if (memcmp(v6loop, NSRCADR6(&itf->sin),
				   sizeof(NSRCADR6(&itf->sin))))
				itf->flags &= ~INT_LOOPBACK;
		}
	}
}


/*
 * refresh_interface
 *
 * some OSes have been observed to keep
 * cached routes even when more specific routes
 * become available.
 * this can be mitigated by re-binding
 * the socket.
 */
static bool
refresh_interface(
	endpt * interface
	)
{
#ifdef  OS_MISSES_SPECIFIC_ROUTE_UPDATES
	if (interface->fd != INVALID_SOCKET) {
		close_and_delete_fd_from_list(interface->fd);

		/* create new socket picking up a new first hop binding
		   at connect() time */
		interface->fd = open_socket(&interface->sin, false, interface);
		 /*
		  * reset TTL indication so TTL is is set again
		  * next time around
		  */
		return (interface->fd != INVALID_SOCKET);
	} else
		return false;	/* invalid sockets are not refreshable */
#else /* !OS_MISSES_SPECIFIC_ROUTE_UPDATES */
	return (interface->fd != INVALID_SOCKET);
#endif /* !OS_MISSES_SPECIFIC_ROUTE_UPDATES */
}

/*
 * interface_update - externally callable update function
 */
void
interface_update(
	interface_receiver_t	receiver,
	void *			data)
{
	bool new_interface_found;

	if (disable_dynamic_updates)
		return;

	new_interface_found = update_interfaces(NTP_PORT, receiver, data);

	if (!new_interface_found)
		return;

#ifdef ENABLE_DNS_LOOKUP
#ifdef DEBUG
	msyslog(LOG_DEBUG, "IO: new interface(s) found: waking up resolver");
#endif
	dns_new_interface();
#endif
}


static bool
is_wildcard_addr(
	const sockaddr_u *psau
	)
{
	if (IS_IPV4(psau) && !NSRCADR(psau))
		return true;

	if (IS_IPV6(psau) && S_ADDR6_EQ(psau, &in6addr_any))
		return true;

	return false;
}


#ifdef NEED_REUSEADDR_FOR_IFADDRBIND
/*
 * enable/disable re-use of wildcard address socket
 */
static void
set_wildcard_reuse(
	unsigned short	family,
	int	on
	)
{
	endpt *any;
	SOCKET fd = INVALID_SOCKET;

	any = ANY_INTERFACE_BYFAM(family);
	if (any != NULL)
		fd = any->fd;

	if (fd != INVALID_SOCKET) {
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
			       (char *)&on, sizeof(on)))
			msyslog(LOG_ERR,
				"IO: set_wildcard_reuse: setsockopt(SO_REUSEADDR, %s) failed: %m",
				on ? "on" : "off");

		DPRINT(4, ("set SO_REUSEADDR to %s on %s\n",
			   on ? "on" : "off",
			   socktoa(&any->sin)));
	}
}
#endif /* NEED_REUSEADDR_FOR_IFADDRBIND */


static bool
check_flags6(
	sockaddr_u *psau,
	const char *name,
	uint32_t flags6
	)
{
#if defined(SIOCGIFAFLAG_IN6)
	struct in6_ifreq ifr6;
	int fd;

	if (AF(psau) != AF_INET6)
		return false;
	if ((fd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
		return false;
	ZERO(ifr6);
	memcpy(&ifr6.ifr_addr, &psau->sa6, sizeof(ifr6.ifr_addr));
	strlcpy(ifr6.ifr_name, name, sizeof(ifr6.ifr_name));
	if (ioctl(fd, SIOCGIFAFLAG_IN6, &ifr6) < 0) {
		close(fd);
		return false;
	}
	close(fd);
	if ((ifr6.ifr_ifru.ifru_flags6 & flags6) != 0)
		return true;
#else
	UNUSED_ARG(psau);
	UNUSED_ARG(name);
	UNUSED_ARG(flags6);
#endif	/* SIOCGIFAFLAG_IN6 */
	return false;
}

static bool
is_anycast(
	sockaddr_u *psau,
	const char *name
	)
{
#ifdef IN6_IFF_ANYCAST
	return check_flags6(psau, name, IN6_IFF_ANYCAST);
#else
	UNUSED_ARG(psau);
	UNUSED_ARG(name);
	return false;
#endif
}

static bool
is_valid(
	sockaddr_u *psau,
	const char *name
	)
{
	uint32_t flags6;

	flags6 = 0;
#ifdef IN6_IFF_DEPARTED
	flags6 |= IN6_IFF_DEPARTED;
#endif
#ifdef IN6_IFF_DETACHED
	flags6 |= IN6_IFF_DETACHED;
#endif
#ifdef IN6_IFF_TENTATIVE
	flags6 |= IN6_IFF_TENTATIVE;
#endif
	return check_flags6(psau, name, flags6) ? false : true;
}

/*
 * update_interface strategy
 *
 * toggle configuration phase
 *
 * Phase 1:
 * forall currently existing interfaces
 *   if address is known:
 *	drop socket - rebind again
 *
 *   if address is NOT known:
 *	attempt to create a new interface entry
 *
 * Phase 2:
 * forall currently known non MCAST and WILDCARD interfaces
 *   if interface does not match configuration phase (not seen in phase 1):
 *	remove interface from known interface list
 *	forall peers associated with this interface
 *         disconnect peer from this interface
 *
 * Phase 3:
 *   attempt to re-assign interfaces to peers
 *
 */

static bool
update_interfaces(
	unsigned short		port,
	interface_receiver_t	receiver,
	void *			data
	)
{
	isc_mem_t *		mctx = (void *)-1;
	interface_info_t	ifi;
	isc_interfaceiter_t *	iter;
	bool			result;
	isc_interface_t		isc_if;
	int			new_interface_found;
	unsigned int		family;
	endpt			enumep;
	endpt *			ep;
	endpt *			next_ep;

	DPRINT(3, ("update_interfaces(%d)\n", port));

	/*
	 * phase one - scan interfaces
	 * - create those that are not found
	 * - update those that are found
	 */

	new_interface_found = false;
	iter = NULL;
	result = isc_interfaceiter_create_bool(mctx, &iter);

	if (!result)
		return false;

	/*
	 * Toggle system interface scan phase to find untouched
	 * interfaces to be deleted.
	 */
	sys_interphase ^= 0x1;

	for (result = isc_interfaceiter_first_bool(iter);
	     result;
	     result = isc_interfaceiter_next_bool(iter)) {

		result = isc_interfaceiter_current_bool(iter, &isc_if);

		if (!result)
			break;

		/* See if we have a valid family to use */
		family = isc_if.address.family;
		if (AF_INET != family && AF_INET6 != family)
			continue;
		if (AF_INET == family && !ipv4_works)
			continue;
		if (AF_INET6 == family && !ipv6_works)
			continue;

		/* create prototype */
		init_interface(&enumep);

		convert_isc_if(&isc_if, &enumep, port);

		DPRINT_INTERFACE(4, (&enumep, "examining ", "\n"));

		/*
		 * Check if and how we are going to use the interface.
		 */
		switch (interface_action(enumep.name, &enumep.sin,
					 enumep.flags)) {

                default:
		case ACTION_IGNORE:
			DPRINT(4, ("ignoring interface %s (%s) - by nic rules\n",
				   enumep.name, socktoa(&enumep.sin)));
			continue;

		case ACTION_LISTEN:
			DPRINT(4, ("listen interface %s (%s) - by nic rules\n",
				   enumep.name, socktoa(&enumep.sin)));
			enumep.ignore_packets = false;
			break;

		case ACTION_DROP:
			DPRINT(4, ("drop on interface %s (%s) - by nic rules\n",
				   enumep.name, socktoa(&enumep.sin)));
			enumep.ignore_packets = true;
			break;
		}

		 /* interfaces must be UP to be usable */
		if (!(enumep.flags & INT_UP)) {
			DPRINT(4, ("skipping interface %s (%s) - DOWN\n",
				   enumep.name, socktoa(&enumep.sin)));
			continue;
		}

		/*
		 * skip any interfaces UP and bound to a wildcard
		 * address - some dhcp clients produce that in the
		 * wild
		 */
		if (is_wildcard_addr(&enumep.sin))
			continue;

		if (is_anycast(&enumep.sin, isc_if.name))
			continue;

		/*
		 * skip any address that is an invalid state to be used
		 */
		if (!is_valid(&enumep.sin, isc_if.name))
			continue;

		/*
		 * map to local *address* in order to map all duplicate
		 * interfaces to an endpt structure with the appropriate
		 * socket.  Our name space is (ip-address), NOT
		 * (interface name, ip-address).
		 */
		ep = getinterface(&enumep.sin, INT_WILDCARD);

		if (ep != NULL && refresh_interface(ep)) {
			/*
			 * found existing and up to date interface -
			 * mark present.
			 */
			if (ep->phase != sys_interphase) {
				/*
				 * On a new round we reset the name so
				 * the interface name shows up again if
				 * this address is no longer shared.
				 * We reset ignore_packets from the
				 * new prototype to respect any runtime
				 * changes to the nic rules.
				 */
				strlcpy(ep->name, enumep.name,
					sizeof(ep->name));
				ep->ignore_packets =
					    enumep.ignore_packets;
			} else {
				/* name collision - rename interface */
				strlcpy(ep->name, "*multiple*",
					sizeof(ep->name));
			}

			DPRINT_INTERFACE(4, (ep, "updating ",
					     " present\n"));

			if (ep->ignore_packets !=
			    enumep.ignore_packets) {
				/*
				 * We have conflicting configurations
				 * for the interface address. This is
				 * caused by using -I <interfacename>
				 * for an interface that shares its
				 * address with other interfaces. We
				 * can not disambiguate incoming
				 * packets delivered to this socket
				 * without extra syscalls/features.
				 * These are not (commonly) available.
				 * Note this is a more unusual
				 * configuration where several
				 * interfaces share an address but
				 * filtering via interface name is
				 * attempted.  We resolve the
				 * configuration conflict by disabling
				 * the processing of received packets.
				 * This leads to no service on the
				 * interface address where the conflict
				 * occurs.
				 */
				msyslog(LOG_ERR,
					"CONFIG: WARNING: conflicting enable configuration for interfaces %s and %s for address %s - unsupported configuration - address DISABLED",
					enumep.name, ep->name,
					socktoa(&enumep.sin));

				ep->ignore_packets = true;
			}

			ep->phase = sys_interphase;

			ifi.action = IFS_EXISTS;
			ifi.ep = ep;
			if (receiver != NULL)
				(*receiver)(data, &ifi);
		} else {
			/*
			 * This is new or refreshing failed - add to
			 * our interface list.  If refreshing failed we
			 * will delete the interface structure in phase
			 * 2 as the interface was not marked current.
			 * We can bind to the address as the refresh
			 * code already closed the offending socket
			 */
			ep = create_interface(port, &enumep);

			if (ep != NULL) {
				ifi.action = IFS_CREATED;
				ifi.ep = ep;
				if (receiver != NULL)
					(*receiver)(data, &ifi);

				new_interface_found = true;
				DPRINT_INTERFACE(3,
					(ep, "updating ",
					 " new - created\n"));
			} else {
				DPRINT_INTERFACE(3,
					(&enumep, "updating ",
					 " new - creation FAILED"));

				msyslog(LOG_INFO,
					"IO: failed to init interface for address %s",
					socktoa(&enumep.sin));
				continue;
			}
		}
	}

	isc_interfaceiter_destroy(&iter);

	/*
	 * phase 2 - delete gone interfaces - reassigning peers to
	 * other interfaces
	 */
	for (ep = ep_list; ep != NULL; ep = next_ep) {
		next_ep = ep->elink;

		/*
		 * if phase does not match sys_phase this interface was
		 * not enumerated during the last interface scan - so it
		 * is gone and will be deleted here unless it did not
		 * originate from interface enumeration INT_WILDCARD,
		 */
		if ((INT_WILDCARD & ep->flags) || ep->phase == sys_interphase)
			continue;

		DPRINT_INTERFACE(3, (ep, "updating ",
				     "GONE - deleting\n"));
		remove_interface(ep);

		ifi.action = IFS_DELETED;
		ifi.ep = ep;
		if (receiver != NULL)
			(*receiver)(data, &ifi);

		/* disconnect peers from deleted endpt. */
		while (ep->peers != NULL)
			set_peerdstadr(ep->peers, NULL);

		/*
		 * update globals in case we lose
		 * a loopback interface
		 */
		if (ep == loopback_interface)
			loopback_interface = NULL;

		delete_interface(ep);
	}

	/*
	 * phase 3 - re-configure as the world has possibly changed
	 *
	 * never ever make this conditional again - it is needed to track
	 * routing updates. see bug #2506
	 */
	refresh_all_peerinterfaces();

	return new_interface_found;
}


/*
 * create_sockets - create a socket for each interface plus a default
 *			socket for when we don't know where to send
 */
static int
create_sockets(
	unsigned short port
	)
{
	maxactivefd = 0;
	FD_ZERO(&activefds);

	DPRINT(2, ("create_sockets(%d)\n", port));

	create_wildcards(port);

	update_interfaces(port, NULL, NULL);

	/*
	 * Now that we have opened all the sockets, turn off the reuse
	 * flag for security.
	 */
	set_reuseaddr(0);

	DPRINT(2, ("create_sockets: Total interfaces = %d\n", ninterfaces));

	return ninterfaces;
}

/*
 * create_interface - create a new interface for a given prototype
 *		      binding the socket.
 */
static endpt *
create_interface(
	unsigned short		port,
	endpt *	protot
	)
{
	sockaddr_u	resmask;
	endpt *		iface;
	DPRINT(2, ("create_interface(%s#%d)\n", socktoa(&protot->sin),
		    port));

	/* build an interface */
	iface = new_interface(protot);

	/*
	 * create socket
	 */
	iface->fd = open_socket(&iface->sin, false, iface);

	if (iface->fd != INVALID_SOCKET)
		log_listen_address(iface);

	if (INVALID_SOCKET == iface->fd) {
		msyslog(LOG_ERR, "IO: unable to create socket on %s (%u) for %s#%d",
			iface->name,
			iface->ifnum,
			socktoa((&iface->sin)),
			port);
		delete_interface(iface);
		return NULL;
	}

	/*
	 * Blacklist our own addresses, no use talking to ourself
	 */
	SET_HOSTMASK(&resmask, AF(&iface->sin));
	hack_restrict(RESTRICT_FLAGS, &iface->sin, &resmask,
		      RESM_NTPONLY | RESM_INTERFACE, RES_IGNORE, 0);

	/*
	 * set globals with the first found
	 * loopback interface of the appropriate class
	 */
	if (NULL == loopback_interface && AF_INET == iface->family
	    && (INT_LOOPBACK & iface->flags))
		loopback_interface = iface;

	/*
	 * put into our interface list
	 */
	add_addr_to_list(&iface->sin, iface);
	add_interface(iface);

	DPRINT_INTERFACE(2, (iface, "created ", "\n"));
	return iface;
}


#ifdef SO_EXCLUSIVEADDRUSE
static void
set_excladdruse(
	SOCKET fd
	)
{
	int one = 1;
	int failed;

	failed = setsockopt(fd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
			    (char *)&one, sizeof(one));

	if (!failed)
		return;

	msyslog(LOG_ERR,
		"IO: setsockopt(%d, SO_EXCLUSIVEADDRUSE, on): %m",
		(int)fd);
}
#endif  /* SO_EXCLUSIVEADDRUSE */


/*
 * set_reuseaddr() - set/clear REUSEADDR on all sockets
 *			NB possible hole - should we be doing this on broadcast
 *			fd's also?
 */
static void
set_reuseaddr(
	int flag
	)
{
#ifndef SO_EXCLUSIVEADDRUSE
	endpt *ep;

	for (ep = ep_list; ep != NULL; ep = ep->elink) {
		if (ep->flags & INT_WILDCARD)
			continue;

		/*
		 * if ep->fd  is INVALID_SOCKET, we might have a adapter
		 * configured but not present
		 */
		DPRINT(4, ("setting SO_REUSEADDR on %.16s@%s to %s\n",
			   ep->name, socktoa(&ep->sin),
			   flag ? "on" : "off"));

		if (ep->fd != INVALID_SOCKET) {
			if (setsockopt(ep->fd, SOL_SOCKET, SO_REUSEADDR,
				       (char *)&flag, sizeof(flag))) {
				msyslog(LOG_ERR, "IO: set_reuseaddr: setsockopt(%s, SO_REUSEADDR, %s) failed: %m",
					socktoa(&ep->sin), flag ? "on" : "off");
			}
		}
	}
#endif /* ! SO_EXCLUSIVEADDRUSE */
}

/*
 * open_socket - open a socket, returning the file descriptor
 */

SOCKET
open_socket(
	sockaddr_u *	addr,
	bool		turn_off_reuse,
	endpt *		interf
	)
{
	SOCKET	fd;
	int	errval;
	/*
	 * int is OK for REUSEADR per
	 * http://www.kohala.com/start/mcast.api.txt
	 */
	const int	on = 1;
	const int	off = 0;

	if (IS_IPV6(addr) && !ipv6_works)
		return INVALID_SOCKET;

	/* create a datagram (UDP) socket */
	fd = socket(AF(addr), SOCK_DGRAM, 0);
	if (INVALID_SOCKET == fd) {
		errval = errno;
		msyslog(LOG_ERR,
			"IO: socket(AF_INET%s, SOCK_DGRAM, 0) failed on address %s: %m",
			IS_IPV6(addr) ? "6" : "", socktoa(addr));

		if (errval == EPROTONOSUPPORT ||
		    errval == EAFNOSUPPORT ||
		    errval == EPFNOSUPPORT)
			return (INVALID_SOCKET);

		errno = errval;
#ifndef __COVERITY__
		msyslog(LOG_ERR,
			"IO: unexpected socket() error %m code %d (not EPROTONOSUPPORT nor EAFNOSUPPORT nor EPFNOSUPPORT) - exiting",
			errno);
		exit(1);
#endif /* __COVERITY__ */
	}

	/*
	 * Fixup the file descriptor for some systems
	 * See bug #530 for details of the issue.
	 */
	fd = move_fd(fd);

	/*
	 * set SO_REUSEADDR since we will be binding the same port
	 * number on each interface according to turn_off_reuse.
	 */
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
		       (const void *)((turn_off_reuse)
				    ? &off
				    : &on),
		       sizeof(on))) {

		msyslog(LOG_ERR,
			"IO: setsockopt SO_REUSEADDR %s fails for address %s: %m",
			(turn_off_reuse)
			    ? "off"
			    : "on",
			socktoa(addr));
		close(fd);
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
		if (setsockopt(fd, IPPROTO_IP, IP_TOS, (char*)&qos,
			       sizeof(qos)))
			msyslog(LOG_ERR,
				"IO: setsockopt IP_TOS (%02x) fails on "
                                "address %s: %m",
				(unsigned)qos, socktoa(addr));
	}

	/*
	 * IPv6 specific options go here
	 */
	if (IS_IPV6(addr)) {
#ifdef IPV6_TCLASS
		if (setsockopt(fd, IPPROTO_IPV6, IPV6_TCLASS, (char*)&qos,
			       sizeof(qos)))
			msyslog(LOG_ERR, "IO: setsockopt IPV6_TCLASS (%02x) "
                                         "fails on address %s: %m",
				         (unsigned)qos, socktoa(addr));
#endif /* IPV6_TCLASS */
		if (isc_net_probe_ipv6only_bool()
		    && setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY,
		    (const void *)&on, sizeof(on)))
			msyslog(LOG_ERR,
				"IO: setsockopt IPV6_V6ONLY on fails on address %s: %m",
				socktoa(addr));
	}

#ifdef NEED_REUSEADDR_FOR_IFADDRBIND
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

#ifdef NEED_REUSEADDR_FOR_IFADDRBIND
	if (!is_wildcard_addr(addr))
		set_wildcard_reuse(AF(addr), 0);
#endif

	if (errval < 0) {
		/*
		 * Don't log this under all conditions
		 */
		if (turn_off_reuse == 0
#ifdef DEBUG
		    || debug > 1 /* SPECIAL DEBUG */
#endif
		    ) {
			msyslog(LOG_ERR,
				"IO: bind(%d) AF_INET%s %s#%d flags 0x%x failed: %m",
				fd, IS_IPV6(addr) ? "6" : "",
				socktoa(addr), SRCPORT(addr),
				interf->flags);
		}

		close(fd);

		return INVALID_SOCKET;
	}

	enable_packetstamps(fd, addr);

	DPRINT(4, ("bind(%d) AF_INET%s, addr %s%%%u#%d, flags 0x%x\n",
		   fd, IS_IPV6(addr) ? "6" : "", socktoa(addr),
		   SCOPE(addr), SRCPORT(addr), interf->flags));

	make_socket_nonblocking(fd);

	add_fd_to_list(fd, FD_TYPE_SOCKET);

#ifdef F_GETFL
	/* F_GETFL may not be defined if the underlying OS isn't really Unix */
	DPRINT(4, ("flags for fd %d: 0x%x\n", fd,
		   (unsigned)fcntl(fd, F_GETFL, 0)));
#endif

	return fd;
}


/*
 * sendpkt - send a packet to the specified destination. Maintain a
 * send error cache so that only the first consecutive error for a
 * destination is logged.
 */
void
sendpkt(
	sockaddr_u *		dest,
	endpt *	ep,
	void *			pkt,
	int			len
	)
{
	endpt *	src;
	ssize_t	cc;

	src = ep;
	if (NULL == src) {
		/*
		 * unbound peer - drop request and wait for better
		 * network conditions
		 */
		DPRINT(2, ("sendpkt(dst=%s, len=%d): no interface - IGNORED\n",
			   socktoa(dest), len));
		return;
	}

	DPRINT(2, ("sendpkt(%d, dst=%s, src=%s, len=%d)\n",
		   src->fd, socktoa(dest), socktoa(&src->sin), len));

	cc = sendto(src->fd, pkt, (unsigned int)len, 0,
		    &dest->sa, SOCKLEN(dest));
	if (cc == -1) {
		src->notsent++;
		pkt_count.packets_notsent++;
	} else	{
		src->sent++;
		pkt_count.packets_sent++;
	}
}



#ifdef REFCLOCK
/*
 * Routine to read the refclock packets for a specific interface
 * Return the number of bytes read. That way we know if we should
 * read it again or go on to the next one if no bytes returned
 *
 * Note: too big to inline
 */
static int
read_refclock_packet(
	SOCKET			fd,
	struct refclockio *	rp
	)
{
	size_t			i;
	ssize_t			buflen;
	int			saved_errno;
	int			consumed;
	struct recvbuf *	rb;
	l_fp			ts;

	/* Could read earlier in normal case,
	 * but too early gets wrong time if data arrives
	 * while we are busy processing other packets.
	 */
	get_systime(&ts);

	rb = get_free_recv_buffer();

	if (NULL == rb) {
		/*
		 * No buffer space available - just drop the packet
		 */
		char buf[RX_BUFF_SIZE];

		buflen = read(fd, buf, sizeof buf);
		pkt_count.packets_dropped++;
		return (buflen);
	}

	i = (rp->datalen == 0
	     || rp->datalen > sizeof(rb->recv_space))
	        ? sizeof(rb->recv_space)
		: rp->datalen;
	do {
		buflen = read(fd, (char *)&rb->recv_space, i);
	} while (buflen < 0 && EINTR == errno);

	if (buflen <= 0) {
		saved_errno = errno;
		freerecvbuf(rb);
		errno = saved_errno;
		return (int)buflen;
	}

	/*
	 * Got one. Mark how and when it got here,
	 * put it on the full list and do bookkeeping.
	 */
	rb->recv_length = (size_t)buflen;
	rb->recv_peer = rp->srcclock;
	rb->dstadr = 0;
	rb->fd = fd;
	rb->recv_time = ts;

	consumed = indicate_refclock_packet(rp, rb);
	if (!consumed) {
		rp->recvcount++;
		// FIXME: should have separate slot for refclock packets
		pkt_count.packets_received++;
	}

	return (int)buflen;
}
#endif	/* REFCLOCK */

/*
 * Routine to read the network NTP packets for a specific interface
 * Return the number of bytes read. That way we know if we should
 * read it again or go on to the next one if no bytes returned
 */
static int
read_network_packet(
	SOCKET			fd,
	endpt *	itf
	)
{
	socklen_t fromlen;
	ssize_t buflen;
	struct recvbuf *rb;
	struct msghdr msghdr;
	struct iovec iovec;
	char control[100];   /* FIXME: Need space for time stamp plus overhead */

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
		DPRINT(4, ("%s on (%lu) fd=%d from %s\n",
			(itf->ignore_packets)
			    ? "ignore"
			    : "drop",
			free_recvbuffs(), fd, socktoa(&from)));
		if (itf->ignore_packets)
			pkt_count.packets_ignored++;
		else
			pkt_count.packets_dropped++;
		return (buflen);
	}

	fromlen = sizeof(rb->recv_srcadr);

	iovec.iov_base        = &rb->recv_space;
	iovec.iov_len         = sizeof(rb->recv_space);
	memset(&msghdr, '\0', sizeof(msghdr));
	msghdr.msg_name       = &rb->recv_srcadr;
	msghdr.msg_namelen    = fromlen;
	msghdr.msg_iov        = &iovec;
	msghdr.msg_iovlen     = 1;
	msghdr.msg_flags      = 0;
	msghdr.msg_control    = (void *)&control;
	msghdr.msg_controllen = sizeof(control);
	buflen                = recvmsg(fd, &msghdr, 0);

	rb->recv_length = (size_t)buflen;

	if (buflen == 0 || (buflen == -1 &&
			    ((EWOULDBLOCK == errno)
			     || (EAGAIN == errno)
	     ))) {
		freerecvbuf(rb);
		return (buflen);
	} else if (buflen < 0) {
		msyslog(LOG_ERR, "IO: recvfrom(%s) fd=%d: %m",
			socktoa(&rb->recv_srcadr), fd);
		DPRINT(5, ("read_network_packet: fd=%d dropped (bad recvfrom)\n",
			   fd));
		freerecvbuf(rb);
		return (buflen);
	}

	DPRINT(3, ("read_network_packet: fd=%d length %d from %s\n",
		   fd, (int)buflen, socktoa(&rb->recv_srcadr)));

	/*
	 * We used to drop network packets with addresses matching the magic
	 * refclock format here. Now we do the check in the protocol machine,
	 * rejecting any source address that matches an active clock.
	 */

	/*
	** Classic Bug 2672: Some OSes (MacOSX, Linux) don't block spoofed ::1
	*/

	if (AF_INET6 == itf->family) {
		DPRINT(2, ("Got an IPv6 packet, from <%s> (%d) to <%s> (%d)\n",
			   socktoa(&rb->recv_srcadr),
			   IN6_IS_ADDR_LOOPBACK(PSOCK_ADDR6(&rb->recv_srcadr)),
			   socktoa(&itf->sin),
			   !IN6_IS_ADDR_LOOPBACK(PSOCK_ADDR6(&itf->sin))
			   ));

		if (   IN6_IS_ADDR_LOOPBACK(PSOCK_ADDR6(&rb->recv_srcadr))
		    && !IN6_IS_ADDR_LOOPBACK(PSOCK_ADDR6(&itf->sin))
		   ) {
			pkt_count.packets_dropped++;
			DPRINT(2, ("DROPPING that packet\n"));
			freerecvbuf(rb);
			return buflen;
		}
		DPRINT(2, ("processing that packet\n"));
	}

	/*
	 * Got one.  Mark how and when it got here,
	 * put it on the full list and do bookkeeping.
	 */
	rb->dstadr = itf;
	rb->fd = fd;
	rb->recv_time = fetch_packetstamp(&msghdr);

	receive(rb);
	freerecvbuf(rb);

	itf->received++;
	pkt_count.packets_received++;
	return (buflen);
}

/*
 * attempt to handle io
 */
void
io_handler(void)
{
	bool flag;
	sigset_t runMask;
	fd_set rdfdes;
	int nfound;

	/*
	 * Use select() on all input fd's for unlimited
	 * time.  select() will terminate on SIGALARM or on the
	 * reception of input.
	 */
#ifdef ENABLE_DNS_LOOKUP
	pthread_sigmask(SIG_BLOCK, &blockMask, &runMask);
#else
	sigprocmask(SIG_BLOCK, &blockMask, &runMask);
#endif
	flag = sig_flags.sawALRM || sig_flags.sawQuit || sig_flags.sawHUP || \
	  sig_flags.sawDNS;
	if (!flag) {
	  rdfdes = activefds;
	  nfound = pselect(maxactivefd+1, &rdfdes, NULL, NULL, NULL, &runMask);
	} else {
	  nfound = -1;
	  errno = EINTR;
	}
#ifdef ENABLE_DNS_LOOKUP
	pthread_sigmask(SIG_SETMASK, &runMask, NULL);
#else
	sigprocmask(SIG_SETMASK, &runMask, NULL);
#endif  

	if (nfound > 0) {
		input_handler(&rdfdes);
	} else if (nfound == -1 && errno != EINTR) {
		msyslog(LOG_ERR, "IO: select() error: %m");
	}
#   ifdef DEBUG
	else if (debug > 4) { /* SPECIAL DEBUG */
		msyslog(LOG_DEBUG, "IO: select(): nfound=%d, error: %m", nfound);
	} else {
		DPRINT(1, ("select() returned %d: %m\n", nfound));
	}
#   endif /* DEBUG */
}

/*
 * input_handler - receive packets
 */
static void
input_handler(
	fd_set *	fds
	)
{
	int		buflen;
	SOCKET		fd;
	size_t		select_count;
	endpt *		ep;
#ifdef REFCLOCK
	struct refclockio *rp;
	int		saved_errno;
	const char *	clk;
#endif
#ifdef USE_ROUTING_SOCKET
	struct asyncio_reader *	asyncio_reader;
	struct asyncio_reader *	next_asyncio_reader;
#endif

	pkt_count.handler_calls++;
	select_count = 0;

	/*
	 * If we have something to do, freeze a timestamp.
	 * See below for the other cases (nothing left to do or error)
	 */

	++pkt_count.handler_pkts;

#ifdef REFCLOCK
	/*
	 * Check out the reference clocks first, if any
	 */

	for (rp = refio; rp != NULL; rp = rp->next) {
		fd = rp->fd;

		if (!FD_ISSET(fd, fds))
			continue;
		++select_count;
		buflen = read_refclock_packet(fd, rp);
		/*
		 * The first read must succeed after select()
		 * indicates readability, or we've reached
		 * a permanent EOF.  http://bugs.ntp.org/1732
		 * reported ntpd munching CPU after a USB GPS
		 * was unplugged because select was indicating
		 * EOF but ntpd didn't remove the descriptor
		 * from the activefds set.
		 */
		if (buflen < 0 && EAGAIN != errno) {
			saved_errno = errno;
			clk = refclock_name(rp->srcclock);
			errno = saved_errno;
			msyslog(LOG_ERR, "IO: %s read: %m", clk);
			maintain_activefds(fd, true);
		} else if (0 == buflen) {
			clk = refclock_name(rp->srcclock);
			msyslog(LOG_ERR, "IO: %s read EOF", clk);
			maintain_activefds(fd, true);
		} else {
			/* drain any remaining refclock input */
			do {
				buflen = read_refclock_packet(fd, rp);
			} while (buflen > 0);
		}
	}
#endif /* REFCLOCK */

	/*
	 * Loop through the interfaces looking for data to read.
	 */
	for (ep = ep_list; ep != NULL; ep = ep->elink) {
		fd = ep->fd;
		if (FD_ISSET(fd, fds))
			do {
				++select_count;
				buflen = read_network_packet(fd, ep);
			} while (buflen > 0);
	}

#ifdef USE_ROUTING_SOCKET
	/*
	 * scan list of asyncio readers - currently only used for routing sockets
	 */
	asyncio_reader = asyncio_reader_list;

	while (asyncio_reader != NULL) {
		/* callback may unlink and free asyncio_reader */
		next_asyncio_reader = asyncio_reader->link;
		if (FD_ISSET(asyncio_reader->fd, fds)) {
			++select_count;
			(*asyncio_reader->receiver)(asyncio_reader);
		}
		asyncio_reader = next_asyncio_reader;
	}
#endif /* USE_ROUTING_SOCKET */

	/*
	 * Done everything from that select.
	 * If nothing to do, just return.
	 * If an error occurred, complain and return.
	 */
	if (select_count == 0) { /* We really had nothing to do */
#ifdef DEBUG
		if (debug) /* SPECIAL DEBUG */
			msyslog(LOG_DEBUG, "IO: input_handler: select() returned 0");
#endif /* DEBUG */
		return;
	}
	/* We've done our work */
	/* We're done... */
	return;
}


/*
 * find an interface suitable for the src address
 */
endpt *
select_peerinterface(
	struct peer *	peer,
	sockaddr_u *	srcadr,
	endpt *		dstadr
	)
{
	endpt *ep;
	endpt *wild;

#ifndef REFCLOCK
	UNUSED_ARG(peer);
#endif


	wild = ANY_INTERFACE_CHOOSE(srcadr);

	/*
	 * Initialize the peer structure and dance the interface jig.
	 * Reference clocks step the loopback waltz, the others
	 * squaredance around the interface list looking for a buddy. If
	 * the dance peters out, there is always the wildcard interface.
	 * This might happen in some systems and would preclude proper
	 * operation with public key cryptography.
	 */
	if (IS_PEER_REFCLOCK(peer)) {
		ep = loopback_interface;
	} else {
		ep = dstadr;
		if (NULL == ep)
			ep = wild;
	}
	/*
	 * For unicast, we get to find the interface when dstadr is
	 * given to us as the wildcard (ANY_INTERFACE_CHOOSE).
	 */
	if (wild == ep)
		ep = findinterface(srcadr);
	/*
	 * we do not bind to the wildcard interfaces for output
	 * as our (network) source address would be undefined and
	 * crypto will not work without knowing the own transmit address
	 */
	if (ep != NULL && INT_WILDCARD & ep->flags)
		if (!accept_wildcard_if_for_winnt)
			ep = NULL;

	return ep;
}


/*
 * findinterface - find local interface corresponding to address
 */
endpt *
findinterface(
	sockaddr_u *addr
	)
{
	endpt *iface;

	iface = findlocalinterface(addr, INT_WILDCARD, 0);

	if (NULL == iface) {
		DPRINT(4, ("Found no interface for address %s - returning wildcard\n",
			   socktoa(addr)));

		iface = ANY_INTERFACE_CHOOSE(addr);
	} else
		DPRINT(4, ("Found interface #%u %s for address %s\n",
			   iface->ifnum, iface->name, socktoa(addr)));

	return iface;
}

/*
 * findlocalinterface - find local interface corresponding to addr,
 * which does not have any of flags set.  If bast is nonzero, addr is
 * a broadcast address.
 *
 * This code attempts to find the local sending address for an outgoing
 * address by connecting a new socket to destinationaddress:NTP_PORT
 * and reading the sockname of the resulting connect.
 * the complicated sequence simulates the routing table lookup
 * for to first hop without duplicating any of the routing logic into
 * ntpd. preferably we would have used an API call - but its not there -
 * so this is the best we can do here short of duplicating to entire routing
 * logic in ntpd which would be a silly and really unportable thing to do.
 *
 */
static endpt *
findlocalinterface(
	sockaddr_u *	addr,
	int		flags,
	int		bcast
	)
{
	socklen_t	sockaddrlen;
	endpt *		iface;
	sockaddr_u	saddr;
	SOCKET		s;
	int		rtn;
	int		on;

	DPRINT(4, ("Finding interface for addr %s in list of addresses\n",
		   socktoa(addr)));

	s = socket(AF(addr), SOCK_DGRAM, 0);
	if (INVALID_SOCKET == s)
		return NULL;

	/*
	 * If we are looking for broadcast interface we need to set this
	 * socket to allow broadcast
	 */
	if (bcast) {
		on = 1;
		if (SOCKET_ERROR == setsockopt(s, SOL_SOCKET,
						SO_BROADCAST,
						(char *)&on,
						sizeof(on))) {
			close(s);
			return NULL;
		}
	}

	rtn = connect(s, &addr->sa, SOCKLEN(addr));
	if (SOCKET_ERROR == rtn) {
		close(s);
		return NULL;
	}

	sockaddrlen = sizeof(saddr);
	rtn = getsockname(s, &saddr.sa, &sockaddrlen);
	close(s);
	if (SOCKET_ERROR == rtn)
		return NULL;

	DPRINT(4, ("findlocalinterface: kernel maps %s to %s\n",
		   socktoa(addr), socktoa(&saddr)));

	iface = getinterface(&saddr, (uint32_t)flags);

	/*
	 * if we didn't find an exact match on saddr, find the closest
	 * available local address.  This handles the case of the
	 * address suggested by the kernel being excluded by nic rules
	 * or the user's -I and -L options to ntpd.
	 * See http://bugs.ntp.org/1184 and http://bugs.ntp.org/1683
	 * for more background.
	 */
	if (NULL == iface || iface->ignore_packets)
		iface = findclosestinterface(&saddr,
					     (int)(flags | (int)INT_LOOPBACK));

	/* Don't use an interface which will ignore replies */
	if (iface != NULL && iface->ignore_packets)
		iface = NULL;

	return iface;
}


/*
 * findclosestinterface
 *
 * If there are -I/--interface or -L/novirtualips command-line options,
 * or "nic" or "interface" rules in ntp.conf, findlocalinterface() may
 * find the kernel's preferred local address for a given peer address is
 * administratively unavailable to ntpd, and punt to this routine's more
 * expensive search.
 *
 * Find the numerically closest local address to the one connect()
 * suggested.  This matches an address on the same subnet first, as
 * needed by Classic Bug 1184, and provides a consistent choice if
 * there are multiple feasible local addresses, regardless of the
 * order ntpd enumerated them.
 */
endpt *
findclosestinterface(
	sockaddr_u *	addr,
	int		flags
	)
{
	endpt *		ep;
	endpt *		winner;
	sockaddr_u	addr_dist;
	sockaddr_u	min_dist;

	ZERO_SOCK(&min_dist);
	winner = NULL;

	for (ep = ep_list; ep != NULL; ep = ep->elink) {
		if (ep->ignore_packets ||
		    AF(addr) != ep->family ||
		    (unsigned int)flags & ep->flags)
			continue;

		calc_addr_distance(&addr_dist, addr, &ep->sin);
		if (NULL == winner ||
		    -1 == cmp_addr_distance(&addr_dist, &min_dist)) {
			min_dist = addr_dist;
			winner = ep;
		}
	}
	if (NULL == winner)
		DPRINT(4, ("findclosestinterface(%s) failed\n",
			   socktoa(addr)));
	else
		DPRINT(4, ("findclosestinterface(%s) -> %s\n",
			   socktoa(addr), socktoa(&winner->sin)));

	return winner;
}


/*
 * calc_addr_distance - calculate the distance between two addresses,
 *			the absolute value of the difference between
 *			the addresses numerically, stored as an address.
 */
static void
calc_addr_distance(
	sockaddr_u *		dist,
	const sockaddr_u *	a1,
	const sockaddr_u *	a2
	)
{
	uint32_t	a1val;
	uint32_t	a2val;
	uint32_t	v4dist;
	bool	found_greater;
	bool	a1_greater;
	int	i;

	REQUIRE(AF(a1) == AF(a2));

	ZERO_SOCK(dist);
	AF(dist) = AF(a1);

	/* v4 can be done a bit simpler */
	if (IS_IPV4(a1)) {
		a1val = SRCADR(a1);
		a2val = SRCADR(a2);
		v4dist = (a1val > a2val)
			     ? a1val - a2val
			     : a2val - a1val;
		SET_ADDR4(dist, v4dist);

		return;
	}

	found_greater = false;
	a1_greater = false;	/* suppress pot. uninit. warning */
	for (i = 0; i < (int)sizeof(NSRCADR6(a1)); i++) {
		if (!found_greater &&
		    NSRCADR6(a1)[i] != NSRCADR6(a2)[i]) {
			found_greater = true;
			a1_greater = (NSRCADR6(a1)[i] > NSRCADR6(a2)[i]);
		}
		if (!found_greater) {
			NSRCADR6(dist)[i] = 0;
		} else {
			if (a1_greater)
				NSRCADR6(dist)[i] = NSRCADR6(a1)[i] -
						    NSRCADR6(a2)[i];
			else
				NSRCADR6(dist)[i] = NSRCADR6(a2)[i] -
						    NSRCADR6(a1)[i];
		}
	}
}


/*
 * cmp_addr_distance - compare two address distances, returning -1, 0,
 *		       1 to indicate their relationship.
 */
static int
cmp_addr_distance(
	const sockaddr_u *	d1,
	const sockaddr_u *	d2
	)
{
	int	i;

	REQUIRE(AF(d1) == AF(d2));

	if (IS_IPV4(d1)) {
		if (SRCADR(d1) < SRCADR(d2))
			return COMPARE_LESSTHAN;
		else if (SRCADR(d1) == SRCADR(d2))
			return COMPARE_EQUAL;
		else
			return COMPARE_GREATERTHAN;
	}

	for (i = 0; i < (int)sizeof(NSRCADR6(d1)); i++) {
		if (NSRCADR6(d1)[i] < NSRCADR6(d2)[i])
			return COMPARE_LESSTHAN;
		else if (NSRCADR6(d1)[i] > NSRCADR6(d2)[i])

		    return COMPARE_GREATERTHAN;
	}

	return COMPARE_EQUAL;
}



/*
 * fetch an interface structure the matches the
 * address and has the given flags NOT set
 */
endpt *
getinterface(
	sockaddr_u *	addr,
	uint32_t		flags
	)
{
	endpt *iface;

	iface = find_addr_in_list(addr);

	if (iface != NULL && (iface->flags & flags))
		iface = NULL;

	return iface;
}

/*
 * io_clr_stats - clear I/O module statistics
 */
void
io_clr_stats(void)
{
	pkt_count.packets_dropped = 0;
	pkt_count.packets_ignored = 0;
	pkt_count.packets_received = 0;
	pkt_count.packets_sent = 0;
	pkt_count.packets_notsent = 0;

	pkt_count.handler_calls = 0;
	pkt_count.handler_pkts = 0;
	pkt_count.io_timereset = current_time;
}


#ifdef REFCLOCK
/*
 * io_addclock - add a reference clock to the list and arrange that we
 *				 get SIGIO interrupts from it.
 */
bool
io_addclock(
	struct refclockio *rio
	)
{
	/*
	 * Stuff the I/O structure in the list and mark the descriptor
	 * in use.  There is a harmless (I hope) race condition here.
	 */
	rio->active = true;

	/*
	 * enqueue
	 */
	LINK_SLIST(refio, rio, next);

	/*
	 * register fd
	 */
	add_fd_to_list(rio->fd, FD_TYPE_FILE);

	return true;
}


/*
 * io_closeclock - close the clock in the I/O structure given
 */
void
io_closeclock(
	struct refclockio *rio
	)
{
	struct refclockio *unlinked;

	/*
	 * Remove structure from the list
	 */
	rio->active = false;
	UNLINK_SLIST(unlinked, refio, rio, next, struct refclockio);
	if (NULL != unlinked) {
		/*
		 * Close the descriptor.
		 */
		close_and_delete_fd_from_list(rio->fd);
	}
	rio->fd = -1;
}
#endif	/* REFCLOCK */


/*
 * Add and delete functions for the list of open sockets
 */
static void
add_fd_to_list(
	SOCKET fd,
	enum desc_type type
	)
{
	vsock_t *lsock = emalloc(sizeof(*lsock));

	lsock->fd = fd;
	lsock->type = type;

	LINK_SLIST(fd_list, lsock, link);
	maintain_activefds(fd, false);
}


static void
close_and_delete_fd_from_list(
	SOCKET fd
	)
{
	vsock_t *lsock;

	UNLINK_EXPR_SLIST(lsock, fd_list, fd ==
	    UNLINK_EXPR_SLIST_CURRENT()->fd, link, vsock_t);

	if (NULL == lsock)
		return;

	switch (lsock->type) {

	case FD_TYPE_SOCKET:
		close(lsock->fd);
		break;

	case FD_TYPE_FILE:
		close(lsock->fd);
		break;

	default:
		msyslog(LOG_ERR,
			"IO: internal error - illegal descriptor type %d - EXITING",
			(int)lsock->type);
		exit(1);
	}

	free(lsock);
	/*
	 * remove from activefds
	 */
	maintain_activefds(fd, true);
}


static void
add_addr_to_list(
	sockaddr_u *	addr,
	endpt *		ep
	)
{
	remaddr_t *laddr;

#ifdef DEBUG
	if (find_addr_in_list(addr) == NULL) {
#endif
		/* not there yet - add to list */
		laddr = emalloc(sizeof(*laddr));
		laddr->addr = *addr;
		laddr->ep = ep;

		LINK_SLIST(remoteaddr_list, laddr, link);

		DPRINT(4, ("Added addr %s to list of addresses\n",
			   socktoa(addr)));
#ifdef DEBUG
	} else
		DPRINT(4, ("WARNING: Attempt to add duplicate addr %s to address list\n",
			   socktoa(addr)));
#endif
}


static void
delete_interface_from_list(
	endpt *iface
	)
{
	remaddr_t *unlinked;

	for (;;) {
		UNLINK_EXPR_SLIST(unlinked, remoteaddr_list, iface ==
		    UNLINK_EXPR_SLIST_CURRENT()->ep, link,
		    remaddr_t);

		if (unlinked == NULL)
			break;
		DPRINT(4, ("Deleted addr %s for interface #%u %s "
			   "from list of addresses\n",
			   socktoa(&unlinked->addr), iface->ifnum,
			   iface->name));
		free(unlinked);
	}
}


static endpt *
find_addr_in_list(
	sockaddr_u *addr
	)
{
	remaddr_t *entry;

	DPRINT(4, ("Searching for addr %s in list of addresses - ",
		   socktoa(addr)));

	for (entry = remoteaddr_list;
	     entry != NULL;
	     entry = entry->link)
		if (SOCK_EQ(&entry->addr, addr)) {
			DPRINT(4, ("FOUND\n"));
			return entry->ep;
		}

	DPRINT(4, ("NOT FOUND\n"));
	return NULL;
}

const char *
latoa(
	endpt *la
	)
{
	return (NULL == la)
		   ? "<null>"
		   : socktoa(&la->sin);
}


#ifdef USE_ROUTING_SOCKET
# ifndef UPDATE_GRACE
#  define UPDATE_GRACE	2	/* wait UPDATE_GRACE seconds before scanning */
# endif

static void
process_routing_msgs(struct asyncio_reader *reader)
{
	char buffer[5120];
	ssize_t cnt;
        int msg_type;
#ifdef HAVE_LINUX_RTNETLINK_H
	struct nlmsghdr *nh;
#else
	struct rt_msghdr rtm;
	char *p;
#endif

	if (disable_dynamic_updates) {
		/*
		 * discard ourselves if we are not needed any more
		 * usually happens when running unprivileged
		 */
		remove_asyncio_reader(reader);
		delete_asyncio_reader(reader);
		return;
	}

	cnt = read(reader->fd, buffer, sizeof(buffer));

	if (cnt < 0) {
		if (errno == ENOBUFS) {
			msyslog(LOG_ERR,
				"IO: routing socket reports: %m");
		} else {
			msyslog(LOG_ERR,
				"IO: routing socket reports: %m - disabling");
			remove_asyncio_reader(reader);
			delete_asyncio_reader(reader);
		}
		return;
	}

	/*
	 * process routing message
	 */
#ifdef HAVE_LINUX_RTNETLINK_H
	for (nh = (struct nlmsghdr *)buffer;
	     NLMSG_OK(nh, (unsigned) cnt);
	     nh = NLMSG_NEXT(nh, cnt)) {
		msg_type = nh->nlmsg_type;
#else
	for (p = buffer;
	     (p + sizeof(struct rt_msghdr)) <= (buffer + cnt);
	     p += rtm.rtm_msglen) {
		memcpy(&rtm, p, sizeof(rtm));
		if (rtm.rtm_version != RTM_VERSION) {
			msyslog(LOG_ERR,
				"IO: version mismatch (got %d - expected %d) on routing socket - disabling",
				rtm.rtm_version, RTM_VERSION);

			remove_asyncio_reader(reader);
			delete_asyncio_reader(reader);
			return;
		}
		msg_type = rtm.rtm_type;
#endif
		switch (msg_type) {
#ifdef RTM_NEWADDR
		case RTM_NEWADDR:
#endif
#ifdef RTM_DELADDR
		case RTM_DELADDR:
#endif
#ifdef RTM_ADD
		case RTM_ADD:
#endif
#ifdef RTM_DELETE
		case RTM_DELETE:
#endif
#ifdef RTM_REDIRECT
		case RTM_REDIRECT:
#endif
#ifdef RTM_CHANGE
		case RTM_CHANGE:
#endif
#ifdef RTM_LOSING
		case RTM_LOSING:
#endif
#ifdef RTM_IFINFO
		case RTM_IFINFO:
#endif
#ifdef RTM_IFANNOUNCE
		case RTM_IFANNOUNCE:
#endif
#ifdef RTM_NEWLINK
		case RTM_NEWLINK:
#endif
#ifdef RTM_DELLINK
		case RTM_DELLINK:
#endif
#ifdef RTM_NEWROUTE
		case RTM_NEWROUTE:
#endif
#ifdef RTM_DELROUTE
		case RTM_DELROUTE:
#endif
			/*
			 * we are keen on new and deleted addresses and
			 * if an interface goes up and down or routing
			 * changes
			 */
			DPRINT(3, ("routing message op = %d: scheduling interface update\n",
				   msg_type));
			timer_interfacetimeout(current_time + UPDATE_GRACE);
			break;
#ifdef HAVE_LINUX_RTNETLINK_H
		case NLMSG_DONE:
			/* end of multipart message */
			return;
#endif
		default:
			/*
			 * the rest doesn't bother us.
			 */
			DPRINT(4, ("routing message op = %d: ignored\n",
				   msg_type));
			break;
		}
	}
}

/*
 * set up routing notifications
 */
static void
init_async_notifications()
{
	struct asyncio_reader *reader;
#ifdef HAVE_LINUX_RTNETLINK_H
	int fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	struct sockaddr_nl sa;
#else
	int fd = socket(PF_ROUTE, SOCK_RAW, 0);
#endif
	if (fd < 0) {
		msyslog(LOG_ERR,
			"IO: unable to open routing socket (%m) - using polled interface update");
		return;
	}

	fd = move_fd(fd);
#ifdef HAVE_LINUX_RTNETLINK_H
	ZERO(sa);
	sa.nl_family = PF_NETLINK;
	sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR
		       | RTMGRP_IPV6_IFADDR | RTMGRP_IPV4_ROUTE
		       | RTMGRP_IPV4_MROUTE | RTMGRP_IPV6_ROUTE
		       | RTMGRP_IPV6_MROUTE;
	if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		msyslog(LOG_ERR,
			"IO: bind failed on routing socket (%m) - using polled interface update");
		return;
	}
#endif
	make_socket_nonblocking(fd);

	reader = new_asyncio_reader();

	reader->fd = fd;
	reader->receiver = process_routing_msgs;

	add_asyncio_reader(reader, FD_TYPE_SOCKET);
	msyslog(LOG_INFO,
		"IO: Listening on routing socket on fd #%d for interface updates",
		fd);
}
#else
/* USE_ROUTING_SOCKET not defined */
static void
init_async_notifications(void)
{
}
#endif
