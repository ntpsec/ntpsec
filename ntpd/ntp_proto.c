/*
 * ntp_proto.c - NTP version 4 protocol machinery
 *
 */
#include "config.h"

#include "ntpd.h"
#include "ntp_endian.h"
#include "ntp_stdlib.h"
#include "ntp_leapsec.h"
#include "ntp_dns.h"
#include "ntp_auth.h"
#include "timespecops.h"

#include <string.h>
#include <stdio.h>
#ifdef HAVE_LIBSCF_H
#include <libscf.h>
#endif
#include <unistd.h>

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
#define MINDISPERSE	.001	/* min distance */
#define CLOCK_SGATE	3.	/* popcorn spike gate */
#define	NTP_ORPHWAIT	300	/* orphan wait (s) */

/*
 * pool soliciting restriction duration (s)
 */
#define	POOL_SOLICIT_WINDOW	8

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
double	sys_offset;	/* current local clock offset */
double	sys_mindisp = MINDISPERSE; /* minimum distance (s) */
static double	sys_maxdist = MAXDISTANCE; /* selection threshold */
double	sys_maxdisp = MAXDISPERSE; /* maximum dispersion */
double	sys_jitter;		/* system jitter */
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

/*
 * Statistics counters - first the good, then the bad
 * These get reset every hour if sysstats is enabled.
 */
volatile struct statistics_counters stat_count;

double	measured_tick;		/* non-overridable sys_tick (s) */

static	void	clock_combine	(peer_select *, int, int);
static	void	clock_select	(void);
static	void	clock_update	(struct peer *);
static	void	fast_xmit	(struct recvbuf *, int, auth_info*, int);
static	int	local_refid	(struct peer *);
static	void	measure_precision(const bool);
static	double	measure_tick_fuzz(void);
static	void	peer_xmit	(struct peer *);
static	int	peer_unfit	(struct peer *);
static	double	root_distance	(struct peer *);


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
   out-of-range version number or an unsupported mode.
*/
static bool
is_vn_mode_acceptable(
	struct recvbuf const* rbufp
	)
{
	return rbufp->recv_length >= 1 &&
	    PKT_VERSION(rbufp->recv_space.X_recv_buffer[0]) >= 1 &&
	    PKT_VERSION(rbufp->recv_space.X_recv_buffer[0]) <= 4 &&
	    PKT_MODE(rbufp->recv_space.X_recv_buffer[0]) != MODE_PRIVATE &&
	    PKT_MODE(rbufp->recv_space.X_recv_buffer[0]) != MODE_UNSPEC;
}

static bool
is_control_packet(
	struct recvbuf const* rbufp
	)
{
	return rbufp->recv_length >= 1 &&
	    PKT_VERSION(rbufp->recv_space.X_recv_buffer[0]) <= 4 &&
	    PKT_MODE(rbufp->recv_space.X_recv_buffer[0]) == MODE_CONTROL;
}

/* There used to be a calloc/free for each received packet.
   Now, the parse_pkt version lives in a recvbuf.
   The alloc/free only happens for extensions and we don't support
   any of them.
*/
static void
free_extens(
	struct recvbuf *rbufp
	)
{
	if(rbufp->pkt.extensions != NULL) {
		for(size_t i = 0; i < rbufp->pkt.num_extensions; i++) {
			free(rbufp->pkt.extensions[i].body);
			rbufp->pkt.extensions[i].body = NULL;
		}
		free(rbufp->pkt.extensions);
		rbufp->pkt.extensions = NULL;
	}
}

