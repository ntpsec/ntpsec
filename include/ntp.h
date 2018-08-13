/*
 * ntp.h - NTP definitions for the masses
 *
 * pythonize-header: start ignoring
 */
#ifndef GUARD_NTP_H
#define GUARD_NTP_H

#include <stddef.h>
#include <math.h>

#include "ntp_fp.h"
#include "ntp_types.h"
#include "ntp_lists.h"
#include "ntp_stdlib.h"
#include "ntp_net.h"

extern int32_t ntp_random (void);
extern uint64_t ntp_random64 (void);

/*
 * Calendar arithmetic - contributed by G. Healton
 */
#define YEAR_BREAK 500		/* years < this are tm_year values:
				 * Break < AnyFourDigitYear && Break >
				 * Anytm_yearYear */

#define YEAR_PIVOT 98		/* 97/98: years < this are year 2000+
				 * FYI: official UNIX pivot year is
				 * 68/69 */

/*
 * Number of Days since 1 BC Gregorian to 1 January of given year
 */
#define julian0(year)	(((year) * 365 ) + ((year) > 0 ? (((year) + 3) \
			    / 4 - ((year - 1) / 100) + ((year - 1) / \
			    400)) : 0))

/*
 * to convert simple two-digit years to tm_year style years:
 *
 *	if (year < YEAR_PIVOT)
 *		year += 100;
 *
 * to convert either two-digit OR tm_year years to four-digit years:
 *
 *	if (year < YEAR_PIVOT)
 *		year += 100;
 *
 *	if (year < YEAR_BREAK)
 *		year += 1900;
 */

/* pythonize-header: stop ignoring */

/*
 * NTP protocol parameters.  See section 3.2.6 of the specification.
 */
#define	NTP_VERSION	4	/* current version number */
#define	NTP_OLDVERSION	1 	/* oldest credible version */
#define	NTP_PORT	123	/* included for non-unix machines */

/* pythonize-header: start ignoring */

/*
 * Poll interval parameters
 */
#define NTP_UNREACH	10	/* poll unreach threshold */
#define	NTP_MINPOLL	0	/* log2 min poll interval (1 s) */
#define NTP_MINDPOLL	6	/* log2 default min poll (64 s) */
#define NTP_MAXDPOLL	10	/* log2 default max poll (~17 m) */
#define	NTP_MAXPOLL	17	/* log2 max poll interval (~36 h) */
#define	NTP_MAXPOLL_UNK	99	/* log2 max poll unset */
#define	NTP_RETRY	3	/* max packet retries */
#define	NTP_MINPKT	2	/* guard time (s) */

/*
 * Clock filter algorithm tuning parameter
 */
#define	NTP_SHIFT	8	/* clock filter stages */

/*
 * Selection algorithm tuning parameters
 */
#define MAXDISTANCE	1.5	/* max root distance (select threshold) */
#define HUFFPUFF	900	/* huff-n'-puff sample interval (s) */

/*
 * Miscellaneous stuff
 */
/*
 * Limits of things
 */
#define	MAXFILENAME	256	/* max length of file name */

/*
 * Operations for jitter calculations (these use doubles).
 *
 * Note that we carefully separate the jitter component from the
 * dispersion component (frequency error plus precision). The frequency
 * error component is computed as CLOCK_PHI times the difference between
 * the epoch of the time measurement and the reference time. The
 * precision component is computed as the square root of the mean of the
 * squares of a zero-mean, uniform distribution of unit maximum
 * amplitude. Whether this makes statistical sense may be arguable.
 */
#define SQUARE(x) ((x) * (x))
#define SQRT(x) (sqrt(x))
#define LOGTOD(a)	ldexp(1., (int)(a)) /* log2 to double */
#define ULOGTOD(a)	ldexp(1., (int)(a)) /* ulog2 to double */


/*
 * The netendpt structure is used to hold the addresses and socket
 * numbers of each of the local network addresses we are using.
 * endpt is unrelated to the select algorithm's struct endpoint.
 */
