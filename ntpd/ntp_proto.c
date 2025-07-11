/*
 * ntp_proto.c - NTP version 4 protocol machinery
 *
 */
#include "config.h"

#include "ntp.h"
#include "ntpd.h"
#include "ntp_endian.h"
#include "ntp_stdlib.h"
#include "ntp_leapsec.h"
#include "ntp_dns.h"
#include "ntp_auth.h"
#include "timespecops.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#ifdef HAVE_LIBSCF_H
#include <libscf.h>
#endif
#include <unistd.h>

#define MSSNTP_QUERY_MAC_LEN 16

/*
 * Byte order conversion
 */
#define	HTONS_FP(x)	(htonl(x))

/*
 * Generate the wire-format version (that is, big-endian all the way down)
 * of a timestamp expressed as a 64-bit scalar.
 */
static inline l_fp_w htonl_fp(l_fp lfp) {
    l_fp_w lfpw;
    lfpw.l_ui = htonl(lfpuint(lfp));
    lfpw.l_uf = htonl(lfpfrac(lfp));
    return lfpw;
}


/*
 * Definitions for the clear() routine.  We use memset() to clear
 * the parts of the peer structure which go to zero.  These are
 * used to calculate the start address and length of the area.
 */
#define	CLEAR_TO_ZERO(p)	((char *)&((p)->clear_to_zero))
#define	END_CLEAR_TO_ZERO(p)	((char *)&((p)->end_clear_to_zero))
#define	LEN_CLEAR_TO_ZERO(p)	(END_CLEAR_TO_ZERO(p) - CLEAR_TO_ZERO(p))
/*
 * traffic shaping parameters
 */
#define	NTP_IBURST	6	/* packets in iburst */
#define	RESP_DELAY	1	/* refclock burst delay (s) */

/*
 * Clock filter algorithm tuning parameters
 */
#define MAXDISPERSE	16.	/* max dispersion */
#define NTP_FWEIGHT	.5	/* clock filter weight */

/*
 * Selection algorithm tuning parameters
 */
#define	NTP_MINCLOCK	3	/* min survivors */
#define	NTP_MAXCLOCK	10	/* max candidates */
#define MINDISTANCE	.001	/* min distance */
#define CLOCK_SGATE	3.	/* popcorn spike gate */
#define	NTP_ORPHWAIT	300	/* orphan wait (s) */

#define DIFF(x, y) (SQUARE((x) - (y)))

/*
 * Dealing with stratum.  0 gets mapped to 16 incoming, and back to 0
 * on output.
 */
#define	PKT_TO_STRATUM(s)	((uint8_t)(((s) == (STRATUM_PKT_UNSPEC)) ?\
				(STRATUM_UNSPEC) : (s)))

#define	STRATUM_TO_PKT(s)	((uint8_t)(((s) == (STRATUM_UNSPEC)) ?\
				(STRATUM_PKT_UNSPEC) : (s)))

/*
 * peer_select groups statistics for a peer used by clock_select() and
 * clock_cluster().
 */
typedef struct peer_select_tag {
	struct peer *	peer;
	double		synch;	/* sync distance */
	double		error;	/* jitter */
	double		seljit;	/* selection jitter */
} peer_select;

/*
 * System variables are declared here. Unless specified otherwise, all
 * times are in seconds.
 */
struct system_variables sys_vars;
static uint8_t	xmt_leap;		/* leap indicator sent in client requests */

#ifdef ENABLE_LEAP_SMEAR
struct leap_smear_info leap_smear;
#endif
bool leap_sec_in_progress;

/*
 * Rate controls. Leaky buckets are used to throttle the packet
 * transmission rates in order to protect busy servers such as at NIST
 * and USNO. There is a counter for each association and another for KoD
 * packets. The association counter decrements each second, but not
 * below zero. Each time a packet is sent the counter is incremented by
 * a configurable value representing the average interval between
 * packets. A packet is delayed as long as the counter is greater than
 * zero. Note this does not affect the time value computations.
 */
/*
 * Nonspecified system state variables
 */
l_fp	sys_authdelay;		/* authentication delay */
double	sys_mindist = MINDISTANCE; /* minimum distance (s) */
static double	sys_maxdist = MAXDISTANCE; /* selection threshold */
double	sys_maxdisp = MAXDISPERSE; /* maximum dispersion */
static unsigned long	sys_epoch;	/* last clock update time */
static	double sys_clockhop;	/* clockhop threshold */
static int leap_vote_ins;	/* leap consensus for insert */
static int leap_vote_del;	/* leap consensus for delete */
static	unsigned long	leapsec;	/* seconds to next leap (proximity class) */
int	peer_ntpdate;		/* active peers in ntpdate mode */
static int sys_survivors;		/* truest of the truechimers */

/*
 * TOS and multicast mapping stuff
 */
static int	sys_floor = 0;			/* cluster stratum floor */
static int	sys_ceiling = STRATUM_UNSPEC - 1; /* cluster stratum ceiling */
int	sys_minsane = 1;	/* minimum candidates */
static int	sys_minclock = NTP_MINCLOCK; /* minimum candidates */
int	sys_maxclock = NTP_MAXCLOCK; /* maximum candidates */
int	sys_orphan = STRATUM_UNSPEC + 1; /* orphan stratum */
static int sys_orphwait = NTP_ORPHWAIT; /* orphan wait */

// proto stats structure and variables
struct statistics_counters {
	uint64_t	sys_received;		/* packets received */
	uint64_t	sys_processed;		/* packets for this host */
	uint64_t	sys_restricted;		/* restricted packets */
	uint64_t	sys_newversion;		/* current version  */
	uint64_t	sys_oldversion;		/* old version */
	uint64_t	sys_version1;		/* old cruft: NTPv1 */
	uint64_t	sys_version1client;	/*  NTPv1: mode 3 many */
	uint64_t	sys_version1zero;	/*  NTPv1: mode 0 as per RFC */
	uint64_t	sys_version1symm;	/*  NTPv1: mode 1 old Windows */
	uint64_t	sys_badlength;		/* bad length or format */
	uint64_t	sys_badauth;		/* bad authentication */
	uint64_t	sys_declined;		/* declined */
	uint64_t	sys_limitrejected;	/* rate exceeded */
	uint64_t	sys_kodsent;		/* KoD sent */
};
volatile struct statistics_counters stat_proto_hourago, stat_proto_total;
uptime_t	sys_stattime;		/* time since sysstats "reset" */

uptime_t	use_stattime;		/* time since usestats reset */

uptime_t stat_stattime(void)
{
  return current_time - sys_stattime;
}

uptime_t stat_total_stattime(void)
{
  return current_time;
}

#define stat_sys_dumps(member)\
uint64_t stat_##member(void) {\
  return stat_proto_total.sys_##member - stat_proto_hourago.sys_##member;\
}\
uint64_t stat_total_##member(void) {\
  return stat_proto_total.sys_##member;\
}

stat_sys_dumps(received)
stat_sys_dumps(processed)
stat_sys_dumps(restricted)
stat_sys_dumps(newversion)
stat_sys_dumps(oldversion)
stat_sys_dumps(version1)
stat_sys_dumps(version1client)
stat_sys_dumps(version1zero)
stat_sys_dumps(version1symm)
stat_sys_dumps(badlength)
stat_sys_dumps(badauth)
stat_sys_dumps(declined)
stat_sys_dumps(limitrejected)
stat_sys_dumps(kodsent)

#undef stat_sys_dumps

void increment_restricted(void)
{
  stat_proto_total.sys_restricted++;
}

uptime_t stat_use_stattime(void)
{
  return current_time - use_stattime;
}

void set_use_stattime(uptime_t stattime) {
  use_stattime = stattime;
}


static	void	clock_combine	(peer_select *, int, int);
static	void	clock_select	(void);
static	void	clock_update	(struct peer *);
static	void	fast_xmit	(struct recvbuf *, int, auth_info*, int);
static	int	local_refid	(struct peer *);
static	void	peer_xmit	(struct peer *);
static	int	peer_unfit	(struct peer *);
static	double	root_distance	(struct peer *);
#ifndef DISABLE_NTS
static	void	restart_nts_ke	(struct peer *);
#endif
static	void	maybe_log_junk	(const char *tag, struct recvbuf *rbuf);

void
set_sys_leap(unsigned char new_sys_leap) {
	sys_vars.sys_leap = new_sys_leap;
	xmt_leap = sys_vars.sys_leap;

	/*
	 * Under certain conditions we send faked leap bits to clients, so
	 * eventually change xmt_leap below, but never change LEAP_NOTINSYNC.
	 */
	if (xmt_leap != LEAP_NOTINSYNC) {
		if (leap_sec_in_progress) {
			/* always send "not sync" */
			xmt_leap = LEAP_NOTINSYNC;
		}
#ifdef ENABLE_LEAP_SMEAR
		else {
		    /*
		     * If leap smear is enabled in general we must
		     * never send a leap second warning to clients, so
		     * make sure we only send "in sync".
		     */
		    if (leap_smear.enabled)
				xmt_leap = LEAP_NOWARNING;
		}
#endif	/* ENABLE_LEAP_SMEAR */
	}
}

/* Returns false for packets we want to reject out of hand: those with an
   out-of-range version number, an unsupported mode, or too short.
*/
static bool
is_packet_not_low_rot(
	struct recvbuf const* rbufp
	)
{
	return rbufp->recv_length >= 12 &&
	    PKT_VERSION(rbufp->recv_buffer[0]) >= NTP_OLDVERSION &&
	    PKT_VERSION(rbufp->recv_buffer[0]) <= NTP_VERSION &&
	    ( PKT_MODE(rbufp->recv_buffer[0]) == MODE_ACTIVEx ||
	      PKT_MODE(rbufp->recv_buffer[0]) == MODE_CLIENT ||
	      PKT_MODE(rbufp->recv_buffer[0]) == MODE_SERVER ||
              PKT_MODE(rbufp->recv_buffer[0]) == MODE_CONTROL);
}

static bool
is_control_packet(
	struct recvbuf const* rbufp
	)
{
	return rbufp->recv_length >= 1 &&
	    PKT_VERSION(rbufp->recv_buffer[0]) <= NTP_VERSION &&
	    PKT_MODE(rbufp->recv_buffer[0]) == MODE_CONTROL;
}


static bool
parse_packet(
	struct recvbuf * rbufp
	)
{
	REQUIRE(rbufp != NULL);

	size_t recv_length = rbufp->recv_length;
	uint8_t const* recv_buf = rbufp->recv_buffer;

	if(recv_length < LEN_PKT_NOMAC) {
		/* Data is too short to possibly be a valid packet. */
		return false;
	}

	struct parsed_pkt * pkt = &rbufp->pkt;
	uint8_t const* bufptr = recv_buf + LEN_PKT_NOMAC;

	/* Parse header fields */
	pkt->li_vn_mode = recv_buf[0];
	pkt->stratum = recv_buf[1];
	pkt->ppoll = recv_buf[2];
	pkt->precision = (int8_t)recv_buf[3];
	pkt->rootdelay = ntp_be32dec(recv_buf + 4);
	pkt->rootdisp = ntp_be32dec(recv_buf + 8);
	memcpy(pkt->refid, recv_buf + 12, REFIDLEN);
	pkt->reftime = ntp_be64dec(recv_buf + 16);
	pkt->org = ntp_be64dec(recv_buf + 24);
	pkt->rec = ntp_be64dec(recv_buf + 32);
	pkt->xmt = ntp_be64dec(recv_buf + 40);

	rbufp->keyid_present = false;
	rbufp->keyid = 0;
	rbufp->mac_len = 0;

	rbufp->extens_present = false;
	rbufp->ntspacket.valid = false;

	if(PKT_VERSION(pkt->li_vn_mode) > NTP_VERSION) {
		/* Unsupported version */
		return false;
	} else if(PKT_VERSION(pkt->li_vn_mode) == MODE_SERVER) {
		/* Only version 4 packets support extensions. */
		/* But they also support shared key authentication. */
		if (recv_length > (LEN_PKT_NOMAC+MAX_MAC_LEN)) {
			rbufp->extens_present = true;
			return true;
		}
	}

	/* Parse the authenticator */
	switch(recv_buf + recv_length - bufptr) {
	    case 0:
		/* No authenticator */
		rbufp->keyid_present = false;
		rbufp->keyid = 0;
		rbufp->mac_len = 0;
		break;
	    case 4:
		/* crypto-NAK */
		if(PKT_VERSION(pkt->li_vn_mode) < 3) {
			/* Only allowed as of NTPv3 */
			return false;
		}
		rbufp->keyid_present = true;
		rbufp->keyid = ntp_be32dec(bufptr);
		rbufp->mac_len = 0;
		break;
	    case 6:
		/* NTPv2 authenticator, which we allow but strip because
		   we don't support it any more */
		if(PKT_VERSION(pkt->li_vn_mode) != 2) { return false; }
		rbufp->keyid_present = false;
		rbufp->keyid = 0;
		rbufp->mac_len = 0;
		break;
	    case 20:
		/* AES-128 CMAC, MD5 digest */
		if(PKT_VERSION(pkt->li_vn_mode) < 3) {
			/* Only allowed as of NTPv3 */
			return false;
		}
		rbufp->keyid_present = true;
		rbufp->keyid = ntp_be32dec(bufptr);
		rbufp->mac_len = 16;
		break;
	    case 24:
		/* SHA-1 digest */
		if(PKT_VERSION(pkt->li_vn_mode) < 3) {
			/* Only allowed as of NTPv3 */
			return false;
		}
		rbufp->keyid_present = true;
		rbufp->keyid = ntp_be32dec(bufptr);
		rbufp->mac_len = 20;
		break;
	    case 72:
		/* MS-SNTP */
		if(PKT_VERSION(pkt->li_vn_mode) != 3) {
			/* Only allowed for NTPv3 */
			return false;
		}

		/* We don't deal with the MS-SNTP fields, so just strip
		 * them.
		 */
		rbufp->keyid_present = false;
		rbufp->keyid = 0;
		rbufp->mac_len = 0;

		break;
	    default:
		/* Any other length is illegal */
		return false;
	}

	return true;
}