static bool
parse_packet(
	struct recvbuf * rbufp
	)
{
	REQUIRE(rbufp != NULL);

	size_t recv_length = rbufp->recv_length;
	uint8_t const* recv_buf = rbufp->recv_space.X_recv_buffer;

	if(recv_length < LEN_PKT_NOMAC) {
		/* Packet is too short to possibly be valid. */
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

	/* Make sure these are clean before we might bail. */
        pkt->num_extensions = 0;
	pkt->extensions = NULL;

	rbufp->keyid_present = false;
	rbufp->keyid = 0;
	rbufp->mac_len = 0;

	if(PKT_VERSION(pkt->li_vn_mode) > 4) {
		/* Unsupported version */
		goto fail;
	} else if(PKT_VERSION(pkt->li_vn_mode) == 4) {
		/* Only version 4 packets support extensions. */

		/* Count and validate extensions */
		size_t ext_count = 0;
		size_t extlen = 0;
		while(bufptr <= recv_buf + recv_length - 28) {
			extlen = ntp_be16dec(bufptr + 2);
			if(extlen % 4 != 0 || extlen < 16) {
				/* Illegal extension length */
				goto fail;
			}
			if((size_t)(recv_buf + recv_length - bufptr) < extlen) {
				/* Extension length field points past
				 * end of packet */
				goto fail;
			}
			bufptr += extlen;
			ext_count++;
		}

		pkt->num_extensions = (unsigned int)ext_count;
		pkt->extensions = calloc(ext_count, sizeof (struct exten));
		if(pkt->extensions == NULL) { goto fail; }

		/* Copy extensions */
		bufptr = recv_buf + LEN_PKT_NOMAC;
		for(size_t i = 0; i < ext_count; i++) {
			pkt->extensions[i].type = ntp_be16dec(bufptr);
			pkt->extensions[i].len = ntp_be16dec(bufptr + 2) - 4;
			pkt->extensions[i].body =
			    calloc(1, pkt->extensions[i].len);
			if(pkt->extensions[i].body == NULL) { goto fail; }
			memcpy(pkt->extensions[i].body, bufptr + 4,
			       pkt->extensions[i].len);
			bufptr += pkt->extensions[i].len + 4;
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
			goto fail;
		}
		rbufp->keyid_present = true;
		rbufp->keyid = ntp_be32dec(bufptr);
		rbufp->mac_len = 0;
		break;
	    case 6:
		/* NTPv2 authenticator, which we allow but strip because
		   we don't support it any more */
		if(PKT_VERSION(pkt->li_vn_mode) != 2) { goto fail; }
		rbufp->keyid_present = false;
		rbufp->keyid = 0;
		rbufp->mac_len = 0;
		break;
	    case 20:
		/* AES-128 CMAC, MD5 digest */
		if(PKT_VERSION(pkt->li_vn_mode) < 3) {
			/* Only allowed as of NTPv3 */
			goto fail;
		}
		rbufp->keyid_present = true;
		rbufp->keyid = ntp_be32dec(bufptr);
		rbufp->mac_len = 16;
		break;
	    case 24:
		/* SHA-1 digest */
		if(PKT_VERSION(pkt->li_vn_mode) < 3) {
			/* Only allowed as of NTPv3 */
			goto fail;
		}
		rbufp->keyid_present = true;
		rbufp->keyid = ntp_be32dec(bufptr);
		rbufp->mac_len = 20;
		break;
	    case 72:
		/* MS-SNTP */
		if(PKT_VERSION(pkt->li_vn_mode) != 3) {
			/* Only allowed for NTPv3 */
			goto fail;
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
		goto fail;
	}

	return true;
  fail:
	free_extens(rbufp);
	return false;
}

/* Returns true if we should not accept any unauthenticated packets from
   this peer. There are three ways the user can configure this requirement:

   1. A 'restrict notrust' command applies to the peer's IP, or,
   2. We've configured the peer with a 'key' option,
   3. The packet wants to create a new associations, and a 'restrict
      nopeer' command applies to the peer's IP.

   The 'peer' argument may be NULL to indicate that we have no current
   association.

   In contrast to NTP classic, We don't enforce 'restrict nopeer'
   against pool-mode responses.

*/
static bool
i_require_authentication(
	struct peer const* peer,
	unsigned short restrict_mask
	)
{
        bool restrict_notrust = restrict_mask & RES_DONTTRUST;
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
	struct recvbuf const* rbufp
	)
{
	return PKT_LEAP(rbufp->pkt.li_vn_mode) == LEAP_NOTINSYNC &&
	    PKT_TO_STRATUM(rbufp->pkt.stratum) == STRATUM_UNSPEC;
}

/* Check the restrictions which can be checked just based on the source
   IP address and the first byte of the packet, namely RES_IGNORE,
   RES_FLAKE, RES_FLAKE, RES_NOQUERY, RES_DONTSERVE, and RES_VERSION. */

static bool check_early_restrictions(
	struct recvbuf const* rbufp,
	unsigned short restrict_mask
	)
{
	return (restrict_mask & RES_IGNORE) ||
	    ((restrict_mask & RES_FLAKE) &&
	     (double)ntp_random() / 0x7fffffff < .1) ||
	    (restrict_mask & (is_control_packet(rbufp) ? RES_NOQUERY : RES_DONTSERVE)) ||
	    rbufp->recv_length < 1 ||
	    ((restrict_mask & RES_VERSION) &&
	     (rbufp->recv_length < 1 ||
	      PKT_VERSION(rbufp->recv_space.X_recv_buffer[0]) != NTP_VERSION));
}

static void
handle_fastxmit(
	struct recvbuf *rbufp,
	unsigned short restrict_mask,
	auth_info* auth
	)
{
        int xmode =
            PKT_MODE(rbufp->pkt.li_vn_mode) == MODE_ACTIVE ? MODE_PASSIVE : MODE_SERVER;
	fast_xmit(rbufp, xmode, auth, restrict_mask);
}

static void
handle_procpkt(
	struct recvbuf *rbufp,
	struct peer *peer
	)
{
	int outcount = peer->outcount;

	/* Shouldn't happen, but include this for safety. */
	if(peer == NULL) { return; }

	peer->flash &= ~PKT_BOGON_MASK;

	/* Duplicate detection */
	if(rbufp->pkt.xmt == peer->xmt) {
		peer->flash |= BOGON1;
		peer->oldpkt++;
		return;
	}

	/* Origin timestamp validation */
	if(PKT_MODE(rbufp->pkt.li_vn_mode) == MODE_SERVER) {
		if(outcount == 0) {
			peer->flash |= BOGON1;
			peer->oldpkt++;
			return;
		}
		if(rbufp->pkt.org == 0) {
			peer->flash |= BOGON3;
			peer->bogusorg++;
			return;
		} else if(rbufp->pkt.org != peer->org_rand) {
			peer->flash |= BOGON2;
			peer->bogusorg++;
			return;
		}
	} else {
		/* This case should be unreachable. */
		stat_count.sys_declined++;
		return;
	}

	/* We've now cryptographically authenticated the packet if
	   required, and we've checked the origin timestamp, so if we've
	   gotten this far we can safely assume the packet is not
	   spoofed and start updating important peer variables.
	*/

	peer->outcount = 0;
	outcount--;

	if(is_kod(rbufp)) {
		if(!memcmp(rbufp->pkt.refid, "RATE", REFIDLEN)) {
			peer->selbroken++;
			report_event(PEVNT_RATE, peer, NULL);
			if (peer->cfg.minpoll < 10) { peer->cfg.minpoll = 10; }
			peer->burst = peer->retry = 0;
			peer->throttle = (NTP_SHIFT + 1) * (1 << peer->cfg.minpoll);
			poll_update(peer, 10);
		}
		return;
	}

	if (PKT_LEAP(rbufp->pkt.li_vn_mode) == LEAP_NOTINSYNC ||
	    PKT_TO_STRATUM(rbufp->pkt.stratum) < sys_floor ||
	    PKT_TO_STRATUM(rbufp->pkt.stratum) >= sys_ceiling) {
		peer->flash |= BOGON6;
		return;
	}

	if(scalbn((double)rbufp->pkt.rootdelay/2.0 + (double)rbufp->pkt.rootdisp, -16) >=
	   sys_maxdisp) {
		peer->flash |= BOGON7;
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
	    clock_phi * delta;

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
	  peer->flash |= BOGON1; /*XXX we should probably allocate a
				   new bogon bit here rather than
				   recycling BOGON1. */
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

	record_raw_stats(peer,
			 /* What we want to be reporting is values in
			    the packet, not the values in the peer
			    structure, but when we reach here they're
			    the same thing. Passing the values in the
			    peer structure is a convenience, because
			    they're already in the l_fp format that
			    record_raw_stats() expects. */
			 PKT_LEAP(rbufp->pkt.li_vn_mode),
			 PKT_VERSION(rbufp->pkt.li_vn_mode),
			 PKT_MODE(rbufp->pkt.li_vn_mode),
			 PKT_TO_STRATUM(rbufp->pkt.stratum),
			 rbufp->pkt.ppoll, rbufp->pkt.precision,
			 rbufp->pkt.rootdelay, rbufp->pkt.rootdisp,
			 /* FIXME: this cast is disgusting */
			 *(const uint32_t*)rbufp->pkt.refid,
			 outcount);

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

	stat_count.sys_received++;

	if(!is_vn_mode_acceptable(rbufp)) {
		stat_count.sys_badlength++;
		goto done;
	}

/* FIXME: This is lots more cleanup to do in this area. */

	restrict_mask = restrictions(&rbufp->recv_srcadr);

	if(check_early_restrictions(rbufp, restrict_mask)) {
		stat_count.sys_restricted++;
		goto done;
	}

	restrict_mask = ntp_monitor(rbufp, restrict_mask);
	if (restrict_mask & RES_LIMITED) {
		stat_count.sys_limitrejected++;
		if(!(restrict_mask & RES_KOD)) { goto done; }
	}

	if(is_control_packet(rbufp)) {
		process_control(rbufp, restrict_mask);
		stat_count.sys_processed++;
		goto done;
	}

	/*
	 * Version check must be after the query packets, since they
	 * intentionally use an early version.
	 */
	{
	uint8_t hisversion = PKT_VERSION(rbufp->recv_buffer[0]);
	if (hisversion == NTP_VERSION) {
		stat_count.sys_newversion++;		/* new version */
	} else if (!(restrict_mask & RES_VERSION) && hisversion >=
	    NTP_OLDVERSION) {
		stat_count.sys_oldversion++;		/* previous version */
	} else {
		stat_count.sys_badlength++;
		goto done;			/* old version */
	}
	}

	if (!parse_packet(rbufp)) {
		stat_count.sys_badlength++;
		goto done;
	}

	if (MODE_SERVER == PKT_MODE(rbufp->pkt.li_vn_mode)) {
	    /* Reply to our request:
	     * Auth check breaks if we findpeer for MODE_CLIENT and
	     * a site we are using as a server uses us as a server
	     * with a different key. */
	    peer = findpeer(rbufp);
	    if (NULL == peer) {
		stat_count.sys_declined++;
		goto done;
	    }
	}

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
				 (uint32_t*)rbufp->recv_space.X_recv_buffer,
				 (int)(rbufp->recv_length - (rbufp->mac_len + 4)),
				 (int)(rbufp->mac_len + 4))) {

			stat_count.sys_badauth++;
			if(peer != NULL) {
				peer->badauth++;
				peer->cfg.flags &= ~FLAG_AUTHENTIC;
				peer->flash |= BOGON5;
			}
			goto done;
		}
	}

	if (peer != NULL) {
	    	peer->received++;
		peer->cfg.flags |= FLAG_AUTHENTIC;
		peer->timereceived = current_time;
	}

	switch (PKT_MODE(rbufp->pkt.li_vn_mode)) {
	    case MODE_ACTIVE:  /* remote site using "peer" in config file */
	    case MODE_CLIENT:  /* Request for us as a server. */
		handle_fastxmit(rbufp, restrict_mask, auth);
		stat_count.sys_processed++;
		break;
	    case MODE_SERVER:  /* Reply to our request. */
		handle_procpkt(rbufp, peer);
		stat_count.sys_processed++;
		peer->processed++;
		break;
	    default:
		/* Everything else is for broadcast modes,
		   which are a security nightmare.  So they go to the
		   bit bucket until this improves.
		*/
		stat_count.sys_declined++;
		break;
	}

  done:
	free_extens(rbufp);
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
#ifdef ENABLE_DNS_LOOKUP
	if (peer->cast_flags & MDF_POOL) {
		peer->outdate = current_time;
		if ((peer_associations <= 2 * sys_maxclock) &&
		    (peer_associations < sys_maxclock ||
		     sys_survivors < sys_minclock))
			if (!dns_probe(peer)) return;
		poll_update(peer, hpoll);
		return;
	}

	/* Does server need DNS lookup? */
	if (peer->cfg.flags & FLAG_DNS) {
		peer->outdate = current_time;
		if (!dns_probe(peer)) return;
		poll_update(peer, hpoll);
		return;
        }
#endif

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
			hpoll = sys_poll;
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
	if (sys_poll < peer->cfg.minpoll)
		sys_poll = peer->cfg.minpoll;
	if (sys_poll > peer->cfg.maxpoll)
		sys_poll = peer->cfg.maxpoll;
	poll_update(peer, sys_poll);
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
		+ sys_jitter
		+ clock_phi * (current_time - peer->update)
		+ fabs(sys_offset);

	if (dtemp > sys_mindisp)
		sys_vars.sys_rootdisp = dtemp;
	else
		sys_vars.sys_rootdisp = sys_mindisp;
	sys_vars.sys_rootdelay = peer->delay + peer->rootdelay;
	sys_vars.sys_reftime = peer->dst;

	DPRINT(1, ("clock_update: at %u sample %u associd %d\n",
		   current_time, peer->epoch, peer->associd));

	/*
	 * Comes now the moment of truth. Crank the clock discipline and
	 * see what comes out.
	 */
	switch (local_clock(peer, sys_offset)) {

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
		msyslog(LOG_ERR, "CLOCK: Panic: offset too big: %.3f", sys_offset);
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
		sys_jitter = LOGTOD(sys_vars.sys_precision);
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
#ifdef HAVE_WORKING_FORK
			if (waitsync_fd_to_close != -1) {
				close(waitsync_fd_to_close);
				waitsync_fd_to_close = -1;
				DPRINT(1, ("notified parent --wait-sync is done\n"));
			}
#endif /* HAVE_WORKING_FORK */

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
	    (1 << peer->cfg.minpoll), ntp_minpkt);
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
			next = ((0x1000UL | (ntp_random() & 0x0ff)) <<
			    hpoll) >> 12;
		next += peer->outdate;
		if (next > utemp)
			peer->nextdate = next;
		else
			peer->nextdate = utemp;
		if (peer->throttle > (1 << peer->cfg.minpoll))
			peer->nextdate += (unsigned long)ntp_minpkt;
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
	peer->ppoll = peer->cfg.maxpoll;
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
	} else if (MODE_PASSIVE == peer->hmode) {
		peer->nextdate += (unsigned long)ntp_minpkt;
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
	    int pseudorandom = peer->associd ^ sock_hash(&peer->srcadr);
	    peer->nextdate += (unsigned long)(pseudorandom % (1 << peer->cfg.minpoll));
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
	dtemp = clock_phi * (current_time - peer->update);
	peer->update = current_time;
	for (i = NTP_SHIFT - 1; i >= 0; i--) {
		if (i != 0)
			peer->filter_disp[j] += dtemp;
		if (peer->filter_disp[j] >= sys_maxdisp) {
			peer->filter_disp[j] = sys_maxdisp;
			dst[i] = sys_maxdisp;
		} else if (peer->update - peer->filter_epoch[j] >
		    (unsigned long)ULOGTOD(allan_xpt)) {
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
 * ENABLE_LOCKCLOCK: (1) If the local clock is the prefer peer, it will always
 * be enabled, even if declared falseticker, (2) only the prefer peer
 * can be selected as the system peer, (3) if the external source is
 * down, the system leap bits are set to 11 and the stratum set to
 * infinity.
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
#ifdef ENABLE_LOCKCLOCK
	set_sys_leap(LEAP_NOTINSYNC);
	sys_vars.sys_stratum = STRATUM_UNSPEC;
	memcpy(&sys_vars.sys_refid, "DOWN", REFIDLEN);
#endif /* ENABLE_LOCKCLOCK */

	/*
	 * Allocate dynamic space depending on the number of
	 * associations.
	 */
	nlist = 1;
	for (peer = peer_list; peer != NULL; peer = peer->p_link)
		nlist++;
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
		if (peer_unfit(peer))
			continue;

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
	for (i = 0; i < nl2; i++)
		indx[i] = i;
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
			if (n >= nlist - allow)
				break;
		}
		n = 0;
		for (j = nl2 - 1; j >= 0; j--) {
			high = endpoint[indx[j]].val;
			n += endpoint[indx[j]].type;
			if (n >= nlist - allow)
				break;
		}

		/*
		 * If an interval containing truechimers is found, stop.
		 * If not, increase the number of falsetickers and go
		 * around again.
		 */
		if (high > low)
			break;
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

		if (j != i)
			peers[j] = peers[i];
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
		peers[0].synch = sys_mindisp;
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
			if (peers[i].error < d)
				d = peers[i].error;
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
		for (j = k + 1; j < nlist; j++)
			peers[j - 1] = peers[j];
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
	 * stratum scaled by sys_mindisp (.001 by default). The goal of
	 * the small stratum factor is to avoid clockhop between a
	 * reference clock and a network peer which has a refclock and
	 * is using an older ntpd, which does not floor sys_rootdisp at
	 * sys_mindisp.
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
			else if (leap_vote_ins < nlist)
				leap_vote_ins++;
		}
		if (peer->leap == LEAP_DELSECOND) {
			if (peer->cfg.flags & FLAG_REFCLOCK)
				leap_vote_del = nlist;
			else if (leap_vote_del < nlist)
				leap_vote_del++;
		}
		if (peer->cfg.flags & FLAG_PREFER)
			sys_prefer = peer;
		speermet = peers[i].seljit * peers[i].synch +
		    peer->stratum * sys_mindisp;
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
		    osys_peer->offset)) < sys_mindisp) {
			if ( D_ISZERO_NS(sys_clockhop) )
				sys_clockhop = sys_mindisp;
			else
				sys_clockhop *= .5;
			DPRINT(1, ("select: clockhop %d %.6f %.6f\n",
				   j, x, sys_clockhop));
			if (fabs(x) < sys_clockhop)
				typesystem = osys_peer;
			else
				sys_clockhop = 0;
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
			sys_offset = typesystem->offset;
			sys_jitter = typesystem->jitter;
			sys_vars.sys_rootdist = root_distance(typesystem);
		}
		DPRINT(1, ("select: combine offset %.9f jitter %.9f\n",
			   sys_offset, sys_jitter));
	}