typedef struct netendpt {
	struct netendpt *elink;		/* endpt list link */
	SOCKET		fd;		/* socket descriptor */
	uint32_t	ifnum;		/* endpt instance count */
	sockaddr_u	sin;		/* unicast address */
	sockaddr_u	mask;		/* subnet mask */
	sockaddr_u	bcast;		/* broadcast address */
	char		name[32];	/* name of interface */
	unsigned short	family;		/* AF_INET/AF_INET6 */
	unsigned short	phase;		/* phase in update cycle */
	uint32_t	flags;		/* interface flags */
	uint32_t	addr_refid;	/* IPv4 addr or IPv6 hash */
	unsigned long	starttime;	/* current_time at creation */
	volatile long	received;	/* number of incoming packets */
	long		sent;		/* number of outgoing packets */
	long		notsent;	/* number of send failures */
	unsigned int	ifindex;	/* for IPV6_MULTICAST_IF */
	bool	ignore_packets; /* listen-read-drop this? */
	struct peer *	peers;		/* list of peers using endpt */
	unsigned int	peercnt;	/* count of same */
} endpt;

/*
 * Flags for interfaces. Do not change these casually as they will be visible
 * in Mode 6 ifstats reports.
 */
#define INT_UP		0x001	/* Interface is up */
#define	INT_PPP		0x002	/* Point-to-point interface */
#define	INT_LOOPBACK	0x004U	/* the loopback interface */
#define	INT_BROADCAST	0x008	/* can broadcast out this interface */
/* #define INT_MULTICAST	0x010	** can multicast out this interface */
#define	INT_BCASTOPEN	0x020U	/* broadcast receive socket is open */
/* #define INT_MCASTOPEN	0x040	** multicasting enabled */
#define INT_WILDCARD	0x080	/* wildcard interface - usually skipped */
#define INT_MCASTIF	0x100	/* bound directly to MCAST address */
#define INT_PRIVACY	0x200	/* RFC 4941 IPv6 privacy address */
/* #define INT_BCASTXMIT	0x400   ** socket setup to allow broadcasts */

/*
 * Read-only control knobs for a peer structure.
 * Packaging these makes context copies a bit more succinct.
 */
struct peer_ctl {
	uint8_t		version;
	uint8_t		minpoll;
	uint8_t		maxpoll;
	uint32_t	flags;
	keyid_t		peerkey;
	double		bias;
	uint32_t	mode;	/* only used by refclocks */
#ifdef REFCLOCK
	uint32_t	baud;
	char		*path;
	char		*ppspath;
#endif /* REFCLOCK */
};

/*
 * Define flasher bits (tests 1 through 11 in packet procedure)
 * These reveal the state at the last grumble from the peer and are
 * most handy for diagnosing problems, even if not strictly a state
 * variable in the spec. These are recorded in the peer structure.
 *
 * Packet errors
 */
#define BOGON1		0X0001	/* duplicate packet */
#define BOGON2		0x0002	/* bogus packet */
#define BOGON3		0x0004	/* protocol unsynchronized */
#define BOGON4		0x0008	/* access denied */
#define BOGON5		0x0010	/* bad authentication */
#define BOGON6		0x0020	/* bad synch or stratum */
#define BOGON7		0x0040	/* bad header */
#define BOGON8		0x0080  /* bad autokey */
#define BOGON9		0x0100	/* bad crypto */
#define	PKT_BOGON_MASK	(BOGON1 | BOGON2 | BOGON3 | BOGON4 | BOGON5 |\
			BOGON6 | BOGON7 | BOGON8 | BOGON9)
/*
 * Peer errors
 */
#define BOGON10		0x0200	/* peer bad synch or stratum */
#define	BOGON11		0x0400	/* peer distance exceeded */
#define BOGON12		0x0800	/* peer synchronization loop */
#define BOGON13		0x1000	/* peer unreacable */
#define	PEER_BOGON_MASK	(BOGON10 | BOGON11 | BOGON12 | BOGON13)

