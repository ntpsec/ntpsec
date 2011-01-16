#include <config.h>

#include "main.h"
#include "kod_management.h"
#include "networking.h"
#include "utilities.h"
#include "log.h"
#include "libntp.h"

#include <event2/dns.h>
#include <event2/util.h>
#include <event2/event.h>

int n_pending_dns = 0;
int n_pending_ntp = 0;
int ai_fam_pref = AF_UNSPEC;
int ntpver = 4;
double steplimit = -1;
int xmt_delay;
SOCKET sock4 = -1;		/* Socket for IPv4 */
SOCKET sock6 = -1;		/* Socket for IPv6 */
char *progname;
struct event_base *base = NULL;
struct evdns_base *dnsbase = NULL;

struct dns_ctx {
	const char *	name;
	int		flags;
#define CTX_BCST	0x0001
#define CTX_UCST	0x0002
#define CTX_CONC	0x0004
#define CTX_unused	0xfffd
	int		key_id;
	struct timeval	timeout;
	struct key *	key;
};

struct xmt_ctx {
	SOCKET			sock;
	struct dns_ctx *	dctx;
	struct ntp_ctx *	nctx;
};

struct ntp_ctx {
	struct dns_ctx *	dctx;
	struct evutil_addrinfo *ai;
	struct pkt		x_pkt;
};

struct key *keys = NULL;
struct timeval timeout_tv;

struct timeval ucst_timeout_tv = { 5, 0 };

static union {
	struct pkt pkt;
	char   buf[1500];
} rbuf;

#define r_pkt  rbuf.pkt

void open_sockets( void );
void handle_lookup( const char *name, int flags );
void dns_cb (int errcode, struct evutil_addrinfo *addr, void *ptr);
void queue_xmt( SOCKET sock, struct dns_ctx *dctx, struct ntp_ctx *nctx );
void xmt_cb( evutil_socket_t, short, void *ptr );
int checkKoD( struct evutil_addrinfo *ai );
void ntp_cb (evutil_socket_t, short, void *);
void set_li_vn_mode (struct pkt *spkt, char leap, char version, char mode);
int sntp_main (int argc, char **argv);
int on_wire (struct addrinfo *host, struct addrinfo *bcastaddr);
int set_time (double offset);

#define NORMALIZE_TIMEVAL(tv)				\
do {							\
	while ((tv).tv_usec < 0) {			\
		(tv).tv_usec += 1000000;		\
		(tv).tv_sec--;				\
	}						\
	while ((tv).tv_usec > 999999) {			\
		(tv).tv_usec -= 1000000;		\
		(tv).tv_sec++;				\
	}						\
} while (0)


/*
 * The actual main function.
 */
