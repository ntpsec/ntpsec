/*
 * ntp_control.c - respond to mode 6 control messages.
 *		   Provides service to ntpq and others.
 */

#include "config.h"

#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>

#include <openssl/evp.h>	/* provides OpenSSL digest API */

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_refclock.h"
#include "ntp_control.h"
#include "ntp_calendar.h"
#include "ntp_stdlib.h"
#include "ntp_config.h"
#include "ntp_assert.h"
#include "ntp_leapsec.h"
#include "lib_strbuf.h"
#include "ntp_syscall.h"
#include "ntp_auth.h"
#include "nts.h"
#include "timespecops.h"

/* Run time constants  */
# include <sys/utsname.h>
struct utsname utsnamebuf;

/* Variables that need updating each time. */
static leap_signature_t lsig;
static struct timex ntx;

/*
 * Statistic counters to keep track of requests and responses.
 */
static unsigned long ctltimereset;	/* time stats reset */
static uint64_t numctlreq;		/* # of requests we've received */
static uint64_t numctlbadpkts;		/* # of bad control packets */
static uint64_t numctlresponses;	/* # of resp packets sent with data */
static uint64_t numctlfrags;		/* # of fragments sent */
static uint64_t numctlerrors;		/* # of error responses sent */
static uint64_t numctltooshort;		/* # of too short input packets */
static uint64_t numctlinputresp;	/* # of responses on input */
static uint64_t numctlinputfrag;	/* # of fragments on input */
static uint64_t numctlinputerr;		/* # of input pkts with err bit set */
static uint64_t numctlbadoffset;	/* # of input pkts with nonzero offset */
static uint64_t numctlbadversion;	/* # of input pkts with unknown version */
static uint64_t numctldatatooshort;	/* data too short for count */
static uint64_t numctlbadop;		/* bad op code found in packet */

static int log_limit = 0;               /* Avoid DDoS to log file */ 

// Refactored C?_VARLIST innards
ssize_t CI_VARLIST(char*, char*, const struct ctl_var*, bool*);
bool CF_VARLIST(const struct ctl_var*, const struct ctl_var*, const struct ctl_var*);

/* undefine to suppress random tags and get fixed emission order */
#define USE_RANDOMIZE_RESPONSES

/*
 * Structure to hold request procedure information
 */

struct ctl_proc {
	short control_code;		/* defined request code */
#define NO_REQUEST	(-1)
	unsigned short flags;			/* flags word */
	/* Only one flag.  Authentication required or not. */
#define NOAUTH	0
#define AUTH	1
	void (*handler) (struct recvbuf *, int); /* handle request */
};


/*
 * Request processing routines
 */
static  void    unmarshall_ntp_control(struct ntp_control *, struct recvbuf *);
static  uint16_t extract_16bits_from_stream(uint8_t *);
static	void	ctl_error	(uint8_t);
#ifdef REFCLOCK
static	unsigned short ctlclkstatus	(struct refclockstat *);
#endif
static	void	ctl_flushpkt	(uint8_t);
static	void	ctl_putdata	(const char *, unsigned int, bool);
static	void	ctl_putstr	(const char *, const char *, size_t);
static	void	ctl_putdblf	(const char *, bool, int, double);
#define	ctl_putdbl(tag, d)	ctl_putdblf(tag, true, 3, d)
#define	ctl_putdbl6(tag, d)	ctl_putdblf(tag, true, 6, d)
#define	ctl_putsfp(tag, sfp)	ctl_putdblf(tag, false, -1, FP_UNSCALE(sfp))
static	void	ctl_putuint	(const char *, uint64_t);
static	void	ctl_puthex	(const char *, uint64_t);
static	void	ctl_putint	(const char *, long);
static	void	ctl_putts	(const char *, l_fp);
static	void	ctl_putadr	(const char *, refid_t, sockaddr_u *);
static	void	ctl_putrefid	(const char *, refid_t);
static	void	ctl_putarray	(const char *, double *, int);
static	void	ctl_putpeer	(int, struct peer *);
static	void	ctl_puttime	(const char *, time_t);
#ifdef REFCLOCK
static	void	ctl_putclock	(int, struct refclockstat *, bool);
#endif	/* REFCLOCK */

/* 2023-Jan-14 Fedora c compiler barfs on function with
 * arg of const struct foo * unless that is used as a result previously.
 */
static	const struct var * ctl_getitem(const struct var *, char **);
static	void	ctl_putsys	(const struct var *);
static	void	ctl_putspecial	(const struct var *);

static	const struct ctl_var *ctl_getitem2(const struct ctl_var *, char **);
static	unsigned short ctlsysstatus	(void);
static	unsigned short	count_var	(const struct ctl_var *);
static	void	control_unspec	(struct recvbuf *, int);
static	void	read_status	(struct recvbuf *, int);
static	void	read_sysvars	(void);
static	void	read_peervars	(void);
static	void	read_variables	(struct recvbuf *, int);
static	void	read_clockstatus(struct recvbuf *, int);
static	void	configure	(struct recvbuf *, int);
static	void	send_mru_entry	(mon_entry *, int);
#ifdef USE_RANDOMIZE_RESPONSES
static	void	send_random_tag_value(int);
#endif /* USE_RANDOMIZE_RESPONSES */
static	void	read_mru_list	(struct recvbuf *, int);
static	void	send_ifstats_entry(endpt *, unsigned int);
static	void	read_ifstats	(struct recvbuf *);
static	void	sockaddrs_from_restrict_u(sockaddr_u *,	sockaddr_u *,
					  restrict_u *, int);
static	void	send_restrict_entry(restrict_u *, int, unsigned int);
static	void	send_restrict_list(restrict_u *, int, unsigned int *);
static	void	read_addr_restrictions(struct recvbuf *);
static	void	read_ordlist	(struct recvbuf *, int);
static	uint32_t	derive_nonce	(sockaddr_u *, uint32_t, uint32_t);
static	void	generate_nonce	(struct recvbuf *, char *, size_t);
static	int	validate_nonce	(const char *, struct recvbuf *);
static	void	req_nonce	(struct recvbuf *, int);

static const struct ctl_proc control_codes[] = {
	{ CTL_OP_UNSPEC,		NOAUTH,	control_unspec },
	{ CTL_OP_READSTAT,		NOAUTH,	read_status },
	{ CTL_OP_READVAR,		NOAUTH,	read_variables },
	{ CTL_OP_WRITEVAR,		AUTH,	NULL },
	{ CTL_OP_READCLOCK,		NOAUTH,	read_clockstatus },
	{ CTL_OP_WRITECLOCK,		NOAUTH,	NULL },
	{ CTL_OP_CONFIGURE,		AUTH,	configure },
	{ CTL_OP_READ_MRU,		NOAUTH,	read_mru_list },
	{ CTL_OP_READ_ORDLIST_A,	AUTH,	read_ordlist },
	{ CTL_OP_REQ_NONCE,		NOAUTH,	req_nonce },
	{ NO_REQUEST,			0,	NULL }
};

#ifndef ENABLE_FUZZ
static const double dbl_zero = 0.0;
#endif

enum var_type {v_time,
	v_str, v_dbl, v_uli, v_li, v_uint, v_int,
	v_u64, v_i64, v_u32, v_i32, v_u8, v_i8, v_bool,
	v_strP, v_u64P, v_u32P, v_uliP,
	v_l_fp, v_l_fp_ms, v_l_fp_sec, v_l_fp_sec6,
	v_u64_r, v_l_fp_sec_r,
	v_mrumem,
	v_since, v_kli, v_special};
enum var_type_special {
	vs_peer, vs_peeradr, vs_peermode,
	vs_systime,
	vs_refid, vs_mruoldest, vs_varlist};
struct var {
  const char* name;
  const int flags;
  const enum var_type type;
  union {
    const time_t* time;
    const char* str;
    const double* dbl;
    const unsigned long int* uli;
    const long int* li;
    const unsigned int* uinnt;
    const int* innt;
    const uint64_t* u64;
    const int64_t* i64;
    const uint32_t* u32;
    const int32_t* i32;
    const uint8_t* u8;
    const int8_t* i8;
    const bool* boool;
    const l_fp* l_fp;
    const uptime_t* up;
    const char* (*strP)(void);
    uint64_t (*u64P)(void);
    uint32_t (*u32P)(void);
    unsigned long int (*uliP)(void);
    const enum var_type_special special;
    } p;
  union {
    /* second pointer for returning recent since-stats-logged */
    const uint64_t* u64;
    const uint64_t* l_fp;
    } p2;  
  };

#define Var_time(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_time, .p.time = &xlocation }
#define Var_str(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_str, .p.str = xlocation }
#define Var_dbl(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_dbl, .p.dbl = &xlocation }
#define Var_uli(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_uli, .p.uli = &xlocation }
#define Var_li(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_li, .p.li = &xlocation }
#define Var_uint(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_uint, .p.uinnt = &xlocation }
#define Var_int(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_int, .p.innt = &xlocation }

#define Var_strP(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_strP, .p.strP = xlocation }
#define Var_u64P(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_u64P, .p.u64P = xlocation }
#define Var_u32P(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_u32P, .p.u32P = xlocation }
#define Var_uliP(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_uliP, .p.uliP = xlocation }

#define Var_u64(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_u64, .p.u64 = &xlocation }
#define Var_u64_r(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_u64_r, \
  .p.u64 = &xlocation, .p2.u64 = &(old_##xlocation) }
#define Var_i64(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_i64, .p.i64 = &xlocation }
#define Var_u32(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_u32, .p.u32 = &xlocation }
#define Var_i32(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_i32, .p.i32 = &xlocation }
#define Var_u8(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_u8, .p.u8 = &xlocation }
#define Var_i8(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_i8, .p.i8 = &xlocation }
#define Var_bool(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_bool, .p.boool = &xlocation }

#define Var_l_fp(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_l_fp, .p.l_fp = &xlocation }
#define Var_l_fp_ms(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_l_fp_ms, .p.l_fp = &xlocation }
#define Var_l_fp_sec(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_l_fp_sec, .p.l_fp = &xlocation }
#define Var_l_fp_r(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_l_fp_sec_r, \
  .p.l_fp = &xlocation, .p2.l_fp = &(old_##xlocation) }
#define Var_l_fp_sec6(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_l_fp_sec6, .p.l_fp = &xlocation }
#define Var_since(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_since, .p.up = &xlocation }

#define Var_mrumem(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_mrumem, .p.u64 = &xlocation }
#define Var_kli(xname, xflags, xlocation) { \
  .name = xname, .flags = xflags, .type = v_kli, .p.li = &xlocation }
#define Var_special(xname, xflags, xspecial) { \
  .name = xname, .flags = xflags, .type = v_special, .p.special = xspecial }