#ifdef REFCLOCK
	/*
	 * If a PPS driver is lit and the combined offset is less than
	 * 0.4 s, select the driver as the PPS peer and use its offset
	 * and jitter. However, if this is the pps driver, use it only
	 * if there is a prefer peer or there are no survivors and none
	 * are required.
	 */
	if (typepps != NULL && fabs(sys_offset) < 0.4 &&
	    (!typepps->is_pps_driver ||
	    (typepps->is_pps_driver && (sys_prefer !=
	    NULL || (typesystem == NULL && sys_minsane == 0))))) {
		typesystem = typepps;
		sys_clockhop = 0;
		typesystem->new_status = CTL_PST_SEL_PPS;
		sys_offset = typesystem->offset;
		sys_jitter = typesystem->jitter;
		sys_vars.sys_rootdist = root_distance(typesystem);
		DPRINT(1, ("select: pps offset %.9f jitter %.9f\n",
			   sys_offset, sys_jitter));
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
			if (sys_orphwait > 0)
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
	sys_offset = z / y;
	sys_jitter = SQRT(w / y + SQUARE(peers[syspeer].seljit));
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
		  + clock_phi * (current_time - peer->update)
		+ peer->rootdisp
		+ peer->jitter;
	/*
	 * Careful squeak here. The value returned must be greater than
	 * the minimum root dispersion in order to avoid clockhop with
	 * highly precise reference clocks. Note that the root distance
	 * cannot exceed the sys_maxdist, as this is the cutoff by the
	 * selection algorithm.
	 */
	if (dtemp < sys_mindisp)
		dtemp = sys_mindisp;
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
		 * RFC in progress - draft-ietf-ntp-data-minimization, 2018-Jul-07
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
		peer->org_rand = ntp_random64();
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

	xpkt.xmt = htonl_fp(peer->org_rand);	/* out in xmt, back in org */


	/*
	 * If the peer (aka server) was configured with a key, authenticate
	 * the packet.  Else, the packet is not authenticated.
	 */
	if (0 != peer->cfg.peerkey) {
		auth_info *auth = authlookup(peer->cfg.peerkey, true);
		if (NULL == auth) {
			report_event(PEVNT_AUTH, peer, "no key");
			peer->flash |= BOGON5;		/* auth error */
			peer->badauth++;
			return;
		}
		/* Maybe bump peer->org_ts to account for crypto time */
		sendlen += authencrypt(auth, (uint32_t *)&xpkt, sendlen);
		if (sendlen > sizeof(xpkt)) {
			msyslog(LOG_ERR, "PROTO: buffer overflow %u", sendlen);
			exit(1);
		}
	}

	sendpkt(&peer->srcadr, peer->dstadr, &xpkt, sendlen);

	peer->sent++;
        peer->outcount++;
	peer->throttle += (1 << peer->cfg.minpoll) - 2;
	DPRINT(1, ("transmit: at %u %s->%s mode %d keyid %08x len %u\n",
		   current_time, peer->dstadr ?
		   socktoa(&peer->dstadr->sin) : "-",
		   socktoa(&peer->srcadr), peer->hmode,
		   peer->cfg.peerkey, sendlen));
}