int
sntp_main (
	int argc,
	char **argv
	)
{
	int i;
	int optct;
	long l;

	progname = argv[0];

	optct = optionProcess(&sntpOptions, argc, argv);
	argc -= optct;
	argv += optct;

#ifdef DEBUG
	debug = DESC(DEBUG_LEVEL).optOccCt;
	DPRINTF(1, ("%s\n", Version));
#endif

	if (atoint(OPT_ARG(NTPVERSION), &l))
		ntpver = l;

	if (atoint(OPT_ARG(STEPLIMIT), &l))
		steplimit = (double)l / 1000;

	/* Initialize logging system */
	init_logging();
	if (HAVE_OPT(FILELOG))
		open_logfile(OPT_ARG(FILELOG));

	if (0 == argc && !HAVE_OPT(BROADCAST) && !HAVE_OPT(CONCURRENT)) {
		printf("%s: Must supply at least one of -b hostname, -c hostname, or hostname.\n", progname);
		exit(EX_USAGE);
	}

	/*
	** Eventually, we probably want:
	** - separate bcst and ucst timeouts
	** - multiple --timeout values in the commandline
	*/
	if (atoint(OPT_ARG(BCTIMEOUT), &l))
		timeout_tv.tv_sec = l;
	else 
		timeout_tv.tv_sec = 68; /* ntpd broadcasts every 64s */
	timeout_tv.tv_usec = 0;

	/* IPv6 available? */
	if (isc_net_probeipv6() != ISC_R_SUCCESS) {
		ai_fam_pref = AF_INET;
		DPRINTF(1, ("No ipv6 support available, forcing ipv4\n"));
	} else {
		/* Check for options -4 and -6 */
		if (HAVE_OPT(IPV4))
			ai_fam_pref = AF_INET;
		else if (HAVE_OPT(IPV6))
			ai_fam_pref = AF_INET6;
	}

	/* TODO: Parse config file if declared */

	/*
	** Init the KOD system.
	** For embedded systems with no writable filesystem,
	** -K /dev/null can be used to disable KoD storage.
	*/
	kod_init_kod_db(OPT_ARG(KOD));

	// HMS: Should we use arg-defalt for this too?
	if (HAVE_OPT(KEYFILE))
		auth_init(OPT_ARG(KEYFILE), &keys);

	/*
	** Considering employing a variable that prevents functions of doing
	** anything until everything is initialized properly
	**
	** HMS: What exactly does the above mean?
	*/

	base = event_base_new();
	if (!base) {
		printf("%s: event_base_new() failed!\n", progname);
		return -1;
	}

	dnsbase = evdns_base_new(base, 1);
	if (!dnsbase) {
		printf("%s: evdns_base_new() failed!\n", progname);
		return -1;
	}

	open_sockets();

	if (HAVE_OPT(BROADCAST)) {
		int		cn = STACKCT_OPT( BROADCAST );
		const char **	cp = STACKLST_OPT( BROADCAST );

		while (cn-- > 0) {
			handle_lookup(*cp, CTX_BCST);
			cp++;
		}
	}

	if (HAVE_OPT(CONCURRENT)) {
		int		cn = STACKCT_OPT( CONCURRENT );
		const char **	cp = STACKLST_OPT( CONCURRENT );

		while (cn-- > 0) {
			handle_lookup(*cp, CTX_UCST|CTX_CONC);
			cp++;
		}
	}

	for (i = 0; i < argc; ++i) {
		handle_lookup(argv[i], CTX_UCST);
	}

	event_base_dispatch(base);

	evdns_base_free(dnsbase, 0);
	event_base_free(base);

	return 0;		/* Might not want 0... */
}


/*
** open sockets and make them non-blocking
*/
void
open_sockets(
	void
	)
{
	sockaddr_u name;

	if (-1 == sock4) {
		sock4 = socket(PF_INET, SOCK_DGRAM, 0);
		if (-1 == sock4) {
			/* error getting a socket */
			msyslog(LOG_ERR, "open_sockets: socket(PF_INET) failed: %m");
			exit(1);
		}
		/* Make it non-blocking */
		make_socket_nonblocking(sock4);

		/* Let's try using a wildcard... */
		ZERO(name);
		AF(&name) = AF_INET;
		SET_ADDR4N(&name, INADDR_ANY);
		SET_PORT(&name, (HAVE_OPT(USERESERVEDPORT) ? 123 : 0));

		if (-1 == bind(sock4, &name.sa,
			       SOCKLEN(&name))) {
			msyslog(LOG_ERR, "open_sockets: bind(sock4) failed: %m");
			exit(1);
		}
	}

	/* We may not always have IPv6... */
	if (-1 == sock6) {
		sock6 = socket(PF_INET6, SOCK_DGRAM, 0);
		if (-1 == sock6) {
			/* error getting a socket */
			msyslog(LOG_ERR, "open_sockets: socket(PF_INET6) failed: %m");
			exit(1);
		}
		/* Make it non-blocking */
		make_socket_nonblocking(sock6);

		/* Let's try using a wildcard... */
		ZERO(name);
		AF(&name) = AF_INET6;
		SET_ADDR6N(&name, in6addr_any);
		SET_PORT(&name, (HAVE_OPT(USERESERVEDPORT) ? 123 : 0));

		if (-1 == bind(sock6, &name.sa,
			       SOCKLEN(&name))) {
			msyslog(LOG_ERR, "open_sockets: bind(sock6) failed: %m");
			exit(1);
		}
	}
	
	return;
}


