/*
 * ntpd.h - Prototypes and external variables for ntpd.
 *
 * Note the first half is primarily function prototypes, type
 * declarations, and preprocessor macros, with variables declared
 * primarily in the second half.
 *
 * Each half is further divided into sections for each source file.
 */

#include "ntp.h"
#include "ntp_stdlib.h"
#include "ntp_syslog.h"
#include "ntp_debug.h"
#include "ntp_syslog.h"
#include "ntp_malloc.h"
#include "ntp_refclock.h"
#include "ntp_control.h"
#include "recvbuff.h"

/*
 * First half: ntpd types, functions, macros
 * -----------------------------------------
 */

/*
 * macro to silence -Wimplicit-fallthrough
 * of course gcc and clang do it differently
 */
#ifdef __clang__
# define FALLTHRU
#elif defined __GNUC__ && (6 < __GNUC__ )
# define FALLTHRU __attribute__ ((fallthrough));
#else
# define FALLTHRU
#endif


/* ntp_config.c */
extern	const char	*getconfig	(const char *);
extern	void	readconfig(const char *);
extern	void	ctl_clr_stats	(void);
extern	u_short ctlpeerstatus	(struct peer *);
extern	void	init_control	(void);
extern	void	process_control (struct recvbuf *, int);
extern	void	report_event	(int, struct peer *, const char *);
extern	int	mprintf_event	(int, struct peer *, const char *, ...)
			NTP_PRINTF(3, 4);

/* ntp_control.c */
/*
 * Structure for translation tables between internal system
 * variable indices and text format.
 */
struct ctl_var {
	u_short code;
	u_short flags;
	const char *text;
};
/*
 * Flag values
 */
#define	CAN_READ	0x01
#define	CAN_WRITE	0x02

#define DEF		0x20
#define	PADDING		0x40
#define	EOV		0x80

#define	RO	(CAN_READ)
#define	RW	(CAN_READ|CAN_WRITE)

extern	char *	add_var (struct ctl_var **, u_long, u_short);
extern	void	free_varlist (struct ctl_var *);
extern	void	set_var (struct ctl_var **, const char *, u_long, u_short);
extern	void	set_sys_var (const char *, u_long, u_short);
extern	const char *	get_ext_sys_var(const char *tag);

/* ntp_io.c */
typedef struct interface_info {
	endpt *	ep;
	uint8_t	action;
} interface_info_t;

typedef void	(*interface_receiver_t)	(void *, interface_info_t *);

extern  bool listen_to_virtual_ips;
extern	endpt *	getinterface		(sockaddr_u *, uint32_t);
extern	endpt *	select_peerinterface	(struct peer *, sockaddr_u *,
					 endpt *);
extern	endpt *	findinterface		(sockaddr_u *);
extern	void	enable_broadcast	(endpt *, sockaddr_u *);
extern	void	interface_update	(interface_receiver_t, void *);
extern  void    io_handler              (void);
extern	void	init_io 	(void);
extern  SOCKET	open_socket	(sockaddr_u *, bool, bool, endpt *);
extern	void	io_open_sockets	(void);
extern	void	io_clr_stats	(void);
extern	void	sendpkt 	(sockaddr_u *, endpt *, void *, int);
#ifdef ENABLE_DEBUG_TIMING
extern	void	collect_timing  (struct recvbuf *, const char *, int, l_fp);
#endif
extern const char * latoa(endpt *);

/* ntp_loopfilter.c */
extern	void	init_loopfilter(void);
extern	int 	local_clock(struct peer *, double);
extern	void	adj_host_clock(void);
extern	void	loop_config(int, double);
extern	void	select_loop(int);
extern	void	huffpuff(void);
extern	u_int	sys_tai;
extern 	int	freq_cnt;

