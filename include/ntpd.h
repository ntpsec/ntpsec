/*
 * ntpd.h - Prototypes and external variables for ntpd.
 *
 * Note the first half is primarily function prototypes, type
 * declarations, and preprocessor macros, with variables declared
 * primarily in the second half.
 *
 * Each half is further divided into sections for each source file.
 */
#ifndef GUARD_NTPD_H
#define GUARD_NTPD_H

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

/* ntpd.c */
extern	void announce_starting(void);

/* ntp_config.c */
extern	const char	*getconfig	(const char *);
extern	void	readconfig(const char *);
extern	void	ctl_clr_stats	(void);
extern	unsigned short ctlpeerstatus	(struct peer *);
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
	unsigned short code;
	unsigned short flags;
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

extern	char *	add_var (struct ctl_var **, unsigned long, unsigned short);
extern	void	free_varlist (struct ctl_var *);
extern	void	set_var (struct ctl_var **, const char *, unsigned long, unsigned short);
extern	void	set_sys_var (const char *, unsigned long, unsigned short);
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
extern	void	interface_update	(interface_receiver_t, void *);
extern  void    io_handler              (void);
extern	void	init_io		(void);
extern	void	io_open_sockets	(void);
extern	void	io_clr_stats	(void);
extern	void	sendpkt		(sockaddr_u *, endpt *, void *, unsigned int);
extern const char * latoa(endpt *);
extern  uint64_t dropped_count(void);
extern  uint64_t ignored_count(void);
extern  uint64_t received_count(void);
extern  void     inc_received_count(void);
extern  uint64_t sent_count(void);
extern  uint64_t notsent_count(void);
extern  uint64_t handler_calls_count(void);
extern  uint64_t handler_pkts_count(void);
extern  uptime_t counter_reset_time(void);

/* ntp_loopfilter.c */
extern	void	init_loopfilter(void);
extern	int	local_clock(struct peer *, double);
extern	void	adj_host_clock(void);
extern	void	loop_config(int, double);
extern	void	select_loop(int);
extern	void	huffpuff(void);
extern	unsigned int	sys_tai;
extern	int	freq_cnt;

/* ntp_monitor.c */
extern	void	init_mon(void);
extern	void	mon_setup(int);
extern	void	mon_setdown(int);
extern	void	mon_start(void);
extern	void	mon_stop(void);
extern	void	mon_timer(void);
extern	unsigned short	ntp_monitor	(struct recvbuf *, unsigned short);
extern	void	mon_clearinterface(endpt *interface);
extern  int	mon_get_oldest_age(l_fp);
extern  mon_entry *mon_get_slot(sockaddr_u *);

/* ntp_peer.c */
extern	void	init_peer	(void);
extern	struct peer *findexistingpeer(sockaddr_u *, const char *,
				      struct peer *, int);
extern	struct peer *findpeer	(struct recvbuf *);
extern	struct peer *findpeerbyassoc(associd_t);
extern  void	set_peerdstadr	(struct peer *, endpt *);
extern	struct peer *newpeer	(sockaddr_u *, const char *,
				 endpt *, uint8_t, struct peer_ctl *,
				 uint8_t, const bool);
extern	void	peer_add_hash (struct peer *);
extern	void	peer_del_hash (struct peer *);
extern	void	peer_all_reset	(void);
extern	void	peer_clr_stats	(void);
extern	void	refresh_all_peerinterfaces(void);
extern  void	peer_refresh_interface(struct peer *);
extern	void	unpeer		(struct peer *);
extern	void	clear_all	(void);
extern	int	score_all	(struct peer *);
extern	void	peer_cleanup	(void);

/* ntp_proto.c */
extern	void	transmit	(struct peer *);
extern	void	receive		(struct recvbuf *);
extern	void	peer_clear	(struct peer *, const char *, const bool);
extern	void	set_sys_leap	(uint8_t);

extern	int	sys_orphan;
extern	double	sys_mindist;
extern	double	sys_maxdisp;

#define stat_sys_form(member)\
extern uint64_t stat_##member(void);\
extern uint64_t stat_total_##member(void)
stat_sys_form(received);
stat_sys_form(processed);
stat_sys_form(restricted);
stat_sys_form(newversion);
stat_sys_form(oldversion);
stat_sys_form(version1);
stat_sys_form(version1client);
stat_sys_form(version1zero);
stat_sys_form(version1symm);
stat_sys_form(badlength);
stat_sys_form(badauth);
stat_sys_form(declined);
stat_sys_form(limitrejected);
stat_sys_form(kodsent);
#undef stat_sys_form

extern uptime_t stat_total_stattime(void);
extern uptime_t stat_stattime(void);

extern void increment_restricted(void);
extern uptime_t stat_use_stattime(void);
extern void set_use_stattime(uptime_t stattime);
extern uptime_t	use_stattime;		/* time since usestats reset */