/*
** handle_lookup
*/
void
handle_lookup(
	const char *name,
	int flags
	)
{
	struct evutil_addrinfo hints;	/* Local copy is OK */
	struct dns_ctx *dns_ctx;
	long l;

	DPRINTF(1, ("handle_lookup(%s,%#x)\n", name, flags));

	ZERO(hints);
	hints.ai_family = ai_fam_pref;
	hints.ai_flags |= EVUTIL_AI_CANONNAME;
	hints.ai_flags |= EVUTIL_AI_NUMERICSERV;
	/*
	** Unless we specify a socktype, we'll get at least two
	** entries for each address: one for TCP and one for
	** UDP. That's not what we want.
	*/
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	dns_ctx = emalloc(sizeof(*dns_ctx));
	memset(dns_ctx, 0, sizeof(*dns_ctx));

	dns_ctx->name = name;
	dns_ctx->flags = flags;
	dns_ctx->timeout =
		(flags & CTX_BCST)
		? timeout_tv
		: ucst_timeout_tv
		;

	// The following should arguably be passed in...
	if (ENABLED_OPT(AUTHENTICATION) &&
	    atoint(OPT_ARG(AUTHENTICATION), &l)) {
		dns_ctx->key_id = l;
		get_key(dns_ctx->key_id, &dns_ctx->key);
	} else {
		dns_ctx->key_id = -1;
		dns_ctx->key = NULL;
	}

	++n_pending_dns;
	evdns_getaddrinfo(dnsbase, name, "123", &hints, dns_cb, dns_ctx);
}


/*
** DNS Callback:
** - For each IP:
** - - open a socket
** - - increment n_pending_ntp
** - - send a request if this is a Unicast callback
** - - queue wait for response
** - decrement n_pending_dns
*/
void
dns_cb(
	int errcode,
	struct evutil_addrinfo *addr,
	void *ptr
	)
{
	struct dns_ctx *dctx = ptr;
	struct ntp_ctx *nctx;

	if (errcode) {
		printf("%s -> %s\n", dctx->name, evutil_gai_strerror(errcode));
	} else {
		struct event *ev;
		struct evutil_addrinfo *ai;

		if (debug > 2)
			printf("%s [%s]\n", dctx->name,
			       (addr->ai_canonname)
			       ? addr->ai_canonname
			       : "");

		xmt_delay = 0;
		for (ai = addr; ai; ai = ai->ai_next) {
			SOCKET sock;

			if (checkKoD(ai))
				continue;

			nctx = emalloc((sizeof *nctx));
			memset(nctx, 0, sizeof *nctx);

			nctx->dctx = dctx;
			nctx->ai = ai;

			switch (ai->ai_family) {
			case AF_INET:
				sock = sock4;
				break;
			case AF_INET6:
				sock = sock6;
				break;
			default:
				msyslog(LOG_ERR, "dns_cb: unexpected ai_family: %d",
					ai->ai_family);
				exit(1);
				break;
			}

			/*
			** We're waiting for a response for either unicast
			** or broadcast, so...
			*/
			++n_pending_ntp;

			/* If this is for a unicast IP, queue a request */
			if (dctx->flags & CTX_UCST)
				queue_xmt(sock, dctx, nctx);

			/*
			** Register an NTP callback for the response
			** or broadcast.
			*/

			/*
			** Do we need separate events if we are using
			** a wildcard socket for responses?
			*/
			ev = event_new(base, sock,
				EV_TIMEOUT|EV_READ|EV_PERSIST,
				ntp_cb, nctx);
			if (NULL == ev) {
				msyslog(LOG_ERR,
					"dns_cb: event_new(base, sock) failed!");
				--n_pending_ntp;
				/* What now? */
			} else {
				event_add(ev, &(dctx->timeout));
			}
		}
		evutil_freeaddrinfo(addr);
		free(ptr);
	}

	/* n_pending_dns really should be >0 here... */
	if (--n_pending_dns == 0 && n_pending_ntp == 0)
		event_base_loopexit(base, NULL);
}


/*
** queue_xmt
*/
void
queue_xmt(
	SOCKET sock,
	struct dns_ctx *dctx,
	struct ntp_ctx *nctx
	)
{
	struct event *ev;
	struct xmt_ctx *xctx;
	struct timeval cb_tv = { 2 * xmt_delay, 0 };

	xctx = emalloc(sizeof(*xctx));
	memset(xctx, 0, sizeof(*xctx));

	xctx->sock = sock;
	xctx->dctx = dctx;
	xctx->nctx = nctx;

	/*
	** xmt_delay increases, starting with 0.
	** Queue an event for xmt_delay*2 seconds from now,
	** and when it "fires" send off a packet to the target.
	*/
	ev = evtimer_new(base, &xmt_cb, xctx);
	if (NULL == ev) {
		msyslog(LOG_ERR, "queue_xmt: evtimer_new() failed!");
		exit(1);
	}

	++xmt_delay;

	evtimer_add(ev, &cb_tv);

	return;
}