/*
 * Does a peer node represent a reference clock?
 */
#ifdef REFCLOCK
#define IS_PEER_REFCLOCK(p)	((p)->procptr != NULL)
#else
#define IS_PEER_REFCLOCK(p)	false
#endif

/*
 * The peer structure. Holds state information relating to the guys
 * we are peering with. Most of this stuff is from section 3.2 of the
 * spec.
 *
 * The mode field is overloaded; it's used in the refclock case to pass
 * in a mode word that may contain a baud rate or subtype as well as flags.
 * Splitting this field would complicate some call sequences that are
 * already unpleasantly intricate.
 * It used to be called ttl which was used by multicast which we dropped.
 */
struct peer {
	struct peer *p_link;	/* link pointer in free & peer lists */
	struct peer *adr_link;	/* link pointer in address hash */
	struct peer *aid_link;	/* link pointer in associd hash */
	struct peer *ilink;	/* list of peers for interface */
	struct peer_ctl cfg;	/* peer configuration block */
	sockaddr_u srcadr;	/* address of remote host */
	char *	hostname;	/* if non-NULL, remote name */
	endpt *	dstadr;		/* local address */
	associd_t associd;	/* association ID */
	uint8_t	hmode;		/* local association mode */
	uint8_t	hpoll;		/* local poll interval */
	uint8_t	cast_flags;	/* additional flags */
	uint8_t	last_event;	/* last peer error code */
	uint8_t	num_events;	/* number of error events */

	/*
	 * Variables used by reference clock support
	 */
#ifdef REFCLOCK
	struct refclockproc *procptr; /* refclock structure pointer */
	bool	is_pps_driver;	/* is this the PPS driver? */
	uint8_t	sstclktype;	/* clock type for system status word */
#endif /* REFCLOCK */

	/*
	 * Variables set by received packet
	 */
	uint8_t	leap;		/* local leap indicator */
	uint8_t	pmode;		/* remote association mode */
	uint8_t	stratum;	/* remote stratum */
	uint8_t	ppoll;		/* remote poll interval */
	int8_t	precision;	/* remote clock precision */
	double	rootdelay;	/* roundtrip delay to primary source */
	double	rootdisp;	/* dispersion to primary source */
	uint32_t	refid;	/* remote reference ID */
	l_fp	reftime;	/* update epoch */

#define clear_to_zero status

	/*
	 * Ephemeral state variables
	 */
	uint8_t	status;		/* peer status */
	uint8_t	new_status;	/* under-construction status */
	uint8_t	reach;		/* reachability register */
	int	flash;		/* protocol error test tally bits */
	uptime_t	epoch;	/* reference epoch */
	int	burst;		/* packets remaining in burst */
	int	retry;		/* retry counter */
	int	filter_nextpt;	/* index into filter shift register */
	double	filter_delay[NTP_SHIFT]; /* delay shift register */
	double	filter_offset[NTP_SHIFT]; /* offset shift register */
	double	filter_disp[NTP_SHIFT]; /* dispersion shift register */
	uptime_t	filter_epoch[NTP_SHIFT]; /* epoch shift register */
	uint8_t	filter_order[NTP_SHIFT]; /* filter sort index */
	l_fp	rec;		/* receive time stamp */
	l_fp	xmt;		/* transmit time stamp */
	l_fp	dst;		/* destination timestamp */
	l_fp	org_ts;		/* origin real-timestamp */
	l_fp	org_rand;	/* origin pseudo-timestamp */
	double	offset;		/* peer clock offset */
	double	delay;		/* peer roundtrip delay */
	double	jitter;		/* peer jitter (squares) */
	double	disp;		/* peer dispersion */