#ifdef ENABLE_LEAP_SMEAR

static void
leap_smear_add_offs(l_fp *t, l_fp *t_recv) {
	UNUSED_ARG(t_recv);
	t += leap_smear.offset;
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
	l_fp	xmt_tx, xmt_ty;
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
		stat_count.sys_kodsent++;
		xpkt.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOTINSYNC,
		    PKT_VERSION(rbufp->pkt.li_vn_mode), xmode);
		xpkt.stratum = STRATUM_PKT_UNSPEC;
		xpkt.ppoll = max(rbufp->pkt.ppoll, ntp_minpoll);
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
		xpkt.li_vn_mode = PKT_LI_VN_MODE(sys_vars.sys_leap,
		    PKT_VERSION(rbufp->pkt.li_vn_mode), xmode);
		xpkt.stratum = STRATUM_TO_PKT(sys_vars.sys_stratum);
		xpkt.ppoll = max(rbufp->pkt.ppoll, ntp_minpoll);
		xpkt.precision = sys_vars.sys_precision;
		xpkt.refid = sys_vars.sys_refid;
		xpkt.rootdelay = HTONS_FP(DTOUFP(sys_vars.sys_rootdelay));
		xpkt.rootdisp = HTONS_FP(DTOUFP(sys_vars.sys_rootdisp));