/*
** xmt_cb
*/
void
xmt_cb(
	evutil_socket_t fd,
	short what,
	void *ptr
	)
{
	struct xmt_ctx *xctx = ptr;
	SOCKET		sock = xctx->sock;
	struct dns_ctx *dctx = xctx->dctx;
	struct ntp_ctx *nctx = xctx->nctx;
	struct evutil_addrinfo *ai = nctx->ai;
	struct timeval	tv_xmt;
	struct pkt	x_pkt;
	int		pkt_len;

	if (0 != GETTIMEOFDAY(&tv_xmt, NULL)) {
		msyslog(LOG_ERR,
			"xmt_cb: gettimeofday() failed: %m");
		exit(1);
	}
	tv_xmt.tv_sec += JAN_1970;

	pkt_len = generate_pkt(&x_pkt, &tv_xmt,
			dctx->key_id, dctx->key);

	/* The current sendpkt does not return status */
	sendpkt(sock, (sockaddr_u *)ai->ai_addr, &x_pkt, pkt_len);
	/* Save the packet we sent... */
	memcpy(&(nctx->x_pkt), &x_pkt, pkt_len);

	DPRINTF(2, ("xmt_cb: UCST: Current time sec: %i msec: %i\n", (unsigned int) tv_xmt.tv_sec, (unsigned int) tv_xmt.tv_usec));

	/*
	** If the send fails:
	** - decrement n_pending_ntp
	** - restart the loop
	*/

	return;
}


/*
** checkKoD
*/
int
checkKoD(
	struct evutil_addrinfo *ai
	)
{
	char *hostname;
	struct kod_entry *reason;

	/* Is there a KoD on file for this address? */
	hostname = addrinfo_to_str(ai);
	DPRINTF(2, ("checkKoD: checking <%s>\n", hostname));
	if (search_entry(hostname, &reason)) {
		printf("prior KoD for %s, skipping.\n",
			hostname);
		free(reason);
		free(hostname);

		return 1;
	}
	free(hostname);

	return 0;
}


/*
** NTP Callback:
** Read in the packet
** Unicast:
** - close socket
** - decrement n_pending_ntp
** - If packet is good, set the time and "exit"
** Broadcast:
** - If packet is good, set the time and "exit"
*/
void
ntp_cb(
	evutil_socket_t fd,
	short what,
	void *ptr
	)
{
	struct ntp_ctx *nctx = ptr;
	int rpktl;
	int rc;

	if (debug > 1)
	    printf("ntp_cb: event on socket %d:%s%s%s%s [%s%s] <%s>\n",
		(int) fd,
		(what & EV_TIMEOUT) ? " timeout" : "",
		(what & EV_READ)    ? " read" : "",
		(what & EV_WRITE)   ? " write" : "",
		(what & EV_SIGNAL)  ? " signal" : "",
		(nctx->dctx->flags & CTX_BCST)	? "BCST" : "",
		(nctx->dctx->flags & CTX_UCST)	? "UCST" : "",
		nctx->dctx->name
		);

	/* Read in the packet */
	rpktl = recvpkt(fd, &r_pkt, sizeof rbuf,
		(nctx->dctx->flags & CTX_UCST) ? &(nctx->x_pkt) :  0);

	DPRINTF(2, ("ntp_cb: recvpkt returned %x\n", rpktl));

	/* If this is a Unicast packet, we're done ... */
	if (nctx->dctx->flags & CTX_UCST) {
		/* Only close() if we use a separate socket for each response */
		// close(fd);
		--n_pending_ntp;
	}

	/* If the packet is good, set the time and we're all done */
	rc = handle_pkt(rpktl, &r_pkt, nctx->ai);

	switch (rc) {
	case 0:
		// Should clean up and exit...
		exit(0);
		break;
	default:
		printf("ntp_cb: handle_pkt() returned %d\n", rc);
		break;
	}

	event_base_loopexit(base, NULL);
}