/* Returns true if we should not accept any unauthenticated packets from
   this peer. There are three ways the user can configure this requirement:

   1. A 'restrict notrust' command applies to the peer's IP, or,
   2. We've configured the peer with a 'key' option,
   3. The packet wants to create a new associations, and a 'restrict
      nopeer' command applies to the peer's IP.

   The 'peer' argument may be NULL to indicate that we have no current
   association.

   In contrast to NTP classic, we ignore 'restrict nopeer'. It used
   to apply to symmetric mode associations, which we don't support, and
   to pool-mode associations, which now poke holes into any restrictions.

*/
static bool
i_require_authentication(
	struct peer const* peer,
	unsigned short restrict_mask
	)
{
        bool restrict_notrust = restrict_mask & RES_NOTRUST;
        bool peer_has_key = peer != NULL && peer->cfg.peerkey != 0;

        return restrict_notrust || peer_has_key;
}

static bool
is_crypto_nak(
	struct recvbuf const* rbufp
	)
{
	return rbufp->keyid_present && rbufp->keyid == 0 && rbufp->mac_len == 0;
}

static bool is_kod(
	struct parsed_pkt *pkt
	)
{
	return PKT_LEAP(pkt->li_vn_mode) == LEAP_NOTINSYNC &&
	    PKT_TO_STRATUM(pkt->stratum) == STRATUM_UNSPEC;
}

/* Check the restrictions which can be checked just based on the source
   IP address and the first byte of the packet, namely RES_IGNORE,
   RES_FLAKE, RES_NOQUERY, RES_NOSERVE, and RES_VERSION. */

/* return true to reject packet */
static bool check_early_restrictions(
	struct recvbuf const* rbufp,
	unsigned short restrict_mask
	)
{
	if (rbufp->recv_length < 1)      // FIXME: should be earlier and bigger
	  return(true);
	int mode = PKT_MODE(rbufp->recv_buffer[0]);
	return (
	  (restrict_mask & RES_IGNORE) ||
	  ((restrict_mask & RES_FLAKE) && (double)ntp_random() / UINT32_MAX < .1) ||
	  ((restrict_mask & RES_NOQUERY) && (MODE_CONTROL == mode)) ||
	  ((restrict_mask & RES_NOSERVE) && (MODE_CONTROL != mode)) ||
	  ((restrict_mask & RES_VERSION) &&
	      (PKT_VERSION(rbufp->recv_buffer[0]) != NTP_VERSION))
	  );
}

/* rawstats_filter
 * Don't print all rejectioned packets or we could get DoSed.
 * Print the packet we use.
 * Print the first rejection.
 *   In particular, we get to see why a response is rejected.
 *   Took-too-long, BOGON14 is common.
 *   This lets us see how long it did take.
 */
static void rawstats_filter(
  struct peer *peer,
  struct recvbuf *rbufp,
  unsigned int flag,
  unsigned int outcount) {
	peer->flash |= flag;
	peer->bogons += 1;
	if (peer->bogons > 1) {
		/* only print one bogon */
		return;
	}
	record_raw_stats(peer, rbufp, flag, outcount);
#if 0
	record_raw_stats(peer,
			 PKT_LEAP(rbufp->pkt.li_vn_mode),
			 PKT_VERSION(rbufp->pkt.li_vn_mode),
			 PKT_MODE(rbufp->pkt.li_vn_mode),
			 PKT_TO_STRATUM(rbufp->pkt.stratum),
			 rbufp->pkt.ppoll, rbufp->pkt.precision,
			 rbufp->pkt.rootdelay, rbufp->pkt.rootdisp,
			 /* FIXME: this cast is disgusting */
			 *(const uint32_t*)rbufp->pkt.refid,
			 outcount);
#endif
}

/* Handle MODE_SERVER, replies to our requests.
 * Authentication done upstream.
 */
static void
handle_procpkt(
	struct recvbuf *rbufp,
	struct peer *peer
	)
{
	unsigned int outcount = peer->outcount;

	peer->flash &= ~PKT_BOGON_MASK;

	/* Duplicate detection */
	if(rbufp->pkt.xmt == peer->xmt) {
		rawstats_filter(peer, rbufp, BOGON1, outcount);
		peer->oldpkt++;
		return;
	}
	if(outcount == 0) {
		rawstats_filter(peer, rbufp, BOGON1, outcount);
		peer->oldpkt++;
		return;
	}

	/* Origin timestamp validation */
	if(rbufp->pkt.org == 0) {
		rawstats_filter(peer, rbufp, BOGON3, outcount);
		peer->bogusorg++;
		return;
	} else if(rbufp->pkt.org != peer->org_rand) {
		rawstats_filter(peer, rbufp, BOGON2, outcount);
		peer->bogusorg++;
		return;
	}

	/* We've now cryptographically authenticated the packet if
	   required, and we've checked the origin timestamp, so if we've
	   gotten this far we can safely assume the packet is not
	   spoofed and start updating important peer variables.
	*/

	peer->outcount = 0;
	outcount--;

	if(is_kod(&rbufp->pkt)) {
		if(!memcmp(rbufp->pkt.refid, "RATE", REFIDLEN)) {
			peer->selbroken++;
			report_event(PEVNT_RATE, peer, NULL);
			peer->burst = peer->retry = 0;
			peer->throttle = (NTP_SHIFT + 1) * (1 << peer->cfg.minpoll);
			if (rbufp->pkt.ppoll > peer->cfg.minpoll)
			    peer->cfg.minpoll = min(peer->ppoll, 10);
			poll_update(peer, min(rbufp->pkt.ppoll, 10));
		}
		return;
	}

	if (PKT_LEAP(rbufp->pkt.li_vn_mode) == LEAP_NOTINSYNC ||
	    PKT_TO_STRATUM(rbufp->pkt.stratum) < sys_floor ||
	    PKT_TO_STRATUM(rbufp->pkt.stratum) >= sys_ceiling) {
		rawstats_filter(peer, rbufp, BOGON6, outcount);
		return;
	}

	if(scalbn((double)rbufp->pkt.rootdelay/2.0 + (double)rbufp->pkt.rootdisp, -16) >=
	   sys_maxdisp) {
		rawstats_filter(peer, rbufp, BOGON7, outcount);
		return;
	}

        /* Compute theta (peer offset), delta (peer distance), and epsilon
	   (peer dispersion) statistics. The timestamps may be large but
	   the difference between them should be small, so it's important
	   to do the subtraction *before* converting to floating point to
	   avoid loss of precision.
	*/

	const double t34 =
	    (rbufp->pkt.xmt >= rbufp->recv_time) ?
	    scalbn((double)(rbufp->pkt.xmt - rbufp->recv_time), -32) :
	    -scalbn((double)(rbufp->recv_time - rbufp->pkt.xmt), -32);
	const double t21 =
	    (rbufp->pkt.rec >= peer->org_ts) ?
	    scalbn((double)(rbufp->pkt.rec - peer->org_ts), -32) :
	    -scalbn((double)(peer->org_ts - rbufp->pkt.rec), -32);
	const double theta = (t21 + t34) / 2.;
	const double delta = fabs(t21 - t34);
	const double epsilon = LOGTOD(sys_vars.sys_precision) +
	    LOGTOD(peer->precision) +
	    loop_data.clock_phi * delta;

	/* One final test before we touch any variables that could
	   affect the clock: don't accept any packets with a round
	   trip delay longer than sys_maxdist.	Failure to enforce
	   this constraint could allow an on-path attacker, despite
	   authentication, to mess with the clock by adding long
	   artificial delays in one direction but not the other.
	   Anything rejected here should be getting rejected later by
	   clock filtering anyhow, but performing this check early
	   makes the desired security invariant easier to verify.
	*/
	if(delta > sys_maxdist) {
	  rawstats_filter(peer, rbufp, BOGON14, outcount);
	  peer->oldpkt++;
	  return;
	}

	peer->leap = PKT_LEAP(rbufp->pkt.li_vn_mode);
	peer->stratum = min(PKT_TO_STRATUM(rbufp->pkt.stratum), STRATUM_UNSPEC);
	peer->pmode = PKT_MODE(rbufp->pkt.li_vn_mode);
	peer->precision = rbufp->pkt.precision;
	peer->rootdelay = scalbn((double)rbufp->pkt.rootdelay, -16);
	peer->rootdisp = scalbn((double)rbufp->pkt.rootdisp, -16);
	memcpy(&peer->refid, rbufp->pkt.refid, REFIDLEN);
	peer->reftime = rbufp->pkt.reftime;
	peer->rec = rbufp->pkt.rec;
	peer->xmt = rbufp->pkt.xmt;
	peer->dst = rbufp->recv_time;

	/* Record good packet */
	record_raw_stats(peer, rbufp, 0, outcount);

	/* If either burst mode is armed, enable the burst.
	 * Compute the headway for the next packet and delay if
	 * necessary to avoid exceeding the threshold. */
	if (peer->retry > 0) {
		peer->retry = 0;
		if (peer->reach)
			peer->burst = min(1 << (peer->hpoll -
			    peer->cfg.minpoll), NTP_SHIFT) - 1;
		else
			peer->burst = NTP_IBURST - 1;
		if (peer->burst > 0)
			peer->nextdate = current_time;
	}
	poll_update(peer, peer->hpoll);

	 /* If the peer was previously unreachable, raise a trap. In any
	  * case, mark it reachable. */
	if (!peer->reach) {
		report_event(PEVNT_REACH, peer, NULL);
		peer->timereachable = current_time;
	}
	peer->reach |= 1;

	/* Hooray! Pass our new sample off to the clock filter. */
	clock_filter(peer, theta + peer->cfg.bias, delta, epsilon);
}