/* ntp_monitor.c */
#define MON_HASH_SIZE		(1U << mon_hash_bits)
#define MON_HASH_MASK		(MON_HASH_SIZE - 1)
#define	MON_HASH(addr)		(sock_hash(addr) & MON_HASH_MASK)
extern	void	init_mon	(void);
extern	void	mon_start	(int);
extern	void	mon_stop	(int);
extern	u_short	ntp_monitor	(struct recvbuf *, u_short);
extern	void	mon_clearinterface(endpt *interface);
extern  int	mon_get_oldest_age(l_fp);

/* ntp_peer.c */
extern	void	init_peer	(void);
extern	struct peer *findexistingpeer(sockaddr_u *, const char *,
				      struct peer *, int);
extern	struct peer *findpeer	(struct recvbuf *, int, int *);
extern	struct peer *findpeerbyassoc(associd_t);
extern  void	set_peerdstadr	(struct peer *, endpt *);
extern	struct peer *newpeer	(sockaddr_u *, const char *,
				 endpt *, uint8_t, uint8_t,
				 uint8_t, uint8_t, u_int, uint8_t, uint32_t,
				 keyid_t, const bool);
extern	void	peer_update_hash (struct peer *);
extern	void	peer_all_reset	(void);
extern	void	peer_clr_stats	(void);
extern	void	refresh_all_peerinterfaces(void);
extern	void	unpeer		(struct peer *);
extern	void	clear_all	(void);
extern	int	score_all	(struct peer *);
extern	struct peer *findmanycastpeer(struct recvbuf *);
extern	void	peer_cleanup	(void);

/* ntp_proto.c */
extern	void	transmit	(struct peer *);
extern	void	receive 	(struct recvbuf *);
extern	void	peer_clear	(struct peer *, const char *, const bool);
extern	void	set_sys_leap	(uint8_t);

extern	int	sys_orphan;
extern	double	sys_mindisp;
extern	double	sys_maxdisp;

extern	void	poll_update	(struct peer *, uint8_t);

extern	void	clock_filter	(struct peer *, double, double, double);
extern	void	init_proto	(const bool);
extern	void	set_sys_tick_precision(double);
extern	void	proto_config	(int, u_long, double);
extern	void	proto_clr_stats (void);



/* ntp_restrict.c */
extern	void	init_restrict	(void);
extern	u_short	restrictions	(sockaddr_u *);
extern	void	hack_restrict	(int, sockaddr_u *, sockaddr_u *,
				 u_short, u_short, u_long);
extern	void	restrict_source	(sockaddr_u *, bool, u_long);

/* ntp_timer.c */
extern	void	init_timer	(void);
extern	void	reinit_timer	(void);
extern	void	timer		(void);
extern	void	timer_clr_stats (void);
extern	void	timer_interfacetimeout (u_long);
extern	volatile int interface_interval;
extern	u_long	orphwait;		/* orphan wait time */

/* ntp_util.c */
extern	void	init_util	(void);
extern	void	write_stats	(void);
extern	void	stats_config	(int, const char *);
extern	void	record_peer_stats (struct peer *, int);
extern	void	record_proto_stats (char *);
extern	void	record_loop_stats (double, double, double, double, int);
extern	void	record_clock_stats (struct peer *, const char *);
extern	int	mprintf_clock_stats(struct peer *, const char *, ...)
			NTP_PRINTF(2, 3);
extern	void	record_raw_stats (sockaddr_u *srcadr, sockaddr_u *dstadr, l_fp *t1, l_fp *t2, l_fp *t3, l_fp *t4, int leap, int version, int mode, int stratum, int ppoll, int precision, double root_delay, double root_dispersion, uint32_t refid, u_int outcount);
extern	void	check_leap_file	(bool is_daily_check, time_t systime);
#ifdef ENABLE_DEBUG_TIMING
extern	void	record_timing_stats (const char *);
#endif

/* packetstamp.c */
extern void	enable_packetstamps(int, sockaddr_u *);
extern l_fp	fetch_packetstamp(struct recvbuf *, struct msghdr *, l_fp);