	/*
	 * End of clear-to-zero area
	 */
	unsigned int   outcount;       /* packets sent without reply */
	unsigned long	update;		/* receive epoch */
#define end_clear_to_zero update
	int	unreach;	/* watchdog counter */
	int	throttle;	/* rate control */
	uptime_t	outdate;	/* send time last packet */
	uptime_t	nextdate;	/* send time next packet */

	/*
	 * Statistic counters
	 */
	unsigned long	timereset;	/* time stat counters were reset */
	unsigned long	timereceived;	/* last packet received time */
	unsigned long	timereachable;	/* last reachable/unreachable time */

	unsigned long	sent;		/* packets sent */
	unsigned long	received;	/* packets received */
	unsigned long	processed;	/* packets processed */
	unsigned long	badauth;	/* bad authentication (BOGON5) */
	unsigned long	bogusorg;	/* bogus origin (BOGON2, BOGON3) */
	unsigned long	oldpkt;		/* old duplicate (BOGON1) */
	unsigned long	seldisptoolarge; /* bad header (BOGON6, BOGON7) */
	unsigned long	selbroken;	/* KoD received */
};

/* pythonize-header: stop ignoring */

/*
 * Values for peer.leap, sys_leap
 */
#define	LEAP_NOWARNING	0x0	/* normal, no leap second warning */
#define	LEAP_ADDSECOND	0x1	/* last minute of day has 61 seconds */
#define	LEAP_DELSECOND	0x2	/* last minute of day has 59 seconds */
#define	LEAP_NOTINSYNC	0x3	/* overload, clock is free running */

/* Packet Modes
 */
#define	MODE_UNSPEC	0	/* unspecified (old version) */
#define	MODE_ACTIVE	1	/* symmetric active mode */
#define	MODE_PASSIVE	2	/* symmetric passive mode */
#define	MODE_CLIENT	3	/* client mode */
#define	MODE_SERVER	4	/* server mode */
#define	MODE_BROADCAST	5	/* broadcast mode */

/*
 * These can appear in packets
 */
#define	MODE_CONTROL	6	/* control mode, ntpq*/
#define	MODE_PRIVATE	7	/* Dead: private mode, was ntpdc */
/*
 * This is a madeup mode for broadcast client.  No longer used by ntpd.
 */
/* #define	MODE_BCLIENT	6	** broadcast client mode */
#define	MODE_BCLIENTX	6	/* for pylib/util.py */

#define	LEN_PKT_NOMAC	48 /* min header length */

/* pythonize-header: start ignoring */

/*
 * Values for peer.stratum, sys_stratum
 */
#define	STRATUM_REFCLOCK ((uint8_t)0) /* default stratum */
/* A stratum of 0 in the packet is mapped to 16 internally */
#define	STRATUM_PKT_UNSPEC ((uint8_t)0) /* unspecified in packet */
#define	STRATUM_UNSPEC	((uint8_t)16) /* unspecified */

/*
 * Values for peer.flags (unsigned int)
 */
#define	FLAG_CONFIG	0x0001u	/* association was configured */
#define	FLAG_PREEMPT	0x0002u	/* preemptable association */
#define	FLAG_AUTHENTIC	0x0004u	/* last message was authentic */
#define	FLAG_REFCLOCK	0x0008u	/* this is actually a reference clock */
#define	FLAG_BC_VOL	0x0010u	/* broadcast client volleying */
#define	FLAG_PREFER	0x0020u	/* prefer peer */
#define	FLAG_BURST	0x0040u	/* burst mode */
#define	FLAG_PPS	0x0080u	/* steered by PPS */
#define	FLAG_IBURST	0x0100u	/* initial burst mode */
#define	FLAG_NOSELECT	0x0200u	/* never select */
#define	FLAG_TRUE	0x0400u	/* force truechimer */
#define	FLAG_DNS	0x0800u	/* needs DNS lookup */
#define FLAG_TSTAMP_PPS	0x4cd000u	/* PPS source provides absolute timestamp */