void
receive(
	struct recvbuf *rbufp
	)
{
	struct peer *peer = NULL;
	unsigned short restrict_mask;
	auth_info* auth = NULL;  /* !NULL if authenticated */
	int mode, xmode;

#ifdef ENABLE_MSSNTP
	uint8_t zero_key[MSSNTP_QUERY_MAC_LEN];
	memset(&zero_key, 0, MSSNTP_QUERY_MAC_LEN);
#endif /* ENABLE_MSSNTP */

	stat_proto_total.sys_received++;

#ifdef NTPv1
	/*Hack for NTPv1.  See #707 */
	if ((NTPv1 == PKT_VERSION(rbufp->recv_buffer[0])) && \
		(48 == rbufp->recv_length)) {
	  mode = PKT_MODE(rbufp->recv_buffer[0]);
	  stat_proto_total.sys_version1++;
	  /* There is a lot of crufty old NTPv1 SNTP traffic.
	   * NTPv1 has no mode field. */
	  switch (mode) {
	    case MODE_CLIENT:
		/* Some packets come with MODE_CLIENT. They will just work. */
		stat_proto_total.sys_version1client++;
		break;
	    case MODE_UNSPEC:
		/* Some packets come with 0. Patch them.
		 *   This assumes the client doesn't check the returned mode. */
		rbufp->recv_buffer[0] = PKT_LI_VN_MODE(0, NTPv1, MODE_CLIENT);
		stat_proto_total.sys_version1zero++;
		break;
	    case MODE_ACTIVEx:
		/* Windows XP and Server 2003 send MODE_ACTIVE
		 *   https://kb.meinbergglobal.com/kb/time_sync/timekeeping_on_windows/configuring_w32time_as_ntp_client
		 * Again, this assumes the client doesn't check the returned mode. */
		rbufp->recv_buffer[0] = PKT_LI_VN_MODE(0, NTPv1, MODE_CLIENT);
		stat_proto_total.sys_version1symm++;
		break;
	    default:
		break;
	  }
	}
#endif
	if(!is_packet_not_low_rot(rbufp)) {
		stat_proto_total.sys_badlength++;
		return;
	}

	/* FIXME: This is lots more cleanup to do in this area. */

	restrict_mask = restrictions(&rbufp->recv_srcadr);

	if(check_early_restrictions(rbufp, restrict_mask)) {
		stat_proto_total.sys_restricted++;
		return;
	}

	restrict_mask = ntp_monitor(rbufp, restrict_mask);
	if (restrict_mask & RES_LIMITED) {
		stat_proto_total.sys_limitrejected++;
		if(!(restrict_mask & RES_KOD)) { return; }
	}

	if(is_control_packet(rbufp)) {
		process_control(rbufp, restrict_mask);
		stat_proto_total.sys_processed++;
		return;
	}

	/*
	 * Version check must be after the query packets, since they
	 * intentionally use an early version.
	 */
	{
	uint8_t hisversion = PKT_VERSION(rbufp->recv_buffer[0]);
	if (hisversion == NTP_VERSION) {
		stat_proto_total.sys_newversion++;		/* new version */
	} else if (!(restrict_mask & RES_VERSION) && hisversion >=
	    NTP_OLDVERSION) {
		stat_proto_total.sys_oldversion++;		/* previous version */
	} else {
		stat_proto_total.sys_badlength++;
		return;			/* old version */
	}
	}

	if (!parse_packet(rbufp)) {
		stat_proto_total.sys_badlength++;
		return;
	}

	mode = PKT_MODE(rbufp->pkt.li_vn_mode);
	if (MODE_SERVER == mode) {
	    /* Reply to our request:
	     * Auth check breaks if we findpeer for MODE_CLIENT and
	     * a site we are using as a server uses us as a server
	     * with a different key. */
	    peer = findpeer(rbufp);
	    if (NULL == peer) {
		stat_proto_total.sys_declined++;
		return;
	    }
	}

#ifdef ENABLE_MSSNTP
	// If in an MS-SNTP restrict range, with a 16-byte all zero authenticator,
	if((RES_MSSNTP == (restrict_mask & RES_MSSNTP))
	    &&(MSSNTP_QUERY_MAC_LEN == rbufp->mac_len)
	    &&(0 == memcmp(&(rbufp->recv_buffer[LEN_PKT_NOMAC + 4]),
		                 zero_key, MSSNTP_QUERY_MAC_LEN))
	) {
	    // switch off the check that was breaking MS-SNTP;
	    rbufp->keyid_present = false;
	} else {
	    // otherwise, switch off MS-SNTP to not break all other time service.
	    restrict_mask &= ~RES_MSSNTP;
	}
#endif /* ENABLE_MSSNTP */

	if(i_require_authentication(peer, restrict_mask) ||
	    /* He wants authentication */
	    rbufp->keyid_present) {
		auth = authlookup(rbufp->keyid, true);
		if (0) msyslog(LOG_INFO, "DEBUG: receive: key %u %s%s, length %d, %s",
		    rbufp->keyid,
		    (NULL == auth)? "N" : "A",
		    (NULL == peer)? "N" : "P",
		    rbufp->mac_len, socktoa(&rbufp->recv_srcadr) );
		// FIXME: crypto-NAK?
		if(
			/* Check whether an authenticator is even present. */
			!rbufp->keyid_present || is_crypto_nak(rbufp) ||
			/* If we require a specific key from this peer,
			   check that it matches. */
			(peer != NULL && peer->cfg.peerkey != 0 &&
			 peer->cfg.peerkey != rbufp->keyid) ||
			(auth == NULL) ||
			/* Verify the MAC.
			   TODO: rewrite authdecrypt() to give it a
			   better name and a saner interface so we don't
			   have to do this screwy buffer-length
			   arithmetic in order to call it. */
			!authdecrypt(auth,
				 (uint32_t*)rbufp->recv_buffer,
				 (int)(rbufp->recv_length - (rbufp->mac_len + 4)),
				 (int)(rbufp->mac_len + 4))) {

			stat_proto_total.sys_badauth++;
			if(peer != NULL) {
				peer->badauth++;
				peer->cfg.flags &= ~FLAG_AUTHENTIC;
				peer->flash |= BOGON5;
			}
			return;
		}
	}

	switch (mode) {
	    case MODE_ACTIVEx: /* remote site using "peer" in config file */
		/* FALLTHROUGH */
	    case MODE_CLIENT:  /* Request for us as a server. */
		if (rbufp->extens_present
#ifndef DISABLE_NTS
		    && !extens_server_recv(&rbufp->ntspacket,
			  rbufp->recv_buffer, rbufp->recv_length)
#endif
) {
			stat_proto_total.sys_declined++;
			maybe_log_junk("EX-REQ", rbufp);
			break;
		}
		xmode = (mode == MODE_ACTIVEx) ? MODE_PASSIVEx : MODE_SERVER;
		fast_xmit(rbufp, xmode, auth, restrict_mask);
		stat_proto_total.sys_processed++;
		break;
	    case MODE_SERVER:  /* Reply to our request to a server. */
/* FIXME: Where is the shared key case tested? */
		if ((peer->cfg.flags & FLAG_NTS)
		     && (!rbufp->extens_present
#ifndef DISABLE_NTS
 || !extens_client_recv(peer,
		          rbufp->recv_buffer, rbufp->recv_length)
#endif
)) {
		    stat_proto_total.sys_declined++;
		    maybe_log_junk("EX-REP", rbufp);
		    break;
		}
		peer->received++;
		peer->cfg.flags |= FLAG_AUTHENTIC;
		peer->timereceived = current_time;
		handle_procpkt(rbufp, peer);
		stat_proto_total.sys_processed++;
		peer->processed++;
		break;
	    default:
		/* Everything else is for broadcast modes,
		   which are a security nightmare.  So they go to the
		   bit bucket until this improves.
		*/
		stat_proto_total.sys_declined++;
		break;
	}

}

/*
 * transmit - transmit procedure called by poll timeout
 */
void
transmit(
	struct peer *peer	/* peer structure pointer */
	)
{
	uint8_t	hpoll;

	/*
	 * The polling state machine. There are two kinds of machines,
	 * those that never expect a reply (broadcast and manycast
	 * server modes) and those that do (all other modes). The dance
	 * is intricate...
	 */
	hpoll = peer->hpoll;

	/*
	 * Pool associations transmit unicast solicitations when there
	 * are less than a hard limit of 2 * sys_maxclock associations,
	 * and either less than sys_minclock survivors or less than
	 * sys_maxclock associations.  The hard limit prevents unbounded
	 * growth in associations if the system clock or network quality
	 * result in survivor count dipping below sys_minclock often.
	 * This was observed testing with pool, where sys_maxclock == 12
	 * resulted in 60 associations without the hard limit.
	 */
	if (peer->cast_flags & MDF_POOL) {
		peer->outdate = current_time;
		if ((peer_associations <= 2 * sys_maxclock) &&
		    (peer_associations < sys_maxclock ||
		     sys_survivors < sys_minclock))
			if (!dns_probe(peer)) {
			    /* DNS thread busy, try again soon */
			    peer->nextdate = current_time;
			    return;
                     }
		poll_update(peer, hpoll);
		return;
	}

	/* Does server need DNS or NTS lookup? */
	if (peer->cfg.flags & FLAG_LOOKUP) {
		peer->outdate = current_time;
		if (!dns_probe(peer)) {
			peer->nextdate = current_time;
			return;
		}
		poll_update(peer, hpoll);
		return;
        }

	/*
	 * In unicast modes the dance is much more intricate. It is
	 * designed to back off whenever possible to minimize network
	 * traffic.
	 */
	if (peer->burst == 0) {
		uint8_t oreach;

		/*
		 * Update the reachability status. If not heard for
		 * three consecutive polls, stuff infinity in the clock
		 * filter.
		 */
		oreach = peer->reach;
		peer->outdate = current_time;
		peer->unreach++;
		peer->reach <<= 1;
		if (!peer->reach) {

			/*
			 * Here the peer is unreachable. If it was
			 * previously reachable raise a trap. Send a
			 * burst if enabled.
			 */
			clock_filter(peer, 0., 0., sys_maxdisp);
			if (oreach) {
				peer_unfit(peer);
				report_event(PEVNT_UNREACH, peer, NULL);
			}
			if ((peer->cfg.flags & FLAG_IBURST) &&
			    peer->retry == 0)
				peer->retry = NTP_RETRY;
		} else {

			/*
			 * Here the peer is reachable. Send a burst if
			 * enabled and the peer is fit.	 Reset unreach
			 * for persistent and ephemeral associations.
			 * Unreach is also reset for survivors in
			 * clock_select().
			 */
			hpoll = clkstate.sys_poll;
			if (!(peer->cfg.flags & FLAG_PREEMPT))
				peer->unreach = 0;
			if ((peer->cfg.flags & FLAG_BURST) && peer->retry ==
			    0 && !peer_unfit(peer))
				peer->retry = NTP_RETRY;
		}

		/*
		 * Watch for timeout.  If ephemeral, toss the rascal;
		 * otherwise, bump the poll interval. Note the
		 * poll_update() routine will clamp it to maxpoll.
		 * If preemptible and we have more peers than maxclock,
		 * and this peer has the minimum score of preemptibles,
		 * demobilize.
		 */
		if (peer->unreach >= NTP_UNREACH) {
			hpoll++;
			/* ephemeral: no FLAG_CONFIG nor FLAG_PREEMPT */
			if (!(peer->cfg.flags & (FLAG_CONFIG | FLAG_PREEMPT))) {
				report_event(PEVNT_RESTART, peer, "timeout");
				peer_clear(peer, "TIME", false);
				unpeer(peer);
				return;
			}
			if ((peer->cfg.flags & FLAG_PREEMPT) &&
			    (peer_associations > sys_maxclock) &&
			    score_all(peer)) {
				report_event(PEVNT_RESTART, peer, "timeout");
				peer_clear(peer, "TIME", false);
				unpeer(peer);
				return;
			}
		}
	} else {
		peer->burst--;
		if (peer->burst == 0) {

			/*
			 * If ntpdate mode and the clock has not been
			 * set and all peers have completed the burst,
			 * we declare a successful failure.
			 */
			if (clock_ctl.mode_ntpdate) {
				peer_ntpdate--;
				if (peer_ntpdate == 0) {
					msyslog(LOG_NOTICE,
					    "SYNC: no servers found");
					if (!termlogit)
						printf(
						    "SYNC: no servers found\n");
					exit(0);
				}
			}
		}
	}
	if (peer->retry > 0)
		peer->retry--;

	peer_xmit(peer);
	poll_update(peer, hpoll);
}

/*
 * clock_update - Called at system process update intervals.
 */