/*
 * Signals we catch for debugging.
 */
#define MOREDEBUGSIG	SIGUSR1
#define LESSDEBUGSIG	SIGUSR2

/* Signal we use for DNS completion
 * No (forked) children so this should be unused.
 */
#define SIGDNS	SIGCHLD

/*
 * Last half: ntpd variables
 * -------------------------
 */

/* ntp_config.c */
extern const char *progname;
extern char	*sys_phone[];		/* ACTS phone numbers */
extern char *ntp_signd_socket;

/* ntp_control.c */
extern keyid_t	ctl_auth_keyid;		/* keyid used for authenticating write requests */
extern	void	reset_auth_stats(void);

/*
 * Other statistics of possible interest
 */
extern u_long packets_dropped;	/* total number of packets dropped on reception */
extern u_long	packets_ignored;	/* received on wild card interface */
extern u_long	packets_received;	/* total number of packets received */
extern u_long	packets_sent;		/* total number of packets sent */
extern u_long	packets_notsent; 	/* total number of packets which couldn't be sent */

extern volatile u_long handler_calls;	/* number of calls to interrupt handler */
extern volatile u_long handler_pkts;	/* number of pkts received by handler */
extern u_long	io_timereset;		/* time counters were reset */

/* ntp_io.c */
extern bool	disable_dynamic_updates;
extern u_int	sys_ifnum;		/* next .ifnum to assign */
extern endpt *	any_interface;		/* IPv4 wildcard */
extern endpt *	any6_interface;		/* IPv6 wildcard */
extern endpt *	loopback_interface;	/* IPv4 loopback for refclocks */
extern endpt *	ep_list;		/* linked list */

/* ntp_loopfilter.c */
extern double	drift_comp;		/* clock frequency (s/s) */
extern double	clock_stability;	/* clock stability (s/s) */
extern double	clock_max_back;		/* max backward offset before step (s) */
extern double	clock_max_fwd;		/* max forward offset before step (s) */
extern double	clock_phi;		/* dispersion rate (s/s) */

/*
 * Clock state machine control flags
 */
extern bool	ntp_enable;		/* clock discipline enabled */
extern bool	pll_control;		/* kernel support available */
extern bool	kern_enable;		/* kernel support enabled */
extern bool	hardpps_enable;		/* kernel PPS discipline enabled */
extern bool	cal_enable;		/* refclock calibrate enable */
extern bool	allow_panic;		/* allow panic correction (-g) */
extern bool	force_step_once;	/* always step time once at startup (-G) */
extern bool	mode_ntpdate;		/* exit on first clock set (-q) */
extern int	peer_ntpdate;		/* count of ntpdate peers */

/*
 * Clock state machine variables
 */
extern uint8_t	sys_poll;		/* system poll interval (log2 s) */
extern int	tc_counter;		/* poll-adjust counter */
extern double	last_offset;		/* last clock offset (s) */
extern uint8_t	allan_xpt;		/* Allan intercept (log2 s) */
extern double	clock_jitter;		/* clock jitter (s) */
extern double	sys_offset;		/* system offset (s) */
extern double	sys_jitter;		/* system jitter (s) */

/* ntp_monitor.c */
extern uint8_t	mon_hash_bits;		/* log2 size of hash table */
extern mon_entry ** mon_hash;		/* MRU hash table */
extern mon_entry mon_mru_list;		/* mru listhead */
extern u_int	mon_enabled;		/* MON_OFF (0) or other MON_* */
extern u_int	mru_entries;		/* mru list count */
extern u_int	mru_peakentries;	/* highest mru_entries */
extern u_int	mru_initalloc;		/* entries to preallocate */
extern u_int	mru_incalloc;		/* allocation batch factor */
extern u_int	mru_mindepth;		/* preempt above this */
extern int	mru_maxage;		/* recycle if older than this */
extern int	mru_minage;		/* recycle if older than this & full */
extern u_int	mru_maxdepth; 		/* MRU size hard limit */
extern u_long	mru_exists;		/* slot already exists */
extern u_long	mru_new;		/* allocated new slot */
extern u_long	mru_recycleold;		/* recycle: age > maxage */
extern u_long	mru_recyclefull;	/* recycle: full and age > minage */
extern u_long	mru_none;		/* couldn't allocate slot */
extern int	mon_age;		/* preemption limit */