#ifdef ENABLE_LEAP_SMEAR
		this_ref_time = sys_vars.sys_reftime;
		if (leap_smear.in_progress) {
			leap_smear_add_offs(&this_ref_time, NULL);
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
			leap_smear_add_offs(&this_recv_time, NULL);
		xpkt.rec = htonl_fp(this_recv_time);
#else
		xpkt.rec = htonl_fp(rbufp->recv_time);
#endif

		get_systime(&xmt_tx);
#ifdef ENABLE_LEAP_SMEAR
		if (leap_smear.in_progress)
			leap_smear_add_offs(&xmt_tx, &this_recv_time);
#endif
		xpkt.xmt = htonl_fp(xmt_tx);
	}

#ifdef ENABLE_MSSNTP
	if (flags & RES_MSSNTP) {
		keyid_t keyid = 0;
		if (NULL != auth) keyid = auth->keyid;
		send_via_ntp_signd(rbufp, xmode, keyid, flags, &xpkt);
		return;
	}
#endif /* ENABLE_MSSNTP */

	/*
	 * If the received packet contains a MAC, the transmitted packet
	 * is authenticated and contains a MAC. If not, the transmitted
	 * packet is not authenticated.
	 */
	sendlen = LEN_PKT_NOMAC;
	if (NULL == auth) {
		sendpkt(&rbufp->recv_srcadr, rbufp->dstadr, &xpkt, (int)sendlen);
		DPRINT(1, ("transmit: at %u %s->%s mode %d len %zu\n",
			   current_time, socktoa(&rbufp->dstadr->sin),
			   socktoa(&rbufp->recv_srcadr), xmode, sendlen));
		return;
	}

	/*
	 * The received packet contains a MAC, so the transmitted packet
	 * must be authenticated. For symmetric key cryptography, use
	 * the predefined and trusted symmetric keys to generate the
	 * cryptosum.
	 */
	get_systime(&xmt_tx);
	sendlen += (size_t)authencrypt(auth, (uint32_t *)&xpkt, (int)sendlen);
	sendpkt(&rbufp->recv_srcadr, rbufp->dstadr, &xpkt, (int)sendlen);
	get_systime(&xmt_ty);
	xmt_ty -= xmt_tx;
	sys_authdelay = xmt_ty;
	DPRINT(1, ("transmit: at %u %s->%s mode %d keyid %08x len %zu\n",
		   current_time, socktoa(&rbufp->dstadr->sin),
		   socktoa(&rbufp->recv_srcadr), xmode, auth->keyid, sendlen));
}