extern	void	poll_update	(struct peer *, uint8_t);

extern	void	clock_filter	(struct peer *, double, double, double);
extern	void	init_proto	(const bool);
#ifdef ENABLE_FUZZ
extern	void	set_sys_tick_precision(double);
#endif
extern	void	proto_config	(int, unsigned long, double);
extern	void	proto_clr_stats (void);

/* ntp_restrict.c */
extern	void	init_restrict	(void);
extern	unsigned short	restrictions	(sockaddr_u *);
extern	void	hack_restrict	(int, sockaddr_u *, sockaddr_u *,
				 unsigned short, unsigned short);
extern	void	restrict_source		(struct peer *);
extern	void	unrestrict_source	(struct peer *);

/* ntp_timer.c */
extern	void	init_timer	(void);
extern	void	reinit_timer	(void);
extern	void	timer		(void);
extern	void	timer_clr_stats (void);
extern	void	timer_interfacetimeout (uptime_t);
extern	int	interface_interval;
extern	uptime_t	orphwait;		/* orphan wait time */

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
extern	void	record_raw_stats (
  struct peer *peer,
  struct recvbuf *rbufp,
  unsigned int flag,
  unsigned int outcount);

extern void record_ref_stats(
    const struct peer *peer,
    int     n,              /* Number of samples */
    int     i,              /* Index of first sample used */
    int     j,              /* Index of last sample used */
    double  t1,             /* Value of first sample */
    double  t2,             /* Value of first sample used */
    double  t3,             /* answer/median */
    double  t4,             /* Value of last sample used */
    double  t5,             /* Value of last sample */
    double  jitter,
    double  std_dev,        /* std deviation of trimmed subset */
    double  std_dev_all     /* std deviation of everything */
    );

extern	void	check_leap_file	(bool is_daily_check, time_t systime);

/* NTS */
extern	void	check_cert_file	(void);

/* packetstamp.c */
extern void	enable_packetstamps(int, sockaddr_u *);
extern l_fp	fetch_packetstamp(struct msghdr *);

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

/* ntp_io.c */
struct ntp_io_data {
  bool disable_dynamic_updates; /* if true, scan interfaces once only */
  unsigned int sys_ifnum;       /* next .ifnum to assign */
  endpt *any_interface;         /* IPv4 wildcard */
  endpt *any6_interface;        /* IPv6 wildcard */
  endpt *loopback_interface;    /* IPv4 loopback for refclocks */
  endpt *ep_list;               /* complete endpt list */
};
extern struct ntp_io_data io_data;

/* ntp_loopfilter.c */
struct ntp_loop_data {
  bool   lockclock;       /* lock to system clock? (externally disciplined?) */
  double drift_comp;      /* clock frequency (s/s) */
  double clock_stability; /* clock stability (wander) (s/s) */
  double clock_max_back;  /* max backward offset before step (s) */
  double clock_max_fwd;   /* max forward offset before step (s) */
  double clock_phi;       /* dispersion rate (s/s) */
};
extern struct ntp_loop_data loop_data;

/*
 * Clock state machine control flags
 */
struct clock_control_flags {
    bool	ntp_enable;		/* clock discipline enabled */
    bool	pll_control;		/* kernel support available */
    bool	kern_enable;		/* kernel support enabled */
    bool	hardpps_enable;		/* kernel PPS discipline enabled */
    bool	allow_panic;		/* allow panic correction (-g) */
    bool	force_step_once;	/* always step time once at startup (-G) */
    bool	mode_ntpdate;		/* exit on first clock set (-q) */
};
extern struct clock_control_flags clock_ctl;
extern bool	cal_enable;		/* refclock calibrate enable */
extern int	peer_ntpdate;		/* count of ntpdate peers */

/*
 * Clock state machine variables
 */
struct clock_state_machine {
  uint8_t sys_poll;     /* system poll interval time constant/poll (log2 s) */
  int     tc_counter;   /* poll-adjust counter */
  double  last_offset;  /* last clock offset (s) */
  uint8_t allan_xpt;    /* Allan intercept (log2 s) */
  double  clock_jitter; /* clock jitter (s) */
  double  sys_offset;   /* system offset (s) */
  double  sys_jitter;   /* system jitter (s) */
};
extern struct clock_state_machine clkstate;