int
generate_pkt (
	struct pkt *x_pkt,
	const struct timeval *tv_xmt,
	int key_id,
	struct key *pkt_key
	)
{
	l_fp xmt;
	int pkt_len = LEN_PKT_NOMAC;

	memset(x_pkt, 0, sizeof(struct pkt));
	TVTOTS(tv_xmt, &xmt);
	HTONL_FP(&xmt, &(x_pkt->xmt));
	x_pkt->stratum = STRATUM_TO_PKT(STRATUM_UNSPEC);
	x_pkt->ppoll = 8;
	/* FIXME! Modus broadcast + adr. check -> bdr. pkt */
	set_li_vn_mode(x_pkt, LEAP_NOTINSYNC, ntpver, 3);
	if (pkt_key != NULL) {
		int mac_size = 20; /* max room for MAC */

		x_pkt->exten[0] = htonl(key_id);
		mac_size = make_mac((char *)x_pkt, pkt_len, mac_size,
				    pkt_key, (char *)&x_pkt->exten[1]);
		if (mac_size)
			pkt_len += mac_size + 4;
	}
	return pkt_len;
}


int
handle_pkt (
	int rpktl,
	struct pkt *rpkt,
	struct addrinfo *host
	)
{
	struct timeval tv_dst;
	int sw_case, digits;
	char *hostname = NULL, *ref, *ts_str = NULL;
	double offset, precision, root_dispersion;
	char addr_buf[INET6_ADDRSTRLEN];
	char *p_SNTP_PRETEND_TIME;
	time_t pretend_time;
#if SIZEOF_TIME_T == 8
	long long ll;
#else
	long l;
#endif

	if (rpktl > 0)
		sw_case = 1;
	else
		sw_case = rpktl;

	switch(sw_case) {
	    case SERVER_UNUSEABLE:
		return -1;
		break;

	    case PACKET_UNUSEABLE:
		break;

	    case SERVER_AUTH_FAIL:
		break;

	    case KOD_DEMOBILIZE:
		/* Received a DENY or RESTR KOD packet */
		hostname = addrinfo_to_str(host);
		ref = (char *)&rpkt->refid;
		add_entry(hostname, ref);

		if (debug)
			printf("sntp handle_pkt: Received KOD packet with code: %c%c%c%c from %s, demobilizing all connections\n",
				   ref[0], ref[1], ref[2], ref[3],
				   hostname);

		msyslog(LOG_WARNING, "Received a KOD packet with code %c%c%c%c from %s, demobilizing all connections",
			ref[0], ref[1], ref[2], ref[3], hostname);
		break;

	    case KOD_RATE:
		/*
		** Hmm...
		** We should probably call add_entry() with an
		** expiration timestamp of several seconds in the future,
		** and back-off even more if we get more RATE responses.
		*/
		break;

	    case 1:
		if (debug > 2) {
			getnameinfo(host->ai_addr, host->ai_addrlen, addr_buf,
				sizeof(addr_buf), NULL, 0, NI_NUMERICHOST);
			printf("sntp handle_pkt: Received %i bytes from %s\n",
			       rpktl, addr_buf);
		}

		GETTIMEOFDAY(&tv_dst, (struct timezone *)NULL);

		p_SNTP_PRETEND_TIME = getenv("SNTP_PRETEND_TIME");
		if (p_SNTP_PRETEND_TIME) {
			pretend_time = 0;
#if SIZEOF_TIME_T == 4
			if (1 == sscanf(p_SNTP_PRETEND_TIME, "%ld", &l))
				pretend_time = (time_t)l;
#elif SIZEOF_TIME_T == 8
			if (1 == sscanf(p_SNTP_PRETEND_TIME, "%lld", &ll))
				pretend_time = (time_t)ll;
#else
# include "GRONK: unexpected value for SIZEOF_TIME_T"
#endif
			if (0 != pretend_time)
				tv_dst.tv_sec = pretend_time;
		}

		offset_calculation(rpkt, rpktl, &tv_dst, &offset,
				   &precision, &root_dispersion);

		for (digits = 0; (precision *= 10.) < 1.; ++digits)
			/* empty */ ;
		if (digits > 6)
			digits = 6;

		ts_str = tv_to_str(&tv_dst);
		printf("%s ", ts_str);
		if (offset > 0)
			printf("+");
		printf("%.*f", digits, offset);
		if (root_dispersion > 0.)
			printf(" +/- %f secs", root_dispersion);
		printf("\n");
		free(ts_str);

		if (p_SNTP_PRETEND_TIME)
			return 0;

		if (ENABLED_OPT(STEP) || ENABLED_OPT(SLEW))
			return set_time(offset);

		return 0;
	}

	return 1;
}