#ifdef ENABLE_DNS_LOOKUP
/*
 * dns_take_server - process DNS query for server.
 */
void
dns_take_server(
	struct peer *server,
	sockaddr_u *rmtadr
	)
{
	int		restrict_mask;
	struct peer *	pp;

	if(!(server->cfg.flags & FLAG_DNS))
		/* Already got an address for this slot. */
		return;

	pp = findexistingpeer(rmtadr, NULL, NULL, MODE_CLIENT);
	if (NULL != pp) {
		/* Already in use. */
		msyslog(LOG_INFO, "DNS: Server skipping: %s", socktoa(rmtadr));
		return;
	}

	msyslog(LOG_INFO, "DNS: Server taking: %s", socktoa(rmtadr));
	server->cfg.flags &= (unsigned)~FLAG_DNS;

	server->srcadr = *rmtadr;
	peer_update_hash(server);

	restrict_mask = restrictions(&server->srcadr);
	if (RES_FLAGS & restrict_mask) {
		msyslog(LOG_INFO, "DNS: Server poking hole in restrictions for: %s",
			socktoa(&server->srcadr));
		restrict_source(&server->srcadr, false, 0);
	}

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
	int			restrict_mask;
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
		       MODE_CLIENT, &pctl, MDF_UCAST | MDF_UCLNT, false);
	peer_xmit(peer);
	if (peer->cfg.flags & FLAG_IBURST)
	  peer->retry = NTP_RETRY;
	poll_update(peer, peer->hpoll);

	restrict_mask = restrictions(&peer->srcadr);
	/* FIXME-DNS: RES_FLAGS includes RES_DONTSERVE?? */
	if (RES_FLAGS & restrict_mask) {
		msyslog(LOG_INFO, "DNS: Pool poking hole in restrictions for: %s",
				socktoa(&peer->srcadr));
		restrict_source(&peer->srcadr, false,
				current_time + POOL_SOLICIT_WINDOW + 1);
	}

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
	const char *txt;
	uint8_t hpoll = peer->hpoll;
	if (hpoll < 8)
		hpoll = 8;      /* min retry: 256 seconds, ~5 min */
	switch (status) {
		case DNS_good:
			txt = "good";
			if (FLAG_DNS & peer->cfg.flags)
				/* server: got answer, but didn't like any */
				/* (all) already in use ?? */
				hpoll += 1;
			else
				/* pool: maybe need more */
				hpoll = 8;
			break;
		case DNS_temp:
			txt = "temp";
			hpoll += 1;
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
		(MDF_UCAST & peer->cast_flags) && !(FLAG_DNS & peer->cfg.flags))
		hpoll = 0;  /* server: no more */
	msyslog(LOG_INFO, "DNS: dns_take_status: %s=>%s, %d",
		peer->hostname, txt, hpoll);
	if (0 == hpoll)
		return; /* hpoll already in use by new server */
	peer->hpoll = hpoll;
	peer->nextdate = current_time + (1U << hpoll);
}