/* ntp_peer.c */
extern struct peer *peer_list;		/* peer structures list */

/*
 * Miscellaneous statistic counters which may be queried.
 */
extern int	peer_associations;	/* mobilized associations */

/* ntp_proto.c */
/*
 * System variables are declared here.	See Section 3.2 of the
 * specification.
 */
extern uint8_t	sys_leap;		/* system leap indicator */
extern uint8_t	sys_stratum;		/* system stratum */
extern int8_t	sys_precision;		/* local clock precision */
extern double	sys_rootdelay;		/* roundtrip delay to primary source */
extern double	sys_rootdisp;		/* dispersion to primary source */
extern uint32_t	sys_refid;		/* reference id */
extern l_fp	sys_reftime;		/* last update time */
extern struct peer *sys_peer;		/* current peer */
extern int	sys_maxclock;		/* maximum candidates */

/*
 * Nonspecified system state variables.
 */
extern l_fp	sys_authdelay;		/* authentication delay */
extern int	sys_minsane;		/* minimum candidates */

/*
 * Statistics counters
 */
extern u_long	sys_stattime;		/* time since reset */
extern u_long	sys_received;		/* packets received */
extern u_long	sys_processed;		/* packets for this host */
extern u_long	sys_restricted;	 	/* restricted packets */
extern u_long	sys_newversion;		/* current version  */
extern u_long	sys_oldversion;		/* old version */
extern u_long	sys_badlength;		/* bad length or format */
extern u_long	sys_badauth;		/* bad authentication */
extern u_long	sys_declined;		/* declined */
extern u_long	sys_limitrejected;	/* rate exceeded */
extern u_long	sys_kodsent;		/* KoD sent */
extern u_long	use_stattime;		/* time since reset */

/* Signalling */
extern volatile bool sawALRM;
extern volatile bool sawHUP;
extern volatile bool sawDNS;
extern volatile bool sawQuit;		/* SIGQUIT, SIGINT, SIGTERM */

/* ntp_restrict.c */
extern restrict_u *	restrictlist4;	/* IPv4 restriction list */
extern restrict_u *	restrictlist6;	/* IPv6 restriction list */
extern int		ntp_minpkt;
extern uint8_t		ntp_minpoll;

/* ntp_signd.c */

#ifdef ENABLE_MSSNTP
/* ntp_signd.c */
extern void send_via_ntp_signd(struct recvbuf *, int, keyid_t, int,
			       struct pkt *);
#endif

/* ntp_timer.c */
extern volatile u_long alarm_overflow;
extern u_long	current_time;		/* seconds since startup */
extern u_long	timer_timereset;
extern u_long	timer_xmtcalls;
extern bool	leap_sec_in_progress;
#ifdef ENABLE_LEAP_SMEAR
extern struct leap_smear_info leap_smear;
extern u_int	leap_smear_intv;
#endif

/* ntp_util.c */
extern	char	statsdir[MAXFILENAME];
extern	bool	stats_control;		/* write stats to fileset? */
extern	double	wander_threshold;

/* ntpd.c */
#ifdef HAVE_WORKING_FORK
extern	int	waitsync_fd_to_close;	/* -w/--wait-sync */
#endif

/* refclock_conf.c */
#ifdef REFCLOCK
/* refclock configuration table */
extern struct refclock * const refclock_conf[];
extern const uint8_t	num_refclock_conf;
#endif