void
offset_calculation (
	struct pkt *rpkt,
	int rpktl,
	struct timeval *tv_dst,
	double *offset,
	double *precision,
	double *root_dispersion
	)
{
	l_fp p_rec, p_xmt, p_ref, p_org, tmp, dst;
	u_fp p_rdly, p_rdsp;
	double t21, t34, delta;

	/* Convert timestamps from network to host byte order */
	p_rdly = NTOHS_FP(rpkt->rootdelay);
	p_rdsp = NTOHS_FP(rpkt->rootdisp);
	NTOHL_FP(&rpkt->reftime, &p_ref);
	NTOHL_FP(&rpkt->org, &p_org);
	NTOHL_FP(&rpkt->rec, &p_rec);
	NTOHL_FP(&rpkt->xmt, &p_xmt);

	*precision = LOGTOD(rpkt->precision);
	DPRINTF(3, ("offset_calculation: precision: %f", *precision));

	*root_dispersion = FPTOD(p_rdsp);

#ifdef DEBUG
	if (debug > 2) {
		printf("sntp rootdelay: %f\n", FPTOD(p_rdly));
		printf("sntp rootdisp: %f\n", *root_dispersion);

		pkt_output(rpkt, rpktl, stdout);

		printf("sntp offset_calculation: rpkt->reftime:\n");
		l_fp_output(&(rpkt->reftime), stdout);
		printf("sntp offset_calculation: rpkt->org:\n");
		l_fp_output(&(rpkt->org), stdout);
		printf("sntp offset_calculation: rpkt->rec:\n");
		l_fp_output(&(rpkt->rec), stdout);
		printf("sntp offset_calculation: rpkt->rec:\n");
		l_fp_output_bin(&(rpkt->rec), stdout);
		printf("sntp offset_calculation: rpkt->rec:\n");
		l_fp_output_dec(&(rpkt->rec), stdout);
		printf("sntp offset_calculation: rpkt->xmt:\n");
		l_fp_output(&(rpkt->xmt), stdout);
	}
#endif

	/* Compute offset etc. */
	tmp = p_rec;
	L_SUB(&tmp, &p_org);
	LFPTOD(&tmp, t21);
	TVTOTS(tv_dst, &dst);
	dst.l_ui += JAN_1970;
	tmp = p_xmt;
	L_SUB(&tmp, &dst);
	LFPTOD(&tmp, t34);
	*offset = (t21 + t34) / 2.;
	delta = t21 - t34;

	DPRINTF(3, ("sntp offset_calculation:\tt21: %.6f\t\t t34: %.6f\n\t\tdelta: %.6f\t offset: %.6f\n", t21, t34, delta, *offset));
}



/* Compute the 8 bits for li_vn_mode */
void
set_li_vn_mode (
	struct pkt *spkt,
	char leap,
	char version,
	char mode
	)
{
	if (leap > 3) {
		msyslog(LOG_DEBUG, "set_li_vn_mode: leap > 3, using max. 3");
		leap = 3;
	}

	if (version < 0 || version > 7) {
		msyslog(LOG_DEBUG, "set_li_vn_mode: version < 0 or > 7, using 4");
		version = 4;
	}

	if (mode > 7) {
		msyslog(LOG_DEBUG, "set_li_vn_mode: mode > 7, using client mode 3");
		mode = 3;
	}

	spkt->li_vn_mode  = leap << 6;
	spkt->li_vn_mode |= version << 3;
	spkt->li_vn_mode |= mode;
}


/*
** set_time corrects the local clock by offset with either
** settimeofday() oradjtime()/adjusttimeofday().
*/
int
set_time(
	double offset
	)
{
	struct timeval tp;

	/* check offset against steplimit */
	if (fabs(offset) > steplimit || !ENABLED_OPT(SLEW)) {
		if (ENABLED_OPT(STEP)) {
			GETTIMEOFDAY(&tp, NULL);

			tp.tv_sec += (long)offset;
			tp.tv_usec += 1e6 * (offset - (long)offset);
			NORMALIZE_TIMEVAL(tp);

			if (SETTIMEOFDAY(&tp, NULL) < 0) {
				msyslog(LOG_ERR, "Time not set: settimeofday(): %m");
				return -1;
			}
			return 0;
		}
	}

	if (ENABLED_OPT(SLEW)) {
		tp.tv_sec = (long)offset;
		tp.tv_usec = 1e6 * (offset - (long)offset);
		NORMALIZE_TIMEVAL(tp);

		if (ADJTIMEOFDAY(&tp, NULL) < 0) {
			msyslog(LOG_ERR, "Time not set: adjtime(): %m");
			return -1;
		}
		return 0;
	}

	return -1;
}