/*
 * dns_new_interface
 *   A new interface is now active
 *   retry danging DNS lookups
 */
void dns_new_interface(void) {
    struct peer *p;
    for (p = peer_list; p != NULL; p = p->p_link) {
	if ((p->cfg.flags & FLAG_DNS) || (p->cast_flags & MDF_POOL)) {
	    p->hpoll = p->cfg.minpoll;
	    transmit(p);   /* does all the work */
	}
    }
}

#endif /* ENABLE_DNS_LOOKUP */



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

	if (p->dstadr != NULL && !(INT_MCASTIF & p->dstadr->flags))
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
	    sys_maxdist + clock_phi * ULOGTOD(peer->hpoll))
		rval |= BOGON11;		/* distance exceeded */
/* Startup bug, https://gitlab.com/NTPsec/ntpsec/issues/68
 *   introduced with ntp-dev-4.2.7p385
 * [2085] Fix root distance and root dispersion calculations.
 */
	if (!(peer->cfg.flags & FLAG_REFCLOCK) && peer->disp >=
	    sys_maxdist + clock_phi * ULOGTOD(peer->hpoll))
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
 * Find the precision of this particular machine
 */
#define MINSTEP		20e-9	/* minimum clock increment (s) */
#define MAXSTEP		1	/* maximum clock increment (s) */
#define MINCHANGES	12	/* minimum number of step samples */
#define MAXLOOPS	((int)(1. / MINSTEP))	/* avoid infinite loop */

/*
 * This routine measures the system precision defined as the minimum of
 * a sequence of differences between successive readings of the system
 * clock. However, if a difference is less than MINSTEP, the clock has
 * been read more than once during a clock tick and the difference is
 * ignored. We set MINSTEP greater than zero in case something happens
 * like a cache miss, and to tolerate underlying system clocks which
 * ensure each reading is strictly greater than prior readings while
 * using an underlying stepping (not interpolated) clock.
 *
 * sys_tick and sys_precision represent the time to read the clock for
 * systems with high-precision clocks, and the tick interval or step
 * size for lower-precision stepping clocks.
 *
 * This routine also measures the time to read the clock on stepping
 * system clocks by counting the number of readings between changes of
 * the underlying clock.  With either type of clock, the minimum time
 * to read the clock is saved as sys_fuzz, and used to ensure the
 * get_systime() readings always increase and are fuzzed below sys_fuzz.
 */
void
measure_precision(const bool verbose)
{
	/*
	 * With sys_fuzz set to zero, get_systime() fuzzing of low bits
	 * is effectively disabled.  trunc_os_clock is false to disable
	 * get_ostime() simulation of a low-precision system clock.
	 */
	set_sys_fuzz(0.);
	trunc_os_clock = false;
	measured_tick = measure_tick_fuzz();
	set_sys_tick_precision(measured_tick);
	if (verbose) {
		msyslog(LOG_INFO, "INIT: precision = %.3f usec (%d)",
			sys_tick * US_PER_S, sys_vars.sys_precision);
		if (sys_fuzz < sys_tick) {
			msyslog(LOG_NOTICE, "INIT: fuzz beneath %.3f usec",
				sys_fuzz * US_PER_S);
		}
	}
}