/*
 * It's ugly that refid is sometimes treated as a  uint32_t and sometimes
 * as a string; that should be fixed. Using this in memcpy() at least
 * contains the problem.
 */
#define REFIDLEN	sizeof(uint32_t)

/* This is the new, sane way of representing packets. All fields are
   in host byte order, and the fixed-point time fields are just integers,
   with uints of 2^-16 or 2^-32 seconds as appropriate. */

struct parsed_pkt {
        uint8_t li_vn_mode;
        uint8_t stratum;
        uint8_t ppoll;
        int8_t precision;
        uint32_t rootdelay;
        uint32_t rootdisp;
        char refid[REFIDLEN];
        uint64_t reftime;
        uint64_t org;
        uint64_t rec;
        uint64_t xmt;
        unsigned num_extensions;
        struct exten *extensions;
};

struct exten {
        uint16_t type;
        uint16_t len;
        uint8_t *body;
};

/* This is the old, insane way of representing packets. It'll gradually
   be phased out and removed. Packets are simply pulled off the wire and
   then type-punned into this structure, so all fields are in network
   byte order. Note that there is no pack pragma. The only reason this
   ever worked at all is that all the fields are self-aligned, so no ABI
   has been evil enough to insert padding between fields. */
struct pkt {
	uint8_t	li_vn_mode;	/* peer leap indicator */
	uint8_t	stratum;	/* peer stratum */
	uint8_t	ppoll;		/* peer poll interval */
	int8_t	precision;	/* peer clock precision */
	u_fp	rootdelay;	/* roundtrip delay to primary source */
	u_fp	rootdisp;	/* dispersion to primary source*/
	uint32_t	refid;		/* reference id */
	l_fp_w	reftime;	/* last update time */
	l_fp_w	org;		/* originate time stamp */
	l_fp_w	rec;		/* receive time stamp */
	l_fp_w	xmt;		/* transmit time stamp */

/* Old style authentication was just appended
 * without the type/length of an extension header. */
/* Length includes 1 word of keyID */
/* MD5 length is 16 bytes => 4+1 */
/* SHA length is 20 bytes => 5+1 */
#define MIN_MAC_LEN	(1 * sizeof(uint32_t))	/* crypto_NAK */
#define	MAX_MAC_LEN	(6 * sizeof(uint32_t))	/* MAX of old style */

	uint32_t	exten[(MAX_MAC_LEN) / sizeof(uint32_t)];
} __attribute__ ((aligned));

/* pythonize-header: stop ignoring */

/*
 * Stuff for extracting things from li_vn_mode
 */
#define	PKT_MODE(li_vn_mode)	((li_vn_mode) & 0x7)
#define	PKT_VERSION(li_vn_mode)	(((li_vn_mode) >> 3) & 0x7)
#define	PKT_LEAP(li_vn_mode)	(((li_vn_mode) >> 6) & 0x3)

/*
 * Stuff for putting things back into li_vn_mode in packets and vn_mode
 * in ntp_monitor.c's mon_entry.
 */
#define VN_MODE(v, m)		((((v) & 7) << 3) | ((m) & 0x7))
#define	PKT_LI_VN_MODE(l, v, m) ((((l) & 3) << 6) | VN_MODE((v), (m)))

/*
 * Event codes. Used for reporting errors/events to the control module
 */
#define	PEER_EVENT	0x080	/* this is a peer event */
#define CRPT_EVENT	0x100	/* this is a crypto event */

/*
 * System event codes
 */
#define	EVNT_UNSPEC	0	/* unspecified */
#define	EVNT_NSET	1	/* freq not set */
#define	EVNT_FSET	2	/* freq set */
#define	EVNT_SPIK	3	/* spike detect */
#define	EVNT_FREQ	4	/* freq mode */
#define	EVNT_SYNC	5	/* clock sync */
#define	EVNT_SYSRESTART	6	/* restart */
#define	EVNT_SYSFAULT	7	/* panic stop */
#define	EVNT_NOPEER	8	/* no sys peer */
#define	EVNT_ARMED	9	/* leap armed */
#define	EVNT_DISARMED	10	/* leap disarmed */
#define	EVNT_LEAP	11	/* leap event */
#define	EVNT_CLOCKRESET	12	/* clock step */
#define	EVNT_KERN	13	/* kernel event */
#define	EVNT_TAI	14	/* TAI */
#define	EVNT_LEAPVAL	15	/* stale leapsecond values */