static void
clock_update(
	struct peer *peer	/* peer structure pointer */
	)
{
	double	dtemp;
	time_t	now;
#ifdef HAVE_LIBSCF_H
	char	*fmri;
#endif /* HAVE_LIBSCF_H */

	/*
	 * Update the system state variables. We do this very carefully,
	 * as the poll interval might need to be clamped differently.
	 */
	sys_vars.sys_peer = peer;
	sys_epoch = peer->epoch;
	if (clkstate.sys_poll < peer->cfg.minpoll)
		clkstate.sys_poll = peer->cfg.minpoll;
	if (clkstate.sys_poll > peer->cfg.maxpoll)
		clkstate.sys_poll = peer->cfg.maxpoll;
	poll_update(peer, clkstate.sys_poll);
	sys_vars.sys_stratum = min(peer->stratum + 1, STRATUM_UNSPEC);
	if (peer->stratum == STRATUM_REFCLOCK ||
	    peer->stratum == STRATUM_UNSPEC)
		sys_vars.sys_refid = peer->refid;
	else
		sys_vars.sys_refid = addr2refid(&peer->srcadr);
	/*
	 * Root Dispersion (E) is defined (in RFC 5905) as:
	 *
	 * E = p.epsilon_r + p.epsilon + p.psi + PHI*(s.t - p.t) + |THETA|
	 *
	 * where:
	 *  p.epsilon_r is the PollProc's root dispersion
	 *  p.epsilon	is the PollProc's dispersion
	 *  p.psi	is the PollProc's jitter
	 *  THETA	is the combined offset
	 *
	 * NB: Think Hard about where these numbers come from and
	 * what they mean.  When did peer->update happen?  Has anything
	 * interesting happened since then?  What values are the most
	 * defensible?	Why?
	 *
	 * DLM thinks this equation is probably the best of all worse choices.
	 */
	dtemp	= peer->rootdisp
		+ peer->disp
		+ clkstate.sys_jitter
		+ loop_data.clock_phi * (current_time - peer->update)
		+ fabs(clkstate.sys_offset);

	if (dtemp > sys_mindist)
		sys_vars.sys_rootdisp = dtemp;
	else
		sys_vars.sys_rootdisp = sys_mindist;
	sys_vars.sys_rootdelay = peer->delay + peer->rootdelay;
	sys_vars.sys_reftime = peer->dst;

	DPRINT(1, ("clock_update: at %u sample %u associd %d\n",
		   current_time, peer->epoch, peer->associd));

	/*
	 * Comes now the moment of truth. Crank the clock discipline and
	 * see what comes out.
	 */
	switch (local_clock(peer, clkstate.sys_offset)) {

	/*
	 * Clock exceeds panic threshold. Life as we know it ends.
	 */
	case -1:
#ifdef HAVE_LIBSCF_H
		/*
		 * For Solaris enter the maintenance mode.
		 */
		if ((fmri = getenv("SMF_FMRI")) != NULL) {
			if (smf_maintain_instance(fmri, 0) < 0) {
				printf("smf_maintain_instance: %s\n",
				    scf_strerror(scf_error()));
				exit(1);
			}
			/*
			 * Sleep until SMF kills us.
			 */
			for (;;)
				pause();
		}
#endif /* HAVE_LIBSCF_H */
		msyslog(LOG_ERR, "CLOCK: Panic: offset too big: %.3f",
			clkstate.sys_offset);
		exit (1);
		/* not reached */

	/*
	 * Clock was stepped. Flush all time values of all peers.
	 */
	case 2:
		clear_all();
		set_sys_leap(LEAP_NOTINSYNC);
		sys_vars.sys_stratum = STRATUM_UNSPEC;
		memcpy(&sys_vars.sys_refid, "STEP", REFIDLEN);
		sys_vars.sys_rootdelay = 0;
		sys_vars.sys_rootdisp = 0;
		sys_vars.sys_reftime = 0;
		clkstate.sys_jitter = LOGTOD(sys_vars.sys_precision);
		leapsec_reset_frame();
		break;

	/*
	 * Clock was slewed. Handle the leapsecond stuff.
	 */
	case 1:

		/*
		 * If this is the first time the clock is set, reset the
		 * leap bits. If crypto, the timer will goose the setup
		 * process.
		 */
		if (sys_vars.sys_leap == LEAP_NOTINSYNC) {
			set_sys_leap(LEAP_NOWARNING);
			/*
			 * If our parent process is waiting for the
			 * first clock sync, send them home satisfied.
			 */
			if (waitsync_fd_to_close != -1) {
				close(waitsync_fd_to_close);
				waitsync_fd_to_close = -1;
				DPRINT(1, ("notified parent --wait-sync is done\n"));
			}

		}

		/*
		 * If there is no leap second pending and the number of
		 * survivor leap bits is greater than half the number of
		 * survivors, try to schedule a leap for the end of the
		 * current month. (This only works if no leap second for
		 * that range is in the table, so doing this more than
		 * once is mostly harmless.)
		 */
		if (leapsec == LSPROX_NOWARN) {
			if (leap_vote_ins > leap_vote_del
			    && leap_vote_ins > sys_survivors / 2) {
				time(&now);
				leapsec_add_dyn(true, now);
			}
			if (leap_vote_del > leap_vote_ins
			    && leap_vote_del > sys_survivors / 2) {
				time(&now);
				leapsec_add_dyn(false, now);
			}
		}
		break;

	/*
	 * Popcorn spike or step threshold exceeded. Pretend it never
	 * happened.
	 */
	default:
		break;
	}
}


/*
 * poll_update - update peer poll interval
 */
void
poll_update(
	struct peer *peer,	/* peer structure pointer */
	uint8_t	mpoll
	)
{
	uptime_t	next, utemp;
	uint8_t	hpoll;

	/*
	 * This routine figures out when the next poll should be sent.
	 * That turns out to be wickedly complicated. One problem is
	 * that sometimes the time for the next poll is in the past when
	 * the poll interval is reduced. We watch out for races here
	 * between the receive process and the poll process.
	 *
	 * Clamp the poll interval between minpoll and maxpoll.
	 */
	hpoll = max(min(peer->cfg.maxpoll, mpoll), peer->cfg.minpoll);

	peer->hpoll = hpoll;

	/*
	 * There are three variables important for poll scheduling, the
	 * current time (current_time), next scheduled time (nextdate)
	 * and the earliest time (utemp). The earliest time is 2 s
	 * seconds, but could be more due to rate management. When
	 * sending in a burst, use the earliest time. When not in a
	 * burst but with a reply pending, send at the earliest time
	 * unless the next scheduled time has not advanced. This can
	 * only happen if multiple replies are pending in the same
	 * response interval. Otherwise, send at the later of the next
	 * scheduled time and the earliest time.
	 *
	 * Now we figure out if there is an override. If a burst is in
	 * progress and we get called from the receive process, just
	 * slink away. If called from the poll process, delay 1 s for a
	 * reference clock, otherwise 2 s.
	 */
	utemp = current_time + (unsigned long)max(peer->throttle - (NTP_SHIFT - 1) *
	    (1 << peer->cfg.minpoll), rstrct.ntp_minpkt);
	if (peer->burst > 0) {
		if (peer->nextdate > current_time)
			return;
#ifdef REFCLOCK
		else if (peer->cfg.flags & FLAG_REFCLOCK)
			peer->nextdate = current_time + RESP_DELAY;
#endif /* REFCLOCK */
		else
			peer->nextdate = utemp;

	/*
	 * The ordinary case. If a retry, use minpoll; if unreachable,
	 * use host poll; otherwise, use the minimum of host and peer
	 * polls; In other words, oversampling is okay but
	 * understampling is evil. Use the maximum of this value and the
	 * headway. If the average headway is greater than the headway
	 * threshold, increase the headway by the minimum interval.
	 */
	} else {
		if (peer->retry > 0)
			hpoll = peer->cfg.minpoll;
		else if (!(peer->reach))
			hpoll = peer->hpoll;
		else
			hpoll = min(peer->ppoll, peer->hpoll);
#ifdef REFCLOCK
		if (peer->cfg.flags & FLAG_REFCLOCK)
			next = 1U << hpoll;
		else
#endif /* REFCLOCK */
			/* add a bit of randomess to next polling time
			 * to disperse traffic */
			next = ((0x1000UL | (ntp_random() & 0x0ff)) <<
			    hpoll) >> 12;
		next += peer->outdate;
		if (next > utemp)
			peer->nextdate = next;
		else
			peer->nextdate = utemp;
		if (peer->throttle > (1 << peer->cfg.minpoll))
			peer->nextdate += (unsigned long)rstrct.ntp_minpkt;
	}
	DPRINT(2, ("poll_update: at %u %s poll %d burst %d retry %d head %d early %u next %u\n",
		   current_time, socktoa(&peer->srcadr), peer->hpoll,
		   peer->burst, peer->retry, peer->throttle,
		   utemp - current_time, peer->nextdate -
		   current_time));
}


/*
 * peer_clear - clear peer filter registers.  See Section 3.4.8 of the
 * spec.
 */
void
peer_clear(
	struct peer *peer,		/* peer structure */
	const char *ident,		/* tally lights */
	const bool initializing1
	)
{
	uint8_t	u;

	/*
	 * Clear all values, including the optional crypto values above.
	 */
	memset(CLEAR_TO_ZERO(peer), 0, LEN_CLEAR_TO_ZERO(peer));
	peer->ppoll = NTP_MAXPOLL_UNK;
	peer->hpoll = peer->cfg.minpoll;
	peer->disp = sys_maxdisp;
	peer->flash = peer_unfit(peer);
	peer->jitter = LOGTOD(sys_vars.sys_precision);

	for (u = 0; u < NTP_SHIFT; u++) {
		peer->filter_order[u] = u;
		peer->filter_disp[u] = sys_maxdisp;
	}
#ifdef REFCLOCK
	if (!(peer->cfg.flags & FLAG_REFCLOCK)) {
#endif
		peer->leap = LEAP_NOTINSYNC;
		peer->stratum = STRATUM_UNSPEC;
		memcpy(&peer->refid, ident, REFIDLEN);
#ifdef REFCLOCK
	}
#endif

	/*
	 * During initialization use the association count to spread out
	 * the polls at one-second intervals. Passive associations'
	 * first poll is delayed by the "discard minimum" to avoid rate
	 * limiting. Other post-startup new or cleared associations
	 * randomize the first poll over the minimum poll interval to
	 * avoid implosion.
	 */
	peer->nextdate = peer->update = peer->outdate = current_time;
	if (initializing1) {
		peer->nextdate += (unsigned long)peer_associations;
	} else {
	    /*
	     * Randomizing the next poll interval used to be done with
	     * ntp_random(); any deterministic but uniformly
	     * distributed function of the peer state would be good
	     * enough.	Furthermore, changing the function creates no
	     * interop problems. For security reasons (to prevent
	     * hypothetical timing attacks) we want at least one input
	     * to be invisible from outside ntpd; the internal
	     * association ID fits the bill.
	     */
	    unsigned int pseudorand = peer->associd ^ sock_hash(&peer->srcadr);
	    peer->nextdate += (pseudorand % (1 << peer->cfg.minpoll));
	}
	DPRINT(1, ("peer_clear: at %u next %u associd %d refid %s\n",
		   current_time, peer->nextdate, peer->associd,
		   ident));
}


/*
 * clock_filter - add incoming clock sample to filter register and run
 *		  the filter procedure to find the best sample.
 */
void
clock_filter(
	struct peer *peer,		/* peer structure pointer */
	double	sample_offset,		/* clock offset */
	double	sample_delay,		/* roundtrip delay */
	double	sample_disp		/* dispersion */
	)
{
	double	dst[NTP_SHIFT];		/* distance vector */
	int	ord[NTP_SHIFT];		/* index vector */
	int	i, j, k, m;
	double	dtemp, etemp, jtemp;
	char	tbuf[80];

	/*
	 * A sample consists of the offset, delay, dispersion and epoch
	 * of arrival. The offset and delay are determined by the on-
	 * wire protocol. The dispersion grows from the last outbound
	 * packet to the arrival of this one increased by the sum of the
	 * peer precision and the system precision as required by the
	 * error budget. First, shift the new arrival into the shift
	 * register discarding the oldest one.
	 */
	j = peer->filter_nextpt;
	peer->filter_offset[j] = sample_offset;
	peer->filter_delay[j] = sample_delay;
	peer->filter_disp[j] = sample_disp;
	peer->filter_epoch[j] = current_time;
	j = (j + 1) % NTP_SHIFT;
	peer->filter_nextpt = j;

	/*
	 * Update dispersions since the last update and at the same
	 * time initialize the distance and index lists. Since samples
	 * become increasingly uncorrelated beyond the Allan intercept,
	 * only under exceptional cases will an older sample be used.
	 * Therefore, the distance list uses a compound metric. If the
	 * dispersion is greater than the maximum dispersion, clamp the
	 * distance at that value. If the time since the last update is
	 * less than the Allan intercept use the delay; otherwise, use
	 * the sum of the delay and dispersion.
	 */
	dtemp = loop_data.clock_phi * (current_time - peer->update);
	peer->update = current_time;
	for (i = NTP_SHIFT - 1; i >= 0; i--) {
		if (i != 0)
			peer->filter_disp[j] += dtemp;
		if (peer->filter_disp[j] >= sys_maxdisp) {
			peer->filter_disp[j] = sys_maxdisp;
			dst[i] = sys_maxdisp;
		} else if (peer->update - peer->filter_epoch[j] >
		    (unsigned long)ULOGTOD(clkstate.allan_xpt)) {
			dst[i] = peer->filter_delay[j] +
			    peer->filter_disp[j];
		} else {
			dst[i] = peer->filter_delay[j];
		}
		ord[i] = j;
		j = (j + 1) % NTP_SHIFT;
	}

	/*
	 * If the clock has stabilized, sort the samples by distance.
	 */
	if (freq_cnt == 0) {
		for (i = 1; i < NTP_SHIFT; i++) {
			for (j = 0; j < i; j++) {
				if (dst[j] > dst[i]) {
					k = ord[j];
					ord[j] = ord[i];
					ord[i] = k;
					etemp = dst[j];
					dst[j] = dst[i];
					dst[i] = etemp;
				}
			}
		}
	}

	/*
	 * Copy the index list to the association structure so ntpq
	 * can see it later. Prune the distance list to leave only
	 * samples less than the maximum dispersion, which disfavors
	 * uncorrelated samples older than the Allan intercept. To
	 * further improve the jitter estimate, of the remainder leave
	 * only samples less than the maximum distance, but keep at
	 * least two samples for jitter calculation.
	 */
	m = 0;
	for (i = 0; i < NTP_SHIFT; i++) {
		peer->filter_order[i] = (uint8_t) ord[i];
		if (dst[i] >= sys_maxdisp || (m >= 2 && dst[i] >=
		    sys_maxdist))
			continue;
		m++;
	}

	/*
	 * Compute the dispersion and jitter. The dispersion is weighted
	 * exponentially by NTP_FWEIGHT (0.5) so it is normalized close
	 * to 1.0. The jitter is the RMS differences relative to the
	 * lowest delay sample.
	 */
	peer->disp = jtemp = 0;
	k = ord[0];
	for (i = NTP_SHIFT - 1; i >= 0; i--) {
		j = ord[i];
		peer->disp = NTP_FWEIGHT * (peer->disp +
		    peer->filter_disp[j]);
		if (i < m)
			jtemp += DIFF(peer->filter_offset[j],
			    peer->filter_offset[k]);
	}

	/*
	 * If no acceptable samples remain in the shift register,
	 * quietly tiptoe home leaving only the dispersion. Otherwise,
	 * save the offset, delay and jitter. Note the jitter must not
	 * be less than the precision.
	 */
	if (m == 0) {
		clock_select();
		return;
	}
	etemp = fabs(peer->offset - peer->filter_offset[k]);
	peer->offset = peer->filter_offset[k];
	peer->delay = peer->filter_delay[k];
	peer->jitter = jtemp;
	if (m > 1)
		peer->jitter /= m - 1;
	peer->jitter = SQRT(peer->jitter);

	/*
	 * If the new sample and the current sample are both valid
	 * and the difference between their offsets exceeds CLOCK_SGATE
	 * (3) times the jitter and the interval between them is less
	 * than twice the host poll interval, consider the new sample
	 * a popcorn spike and ignore it.
	 */
	if (peer->disp < sys_maxdist && peer->filter_disp[k] <
	    sys_maxdist && etemp > CLOCK_SGATE * peer->jitter &&
	    peer->filter_epoch[k] - peer->epoch < 2. *
	    ULOGTOD(peer->hpoll)) {
		snprintf(tbuf, sizeof(tbuf), "%.6f s", etemp);
		report_event(PEVNT_POPCORN, peer, tbuf);
		return;
	}

	/*
	 * A new minimum sample is useful only if it is later than the
	 * last one used. In this design the maximum lifetime of any
	 * sample is not greater than eight times the poll interval, so
	 * the maximum interval between minimum samples is eight
	 * packets.
	 */
	if (peer->filter_epoch[k] <= peer->epoch) {
	  DPRINT(2, ("clock_filter: old sample %u\n", current_time -
		     peer->filter_epoch[k]));
		return;
	}
	peer->epoch = peer->filter_epoch[k];

	/*
	 * The mitigated sample statistics are saved for later
	 * processing. If not synchronized or not in a burst, tickle the
	 * clock select algorithm.
	 */
	record_peer_stats(peer, ctlpeerstatus(peer));
	DPRINT(1, ("clock_filter: n %d off %.6f del %.6f dsp %.6f jit %.6f\n",
		   m, peer->offset, peer->delay, peer->disp,
		   peer->jitter));
	if (peer->burst == 0 || sys_vars.sys_leap == LEAP_NOTINSYNC)
		clock_select();
}