/*
 * measure_tick_fuzz()
 *
 * measures the minimum time to read the clock (stored in sys_fuzz)
 * and returns the tick, the larger of the minimum increment observed
 * between successive clock readings and the time to read the clock.
 */
double
measure_tick_fuzz(void)
{
	l_fp	minstep;	/* MINSTEP as l_fp */
	l_fp	val;		/* current seconds fraction */
	l_fp	last;		/* last seconds fraction */
	l_fp	ldiff;		/* val - last */
	double	tick;		/* computed tick value */
	double	diff;
	long	repeats;
	long	max_repeats;
	int	changes;
	int	i;		/* log2 precision */

	tick = MAXSTEP;
	max_repeats = 0;
	repeats = 0;
	changes = 0;
	minstep = dtolfp(MINSTEP);
	get_systime(&last);
	for (i = 0; i < MAXLOOPS && changes < MINCHANGES; i++) {
		get_systime(&val);
		ldiff = val;
		ldiff -= last;
		last = val;
		if (L_ISGT(ldiff, minstep)) {
			max_repeats = max(repeats, max_repeats);
			repeats = 0;
			changes++;
			diff = lfptod(ldiff);
			tick = min(diff, tick);
		} else {
			repeats++;
		}
	}
	if (changes < MINCHANGES) {
		msyslog(LOG_ERR, "PROTO: Fatal error: precision could not be measured (MINSTEP too large?)");
		exit(1);
	}

	if (0 == max_repeats) {
		set_sys_fuzz(tick);
	} else {
		set_sys_fuzz(tick / max_repeats);
	}

	return tick;
}


void
set_sys_tick_precision(
	double tick
	)
{
	int i;

	if (tick > 1.) {
		msyslog(LOG_ERR,
			"INIT: unsupported tick %.3f > 1s ignored", tick);
		return;
	}
	if (tick < measured_tick) {
		msyslog(LOG_ERR,
			"INIT: tick %.3f less than measured tick %.3f, ignored",
			tick, measured_tick);
		return;
	} else if (tick > measured_tick) {
		trunc_os_clock = true;
		msyslog(LOG_NOTICE,
			"INIT: truncating system clock to multiples of %.9f",
			tick);
	}
	sys_tick = tick;

	/*
	 * Find the nearest power of two.
	 */
	for (i = 0; tick <= 1; i--)
		tick *= 2;
	if (tick - 1 > 1 - tick / 2)
		i++;

	sys_vars.sys_precision = (int8_t)i;
}


/*
 * init_proto - initialize the protocol module's data
 */
void
init_proto(const bool verbose)
{
	l_fp	dummy;

	/*
	 * Fill in the sys_* stuff.  Default is don't listen to
	 * broadcasting, require authentication.
	 */
	sys_vars.sys_leap = LEAP_NOTINSYNC;
	sys_vars.sys_stratum = STRATUM_UNSPEC;
	memcpy(&sys_vars.sys_refid, "INIT", REFIDLEN);
	sys_vars.sys_peer = NULL;
	sys_vars.sys_rootdelay = 0;
	sys_vars.sys_rootdisp = 0;
	sys_vars.sys_reftime = 0;
	sys_jitter = 0;
	measure_precision(verbose);
	get_systime(&dummy);
	sys_survivors = 0;
	stat_count.sys_stattime = current_time;
	orphwait = current_time + (unsigned long)sys_orphwait;
	proto_clr_stats();
	stat_count.use_stattime = current_time;
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
		if (value)
			mon_start(MON_ON);
		else {
			mon_stop(MON_ON);
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

	case PROTO_CALLDELAY:	/* modem call delay (mdelay) */
		break;		/* NOT USED */

	case PROTO_MINCLOCK:	/* minimum candidates (minclock) */
		sys_minclock = (int)dvalue;
		break;

	case PROTO_MINDISP:	/* minimum distance (mindist) */
		sys_mindisp = dvalue;
		break;

	case PROTO_MINSANE:	/* minimum survivors (minsane) */
		sys_minsane = (int)dvalue;
		break;

	case PROTO_ORPHAN:	/* orphan stratum (orphan) */
		sys_orphan = (int)dvalue;
		break;

	case PROTO_ORPHWAIT:	/* orphan wait (orphwait) */
		orphwait -= (unsigned long)sys_orphwait;
		sys_orphwait = (int)dvalue;
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
	stat_count.sys_stattime = current_time;
	stat_count.sys_received = 0;
	stat_count.sys_processed = 0;
	stat_count.sys_newversion = 0;
	stat_count.sys_oldversion = 0;
	stat_count.sys_declined = 0;
	stat_count.sys_restricted = 0;
	stat_count.sys_badlength = 0;
	stat_count.sys_badauth = 0;
	stat_count.sys_limitrejected = 0;
	stat_count.sys_kodsent = 0;
}