/*
 * Peer event codes
 */
#define	PEVNT_MOBIL	(1 | PEER_EVENT) /* mobilize */
#define	PEVNT_DEMOBIL	(2 | PEER_EVENT) /* demobilize */
#define	PEVNT_UNREACH	(3 | PEER_EVENT) /* unreachable */
#define	PEVNT_REACH	(4 | PEER_EVENT) /* reachable */
#define	PEVNT_RESTART	(5 | PEER_EVENT) /* restart */
#define	PEVNT_REPLY	(6 | PEER_EVENT) /* no reply */
#define	PEVNT_RATE	(7 | PEER_EVENT) /* rate exceeded */
#define	PEVNT_DENY	(8 | PEER_EVENT) /* access denied */
#define PEVNT_ARMED	(9 | PEER_EVENT) /* leap armed */
#define	PEVNT_NEWPEER	(10 | PEER_EVENT) /* sys peer */
#define	PEVNT_CLOCK	(11 | PEER_EVENT) /* clock event */
#define	PEVNT_AUTH	(12 | PEER_EVENT) /* bad auth */
#define	PEVNT_POPCORN	(13 | PEER_EVENT) /* popcorn */

/*
 * Clock event codes
 */
#define	CEVNT_NOMINAL	0	/* unspecified */
#define	CEVNT_TIMEOUT	1	/* no reply */
#define	CEVNT_BADREPLY	2	/* bad format */
#define	CEVNT_FAULT	3	/* fault */
#define	CEVNT_PROP	4	/* bad signal */
#define	CEVNT_BADDATE	5	/* bad date */
#define	CEVNT_BADTIME	6	/* bad time */
#define CEVNT_MAX	CEVNT_BADTIME

/* pythonize-header: start ignoring */

/*
 * To speed lookups, peers are hashed by the low order bits of the
 * remote IP address. These definitions relate to that.
 */
#define	NTP_HASH_SIZE		128
#define	NTP_HASH_MASK		(NTP_HASH_SIZE-1)
#define	NTP_HASH_ADDR(src)	(sock_hash(src) & NTP_HASH_MASK)

/*
 * min, and max.  Makes it easier to transliterate the spec without
 * thinking about it.
 */
#define	min(a,b)	(((a) < (b)) ? (a) : (b))
#define	max(a,b)	(((a) > (b)) ? (a) : (b))


/*
 * Configuration items.  These are for the protocol module (proto_config())
 */
/* #define	PROTO_BROADCLIENT	1 */
/* #define	PROTO_PRECISION		2 */
/* #define	PROTO_AUTHENTICATE	3 */
/* #define	PROTO_BROADDELAY	4 */
/* #define	PROTO_AUTHDELAY		5 */
/* #define	PROTO_MULTICAST_ADD	6 */
/* #define	PROTO_MULTICAST_DEL	7 */
#define PROTO_NTP		8
#define PROTO_KERNEL		9
#define PROTO_MONITOR		10
#define PROTO_FILEGEN		11
#define	PROTO_PPS		12
#define PROTO_CAL		13
#define PROTO_MINCLOCK		14
#define	PROTO_MAXCLOCK		15
#define PROTO_MINSANE		16
#define PROTO_FLOOR		17
#define PROTO_CEILING		18
/* #define PROTO_COHORT		19 */
#define PROTO_CALLDELAY		20
#define PROTO_MINDISP		21
#define PROTO_MAXDIST		22
#define	PROTO_MAXDISP		23
#define	PROTO_MAXHOP		24
/* #define	PROTO_BEACON		25 */
#define	PROTO_ORPHAN		26
#define	PROTO_ORPHWAIT		27
/* #define	PROTO_MODE7		28 was ntpdc */