/*
 * clock_select - find the pick-of-the-litter clock
 *
 * When lockclock is on: (1) If the local clock is the prefer peer, it
 * will always be enabled, even if declared falseticker, (2) only the
 * prefer peer can be selected as the system peer, (3) if the external
 * source is down, the system leap bits are set to 11 and the stratum
 * set to infinity.
 */
void
clock_select(void)
{
	struct peer *peer;
	int	i, j, k, n;
	int	nlist, nl2;
	int	allow;
	int	speer;
	double	d, e, f, g;
	double	high, low;
	double	speermet;
	double	orphmet = 2.0 * UINT32_MAX; /* 2x is greater than */
	struct endpoint endp;
	struct peer *osys_peer;
	struct peer *sys_prefer = NULL;	/* prefer peer */
	struct peer *typesystem = NULL;
	struct peer *typeorphan = NULL;
#ifdef REFCLOCK
	struct peer *typemodem = NULL;
	struct peer *typelocal = NULL;
	struct peer *typepps = NULL;
#endif /* REFCLOCK */
	static struct endpoint *endpoint = NULL;
	static int *indx = NULL;
	static peer_select *peers = NULL;
	static unsigned int endpoint_size = 0;
	static unsigned int peers_size = 0;
	static unsigned int indx_size = 0;
	size_t octets;

	/*
	 * Initialize and create endpoint, index and peer lists big
	 * enough to handle all associations.
	 */
	osys_peer = sys_vars.sys_peer;
	sys_survivors = 0;
	if (loop_data.lockclock) {
		set_sys_leap(LEAP_NOTINSYNC);
		sys_vars.sys_stratum = STRATUM_UNSPEC;
		memcpy(&sys_vars.sys_refid, "DOWN", REFIDLEN);
	}

	/*
	 * Allocate dynamic space depending on the number of
	 * associations.
	 */
	nlist = 1;
	for (peer = peer_list; peer != NULL; peer = peer->p_link) {
		nlist++;
	}
	endpoint_size = ALIGNED_SIZE((unsigned int)nlist * 2 * sizeof(*endpoint));
	peers_size = ALIGNED_SIZE((unsigned int)nlist * sizeof(*peers));
	indx_size = ALIGNED_SIZE((unsigned int)nlist * 2 * sizeof(*indx));
	octets = endpoint_size + peers_size + indx_size;
	endpoint = erealloc(endpoint, octets);
	peers = INC_ALIGNED_PTR(endpoint, endpoint_size);
	indx = INC_ALIGNED_PTR(peers, peers_size);

	/*
	 * Initially, we populate the island with all the rifraff peers
	 * that happen to be lying around. Those with seriously
	 * defective clocks are immediately booted off the island. Then,
	 * the falsetickers are culled and put to sea. The truechimers
	 * remaining are subject to repeated rounds where the most
	 * unpopular at each round is kicked off. When the population
	 * has dwindled to sys_minclock, the survivors split a million
	 * bucks and collectively crank the chimes.
	 */
	nlist = nl2 = 0;	/* none yet */
	for (peer = peer_list; peer != NULL; peer = peer->p_link) {
		peer->new_status = CTL_PST_SEL_REJECT;

		/*
		 * Leave the island immediately if the peer is
		 * unfit to synchronize.
		 */
		if ((FLAG_LOOKUP & peer->cfg.flags) ||
                        (MDF_POOL & peer->cast_flags))
			continue;
		if (peer_unfit(peer)) {
			continue;
}

		/*
		 * If this peer is an orphan parent, elect the
		 * one with the lowest metric defined as the
		 * IPv4 address or the first 64 bits of the
		 * hashed IPv6 address.	 To ensure convergence
		 * on the same selected orphan, consider as
		 * well that this system may have the lowest
		 * metric and be the orphan parent.  If this
		 * system wins, sys_peer will be NULL to trigger
		 * orphan mode in timer().
		 */
		if (peer->stratum == sys_orphan) {
			uint32_t	localmet;
			uint32_t peermet;

			if (peer->dstadr != NULL)
				localmet = ntohl(peer->dstadr->addr_refid);
			else
				localmet = UINT32_MAX;
			peermet = ntohl(addr2refid(&peer->srcadr));
			if (peermet < localmet && peermet < orphmet) {
				typeorphan = peer;
				orphmet = peermet;
			}
			continue;
		}

		/*
		 * If this peer could have the orphan parent
		 * as a synchronization ancestor, exclude it
		 * from selection to avoid forming a
		 * synchronization loop within the orphan mesh,
		 * triggering stratum climb to infinity
		 * instability.	 Peers at stratum higher than
		 * the orphan stratum could have the orphan
		 * parent in ancestry so are excluded.
		 * See http://bugs.ntp.org/2050
		 */
		if (peer->stratum > sys_orphan)
			continue;
#ifdef REFCLOCK
		/*
		 * This is an attempt to set up fallbacks in case falseticker
		 * elimination leaves no survivors with better service quality.
		 */
		if (!(peer->cfg.flags & FLAG_PREFER)) {
		    if (peer->sstclktype == CTL_SST_TS_LOCAL) {
			if (current_time > orphwait && typelocal == NULL)
				typelocal = peer;
			continue;
		    } else if (peer->sstclktype == CTL_SST_TS_TELEPHONE) {
			if (current_time > orphwait && typemodem == NULL)
				typemodem = peer;
			continue;
		    }
		}
#endif /* REFCLOCK */

		/*
		 * If we get this far, the peer can stay on the
		 * island, but does not yet have the immunity
		 * idol.
		 */
		peer->new_status = CTL_PST_SEL_SANE;
		f = root_distance(peer);
		peers[nlist].peer = peer;
		peers[nlist].error = peer->jitter;
		peers[nlist].synch = f;
		nlist++;

		/*
		 * Insert each interval endpoint on the unsorted
		 * endpoint[] list.
		 */
		e = peer->offset;
		endpoint[nl2].type = -1;	/* lower end */
		endpoint[nl2].val = e - f;
		nl2++;
		endpoint[nl2].type = 1;		/* upper end */
		endpoint[nl2].val = e + f;
		nl2++;
	}
	/*
	 * Construct sorted indx[] of endpoint[] indexes ordered by
	 * offset.
	 */
	for (i = 0; i < nl2; i++) {
		indx[i] = i;
	}
	for (i = 0; i < nl2; i++) {
		endp = endpoint[indx[i]];
		e = endp.val;
		k = i;
		for (j = i + 1; j < nl2; j++) {
			endp = endpoint[indx[j]];
			if (endp.val < e) {
				e = endp.val;
				k = j;
			}
		}
		if (k != i) {
			j = indx[k];
			indx[k] = indx[i];
			indx[i] = j;
		}
	}
	for (i = 0; i < nl2; i++)
		DPRINT(3, ("select: endpoint %2d %.6f\n",
			   endpoint[indx[i]].type, endpoint[indx[i]].val));

	/*
	 * This is the actual algorithm that cleaves the truechimers
	 * from the falsetickers. The original algorithm was described
	 * in Keith Marzullo's dissertation, but has been modified for
	 * better accuracy.
	 *
	 * Briefly put, we first assume there are no falsetickers, then
	 * scan the candidate list first from the low end upwards and
	 * then from the high end downwards. The scans stop when the
	 * number of intersections equals the number of candidates less
	 * the number of falsetickers. If this doesn't happen for a
	 * given number of falsetickers, we bump the number of
	 * falsetickers and try again. If the number of falsetickers
	 * becomes equal to or greater than half the number of
	 * candidates, the Albanians have won the Byzantine wars and
	 * correct synchronization is not possible.
	 *
	 * Here, nlist is the number of candidates and allow is the
	 * number of falsetickers. Upon exit, the truechimers are the
	 * survivors with offsets not less than low and not greater than
	 * high. There may be none of them.
	 */
	low = 1e9;
	high = -1e9;
	for (allow = 0; 2 * allow < nlist; allow++) {

		/*
		 * Bound the interval (low, high) as the smallest
		 * interval containing points from the most sources.
		 */
		n = 0;
		for (i = 0; i < nl2; i++) {
			low = endpoint[indx[i]].val;
			n -= endpoint[indx[i]].type;
			if (n >= nlist - allow) {
				break;
}
		}
		n = 0;
		for (j = nl2 - 1; j >= 0; j--) {
			high = endpoint[indx[j]].val;
			n += endpoint[indx[j]].type;
			if (n >= nlist - allow) {
				break;
}
		}

		/*
		 * If an interval containing truechimers is found, stop.
		 * If not, increase the number of falsetickers and go
		 * around again.
		 */
		if (high > low) {
			break;
}
	}

	/*
	 * Clustering algorithm. Whittle candidate list of falsetickers,
	 * who leave the island immediately. The TRUE peer is always a
	 * truechimer. We must leave at least one peer to collect the
	 * million bucks.
	 *
	 * We assert the correct time is contained in the interval, but
	 * the best offset estimate for the interval might not be
	 * contained in the interval. For this purpose, a truechimer is
	 * defined as the midpoint of an interval that overlaps the
	 * intersection interval.
	 */
	j = 0;
	for (i = 0; i < nlist; i++) {
		double	h;

		peer = peers[i].peer;
		h = peers[i].synch;
		if ((high <= low || peer->offset + h < low ||
		    peer->offset - h > high) && !(peer->cfg.flags & FLAG_TRUE))
			continue;

#ifdef REFCLOCK
		/*
		 * Eligible PPS peers must survive the intersection
		 * algorithm. Use the first one found, but don't
		 * include any of them in the cluster population.
		 */
		if (peer->cfg.flags & FLAG_PPS) {
			if (typepps == NULL)
				typepps = peer;
			if (!(peer->cfg.flags & FLAG_TSTAMP_PPS))
				continue;
		}
#endif /* REFCLOCK */

		if (j != i) {
			peers[j] = peers[i];
		}
		j++;
	}
	nlist = j;

	/*
	 * If no survivors remain at this point, check if we have a modem
	 * driver, local driver or orphan parent in that order. If so,
	 * nominate the first one found as the only survivor.
	 * Otherwise, give up and leave the island to the rats.
	 */
	if (nlist == 0) {
		peers[0].error = 0;
		peers[0].synch = sys_mindist;
#ifdef REFCLOCK
		if (typemodem != NULL) {
			peers[0].peer = typemodem;
			nlist = 1;
		} else if (typelocal != NULL) {
			peers[0].peer = typelocal;
			nlist = 1;
		} else
#endif /* REFCLOCK */
		if (typeorphan != NULL) {
			peers[0].peer = typeorphan;
			nlist = 1;
		}
	}

	/*
	 * Mark the candidates at this point as truechimers.
	 */
	for (i = 0; i < nlist; i++) {
		peers[i].peer->new_status = CTL_PST_SEL_SELCAND;
		DPRINT(2, ("select: survivor %s %f\n",
			   socktoa(&peers[i].peer->srcadr), peers[i].synch));
	}

	/*
	 * Now, vote outliers off the island by select jitter weighted
	 * by root distance. Continue voting as long as there are more
	 * than sys_minclock survivors and the select jitter of the peer
	 * with the worst metric is greater than the minimum peer
	 * jitter. Stop if we are about to discard a TRUE or PREFER
	 * peer, who of course have the immunity idol.
	 */
	while (1) {
		d = 1e9; // Minimum peer jitter
		e = -1e9; // Worst peer select jitter * synch
		g = 0; // Worst peer select jitter
		k = 0; // Index of the worst peer
		for (i = 0; i < nlist; i++) {
			if (peers[i].error < d) {
				d = peers[i].error;
			}
			peers[i].seljit = 0;
			if (nlist > 1) {
				f = 0;
				for (j = 0; j < nlist; j++)
					f += DIFF(peers[j].peer->offset,
					    peers[i].peer->offset);
				peers[i].seljit = SQRT(f / (nlist - 1));
			}
			if (peers[i].seljit * peers[i].synch > e) {
				g = peers[i].seljit;
				e = peers[i].seljit * peers[i].synch;
				k = i;
			}
		}
		if (nlist <= max(1, sys_minclock) || g <= d ||
		    ((FLAG_TRUE | FLAG_PREFER) & peers[k].peer->cfg.flags))
			break;

		DPRINT(3, ("select: drop %s seljit %.6f jit %.6f\n",
			   socktoa(&peers[k].peer->srcadr), g, d));
		if (nlist > sys_maxclock)
			peers[k].peer->new_status = CTL_PST_SEL_EXCESS;
		for (j = k + 1; j < nlist; j++) {
			peers[j - 1] = peers[j];
		}
		nlist--;
	}

	/*
	 * What remains is a list usually not greater than sys_minclock
	 * peers. Note that unsynchronized peers cannot survive this
	 * far.	 Count and mark these survivors.
	 *
	 * While at it, count the number of leap warning bits found.
	 * This will be used later to vote the system leap warning bit.
	 * If a leap warning bit is found on a reference clock, the vote
	 * is always won.
	 *
	 * Choose the system peer using a hybrid metric composed of the
	 * selection jitter scaled by the root distance augmented by
	 * stratum scaled by sys_mindist (.001 by default). The goal of
	 * the small stratum factor is to avoid clockhop between a
	 * reference clock and a network peer which has a refclock and
	 * is using an older ntpd, which does not floor sys_rootdisp at
	 * sys_mindist.
	 *
	 * In contrast, ntpd 4.2.6 and earlier used stratum primarily
	 * in selecting the system peer, using a weight of 1 second of
	 * additional root distance per stratum.  This heavy bias is no
	 * longer appropriate, as the scaled root distance provides a
	 * more rational metric carrying the cumulative error budget.
	 */
	e = 1e9;
	speer = 0;
	leap_vote_ins = 0;
	leap_vote_del = 0;
	for (i = 0; i < nlist; i++) {
		peer = peers[i].peer;
		peer->unreach = 0;
		peer->new_status = CTL_PST_SEL_SYNCCAND;
		sys_survivors++;
		if (peer->leap == LEAP_ADDSECOND) {
			if (peer->cfg.flags & FLAG_REFCLOCK)
				leap_vote_ins = nlist;
			else if (leap_vote_ins < nlist) {
				leap_vote_ins++;
			}
		}
		if (peer->leap == LEAP_DELSECOND) {
			if (peer->cfg.flags & FLAG_REFCLOCK)
				leap_vote_del = nlist;
			else if (leap_vote_del < nlist) {
				leap_vote_del++;
			}
		}
		if (peer->cfg.flags & FLAG_PREFER)
			sys_prefer = peer;
		speermet = peers[i].seljit * peers[i].synch +
		    peer->stratum * sys_mindist;
		if (speermet < e) {
			e = speermet;
			speer = i;
		}
	}

	/*
	 * Unless there are at least sys_minsane survivors, leave the
	 * building dark. Otherwise, do a clockhop dance. Ordinarily,
	 * use the selected survivor speer. However, if the current
	 * system peer is not speer, stay with the current system peer
	 * as long as it doesn't get too old or too ugly.
	 */
	if (nlist > 0 && nlist >= sys_minsane) {
		double	x;

		typesystem = peers[speer].peer;
		if (osys_peer == NULL || osys_peer == typesystem) {
			sys_clockhop = 0;
		} else if ((x = fabs(typesystem->offset -
		    osys_peer->offset)) < sys_mindist) {
			if ( D_ISZERO_NS(sys_clockhop) ) {
				sys_clockhop = sys_mindist;
			} else {
				sys_clockhop *= .5;
			}
			DPRINT(1, ("select: clockhop %d %.6f %.6f\n",
				   j, x, sys_clockhop));
			if (fabs(x) < sys_clockhop) {
				typesystem = osys_peer;
			} else {
				sys_clockhop = 0;
			}
		} else {
			sys_clockhop = 0;
		}
	}

	/*
	 * Mitigation rules of the game. We have the pick of the
	 * litter in typesystem if any survivors are left. If
	 * there is a prefer peer, use its offset and jitter.
	 * Otherwise, use the combined offset and jitter of all kittens.
	 */
	if (typesystem != NULL) {
		if (sys_prefer == NULL) {
			typesystem->new_status = CTL_PST_SEL_SYSPEER;
			clock_combine(peers, sys_survivors, speer);
		} else {
			typesystem = sys_prefer;
			sys_clockhop = 0;
			typesystem->new_status = CTL_PST_SEL_SYSPEER;
			clkstate.sys_offset = typesystem->offset;
			clkstate.sys_jitter = typesystem->jitter;
			sys_vars.sys_rootdist = root_distance(typesystem);
		}
		DPRINT(1, ("select: combine offset %.9f jitter %.9f\n",
			   clkstate.sys_offset, clkstate.sys_jitter));
	}
#ifdef REFCLOCK
	/*
	 * If a PPS driver is lit and the combined offset is less than
	 * 0.4 s, select the driver as the PPS peer and use its offset
	 * and jitter. However, if this is the pps driver, use it only
	 * if there is a prefer peer or there are no survivors and none
	 * are required.
	 */
	if (typepps != NULL && fabs(clkstate.sys_offset) < 0.4 &&
	    (!typepps->is_pps_driver ||
	     sys_prefer != NULL ||
	     (typesystem != NULL && typepps->cfg.flags & FLAG_PREFER) ||
	     (typesystem == NULL && sys_minsane == 0))) {
		typesystem = typepps;
		sys_clockhop = 0;
		typesystem->new_status = CTL_PST_SEL_PPS;
		clkstate.sys_offset = typesystem->offset;
		clkstate.sys_jitter = typesystem->jitter;
		sys_vars.sys_rootdist = root_distance(typesystem);
		DPRINT(1, ("select: pps offset %.9f jitter %.9f\n",
			   clkstate.sys_offset, clkstate.sys_jitter));
	} else if ( typepps &&
                    ( CTL_PST_SEL_PPS == typepps->new_status )) {
                /* uh, oh, it WAS a valid PPS, but no longer */
		typepps->status = CTL_PST_SEL_REJECT;
        }
#endif /* REFCLOCK */

	/*
	 * If there are no survivors at this point, there is no
	 * system peer. If so and this is an old update, keep the
	 * current statistics, but do not update the clock.
	 */
	if (typesystem == NULL) {
		if (osys_peer != NULL) {
			orphwait = current_time + (unsigned long)sys_orphwait;
			report_event(EVNT_NOPEER, NULL, NULL);
		}
		sys_vars.sys_peer = NULL;
		for (peer = peer_list; peer != NULL; peer = peer->p_link)
			peer->status = peer->new_status;
		return;
	}

	/*
	 * Do not use old data, as this may mess up the clock discipline
	 * stability.
	 */
	if (typesystem->epoch <= sys_epoch)
		return;

	/*
	 * We have found the alpha male. Wind the clock.
	 */
	if (osys_peer != typesystem)
		report_event(PEVNT_NEWPEER, typesystem, NULL);
	for (peer = peer_list; peer != NULL; peer = peer->p_link)
		peer->status = peer->new_status;
	clock_update(typesystem);
}