/* ntp_monitor.c */
struct monitor_data {
	uint8_t	mon_hash_bits;		/* log2 size of hash table */
	/*
	 * Pointers to the hash table and the MRU list.  Memory for the hash
	 * table is allocated only if monitoring is enabled.
	 * Total size can easily exceed 32 bits (4 GB)
	 * Total count is unlikely to exceed 32 bits in 2017
	 *   but memories keep growing.
	 */
	mon_entry ** mon_hash;		/* MRU hash table */
	mon_entry mon_mru_list;		/* mru listhead */
	uint64_t	mru_entries;		/* mru list count */
	uint64_t	mru_hashslots;		/* hash slots in use */
	/*
	 * Initialization state.  We may be monitoring, we may not.  If
	 * we aren't, we may not even have allocated any memory yet.
	 */
	unsigned int	mon_enabled;		/* MON_OFF (0) or other MON_* */

/* Slot allocation sizes */
	uint64_t	mru_peakentries;	/* highest mru_entries */
	uint64_t	mru_initalloc;		/* entries to preallocate */
	uint64_t	mru_incalloc;		/* allocation batch factor */
/* Slot (re)allocation parameters */
	uint64_t	mru_mindepth;		/* preempt above this */
	int		mru_maxage;		/* recycle if older than this */
	int		mru_minage;		/* recycle if older & full */
	uint64_t	mru_maxdepth;		/* MRU size hard limit */
/* Slot (re)allocation counters */
	uint64_t	mru_exists;		/* slot already exists */
	uint64_t	mru_new;		/* allocated new slot */
	uint64_t	mru_recycleold;		/* age > maxage */
	uint64_t	mru_recyclefull;	/* full & age > minage */
	uint64_t	mru_none;		/* couldn't allocate slot */
/* rate limiting */
	float		rate_limit;   /* responses per second */
	float		decay_time;   /* seconds, exponential decay time */
	float		kod_limit ;   /* KoDs per second */
};
extern struct monitor_data mon_data;

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
extern int	sys_maxclock;		/* maximum candidates */
struct system_variables {
	uint8_t	sys_leap;		/* system leap indicator */
	uint8_t	sys_stratum;		/* system stratum */
	int8_t	sys_precision;		/* local clock precision */
	double	sys_rootdelay;		/* roundtrip delay to primary source */
	double	sys_rootdisp;		/* dispersion to primary source */
	double	sys_rootdist;		/* distance to primary source */
	refid_t	sys_refid;		/* reference id */
	l_fp	sys_reftime;		/* last update time */
	struct peer *sys_peer;		/* current peer */
};
extern struct system_variables sys_vars;

/*
 * Nonspecified system state variables.
 */
extern l_fp	sys_authdelay;		/* authentication delay */
extern int	sys_minsane;		/* minimum candidates */

/* Signalling: Set by signal handlers */
struct signals_detected {
	bool sawALRM;
	bool sawHUP;
	bool sawDNS;
	bool sawQuit;                   /* SIGQUIT, SIGINT, SIGTERM */
};
extern volatile struct signals_detected sig_flags;


/* ntp_restrict.c */
struct restriction_data {
  restrict_u *restrictlist4; /* IPv4 restriction list */
  restrict_u *restrictlist6; /* IPv6 restriction list */
  int        ntp_minpkt;     /* minimum (log 2 s) */
  uint8_t    ntp_minpoll;    /* increment (log 2 s) */
};
extern struct restriction_data rstrct;

/* ntp_signd.c */

#ifdef ENABLE_MSSNTP
/* ntp_signd.c */
extern void send_via_ntp_signd(struct recvbuf *, keyid_t, int, void *);
#endif

/* ntp_timer.c */
extern unsigned long alarm_overflow;
extern uptime_t	current_time;		/* seconds since startup */
extern uptime_t	timer_timereset;
extern unsigned long	timer_xmtcalls;
extern bool		leap_sec_in_progress;
#ifdef ENABLE_LEAP_SMEAR
extern struct leap_smear_info leap_smear;
extern unsigned int		leap_smear_intv;
#endif

/* ntp_util.c */
extern	char	statsdir[MAXFILENAME];
extern	bool	stats_control;		/* write stats to fileset? */
extern	double	wander_threshold;

/* ntpd.c */
extern	int	waitsync_fd_to_close;	/* -w/--wait-sync */

/* refclock_conf.c */
#ifdef REFCLOCK
/* refclock configuration table */
extern struct refclock * const refclock_conf[];
extern const uint8_t	num_refclock_conf;
#endif

/* nts_extens.c */
bool extens_init(void);
int extens_client_send(struct peer *peer, struct pkt *xpkt);
bool extens_server_recv(struct ntspacket_t *ntspacket, uint8_t *pkt, int lng);
int extens_server_send(struct ntspacket_t *ntspacket, struct pkt *xpkt);
bool extens_client_recv(struct peer *peer, uint8_t *pkt, int lng);

/* nts.c */
void nts_init(void);   /* Before sandbox() */
void nts_init2(void);  /* After sandbox() */
bool nts_probe(struct peer *peer);
bool nts_check(struct peer *peer);
void nts_timer(void);

/* ntp_sandbox.c */
#ifdef HAVE_SECCOMP_H
void setup_SIGSYS_trap(void);
#endif


#endif	/* GUARD_NTPD_H */