/*
 * Configuration items for the loop filter
 */
#define	LOOP_DRIFTINIT		1	/* iniitialize frequency */
#define	LOOP_KERN_CLEAR		2	/* set initial frequency offset */
#define LOOP_MAX		3	/* set both step offsets */
#define LOOP_MAX_BACK		4	/* set bacward-step offset */
#define LOOP_MAX_FWD		5	/* set forward-step offset */
#define LOOP_PANIC		6	/* set panic offseet */
#define LOOP_PHI		7	/* set dispersion rate */
#define LOOP_MINSTEP		8	/* set step timeout */
#define LOOP_MINPOLL		9	/* set min poll interval (log2 s) */
#define LOOP_ALLAN		10	/* set minimum Allan intercept */
#define LOOP_HUFFPUFF		11	/* set huff-n'-puff filter length */
#define LOOP_FREQ		12	/* set initial frequency */
#define	LOOP_LEAP		13	/* insert leap after second 23:59 */
#define	LOOP_TICK		14	/* sim. low precision clock */

/*
 * Configuration items for the stats printer
 */
#define	STATS_FREQ_FILE		1	/* configure drift file */
#define STATS_STATSDIR		2	/* directory prefix for stats files */
#define	STATS_PID_FILE		3	/* configure ntpd PID file */
#define	STATS_LEAP_FILE		4	/* configure ntpd leapseconds file */

/*
 * Structure used optionally for monitoring when this is turned on.
 */
typedef struct mon_data	mon_entry;
struct mon_data {
	mon_entry *	hash_next;	/* next structure in hash list */
	DECL_DLIST_LINK(mon_entry, mru);/* MRU list link pointers */
	endpt *		lcladr;		/* address on which this arrived */
	l_fp		first;		/* first time seen */
	l_fp		last;		/* last time seen */
	int		leak;		/* leaky bucket accumulator */
	int		count;		/* total packet count */
	unsigned short	flags;		/* restrict flags */
	uint8_t		vn_mode;	/* packet mode & version */
	sockaddr_u	rmtadr;		/* address of remote host */
};

/*
 * Values for cast_flags in mon_entry and struct peer.  mon_entry uses
 * only MDF_UCAST and MDF_BCAST.
 */
#define	MDF_UCAST	0x01	/* unicast client */
/* #define MDF_MCAST	0x02	** multicast server (not used) */
/* #define MDF_BCAST	0x04	** broadcast server (not used) */
#define	MDF_POOL	0x08	/* pool client solicitor */
/* #define MDF_ACAST	0x10	** manycast client solicitor (not used) */
#define	MDF_BCLNT	0x20	/* eph. broadcast/multicast client */
#define MDF_UCLNT	0x40	/* preemptible manycast or pool client */
/*
 * In the context of struct peer in ntpd, one cast_flags bit
 * represent configured associations which never receive packets, and
 * whose reach is always 0: MDF_BCAST  (Historical)
 */
#define MDF_TXONLY_MASK	(MDF_POOL)
/*
 * manycastclient-like solicitor association cast_flags bits
 */
#define MDF_SOLICIT_MASK	MDF_POOL
/*
 * Values used with mon_enabled to indicate reason for enabling monitoring
 */
#define MON_OFF		0x00		/* no monitoring */
#define MON_ON		0x01		/* monitoring explicitly enabled */
#define MON_RES		0x02		/* implicit monitoring for RES_LIMITED */
/*
 * Structure used for restrictlist entries
 */
typedef struct res_addr4_tag {
	uint32_t	addr;		/* IPv4 addr (host order) */
	uint32_t	mask;		/* IPv4 mask (host order) */
} res_addr4;