static void
clock_combine(
	peer_select *	peers,	/* survivor list */
	int		npeers,	/* number of survivors */
	int		syspeer	/* index of sys.peer */
	)
{
	int	i;
	double	x, y, z, w;

	y = z = w = 0;
	for (i = 0; i < npeers; i++) {
		x = 1. / peers[i].synch;
		y += x;
		z += x * peers[i].peer->offset;
		w += x * DIFF(peers[i].peer->offset,
		    peers[syspeer].peer->offset);
	}
	clkstate.sys_offset = z / y;
	clkstate.sys_jitter = SQRT(w / y + SQUARE(peers[syspeer].seljit));
	sys_vars.sys_rootdist = peers[syspeer].synch;
}


/*
 * root_distance - compute synchronization distance from peer to root
 */
static double
root_distance(
	struct peer *peer	/* peer structure pointer */
	)
{
	double	dtemp;

	/*
	 * Root Distance (LAMBDA) is defined as:
	 * (delta + DELTA)/2 + epsilon + EPSILON + phi
	 *
	 * where:
	 *  delta   is the round-trip delay
	 *  DELTA   is the root delay
	 *  epsilon is the remote server precision + local precision
	 *	    + (15 usec each second)
	 *  EPSILON is the root dispersion
	 *  phi	    is the peer jitter statistic
	 *
	 * NB: Think hard about why we are using these values, and what
	 * the alternatives are, and the various pros/cons.
	 *
	 * DLM thinks these are probably the best choices from any of the
	 * other worse choices.
	 */
	dtemp = (peer->delay + peer->rootdelay) / 2
		+ LOGTOD(peer->precision)
		  + LOGTOD(sys_vars.sys_precision)
		  + loop_data.clock_phi * (current_time - peer->update)
		+ peer->rootdisp
		+ peer->jitter;
	/*
	 * Careful squeak here. The value returned must be greater than
	 * the minimum root dispersion in order to avoid clockhop with
	 * highly precise reference clocks. Note that the root distance
	 * cannot exceed the sys_maxdist, as this is the cutoff by the
	 * selection algorithm.
	 */
	if (dtemp < sys_mindist) {
		dtemp = sys_mindist;
	}
	return (dtemp);
}


/*
 * peer_xmit - send client-mode packet for persistent association.
 */