static const struct var sys_var[] = {
  Var_u32("ss_uptime", RO, current_time),
  Var_u8("leap", RO|DEF, sys_vars.sys_leap),        // Was RW
  Var_u8("stratum", RO|DEF, sys_vars.sys_stratum),
  Var_i8("precision", RO|DEF, sys_vars.sys_precision),
  Var_dbl("rootdelay", RO|DEF|ToMS, sys_vars.sys_rootdelay),
  Var_dbl("rootdisp", RO|DEF|ToMS, sys_vars.sys_rootdisp),
  Var_dbl("rootdist", RO|DEF|ToMS, sys_vars.sys_rootdist),
  Var_special("refid", RO|DEF, vs_refid),
  Var_l_fp("reftime", RO|DEF, sys_vars.sys_reftime),
  Var_u8("tc", RO|DEF, clkstate.sys_poll),
  Var_special("peer", RO|DEF, vs_peer),
  Var_dbl("offset", RO|DEF|ToMS|DBL6, clkstate.last_offset),
  Var_dbl("frequency", RO|DEF|ToPPM, loop_data.drift_comp),

  Var_dbl("sys_jitter", RO|DEF|ToMS|DBL6, clkstate.sys_jitter),
  Var_dbl("clk_jitter", RO|DEF|ToMS|DBL6, clkstate.clock_jitter),
  Var_special("clock", RO|DEF, vs_systime),
  Var_str("processor", RO|DEF, utsnamebuf.machine),
  Var_str("system", RO|DEF, utsnamebuf.sysname),
  // old code appended release to system
  Var_str("release", RO|DEF, utsnamebuf.release),
  Var_strP("version", RO|DEF, ntpd_version),

  Var_dbl("clk_wander", RO|DEF|ToPPM|DBL6, loop_data.clock_stability),
  Var_special("sys_var_list", RO|DEF, vs_varlist),
  Var_uint("tai", RO|DEF, sys_tai),
  Var_time("leapsec", RO|DEF|N_LEAP, lsig.ttime),
  Var_time("expire", RO|DEF|N_LEAP, lsig.etime),
  Var_u8("mintc", RO|DEF, rstrct.ntp_minpoll),

  Var_uint("mru_enabled", RO, mon_data.mon_enabled),
  Var_u64("mru_hashslots", RO, mon_data.mru_hashslots),
  Var_u64("mru_depth", RO, mon_data.mru_entries),
  Var_u64("mru_deepest", RO, mon_data.mru_peakentries),
  Var_u64("mru_mindepth", RO, mon_data.mru_mindepth),
  Var_int("mru_maxage", RO, mon_data.mru_maxage),
  Var_int("mru_minage", RO, mon_data.mru_minage),
  Var_u64("mru_maxdepth", RO, mon_data.mru_maxdepth),
  Var_mrumem("mru_mem", RO, mon_data.mru_entries),
  Var_mrumem("mru_maxmem", RO, mon_data.mru_maxdepth),
  Var_u64("mru_exists", RO, mon_data.mru_exists),
  Var_u64("mru_new", RO, mon_data.mru_new),
  Var_u64("mru_recycleold", RO, mon_data.mru_recycleold),
  Var_u64("mru_recyclefull", RO, mon_data.mru_recyclefull),
  Var_u64("mru_none", RO, mon_data.mru_none),
  Var_special("mru_oldest_age", RO, vs_mruoldest),

#define Var_Pair(name, location) \
  Var_u64P(name, RO, stat_##location), \
  Var_u64P(name "_r", RO, stat_total_##location)

  Var_u32("ss_uptime", RO, current_time),
  Var_u32P("ss_reset", RO, stat_stattime),
  Var_u32P("ss_reset_r", RO, stat_total_stattime),
  Var_Pair("ss_received", received),
  Var_Pair("ss_thisver", newversion),
  Var_Pair("ss_oldver", oldversion),
  Var_Pair("ss_ver1", version1),
  Var_Pair("ss_ver1client", version1client),
  Var_Pair("ss_ver1zero", version1zero),
  Var_Pair("ss_ver1symm", version1symm),
  Var_Pair("ss_badformat", badlength),
  Var_Pair("ss_badauth", badauth),
  Var_Pair("ss_declined", declined),
  Var_Pair("ss_restricted", restricted),
  Var_Pair("ss_limited", limitrejected),
  Var_Pair("ss_kodsent", kodsent),
  Var_Pair("ss_processed", processed),
#undef Var_Pair

/* We own this one.  See above.  No proc mode.
 * Note that lots of others are not (yet?) in this table.  */
  Var_u64("ss_numctlreq", RO, numctlreq),

  Var_special("peeradr", RO, vs_peeradr),
  Var_special("peermode", RO, vs_peermode),

/* authinfo: Shared Key Authentication */
  Var_since("authreset", RO, auth_timereset),
  Var_l_fp_ms("authdelay", RO, sys_authdelay),
  Var_uint("authkeys", RO, authnumkeys),
  Var_uint("authfreek", RO, authnumfreekeys),
  Var_uli("authklookups", RO, authkeylookups),
  Var_uli("authknotfound", RO, authkeynotfound),
  Var_uli("authencrypts", RO, authencryptions),
  Var_uli("authdigestencrypts", RO, authdigestencrypt),
  Var_uli("authcmacencrypts", RO, authcmacencrypt),
  Var_uli("authdecrypts", RO, authdecryptions),
  Var_uli("authdigestdecrypts", RO, authdigestdecrypt),
  Var_uli("authdigestfails", RO, authdigestfail),
  Var_uli("authcmacdecrypts", RO, authcmacdecrypt),
  Var_uli("authcmacfails", RO, authcmacfail),

/* kerninfo: Kernel timekeeping info */
  Var_kli("koffset", RO|N_CLOCK|KNUToMS, ntx.offset),
  Var_kli("kfreq", RO|N_CLOCK|K_16, ntx.freq),
  Var_kli("kmaxerr", RO|N_CLOCK|KUToMS, ntx.maxerror),
  Var_kli("kesterr", RO|N_CLOCK|KUToMS, ntx.esterror),
  Var_int("kstflags", RO|N_CLOCK, ntx.status),           // turn to text
  Var_li("ktimeconst", RO|N_CLOCK, ntx.constant),
  Var_kli("kprecis", RO|N_CLOCK|KUToMS, ntx.precision),
  Var_kli("kfreqtol", RO|N_CLOCK|K_16, ntx.tolerance),  // Not in man page
  Var_kli("kppsfreq", RO|N_CLOCK|K_16, ntx.ppsfreq),
  Var_kli("kppsstab", RO|N_CLOCK|K_16, ntx.stabil),
  Var_kli("kppsjitter", RO|N_CLOCK|KNUToMS, ntx.jitter),
  Var_int("kppscalibdur", RO|N_CLOCK, ntx.shift),       // 1<<shift
  Var_li("kppscalibs", RO|N_CLOCK, ntx.calcnt),
  Var_li("kppscaliberrs", RO|N_CLOCK, ntx.errcnt),
  Var_li("kppsjitexc", RO|N_CLOCK, ntx.jitcnt),
  Var_li("kppsstbexc", RO|N_CLOCK, ntx.stbcnt),


/* refclock stuff in ntp_io */
  Var_since("iostats_reset", RO, io_timereset),
  Var_u64P("io_dropped", RO, dropped_count),
  Var_u64P("io_ignored", RO, ignored_count),
  Var_u64P("io_received", RO, received_count),
  Var_u64P("io_sent", RO, sent_count),
  Var_u64P("io_sendfailed", RO, notsent_count),
  Var_u64P("io_wakeups", RO, handler_calls_count),
  Var_u64P("io_pkt_reads", RO, handler_pkts_count),
#ifdef REFCLOCK
  Var_u64P("io_ref_reads", RO, handler_refrds_count),
#endif

// receive buffers
  Var_uliP("total_rbuf", RO, total_recvbuffs),
  Var_uliP("free_rbuf", RO, free_recvbuffs),
  Var_uliP("used_rbuf", RO, lowater_additions),

  Var_since("timerstats_reset", RO, timer_timereset),
  Var_uli("timer_overruns", RO, alarm_overflow),
  Var_uli("timer_xmts", RO, timer_xmtcalls),

#ifdef ENABLE_FUZZ
  Var_dbl("fuzz", RO|DBL6|ToMS, sys_fuzz),
  Var_dbl("tick", RO|DBL6|ToMS, sys_tick),
#else
  Var_dbl("fuzz", RO, dbl_zero),
  Var_dbl("tick", RO, dbl_zero),
#endif

  Var_uli("clk_wander_threshold", RO|ToPPM, timer_xmtcalls),

#ifdef ENABLE_LEAP_SMEAR
  /* Old code returned nothing if leap.smear_intv was 0 */
  Var_uint("leapsmearinterval", RO, leap_smear_intv),
  Var_dbl("leapsmearoffset", RO|ToMS, leap_smear.doffset),
#endif

  Var_bool("lockclock", RO, loop_data.lockclock),

#ifndef DISABLE_NTS
#define Var_Pair(name, location) \
  Var_u64(name, RO, location), \
  Var_u64_r(name "_r", RO, location)
#define Var_PairF(name, location) \
  Var_l_fp_sec(name, RO, location), \
  Var_l_fp_r(name "_r", RO, location)
/* ntsinfo: NTS statistics */
  Var_Pair("nts_client_send", nts_cnt.client_send),
  Var_Pair("nts_client_recv_good", nts_cnt.client_recv_good),
  Var_Pair("nts_client_recv_bad", nts_cnt.client_recv_bad),
  Var_Pair("nts_server_send", nts_cnt.server_send),
  Var_Pair("nts_server_recv_good", nts_cnt.server_recv_good),
  Var_Pair("nts_server_recv_bad", nts_cnt.server_recv_bad),
  Var_Pair("nts_cookie_make", nts_cnt.cookie_make),
  Var_Pair("nts_cookie_not_server", nts_cnt.cookie_not_server),
  Var_Pair("nts_cookie_decode_total", nts_cnt.cookie_decode_total),
  Var_Pair("nts_cookie_decode_current", nts_cnt.cookie_decode_current),
  /* Following line is a hack for old versions of ntpq
   * nts_cookie_decode is old name for nts_cookie_decode_current */
  Var_Pair("nts_cookie_decode", nts_cnt.cookie_decode_current),
  Var_Pair("nts_cookie_decode_old", nts_cnt.cookie_decode_old),
  Var_Pair("nts_cookie_decode_old2", nts_cnt.cookie_decode_old2),
  Var_Pair("nts_cookie_decode_older", nts_cnt.cookie_decode_older),
  Var_Pair("nts_cookie_decode_too_old", nts_cnt.cookie_decode_too_old),
  Var_Pair("nts_cookie_decode_error", nts_cnt.cookie_decode_error),
  Var_Pair("nts_ke_serves_good", ntske_cnt.serves_good),
  Var_PairF("nts_ke_serves_good_wall", ntske_cnt.serves_good_wall),
  Var_PairF("nts_ke_serves_good_cpu", ntske_cnt.serves_good_cpu),
  Var_Pair("nts_ke_serves_nossl", ntske_cnt.serves_nossl),
  Var_PairF("nts_ke_serves_nossl_wall", ntske_cnt.serves_nossl_wall),
  Var_PairF("nts_ke_serves_nossl_cpu", ntske_cnt.serves_nossl_cpu),
  Var_Pair("nts_ke_serves_bad", ntske_cnt.serves_bad),
  Var_PairF("nts_ke_serves_bad_wall", ntske_cnt.serves_bad_wall),
  Var_PairF("nts_ke_serves_bad_cpu", ntske_cnt.serves_bad_cpu),
  Var_Pair("nts_ke_probes_good", ntske_cnt.probes_good),
  Var_Pair("nts_ke_probes_bad", ntske_cnt.probes_bad),
#undef Var_Pair
#undef Var_PairF
#endif

#ifdef ENABLE_MSSNTP
#define Var_Pair(name, location) \
  Var_u64(name, RO, location), \
  Var_u64_r(name "_r", RO, location)
#define Var_PairF(name, location) \
  Var_l_fp_sec(name, RO, location), \
  Var_l_fp_r(name "_r", RO, location)
  Var_Pair("mssntp_serves", mssntp_cnt.serves),
  Var_Pair("mssntp_serves_no", mssntp_cnt.serves_no),
  Var_Pair("mssntp_serves_err", mssntp_cnt.serves_err),
  Var_Pair("mssntp_serves_good", mssntp_cnt.serves_good),
  Var_PairF("mssntp_serves_good_wall", mssntp_cnt.serves_good_wall),
  Var_l_fp_sec6("mssntp_serves_good_slowest", RO,
    mssntp_cnt.serves_good_slowest),
  Var_l_fp_sec6("mssntp_serves_good_slowest_r", RO,
    old_mssntp_cnt.serves_good_slowest),
  Var_Pair("mssntp_serves_bad", mssntp_cnt.serves_bad),
  Var_PairF("mssntp_serves_bad_wall", mssntp_cnt.serves_bad_wall),
  Var_l_fp_sec6("mssntp_serves_bad_slowest", RO,
    mssntp_cnt.serves_bad_slowest),
  Var_l_fp_sec6("mssntp_serves_bad_slowest_r", RO,
    old_mssntp_cnt.serves_bad_slowest),
#undef Var_Pair
#undef Var_PairF
#endif


  { .flags=EOV }                  // end marker for scans

};



static struct ctl_var *ext_sys_var = NULL;

/*
 * Peer variable list. Not order-sensitive.
 */
static const struct ctl_var peer_var2[] = {
	{ 0,		PADDING, "" },
#define	CP_CONFIG		1
	{ CP_CONFIG,	RO, "config" },
#define	CP_AUTHENABLE		2
	{ CP_AUTHENABLE, RO,	"authenable" },
#define	CP_AUTHENTIC		3
	{ CP_AUTHENTIC, RO, "authentic" },
#define	CP_SRCADR		4
	{ CP_SRCADR,	RO|DEF , "srcadr" },
#define	CP_SRCPORT		5
	{ CP_SRCPORT,	RO|DEF, "srcport" },
#define	CP_DSTADR		6
	{ CP_DSTADR,	RO|DEF, "dstadr" },
#define	CP_DSTPORT		7
	{ CP_DSTPORT,	RO|DEF, "dstport" },
#define	CP_LEAP			8
	{ CP_LEAP,	RO|DEF, "leap" },
#define	CP_HMODE		9
	{ CP_HMODE,	RO|DEF, "hmode" },
#define	CP_STRATUM		10
	{ CP_STRATUM,	RO|DEF, "stratum" },
#define	CP_PPOLL		11
	{ CP_PPOLL,	RO|DEF, "ppoll" },
#define	CP_HPOLL		12
	{ CP_HPOLL,	RO|DEF, "hpoll" },
#define	CP_PRECISION		13
	{ CP_PRECISION,	RO|DEF, "precision" },
#define	CP_ROOTDELAY		14
	{ CP_ROOTDELAY,	RO|DEF, "rootdelay" },
#define	CP_ROOTDISPERSION	15
	{ CP_ROOTDISPERSION, RO|DEF, "rootdisp" },
#define	CP_REFID		16
	{ CP_REFID,	RO|DEF, "refid" },
#define	CP_REFTIME		17
	{ CP_REFTIME,	RO|DEF, "reftime" },
        /* Placeholder. Reporting of "org" is disabled because
           leaking it creates a vulnerability */
#define	CP_ORG			18
        { CP_ORG,	RO, "org" },
#define	CP_REC			19
	{ CP_REC,	RO|DEF, "rec" },
#define	CP_XMT			20
	{ CP_XMT,	RO|DEF, "xmt" },
#define	CP_REACH		21
	{ CP_REACH,	RO|DEF, "reach" },
#define	CP_UNREACH		22
	{ CP_UNREACH,	RO|DEF, "unreach" },
#define	CP_TIMER		23
	{ CP_TIMER,	RO, "timer" },
#define	CP_DELAY		24
	{ CP_DELAY,	RO|DEF, "delay" },
#define	CP_OFFSET		25
	{ CP_OFFSET,	RO|DEF, "offset" },
#define	CP_JITTER		26
	{ CP_JITTER,	RO|DEF, "jitter" },
#define	CP_DISPERSION		27
	{ CP_DISPERSION, RO|DEF, "dispersion" },
#define	CP_KEYID		28
	{ CP_KEYID,	RO|DEF, "keyid" },
#define	CP_FILTDELAY		29
	{ CP_FILTDELAY,	RO|DEF, "filtdelay" },
#define	CP_FILTOFFSET		30
	{ CP_FILTOFFSET, RO|DEF, "filtoffset" },
#define	CP_PMODE		31
	{ CP_PMODE,	RO|DEF, "pmode" },
#define	CP_RECEIVED		32
	{ CP_RECEIVED,	RO, "received"},
#define	CP_SENT			33
	{ CP_SENT,	RO, "sent" },
#define	CP_FILTERROR		34
	{ CP_FILTERROR,	RO|DEF, "filtdisp" },
#define	CP_FLASH		35
	{ CP_FLASH,	RO|DEF, "flash" },
#define	CP_MODE			36
	{ CP_MODE,	RO|DEF, "mode" },
#define	CP_VARLIST		37
	{ CP_VARLIST,	RO, "peer_var_list" },
#define	CP_RATE			38
	{ CP_RATE,	RO|DEF, "headway" },
#define	CP_BIAS			39
	{ CP_BIAS,	RO|DEF, "bias" },
#define	CP_SRCHOST		40
	{ CP_SRCHOST,	RO|DEF, "srchost" },
#define	CP_TIMEREC		41
	{ CP_TIMEREC,	RO, "timerec" },
#define	CP_TIMEREACH		42
	{ CP_TIMEREACH,	RO, "timereach" },
#define	CP_BADAUTH		43
	{ CP_BADAUTH,	RO, "badauth" },
#define	CP_BOGUSORG		44
	{ CP_BOGUSORG,	RO, "bogusorg" },
#define	CP_OLDPKT		45
	{ CP_OLDPKT,	RO, "oldpkt" },
#define	CP_SELDISP		46
	{ CP_SELDISP,	RO, "seldisp" },
#define	CP_SELBROKEN		47
	{ CP_SELBROKEN,	RO, "selbroken" },
#define	CP_CANDIDATE		48
	{ CP_CANDIDATE, RO, "candidate" },
	/* new in NTPsec */
#define	CP_NTSCOOKIES		49
	{ CP_NTSCOOKIES, RO|DEF, "ntscookies" },
#define	CP_MAXCODE		((sizeof(peer_var2)/sizeof(peer_var2[0])) - 1)
	{ 0,		EOV, "" }
};

#ifdef REFCLOCK
/*
 * Clock variable list. Not order-sensitive.
 */

static const struct ctl_var clock_var2[] = {
	{ 0,			PADDING, "" },
#define	CC_NAME		1
	{ CC_NAME,		RO|DEF, "name" },
#define	CC_TIMECODE	2
	{ CC_TIMECODE,		RO|DEF, "timecode" },
#define	CC_POLL		3
	{ CC_POLL,		RO|DEF, "poll" },
#define	CC_NOREPLY	4
	{ CC_NOREPLY,		RO|DEF, "noreply" },
#define	CC_BADFORMAT	5
	{ CC_BADFORMAT, 	RO|DEF, "badformat" },
#define	CC_BADDATA	6
	{ CC_BADDATA,		RO|DEF, "baddata" },
#define	CC_FUDGETIME1	7
	{ CC_FUDGETIME1,	RO|DEF, "fudgetime1" },
#define	CC_FUDGETIME2	8
	{ CC_FUDGETIME2,	RO|DEF, "fudgetime2" },
#define	CC_FUDGEVAL1	9
	{ CC_FUDGEVAL1, 	RO|DEF, "stratum" },
#define	CC_FUDGEVAL2	10
	{ CC_FUDGEVAL2, 	RO|DEF, "refid" },
#define	CC_FLAGS	11
	{ CC_FLAGS,		RO|DEF, "flags" },
#define	CC_DEVICE	12
	{ CC_DEVICE,		RO|DEF, "device" },
#define	CC_VARLIST	13
	{ CC_VARLIST,		RO, 	"clock_var_list"},
#define	CC_MAXCODE	CC_VARLIST
	{ 0,			EOV,	""  }
};
#endif

/*
 * MRU string constants shared by send_mru_entry() and read_mru_list().
 */
static const char addr_fmt[] =		"addr.%d";
static const char last_fmt[] =		"last.%d";

/*
 * Keyid used for authenticating write requests.
 */
keyid_t ctl_auth_keyid;

/*
 * We keep track of the last error reported by the system internally
 */
static	uint8_t ctl_sys_last_event;
static	uint8_t ctl_sys_num_events;



/*
 * Response packet used by these routines. Also some state information
 * so that we can handle packet formatting within a common set of
 * subroutines.  Note we try to enter data in place whenever possible,
 * but the need to set the more bit correctly means we occasionally
 * use the extra buffer and copy.
 */
static struct ntp_control rpkt;
static uint8_t	res_version;
static uint8_t	res_opcode;
static associd_t res_associd;
static unsigned short	res_frags;	/* datagrams in this response */
static int	res_offset;	/* offset of payload in response */
static uint8_t * datapt;
static int	datalinelen;
static bool	datasent;	/* flag to avoid initial ", " */
static bool	datanotbinflag;
static sockaddr_u *rmt_addr;
static endpt *lcl_inter;

static auth_info* res_auth;  /* !NULL => authenticate */

#define MAXDATALINELEN	(72)

/*
 * Pointers for saving state when decoding request packets
 */
static	char *reqpt;
static	char *reqend;

/*
 * init_control - initialize request data
 */
void
init_control(void) {
	uname(&utsnamebuf);

	ctl_clr_stats();

	ctl_auth_keyid = 0;
	/* these may be unused with the old trap facility gone */
	ctl_sys_last_event = EVNT_UNSPEC;
	ctl_sys_num_events = 0;

#ifdef ENABLE_CLASSIC_MODE
	/* a relic from when there were multiple nonstandard ways to set time */
#define PRESET	"settimeofday=\"clock_settime\""
	set_sys_var(PRESET, sizeof(PRESET), RO);
#undef PRESET
#endif /* ENABLE_CLASSIC_MODE */

}

/*
 * unmarshall_ntp_control - unmarshall data stream into a ntp_sontrol struct
 */
void
unmarshall_ntp_control(struct ntp_control *pkt, struct recvbuf *rbufp) {
	pkt->li_vn_mode = (uint8_t)rbufp->recv_buffer[0];
	pkt->r_m_e_op = (uint8_t)rbufp->recv_buffer[1];
	pkt->sequence = extract_16bits_from_stream(&rbufp->recv_buffer[2]);
	pkt->status = extract_16bits_from_stream(&rbufp->recv_buffer[4]);
	pkt->associd = extract_16bits_from_stream(&rbufp->recv_buffer[6]);
	pkt->offset = extract_16bits_from_stream(&rbufp->recv_buffer[8]);
	pkt->count = extract_16bits_from_stream(&rbufp->recv_buffer[10]);
	memcpy(&pkt->data, rbufp->recv_buffer + 12, 480 + MAX_MAC_LEN);
}

uint16_t
extract_16bits_from_stream(uint8_t *addr) {
	uint16_t var = 0;
	var = (uint16_t)*addr << 8;
	var |= (uint16_t)*(addr + 1);
	var = ntohs(var);
	return var;
}

/*
 * ctl_error - send an error response for the current request
 */
static void
ctl_error(
	uint8_t errcode
	)
{
	int		maclen;

	numctlerrors++;
	DPRINT(3, ("sending control error %u\n", errcode));

	/*
	 * Fill in the fields. We assume rpkt.sequence and rpkt.associd
	 * have already been filled in.
	 */
	rpkt.r_m_e_op = (uint8_t)CTL_RESPONSE | CTL_ERROR |
			(res_opcode & CTL_OP_MASK);
	rpkt.status = htons((unsigned short)(errcode & 0xff) << 8);
	rpkt.count = 0;

	/*
	 * send packet and bump counters
	 */
	if (NULL != res_auth) {
		maclen = authencrypt(res_auth, (uint32_t *)&rpkt,
				     CTL_HEADER_LEN);
		sendpkt(rmt_addr, lcl_inter, &rpkt,
                        (int)CTL_HEADER_LEN + maclen);
	} else
		sendpkt(rmt_addr, lcl_inter, &rpkt, CTL_HEADER_LEN);
}

/*
 * process_control - process an incoming control message
 */
void
process_control(
	struct recvbuf *rbufp,
	int restrict_mask
	)
{
	struct ntp_control *pkt;
	struct ntp_control pkt_core;
	int req_count;
	int req_data;
	const struct ctl_proc *cc;
	keyid_t *pkid;
	int properlen;
	size_t maclen;

	DPRINT(3, ("in process_control()\n"));

	/*
	 * Save the addresses for error responses
	 */
	numctlreq++;
	rmt_addr = &rbufp->recv_srcadr;
	lcl_inter = rbufp->dstadr;
	unmarshall_ntp_control(&pkt_core, rbufp);
	pkt = &pkt_core;

	/*
	 * If the length is less than required for the header, or
	 * it is a response or a fragment, ignore this.
	 */
	if (rbufp->recv_length < (int)CTL_HEADER_LEN
	    || (CTL_RESPONSE | CTL_MORE | CTL_ERROR) & pkt->r_m_e_op
	    || pkt->offset != 0) {
		DPRINT(1, ("invalid format in control packet\n"));
		if (rbufp->recv_length < (int)CTL_HEADER_LEN)
			numctltooshort++;
		if (CTL_RESPONSE & pkt->r_m_e_op)
			numctlinputresp++;
		if (CTL_MORE & pkt->r_m_e_op)
			numctlinputfrag++;
		if (CTL_ERROR & pkt->r_m_e_op)
			numctlinputerr++;
		if (pkt->offset != 0)
			numctlbadoffset++;
		return;
	}
	res_version = PKT_VERSION(pkt->li_vn_mode);
	if (res_version > NTP_VERSION || res_version < NTP_OLDVERSION) {
		DPRINT(1, ("unknown version %d in control packet\n",
			   res_version));
		numctlbadversion++;
		return;
	}

	/*
	 * Pull enough data from the packet to make intelligent
	 * responses
	 */
	rpkt.li_vn_mode = PKT_LI_VN_MODE(sys_vars.sys_leap, res_version,
					 MODE_CONTROL);
	res_opcode = pkt->r_m_e_op;
	rpkt.sequence = pkt->sequence;
	rpkt.associd = pkt->associd;
	rpkt.status = 0;
	res_frags = 1;
	res_offset = 0;
	res_associd = ntohs(pkt->associd);
	res_auth = NULL;
	req_count = (int)ntohs(pkt->count);
	datanotbinflag = false;
	datalinelen = 0;
	datasent = false;
	datapt = rpkt.data;

	if ((rbufp->recv_length & 0x3) != 0)
		DPRINT(3, ("Control packet length %zu unrounded\n",
			   rbufp->recv_length));

	/*
	 * We're set up now. Make sure we've got at least enough
	 * incoming data space to match the count.
	 */
	req_data = (int)rbufp->recv_length - (int)CTL_HEADER_LEN;
	if (req_data < req_count || rbufp->recv_length & 0x3) {
		ctl_error(CERR_BADFMT);
		numctldatatooshort++;
		return;
	}

	if (CTL_MAX_DATA_LEN < req_count) {
                /* count too big */
		ctl_error(CERR_BADFMT);
		numctlbadpkts++;
		return;
	}

	properlen = req_count + (int)CTL_HEADER_LEN;
	/* round up proper len to a 4 octet boundary */

	properlen = (properlen + 3) & ~3;
	maclen = rbufp->recv_length - (size_t)properlen;
	if ((rbufp->recv_length & 3) == 0 &&
	    maclen >= MIN_MAC_LEN && maclen <= MAX_MAC_LEN) {
		keyid_t keyid;
		pkid = (void *)((char *)pkt + properlen);
		keyid = ntohl(*pkid);
		DPRINT(3, ("recv_len %zu, properlen %d, wants auth with keyid %08x, MAC length=%zu\n",
			   rbufp->recv_length, properlen, keyid,
			   maclen));

		res_auth = authlookup(keyid, true);  // FIXME
		if (NULL == res_auth)
			DPRINT(3, ("invalid keyid %08x\n", keyid));
		else if (authdecrypt(res_auth, (uint32_t *)pkt,
				     (int)rbufp->recv_length - (int)maclen,
				     (int)maclen)) {
			DPRINT(3, ("authenticated okay\n"));
		} else {
			res_auth = NULL;
			DPRINT(3, ("authentication failed\n"));
		}
	}

	/*
	 * Set up translate pointers
	 */
	reqpt = (char *)pkt->data;
#ifndef __COVERITY__
	if (CTL_MAX_DATA_LEN < req_count) {
                /* count too big - backstop to prevent stack overflow*/
		/* coverity[deadcode] */
		ctl_error(CERR_BADFMT);
		numctlbadpkts++;
		return;
	}
#endif /* __COVERITY__ */
	reqend = reqpt + req_count;

	/*
	 * Look for the opcode processor
	 */
	for (cc = control_codes; cc->control_code != NO_REQUEST; cc++) {
		if (cc->control_code == res_opcode) {
			DPRINT(3, ("opcode %d, found command handler\n",
				   res_opcode));
			if (NULL == cc->handler) {
				ctl_error(CERR_BADOP);  // Not Implemented
				return;
			}
			if (cc->flags == AUTH
			    && (NULL == res_auth
				|| res_auth->keyid != ctl_auth_keyid)) {
				ctl_error(CERR_PERMISSION);
				return;
			}
			(cc->handler)(rbufp, restrict_mask);
			return;
		}
	}

	/*
	 * Can't find this one, return an error.
	 */
	numctlbadop++;
	ctl_error(CERR_BADOP);
	return;
}


/*
 * ctlpeerstatus - return a status word for this peer
 */
unsigned short
ctlpeerstatus(
	struct peer *p
	)
{
	unsigned short status;

	status = p->status;
	if (FLAG_CONFIG & p->cfg.flags)
		status |= CTL_PST_CONFIG;
	if ((p->cfg.peerkey) || (FLAG_NTS & p->cfg.flags))
		status |= CTL_PST_AUTHENABLE;
	if (FLAG_AUTHENTIC & p->cfg.flags)
		status |= CTL_PST_AUTHENTIC;
	if (p->reach)
		status |= CTL_PST_REACH;
	if (MDF_TXONLY_MASK & p->cast_flags)
		status |= CTL_PST_BCAST;

	return CTL_PEER_STATUS(status, p->num_events, p->last_event);
}


/*
 * ctlclkstatus - return a status word for this clock
 */
#ifdef REFCLOCK
static unsigned short
ctlclkstatus(
	struct refclockstat *pcs
	)
{
	return CTL_PEER_STATUS(0, pcs->lastevent, pcs->currentstatus);
}
#endif


/*
 * ctlsysstatus - return the system status word
 */
static unsigned short
ctlsysstatus(void)
{
	uint8_t this_clock;

	this_clock = CTL_SST_TS_UNSPEC;
#ifdef REFCLOCK
	if (sys_vars.sys_peer != NULL) {
		if (CTL_SST_TS_UNSPEC != sys_vars.sys_peer->sstclktype)
			this_clock = sys_vars.sys_peer->sstclktype;
	}
#else /* REFCLOCK */
	if (sys_vars.sys_peer != 0)
		this_clock = CTL_SST_TS_NTP;
#endif /* REFCLOCK */
	return CTL_SYS_STATUS(sys_vars.sys_leap, this_clock, ctl_sys_num_events,
			      ctl_sys_last_event);
}


/*
 * ctl_flushpkt - write out the current packet and prepare
 *		  another if necessary.
 */
static void
ctl_flushpkt(
	uint8_t more
	)
{
	int dlen;
	int sendlen;
	int maclen;
	int totlen;

	dlen = datapt - rpkt.data;
	if (!more && datanotbinflag && dlen + 2 < CTL_MAX_DATA_LEN) {
		/*
		 * Big hack, output a trailing \r\n
		 */
		*datapt++ = '\r';
		*datapt++ = '\n';
		dlen += 2;
	}
	sendlen = dlen + (int)CTL_HEADER_LEN;

	/*
	 * Zero-fill the unused part of the packet.  This wasn't needed
	 * when the clients were all in C, for which the first NUL is
	 * a string terminator.  But Python allows NULs in strings,
	 * which means Python mode 6 clients might actually see the trailing
	 * garbage.
	 */
	memset(rpkt.data + sendlen, '\0', sizeof(rpkt.data) - (size_t)sendlen);

	/*
	 * Pad to a multiple of 32 bits
	 */
	while (sendlen & 0x3) {
		sendlen++;
	}

	/*
	 * Fill in the packet with the current info
	 */
	rpkt.r_m_e_op = CTL_RESPONSE | more |
			(res_opcode & CTL_OP_MASK);
	rpkt.count = htons((unsigned short)dlen);
	rpkt.offset = htons((unsigned short)res_offset);
	if (NULL != res_auth) {
		keyid_t keyid;
		totlen = sendlen;
		/*
		 * If we are going to authenticate, then there
		 * is an additional requirement that the MAC
		 * begin on a 64 bit boundary.
		 */
		while (totlen & 7) {
			totlen++;
		}
		keyid = htonl(res_auth->keyid);
		memcpy(datapt, &keyid, sizeof(keyid));
		maclen = authencrypt(res_auth,
				     (uint32_t *)&rpkt, totlen);
		sendpkt(rmt_addr, lcl_inter, &rpkt, totlen + maclen);
	} else {
		sendpkt(rmt_addr, lcl_inter, &rpkt, sendlen);
	}
	if (more) {
		numctlfrags++;
	} else {
		numctlresponses++;
	}

	/*
	 * Set us up for another go around.
	 */
	res_frags++;
	res_offset += dlen;
	datapt = rpkt.data;
}


/*
 * ctl_putdata - write data into the packet, fragmenting and starting
 * another if this one is full.
 */
static void
ctl_putdata(
	const char *dp,
	unsigned int dlen,
	bool bin		/* set to true when data is binary */
	)
{
	unsigned int overhead;
	unsigned int currentlen;
	const uint8_t * dataend = &rpkt.data[CTL_MAX_DATA_LEN];

	overhead = 0;
	if (!bin) {
	    datanotbinflag = true;
	    overhead = 3;
	    if (datasent) {
		*datapt++ = ',';
		datalinelen++;
		if ((dlen + (unsigned int)datalinelen + 1) >= MAXDATALINELEN) {
			*datapt++ = '\r';
			*datapt++ = '\n';
			datalinelen = 0;
		} else {
			*datapt++ = ' ';
			datalinelen++;
		}
	    }
	}

	/*
	 * Save room for trailing junk
	 */
	while (dlen + overhead + datapt > dataend) {
		/*
		 * Not enough room in this one, flush it out.
		 */
		currentlen = (unsigned int)(dataend - datapt);
                if (dlen < currentlen) currentlen = dlen;

		memcpy(datapt, dp, currentlen);

		datapt += currentlen;
		dp += currentlen;
		dlen -= currentlen;
		datalinelen += (int)currentlen;

		ctl_flushpkt(CTL_MORE);
	}

	memcpy(datapt, dp, dlen);
	datapt += dlen;
	datalinelen += (int)dlen;
	datasent = true;
}


/*
 * ctl_putstr - write a tagged string into the response packet
 *		in the form:
 *
 *		tag="data"
 *
 *		len is the data length excluding the NUL terminator,
 *		as in ctl_putstr("var", "value", strlen("value"));
 *		The write will be truncated if data contains  a NUL,
 *		so don't do that.
 */
static void
ctl_putstr(
	const char *	tag,
	const char *	data,
	size_t		len
	)
{
	char buffer[512];

        strlcpy(buffer, tag, sizeof(buffer));
        strlcat(buffer, "=\"", sizeof(buffer));
        if (0 < len) {
		strlcat(buffer, data, sizeof(buffer));
	}
        strlcat(buffer, "\"", sizeof(buffer));

	ctl_putdata(buffer, strlen(buffer), false);
}


/*
 * ctl_putunqstr - write a tagged string into the response packet
 *		   in the form:
 *
 *		   tag=data
 *
 *	len is the data length excluding the NUL terminator.
 *	data must not contain a comma or whitespace.
 */
static void
ctl_putunqstr(
	const char *	tag,
	const char *	data,
	size_t		len
	)
{
	char buffer[512];

	if ((strlen(tag) + 2 + len) >= sizeof(buffer)) {
		return;
	}

        strlcpy(buffer, tag, sizeof(buffer));
        strlcat(buffer, "=", sizeof(buffer));
	if (len > 0) {
		strlcat(buffer, data, sizeof(buffer));
	}
	ctl_putdata(buffer, strlen(buffer), false);
}


/*
 * ctl_putdblf - write a tagged, signed double into the response packet
 */
static void
ctl_putdblf(
	const char *	tag,
	bool		use_f,
	int		precision,
	double		d
	)
{
        char buf[50];
        snprintf(buf, sizeof(buf), use_f ? "%.*f" : "%.*g", precision, d);
	ctl_putunqstr(tag, buf, strlen(buf));
}


/*
 * ctl_putuint - write a tagged unsigned integer into the response
 */
static void
ctl_putuint(
	const char *tag,
	uint64_t uval
	)
{
        char buf[50];
        snprintf(buf, sizeof(buf), "%" PRIu64, uval);
	ctl_putunqstr(tag, buf, strlen(buf));
}

/*
 * ctl_puttime - write a decoded filestamp into the response
 */
static void
ctl_puttime(
	const char *tag,
	time_t uval
	)
{
	struct tm tmbuf, *tm = NULL;
        char buf[50];
	tm = gmtime_r(&uval, &tmbuf);
	if (NULL == tm) {
		return;
	}
	snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02dZ",
                 tm->tm_year + 1900,
		 tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
	ctl_putunqstr(tag, buf, strlen(buf));
}


/*
 * ctl_puthex - write a tagged unsigned integer, in hex, into the
 * response
 */
static void
ctl_puthex(
	const char *tag,
	uint64_t uval
	)
{
        char buf[50];
	snprintf(buf, sizeof(buf), "0x%" PRIx64, uval);
	ctl_putunqstr(tag, buf, strlen(buf));
}


/*
 * ctl_putint - write a tagged signed integer into the response
 */
static void
ctl_putint(
	const char *tag,
	long ival
	)
{
        char buf[50];
	snprintf(buf, sizeof(buf), "%ld", ival);
	ctl_putunqstr(tag, buf, strlen(buf));
}


/*
 * ctl_putts - write a tagged timestamp, in hex, into the response
 */
static void
ctl_putts(
	const char *tag,
	l_fp ts
	)
{
	char buf[50];
	snprintf(buf, sizeof(buf), "0x%08x.%08x",
		 (unsigned int)lfpuint(ts), (unsigned int)lfpfrac(ts));
	ctl_putunqstr(tag, buf, strlen(buf));
}


/*
 * ctl_putadr - write an IP address into the response
 */
static void
ctl_putadr(
	const char *tag,
	refid_t addr32,
	sockaddr_u *addr
	)
{
        const char *cq;
	if (NULL == addr) {
		struct in_addr in4;
		in4.s_addr = addr32;
		cq = inet_ntoa(in4);
	} else
		cq = socktoa(addr);
	ctl_putunqstr(tag, cq, strlen(cq));
}


/*
 * ctl_putrefid - send a refid as printable text
 */
static void
ctl_putrefid(
	const char *	tag,
	refid_t		refid
	)
{
        char    buf[sizeof(refid) + 1];
        char *  cp;
        unsigned int i;

        /* refid is really a 4 byte, un-terminated, string */
        cp = (char *)&refid;
        /* make sure all printable */
        for (i = 0; sizeof(refid) > i && '\0' != cp[i]; i++) {
                if (isgraph((int)cp[i]))
                        buf[i] = cp[i];
                else
                        buf[i] = '.';
        }
        buf[i] = '\0';
	ctl_putunqstr(tag, buf, strlen(buf));
}


/*
 * ctl_putarray - write a tagged eight element double array into the response
 */
static void
ctl_putarray(
	const char *tag,
	double *arr,
	int start
	)
{
	char buffer[200];
	char buf[50];
	int i;
	buffer[0] = 0;
	i = start;
	do {
		if (i == 0)
			i = NTP_SHIFT;
		i--;
		snprintf(buf, sizeof(buf), " %.2f", arr[i] * MS_PER_S);
                strlcat(buffer, buf, sizeof(buffer));
	} while (i != start);
	ctl_putunqstr(tag, buffer, strlen(buffer));
}


/*
 * ctl_putsys - output a system variable
 */
static void
ctl_putsys(const struct var * v) {
	static unsigned long ntp_adjtime_time;
	static unsigned long ntp_leap_time;

/* older compilers don't allow declarations on each case without {} */
	    double temp_d;
	    uptime_t temp_up;
            uint64_t mem;

/* FIXME: ****
 * This should get pushed up a layer: flag, once per request
 * This could get data from 2 samples if the clock ticks while we are working..
 */
	/* The Kernel clock variables need up-to-date output of ntp_adjtime() */
	if (v->flags&N_CLOCK && current_time != ntp_adjtime_time) {
		ZERO(ntx);
		if (ntp_adjtime(&ntx) < 0)
                    msyslog(LOG_ERR,
                            "MODE6: ntp_adjtime() for mode 6 query failed: %s", strerror(errno));
                ntp_adjtime_time = current_time;
	}

	/* The leap second variables need up-to-date info */
        if (v->flags&N_LEAP && current_time != ntp_leap_time) {
                leapsec_getsig(&lsig);
                ntp_leap_time = current_time;
        }

	switch (v->type) {

	case v_str: ctl_putstr(v->name, v->p.str, strlen(v->p.str)); break;
	case v_strP: ctl_putstr(v->name, v->p.strP(), strlen(v->p.strP())); break;

	case v_dbl:
	    temp_d = *v->p.dbl;
	    if (v->flags&ToMS) temp_d *= MS_PER_S;   // to Milliseconds
	    if (v->flags&ToPPM) temp_d *= US_PER_S;  // to PPM
	    if (v->flags&DBL6)
		ctl_putdbl6(v->name, temp_d);
	    else
		ctl_putdbl(v->name, temp_d);
	    break;

	case v_kli:
	    if (v->flags&K_16) {
		/* value is scaled by 16 bits */
		temp_d = FP_UNSCALE(*v->p.li);
	    } else {
		temp_d = (double)*v->p.li;
	    };
	    if (v->flags & (KNUToMS | KUToMS)) {
		/* value is in nanoseconds or microseconds */
# ifdef STA_NANO
		if ((v->flags & KNUToMS) && (ntx.status & STA_NANO)) {
			temp_d *= 1E-9;
		} else
# endif
		{
			temp_d *= 1E-6;
		}

		temp_d *= MS_PER_S;
	    }
            ctl_putdbl(v->name, temp_d);
	    break;

	case v_since:
	    temp_up = current_time - *v->p.up;
            ctl_putuint(v->name, temp_up);
	    break;

	case v_uli: ctl_putuint(v->name, *v->p.uli); break;
	case v_uint: ctl_putuint(v->name, *v->p.uinnt); break;
	case v_u64: ctl_putuint(v->name, *v->p.u64); break;
	case v_u64_r:
		ctl_putuint(v->name, *v->p.u64-*v->p2.u64); break;
	case v_u32: ctl_putuint(v->name, *v->p.u32); break;
	case v_u8: ctl_putuint(v->name, *v->p.u8); break;

	case v_u64P: ctl_putuint(v->name, v->p.u64P()); break;
	case v_u32P: ctl_putuint(v->name, v->p.u32P()); break;
	case v_uliP: ctl_putuint(v->name, v->p.uliP()); break;

	case v_li: ctl_putint(v->name, *v->p.li); break;
	case v_int: ctl_putint(v->name, *v->p.innt); break;
	case v_i64: ctl_putint(v->name, *v->p.i64); break;
	case v_i32: ctl_putint(v->name, *v->p.i32); break;
	case v_i8: ctl_putint(v->name, *v->p.i8); break;

	case v_bool: ctl_putint(v->name, *v->p.boool); break;

	case v_time: ctl_puttime(v->name, *v->p.time); break;

	/* time of day */
	case v_l_fp: ctl_putts(v->name, *v->p.l_fp); break;

	/* time differences */
	case v_l_fp_ms:
	    temp_d = lfptod(*v->p.l_fp);
	    temp_d *= MS_PER_S;
	    ctl_putdbl(v->name, temp_d);
            break;
	case v_l_fp_sec:
	    temp_d = lfptod(*v->p.l_fp);
	    ctl_putdbl(v->name, temp_d);
            break;
	case v_l_fp_sec6:
	    temp_d = lfptod(*v->p.l_fp);
	    ctl_putdbl6(v->name, temp_d);
            break;
	case v_l_fp_sec_r:
	    temp_d = lfptod(*v->p.l_fp-*v->p2.l_fp);
	    ctl_putdbl(v->name, temp_d);
            break;

	case v_mrumem:
            mem = *v->p.u64 * sizeof(mon_entry);
            mem = (mem + 512) / 1024;
            ctl_putuint(v->name, mem);
            break;

	case v_special: ctl_putspecial(v); break;

        default: {
            /* -Wswitch-enum will warn if this is possible */
            if (log_limit++ > 10) return;  /* Avoid log file clutter/DDoS */
            msyslog(LOG_ERR, "ERR: ctl_putsys() needs work type=%u\n", v->type);
            break;
            }
	}
}

/*
 * ctl_putspecial - output a system variable -- special cases
 */
static void
ctl_putspecial(const struct var * v) {

/* older compilers don't allow delecrations on each case */
        int64_t i;
	const char *ss = "0.0.0.0:0";
        l_fp tmp;
        uint64_t u;
        l_fp now;
        struct ctl_var *cv;

  switch (v->p.special) {
    case vs_peer:
        i = 0;
        if (NULL != sys_vars.sys_peer)
            i = sys_vars.sys_peer->associd;
        ctl_putuint(v->name, i);
        break;
    case vs_peeradr:
        if (NULL != sys_vars.sys_peer)
           ss = sockporttoa(&sys_vars.sys_peer->srcadr);
        ctl_putstr(v->name, ss, strlen(ss));
        break;
    case vs_peermode:
        u = MODE_UNSPEC;
        if (NULL != sys_vars.sys_peer)
            u = sys_vars.sys_peer->hmode;
        ctl_putuint(v->name, u);
        break;
    case vs_systime:
	get_systime(&tmp);
        ctl_putts(v->name, tmp);
        break;
    case vs_refid:
        if (sys_vars.sys_stratum > 1 &&
            sys_vars.sys_stratum < STRATUM_UNSPEC)
            ctl_putadr(v->name, sys_vars.sys_refid, NULL);
        else
            ctl_putrefid(v->name, sys_vars.sys_refid);
        break;
    case vs_mruoldest:
        get_systime(&now);
        ctl_putuint(v->name, mon_get_oldest_age(now));
        break;
    case vs_varlist:
        for (cv = ext_sys_var; !(EOV & cv->flags); cv++) {
            ctl_putdata(cv->text, strlen(cv->text), false);
        }
        break;
    default:
        /* -Wswitch-enum will warn if this is possible */
        if (log_limit++ > 10) return;  /* Avoid log file clutter/DDoS */
        msyslog(LOG_ERR, "ERR: ctl_putspecial() needs work special=%u\n", v->p.special);
        break;
    }
}

#define CASE_DBL(number, variable)      case number: \
                ctl_putdbl(CV_NAME, variable); \
                break
#define CASE_DBL6(number, variable)     case number: \
                ctl_putdbl6(CV_NAME, variable); \
                break
#define CASE_HEX(number, variable)      case number: \
                ctl_puthex(CV_NAME, variable); \
                break
#define CASE_INT(number, variable)      case number: \
                ctl_putint(CV_NAME, variable); \
                break
#define CASE_UINT(number, variable)     case number: \
                ctl_putuint(CV_NAME, variable); \
                break
#define CASE_TS(number, variable)       case number: \
                ctl_putts(CV_NAME, variable); \
                break

#define CV_NAME peer_var2[id].text
/*
 * ctl_putpeer - output a peer variable
 */
static void
ctl_putpeer(
	int id,
	struct peer *p
	)
{
	switch (id) {

	case CP_CONFIG:
		ctl_putuint(CV_NAME,
			    !(FLAG_PREEMPT & p->cfg.flags));
		break;

	CASE_UINT(CP_AUTHENABLE, !(p->cfg.peerkey));

	case CP_AUTHENTIC:
		ctl_putuint(CV_NAME,
			    !!(FLAG_AUTHENTIC & p->cfg.flags));
		break;

	case CP_SRCADR:
		ctl_putadr(CV_NAME, 0, &p->srcadr);
		break;

	CASE_UINT(CP_SRCPORT, SRCPORT(&p->srcadr));

	case CP_SRCHOST:
		if (p->hostname != NULL)
			ctl_putstr(CV_NAME, p->hostname,
				   strlen(p->hostname));
#ifdef REFCLOCK
		if (p->procptr != NULL) {
		    char buf1[256];
		    strlcpy(buf1, refclock_name(p), sizeof(buf1));
		    ctl_putstr(CV_NAME, buf1, strlen(buf1));
		}
#endif /* REFCLOCK */
		break;

	case CP_DSTADR:
		ctl_putadr(CV_NAME, 0,
			   (p->dstadr != NULL)
				? &p->dstadr->sin
				: NULL);
		break;

	case CP_DSTPORT:
		ctl_putuint(CV_NAME,
			    (p->dstadr != NULL)
				? SRCPORT(&p->dstadr->sin)
				: 0);
		break;

	CASE_UINT(CP_RATE, p->throttle);

	CASE_UINT(CP_LEAP, p->leap);

	CASE_UINT(CP_HMODE, p->hmode);

	CASE_UINT(CP_STRATUM, p->stratum);

	CASE_UINT(CP_PPOLL, p->ppoll);

	CASE_UINT(CP_HPOLL, p->hpoll);

	CASE_INT(CP_PRECISION, p->precision);

	CASE_DBL(CP_ROOTDELAY, p->rootdelay * MS_PER_S);

	CASE_DBL(CP_ROOTDISPERSION, p->rootdisp * MS_PER_S);

	case CP_REFID:
#ifdef REFCLOCK
		if (p->cfg.flags & FLAG_REFCLOCK) {
			ctl_putrefid(CV_NAME, p->refid);
			break;
		}
#endif
		if (p->stratum > 1 && p->stratum < STRATUM_UNSPEC)
			ctl_putadr(CV_NAME, p->refid,
				   NULL);
		else
			ctl_putrefid(CV_NAME, p->refid);
		break;

	CASE_TS(CP_REFTIME, p->reftime);

	CASE_TS(CP_REC, p->dst);

	CASE_TS(CP_XMT, p->xmt);

	case CP_BIAS:
		if ( !D_ISZERO_NS(p->cfg.bias) )
			ctl_putdbl(CV_NAME, p->cfg.bias);
		break;

	CASE_HEX(CP_REACH, p->reach);

	CASE_HEX(CP_FLASH, p->flash);

	case CP_MODE:
#ifdef REFCLOCK
		if (p->cfg.flags & FLAG_REFCLOCK) {
			ctl_putuint(CV_NAME, p->cfg.mode);
			break;
		}
#endif
		break;

	CASE_UINT(CP_UNREACH, (unsigned long)p->unreach);

	CASE_UINT(CP_TIMER, p->nextdate - current_time);

	CASE_DBL6(CP_DELAY, p->delay * MS_PER_S);

	CASE_DBL6(CP_OFFSET, p->offset * MS_PER_S);

	CASE_DBL6(CP_JITTER, p->jitter * MS_PER_S);

	CASE_DBL6(CP_DISPERSION, p->disp * MS_PER_S);

	case CP_KEYID:
		if (p->cfg.peerkey > NTP_MAXKEY)
			ctl_puthex(CV_NAME, p->cfg.peerkey);
		else
			ctl_putuint(CV_NAME, p->cfg.peerkey);
		break;

	case CP_FILTDELAY:
		ctl_putarray(CV_NAME, p->filter_delay,
			     p->filter_nextpt);
		break;

	case CP_FILTOFFSET:
		ctl_putarray(CV_NAME, p->filter_offset,
			     p->filter_nextpt);
		break;

	case CP_FILTERROR:
		ctl_putarray(CV_NAME, p->filter_disp,
			     p->filter_nextpt);
		break;

	CASE_UINT(CP_PMODE, p->pmode);

	CASE_UINT(CP_RECEIVED, p->received);

	CASE_UINT(CP_SENT, p->sent);

	case CP_VARLIST:
		(void)CF_VARLIST(&peer_var2[id], peer_var2, NULL);
		break;

	CASE_UINT(CP_TIMEREC, current_time - p->timereceived);

	CASE_UINT(CP_TIMEREACH, current_time - p->timereachable);

	CASE_UINT(CP_BADAUTH, p->badauth);

	CASE_UINT(CP_BOGUSORG, p->bogusorg);

	CASE_UINT(CP_OLDPKT, p->oldpkt);

	CASE_UINT(CP_SELDISP, p->seldisptoolarge);

	CASE_UINT(CP_SELBROKEN, p->selbroken);

	CASE_UINT(CP_CANDIDATE, p->status);

	CASE_INT(CP_NTSCOOKIES, p->nts_state.count);

	default:
		break;
	}
}
#undef CV_NAME


ssize_t CI_VARLIST(
    char *buffer_lap,
    char *buf_end,
    const struct ctl_var *table,
    bool *first
    ) {
	char *start = buffer_lap;
	char *string_split;
	size_t string_length;
	const struct ctl_var *row;
	if (NULL == table) {
		return 0;
	}
	for (row = table; !(EOV & row->flags); row++) {
		if (PADDING & row->flags)
			continue;
		string_split = strchr(row->text, '=');
		if (string_split == NULL) {
			string_length = strlen(row->text);
		} else {
			string_length = string_split - row->text; 
		}
		if (string_length >= buf_end - buffer_lap - (size_t)1) {
			return -1;
		}
		if (!*first) {
			*buffer_lap++ = ',';
		} else {
			*first = false;
		}
		memcpy(buffer_lap, row->text, string_length);
		buffer_lap+= string_length;
	}
	*buffer_lap = '\0';
	return buffer_lap - start;
}


bool CF_VARLIST(
    const struct ctl_var *entry,
    const struct ctl_var *table1,
    const struct ctl_var *table2
    ) {
	char buf[1500];  // Arbitrary length greeter than used to be.
	char *buffer_lap, *buffer_end;
	bool first = true;
	ssize_t increment;
	memset(buf, '.', sizeof(buf));
	buf[0] = '\0';
	buffer_lap = buf;
	buffer_end = buf + sizeof(buf);
	if (strlen(entry->text) + 4 > sizeof(buf)) {
		return false;	// really long var name
	}

	snprintf(buffer_lap, sizeof(buf), "%s=\"", entry->text);
	buffer_lap += strlen(buffer_lap);
	increment = CI_VARLIST(buffer_lap, buffer_end,
			    table1, &first);
	if (increment <= 0) {
		return false;
	}
	buffer_lap += increment;
	increment = CI_VARLIST(buffer_lap, buffer_end,
			       table2, &first);
	if (increment < 0) {
		return false;
	}
	buffer_lap += increment;
	if (buffer_lap + 2 >= buffer_end)
		return false;

	*buffer_lap++ = '"';
	*buffer_lap = '\0';
	ctl_putdata(buf, (unsigned)(buffer_lap - buf), false);
	return true;
}


#ifdef REFCLOCK
#define CV_NAME clock_var2[id].text
/*
 * ctl_putclock - output clock variables
 */
static void
ctl_putclock(
	int id,
	struct refclockstat *pcs,
	bool mustput
	)
{
	switch (id) {

	case CC_NAME:
		if (pcs->clockname == NULL ||
		    *(pcs->clockname) == '\0') {
			if (mustput)
				ctl_putstr(CV_NAME,
					   "", 0);
		} else {
			ctl_putstr(CV_NAME,
				   pcs->clockname,
				   strlen(pcs->clockname));
		}
		break;
	case CC_TIMECODE:
		ctl_putstr(CV_NAME,
			   pcs->p_lastcode,
			   (unsigned)pcs->lencode);
		break;

	CASE_UINT(CC_POLL, pcs->polls);

	CASE_UINT(CC_NOREPLY, pcs->noresponse);

	CASE_UINT(CC_BADFORMAT, pcs->badformat);

	CASE_UINT(CC_BADDATA, pcs->baddata);

	case CC_FUDGETIME1:
		if (mustput || (pcs->haveflags & CLK_HAVETIME1))
			ctl_putdbl(CV_NAME,
				   pcs->fudgetime1 * MS_PER_S);
		break;

	case CC_FUDGETIME2:
		if (mustput || (pcs->haveflags & CLK_HAVETIME2))
			ctl_putdbl(CV_NAME,
				   pcs->fudgetime2 * MS_PER_S);
		break;

	case CC_FUDGEVAL1:
		if (mustput || (pcs->haveflags & CLK_HAVEVAL1))
			ctl_putint(CV_NAME,
				   pcs->fudgeval1);
		break;

	case CC_FUDGEVAL2:
		if (mustput || (pcs->haveflags & CLK_HAVEVAL2)) {
			if (pcs->fudgeval1 > 1)
				ctl_putadr(CV_NAME,
					   pcs->fudgeval2, NULL);
			else
				ctl_putrefid(CV_NAME,
					     pcs->fudgeval2);
		}
		break;

	CASE_UINT(CC_FLAGS, pcs->flags);

	case CC_DEVICE:
		if (pcs->clockdesc == NULL ||
		    *(pcs->clockdesc) == '\0') {
			if (mustput)
				ctl_putstr(CV_NAME,
					   "", 0);
		} else {
			ctl_putstr(CV_NAME,
				   pcs->clockdesc,
				   strlen(pcs->clockdesc));
		}
		break;

	case CC_VARLIST:
		(void)CF_VARLIST(&clock_var2[id], clock_var2, pcs->kv_list);
		break;

        default:
                /* huh? */
                break;
	}
}
#undef CV_NAME
#endif
#undef CASE_UINT
#undef CASE_TS
#undef CASE_SFP
#undef CASE_INT
#undef CASE_HEX
#undef CASE_DBL6
#undef CASE_DBL


/*
 * ctl_getitem - get the next data item from the incoming packet
 * Return NULL on error, pointer to EOV on can't find.
 * Advance reqpt on success.
 */
static const struct var *
ctl_getitem(
	const struct var *var_list,
	char **data                 // for writes
	)
{
	static char buf[128];
	static u_long quiet_until;
	const struct var *v;
	size_t len;
	char *cp;
	char *tp;

	/* Old code deleted white space.  Don't send it. */

	/* Scan the string in the packet until we hit comma or
	 * EoB. Register position of first '=' on the fly. */
	for (tp = NULL, cp = reqpt; cp < reqend; ++cp) {
		if (*cp == '=' && tp == NULL) {
			tp = cp;
		}
		if (*cp == ',') {
			break;
		}
	}

	/* Process payload for write requests, if any. */
	*data = NULL;
	if (NULL != tp) {
		const char *plhead = tp + 1; /* skip the '=' */
		const char *pltail = cp;
		size_t      plsize;

		/* check payload size, terminate packet on overflow */
		plsize = (size_t)(pltail - plhead);
		if (plsize >= sizeof(buf)-1) {
			goto badpacket;
		}

		/* copy data, NUL terminate, and set result data ptr */
		memcpy(buf, plhead, plsize);
		buf[plsize] = '\0';
		*data = buf;
	} else {
		/* no payload, current end --> current name termination */
		tp = cp;
	}

	len = tp-reqpt;
	for (v = var_list; !(EOV & v->flags); ++v) {
		/* Check if the var name matches the buffer. The
		 * name is bracketed by [reqpt..tp] and not NUL
		 * terminated, and it contains no '=' char.
		 */
		if (len == strlen(v->name) 
                    && 0 == memcmp(reqpt, v->name, len)) {
			break;
		}
	}
	if (EOV & v->flags) {
		return v;
	}
	if (cp < reqend) cp++;  // skip over ","
	reqpt = cp;		// advance past this slot
	return v;

  badpacket:
	/*TODO? somehow indicate this packet was bad, apart from syslog? */
	numctlbadpkts++;
	NLOG(NLOG_SYSEVENT)
	if (quiet_until <= current_time) {
		unsigned int port = SRCPORT(rmt_addr);
		quiet_until = current_time + 300;
	/* The port variable above suppresses a warning on NetBSD 8.0
	 * http://gnats.netbsd.org/cgi-bin/query-pr-single.pl?number=53619
	 * A cast on SRCPORT without the dummy variable didn't work.
	 */
	msyslog(LOG_WARNING,
		"Possible 'ntpdx' exploit from %s#%u"
		" (possibly spoofed)",
	 	socktoa(rmt_addr), port);
	    }
	reqpt = reqend; /* never again for this packet! */
	return NULL;
}


/*
 * ctl_getitem2 - get the next data item from the incoming packet
 */
static const struct ctl_var *
ctl_getitem2(
	const struct ctl_var *var_list,
	char **data
	)
{
	/* [Bug 3008] First check the packet data sanity, then search
	 * the key. This improves the consistency of result values: If
	 * the result is NULL once, it will never be EOV again for this
	 * packet; If it's EOV, it will never be NULL again until the
	 * variable is found and processed in a given 'var_list'. (That
	 * is, a result is returned that is neither NULL nor EOV).
	 */
	static const struct ctl_var eol = { 0, EOV, NULL };
	static char buf[128];
	static u_long quiet_until;
	const struct ctl_var *v;
	char *cp;
	char *tp;

	/*
	 * Part One: Validate the packet state
	 */

	/* Delete leading commas and white space */
	while (reqpt < reqend && (*reqpt == ',' ||
				  isspace((unsigned char)*reqpt))) {
		reqpt++;
	}
	if (reqpt >= reqend) {
		return NULL;
	}

	/* Scan the string in the packet until we hit comma or
	 * EoB. Register position of first '=' on the fly. */
	for (tp = NULL, cp = reqpt; cp < reqend; ++cp) {
		if (*cp == '=' && tp == NULL) {
			tp = cp;
		}
		if (*cp == ',') {
			break;
		}
	}

	/* Process payload, if any. */
	*data = NULL;
	if (NULL != tp) {
		/* eventually strip white space from argument. */
		const char *plhead = tp + 1; /* skip the '=' */
		const char *pltail = cp;
		size_t      plsize;

		while (plhead != pltail && isspace((u_char)plhead[0])) {
			++plhead;
		}
		while (plhead != pltail && isspace((u_char)pltail[-1])) {
			--pltail;
		}

		/* check payload size, terminate packet on overflow */
		plsize = (size_t)(pltail - plhead);
		if (plsize >= sizeof(buf)) {
			goto badpacket;
		}

		/* copy data, NUL terminate, and set result data ptr */
		memcpy(buf, plhead, plsize);
		buf[plsize] = '\0';
		*data = buf;
	} else {
		/* no payload, current end --> current name termination */
		tp = cp;
	}

	/* Part Two
	 *
	 * Now we're sure that the packet data itself is sane. Scan the
	 * list now. Make sure a NULL list is properly treated by
	 * returning a synthetic End-Of-Values record. We must not
	 * return NULL pointers after this point, or the behaviour would
	 * become inconsistent if called several times with different
	 * variable lists after an EoV was returned.  (Such a behavior
	 * actually caused Bug 3008.)
	 */

	if (NULL == var_list)
		return &eol;

	for (v = var_list; !(EOV & v->flags); ++v)
		if (!(PADDING & v->flags)) {
			/* Check if the var name matches the buffer. The
			 * name is bracketed by [reqpt..tp] and not NUL
			 * terminated, and it contains no '=' char. The
			 * lookup value IS NUL-terminated but might
			 * include a '='... We have to look out for
			 * that!
			 */
			const char *sp1 = reqpt;
			const char *sp2 = v->text;

			/* [Bug 3412] do not compare past NUL byte in name */
			while (   (sp1 != tp)
			       && ('\0' != *sp2) && (*sp1 == *sp2)) {
				++sp1;
				++sp2;
			}
			if (sp1 == tp && (*sp2 == '\0' || *sp2 == '='))
				break;
		}

	/* See if we have found a valid entry or not. If found, advance
	 * the request pointer for the next round; if not, clear the
	 * data pointer so we have no dangling garbage here.
	 */
	if (EOV & v->flags)
		*data = NULL;
	else
		reqpt = cp + (cp != reqend);
	return v;

  badpacket:
	/*TODO? somehow indicate this packet was bad, apart from syslog? */
	numctlbadpkts++;
	NLOG(NLOG_SYSEVENT)
	    if (quiet_until <= current_time) {
		    unsigned int port = SRCPORT(rmt_addr);
		    quiet_until = current_time + 300;
	/* The port variable above suppresses a warning on NetBSD 8.0
	 * http://gnats.netbsd.org/cgi-bin/query-pr-single.pl?number=53619
	 * A cast on SRCPORT without the dummy variable didn't work.
	 */
		    msyslog(LOG_WARNING,
			    "Possible 'ntpdx' exploit from %s#%u"
                            " (possibly spoofed)",
			    socktoa(rmt_addr), port);
	    }
	reqpt = reqend; /* never again for this packet! */
	return NULL;
}


/*
 * control_unspec - response to an unspecified op-code
 */
/*ARGSUSED*/
static void
control_unspec(
	struct recvbuf *rbufp,
	int restrict_mask
	)
{
	struct peer *peer;

	UNUSED_ARG(rbufp);
	UNUSED_ARG(restrict_mask);

	/*
	 * What is an appropriate response to an unspecified op-code?
	 * I return no errors and no data, unless a specified association
	 * doesn't exist.
	 */
	if (res_associd) {
		peer = findpeerbyassoc(res_associd);
		if (NULL == peer) {
			ctl_error(CERR_BADASSOC);
			return;
		}
		rpkt.status = htons(ctlpeerstatus(peer));
	} else
		rpkt.status = htons(ctlsysstatus());
	ctl_flushpkt(0);
}


/*
 * read_status - return either a list of associd's, or a particular
 * peer's status.
 */
/*ARGSUSED*/
static void
read_status(
	struct recvbuf *rbufp,
	int restrict_mask
	)
{
	struct peer *peer;
	size_t n;
	/* a_st holds association ID, status pairs alternating */
	unsigned short a_st[CTL_MAX_DATA_LEN / sizeof(unsigned short)];

	UNUSED_ARG(rbufp);
	UNUSED_ARG(restrict_mask);
	DPRINT(3, ("read_status: ID %d\n", res_associd));
	/*
	 * Two choices here. If the specified association ID is
	 * zero we return all known association ID's.  Otherwise
	 * we return a bunch of stuff about the particular peer.
	 */
	if (res_associd) {
		peer = findpeerbyassoc(res_associd);
		if (NULL == peer) {
			ctl_error(CERR_BADASSOC);
			return;
		}
		rpkt.status = htons(ctlpeerstatus(peer));
		if (NULL != res_auth)  /* FIXME: what's this for? */
			peer->num_events = 0;
		/*
		 * For now, output everything we know about the
		 * peer. May be more selective later.
		 */
		for (const struct ctl_var *kv = peer_var2; kv && !(EOV & kv->flags); kv++)
			if (kv->flags & DEF)
				ctl_putpeer(kv->code, peer);
		ctl_flushpkt(0);
		return;
	}
	n = 0;
	rpkt.status = htons(ctlsysstatus());
	for (peer = peer_list; peer != NULL; peer = peer->p_link) {
		a_st[n++] = htons(peer->associd);
		a_st[n++] = htons(ctlpeerstatus(peer));
		/* two entries each loop iteration, so n + 1 */
		if (n + 1 >= COUNTOF(a_st)) {
			ctl_putdata((void *)a_st, n * sizeof(a_st[0]),
				    true);
			n = 0;
		}
	}
	if (n)
		ctl_putdata((void *)a_st, n * sizeof(a_st[0]), true);
	ctl_flushpkt(0);
}


/*
 * read_peervars - half of read_variables() implementation
 */
static void
read_peervars(void)
{
	const struct ctl_var *v;
	struct peer *peer;
	size_t i;
	char *	valuep;
	bool	wants[CP_MAXCODE + 1];
	bool	gotvar;

	/*
	 * Wants info for a particular peer. See if we know
	 * the guy.
	 */
	peer = findpeerbyassoc(res_associd);
	if (NULL == peer) {
		ctl_error(CERR_BADASSOC);
		return;
	}
	rpkt.status = htons(ctlpeerstatus(peer));
	if (NULL != res_auth)  /* FIXME: What's this for?? */
		peer->num_events = 0;
	ZERO(wants);
	gotvar = false;
	while (NULL != (v = ctl_getitem2(peer_var2, &valuep))) {
		if (v->flags & EOV) {
			ctl_error(CERR_UNKNOWNVAR);
			return;
		}
		INSIST(v->code < COUNTOF(wants));
		wants[v->code] = 1;
		gotvar = true;
	}
	if (gotvar) {
		for (i = 1; i < COUNTOF(wants); i++)
			if (wants[i])
				ctl_putpeer((int)i, peer);
	} else
		for (const struct ctl_var *kv = peer_var2; kv && !(EOV & kv->flags); kv++)
			if (kv->flags & DEF)
				ctl_putpeer(kv->code, peer);
	ctl_flushpkt(0);
}


/*
 * read_sysvars - half of read_variables() implementation
 */
static void
read_sysvars(void)
{
	const struct var *v;
	const struct ctl_var *v2;
	char *	valuep;
	const char * pch;

	/*
	 * Wants system variables. Figure out which he wants
	 * and give them to him.
	 */

	/* Old code had a wants bit map.  Two passes.
	 * Maybe to verify all target names before giving a partial answer.
	 */
	rpkt.status = htons(ctlsysstatus());

	if (reqpt == reqend) {
		/* No names provided, send back defaults */
		for (v = sys_var; v && !(EOV & v->flags); v++)
			if (DEF & v->flags)
				ctl_putsys(v);
		for (v2 = ext_sys_var; v2 && !(EOV & v2->flags); v2++)
			if (DEF & v2->flags)
				ctl_putdata(v2->text, strlen(v2->text),
					    false);
		ctl_flushpkt(0);
		return;
	}

/* This code structure is ugly.
 * The basic problem is that parsing the input stream is burried in ctl_getitem
 * and we need to know if parsing failed or it couldn't find a name.
 * If it can't find a name, we try ext_sys_var.
 * Return NULL on error, pointer to EOV on can't find.
 * Advance reqpt on success.
 */
	while (reqpt < reqend) {
		v = ctl_getitem(sys_var, &valuep);
		if (NULL == v)
			break;  // parsing error
		if (!(v->flags & EOV)) {
			ctl_putsys(v);
		} else {
			v2 = ctl_getitem2(ext_sys_var, &valuep);
			if (NULL == v2) {
				ctl_error(CERR_BADVALUE);
				return;
			}

			if (EOV & v2->flags) {
				ctl_error(CERR_UNKNOWNVAR);
				return;
			}
			pch = ext_sys_var[v2->code].text;
			ctl_putdata(pch, strlen(pch), false);
		}
	}

	ctl_flushpkt(0);
}


/*
 * read_variables - return the variables the caller asks for
 */
/*ARGSUSED*/
static void
read_variables(
	struct recvbuf *rbufp,
	int restrict_mask
	)
{
	UNUSED_ARG(rbufp);
	UNUSED_ARG(restrict_mask);

	if (res_associd)
		read_peervars();
	else
		read_sysvars();
}


/*
 * configure() processes ntpq :config/config-from-file, allowing
 *		generic runtime reconfiguration.
 */
static void configure(
	struct recvbuf *rbufp,
	int restrict_mask
	)
{
	size_t data_count;
	int retval;
	bool replace_nl;

	/* I haven't yet implemented changes to an existing association.
	 * Hence check if the association id is 0
	 */
	if (res_associd != 0) {
		ctl_error(CERR_BADVALUE);
		return;
	}

	if (RES_NOMODIFY & restrict_mask) {
		snprintf(remote_config.err_msg,
			 sizeof(remote_config.err_msg),
			 "runtime configuration prohibited by restrict ..."
                         " nomodify");
		ctl_putdata(remote_config.err_msg,
			    strlen(remote_config.err_msg), false);
		ctl_flushpkt(0);
		NLOG(NLOG_SYSINFO)
			msyslog(LOG_NOTICE,
				"MODE6: runtime config from %s rejected due"
                                " to nomodify restriction",
				socktoa(&rbufp->recv_srcadr));
		increment_restricted();
		return;
	}

	/* Initialize the remote config buffer */
	data_count = (size_t)(reqend - reqpt);

	if (data_count > sizeof(remote_config.buffer) - 2) {
		snprintf(remote_config.err_msg,
			 sizeof(remote_config.err_msg),
			 "runtime configuration failed: request too long");
		ctl_putdata(remote_config.err_msg,
			    strlen(remote_config.err_msg), false);
		ctl_flushpkt(0);
		msyslog(LOG_NOTICE,
			"MODE6: runtime config from %s rejected: request"
                        " too long",
			socktoa(&rbufp->recv_srcadr));
		return;
	}

	memcpy(remote_config.buffer, reqpt, data_count);
	if (data_count > 0
	    && '\n' != remote_config.buffer[data_count - 1])
		remote_config.buffer[data_count++] = '\n';
	remote_config.buffer[data_count] = '\0';
	remote_config.pos = 0;
	remote_config.err_pos = 0;
	remote_config.no_errors = 0;

	/* do not include terminating newline in log */
	if (data_count > 0
	    && '\n' == remote_config.buffer[data_count - 1]) {
		remote_config.buffer[data_count - 1] = '\0';
		replace_nl = true;
	} else {
		replace_nl = false;
	}

	DPRINT(1, ("Got Remote Configuration Command: %s\n",
		   remote_config.buffer));
	msyslog(LOG_NOTICE, "MODE6: %s config: %s",
		socktoa(&rbufp->recv_srcadr),
		remote_config.buffer);

	if (replace_nl)
		remote_config.buffer[data_count - 1] = '\n';

	config_remotely(&rbufp->recv_srcadr);

	/*
	 * Check if errors were reported. If not, output 'Config
	 * Succeeded'.  Else output the error count.  It would be nice
	 * to output any parser error messages.
	 */
	if (0 == remote_config.no_errors) {
		retval = snprintf(remote_config.err_msg,
				  sizeof(remote_config.err_msg),
				  "Config Succeeded");
		if (retval > 0)
			remote_config.err_pos += retval;
	}

	ctl_putdata(remote_config.err_msg, (unsigned int)remote_config.err_pos,
                    false);
	ctl_flushpkt(0);

	DPRINT(1, ("Reply: %s\n", remote_config.err_msg));

	if (remote_config.no_errors > 0)
		msyslog(LOG_NOTICE, "MODE6: %d error in %s config",
			remote_config.no_errors,
			socktoa(&rbufp->recv_srcadr));
}


/*
 * derive_nonce - generate client-address-specific nonce value
 *		  associated with a given timestamp.
 */
static uint32_t derive_nonce(
	sockaddr_u *	addr,
	uint32_t		ts_i,
	uint32_t		ts_f
	)
{
	static uint8_t	salt[16];
	static unsigned long	next_salt_update = 0;
	union d_tag {
		uint8_t	digest[EVP_MAX_MD_SIZE];
		uint32_t extract;
	}		d;
	EVP_MD_CTX	*ctx;
	unsigned int	len;

	if (current_time >= next_salt_update) {
		ntp_RAND_bytes(&salt[0], sizeof(salt));
		next_salt_update = current_time+SECSPERHR;
		if (0) msyslog(LOG_INFO, "derive_nonce: update salt, %lld", \
			(long long)next_salt_update);
	}

	ctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(ctx, EVP_md5(), NULL);
	EVP_DigestUpdate(ctx, salt, sizeof(salt));
	EVP_DigestUpdate(ctx, &ts_i, sizeof(ts_i));
	EVP_DigestUpdate(ctx, &ts_f, sizeof(ts_f));
	if (IS_IPV4(addr))
		EVP_DigestUpdate(ctx, &SOCK_ADDR4(addr),
			         sizeof(SOCK_ADDR4(addr)));
	else
		EVP_DigestUpdate(ctx, &SOCK_ADDR6(addr),
			         sizeof(SOCK_ADDR6(addr)));
	EVP_DigestUpdate(ctx, &NSRCPORT(addr), sizeof(NSRCPORT(addr)));
	EVP_DigestUpdate(ctx, salt, sizeof(salt));
	EVP_DigestFinal_ex(ctx, d.digest, &len);
	EVP_MD_CTX_destroy(ctx);

	return d.extract;
}


/*
 * generate_nonce - generate client-address-specific nonce string.
 */
static void generate_nonce(
	struct recvbuf *	rbufp,
	char *			nonce,
	size_t			nonce_octets
	)
{
	uint32_t derived;

	derived = derive_nonce(&rbufp->recv_srcadr,
			       lfpuint(rbufp->recv_time),
			       lfpfrac(rbufp->recv_time));
	snprintf(nonce, nonce_octets, "%08x%08x%08x",
		 lfpuint(rbufp->recv_time), lfpfrac(rbufp->recv_time), derived);
}


/*
 * validate_nonce - validate client-address-specific nonce string.
 *
 * Returns true if the local calculation of the nonce matches the
 * client-provided value and the timestamp is recent enough.
 */
static int validate_nonce(
	const char *		pnonce,
	struct recvbuf *	rbufp
	)
{
	unsigned int	ts_i;
	unsigned int	ts_f;
	l_fp		ts;
	l_fp		now_delta;
	unsigned int	supposed;
	unsigned int	derived;

	if (3 != sscanf(pnonce, "%08x%08x%08x", &ts_i, &ts_f, &supposed))
		return false;

	ts = lfpinit_u(ts_i, (uint32_t)ts_f);
	derived = derive_nonce(&rbufp->recv_srcadr, lfpuint(ts), lfpfrac(ts));
	get_systime(&now_delta);
	now_delta -= ts;

	return (supposed == derived && lfpuint(now_delta) < NONCE_TIMEOUT);
}


#ifdef USE_RANDOMIZE_RESPONSES
/*
 * send_random_tag_value - send a randomly-generated three character
 *			   tag prefix, a '.', an index, a '=' and a
 *			   random integer value.
 *
 * To try to force clients to ignore unrecognized tags in mrulist,
 * reslist, and ifstats responses, the first and last rows are spiced
 * with randomly-generated tag names with correct .# index.  Make it
 * three characters knowing that none of the currently-used subscripted
 * tags have that length, avoiding the need to test for
 * tag collision.
 */
static void
send_random_tag_value(
	int	indx
	)
{
	int	noise;
	char	buf[32];

	/* coverity[DC.WEAK_CRYPTO] */
	noise = random();
	buf[0] = 'a' + noise % 26;
	noise >>= 5;
	buf[1] = 'a' + noise % 26;
	noise >>= 5;
	buf[2] = 'a' + noise % 26;
	noise >>= 5;
	buf[3] = '.';
	snprintf(&buf[4], sizeof(buf) - 4, "%d", indx);
	ctl_putuint(buf, (unsigned long)noise);
}
#endif /* USE_RANDOMIZE_RESPONSE */


/*
 * Send a MRU list entry in response to a "ntpq -c mrulist" operation.
 *
 * To keep clients honest about not depending on the order of values,
 * and thereby avoid being locked into ugly workarounds to maintain
 * backward compatibility later as new fields are added to the response,
 * the order is random.
 */
static void
send_mru_entry(
	mon_entry *	mon,
	int		count
	)
{
	const char first_fmt[] =	"first.%d";
	const char ct_fmt[] =		"ct.%d";
	const char mv_fmt[] =		"mv.%d";
	const char rs_fmt[] =		"rs.%d";
	const char sc_fmt[] =		"sc.%d";
	const char dr_fmt[] =		"dr.%d";
	char	tag[32];
	bool	sent[8]; /* 8 tag=value pairs */
	uint32_t noise;
	unsigned int	which = 0;
	unsigned int	remaining;
	const char * pch;

	remaining = COUNTOF(sent);
	ZERO(sent);
	/* coverity[DC.WEAK_CRYPTO] */
	noise = (uint32_t)random();
	while (remaining > 0) {
#ifdef USE_RANDOMIZE_RESPONSES
	 	which = (noise & 7) % COUNTOF(sent);
#endif /* USE_RANDOMIZE_RESPONSES */
		noise >>= 3;
		while (sent[which])
			which = (which + 1) % COUNTOF(sent);

		switch (which) {

		case 0:
			snprintf(tag, sizeof(tag), addr_fmt, count);
			pch = sockporttoa(&mon->rmtadr);
			ctl_putunqstr(tag, pch, strlen(pch));
			break;

		case 1:
			snprintf(tag, sizeof(tag), last_fmt, count);
			ctl_putts(tag, mon->last);
			break;

		case 2:
			snprintf(tag, sizeof(tag), first_fmt, count);
			ctl_putts(tag, mon->first);
			break;

		case 3:
			snprintf(tag, sizeof(tag), ct_fmt, count);
			ctl_putint(tag, mon->count);
			break;

		case 4:
			snprintf(tag, sizeof(tag), mv_fmt, count);
			ctl_putuint(tag, mon->vn_mode);
			break;

		case 5:
			snprintf(tag, sizeof(tag), rs_fmt, count);
			ctl_puthex(tag, mon->flags);
			break;

		case 6:
			snprintf(tag, sizeof(tag), sc_fmt, count);
			ctl_putdblf(tag, true, 3, mon->score);
			break;

		case 7:
			snprintf(tag, sizeof(tag), dr_fmt, count);
			ctl_putuint(tag, mon->dropped);
			break;

		default:
			/* huh? */
			break;
		}
		sent[which] = true;
		remaining--;
	}
}


/*
 * read_mru_list - supports ntpq's mrulist command.
 *
 * The approach was suggested by Ry Jones.  A finite and variable number
 * of entries are retrieved per request, to avoid having responses with
 * such large numbers of packets that socket buffers are overflowed and
 * packets lost.  The entries are retrieved oldest-first, taking into
 * account that the MRU list will be changing between each request.  We
 * can expect to see duplicate entries for addresses updated in the MRU
 * list during the fetch operation.  In the end, the client can assemble
 * a close approximation of the MRU list at the point in time the last
 * response was sent by ntpd.  The only difference is it may be longer,
 * containing some number of oldest entries which have since been
 * reclaimed.  If necessary, the protocol could be extended to zap those
 * from the client snapshot at the end, but so far that doesn't seem
 * useful.
 *
 * To accommodate the changing MRU list, the starting point for requests
 * after the first request is supplied as a series of last seen
 * timestamps and associated addresses, the newest ones the client has
 * received.  As long as at least one of those entries hasn't been
 * bumped to the head of the MRU list, ntpd can pick up at that point.
 * Otherwise, the request is failed and it is up to ntpq to back up and
 * provide the next newest entry's timestamps and addresses, conceivably
 * backing up all the way to the starting point.
 *
 * input parameters:
 *	nonce=		Regurgitated nonce retrieved by the client
 *			previously using CTL_OP_REQ_NONCE, demonstrating
 *			ability to receive traffic sent to its address.
 *	frags=		Limit on datagrams (fragments) in response.  Used
 *			by newer ntpq versions instead of limit= when
 *			retrieving multiple entries.
 *	limit=		Limit on MRU entries returned.  One of frags= or
 *			limit= must be provided.
 *			limit=1 is a special case:  Instead of fetching
 *			beginning with the supplied starting points
 *			(provided by a last.x and addr.x where 0 <= x
 *			<= 15, default the beginning of time) newer
 *			neighbor, fetch the supplied entries (provided
 *			by addr.x= entries where 0 <= x <= 15), and in
 *			that case the #.last timestamp can be zero.
 *			This enables fetching a multiple entries from
 *			given  IP addresses.  When limit is not one and
 *			frags= is provided, the fragment limit controls.
 *			NOTE: a single mrulist command may cause many
 *			query/response rounds allowing limits as low as
 *			3 to potentially retrieve thousands of entries
 *			in responses.
 *	mincount=	(decimal) Return entries with count >= mincount.
 *	mindrop=	(decimal) Return entries with drop >= mindrop.
 *	minscore=	(float) Return entries with score >= minscore.
 *	laddr=		Return entries associated with the server's IP
 *			address given.  No port specification is needed,
 *			and any supplied is ignored.
 *      recent=		Set the reporting start point to retrieve roughly
 *			a specified number of most recent entries
 *			'Roughly' because the logic cannot anticipate
 *			update volume. Use this to volume-limit the
 *			response when you are monitoring something like
 *			a pool server with a very long MRU list.
 *	resall=		0x-prefixed hex restrict bits which must all be
 *			lit for an MRU entry to be included.
 *			Has precedence over any resany=.
 *	resany=		0x-prefixed hex restrict bits, at least one of
 *			which must be list for an MRU entry to be
 *			included.
 *	last.0=		0x-prefixed hex l_fp timestamp of newest entry
 *			which client previously received.
 *	addr.0=		text of newest entry's IP address and port,
 *			IPv6 addresses in bracketed form: [::]:123
 *	last.1=		timestamp of 2nd newest entry client has.
 *	addr.1=		address of 2nd newest entry.
 *	[...]
 *
 * ntpq provides as many last/addr pairs as will fit in a single request
 * packet, except for the first request in a MRU fetch operation.
 *
 * The response begins with a new nonce value to be used for any
 * followup request.  Following the nonce is the next newer entry than
 * referred to by last.0 and addr.0, if the "0" entry has not been
 * bumped to the front.  If it has, the first entry returned will be the
 * next entry newer than referred to by last.1 and addr.1, and so on.
 * If none of the referenced entries remain unchanged, the request fails
 * and ntpq backs up to the next earlier set of entries to resync.
 *
 * Except for the first response, the response begins with confirmation
 * of the entry that precedes the first additional entry provided:
 *
 *	last.older=	hex l_fp timestamp matching one of the input
 *			.last timestamps, which entry now precedes the
 *			response 0. entry in the MRU list.
 *	addr.older=	text of address corresponding to older.last.
 *
 * And in any case, a successful response contains sets of values
 * comprising entries, with the oldest numbered 0 and incrementing from
 * there:
 *
 *	addr.#		text of IPv4 or IPv6 address and port
 *	last.#		hex l_fp timestamp of last receipt
 *	first.#		hex l_fp timestamp of first receipt
 *	ct.#		count of packets received
 *	mv.#		mode and version
 *	rs.#		restriction mask (RES_* bits)
 *
 * Note the code currently assumes there are no valid three letter
 * tags sent with each row, and needs to be adjusted if that changes.
 *
 * The client should accept the values in any order, and ignore .#
 * values which it does not understand, to allow a smooth path to
 * future changes without requiring a new opcode.  Clients can rely
 * on all *.0 values preceding any *.1 values, that is all values for
 * a given index number are together in the response.
 *
 * The end of the response list is noted with one or two tag=value
 * pairs.  Unconditionally:
 *
 *	now=		0x-prefixed l_fp timestamp at the server marking
 *			the end of the operation.
 *
 * If any entries were returned, now= is followed by:
 *
 *	last.newest=	hex l_fp identical to last.# of the prior
 *			entry.
 */
static void read_mru_list(
	struct recvbuf *rbufp,
	int restrict_mask
	)
{
	static const char	nulltxt[1] = 		{ '\0' };
	static const char	nonce_text[] =		"nonce";
	static const char	frags_text[] =		"frags";
	static const char	limit_text[] =		"limit";
	static const char	mincount_text[] =	"mincount";
	static const char	mindrop_text[] =	"mindrop";
	static const char	minscore_text[] =	"minscore";
	static const char	resall_text[] =		"resall";
	static const char	resany_text[] =		"resany";
	static const char	maxlstint_text[] =	"maxlstint";
	static const char	minlstint_text[] =	"minlstint";
	static const char	laddr_text[] =		"laddr";
	static const char	recent_text[] =		"recent";
	static const char	resaxx_fmt[] =		"0x%hx";

	unsigned int		limit;
	unsigned short		frags;
	unsigned short		resall;
	unsigned short		resany;
	int			mincount;
	unsigned int		mindrop;
	float			minscore;
	unsigned int		maxlstint;
	unsigned int		minlstint;
	sockaddr_u		laddr;
	unsigned int		recent;
	endpt *                 lcladr;
	unsigned int		count;
	static unsigned int	countdown;
	unsigned int		ui;
	unsigned int		uf;
	l_fp			last[16];
	sockaddr_u		addr[COUNTOF(last)];
	char			buf[128];
	struct ctl_var *	in_parms;
	const struct ctl_var *	v;
	const char *		val;
	const char *		pch;
	char *			pnonce;
	int			nonce_valid;
	size_t			i;
	int			priors;
	mon_entry *		mon;
	mon_entry *		prior_mon;
	l_fp			now;

	if (RES_NOMRULIST & restrict_mask) {
		ctl_error(CERR_PERMISSION);
		NLOG(NLOG_SYSINFO)
			msyslog(LOG_NOTICE,
				"MODE6: mrulist from %s rejected due to"
                                " nomrulist restriction",
				socktoa(&rbufp->recv_srcadr));
		increment_restricted();
		return;
	}
	/*
	 * fill in_parms var list with all possible input parameters.
	 */
	in_parms = NULL;
	set_var(&in_parms, nonce_text, sizeof(nonce_text), 0);
	set_var(&in_parms, frags_text, sizeof(frags_text), 0);
	set_var(&in_parms, limit_text, sizeof(limit_text), 0);
	set_var(&in_parms, mincount_text, sizeof(mincount_text), 0);
	set_var(&in_parms, mindrop_text, sizeof(mindrop_text), 0);
	set_var(&in_parms, minscore_text, sizeof(minscore_text), 0);
	set_var(&in_parms, resall_text, sizeof(resall_text), 0);
	set_var(&in_parms, resany_text, sizeof(resany_text), 0);
	set_var(&in_parms, maxlstint_text, sizeof(maxlstint_text), 0);
	set_var(&in_parms, minlstint_text, sizeof(minlstint_text), 0);
	set_var(&in_parms, laddr_text, sizeof(laddr_text), 0);
	set_var(&in_parms, recent_text, sizeof(recent_text), 0);
	for (i = 0; i < COUNTOF(last); i++) {
		snprintf(buf, sizeof(buf), last_fmt, (int)i);
		set_var(&in_parms, buf, strlen(buf) + 1, 0);
		snprintf(buf, sizeof(buf), addr_fmt, (int)i);
		set_var(&in_parms, buf, strlen(buf) + 1, 0);
	}

	/* decode input parms */
	pnonce = NULL;
	frags = 0;
	limit = 0;
	mincount = 0;
	mindrop = 0;
	minscore = 0.0;
	resall = 0;
	resany = 0;
	maxlstint = 0;
	minlstint = 0;
	recent = 0;
	lcladr = NULL;
	priors = 0;
	ZERO(last);
	ZERO(addr);

	/* have to go through '(void*)' to drop 'const' property from pointer.
	 * ctl_getitem2()' needs some cleanup, too.... perlinger@ntp.org
	 */
	while (NULL != (v = ctl_getitem2(in_parms, (void*)&val)) &&
	       !(EOV & v->flags)) {
		int si;

		if (NULL == val)
			val = nulltxt;

		if (!strcmp(nonce_text, v->text)) {
			free(pnonce);
			pnonce = (*val) ? estrdup(val) : NULL;
		} else if (!strcmp(frags_text, v->text)) {
			if (1 != sscanf(val, "%hu", &frags))
				goto blooper;
		} else if (!strcmp(limit_text, v->text)) {
			if (1 != sscanf(val, "%u", &limit))
				goto blooper;
		} else if (!strcmp(mincount_text, v->text)) {
			if (1 != sscanf(val, "%d", &mincount))
				goto blooper;
			if (mincount < 0)
				mincount = 0;
		} else if (!strcmp(mindrop_text, v->text)) {
			if (1 != sscanf(val, "%u", &mindrop))
				goto blooper;
		} else if (!strcmp(minscore_text, v->text)) {
			if (1 != sscanf(val, "%f", &minscore))
				goto blooper;
			if (minscore < 0)
				minscore = 0.0;
		} else if (!strcmp(resall_text, v->text)) {
			if (1 != sscanf(val, resaxx_fmt, &resall))
				goto blooper;
		} else if (!strcmp(resany_text, v->text)) {
			if (1 != sscanf(val, resaxx_fmt, &resany))
				goto blooper;
		} else if (!strcmp(maxlstint_text, v->text)) {
			if (1 != sscanf(val, "%u", &maxlstint))
				goto blooper;
		} else if (!strcmp(minlstint_text, v->text)) {
			if (1 != sscanf(val, "%u", &minlstint))
				goto blooper;
		} else if (!strcmp(laddr_text, v->text)) {
			if (decodenetnum(val, &laddr))
				goto blooper;
			lcladr = getinterface(&laddr, 0);
		} else if (!strcmp(recent_text, v->text)) {
			if (1 != sscanf(val, "%u", &recent))
				goto blooper;
		} else if (1 == sscanf(v->text, last_fmt, &si) &&
			   (size_t)si < COUNTOF(last)) {
			if (2 != sscanf(val, "0x%08x.%08x", &ui, &uf))
				goto blooper;
			last[si] = lfpinit_u(ui, uf);
			if (!SOCK_UNSPEC(&addr[si]) && si == priors)
				priors++;
		} else if (1 == sscanf(v->text, addr_fmt, &si) &&
			   (size_t)si < COUNTOF(addr)) {
			if (decodenetnum(val, &addr[si]))
				goto blooper;
			if (lfpuint(last[si]) && lfpfrac(last[si]) && si == priors)
				priors++;
		} else {
			DPRINT(1, ("read_mru_list: invalid key item: '%s'"
                                   " (ignored)\n",
				   v->text));
			continue;

		blooper:
			DPRINT(1, ("read_mru_list: invalid param for '%s'"
                                   ": '%s' (bailing)\n",
				   v->text, val));
			free(pnonce);
			pnonce = NULL;
			break;
		}
	}
	free_varlist(in_parms);
	in_parms = NULL;

	/* return no responses until the nonce is validated */
	if (NULL == pnonce) {
		return;
	}

	nonce_valid = validate_nonce(pnonce, rbufp);
	free(pnonce);
	if (!nonce_valid) {
		return;
	}

	if ((0 == frags && !(0 < limit && limit <= MRU_ROW_LIMIT)) ||
	    frags > MRU_FRAGS_LIMIT) {
		ctl_error(CERR_BADVALUE);
		return;
	}

	/*
	 * If either frags or limit is not given, use the max.
	 */
	if (0 != frags && 0 == limit) {
		limit = UINT_MAX;
	} else if (0 != limit && 0 == frags)
		frags = MRU_FRAGS_LIMIT;

	mon = NULL;
	if (limit == 1) {
		for (i = 0; i < COUNTOF(last); i++) {
			mon = mon_get_slot(&addr[i]);
			if (mon != NULL) {
				send_mru_entry(mon, i);
			}
		}
		generate_nonce(rbufp, buf, sizeof(buf));
		ctl_putunqstr("nonce", buf, strlen(buf));
		get_systime(&now);
		ctl_putts("now", now);
		ctl_flushpkt(0);
		return;
	}

	/*
	 * Find the starting point if one was provided.
	 */
	for (i = 0; i < (size_t)priors; i++) {
		mon = mon_get_slot(&addr[i]);
		if (mon != NULL) {
			if (mon->last == last[i])
				break;
		}
	}

	/* If a starting point was provided... */
	if (priors) {
		/* and none could be found unmodified... */
		if (NULL == mon) {
			/* tell ntpq to try again with older entries */
			ctl_error(CERR_UNKNOWNVAR);
			return;
		}
		/* confirm the prior entry used as starting point */
		ctl_putts("last.older", mon->last);
		pch = sockporttoa(&mon->rmtadr);
		ctl_putunqstr("addr.older", pch, strlen(pch));

		/*
		 * Move on to the first entry the client doesn't have,
		 * except in the special case of a limit of one.  In
		 * that case return the starting point entry.
		 */
		if (limit > 1)
			mon = PREV_DLIST(mon_data.mon_mru_list, mon, mru);
	} else {	/* start with the oldest */
		mon = TAIL_DLIST(mon_data.mon_mru_list, mru);
		countdown = mon_data.mru_entries;
	}

	/*
	 * send up to limit= entries in up to frags= datagrams
	 */
	get_systime(&now);
	generate_nonce(rbufp, buf, sizeof(buf));
	ctl_putunqstr("nonce", buf, strlen(buf));
	prior_mon = NULL;
	for (count = 0;
	     mon != NULL && res_frags < frags && count < limit;
	     mon = PREV_DLIST(mon_data.mon_mru_list, mon, mru)) {

		if (mon->count < mincount)
			continue;
		if (mon->dropped < mindrop)
			continue;
		if (mon->score < minscore)
			continue;
		if (resall && resall != (resall & mon->flags))
			continue;
		if (resany && !(resany & mon->flags))
			continue;
		if (maxlstint > 0 && lfpuint(now) - lfpuint(mon->last) >
		    maxlstint)
			continue;
		if (minlstint > 0 && lfpuint(now) - lfpuint(mon->last) <
		    minlstint)
			continue;
		if (lcladr != NULL && mon->lcladr != lcladr)
			continue;
		if (recent != 0 && countdown-- > recent)
			continue;
		send_mru_entry(mon, (int)count);
#ifdef USE_RANDOMIZE_RESPONSES
		if (!count)
			send_random_tag_value(0);
#endif /* USE_RANDOMIZE_RESPONSES */
		count++;
		prior_mon = mon;
	}

	/*
	 * If this batch completes the MRU list, say so explicitly with
	 * a now= l_fp timestamp.
	 */
	if (NULL == mon) {
#ifdef USE_RANDOMIZE_RESPONSES
		if (count > 1) {
			send_random_tag_value((int)count - 1);
		}
#endif /* USE_RANDOMIZE_RESPONSES */
		ctl_putts("now", now);
		/* if any entries were returned confirm the last */
		if (prior_mon != NULL)
			ctl_putts("last.newest", prior_mon->last);
	}
	ctl_flushpkt(0);
}

/*
 * Send a ifstats entry in response to a "ntpq -c ifstats" request.
 *
 * To keep clients honest about not depending on the order of values,
 * and thereby avoid being locked into ugly workarounds to maintain
 * backward compatibility later as new fields are added to the response,
 * the order is random.
 */
static void
send_ifstats_entry(
	endpt *	la,
	unsigned int	ifnum
	)
{
	const char addr_fmtu[] =	"addr.%u";
	const char bcast_fmt[] =	"bcast.%u";
	const char en_fmt[] =		"en.%u";	/* enabled */
	const char name_fmt[] =		"name.%u";
	const char flags_fmt[] =	"flags.%u";
	const char rx_fmt[] =		"rx.%u";
	const char tx_fmt[] =		"tx.%u";
	const char txerr_fmt[] =	"txerr.%u";
	const char pc_fmt[] =		"pc.%u";	/* peer count */
	const char up_fmt[] =		"up.%u";	/* uptime */
	char	tag[32];
	uint8_t	sent[IFSTATS_FIELDS]; /* 12 tag=value pairs */
	int	noisebits;
	uint32_t noise;
	unsigned int	which = 0;
	unsigned int	remaining;
	const char *pch;

	remaining = COUNTOF(sent);
	ZERO(sent);
	noise = 0;
	noisebits = 0;
	while (remaining > 0) {
		if (noisebits < 4) {
			/* coverity[DC.WEAK_CRYPTO] */
			noise = (uint32_t)random();
			noisebits = 31;
		}
#ifdef USE_RANDOMIZE_RESPONSES
		which = (noise & 0xf) % COUNTOF(sent);
#endif /* USE_RANDOMIZE_RESPONSES */
		noise >>= 4;
		noisebits -= 4;

		while (sent[which])
			which = (which + 1) % COUNTOF(sent);

		switch (which) {

		case 0:
			snprintf(tag, sizeof(tag), addr_fmtu, ifnum);
			pch = sockporttoa(&la->sin);
			ctl_putunqstr(tag, pch, strlen(pch));
			break;

		case 1:
			snprintf(tag, sizeof(tag), bcast_fmt, ifnum);
			if (INT_BCASTOPEN & la->flags)
				pch = sockporttoa(&la->bcast);
			else
				pch = "";
			ctl_putunqstr(tag, pch, strlen(pch));
			break;

		case 2:
			snprintf(tag, sizeof(tag), en_fmt, ifnum);
			ctl_putint(tag, !la->ignore_packets);
			break;

		case 3:
			snprintf(tag, sizeof(tag), name_fmt, ifnum);
			ctl_putstr(tag, la->name, strlen(la->name));
			break;

		case 4:
			snprintf(tag, sizeof(tag), flags_fmt, ifnum);
			ctl_puthex(tag, la->flags);
			break;

		case 5:
			snprintf(tag, sizeof(tag), rx_fmt, ifnum);
			ctl_putint(tag, la->received);
			break;

		case 6:
			snprintf(tag, sizeof(tag), tx_fmt, ifnum);
			ctl_putint(tag, la->sent);
			break;

		case 7:
			snprintf(tag, sizeof(tag), txerr_fmt, ifnum);
			ctl_putint(tag, la->notsent);
			break;

		case 8:
			snprintf(tag, sizeof(tag), pc_fmt, ifnum);
			ctl_putuint(tag, la->peercnt);
			break;

		case 9:
			snprintf(tag, sizeof(tag), up_fmt, ifnum);
			ctl_putuint(tag, current_time - la->starttime);
			break;

		default:
			/* Get here if IFSTATS_FIELDS is too big. */
			break;
		}
		sent[which] = true;
		remaining--;
	}
#ifdef USE_RANDOMIZE_RESPONSES
	send_random_tag_value((int)ifnum);
#endif /* USE_RANDOMIZE_RESPONSES */
}


/*
 * read_ifstats - send statistics for each local address, exposed by
 *		  ntpq -c ifstats
 */
static void
read_ifstats(
	struct recvbuf *	rbufp
	)
{
	unsigned int	ifidx;
	endpt *	la;

	UNUSED_ARG(rbufp);

	/*
	 * loop over [0..sys_ifnum] searching ep_list for each
	 * ifnum in turn.
	 */
	for (ifidx = 0; ifidx < io_data.sys_ifnum; ifidx++) {
		for (la = io_data.ep_list; la != NULL; la = la->elink)
			if (ifidx == la->ifnum)
				break;
		if (NULL == la)
			continue;
		/* return stats for one local address */
		send_ifstats_entry(la, ifidx);
	}
	ctl_flushpkt(0);
}

static void
sockaddrs_from_restrict_u(
	sockaddr_u *	psaA,
	sockaddr_u *	psaM,
	restrict_u *	pres,
	int		ipv6
	)
{
	ZERO(*psaA);
	ZERO(*psaM);
	if (!ipv6) {
		SET_AF(psaA, AF_INET);
		PSOCK_ADDR4(psaA)->s_addr = htonl(pres->u.v4.addr);
		SET_AF(psaM, AF_INET);
		PSOCK_ADDR4(psaM)->s_addr = htonl(pres->u.v4.mask);
	} else {
		SET_AF(psaA, AF_INET6);
		memcpy(&SOCK_ADDR6(psaA), &pres->u.v6.addr,
		       sizeof(SOCK_ADDR6(psaA)));
		SET_AF(psaM, AF_INET6);
		memcpy(&SOCK_ADDR6(psaM), &pres->u.v6.mask,
		       sizeof(SOCK_ADDR6(psaA)));
	}
}


/*
 * Send a restrict entry in response to a "ntpq -c reslist" request.
 *
 * To keep clients honest about not depending on the order of values,
 * and thereby avoid being locked into ugly workarounds to maintain
 * backward compatibility later as new fields are added to the response,
 * the order is random.
 */
static void
send_restrict_entry(
	restrict_u *	pres,
	int		ipv6,
	unsigned int		idx
	)
{
	const char addr_fmtu[] =	"addr.%u";
	const char mask_fmtu[] =	"mask.%u";
	const char hits_fmt[] =		"hits.%u";
	const char flags_fmt[] =	"flags.%u";
	char		tag[32];
	uint8_t		sent[RESLIST_FIELDS]; /* 4 tag=value pairs */
	int		noisebits;
	uint32_t		noise;
	unsigned int		which = 0;
	unsigned int		remaining;
	sockaddr_u	addr;
	sockaddr_u	mask;
	const char *	pch;
	char *		buf;
	const char *	match_str;
	const char *	access_str;

	sockaddrs_from_restrict_u(&addr, &mask, pres, ipv6);
	remaining = COUNTOF(sent);
	ZERO(sent);
	noise = 0;
	noisebits = 0;
	while (remaining > 0) {
		if (noisebits < 2) {
			/* coverity[DC.WEAK_CRYPTO] */
			noise = (uint32_t)random();
			noisebits = 31;
		}
#ifdef USE_RANDOMIZE_RESPONSES
		which = (noise & 0x3) % COUNTOF(sent);
#endif /* USE_RANDOMIZE_RESPONSES */
		noise >>= 2;
		noisebits -= 2;

		while (sent[which])
			which = (which + 1) % COUNTOF(sent);

		switch (which) {

		case 0:
			snprintf(tag, sizeof(tag), addr_fmtu, idx);
			pch = socktoa(&addr);
			ctl_putunqstr(tag, pch, strlen(pch));
			break;

		case 1:
			snprintf(tag, sizeof(tag), mask_fmtu, idx);
			pch = socktoa(&mask);
			ctl_putunqstr(tag, pch, strlen(pch));
			break;

		case 2:
			snprintf(tag, sizeof(tag), hits_fmt, idx);
			ctl_putuint(tag, pres->hitcount);
			break;

		case 3:
			snprintf(tag, sizeof(tag), flags_fmt, idx);
			match_str = res_match_flags(pres->mflags);
			access_str = res_access_flags(pres->flags);
			if ('\0' == match_str[0]) {
				pch = access_str;
			} else {
				buf = lib_getbuf();
				snprintf(buf, LIB_BUFLENGTH, "%s %s",
					 match_str, access_str);
				pch = buf;
			}
			ctl_putunqstr(tag, pch, strlen(pch));
			break;

		default:
			/* huh? */
			break;
		}
		sent[which] = true;
		remaining--;
	}
#ifdef USE_RANDOMIZE_RESPONSES
	send_random_tag_value((int)idx);
#endif /* USE_RANDOMIZE_RESPONSES */
}


static void
send_restrict_list(
	restrict_u *	pres,
	int		ipv6,
	unsigned int *		pidx
	)
{
	for ( ; pres != NULL; pres = pres->link) {
		send_restrict_entry(pres, ipv6, *pidx);
		(*pidx)++;
	}
}


/*
 * read_addr_restrictions - returns IPv4 and IPv6 access control lists
 */
static void
read_addr_restrictions(
	struct recvbuf *	rbufp
)
{
	unsigned int idx;

	UNUSED_ARG(rbufp);

	idx = 0;
	send_restrict_list(rstrct.restrictlist4, false, &idx);
	send_restrict_list(rstrct.restrictlist6, true, &idx);
	ctl_flushpkt(0);
}


/*
 * read_ordlist - CTL_OP_READ_ORDLIST_A for ntpq -c ifstats & reslist
 */
static void
read_ordlist(
	struct recvbuf *	rbufp,
	int			restrict_mask
	)
{
	const char ifstats_s[] = "ifstats";
	const size_t ifstatint8_ts = COUNTOF(ifstats_s) - 1;
	const char addr_rst_s[] = "addr_restrictions";
	const size_t a_r_chars = COUNTOF(addr_rst_s) - 1;
	struct ntp_control *	cpkt;
	struct ntp_control pkt_core;
	unsigned short		qdata_octets;

	UNUSED_ARG(rbufp);
	UNUSED_ARG(restrict_mask);

	/*
	 * CTL_OP_READ_ORDLIST_A was first named CTL_OP_READ_IFSTATS and
	 * used only for ntpq -c ifstats.  With the addition of reslist
	 * the same opcode was generalized to retrieve ordered lists
	 * which require authentication.  The request data is empty or
	 * contains "ifstats" (not null terminated) to retrieve local
	 * addresses and associated stats.  It is "addr_restrictions"
	 * to retrieve the IPv4 then IPv6 remote address restrictions,
	 * which are access control lists.  Other request data return
	 * CERR_UNKNOWNVAR.
	 */
	unmarshall_ntp_control(&pkt_core, rbufp);
	cpkt = &pkt_core;
	qdata_octets = ntohs(cpkt->count);
	if (0 == qdata_octets || (ifstatint8_ts == qdata_octets &&
	    !memcmp(ifstats_s, cpkt->data, ifstatint8_ts))) {
		read_ifstats(rbufp);
		return;
	}
	if (a_r_chars == qdata_octets &&
	    !memcmp(addr_rst_s, cpkt->data, a_r_chars)) {
		read_addr_restrictions(rbufp);
		return;
	}
	ctl_error(CERR_UNKNOWNVAR);
}


/*
 * req_nonce - CTL_OP_REQ_NONCE for ntpq -c mrulist prerequisite.
 */
static void req_nonce(
	struct recvbuf *	rbufp,
	int			restrict_mask
	)
{
	char	buf[64];

	UNUSED_ARG(restrict_mask);

	generate_nonce(rbufp, buf, sizeof(buf));
	ctl_putunqstr("nonce", buf, strlen(buf));
	ctl_flushpkt(0);
}


/*
 * read_clockstatus - return clock radio status
 */
/*ARGSUSED*/
static void
read_clockstatus(
	struct recvbuf *rbufp,
	int restrict_mask
	)
{
#ifndef REFCLOCK
	UNUSED_ARG(rbufp);
	UNUSED_ARG(restrict_mask);
	/*
	 * If no refclock support, no data to return
	 */
	ctl_error(CERR_BADASSOC);
#else
	const struct ctl_var *	v;
	int			i;
	struct peer *		peer;
	char *			valuep;
	uint8_t *		wants;
	size_t			wants_alloc;
	bool			gotvar;
	struct ctl_var *	kv;
	struct refclockstat	cs;

	UNUSED_ARG(rbufp);
	UNUSED_ARG(restrict_mask);

	if (res_associd != 0) {
		peer = findpeerbyassoc(res_associd);
	} else {
		/*
		 * Find a clock for this jerk.	If the system peer
		 * is a clock use it, else search peer_list for one.
		 */
		if (sys_vars.sys_peer != NULL && (FLAG_REFCLOCK &
		    sys_vars.sys_peer->cfg.flags))
			peer = sys_vars.sys_peer;
		else
			for (peer = peer_list;
			     peer != NULL;
			     peer = peer->p_link)
				if (FLAG_REFCLOCK & peer->cfg.flags)
					break;
	}
	if (NULL == peer || !(FLAG_REFCLOCK & peer->cfg.flags)) {
		ctl_error(CERR_BADASSOC);
		return;
	}
	/*
	 * If we got here we have a peer which is a clock. Get his
	 * status.
	 */
	cs.kv_list = NULL;
	refclock_control(&peer->srcadr, NULL, &cs);
	kv = cs.kv_list;
	/*
	 * Look for variables in the packet.
	 */
	rpkt.status = htons(ctlclkstatus(&cs));
	wants_alloc = CC_MAXCODE + 1 + count_var(kv);
	wants = emalloc_zero(wants_alloc);
	gotvar = false;
	while (NULL != (v = ctl_getitem2(clock_var2, &valuep))) {
		if (!(EOV & v->flags)) {
			wants[v->code] = true;
			gotvar = true;
		} else {
			v = ctl_getitem2(kv, &valuep);
			if (NULL == v) {
				ctl_error(CERR_BADVALUE);
				free(wants);
				free_varlist(cs.kv_list);
				return;
			}

			if (EOV & v->flags) {
				ctl_error(CERR_UNKNOWNVAR);
				free(wants);
				free_varlist(cs.kv_list);
				return;
			}
			wants[CC_MAXCODE + 1 + v->code] = true;
			gotvar = true;
		}
	}

	if (gotvar) {
	    for (i = 1; i <= (int)CC_MAXCODE; i++)
			if (wants[i])
				ctl_putclock(i, &cs, true);
		if (kv != NULL)
			for (i = 0; !(EOV & kv[i].flags); i++)
				if (wants[(unsigned int)i + CC_MAXCODE + 1])
					ctl_putdata(kv[i].text,
						    strlen(kv[i].text),
						    false);
	} else {
		for (v = clock_var2; v != NULL && !(EOV & v->flags); v++)
			if (DEF & v->flags)
				ctl_putclock(v->code, &cs, false);
		for ( ; kv != NULL && !(EOV & kv->flags); kv++)
			if (DEF & kv->flags)
				ctl_putdata(kv->text, strlen(kv->text),
					    false);
	}

	free(wants);
	free_varlist(cs.kv_list);

	ctl_flushpkt(0);
#endif
}


/*
 * report_event - report an event to log files
 *
 * Code lives here because in past times it reported through the
 * obsolete trap facility.
 */
void
report_event(
	int	err,		/* error code */
	struct peer *peer,	/* peer structure pointer */
	const char *str		/* protostats string */
	)
{
	#define NTP_MAXSTRLEN	256	/* max string length */
	char	statstr[NTP_MAXSTRLEN];
	size_t	len;

	/*
	 * Report the error to the protostats file and system log
	 */
	if (peer == NULL) {

		/*
		 * Discard a system report if the number of reports of
		 * the same type exceeds the maximum.
		 */
		if (ctl_sys_last_event != (uint8_t)err) {
			ctl_sys_num_events= 0;
		}
		if (ctl_sys_num_events >= CTL_SYS_MAXEVENTS)
			return;

		ctl_sys_last_event = (uint8_t)err;
		ctl_sys_num_events++;
		snprintf(statstr, sizeof(statstr),
		    "0.0.0.0 %04x %02x %s",
		    ctlsysstatus(), (unsigned)err, eventstr(err));
		if (str != NULL) {
			len = strlen(statstr);
			snprintf(statstr + len, sizeof(statstr) - len,
			    " %s", str);
		}
		NLOG(NLOG_SYSEVENT)
			msyslog(LOG_INFO, "PROTO: %s", statstr);
	} else {

		/*
		 * Discard a peer report if the number of reports of
		 * the same type exceeds the maximum for that peer.
		 */
		const char *	src;
		uint8_t		errlast;

		errlast = (uint8_t)err & ~PEER_EVENT;
		if (peer->last_event != errlast)
			peer->num_events = 0;
		if (peer->num_events >= CTL_PEER_MAXEVENTS)
			return;

		peer->last_event = errlast;
		peer->num_events++;
#ifdef REFCLOCK
		if (IS_PEER_REFCLOCK(peer))
			src = refclock_name(peer);
		else
#endif /* REFCLOCK */
		if (AF_UNSPEC == AF(&peer->srcadr))
		    src = peer->hostname;
		else
		    src = socktoa(&peer->srcadr);

		snprintf(statstr, sizeof(statstr),
		    "%s %04x %02x %s", src,
		    ctlpeerstatus(peer), (unsigned)err, eventstr(err));
		if (str != NULL) {
			len = strlen(statstr);
			snprintf(statstr + len, sizeof(statstr) - len,
			    " %s", str);
		}
		NLOG(NLOG_PEEREVENT)
			msyslog(LOG_INFO, "PROTO: %s", statstr);
	}
	record_proto_stats(statstr);
	DPRINT(1, ("event at %u %s\n", current_time, statstr));

}


/*
 * mprintf_event - printf-style varargs variant of report_event()
 */
int
mprintf_event(
	int		evcode,		/* event code */
	struct peer *	p,		/* may be NULL */
	const char *	fmt,		/* msnprintf format */
	...
	)
{
	va_list	ap;
	int	rc;
	char	msg[512];

	va_start(ap, fmt);
	rc = vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);
	report_event(evcode, p, msg);

	return rc;
}


/*
 * ctl_clr_stats - clear stat counters
 */
void
ctl_clr_stats(void)
{
	ctltimereset = current_time;
	numctlreq = 0;
	numctlbadpkts = 0;
	numctlresponses = 0;
	numctlfrags = 0;
	numctlerrors = 0;
	numctlfrags = 0;
	numctltooshort = 0;
	numctlinputresp = 0;
	numctlinputfrag = 0;
	numctlinputerr = 0;
	numctlbadoffset = 0;
	numctlbadversion = 0;
	numctldatatooshort = 0;
	numctlbadop = 0;
}

static unsigned short
count_var(
	const struct ctl_var *k
	)
{
	unsigned int c;

	if (NULL == k) {
		return 0;
	}

	c = 0;
	while (!(EOV & (k++)->flags))
		c++;

	ENSURE(c <= USHRT_MAX);
	return (unsigned short)c;
}


char *
add_var(
	struct ctl_var **kv,
	unsigned long size,
	unsigned short def
	)
{
	unsigned short	c;
	struct ctl_var *k;
	char *		buf;

	c = count_var(*kv);
	*kv  = erealloc(*kv, (c + 2U) * sizeof(**kv));
	k = *kv;
	buf = emalloc(size);
	k[c].code  = c;
	k[c].text  = buf;
	k[c].flags = def;
	k[c + 1].code  = 0;
	k[c + 1].text  = NULL;
	k[c + 1].flags = EOV;

	return buf;
}


void
set_var(
	struct ctl_var **kv,
	const char *data,
	unsigned long size,
	unsigned short def
	)
{
	struct ctl_var *k;
	const char *s;
	const char *t;
	char *td;

	if (NULL == data || !size) {
		return;
	}

	k = *kv;
	if (k != NULL) {
		while (!(EOV & k->flags)) {
			if (NULL == k->text)	{
				td = emalloc(size);
				memcpy(td, data, size);
				k->text = td;
				k->flags = def;
				return;
			} else {
				s = data;
				t = k->text;
				while (*t != '=' && *s == *t) {
					s++;
					t++;
				}
				if (*s == *t && ((*t == '=') || !*t)) {
                                    td = erealloc((void *)(intptr_t)k->text,
                                                  size);
                                    memcpy(td, data, size);
                                    k->text = td;
                                    k->flags = def;
                                    return;
				}
			}
			k++;
		}
	}
	td = add_var(kv, size, def);
	memcpy(td, data, size);
}


void
set_sys_var(
	const char *data,
	unsigned long size,
	unsigned short def
	)
{
	set_var(&ext_sys_var, data, size, def);
}


/*
 * get_ext_sys_var() retrieves the value of a user-defined variable or
 * NULL if the variable has not been setvar'd.
 */
const char *
get_ext_sys_var(const char *tag)
{
	struct ctl_var *	v;
	size_t			c;
	const char *		val;

	val = NULL;
	c = strlen(tag);
	for (v = ext_sys_var; !(EOV & v->flags); v++) {
		if (NULL != v->text && !memcmp(tag, v->text, c)) {
			if ('=' == v->text[c]) {
				val = v->text + c + 1;
				break;
			} else if ('\0' == v->text[c]) {
				val = "";
				break;
			}
		}
	}

	return val;
}


void
free_varlist(
	struct ctl_var *kv
	)
{
	struct ctl_var *k;
	if (kv) {
		for (k = kv; !(k->flags & EOV); k++)
			free((void *)(intptr_t)k->text);
		free((void *)kv);
	}
}