typedef struct res_addr6_tag {
	struct in6_addr addr;		/* IPv6 addr (net order) */
	struct in6_addr mask;		/* IPv6 mask (net order) */
} res_addr6;

typedef struct restrict_u_tag	restrict_u;
struct restrict_u_tag {
	restrict_u *		link;	/* link to next entry */
	uint32_t		hitcount;	/* number of packets matched */
	unsigned short		flags;	/* accesslist flags */
	unsigned short		mflags;	/* match flags */
	unsigned long		expire;	/* valid until time */
	union {				/* variant starting here */
		res_addr4 v4;
		res_addr6 v6;
	} u;
};
#define	V4_SIZEOF_RESTRICT_U	(offsetof(restrict_u, u)	\
				 + sizeof(res_addr4))
#define	V6_SIZEOF_RESTRICT_U	(offsetof(restrict_u, u)	\
				 + sizeof(res_addr6))

/* pythonize-header: stop ignoring */

/*
 * Access flags.  Do not change or garbage-collect these, they are exposed
 * through the Mode 6 protocol.
 */
#define	RES_IGNORE		0x0001	/* ignore packet */
#define	RES_DONTSERVE		0x0002	/* access denied */
#define	RES_DONTTRUST		0x0004	/* authentication required */
#define	RES_VERSION		0x0008	/* version mismatch */
#define	RES_NOPEER		0x0010	/* new association denied */
#define RES_LIMITED		0x0020	/* packet rate exceeded */
#define RES_FLAGS		(RES_IGNORE | RES_DONTSERVE |\
				    RES_DONTTRUST | RES_VERSION |\
				    RES_NOPEER | RES_LIMITED)

#define	RES_NOQUERY		0x0040	/* mode 6 packet denied */
#define	RES_NOMODIFY		0x0080	/* mode 6 modify denied */
#define	RES_NOTRAP		0x0100	/* mode 6 set trap denied (not used) */
#define	RES_LPTRAP		0x0200	/* mode 6 low priority trap (not used) */

#define	RES_KOD			0x0400	/* send kiss of death packet */
#define	RES_MSSNTP		0x0800	/* enable MS-SNTP authentication */
#define	RES_FLAKE		0x1000	/* flakeway - drop 10% */
#define	RES_NOMRULIST		0x2000	/* mode 6 mrulist denied */

/* pythonize-header: start ignoring */

/*
 * Match flags
 */
#define	RESM_INTERFACE		0x1000	/* this is an interface */
#define	RESM_NTPONLY		0x2000	/* match source port 123 */
#define RESM_SOURCE		0x4000	/* from "restrict source" */

/*
 * Restriction configuration ops
 */
#define	RESTRICT_FLAGS		1	/* add flags to restrict entry */
#define	RESTRICT_UNFLAG		2	/* remove flags from restrict entry */
#define	RESTRICT_REMOVE		3	/* remove a restrict entry */
#define	RESTRICT_REMOVEIF	4	/* remove an interface restrict entry */

/*
 * Endpoint structure for the select algorithm
 */
struct endpoint {
	double	val;			/* offset of endpoint */
	int	type;			/* interval entry/exit */
};

/*
 * Association matching AM[] return codes
 */
#define AM_ERR		-1		/* error */
#define AM_NOMATCH	0		/* no match */
#define AM_PROCPKT	1		/* server/symmetric packet */
#define AM_BCST		2		/* broadcast packet */
#define AM_FXMIT	3		/* client packet */
#define AM_MANYCAST	4		/* manycast or pool */
#define AM_NEWPASS	5		/* new passive */
#define AM_NEWBCL	6		/* new broadcast */
#define AM_POSSBCL	7		/* discard broadcast */

/* ntpq -c mrulist rows per request limit in ntpd */
#define MRU_ROW_LIMIT	256
/* similar datagrams per response limit for ntpd */
#define MRU_FRAGS_LIMIT	128

#endif /* GUARD_NTP_H */