static void
peer_xmit(
	struct peer *peer	/* peer structure pointer */
	)
{
	struct		pkt xpkt;	/* transmit packet */
	unsigned int	sendlen;

	if (!peer->dstadr)		/* drop peers without interface */
		return;

	sendlen = LEN_PKT_NOMAC;
	if (NTP_VERSION == peer->cfg.version) {
		/* Hide most of info for privacy
		 * RFC in progress - draft-ietf-ntp-data-minimization, 2020-Feb-16
		 */
		xpkt.li_vn_mode = PKT_LI_VN_MODE(
			LEAP_NOWARNING, peer->cfg.version, MODE_CLIENT);
		xpkt.stratum = 0;
		xpkt.ppoll = 0;
		xpkt.precision = 0x20;
		xpkt.refid = 0;
		xpkt.rootdelay = 0;
		xpkt.rootdisp =	0;
		xpkt.reftime = htonl_fp(0);
		xpkt.org = htonl_fp(0);
		xpkt.rec = htonl_fp(0);
		ntp_RAND_bytes((unsigned char *)&peer->org_rand,
			sizeof(peer->org_rand));
		get_systime(&peer->org_ts);	/* as late as possible */
	} else {
		xpkt.li_vn_mode = PKT_LI_VN_MODE(
			sys_vars.sys_leap, peer->cfg.version, peer->hmode);
		xpkt.stratum = STRATUM_TO_PKT(sys_vars.sys_stratum);
		xpkt.ppoll = peer->hpoll;
		xpkt.precision = sys_vars.sys_precision;
		xpkt.refid = sys_vars.sys_refid;
		xpkt.rootdelay = HTONS_FP(DTOUFP(sys_vars.sys_rootdelay));
		xpkt.rootdisp =	 HTONS_FP(DTOUFP(sys_vars.sys_rootdisp));
		xpkt.reftime = htonl_fp(sys_vars.sys_reftime);
		xpkt.org = htonl_fp(peer->xmt);
		xpkt.rec = htonl_fp(peer->dst);
		get_systime(&peer->org_ts);	/* as late as possible */
		peer->org_rand = peer->org_ts;
	}

	/* FIXME bump org_ts to compensate for crypto */
	xpkt.xmt = htonl_fp(peer->org_rand);	/* out in xmt, back in org */

	/* 3 way branch to add authentication:
	 *  1) NTS
	 *  2) Shared KEY
	 *  3) none
	 */
	if (FLAG_NTS & peer->cfg.flags) {
#ifndef DISABLE_NTS
		if (0 < peer->nts_state.count)
		  sendlen += extens_client_send(peer, &xpkt);
		else {
		  restart_nts_ke(peer);  /* out of cookies */
		  return;
		}
#endif
	} else if (0 != peer->cfg.peerkey) {
		auth_info *auth = authlookup(peer->cfg.peerkey, true);
		if (NULL == auth) {
			report_event(PEVNT_AUTH, peer, "no key");
			peer->flash |= BOGON5;		/* auth error */
			peer->badauth++;
			return;
		}
		sendlen += authencrypt(auth, (uint32_t *)&xpkt, sendlen);
	}

	sendpkt(&peer->srcadr, peer->dstadr, &xpkt, sendlen);

	peer->sent++;
        peer->outcount++;
        peer->bogons = 0;
	peer->throttle += (1 << peer->cfg.minpoll) - 2;
	DPRINT(1, ("transmit: at %u %s->%s mode %d keyid %08x len %u\n",
		   current_time, peer->dstadr ?
		   socktoa(&peer->dstadr->sin) : "-",
		   socktoa(&peer->srcadr), peer->hmode,
		   peer->cfg.peerkey, sendlen));
}


#ifdef ENABLE_LEAP_SMEAR

static void
leap_smear_add_offs(l_fp *t) {
	*t += leap_smear.offset;
}

#endif	/* ENABLE_LEAP_SMEAR */

/*
 * fast_xmit - Send packet for nonpersistent association. Note that
 * neither the source or destination can be a broadcast address.
 */
static void
fast_xmit(
	struct recvbuf *rbufp,	/* receive packet pointer */
	int	xmode,		/* receive mode */
	auth_info *auth,	/* !NULL for authentication */
	int	flags		/* restrict mask */
	)
{
	struct pkt xpkt;	/* transmit packet structure */
	l_fp	xmt_tx;
	struct timespec	start, finish;
	size_t	sendlen;

	/*
	 * Initialize transmit packet header fields from the receive
	 * buffer provided. We leave the fields intact as received, but
	 * set the peer poll at the maximum of the receive peer poll and
	 * the system minimum poll (ntp_minpoll). This is for KoD rate
	 * control and not strictly specification compliant, but doesn't
	 * break anything.
	 */

	/*
	 * If this is a kiss-o'-death (KoD) packet, show leap
	 * unsynchronized, stratum zero, reference ID the four-character
	 * kiss code and system root delay. Note we don't reveal the
	 * local time, so these packets can't be used for
	 * synchronization.
	 */
	if (flags & RES_KOD) {
		stat_proto_total.sys_kodsent++;
		xpkt.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOTINSYNC,
		    PKT_VERSION(rbufp->pkt.li_vn_mode), xmode);
		xpkt.stratum = STRATUM_PKT_UNSPEC;
		xpkt.ppoll = max(rbufp->pkt.ppoll, rstrct.ntp_minpoll);
		xpkt.precision = rbufp->pkt.precision;
		memcpy(&xpkt.refid, "RATE", REFIDLEN);
		xpkt.rootdelay = htonl(rbufp->pkt.rootdelay);
		xpkt.rootdisp = htonl(rbufp->pkt.rootdisp);
		xpkt.reftime.l_ui = htonl(rbufp->pkt.reftime >> 32);
		xpkt.reftime.l_uf = htonl(rbufp->pkt.reftime & 0xFFFFFFFF);
		xpkt.org.l_ui = htonl(rbufp->pkt.xmt >> 32);
		xpkt.org.l_uf = htonl(rbufp->pkt.xmt & 0xFFFFFFFF);
		xpkt.rec.l_ui = htonl(rbufp->pkt.xmt >> 32);
		xpkt.rec.l_uf = htonl(rbufp->pkt.xmt & 0xFFFFFFFF);
		xpkt.xmt.l_ui = htonl(rbufp->pkt.xmt >> 32);
		xpkt.xmt.l_uf = htonl(rbufp->pkt.xmt & 0xFFFFFFFF);

	/*
	 * This is a normal packet. Use the system variables.
	 */
	} else {
#ifdef ENABLE_LEAP_SMEAR
		/*
		 * Make copies of the variables which can be affected by smearing.
		 */
		l_fp this_ref_time;
		l_fp this_recv_time;
#endif

		/*
		 * If we are inside the leap smear interval we add the
		 * current smear offset to the packet receive time, to
		 * the packet transmit time, and eventually to the
		 * reftime to make sure the reftime isn't later than
		 * the transmit/receive times.
		 */
		/* Note: This returns the same data for all versions.
		 * The version is copied from the request.
		 * There are minor differences between v3 and v4.
		 * So far, nobody cares.
		 * Note: There is significant NTPv1 traffic.  See #707
		 */
		xpkt.li_vn_mode = PKT_LI_VN_MODE(xmt_leap,
		    PKT_VERSION(rbufp->pkt.li_vn_mode), xmode);
		xpkt.stratum = STRATUM_TO_PKT(sys_vars.sys_stratum);
		xpkt.ppoll = max(rbufp->pkt.ppoll, rstrct.ntp_minpoll);
		xpkt.precision = sys_vars.sys_precision;
		xpkt.refid = sys_vars.sys_refid;
		xpkt.rootdelay = HTONS_FP(DTOUFP(sys_vars.sys_rootdelay));
		xpkt.rootdisp = HTONS_FP(DTOUFP(sys_vars.sys_rootdisp));

#ifdef ENABLE_LEAP_SMEAR
		this_ref_time = sys_vars.sys_reftime;
		if (leap_smear.in_progress) {
			leap_smear_add_offs(&this_ref_time);
			xpkt.refid = convertLFPToRefID(leap_smear.offset);
			DPRINT(2, ("fast_xmit: leap_smear.in_progress: refid %8x, smear %s\n",
				ntohl(xpkt.refid),
				lfptoa(leap_smear.offset, 8)
				));
		}
		xpkt.reftime = htonl_fp(this_ref_time);
#else
		xpkt.reftime = htonl_fp(sys_vars.sys_reftime);
#endif

		xpkt.org.l_ui = htonl(rbufp->pkt.xmt >> 32);
		xpkt.org.l_uf = htonl(rbufp->pkt.xmt & 0xFFFFFFFF);

#ifdef ENABLE_LEAP_SMEAR
		this_recv_time = rbufp->recv_time;
		if (leap_smear.in_progress)
			leap_smear_add_offs(&this_recv_time);
		xpkt.rec = htonl_fp(this_recv_time);
#else
		xpkt.rec = htonl_fp(rbufp->recv_time);
#endif

		get_systime(&xmt_tx);
#ifdef ENABLE_LEAP_SMEAR
		if (leap_smear.in_progress)
			leap_smear_add_offs(&xmt_tx);
#endif
		xpkt.xmt = htonl_fp(xmt_tx);
	}


#ifdef ENABLE_MSSNTP
	if (flags & RES_MSSNTP) {
		send_via_ntp_signd(rbufp, &xpkt); // Simplified the API
		return;
	}
#endif /* ENABLE_MSSNTP */


	/* 3 way branch to add authentication:
         *  1) NTS
         *  2) Shared KEY
         *  3) none
	 */
	sendlen = LEN_PKT_NOMAC;
	clock_gettime(CLOCK_MONOTONIC, &start);
	if (rbufp->ntspacket.valid) {
#ifndef DISABLE_NTS
	  sendlen += extens_server_send(&rbufp->ntspacket, &xpkt);
#endif
        } else if (NULL != auth) {
	  sendlen += (size_t)authencrypt(auth, (uint32_t *)&xpkt, (int)sendlen);
        }
	if (sendlen > rbufp->recv_length) {
	  /* About to send a response that is bigger than the request.
	   * That can be used for DDoS amplification, so don't do that.
	   * This shouldn't happen, but check here in case of a bug.
	   */
	  maybe_log_junk("DDoS", rbufp);	/* needs a counter */
	  return;
	}
	sendpkt(&rbufp->recv_srcadr, rbufp->dstadr, &xpkt, (int)sendlen);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	sys_authdelay = tspec_intv_to_lfp(sub_tspec(finish, start));
	/* Previous versions of this code had separate DPRINT-s so it
	 * could print the key on the auth case.  That requires separate
	 * sendpkt-s on each branch or the DPRINT pollutes the timing. */
	DPRINT(1, ("transmit: at %u %s->%s len %zu\n",
		   current_time, socktoa(&rbufp->dstadr->sin),
		   socktoa(&rbufp->recv_srcadr), sendlen));
}


/*
 * dns_take_server - process DNS query for server.
 */
void
dns_take_server(
	struct peer *server,
	sockaddr_u *rmtadr
	)
{
	struct peer *	pp;

	if(!(server->cfg.flags & FLAG_LOOKUP))
		/* Already got an address for this slot. */
		return;

	pp = findexistingpeer(rmtadr, NULL, NULL, MODE_CLIENT);
	if (NULL != pp) {
		/* Already in use. */
		msyslog(LOG_INFO, "DNS: Server skipping: %s", socktoa(rmtadr));
		return;
	}

	if (NTP_PORT == SRCPORT(rmtadr))
          msyslog(LOG_INFO, "DNS: Server taking: %s", socktoa(rmtadr));
        else
          msyslog(LOG_INFO, "DNS: Server taking: %s", sockporttoa(rmtadr));

	server->cfg.flags &= (unsigned)~FLAG_LOOKUP;
	server->srcadr = *rmtadr;
	{ /* HACK: fixup refid to show progress */
	char tag[REFIDLEN];
	if (server->cfg.flags & FLAG_NTS) {
		memcpy(tag, "NTS?", REFIDLEN);
	} else {
		memcpy(tag, "DNS?", REFIDLEN);
	}
	switch(AF(rmtadr)) {
	  case AF_INET:
		tag[REFIDLEN-1] = '4';
		break;
	  case AF_INET6:
		tag[REFIDLEN-1] = '6';
		break;
	  default:
		break;
	}
	memcpy(&server->refid, tag, REFIDLEN);
	}
	peer_add_hash(server);
	restrict_source(server);

	peer_refresh_interface(server);

	server->hpoll = server->cfg.minpoll;
	server->nextdate = current_time;
	peer_xmit(server);
	if (server->cfg.flags & FLAG_IBURST)
	  server->retry = NTP_RETRY;
	poll_update(server, server->hpoll);
}

/*
 dns_take_pool - process DNS query for pool.
 */
void
dns_take_pool(
	struct peer *pool,	/* pool solicitor association */
	sockaddr_u *		rmtadr
	)
{
	struct peer_ctl		pctl;
	struct peer *		peer;
	endpt *			lcladr;

	peer = findexistingpeer(rmtadr, NULL, NULL, MODE_CLIENT);
	if (NULL != peer) {
		/* This address is already in use. */
		msyslog(LOG_INFO, "DNS: Pool skipping: %s", socktoa(rmtadr));
		return;
	}

	msyslog(LOG_INFO, "DNS: Pool taking: %s", socktoa(rmtadr));

	lcladr = findinterface(rmtadr);
	memset(&pctl, '\0', sizeof(struct peer_ctl));
	pctl.version = pool->cfg.version;
	pctl.minpoll = pool->cfg.minpoll;
	pctl.maxpoll = pool->cfg.maxpoll;
	pctl.flags = FLAG_PREEMPT | (FLAG_IBURST & pool->cfg.flags);
	pctl.mode = 0;
	pctl.peerkey = 0;
	peer = newpeer(rmtadr, NULL, lcladr,
		       MODE_CLIENT, &pctl, MDF_UCAST, false);
	peer_xmit(peer);
	if (peer->cfg.flags & FLAG_IBURST)
	  peer->retry = NTP_RETRY;
	poll_update(peer, peer->hpoll);

	DPRINT(1, ("dns_take_pool: at %u %s->%s pool\n",
		   current_time, latoa(lcladr), socktoa(rmtadr)));
}

/*
 * dns_take_status -- setup retry time
 * There are 2 considerations.
 * 1) clutter in the log file
 * 2) the load on the server
 *   (API is broken, no way to get TTL)
 */
void dns_take_status(struct peer* peer, DNS_Status status) {
	uint8_t hpoll = peer->hpoll;
	const char *txt;
	const char *hostname = peer->hostname;

	if (NULL == hostname) {
		hostname = socktoa(&peer->srcadr);
	}

	if (hpoll < 8)
		hpoll = 8;      /* min retry: 256 seconds, ~5 min */
	switch (status) {
		case DNS_good:
			txt = "good";
			if (FLAG_LOOKUP & peer->cfg.flags)
				/* server: got answer, but didn't like any */
				/* (all) already in use ?? */
				hpoll += 1;
			else
				/* pool: maybe need more */
				hpoll = 8;
			break;
		case DNS_temp:
			/* DNS not working yet.  ??
			 * Want to retry soon,
			 * but also want to avoid log clutter.
			 * Beware, Fedora 29 lies:
			 *   What I expect to be temp (no Wifi)
			 *   gets EAI_NONAME, Name or service not known
			 */
			txt = "temp";
			hpoll = 3;
			break;
		case DNS_error:
			txt = "error";
			hpoll += 4;
			break;
		default:
			txt = "default";
			hpoll += 4;
	}
	if (hpoll > 12)
		hpoll = 12;  /* 4096, a bit over an hour */
	if ((DNS_good == status) &&
		(MDF_UCAST & peer->cast_flags) && !(FLAG_LOOKUP & peer->cfg.flags))
		hpoll = 0;  /* server: no more */
	msyslog(LOG_INFO, "DNS: dns_take_status: %s=>%s, %d",
		hostname, txt, hpoll);
	if (0 == hpoll)
		return; /* hpoll already in use by new server */
	peer->hpoll = hpoll;
	peer->nextdate = current_time + (1U << hpoll);
}

#ifndef DISABLE_NTS
/* NTS out of cookies
 * Beware of clutter in NTS-KE server logs
 * There are actually several cases:
 *   No NTS-KE server
 *   NTS-KE server answers, but we don't like it.
 *   NTS-KE works, but NTP server doesn't respond.
 */
static void restart_nts_ke(struct peer *peer) {
	uint8_t hpoll = peer->hpoll;
	peer_del_hash(peer);
	hpoll += 2;
	if (hpoll < 8)
		hpoll = 8;      /* min retry: 256 seconds, ~5 min */
	if (hpoll > 12)
		hpoll = 12;	/* 4096, a bit over an hour */
	peer->ppoll = NTP_MAXPOLL_UNK;
	peer->hpoll = hpoll;
	peer->nextdate = current_time + (1U << hpoll);
	peer->cfg.flags |= FLAG_LOOKUP;
};
#endif

/*
 * dns_try_again
 *   retry danging DNS and NTS lookups
 */
void dns_try_again(void) {
	struct peer *p;
	for (p = peer_list; p != NULL; p = p->p_link) {
		if ((p->cfg.flags & FLAG_LOOKUP) || (p->cast_flags & MDF_POOL)) {
			p->ppoll = NTP_MAXPOLL_UNK;
			p->hpoll = p->cfg.minpoll;
			transmit(p);   /* does all the work */
		}
	}
}


/*
 * local_refid(peer) - check peer refid to avoid selecting peers
 *		       currently synced to this ntpd.
 */
static int
local_refid(
	struct peer *	p
	)
{
	endpt *	unicast_ep;

// FIXME: Is this ever NULL?  MCAST leftover??
// (actually, just check)
	if (p->dstadr != NULL)
		unicast_ep = p->dstadr;
	else
		unicast_ep = findinterface(&p->srcadr);

	if (unicast_ep != NULL && p->refid == unicast_ep->addr_refid)
		return true;
	else
		return false;
}


/*
 * Determine if the peer is unfit for synchronization
 *
 * A peer is unfit for synchronization if
 * > BOGON10 bad leap or stratum below floor or at or above ceiling
 * > BOGON11 root distance exceeded for remote peer
 * > BOGON12 a direct or indirect synchronization loop would form
 * > BOGON13 unreachable or noselect
 */
int				/* false if fit, true if unfit */
peer_unfit(
	struct peer *peer	/* peer structure pointer */
	)
{
	int	rval = 0;

	/*
	 * A stratum error occurs if (1) the server has never been
	 * synchronized, (2) the server stratum is below the floor or
	 * greater than or equal to the ceiling.
	 */
	if (peer->leap == LEAP_NOTINSYNC || peer->stratum < sys_floor ||
	    peer->stratum >= sys_ceiling)
		rval |= BOGON10;		/* bad synch or stratum */

	/*
	 * A distance error for a remote peer occurs if the root
	 * distance is greater than or equal to the distance threshold
	 * plus the increment due to one host poll interval.
	 */
	if (!(peer->cfg.flags & FLAG_REFCLOCK) && root_distance(peer) >=
	    sys_maxdist + loop_data.clock_phi * ULOGTOD(peer->hpoll))
		rval |= BOGON11;		/* distance exceeded */
/* Startup bug, https://gitlab.com/NTPsec/ntpsec/issues/68
 *   introduced with ntp-dev-4.2.7p385
 * [2085] Fix root distance and root dispersion calculations.
 */
	if (!(peer->cfg.flags & FLAG_REFCLOCK) && peer->disp >=
	    sys_maxdist + loop_data.clock_phi * ULOGTOD(peer->hpoll))
		rval |= BOGON11;		/* Initialization */

	/*
	 * A loop error occurs if the remote peer is synchronized to the
	 * local peer or if the remote peer is synchronized to the same
	 * server as the local peer but only if the remote peer is
	 * neither a reference clock nor an orphan.
	 */
	if (peer->stratum > 1 && local_refid(peer))
		rval |= BOGON12;		/* synchronization loop */

	/*
	 * An unreachable error occurs if the server is unreachable or
	 * the noselect bit is set.
	 */
	if (!peer->reach || (peer->cfg.flags & FLAG_NOSELECT))
		rval |= BOGON13;		/* unreachable */

	peer->flash &= ~PEER_BOGON_MASK;
	peer->flash |= rval;
	return (rval);
}



/*
 * init_proto - initialize the protocol module's data
 */
void
init_proto(const bool verbose)
{
	l_fp	dummy;

	/*
	 * Fill in the sys_* stuff.
	 * Default is require authentication.
	 */
	set_sys_leap(LEAP_NOTINSYNC);
	sys_vars.sys_stratum = STRATUM_UNSPEC;
	memcpy(&sys_vars.sys_refid, "INIT", REFIDLEN);
	sys_vars.sys_peer = NULL;
	sys_vars.sys_rootdelay = 0;
	sys_vars.sys_rootdisp = 0;
	sys_vars.sys_reftime = 0;
	clkstate.sys_jitter = 0;
	UNUSED_ARG(verbose);
	sys_vars.sys_precision = -30; /* ns */  // FIXME FUZZ
	get_systime(&dummy);
	sys_survivors = 0;
	sys_stattime = current_time;
	orphwait = current_time + (unsigned long)sys_orphwait;
	proto_clr_stats();
	use_stattime = current_time;
	clock_ctl.hardpps_enable = false;
	stats_control = true;
}


/*
 * proto_config - configure the protocol module
 */
void
proto_config(
	int	item,
	unsigned long	value,
	double	dvalue
	)
{
	/*
	 * Figure out what he wants to change, then do it
	 */
	DPRINT(2, ("proto_config: code %d value %lu dvalue %lf\n",
		   item, value, dvalue));

	switch (item) {

	/*
	 * enable and disable commands - arguments are Boolean.
	 */
#ifdef REFCLOCK
	case PROTO_CAL:		/* refclock calibrate (calibrate) */
		cal_enable = value;
		break;
#endif /* REFCLOCK */

	case PROTO_KERNEL:	/* kernel discipline (kernel) */
		select_loop((int)value);
		break;

	case PROTO_MONITOR:	/* monitoring (monitor) */
		if (value) {
			mon_setup(MON_ON);
		} else {
			mon_setdown(MON_ON);
			if (mon_data.mon_enabled)
				msyslog(LOG_WARNING,
					"CONFIG: 'monitor' cannot be disabled while 'limited' is enabled");
		}
		break;

	case PROTO_NTP:		/* NTP discipline (ntp) */
		clock_ctl.ntp_enable = (bool)value;
		break;

	case PROTO_PPS:		/* PPS discipline (pps) */
		clock_ctl.hardpps_enable = (bool)value;
		break;

	case PROTO_FILEGEN:	/* statistics (stats) */
		stats_control = (bool)value;
		break;

	/*
	 * tos command - arguments are double, sometimes cast to int
	 */

	case PROTO_CEILING:	/* stratum ceiling (ceiling) */
		sys_ceiling = (int)dvalue;
		break;

	case PROTO_FLOOR:	/* stratum floor (floor) */
		sys_floor = (int)dvalue;
		break;

	case PROTO_MAXCLOCK:	/* maximum candidates (maxclock) */
		sys_maxclock = (int)dvalue;
		break;

	case PROTO_MAXDISP:	/* maximum dispersion (maxdisp) */
		sys_maxdisp = dvalue;
		break;

	case PROTO_MAXDIST:	/* select threshold (maxdist) */
		sys_maxdist = dvalue;
		break;

	case PROTO_MINCLOCK:	/* minimum candidates (minclock) */
		sys_minclock = (int)dvalue;
		break;

	case PROTO_MINDIST:	/* minimum distance (mindist) */
		sys_mindist = dvalue;
		break;

	case PROTO_MINSANE:	/* minimum survivors (minsane) */
		sys_minsane = (int)dvalue;
		break;

	case PROTO_ORPHAN:	/* orphan stratum (orphan) */
		sys_orphan = (int)dvalue;
		break;

	case PROTO_ORPHWAIT:	/* orphan wait (orphwait) */
		orphwait -= (unsigned long)sys_orphwait;
		sys_orphwait = (dvalue >= 1) ? (int)dvalue : NTP_ORPHWAIT;
		orphwait += (unsigned long)sys_orphwait;
		break;

	default:
		msyslog(LOG_NOTICE,
		    "CONFIG: unsupported protocol option %d", item);
	}
}


/*
 * proto_clr_stats - clear protocol stat counters
 */
void
proto_clr_stats(void)
{
    stat_proto_hourago = stat_proto_total;
    sys_stattime = current_time;
}


/* limit logging so bad guys can't DDoS us by sending crap */

void maybe_log_junk(const char *tag, struct recvbuf *rbufp) {
  static float junk_limit = 2.0;         /* packets per hour */
  static float junk_score = 0;           /* score, packets/hour */
  static float junk_decay = 2.0;         /* hours, exponential decay time */
  static l_fp  junk_last = 0;            /* time of last attempted print */
  static long  junk_count = 0;           /* total count */
  static long  junk_print = 0;           /* printed count */
#define JUNKSIZE 500
    char buf[JUNKSIZE];
    int lng = rbufp->recv_length;
    int i, j;

    junk_count++;
    if (0 == junk_last) {
      /* first time */
      junk_last = rbufp->recv_time;
    } else {
      l_fp interval_fp = rbufp->recv_time - junk_last;
      float since_last = ldexpf(interval_fp, -32)/3600.0;
      junk_last = rbufp->recv_time;
      junk_score *= expf(-since_last/junk_decay);
      if (junk_limit < junk_score)
	return;
    }
    junk_print++;
    junk_score += 1.0/junk_decay;  /* only count the ones we print */

    msyslog(LOG_INFO,
	"%s: Count=%ld Print=%ld, Score=%.3f, M%d V%d from %s, lng=%d",
	tag, junk_count, junk_print, junk_score,
        PKT_MODE(rbufp->pkt.li_vn_mode), PKT_VERSION(rbufp->pkt.li_vn_mode),
        sockporttoa(&rbufp->recv_srcadr), lng);
    for (i=0,j=0; i<lng; i++) {
      if ((j+4)>JUNKSIZE) break;
      if (0 == (i%4)) buf[j++] = ' ';
      j += snprintf(&buf[j], (JUNKSIZE-j), "%02x", rbufp->recv_buffer[i]);
    }
    msyslog(LOG_INFO,
	"%s: %s", tag, buf);
}

