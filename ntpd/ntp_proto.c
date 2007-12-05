/*
 * ntp_proto.c - NTP version 4 protocol machinery
 *
 * ATTENTION: Get approval from Dave Mills on all changes to this file!
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ntpd.h"
#include "ntp_stdlib.h"
#include "ntp_unixtime.h"
#include "ntp_control.h"
#include "ntp_string.h"

#include <stdio.h>
#ifdef HAVE_LIBSCF_H
#include <libscf.h>
#include <unistd.h>
#endif /* HAVE_LIBSCF_H */


#if defined(VMS) && defined(VMS_LOCALUNIT)	/*wjm*/
#include "ntp_refclock.h"
#endif

#if defined(__FreeBSD__) && __FreeBSD__ >= 3
#include <sys/sysctl.h>
#endif

/*
 * This macro defines the authentication state. If x is 1 authentication
 * is required; othewise it is optional.
 */
#define	AUTH(x, y)	((x) ? (y) == AUTH_OK : (y) == AUTH_OK || \
			    (y) == AUTH_NONE)

/*
 * traffic shaping parameters
 */
#define	NTP_IBURST	6	/* packets in iburst */
#define	RESP_DELAY	1	/* Autokey response delay (s) */

/*
 * System variables are declared here. Unless specified otherwise, all
 * times are in seconds.
 */
u_char	sys_leap;		/* system leap indicator */
u_char	sys_stratum;		/* system stratum */
s_char	sys_precision;		/* local clock precision (log2 s) */
double	sys_rootdelay;		/* roundtrip delay to primary source */
double	sys_rootdisp;		/* dispersion to primary source */
u_int32 sys_refid;		/* reference id (network byte order) */
l_fp	sys_reftime;		/* last update time */
struct	peer *sys_peer;		/* current peer */
struct	peer *sys_pps;		/* PPS peer */
struct	peer *sys_prefer;	/* prefer peer */

/*
 * Rate controls. Leaky buckets are used to throttle the packet
 * transmission rates in order to protect busy servers such as at NIST
 * and USNO. There is a counter for each association and another for KoD
 * packets. The association counter decrements each second, but not
 * below zero. Each time a packet is sent the counter is incremented by
 * a configurable value representing the average interval between
 * packets. A packet is delayed as long as the counter is greater than
 * zero. Note this does not affect the time value computations.
 *
 * The KoD counter is initialized once each second at a designated value
 * and decrements as each KoD is sent, but not below zero. KoD packets
 * are discarded as long as the counter is zero. The default parameters
 * limit the association rate to 8 packets in 128 seconds and the KoD
 * rate to 2 packets per second.
 */
int	sys_kod;		/* kod credit */
int	sys_kod_rate = 2;	/* max KoD rate (pkt/s) */

/*
 * Nonspecified system state variables
 */
int	sys_bclient;		/* broadcast client enable */
double	sys_bdelay;		/* broadcast client default delay */
int	sys_authenticate;	/* requre authentication for config */
l_fp	sys_authdelay;		/* authentication delay */
static	double sys_offset;	/* current local clock offset */
static	u_long sys_authdly[2];	/* authentication delay shift reg */
double	sys_mindisp = MINDISPERSE; /* min disp increment (s) */
double	sys_maxdist = MAXDISTANCE; /* selection threshold */
double	sys_jitter;		/* system jitter */
u_long 	sys_epoch;		/* last clock update time */
static	int sys_hopper;		/* anticlockhop counter */
static	int sys_maxhop = MAXHOP; /* anticlockhop counter threshold */
int	leap_tai;		/* TAI at next next leap */
u_long	leap_sec;		/* next leap */
u_long	leap_expire;		/* leapfile expiration */
static int leap_next;		/* leap consensus */
keyid_t	sys_private;		/* private value for session seed */
int	sys_manycastserver;	/* respond to manycast client pkts */
int	peer_ntpdate;		/* active peers in ntpdate mode */
int	sys_survivors;		/* truest of the truechimers */

/*
 * TOS and multicast mapping stuff
 */
int	sys_floor = 0;		/* cluster stratum floor */
int	sys_ceiling = STRATUM_UNSPEC; /* cluster stratum ceiling */
int	sys_minsane = 1;	/* minimum candidates */
int	sys_minclock = NTP_MINCLOCK; /* minimum candidates */
int	sys_maxclock = NTP_MAXCLOCK; /* maximum candidates */
int	sys_cohort = 0;		/* cohort switch */
int	sys_orphan = STRATUM_UNSPEC + 1; /* orphan stratum */
int	sys_beacon = BEACON;	/* manycast beacon interval */
int	sys_ttlmax;		/* max ttl mapping vector index */
u_char	sys_ttl[MAX_TTL];	/* ttl mapping vector */

/*
 * Statistics counters - first the good, then the bad
 */
u_long	sys_stattime;		/* time since reset */
u_long	sys_received;		/* packets received */
u_long	sys_processed;		/* packets for this host */
u_long	sys_newversion;		/* current version */
u_long	sys_oldversion;		/* old version */

u_long	sys_restricted;		/* access denied */
u_long	sys_badlength;		/* bad length or format */
u_long	sys_badauth;		/* bad authentication */
u_long	sys_declined;		/* packets declined */
u_long	sys_limitrejected;	/* rate exceeded */

static	double	root_distance	(struct peer *);
static	void	clock_combine	(struct peer **, int);
static	void	peer_xmit	(struct peer *);
static	void	fast_xmit	(struct recvbuf *, int, keyid_t,
				    char *);
static	void	clock_update	(struct peer *);
static	int	default_get_precision (void);
static	int	peer_unfit	(struct peer *);


/*
 * transmit - transmit procedure called by poll timeout
 */
void
transmit(
	struct peer *peer	/* peer structure pointer */
	)
{
	int	hpoll;

	/*
	 * The polling state machine. There are two kinds of machines,
	 * those that never expect a reply (broadcast and manycast
	 * server modes) and those that do (all other modes). The dance
	 * is intricate...
	 */
	hpoll = peer->hpoll;

	/*
	 * In broadcast mode the poll interval is never changed from
	 * minpoll.
	 */
	if (peer->cast_flags & (MDF_BCAST | MDF_MCAST)) {
		peer->outdate = current_time;
		if (sys_leap != LEAP_NOTINSYNC)
			peer_xmit(peer);
		poll_update(peer, hpoll);
		return;
	}

	/*
	 * In manycast mode we start with unity ttl. The ttl is
	 * increased by one for each poll until either sys_maxclock
	 * servers have been found or the maximum ttl is reached. When
	 * sys_maxclock servers are found we stop polling until one or
	 * more servers have timed out or until less than minpoll
	 * associations turn up. In this case additional better servers
	 * are dragged in and preempt the existing ones.
	 */
	if (peer->cast_flags & MDF_ACAST) {
		peer->outdate = current_time;
		if (peer->unreach > sys_beacon) {
			peer->unreach = 0;
			peer->ttl = 0;
			peer_xmit(peer);
		} else if (sys_survivors < sys_minclock ||
		    peer_associations < sys_maxclock) {
			if (peer->ttl < sys_ttlmax)
				peer->ttl++;
			peer_xmit(peer);
		}
		peer->unreach++;
		poll_update(peer, hpoll);
		return;
	}

	/*
	 * In unicast modes the dance is much more intricate. It is
	 * desigmed to back off whenever possible to minimize network
	 * traffic.
	 */
	if (peer->burst == 0) {
		u_char oreach;

		/*
		 * Update the reachability status. If not heard for
		 * three consecutive polls, stuff infinity in the clock
		 * filter. 
		 */
		oreach = peer->reach & 0xfe;
		peer->outdate = current_time;
		peer->unreach++;
		peer->reach <<= 1;
		if (!(peer->reach & 0x0f))
			clock_filter(peer, 0., 0., MAXDISPERSE);
		if (!peer->reach) {

			/*
			 * Here the peer is unreachable. If it was
			 * previously reachable raise a trap. Send a
			 * burst if enabled.
			 */
			if (oreach) {
				report_event(EVNT_UNREACH, peer);
				peer->timereachable = current_time;
			}
			if (peer->flags & FLAG_IBURST && peer->retry ==
			    0)
				peer->retry = NTP_RETRY;
		} else {

			/*
			 * Here the peer is reachable. Send a burst if
			 * enabled and the peer is fit.
			 */
			hpoll = sys_poll;
			if (!oreach || !(peer->flags & FLAG_PREEMPT))
				peer->unreach = 0;
			if (peer->flags & FLAG_BURST && peer->retry ==
			    0 && !peer_unfit(peer))
				peer->retry = NTP_RETRY;
		}

		/*
		 * Watch for timeout. If preemptable, toss the rascal;
		 * otherwise, bump the poll interval. Note the
		 * poll_update() routine will clamp it to maxpoll.
		 */ 
		if (peer->unreach >= NTP_UNREACH) {
			if (peer->flags & FLAG_PREEMPT &&
			    sys_survivors >= sys_maxclock) {
				peer_clear(peer, "TIME");
				unpeer(peer);
				return;

			} else {
#ifdef OPENSSL
				/*
				 * If timeout in Autokey dance, restart
				 * the protocol.
				 */
				if (peer->crypto) {
					peer_clear(peer, "TIME");
					peer->unreach = 0;
				}
#endif /* OPENSSL */
				hpoll++;
			}
		}
	} else {
		peer->burst--;

		/*
		 * If a broadcast client at this point, the burst has
		 * concluded, so we switch to client mode and purge the
		 * keylist, since no further transmissions will be made.
		 */
		if (peer->burst == 0) {
			if (peer->cast_flags & MDF_BCLNT) {
				peer->hmode = MODE_BCLIENT;
#ifdef OPENSSL
				key_expire(peer);
#endif /* OPENSSL */
			}

			/*
			 * If ntpdate mode and the clock has not been
			 * set and all peers have completed the burst,
			 * we declare a successful failure.
			 */
			if (mode_ntpdate) {
				peer_ntpdate--;
				if (peer_ntpdate == 0) {
					msyslog(LOG_NOTICE,
					    "ntpdate: no reply; clock not set");
					exit (0);
				}
			}
		}
	}
	if (peer->retry > 0)
		peer->retry--;

	/*
	 * Do not transmit if in broadcast client mode. 
	 */
	if (peer->hmode != MODE_BCLIENT)
		peer_xmit(peer);
	poll_update(peer, hpoll);
}


/*
 * receive - receive procedure called for each packet received
 */
void
receive(
	struct recvbuf *rbufp
	)
{
	register struct peer *peer;	/* peer structure pointer */
	register struct pkt *pkt;	/* receive packet pointer */
	int	hisversion;		/* packet version */
	int	hisleap;		/* packet leap indicator */
	int	hismode;		/* packet mode */
	int	hisstratum;		/* packet stratum */
	int	restrict_mask;		/* restrict bits */
	int	has_mac;		/* length of MAC field */
	int	authlen;		/* offset of MAC field */
	int	is_authentic = 0;	/* cryptosum ok */
	keyid_t	skeyid = 0;		/* key ID */
	struct sockaddr_storage *dstadr_sin; /* active runway */
	struct peer *peer2;		/* aux peer structure pointer */
	l_fp	p_org;			/* origin timestamp */
	l_fp	p_rec;			/* receive timestamp */
	l_fp	p_xmt;			/* transmit timestamp */
#ifdef OPENSSL
	keyid_t tkeyid = 0;		/* temporary key ID */
	keyid_t	pkeyid = 0;		/* previous key ID */
	struct autokey *ap;		/* autokey structure pointer */
	int	rval;			/* cookie snatcher */
#endif /* OPENSSL */
	int retcode = AM_NOMATCH;

	/*
	 * Monitor the packet and get restrictions. Note that the packet
	 * length for control and private mode packets must be checked
	 * by the service routines. Note that no statistics counters are
	 * recorded for restrict violations, since these counters are in
	 * the restriction routine. Note the careful distinctions here
	 * between a packet with a format error and a packet that is
	 * simply discarded without prejudice. Some restrictions have to
	 * be handled later in order to generate a kiss-o'-death packet.
	 */
	/*
	 * Bogus port check is before anything, since it probably
	 * reveals a clogging attack.
	 */
	sys_received++;
	if (SRCPORT(&rbufp->recv_srcadr) == 0) {
		sys_badlength++;
		return;				/* bogus port */
	}
	restrict_mask = restrictions(&rbufp->recv_srcadr);
	restrict_mask = ntp_monitor(rbufp, restrict_mask);
#ifdef DEBUG
	if (debug > 1)
		printf("receive: at %ld %s<-%s flags %x restrict %03x\n",
		    current_time, stoa(&rbufp->dstadr->sin),
		    stoa(&rbufp->recv_srcadr),
		    rbufp->dstadr->flags, restrict_mask);
#endif
	pkt = &rbufp->recv_pkt;
	hisversion = PKT_VERSION(pkt->li_vn_mode);
	hisleap = PKT_LEAP(pkt->li_vn_mode);
	hismode = (int)PKT_MODE(pkt->li_vn_mode);
	hisstratum = PKT_TO_STRATUM(pkt->stratum);
	if (restrict_mask & RES_IGNORE) {
		sys_restricted++;
		return;				/* ignore everything */
	}
	if (hismode == MODE_PRIVATE) {
		if (restrict_mask & RES_NOQUERY) {
			sys_restricted++;
			return;			/* no query private */
		}
		process_private(rbufp, ((restrict_mask &
		    RES_NOMODIFY) == 0));
		return;
	}
	if (hismode == MODE_CONTROL) {
		if (restrict_mask & RES_NOQUERY) {
			sys_restricted++;
			return;			/* no query control */
		}
		process_control(rbufp, restrict_mask);
		return;
	}
	if (restrict_mask & RES_DONTSERVE) {
		sys_restricted++;
		return;				/* no time */
	}
	if (rbufp->recv_length < LEN_PKT_NOMAC) {
		sys_badlength++;
		return;				/* runt packet */
	}
	
	/*
	 * Version check must be after the query packets, since they
	 * intentionally use early version.
	 */
	if (hisversion == NTP_VERSION) {
		sys_newversion++;		/* new version */
	} else if (!(restrict_mask & RES_VERSION) && hisversion >=
	    NTP_OLDVERSION) {
		sys_oldversion++;		/* previous version */
	} else {
		sys_badlength++;
		return;				/* old version */
	}

	/*
	 * Figure out his mode and validate the packet. This has some
	 * legacy raunch that probably should be removed. In very early
	 * NTP versions mode 0 was equivalent to what later versions
	 * would interpret as client mode.
	 */
	if (hismode == MODE_UNSPEC) {
		if (hisversion == NTP_OLDVERSION) {
			hismode = MODE_CLIENT;
		} else {
			sys_badlength++;
			return;                 /* invalid mode */
		}
	}

	/*
	 * Parse the extension field if present. We figure out whether
	 * an extension field is present by measuring the MAC size. If
	 * the number of words following the packet header is 0, no MAC
	 * is present and the packet is not authenticated. If 1, the
	 * packet is a crypto-NAK; if 3, the packet is authenticated
	 * with DES; if 5, the packet is authenticated with MD5. If 2 or
	 * 4, the packet is a runt and discarded forthwith. If greater
	 * than 5, an extension field is present, so we subtract the
	 * length of the field and go around again.
	 */
	authlen = LEN_PKT_NOMAC;
	has_mac = rbufp->recv_length - authlen;
	while (has_mac > 0) {
		int temp;

		if (has_mac % 4 != 0 || has_mac < 0) {
			sys_badlength++;
			return;			/* bad MAC length */
		}
		if (has_mac == 1 * 4 || has_mac == 3 * 4 || has_mac ==
		    MAX_MAC_LEN) {
			skeyid = ntohl(((u_int32 *)pkt)[authlen / 4]);
			break;

		} else if (has_mac > MAX_MAC_LEN) {
			temp = ntohl(((u_int32 *)pkt)[authlen / 4]) &
			    0xffff;
			if (temp < 4 || temp > NTP_MAXEXTEN || temp % 4
			    != 0) {
				sys_badlength++;
				return;		/* bad MAC length */
			}
			authlen += temp;
			has_mac -= temp;
		} else {
			sys_badlength++;
			return;			/* bad MAC length */
		}
	}
#ifdef OPENSSL
	pkeyid = tkeyid = 0;
#endif /* OPENSSL */

	/*
	 * We have tossed out as many buggy packets as possible early in
	 * the game to reduce the exposure to a clogging attack. Now we
	 * have to burn some cycles to find the association and
	 * authenticate the packet if required. Note that we burn only
	 * MD5 cycles, again to reduce exposure. There may be no
	 * matching association and that's okay.
	 *
	 * More on the autokey mambo. Normally the local interface is
	 * found when the association was mobilized with respect to a
	 * designated remote address. We assume packets arriving from
	 * the remote address arrive via this interface and the local
	 * address used to construct the autokey is the unicast address
	 * of the interface. However, if the sender is a broadcaster,
	 * the interface broadcast address is used instead.
	 & Notwithstanding this technobabble, if the sender is a
	 * multicaster, the broadcast address is null, so we use the
	 * unicast address anyway. Don't ask.
	 */
	peer = findpeer(&rbufp->recv_srcadr, rbufp->dstadr,  hismode,
	    &retcode);
	dstadr_sin = &rbufp->dstadr->sin;
	NTOHL_FP(&pkt->org, &p_org);
	NTOHL_FP(&pkt->rec, &p_rec);
	NTOHL_FP(&pkt->xmt, &p_xmt);

	/*
	 * Authentication is conditioned by three switches:
	 *
	 * NOPEER  (RES_NOPEER) do not mobilize an association unless
	 *         authenticated
	 * NOTRUST (RES_DONTTRUST) do not allow access unless
	 *         authenticated (implies NOPEER)
	 * enable  (sys_authenticate) master NOPEER switch, by default
	 *         on
	 *
	 * The NOPEER and NOTRUST can be specified on a per-client basis
	 * using the restrict command. The enable switch if on implies
	 * NOPEER for all clients. There are four outcomes:
	 *
	 * NONE    The packet has no MAC.
	 * OK      the packet has a MAC and authentication succeeds
	 * ERROR   the packet has a MAC and authentication fails
	 * CRYPTO  crypto-NAK. The MAC has four octets only.
	 *
	 * Note: The AUTH(x, y) macro is used to filter outcomes. If x
	 * is zero, acceptable outcomes of y are NONE and OK. If x is
	 * one, the only acceptable outcome of y is OK.
	 */
	if (has_mac == 0) {
		is_authentic = AUTH_NONE; /* not required */
#ifdef DEBUG
		if (debug)
			printf(
			    "receive: at %ld %s<-%s mode %d len %d\n",
			    current_time, stoa(dstadr_sin),
			    stoa(&rbufp->recv_srcadr), hismode,
			    authlen);
#endif
	} else if (has_mac == 4) {
			is_authentic = AUTH_CRYPTO; /* crypto-NAK */
#ifdef DEBUG
		if (debug)
			printf(
			    "receive: at %ld %s<-%s mode %d keyid %08x len %d auth %d\n",
			    current_time, stoa(dstadr_sin),
			    stoa(&rbufp->recv_srcadr), hismode, skeyid,
			    authlen + has_mac, is_authentic);
#endif
	} else {
#ifdef OPENSSL
		/*
		 * For autokey modes, generate the session key
		 * and install in the key cache. Use the socket
		 * broadcast or unicast address as appropriate.
		 */
		if (skeyid > NTP_MAXKEY) {
		
			/*
			 * More on the autokey dance (AKD). A cookie is
			 * constructed from public and private values.
			 * For broadcast packets, the cookie is public
			 * (zero). For packets that match no
			 * association, the cookie is hashed from the
			 * addresses and private value. For server
			 * packets, the cookie was previously obtained
			 * from the server. For symmetric modes, the
			 * cookie was previously constructed using an
			 * agreement protocol; however, should PKI be
			 * unavailable, we construct a fake agreement as
			 * the EXOR of the peer and host cookies.
			 *
			 * hismode	ephemeral	persistent
			 * =======================================
			 * active	0		cookie#
			 * passive	0%		cookie#
			 * client	sys cookie	0%
			 * server	0%		sys cookie
			 * broadcast	0		0
			 *
			 * # if unsync, 0
			 * % can't happen
			 */
			if (hismode == MODE_BROADCAST) {

				/*
				 * For broadcaster, use the interface
				 * broadcast address when available;
				 * otherwise, use the unicast address
				 * found when the association was
				 * mobilized. However, if this is from
				 * the wildcard interface, game over.
				 */
				if (crypto_flags && rbufp->dstadr ==
				    any_interface) {
					sys_restricted++;
					return;	     /* no wildcard */
				}
				pkeyid = 0;
				if (!SOCKNUL(&rbufp->dstadr->bcast))
					dstadr_sin =
					    &rbufp->dstadr->bcast;
			} else if (peer == NULL) {
				pkeyid = session_key(
				    &rbufp->recv_srcadr, dstadr_sin, 0,
				    sys_private, 0);
			} else {
				pkeyid = peer->pcookie;
			}

			/*
			 * The session key includes both the public
			 * values and cookie. In case of an extension
			 * field, the cookie used for authentication
			 * purposes is zero. Note the hash is saved for
			 * use later in the autokey mambo.
			 */
			if (authlen > LEN_PKT_NOMAC && pkeyid != 0) {
				session_key(&rbufp->recv_srcadr,
				    dstadr_sin, skeyid, 0, 2);
				tkeyid = session_key(
				    &rbufp->recv_srcadr, dstadr_sin,
				    skeyid, pkeyid, 0);
			} else {
				tkeyid = session_key(
				    &rbufp->recv_srcadr, dstadr_sin,
				    skeyid, pkeyid, 2);
			}

		}
#endif /* OPENSSL */

		/*
		 * Compute the cryptosum. Note a clogging attack may
		 * succeed in bloating the key cache. If an autokey,
		 * purge it immediately, since we won't be needing it
		 * again. If the packet is authentic, it can mobilize an
		 * association. Note that there is no key zero.
		 */
		if (!authdecrypt(skeyid, (u_int32 *)pkt, authlen,
		    has_mac)) {
			is_authentic = AUTH_ERROR;
			sys_badauth++;
		} else {
			is_authentic = AUTH_OK;
		}
#ifdef OPENSSL
		if (skeyid > NTP_MAXKEY)
			authtrust(skeyid, 0);
#endif /* OPENSSL */
#ifdef DEBUG
		if (debug)
			printf(
			    "receive: at %ld %s<-%s mode %d keyid %08x len %d auth %d\n",
			    current_time, stoa(dstadr_sin),
			    stoa(&rbufp->recv_srcadr), hismode, skeyid,
			    authlen + has_mac, is_authentic);
#endif
	}

	/*
	 * The association matching rules are implemented by a set of
	 * routines and an association table. A packet matching an
	 * association is processed by the peer process for that
	 * association. If there are no errors, an ephemeral association
	 * is mobilized: a broadcast packet mobilizes a broadcast client
	 * aassociation; a manycast server packet mobilizes a manycast
	 * client association; a symmetric active packet mobilizes a
	 * symmetric passive association.
	 */
	switch (retcode) {

	/*
	 * This is a client mode packet not matching any association. If
	 * an ordinary client, simply toss a server mode packet back
	 * over the fence. If a manycast client, we have to work a
	 * little harder.
	 */
	case AM_FXMIT:

		/*
		 * The vanilla case is when this is not a multicast
		 * interface. If authentication succeeds, return a
		 * server mode packet; if not and the key ID is nonzero,
		 * return a crypto-NAK.
		 */
		if (restrict_mask & RES_LIMITED) {
			sys_limitrejected++;
			if (restrict_mask & RES_DEMOBILIZE)
				fast_xmit(rbufp, hismode, skeyid,
				    "RATE");
			return;
		}
		if (!(rbufp->dstadr->flags & INT_MCASTOPEN)) {
			if (AUTH(restrict_mask & RES_DONTTRUST,
			   is_authentic))
				fast_xmit(rbufp, hismode, skeyid, NULL);
			else if (is_authentic == AUTH_ERROR)
				fast_xmit(rbufp, hismode, 0, NULL);
			return;			/* hooray */
		}

		/*
		 * This must be manycast. Do not respond if not
		 * configured as a manycast server.
		 */
		if (!sys_manycastserver) {
			sys_restricted++;
			return;			/* not enabled */
		}

		/*
		 * Do not respond if we have no system peer or our
		 * stratum is greater than the manycaster or the
		 * manycaster has already synchronized to us.
		 */
		if (sys_peer == NULL || sys_stratum >= hisstratum ||
		    (!sys_cohort && sys_stratum == hisstratum + 1) ||
		    rbufp->dstadr->addr_refid == pkt->refid) {
			sys_declined++;
			return;			/* no help */
		}

		/*
		 * Respond only if authentication succeeds. Don't do a
		 * crypto-NAK, as that would not be useful.
		 */
		if (AUTH(restrict_mask & RES_DONTTRUST, is_authentic))
			fast_xmit(rbufp, hismode, skeyid, NULL);

		return;				/* hooray */

	/*
	 * This is a server mode packet returned in response to a client
	 * mode packet sent to a multicast group address. The origin
	 * timestamp is a good nonce to reliably associate the reply
	 * with what was sent. If there is no match, that's curious and
	 * could be an intruder attempting to clog, so we just ignore
	 * it.
	 *
	 * If the packet is authentic and the manycast association is
	 * found, we mobilize a client association and copy pertinent
	 * variables from the manycast association to the new client
	 * association. If not, just ignore the packet.
	 *
	 * There is an implosion hazard at the manycast client, since
	 * the manycast servers send the server packet immediately. If
	 * the guy is already here, don't fire up a duplicate.
	 */
	case AM_MANYCAST:
		if (!AUTH(sys_authenticate | (restrict_mask &
		    (RES_NOPEER | RES_DONTTRUST)), is_authentic)) {
			sys_restricted++;
			return;			/* access denied */
		}

		/*
		 * Do not respond if unsynchronized or stratum is below
		 * the floor or at or above the ceiling.
		 */
		if (hisleap == LEAP_NOTINSYNC || hisstratum <
		    sys_floor || hisstratum >= sys_ceiling) {
			sys_declined++;
			return;			/* no helpm */
		}
		if ((peer2 = findmanycastpeer(rbufp)) == NULL) {
			sys_restricted++;
			return;			/* not enabled */
		}
		if ((peer = newpeer(&rbufp->recv_srcadr,
		    rbufp->dstadr, MODE_CLIENT,
		    hisversion, NTP_MINDPOLL, NTP_MAXDPOLL,
		    FLAG_IBURST | FLAG_PREEMPT, MDF_UCAST | MDF_ACLNT,
		    0, skeyid)) == NULL) {
			peer->oldpkt++;
			return;			/* ignore duplicate  */
		}

		/*
		 * We don't need these, but it warms the billboards.
		 */
		peer->ttl = peer2->ttl;
		break;

	/*
	 * This is the first packet received from a broadcast server. If
	 * the packet is authentic and we are enabled as broadcast
	 * client, mobilize a broadcast client association. We don't
	 * kiss any frogs here.
	 */
	case AM_NEWBCL:
		if (!AUTH(sys_authenticate | (restrict_mask &
		    (RES_NOPEER | RES_DONTTRUST)), is_authentic)) {
			sys_restricted++;
			return;			/* access denied */
		}

		/*
		 * Do not respond if unsynchronized or stratum is below
		 * the floor or at or above the ceiling.
		 */
		if (hisleap == LEAP_NOTINSYNC || hisstratum <
		    sys_floor || hisstratum >= sys_ceiling) {
			sys_declined++;
			return;			/* no help */
		}

		/*
		 * Do not respond if we have a system peer and our
		 * stratum is not greater than the broadcaster or the
		 * broadcaster has already synchronized to us.
		 */
		if ((sys_peer != NULL && sys_stratum <= hisstratum) ||
		    rbufp->dstadr->addr_refid == pkt->refid) {
			sys_declined++;
			return;			/* no help */
		}

		/*
		 * If not enabled, just skedaddle.
		 */
		if (sys_bclient == 0) {
			sys_restricted++;
			return;			/* not enabled */

		/*
		 * Do not execute the initial volley.
		 */
		} else if (sys_bdelay != 0 || sys_bclient > 1) {
#ifdef OPENSSL
			/*
			 * If a two-way exchange is not possible,
			 * neither is Autokey.
			 */
			if (skeyid > NTP_MAXKEY) {
				msyslog(LOG_INFO,
				    "receive: autokey requires two-way communication");
				sys_restricted++;
				return;		/* no autokey */
			}
#endif /* OPENSSL */
			if ((peer = newpeer(&rbufp->recv_srcadr,
			    rbufp->dstadr, MODE_BCLIENT, hisversion,
			    pkt->ppoll, NTP_MAXDPOLL, 0, MDF_BCLNT, 0,
			    skeyid)) == NULL) {
				peer->oldpkt++;
				return;		/* ignore duplicate */
			}

		/*
		 * Execute the initial volley in order to calibrate the
		 * propagation delay and run the Autokey protocol.
		 *
		 * Note that the minpoll is taken from the broadcast
		 * packet, normally 6 (64 s).
		 */
		} else {
			if ((peer = newpeer(&rbufp->recv_srcadr,
			    rbufp->dstadr, MODE_CLIENT, hisversion,
			    pkt->ppoll, NTP_MAXDPOLL, FLAG_MCAST |
			    FLAG_IBURST | FLAG_PREEMPT, MDF_BCLNT, 0,
			    skeyid)) == NULL) {
				peer->oldpkt++;
				return;		/* ignore duplicate */
			}
#ifdef OPENSSL
			/*
			 * Ordinarily this will be an association reply
			 * message which initializes the dance. If an
			 * autokey reply message, it is ignored. Any
			 * other message is a protocol error leaving the
			 * asssociation ready for a following
			 * association reply message.
			 */
			if (skeyid > NTP_MAXKEY)
				crypto_recv(peer, rbufp);
#endif /* OPENSSL */
			return;			/* hooray */
		}
		break;

	/*
	 * This is the first packet received from a symmetric active
	 * peer. If the packet is authentic and the first he sent,
	 * mobilize a passive association. If not, kiss the frog.
	 */
	case AM_NEWPASS:
		if (!AUTH(sys_authenticate | (restrict_mask &
		    (RES_NOPEER | RES_DONTTRUST)), is_authentic)) {
			fast_xmit(rbufp, hismode, 0, NULL);
			sys_restricted++;
			return;			/* access denied */
		}

		/*
		 * Do not respond if unsynchronized or stratum is below
		 * the floor or at or above the ceiling.
		 */
		if (hisleap == LEAP_NOTINSYNC || hisstratum <
		    sys_floor || hisstratum >= sys_ceiling) {
			sys_declined++;
			return;			/* no help */
		}

		/*
		 * If correctly authenticated but not enabled, send a
		 * summetric active response without mobilizing
		 * an association.
		 */
		if (!AUTH(sys_authenticate | (restrict_mask &
		    RES_NOPEER), is_authentic)) {
			fast_xmit(rbufp, hismode, skeyid, NULL);
			return;			/* hooray */
		}

		/*
		 * The message is correctly authenticated and
		 * enabled. Mobiliae a symmetric passive association.
		 */
		if ((peer = newpeer(&rbufp->recv_srcadr,
		    rbufp->dstadr, MODE_PASSIVE, hisversion,
		    NTP_MINDPOLL, NTP_MAXDPOLL, FLAG_PREEMPT, MDF_UCAST,
		    0, skeyid)) == NULL) {
			peer->oldpkt++;
			return;			/* ignore duplicate */
		}
		break;

	/*
	 * Process regular packet. Nothing special.
	 */
	case AM_PROCPKT:
		break;

	/*
	 * A passive packet matches a passive association. This is
	 * usually the result of reconfiguring a client on the fly. As
	 * this association might be legitamate and this packet an
	 * attempt to deny service, just ignore it.
	 */
	case AM_ERR:
		sys_declined++;
		return;

	/*
	 * For everything else there is the bit bucket.
	 */
	default:
		sys_declined++;
		return;
	}
	peer->received++;
	peer->flash &= ~PKT_TEST_MASK;

	/*
	 * Next comes a rigorous schedule of timestamp checking. If the
	 * transmit timestamp is zero, the server is horribly broken.
	 */
	if (L_ISZERO(&p_xmt)) {
		peer->flash |= TEST3;		/* protocol unsynch */
		peer->bogusorg++;

	/*
	 * If the transmit timestamp duplicates a previous one, the
	 * packet is a replay. This prevents the bad guys from replaying
	 * the most recent packet, authenticated or not.
	 */
	} else if (L_ISEQU(&peer->org, &p_xmt)) {
		peer->flash |= TEST1;		/* duplicate packet */
		peer->oldpkt++;

	/*
	 * If this is a broadcast mode packet, skip further checking.
	 */
	} else if (hismode != MODE_BROADCAST) {
		if (L_ISZERO(&p_org)) {
			peer->flash |= TEST3;	/* protocol unsynch */
			peer->seldisptoolarge++;
		} else if (!L_ISEQU(&p_org, &peer->xmt)) {
			peer->flash |= TEST2;	/* bogus packet */
			peer->bogusorg++;
		}
	}

	/*
	 * If this is a client association and the flashers are not
	 * dark, drop the packet forthwith.
	 */
	if (peer->hmode == MODE_CLIENT && (peer->flash &
	     PKT_TEST_MASK)) {
		return;

	/*
	 * If this is a crypto_NAK and the flashers are dark, the server
	 * might have just changed keys. Reset the association and
	 * restart the protocol.
	 */
	} else if (is_authentic == AUTH_CRYPTO && !(peer->flash &
	    PKT_TEST_MASK)) {
		peer_clear(peer, "AUTH");
		return;

	/* 
	 * If the packet is authenticated or the server not trusted,
	 * received packets must be correctly authenticated. This is
	 * designed to avoid a bait-and-switch attack, which was
	 * possible in past versions. If symmetric modes, return a
	 * crypto-NAK. The peer should restart the protocol.
	 */
	} else if (!AUTH(has_mac || (restrict_mask & RES_DONTTRUST),
	    is_authentic)) {
		peer->flash |= TEST5;
		if (hismode == MODE_ACTIVE || hismode == MODE_PASSIVE)
			fast_xmit(rbufp, hismode, 0, NULL);
		sys_restricted++;
		return;				/* access denied */
	}

	/*
	 * Update the timestamps. If a flash is lit, abandon ship. 
	 */
	peer->org = p_xmt;
	peer->rec = rbufp->recv_time;
	L_CLR(&peer->xmt);			/* avoid replay */
	if (peer->flash & PKT_TEST_MASK)
		return;				/* Davy Jones */

	/*
	 * Test for kiss-o'death packet. We carefully avoid a hazard
	 * when a terrorist broadcaster kisses the frog.
	 */
	if (hismode == MODE_SERVER && hisleap == LEAP_NOTINSYNC &&
	    hisstratum == STRATUM_UNSPEC) {
		if (memcmp(&pkt->refid, "RATE", 4) == 0) {
			peer_clear(peer, "RATE");
			peer->flash |= TEST4;	/* rate quench */
			msyslog(LOG_INFO,
			    "receive: server %s maximum rate exceeded",
			    stoa(&rbufp->recv_srcadr));
			return;
		}
	}

	/*
	 * That was hard and I am sweaty, but the packet is squeaky
	 * clean. Get on with real work.
	 */
	peer->timereceived = current_time;
	if (is_authentic == AUTH_OK)
		peer->flags |= FLAG_AUTHENTIC;
	else
		peer->flags &= ~FLAG_AUTHENTIC;

#ifdef OPENSSL
	/*
	 * More autokey dance. The rules of the cha-cha are as follows:
	 *
	 * 1. If there is no key or the key is not auto, do nothing.
	 *
	 * 2. If this packet is in response to the one just previously
	 *    sent or from a broadcast server, do the extension fields.
	 *    Otherwise, assume bogosity and bail out.
	 *
	 * 3. If an extension field contains a verified signature, it is
	 *    self-authenticated and we sit the dance.
	 *
	 * 4. If this is a server reply, check only to see that the
	 *    transmitted key ID matches the received key ID.
	 *
	 * 5. Check to see that one or more hashes of the current key ID
	 *    matches the previous key ID or ultimate original key ID
	 *    obtained from the broadcaster or symmetric peer. If no
	 *    match, sit the dance and wait for timeout.
	 *
	 * In case of crypto error, fire the orchestra and stop dancing.
	 * This is considered a permanant error, so light the crypto bit
	 * to suppress further requests.
	 */
	if (crypto_flags && (peer->flags & FLAG_SKEY)) {
		peer->flash |= TEST8;
		rval = crypto_recv(peer, rbufp);
		if (rval != XEVNT_OK) {
			peer_clear(peer, "CRYP");
			peer->flash |= TEST9;	/* crypto error */
			return;

		} else if (hismode == MODE_SERVER) {
			if (skeyid == peer->keyid)
				peer->flash &= ~TEST8;
		} else if (!(peer->flash & TEST8)) {
			peer->pkeyid = skeyid;
		} else if ((ap = (struct autokey *)peer->recval.ptr) !=
		    NULL) {
			int i;

			for (i = 0; ; i++) {
				if (tkeyid == peer->pkeyid ||
				    tkeyid == ap->key) {
					peer->flash &= ~TEST8;
					peer->pkeyid = skeyid;
					break;
				}
				if (i > ap->seq)
					break;

				tkeyid = session_key(
				    &rbufp->recv_srcadr, dstadr_sin,
				    tkeyid, pkeyid, 0);
			}
		}
		if (!(peer->crypto & CRYPTO_FLAG_PROV)) /* test 9 */
			peer->flash |= TEST8;	/* not proventic */

		/*
		 * If the transmit queue is nonempty, clamp the host
		 * poll interval to the packet poll interval.
		 */
		if (peer->cmmd != 0) {
			peer->ppoll = pkt->ppoll;
			poll_update(peer, peer->hpoll);
		}
	}
#endif /* OPENSSL */

	/*
	 * The dance is complete and the flash bits have been lit. Toss
	 * the packet over the fence for processing, which may light up
	 * more flashers.
	 */
	process_packet(peer, pkt, rbufp->recv_length);
}


/*
 * process_packet - Packet Procedure, a la Section 3.4.4 of the
 *	specification. Or almost, at least. If we're in here we have a
 *	reasonable expectation that we will be having a long term
 *	relationship with this host.
 */
void
process_packet(
	register struct peer *peer,
	register struct pkt *pkt,
	u_int	len
	)
{
	double	t34, t21;
	double	p_offset, p_del, p_disp;
	l_fp	p_rec, p_xmt, p_org, p_reftime, ci;
	u_char	pmode, pleap, pstratum;
	double	etemp, ftemp;	/* experimental */
	int	itemp;
	double	td;

	sys_processed++;
	peer->processed++;
	p_del = FPTOD(NTOHS_FP(pkt->rootdelay));
	p_disp = FPTOD(NTOHS_FP(pkt->rootdisp));
	NTOHL_FP(&pkt->reftime, &p_reftime);
	NTOHL_FP(&pkt->rec, &p_rec);
	NTOHL_FP(&pkt->xmt, &p_xmt);
	pmode = PKT_MODE(pkt->li_vn_mode);
	pleap = PKT_LEAP(pkt->li_vn_mode);
	if (pmode != MODE_BROADCAST)
		NTOHL_FP(&pkt->org, &p_org);
	else
		p_org = peer->rec;
	pstratum = PKT_TO_STRATUM(pkt->stratum);

	/*
	 * Capture the header values.
	 */
	record_raw_stats(&peer->srcadr, peer->dstadr ?
	    &peer->dstadr->sin : NULL, &p_org, &p_rec, &p_xmt,
	    &peer->rec);
	peer->leap = pleap;
	peer->stratum = min(pstratum, STRATUM_UNSPEC);
	peer->pmode = pmode;
	peer->ppoll = pkt->ppoll;
	peer->precision = pkt->precision;
	peer->rootdelay = p_del;
	peer->rootdisp = p_disp;
	peer->refid = pkt->refid;		/* network byte order */
	peer->reftime = p_reftime;

	/*
	 * Verify the server is synchronized; that is, the leap bits and
	 * stratum are valid, the root delay and root dispersion are
	 * valid and the reference timestamp is not later than the
	 * transmit timestamp.
	 */
	if (pleap == LEAP_NOTINSYNC ||		/* test 6 */
	    pstratum < sys_floor || pstratum >= sys_ceiling)
		peer->flash |= TEST6;		/* peer not synch */
	if (p_del / 2 + p_disp >=		/* test 7 */
	    MAXDISPERSE || !L_ISHIS(&p_xmt, &p_reftime))
		peer->flash |= TEST7;		/* bad header */

	/*
	 * If any tests fail at this point, the packet is discarded.
	 * Note that some flashers may have already been set in the
	 * receive() routine.
	 */
	if (peer->flash & PKT_TEST_MASK) {
#ifdef DEBUG
		if (debug)
			printf("packet: flash header %04x\n",
			    peer->flash);
#endif
		return;
	}

	/*
	 * If the peer was previously unreachable, raise a trap. If a
	 * burst mode is active, initialize the burst. The unreachable
	 * burst is always NTP_IBURST (6) packets; the reachable burst
	 * is tailored not to exceed the minimum average headway of 16
	 * s.
	 */ 
	if (!peer->reach) {
		report_event(EVNT_REACH, peer);
		peer->timereachable = current_time;
		if (peer->retry > 0) {
			peer->retry = 0;
			peer->burst = NTP_IBURST - 1;
			peer->nextdate = peer->outdate +
			    res_min_interval;
		}
	} else {
		if (peer->retry > 0) {
			peer->retry = 0;
			peer->burst = min(1 << (peer->hpoll -
			    NTP_MINPOLL), NTP_SHIFT) - 1;
			if (peer->burst > 0)
				peer->nextdate = peer->outdate +
				    res_min_interval;
		}
	}
	peer->reach |= 1;
	poll_update(peer, peer->hpoll);

	/*
	 * For a client/server association, calculate the clock offset,
	 * roundtrip delay and dispersion. The equations are reordered
	 * from the spec for more efficient use of temporaries. For a
	 * broadcast association, offset the last measurement by the
	 * computed delay during the client/server volley. Note that
	 * org has been set to the time of last reception. Note the
	 * computation of dispersion includes the system precision plus
	 * that due to the frequency error since the origin time.
	 *
	 * It is very important to respect the hazards of overflow. The
	 * only permitted operation on raw timestamps is subtraction,
	 * where the result is a signed quantity spanning from 68 years
	 * in the past to 68 years in the future. To avoid loss of
	 * precision, these calculations are done using 64-bit integer
	 * arithmetic. However, the offset and delay calculations are
	 * sums and differences of these first-order differences, which
	 * if done using 64-bit integer arithmetic, would be valid over
	 * only half that span. Since the typical first-order
	 * differences are usually very small, they are converted to 64-
	 * bit doubles and all remaining calculations done in floating-
	 * point arithmetic. This preserves the accuracy while retaining
	 * the 68-year span.
	 *
	 * Let t1 = p_org, t2 = p_rec, t3 = p_xmt, t4 = peer->rec:
	 */
	ci = p_xmt;			/* t3 - t4 */
	L_SUB(&ci, &peer->rec);
	LFPTOD(&ci, t34);
	ci = p_rec;			/* t2 - t1 */
	L_SUB(&ci, &p_org);
	LFPTOD(&ci, t21);
	ci = peer->rec;			/* t4 - t1 */
	L_SUB(&ci, &p_org);

	/*
	 * This code calculates the outbound and inbound data rates by
	 * measuring the differences between timestamps at different
	 * packet lengths. This is helpful in cases of large asymmetric
	 * delays commonly experienced on deep space communication
	 * links.
	 */
	if (peer->t21_last > 0 && peer->t34_bytes > 0) {
		itemp = peer->t21_bytes - peer->t21_last;
		if (itemp > 25) {
			etemp = t21 - peer->t21;
			if (fabs(etemp) > 1e-6) {
				ftemp = itemp / etemp;
				if (ftemp > 1000.)
					peer->r21 = ftemp;
			}
		}
		itemp = len - peer->t34_bytes;
		if (itemp > 25) {
			etemp = -t34 - peer->t34;
			if (fabs(etemp) > 1e-6) {
				ftemp = itemp / etemp;
				if (ftemp > 1000.)
					peer->r34 = ftemp;
			}
		}
	}
	peer->t21 = t21;
	peer->t21_last = peer->t21_bytes;
	peer->t34 = -t34;
	peer->t34_bytes = len;
	p_del = fabs(t21 - t34);
#ifdef DEBUG
	if (debug > 1)
		printf("proto: t21 %.9lf %d t34 %.9lf %d\n", peer->t21,
		    peer->t21_bytes, peer->t34, peer->t34_bytes);
#endif

	/*
	 * The following section compensates for different data rates on
	 * the outbound (d21) and inbound (t34) directions. To do this,
	 * it finds t such that r21 * t - r34 * (d - t) = 0, where d is
	 * the roundtrip delay. Then it calculates the correction as a
	 * fraction of d.
	 */
	if (peer->r21 > 0 && peer->r34 > 0 && p_del > 0) {
		if (pmode != MODE_BROADCAST)
			td = (peer->r34 / (peer->r21 + peer->r34) -
			    .5) * p_del;
		else
			td = 0;
#if 0 /* temporarily disabled */
		t21 -= td;
		t34 -= td;
#endif
#ifdef DEBUG
		if (debug > 2)
			printf("proto: del %.6lf r21 %.1lf r34 %.1lf %.6lf\n",
			    p_del, peer->r21 / 1e3, peer->r34 / 1e3,
			    td);
#endif
	} 

	/*
	 * If running in a broadcast association, the clock offset is
	 * (t3 - t4) corrected by the one-way delay, but we can't
	 * measure that directly. Therefore, we start up in MODE_CLIENT
	 * mode, set FLAG_MCAST and exchange eight messages to determine
	 * the clock offset. When the last message is sent, we switch to
	 * MODE_BCLIENT mode. The next broadcast message after that
	 * computes the broadcast offset and clears FLAG_MCAST.
	 */
	if (pmode == MODE_BROADCAST) {
		p_offset = t34;
		if (peer->flags & FLAG_MCAST) {
			peer->estbdelay = peer->offset - p_offset;
			if (peer->hmode == MODE_CLIENT)
				return;

			peer->flags &= ~(FLAG_MCAST | FLAG_BURST);
		}
		p_offset += peer->estbdelay;
		p_del = peer->delay;
	} else {
		p_offset = (t21 + t34) / 2.;
	}
	p_disp = LOGTOD(sys_precision) + LOGTOD(peer->precision) +
	    clock_phi * p_del;
	clock_filter(peer, p_offset, p_del, p_disp);
}


/*
 * clock_update - Called at system process update intervals.
 */
static void
clock_update(
	struct peer *peer	/* peer structure pointer */
	)
{
	u_char	ostratum;
	double	dtemp;
	l_fp	now;
	u_long	epoch;
#ifdef HAVE_LIBSCF_H
	char	*fmri;
#endif /* HAVE_LIBSCF_H */

	/*
	 * There must be a system peer at this point. If we just changed
	 * the system peer, but have a newer sample from the old one,
	 * wait until newer data are available. Note that the root
	 * dispersion is updated even if the offset has already been
	 * processed.
	 */
	dtemp = peer->disp + sys_jitter + fabs(sys_offset) + clock_phi *
	    (current_time - peer->update);
#ifdef REFCLOCK
	if (!(peer->flags & FLAG_REFCLOCK) && sys_rootdisp <
	    sys_mindisp)
		dtemp = sys_mindisp;
#else
	if (sys_rootdisp < sys_mindisp)
		dtemp = sys_mindisp;
#endif /* REFCLOCK */
	sys_rootdisp = dtemp + peer->rootdisp;
	epoch = peer->epoch - sys_epoch;
	if (epoch <= 0)
		return;

#ifdef DEBUG
	if (debug)
		printf(
		    "clock_update: at %lu sample %lu associd %d\n",
		    current_time, peer->epoch, peer->associd);
#endif
	sys_epoch = peer->epoch;
	ostratum = sys_stratum;
	switch (local_clock(peer, epoch, sys_offset)) {

	/*
	 * Clock exceeds panic threshold. Life as we know it ends.
	 */
	case -1:
		report_event(EVNT_SYSFAULT, NULL);
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
		exit (-1);
		/* not reached */

	/*
	 * Clock was stepped. Flush all time values of all peers.
	 */
	case 2:
		clear_all();
		sys_leap = LEAP_NOTINSYNC;
		sys_stratum = STRATUM_UNSPEC;
		sys_rootdelay = 0;
		sys_rootdisp = 0;
		L_CLR(&sys_reftime);
		sys_jitter = LOGTOD(sys_precision);
		memcpy(&sys_refid, "STEP", 4);
		report_event(EVNT_CLOCKRESET, NULL);
		break;

	/*
	 * Clock was slewed. Update the system stratum, leap bits, root
	 * delay, root dispersion, reference ID and reference time.
	 * Except for reference clocks, the minimum dispersion increment
	 * is not less than sys_mindisp.
	 */
	case 1:

		/*
		 * If this is the first time the clock is set, reset the
		 * leap bits. If crypto, the timer will goose the setup
		 * process.
		 */
		if (sys_leap == LEAP_NOTINSYNC) {
			sys_leap = LEAP_NOWARNING;
			report_event(EVNT_SYNCCHG, NULL);
		}
		sys_stratum = min(peer->stratum + 1, STRATUM_UNSPEC);
		sys_rootdelay = peer->delay + peer->rootdelay;
		sys_reftime = peer->rec;

		/*
		 * If a leapseconds file is not present and the number
		 * of survivor leap bits is greater than half the number
		 * of survivors, schedule a leap for the end of the
		 * current month.
		 */
		get_systime(&now);
		if (leap_expire == 0) {
			u_long leapsec;

			leapsec = leap_month(now.l_ui);
			if (leapsec < 28 * 86400) {
				leapsec += now.l_ui;
				if (leapsec != leap_sec) {
					leap_sec = leapsec;
					msyslog(LOG_NOTICE,
					    "proto: leap epoch %lu",						    leap_sec);
				}
			} else {
				leap_sec = 0;
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
	if (ostratum != sys_stratum)
		report_event(EVNT_PEERSTCHG, NULL);
}


/*
 * poll_update - update peer poll interval
 */
void
poll_update(
	struct peer *peer,	/* peer structure pointer */
	int	mpoll
	)
{
	int	hpoll;

	/*
	 * This routine figures out when the next poll should be sent.
	 * That turns out to be wickedly complicated. One problem is
	 * that sometimes the time for the next poll is in the past when
	 * the poll interval is reduced. We watch out for races here
	 * between the receive process and the poll process.
	 *
	 * First, bracket the poll interval according to the type of
	 * association and options. If a fixed interval is configured,
	 * use minpoll. This primarily is for reference clocks, but
	 * works for any association. Otherwise, clamp the poll interval
	 * between minpoll and maxpoll.
	 */
	if (peer->flags & FLAG_FIXPOLL)
		hpoll = peer->minpoll;
	else
		hpoll = max(min(peer->maxpoll, mpoll), peer->minpoll);

#ifdef OPENSSL
	/*
	 * If during the crypto protocol the poll interval has changed,
	 * the lifetimes in the key list are probably bogus. Purge the
	 * the key list and regenerate it later.
	 */
	if (hpoll != peer->hpoll)
		key_expire(peer);
#endif /* OPENSSL */
	peer->hpoll = hpoll;

	/*
	 * Now we figure out if there is an override. If during the
	 * crypto protocol and a responce message is pending, delay one
	 * second.
	 */
#ifdef OPENSSL
	if (peer->cmmd != NULL && (sys_leap != LEAP_NOTINSYNC ||
	    peer->crypto)) {
		peer->nextdate = current_time + RESP_DELAY;

	/*
	 * If we get called from the receive routine while a burst is
	 * pending, just slink away. If a reference clock, delay one
	 * second; otherwise, delay two seconds.
	 */
	} else if (peer->burst > 0) {
#else /* OPENSSL */
	if (peer->burst > 0) {
#endif /* OPENSSL */
		if (peer->nextdate != current_time)
			return;
#ifdef REFCLOCK
		else if (peer->flags & FLAG_REFCLOCK)
			peer->nextdate += RESP_DELAY;
#endif /* REFCLOCK */
		else
			peer->nextdate += res_min_interval;

	/*
	 * The ordinary case. If a retry, use minpoll; otherwise use the
	 * minimum of the host and peer intervals. In other words,
	 * oversampling is okay but understampling is evil.
	 */
	} else {
		if (peer->retry > 0)
			hpoll = peer->minpoll;
		else
			hpoll = min(peer->ppoll, peer->hpoll);
		peer->nextdate = peer->outdate + RANDPOLL(hpoll);
	}

	/*
	 * If the time for the next poll has already happened, bring it
	 * up to the next second after this one. This way the only way
	 * to get nexdate == current time is from the poll routine.
	 */
	if (peer->nextdate <= current_time)
		peer->nextdate = current_time + 1;
#ifdef DEBUG
	if (debug > 1)
		printf("poll_update: at %lu %s flags %04x poll %d burst %d retry %d next %lu\n",
		    current_time, ntoa(&peer->srcadr), peer->flags,
		    peer->hpoll, peer->burst, peer->retry,
		    peer->nextdate);
#endif
}


/*
 * peer_clear - clear peer filter registers.  See Section 3.4.8 of the
 * spec.
 */
void
peer_clear(
	struct peer *peer,		/* peer structure */
	char	*ident			/* tally lights */
	)
{
	int	i;
#ifdef OPENSSL
	char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
#endif /* OPENSSL */

	if (peer == sys_peer)
		sys_peer = NULL;

#ifdef OPENSSL
	/*
	 * If cryptographic credentials have been acquired, toss them to
	 * Valhalla. Note that autokeys are ephemeral, in that they are
	 * tossed immediately upon use. Therefore, the keylist can be
	 * purged anytime without needing to preserve random keys. Note
	 * that, if the peer is purged, the cryptographic variables are
	 * purged, too. This makes it much harder to sneak in some
	 * unauthenticated data in the clock filter.
	 */
	key_expire(peer);
	if (peer->iffval != NULL)
		BN_free(peer->iffval);
	value_free(&peer->cookval);
	value_free(&peer->recval);
	value_free(&peer->encrypt);
	value_free(&peer->sndval);
	if (peer->cmmd != NULL)
		free(peer->cmmd);
	if (peer->subject != NULL)
		free(peer->subject);
	if (peer->issuer != NULL)
		free(peer->issuer);
#endif /* OPENSSL */

	/*
	 * Clear all values, including the optional crypto values above.
	 */
	memset(CLEAR_TO_ZERO(peer), 0, LEN_CLEAR_TO_ZERO);
	peer->estbdelay = sys_bdelay;
	peer->ppoll = peer->maxpoll;
	peer->hpoll = peer->minpoll;
	peer->disp = MAXDISPERSE;
	peer->jitter = LOGTOD(sys_precision);
	for (i = 0; i < NTP_SHIFT; i++) {
		peer->filter_order[i] = i;
		peer->filter_disp[i] = MAXDISPERSE;
	}
#ifdef REFCLOCK
	if (!(peer->flags & FLAG_REFCLOCK)) {
		peer->leap = LEAP_NOTINSYNC;
		peer->stratum = STRATUM_UNSPEC;
		memcpy(&peer->refid, ident, 4);
	}
#else
	peer->leap = LEAP_NOTINSYNC;
	peer->stratum = STRATUM_UNSPEC;
	memcpy(&peer->refid, ident, 4);
#endif /* REFCLOCK */

	/*
	 * During initialization use the association count to spread out
	 * the polls at one-second intervals. Othersie, randomize over
	 * the minimum poll interval in order to avoid broadcast
	 * implosion.
	 */
	peer->nextdate = peer->update = peer->outdate = current_time;
	if (initializing)
		peer->nextdate += peer_associations;
	else if (peer->hmode == MODE_PASSIVE)
		peer->nextdate += RESP_DELAY;
	else
		peer->nextdate += ntp_random() % peer_associations;
#ifdef OPENSSL
	sprintf(statstr, "clear %d ident %s", peer->associd,
	    ident);
		record_crypto_stats(&peer->srcadr, statstr);
#endif /* OPENSSL */
#ifdef DEBUG
	if (debug)
		printf(
		    "peer_clear: at %ld next %ld associd %d refid %s\n",
		    current_time, peer->nextdate, peer->associd,
		    ident);
#endif
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
	double	dtemp, etemp;

	/*
	 * A sample consists of the offset, delay, dispersion and epoch
	 * of arrival.The offset and delay are determined by the on-wire
	 * protcol. The dispersion grows from the last outbound packet
	 * to the arrival of this one increased by the sum of the peer
	 * precision and the system precision as required by the error
	 * budget. First, shift the new arrival into the shift register
	 * discarding the oldest one.
	 */
	j = peer->filter_nextpt;
	peer->filter_offset[j] = sample_offset;
	peer->filter_delay[j] = sample_delay;
	peer->filter_disp[j] = sample_disp;
	peer->filter_epoch[j] = current_time;
	j = (j + 1) % NTP_SHIFT;
	peer->filter_nextpt = j;

	/*
	 * The filter metric design is based on the observation that
	 * phase noise dominates at update intervals less than the Allan
	 * intercept, while frequency noise dominates above that.
	 * Therefore, the metric uses delay at intervals less than the
	 * intercept and dispersion otherwise.
	 *
	 * Update dispersions since the last update and at the same
	 * time initialize the distance and index lists. The distance
	 * list uses a compound metric. If the dispersion is greater
	 * than the maximum dispersion, clamp the distance at that
	 * value. If the time since the last update is less than the
	 * Allan intercept use the delay; otherwise, use the sum of the
	 * delay and dispersion.
	 */
	dtemp = clock_phi * (current_time - peer->update);
	peer->update = current_time;
	for (i = NTP_SHIFT - 1; i >= 0; i--) {
		if (i != 0)
			peer->filter_disp[j] += dtemp;
		if (peer->filter_disp[j] >= MAXDISPERSE) { 
			peer->filter_disp[j] = MAXDISPERSE;
			dst[i] = MAXDISPERSE;
		} else if (peer->update - peer->filter_epoch[j] >
		    ULOGTOD(allan_xpt)) {
			dst[i] = peer->filter_delay[j] +
			    peer->filter_disp[j];
		} else {
			dst[i] = peer->filter_delay[j];
		}
		ord[i] = j;
		j = (j + 1) % NTP_SHIFT;
	}

        /*
	 * If the clock discipline has stabilized, sort the samples by
	 * distance.  
	 */
	if (state == 4) {
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
	 * samples less than the maximum dispersion and, of the
	 * remainder, only samples less than the maximum distance, but
	 * keep at least two samples for jitter calculation.
	 */
	m = 0;
	for (i = 0; i < NTP_SHIFT; i++) {
		peer->filter_order[i] = (u_char) ord[i];
		if (dst[i] >= MAXDISPERSE || (m >= 2 && dst[i] >=
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
	peer->disp = peer->jitter = 0;
	k = ord[0];
	for (i = NTP_SHIFT - 1; i >= 0; i--) {
		j = ord[i];
		peer->disp = NTP_FWEIGHT * (peer->disp +
		    peer->filter_disp[j]);
		if (i < m)
			peer->jitter += DIFF(peer->filter_offset[j],
			    peer->filter_offset[k]);
	}

	/*
	 * If no acceptable samples remain in the shift register,
	 * quietly tiptoe home leaving only the dispersion. Otherwise,
	 * save the offset, delay and jitter. Note the jitter must not
	 * be less than the precision.
	 */
	if (m == 0)
		return;

	etemp = fabs(peer->offset - peer->filter_offset[k]);
	peer->offset = peer->filter_offset[k];
	peer->delay = peer->filter_delay[k];
	if (m > 1)
		peer->jitter /= m - 1;
	peer->jitter = max(SQRT(peer->jitter), LOGTOD(sys_precision));

	/*
	 * If the the new sample and the current sample are both valid
	 * and the difference between their offsets exceeds CLOCK_SGATE
	 * (3) times the jitter and the interval between them is less
	 * than twice the host poll interval, consider the new sample
	 * a popcorn spike and ignore it.
	 */
	if (peer->disp < sys_maxdist && peer->filter_disp[k] <
	    sys_maxdist && etemp > CLOCK_SGATE * peer->jitter &&
	    peer->filter_epoch[k] - peer->epoch < 2. *
	    ULOGTOD(peer->hpoll)) {
#ifdef DEBUG
		if (debug)
			printf("clock_filter: popcorn %.6f %.6f\n",
			    etemp, dtemp);
#endif
		return;
	}

	/*
	 * A new sample is useful only if it is later than the last
	 * one used. In this design the maximum lifetime of any sample
	 * is not greater than eight times the poll interval and the
	 * interval between minimum samples is not greater than eight
	 * packets.
	 */
	if (peer->filter_epoch[k] > peer->epoch)
		peer->epoch = peer->filter_epoch[k];

	/*
	 * The mitigated sample statistics are saved for later
	 * processing. If not synchronized or not in a burst, tickle the
	 * clock select algorithm.
	 */
	record_peer_stats(&peer->srcadr, ctlpeerstatus(peer),
	    peer->offset, peer->delay, peer->disp, peer->jitter);
#ifdef DEBUG
	if (debug)
		printf(
		    "clock_filter: n %d off %.6f del %.6f dsp %.6f jit %.6f, age %lu\n",
		    m, peer->offset, peer->delay, peer->disp,
		    peer->jitter, current_time - peer->filter_epoch[k]);
#endif
	if (peer->burst == 0 || sys_leap == LEAP_NOTINSYNC)
		clock_select();
}


/*
 * clock_select - find the pick-of-the-litter clock
 *
 * LOCKCLOCK: (1) If the local clock is the prefer peer, it will always
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
	int	nlist, nl3;
	int	allow, osurv;
	double	d, e, f, g;
	double	high, low;
	double	seljitter;
	double	synch[NTP_MAXASSOC], error[NTP_MAXASSOC];
	struct peer *osys_peer = NULL;
	struct peer *typeacts = NULL;
	struct peer *typelocal = NULL;
	struct peer *typesystem = NULL;

	static int list_alloc = 0;
	static struct endpoint *endpoint = NULL;
	static int *indx = NULL;
	static struct peer **peer_list = NULL;
	static u_int endpoint_size = 0;
	static u_int indx_size = 0;
	static u_int peer_list_size = 0;

	/*
	 * Initialize and create endpoint, index and peer lists big
	 * enough to handle all associations.
	 */
	osys_peer = sys_peer;
	sys_peer = NULL;
	sys_pps = NULL;
	sys_prefer = NULL;
	osurv = sys_survivors;
	sys_survivors = 0;
#ifdef LOCKCLOCK
	sys_leap = LEAP_NOTINSYNC;
	sys_stratum = STRATUM_UNSPEC;
	memcpy(&sys_refid, "DOWN", 4);
#endif /* LOCKCLOCK */
	nlist = 0;
	for (n = 0; n < NTP_HASH_SIZE; n++)
		nlist += peer_hash_count[n];
	if (nlist > list_alloc) {
		if (list_alloc > 0) {
			free(endpoint);
			free(indx);
			free(peer_list);
		}
		while (list_alloc < nlist) {
			list_alloc += 5;
			endpoint_size += 5 * 3 * sizeof(*endpoint);
			indx_size += 5 * 3 * sizeof(*indx);
			peer_list_size += 5 * sizeof(*peer_list);
		}
		endpoint = (struct endpoint *)emalloc(endpoint_size);
		indx = (int *)emalloc(indx_size);
		peer_list = (struct peer **)emalloc(peer_list_size);
	}

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
	nlist = nl3 = 0;	/* none yet */
	for (n = 0; n < NTP_HASH_SIZE; n++) {
		for (peer = peer_hash[n]; peer != NULL; peer =
		    peer->next) {
			peer->flags &= ~FLAG_SYSPEER;
			peer->status = CTL_PST_SEL_REJECT;

			/*
			 * Leave the island immediately if the peer is
			 * unfit to synchronize.
			 */
			if (peer_unfit(peer))
				continue;

			/*
			 * Don't allow the local clock or modem drivers
			 * in the kitchen at this point, unless the
			 * prefer peer. Do that later, but only if
			 * nobody else is around. These guys are all
			 * configured, so we never throw them away.
			 */
#ifdef REFCLOCK
			if (peer->refclktype == REFCLK_LOCALCLOCK
#if defined(VMS) && defined(VMS_LOCALUNIT)
			/* wjm: VMS_LOCALUNIT taken seriously */
			    && REFCLOCKUNIT(&peer->srcadr) !=
			    VMS_LOCALUNIT
#endif	/* VMS && VMS_LOCALUNIT */
				) {
				typelocal = peer;
#ifndef LOCKCLOCK
				if (!(peer->flags & FLAG_PREFER))
					continue; /* no local clock */
#endif /* LOCKCLOCK */
			}
			if (peer->sstclktype == CTL_SST_TS_TELEPHONE) {
				typeacts = peer;
				if (!(peer->flags & FLAG_PREFER))
					continue; /* no acts */
			}
#endif /* REFCLOCK */

			/*
			 * If we get this far, the peer can stay on the
			 * island, but does not yet have the immunity
			 * idol.
			 */
			peer->status = CTL_PST_SEL_SANE;
			peer_list[nlist++] = peer;

			/*
			 * Insert each interval endpoint on the sorted
			 * list.
			 */
			e = peer->offset;	 /* Upper end */
			f = root_distance(peer);
			e = e + f;
			for (i = nl3 - 1; i >= 0; i--) {
				if (e >= endpoint[indx[i]].val)
					break;

				indx[i + 3] = indx[i];
			}
			indx[i + 3] = nl3;
			endpoint[nl3].type = 1;
			endpoint[nl3++].val = e;

			e = e - f;		/* Center point */
			for (; i >= 0; i--) {
				if (e >= endpoint[indx[i]].val)
					break;

				indx[i + 2] = indx[i];
			}
			indx[i + 2] = nl3;
			endpoint[nl3].type = 0;
			endpoint[nl3++].val = e;

			e = e - f;		/* Lower end */
			for (; i >= 0; i--) {
				if (e >= endpoint[indx[i]].val)
					break;

				indx[i + 1] = indx[i];
			}
			indx[i + 1] = nl3;
			endpoint[nl3].type = -1;
			endpoint[nl3++].val = e;
		}
	}
#ifdef DEBUG
	if (debug > 2)
		for (i = 0; i < nl3; i++)
			printf("select: endpoint %2d %.6f\n",
			   endpoint[indx[i]].type,
			   endpoint[indx[i]].val);
#endif
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
	 * susvivors with offsets not less than low and not greater than
	 * high. There may be none of them.
	 */
	low = 1e9;
	high = -1e9;
	for (allow = 0; 2 * allow < nlist; allow++) {
		int	found;

		/*
		 * Bound the interval (low, high) as the largest
		 * interval containing points from presumed truechimers.
		 */
		found = 0;
		n = 0;
		for (i = 0; i < nl3; i++) {
			low = endpoint[indx[i]].val;
			n -= endpoint[indx[i]].type;
			if (n >= nlist - allow)
				break;
			if (endpoint[indx[i]].type == 0)
				found++;
		}
		n = 0;
		for (j = nl3 - 1; j >= 0; j--) {
			high = endpoint[indx[j]].val;
			n += endpoint[indx[j]].type;
			if (n >= nlist - allow)
				break;
			if (endpoint[indx[j]].type == 0)
				found++;
		}

		/*
		 * If the number of candidates found outside the
		 * interval is greater than the number of falsetickers,
		 * then at least one truechimer is outside the interval,
		 * so go around again. This is what makes this algorithm
		 * different than Marzullo's.
		 */
		if (found > allow)
			continue;

		/*
		 * If an interval containing truechimers is found, stop.
		 * If not, increase the number of falsetickers and go
		 * around again.
		 */
		if (high > low)
			break;
	}

	/*
	 * Clustering algorithm. Construct candidate list in order first
	 * by stratum then by root distance, but keep only the best
	 * NTP_MAXASSOC of them. Scan the list to find falsetickers, who
	 * leave the island immediately. The TRUE peer is always a
	 * truechimer. We must leave at least one peer to collect the
	 * million bucks.
	 */
	j = 0;
	for (i = 0; i < nlist; i++) {
		peer = peer_list[i];
		if (nlist > 1 && (peer->offset <= low || peer->offset >=
		    high) && !(peer->flags & FLAG_TRUE))
			continue;

		peer->status = CTL_PST_SEL_DISTSYSPEER;

		/*
		 * The order metric is formed from the stratum times
		 * max distance (1.) plus the root distance. It strongly
		 * favors the lowest stratum, but a higher stratum peer
		 * can capture the clock if the low stratum dominant
		 * hasn't been heard for awhile.
		 */
		d = root_distance(peer) + peer->stratum * sys_maxdist;
		if (j >= NTP_MAXASSOC) {
			if (d >= synch[j - 1])
				continue;
			else
				j--;
		}
		for (k = j; k > 0; k--) {
			if (d >= synch[k - 1])
				break;

			peer_list[k] = peer_list[k - 1];
			error[k] = error[k - 1];
			synch[k] = synch[k - 1];
		}
		peer_list[k] = peer;
		error[k] = peer->jitter;
		synch[k] = d;
		j++;
	}
	nlist = j;

	/*
	 * If no survivors remain at this point, check if the local
	 * clock or modem drivers have been found. If so, nominate one
	 * of them as the only survivor. Otherwise, give up and leave
	 * the island to the rats.
	 */
	if (nlist == 0) {
		error[0] = 0;
		synch[0] = 0;
		if (typeacts != NULL) {
			typeacts->status = CTL_PST_SEL_DISTSYSPEER;
			peer_list[0] = typeacts;
			nlist = 1;
		} else if (typelocal != NULL) {
			typelocal->status = CTL_PST_SEL_DISTSYSPEER;
			peer_list[0] = typelocal;
			nlist = 1;
		} else {
			if (osys_peer != NULL) {
				NLOG(NLOG_SYNCSTATUS)
				    msyslog(LOG_INFO,
				    "no servers reachable");
				report_event(EVNT_PEERSTCHG, NULL);
			}
		}
	}

	/*
	 * We can only trust the survivors if the number of candidates
	 * sys_minsane is at least the number required to detect and
	 * cast out one falsticker. For the Byzantine agreement
	 * algorithm used here, that number is 4; however, the default
	 * sys_minsane is 1 to speed initial synchronization. Careful
	 * operators will tinker a higher value and use at least that
	 * number of synchronization sources.
	 */
	for (i = 0; i < nlist; i++)
		peer_list[i]->status = CTL_PST_SEL_SELCAND;
	if (nlist < sys_minsane)
		return;

	/*
	 * Now, vote outlyers off the island by select jitter weighted
	 * by root distance. Continue voting as long as there are more
	 * than sys_minclock survivors and the minimum select jitter is
	 * greater than the maximum peer jitter. Stop if we are about to
	 * discard a TRUE or PREFER  peer, who of course has the
	 * immunity idol.
	 */
	seljitter = 0;
	while (1) {
		d = 1e9;
		e = -1e9;
		f = g = 0;
		k = 0;
		for (i = 0; i < nlist; i++) {
			if (error[i] < d)
				d = error[i];
			f = 0;
			if (nlist > 1) {
				for (j = 0; j < nlist; j++)
					f += DIFF(peer_list[j]->offset,
					    peer_list[i]->offset);
				f = SQRT(f / (nlist - 1));
			}
			if (f * synch[i] > e) {
				g = f;
				e = f * synch[i];
				k = i;
			}
		}
		f = max(f, LOGTOD(sys_precision));
		if (nlist <= sys_minclock || f <= d ||
		    peer_list[k]->flags & (FLAG_TRUE | FLAG_PREFER)) {
			seljitter = f;
			break;
		}
#ifdef DEBUG
		if (debug > 2)
			printf(
			    "select: drop %s select %.6f jitter %.6f\n",
			    ntoa(&peer_list[k]->srcadr), g, d);
#endif
		for (j = k + 1; j < nlist; j++) {
			peer_list[j - 1] = peer_list[j];
			error[j - 1] = error[j];
		}
		nlist--;
	}

	/*
	 * What remains is a list usually not greater than sys_minclock
	 * peers. We want only the peer at the lowest stratum to become
	 * the system peer, although all survivors are eligible for the
	 * combining algorithm. Check for prefer and pps peers at any
	 * stratum. Note that the head of the list is at the lowest
	 * stratum and that unsynchronized peers cannot survivethis far.
	 *
	 * While at it, count the number of leap warning bits found.
	 * This will be used later to vote the system leap warning bit.
	 * If a leap warning bit is found on a reference clock, the vote
	 * is always won.
	 */
	leap_next = 0;
	for (i = 0; i < nlist; i++) {
		sys_survivors++;
		peer = peer_list[i];
		if (i < sys_maxclock)
			peer->unreach = 0;
		if (peer->leap == LEAP_ADDSECOND) {
			if (peer->flags & FLAG_REFCLOCK)
				leap_next = nlist;
			else 
				leap_next++;
		}
		peer->status = CTL_PST_SEL_SYNCCAND;
		if (peer->flags & FLAG_PREFER)
			sys_prefer = peer;
		if (peer == osys_peer)
			typesystem = peer;
#ifdef REFCLOCK
		if (peer->refclktype == REFCLK_ATOM_PPS)
			sys_pps = peer;
#endif /* REFCLOCK */
#if DEBUG
		if (debug > 1)
			printf("cluster: survivor %s metric %.6f\n",
			    ntoa(&peer_list[i]->srcadr), synch[i]);
#endif
	}

	/*
	 * Anticlockhop provision. Keep the current system peer if it is
	 * a survivor but not first in the list. But do that only HOPPER
	 * times.
	 */
	if (typesystem == NULL || typesystem == peer_list[0] ||
	    sys_hopper > sys_maxhop) {
		typesystem = peer_list[0];
		sys_hopper = 0;
	} else {
		sys_hopper++;
	}

	/*
	 * Mitigation rules of the game. There are several types of
	 * peers that can be selected here: (1) orphan, (2) prefer peer
	 * (flag FLAG_PREFER) (3) pps peers (type REFCLK_ATOM_PPS), (4)
	 * the existing system peer, if any, and (5) the head of the
	 * survivor list.
	 */
	if (typesystem->stratum == sys_orphan) {
		sys_peer = typesystem;
		sys_peer->status = CTL_PST_SEL_SYSPEER;
		sys_offset = sys_peer->offset;
		sys_refid = addr2refid(&sys_peer->srcadr);
		sys_jitter = LOGTOD(sys_precision);
#ifdef DEBUG
		if (debug > 1)
			printf("select: orphan offset %.6f\n",
			    sys_offset);
#endif
	} else if (sys_prefer) {

		/*
		 * If a pps peer is present, choose it; otherwise,
		 * choose the prefer peer.
		 */
		if (sys_pps) {
			sys_peer = sys_pps;
			sys_peer->status = CTL_PST_SEL_PPS;
			sys_offset = sys_peer->offset;
			if (!pps_control)
				NLOG(NLOG_SYSEVENT)
				    msyslog(LOG_INFO,
				    "pps sync enabled");
			pps_control = current_time;
#ifdef DEBUG
			if (debug > 1)
				printf("select: pps offset %.6f\n",
				    sys_offset);
#endif
		} else {
			sys_peer = sys_prefer;
			sys_peer->status = CTL_PST_SEL_SYSPEER;
			sys_offset = sys_peer->offset;
#ifdef DEBUG
			if (debug > 1)
				printf("select: prefer offset %.6f\n",
				    sys_offset);
#endif
		}
		if (sys_peer->stratum == STRATUM_REFCLOCK ||
		    sys_peer->stratum == STRATUM_UNSPEC)
			sys_refid = sys_peer->refid;
		else
			sys_refid = addr2refid(&sys_peer->srcadr);
		sys_jitter = sys_peer->jitter;
	} else {

		/*
		 * Otherwise, choose the anticlockhopper.
		 */ 
		sys_peer = typesystem;
		sys_peer->status = CTL_PST_SEL_SYSPEER;
		clock_combine(peer_list, nlist);
		if (sys_peer->stratum == STRATUM_REFCLOCK ||
		    sys_peer->stratum == STRATUM_UNSPEC)
			sys_refid = sys_peer->refid;
		else
			sys_refid = addr2refid(&sys_peer->srcadr);
		sys_jitter = SQRT(SQUARE(sys_peer->jitter) +
		    SQUARE(sys_jitter) + SQUARE(seljitter));
#ifdef DEBUG
		if (debug > 1)
			printf("select: combine offset %.6f\n",
			    sys_offset);
#endif
	}

	/*
	 * We have found the alpha male.
	 */
	sys_peer->flags |= FLAG_SYSPEER;
	if (osys_peer != sys_peer) {
		char *src;

		report_event(EVNT_PEERSTCHG, NULL);

#ifdef REFCLOCK
                if (sys_peer->flags & FLAG_REFCLOCK)
                        src = refnumtoa(&sys_peer->srcadr);
                else
#endif /* REFCLOCK */
                        src = ntoa(&sys_peer->srcadr);
		NLOG(NLOG_SYNCSTATUS)
		    msyslog(LOG_INFO, "synchronized to %s, stratum %d",
			src, sys_peer->stratum);
	}
	clock_update(sys_peer);
}


/*
 * clock_combine - compute system offset and jitter from selected peers
 */
static void
clock_combine(
	struct peer **peers,	/* survivor list */
	int	npeers		/* number of survivors */
	)
{
	int	i;
	double	x, y, z, w;

	y = z = w = 0;
	for (i = 0; i < npeers; i++) {
		x = root_distance(peers[i]);
		y += 1. / x;
		z += peers[i]->offset / x;
		w += SQUARE(peers[i]->offset - peers[0]->offset) / x;
	}
	sys_offset = z / y;
	sys_jitter = SQRT(w / y);
}


/*
 * root_distance - compute synchronization distance from peer to root
 */
static double
root_distance(
	struct peer *peer	/* peer structure pointer */
	)
{

	/*
	 * Careful squeak here. The value returned must be greater than
	 * the minimum root dispersion in order to avoid clockhop with
	 * highly precise reference clocks. In orphan mode use the peer
	 * address to fool the mitigation algorithm.
	 */
	if (peer->stratum == sys_orphan)
		return (addr2refid(&peer->srcadr) / FRAC * sys_maxdist /
		    1.);

	return ((peer->delay + peer->rootdelay) / 2 + peer->disp +
	    peer->rootdisp + clock_phi * (current_time - peer->update) +
	    peer->jitter);
}


/*
 * peer_xmit - send packet for persistent association.
 */
static void
peer_xmit(
	struct peer *peer	/* peer structure pointer */
	)
{
	struct pkt xpkt;	/* transmit packet */
	int	sendlen, authlen;
	keyid_t	xkeyid = 0;	/* transmit key ID */
	l_fp	xmt_tx;

	if (!peer->dstadr)	/* drop peers without interface */
		return;

	/*
	 * Construct the packet header only if rate and crypto ok.
	 */
	if (peer->flash & (TEST4 | TEST9))
		return;

	xpkt.li_vn_mode = PKT_LI_VN_MODE(sys_leap, peer->version,
	    peer->hmode);
	xpkt.stratum = STRATUM_TO_PKT(sys_stratum);
	xpkt.ppoll = peer->hpoll;
	xpkt.precision = sys_precision;
	xpkt.refid = sys_refid;
	xpkt.rootdelay = HTONS_FP(DTOFP(sys_rootdelay));
	xpkt.rootdisp =  HTONS_FP(DTOUFP(sys_rootdisp));
	HTONL_FP(&sys_reftime, &xpkt.reftime);
	HTONL_FP(&peer->org, &xpkt.org);
	HTONL_FP(&peer->rec, &xpkt.rec);

	/*
	 * If the received packet contains a MAC, the transmitted packet
	 * is authenticated and contains a MAC. If not, the transmitted
	 * packet is not authenticated.
	 *
	 * It is most important when autokey is in use that the local
	 * interface IP address be known before the first packet is
	 * sent. Otherwise, it is not possible to compute a correct MAC
	 * the recipient will accept. Thus, the I/O semantics have to do
	 * a little more work. In particular, the wildcard interface
	 * might not be usable.
	 */
	sendlen = LEN_PKT_NOMAC;
	if (!(peer->flags & FLAG_AUTHENABLE)) {
		get_systime(&peer->xmt);
		HTONL_FP(&peer->xmt, &xpkt.xmt);
		peer->t21_bytes = sendlen;
		sendpkt(&peer->srcadr, peer->dstadr, sys_ttl[peer->ttl],
		    &xpkt, sendlen);
		peer->sent++;
		peer->throttle += res_avg_interval;
#ifdef DEBUG
		if (debug)
			printf("transmit: at %ld %s->%s mode %d len %d\n",
		    	    current_time, peer->dstadr ?
			    stoa(&peer->dstadr->sin) : "-",
		            stoa(&peer->srcadr), peer->hmode, sendlen);
#endif
		return;
	}

	/*
	 * Authentication is enabled, so the transmitted packet must be
	 * authenticated. If autokey is enabled, fuss with the various
	 * modes; otherwise, symmetric key cryptography is used.
	 */
#ifdef OPENSSL
	if (crypto_flags && (peer->flags & FLAG_SKEY)) {
		struct exten *exten;	/* extension field */

		/*
		 * The Public Key Dance (PKD): Cryptographic credentials
		 * are contained in extension fields, each including a
		 * 4-octet length/code word followed by a 4-octet
		 * association ID and optional additional data. Optional
		 * data includes a 4-octet data length field followed by
		 * the data itself. Request messages are sent from a
		 * configured association; response messages can be sent
		 * from a configured association or can take the fast
		 * path without ever matching an association. Response
		 * messages have the same code as the request, but have
		 * a response bit and possibly an error bit set. In this
		 * implementation, a message may contain no more than
		 * one command and one or more responses.
		 *
		 * Cryptographic session keys include both a public and
		 * a private componet. Request and response messages
		 * using extension fields are always sent with the
		 * private component set to zero. Packets without
		 * extension fields indlude the private component when
		 * the session key is generated.
		 */
		while (1) {
		
			/*
			 * Allocate and initialize a keylist if not
			 * already done. Then, use the list in inverse
			 * order, discarding keys once used. Keep the
			 * latest key around until the next one, so
			 * clients can use client/server packets to
			 * compute propagation delay.
			 *
			 * Note that once a key is used from the list,
			 * it is retained in the key cache until the
			 * next key is used. This is to allow a client
			 * to retrieve the encrypted session key
			 * identifier to verify authenticity.
			 *
			 * If for some reason a key is no longer in the
			 * key cache, a birthday has happened or the key
			 * has expired, so the pseudo-random sequence is
			 * broken. In that case, purge the keylist and
			 * regenerate it.
			 */
			if (peer->keynumber == 0)
				make_keylist(peer, peer->dstadr);
			else
				peer->keynumber--;
			xkeyid = peer->keylist[peer->keynumber];
			if (authistrusted(xkeyid))
				break;
			else
				key_expire(peer);
		}
		peer->keyid = xkeyid;
		exten = NULL;
		switch (peer->hmode) {

		/*
		 * In broadcast server mode the autokey values are
		 * required by the broadcast clients. Push them when a
		 * new keylist is generated; otherwise, push the
		 * association message so the client can request them at
		 * other times.
		 */
		case MODE_BROADCAST:
			if (peer->flags & FLAG_ASSOC)
				exten = crypto_args(peer, CRYPTO_AUTO |
				    CRYPTO_RESP, peer->associd, NULL);
			else
				exten = crypto_args(peer, CRYPTO_ASSOC |
				    CRYPTO_RESP, peer->associd, NULL);
			break;

		/*
		 * In symmetric modes the digest, certificate, agreement
		 * parameters, cookie and autokey values are required.
		 * The leapsecond table is optional. But, a passive peer
		 * will not believe the active peer until the latter has
		 * synchronized, so the agreement must be postponed
		 * until then. In any case, if a new keylist is
		 * generated, the autokey values are pushed.
		 *
		 * If the crypto bit is lit, don't send requests.
		 */
		case MODE_ACTIVE:
		case MODE_PASSIVE:

			/*
			 * Parameter and certificate.
			 */
			if (!peer->crypto)
				exten = crypto_args(peer, CRYPTO_ASSOC,
				    peer->associd, sys_hostname);
			else if (!(peer->crypto & CRYPTO_FLAG_VALID))
				exten = crypto_args(peer, CRYPTO_CERT,
				    peer->associd, peer->issuer);

			/*
			 * Identity. Note we have to sign the
			 * certificate before the cookie to avoid a
			 * deadlock when the passive peer is walking the
			 * certificate trail. Awesome.
			 */
			else if (!(peer->crypto & CRYPTO_FLAG_VRFY))
				exten = crypto_args(peer,
				    crypto_ident(peer), peer->associd,
				    NULL);
			else if (sys_leap != LEAP_NOTINSYNC &&
			    !(peer->crypto & CRYPTO_FLAG_SIGN))
				exten = crypto_args(peer, CRYPTO_SIGN,
				    peer->associd, sys_hostname);

			/*
			 * Autokey. We request the cookie only when the
			 * server and client are synchronized and
			 * signatures work both ways. On the other hand,
			 * the active peer needs the autokey values
			 * before then and when the passive peer is
			 * waiting for the active peer to synchronize.
			 * Any time we regenerate the key list, we offer
			 * the autokey values without being asked.
			 */
			else if (sys_leap != LEAP_NOTINSYNC &&
			    peer->leap != LEAP_NOTINSYNC &&
			    !(peer->crypto & CRYPTO_FLAG_AGREE))
				exten = crypto_args(peer, CRYPTO_COOK,
				    peer->associd, NULL);
			else if (peer->flags & FLAG_ASSOC)
				exten = crypto_args(peer, CRYPTO_AUTO |
				    CRYPTO_RESP, peer->associd, NULL);
			else if (!(peer->crypto & CRYPTO_FLAG_AUTO))
				exten = crypto_args(peer, CRYPTO_AUTO,
				    peer->associd, NULL);

			/*
			 * Postamble. We trade leapseconds only when the
			 * server and client are synchronized.
			 */
			else if (sys_leap != LEAP_NOTINSYNC &&
			    peer->leap != LEAP_NOTINSYNC &&
			    !(peer->crypto & CRYPTO_FLAG_LEAP))
				exten = crypto_args(peer, CRYPTO_TAI,
				    peer->associd, NULL);
			break;

		/*
		 * In client mode the digest, certificate, agreement
		 * parameters and cookie are required. The leapsecond
		 * table is optional. If broadcast client mode, the
		 * autokey values are required as well. In broadcast
		 * client mode, these values must be acquired during the
		 * client/server exchange to avoid having to wait until
		 * the next key list regeneration. Otherwise, the poor
		 * dude may die a lingering death until becoming
		 * unreachable and attempting rebirth.
		 *
		 * If neither the server or client have the agreement
		 * parameters, the protocol transmits the cookie in the
		 * clear. If the server has the parameters, the client
		 * requests them and the protocol blinds it using the
		 * agreed key. It is a protocol error if the client has
		 * the parameters but the server does not.
		 *
		 * If the crypto bit is lit, don't send requests.
		 */
		case MODE_CLIENT:

			/*
			 * Parameter and certificate.
			 */
			if (!peer->crypto)
				exten = crypto_args(peer, CRYPTO_ASSOC,
				    peer->associd, sys_hostname);
			else if (!(peer->crypto & CRYPTO_FLAG_VALID))
				exten = crypto_args(peer, CRYPTO_CERT,
				    peer->associd, peer->issuer);

			/*
			 * Identity. Nothing special here.
			 */
			else if (!(peer->crypto & CRYPTO_FLAG_VRFY))
				exten = crypto_args(peer,
				    crypto_ident(peer), peer->associd,
				    NULL);

			/*
			 * Cookie and autokey data. In broadcast client
			 * mode we request the autokey values now to
			 * avoid having to wait for the next key list.
			 */
			else if (!(peer->crypto & CRYPTO_FLAG_AGREE))
				exten = crypto_args(peer, CRYPTO_COOK,
				    peer->associd, NULL);
			else if (!(peer->crypto & CRYPTO_FLAG_AUTO) &&
			    (peer->cast_flags & MDF_BCLNT))
				exten = crypto_args(peer, CRYPTO_AUTO,
				    peer->assoc, NULL);

			/*
			 * Postamble. We can sign the certificate here,
			 * since there is no chance of deadlock.
			 */
			else if (sys_leap != LEAP_NOTINSYNC &&
				!(peer->crypto & CRYPTO_FLAG_SIGN))
				exten = crypto_args(peer, CRYPTO_SIGN,
				    peer->associd, sys_hostname);
			else if (sys_leap != LEAP_NOTINSYNC &&
			    !(peer->crypto & CRYPTO_FLAG_LEAP))
				exten = crypto_args(peer, CRYPTO_TAI,
				    peer->associd, NULL);
			break;
		}

		/*
		 * Build the extension fields as directed. A response to
		 * a request is always sent, even if an error. If an
		 * error occurs when sending a request, the crypto
		 * machinery broke or was misconfigured. In that case
		 * light the crypto bit to suppress further requests.
		 */
		if (peer->cmmd != NULL) {
			peer->cmmd->associd = htonl(peer->associd);
			crypto_xmit(&xpkt, &peer->srcadr, &sendlen,
			    peer->cmmd, 0);
			free(peer->cmmd);
			peer->cmmd = NULL;
		}
		if (exten != NULL) {
			if (exten->opcode != 0)
				crypto_xmit(&xpkt, &peer->srcadr,
				    &sendlen, exten, 0);
			free(exten);
		}

		/*
		 * If extension fields are present, we must use a
		 * private cookie value of zero. Don't send if the
		 * crypto bit is set and no extension field is present,
		 * but in that case give back the key. Most intricate.
		 */
		if (sendlen > LEN_PKT_NOMAC) {
			session_key(&peer->dstadr->sin, &peer->srcadr,
			    xkeyid, 0, 2);
		}
	} 
#endif /* OPENSSL */

	/*
	 * Stash the transmit timestamp corrected for the encryption
	 * delay. If autokey, give back the key, as we use keys only
	 * once. Check for errors such as missing keys, etc.
	 */
	xkeyid = peer->keyid;
	get_systime(&peer->xmt);
	L_ADD(&peer->xmt, &sys_authdelay);
	HTONL_FP(&peer->xmt, &xpkt.xmt);
	authlen = authencrypt(xkeyid, (u_int32 *)&xpkt, sendlen);
	if (authlen == 0) {
		msyslog(LOG_INFO, "transmit: %s key %u not found",
		    stoa(&peer->srcadr), xkeyid);
		peer->flash |= TEST9;		/* no key found */
		return;
	}
	sendlen += authlen;
#ifdef OPENSSL
	if (xkeyid > NTP_MAXKEY)
		authtrust(xkeyid, 0);
#endif /* OPENSSL */
	get_systime(&xmt_tx);
	if (sendlen > sizeof(xpkt)) {
		msyslog(LOG_ERR, "buffer overflow %u", sendlen);
		exit (-1);
	}
	peer->t21_bytes = sendlen;
	sendpkt(&peer->srcadr, peer->dstadr, sys_ttl[peer->ttl], &xpkt,
	    sendlen);
	peer->sent++;
	peer->throttle += res_avg_interval;

	/*
	 * Calculate the encryption delay. Keep the minimum over
	 * the latest two samples.
	 */
	L_SUB(&xmt_tx, &peer->xmt);
	L_ADD(&xmt_tx, &sys_authdelay);
	sys_authdly[1] = sys_authdly[0];
	sys_authdly[0] = xmt_tx.l_uf;
	if (sys_authdly[0] < sys_authdly[1])
		sys_authdelay.l_uf = sys_authdly[0];
	else
		sys_authdelay.l_uf = sys_authdly[1];
#ifdef OPENSSL
#ifdef DEBUG
	if (debug)
		printf("transmit: at %ld %s->%s mode %d keyid %08x len %d index %d\n",
		    current_time, peer->dstadr ?
		    ntoa(&peer->dstadr->sin) : "-",
	 	    ntoa(&peer->srcadr), peer->hmode, xkeyid, sendlen,
		    peer->keynumber);
#endif
#else /* OPENSSL */
#ifdef DEBUG
	if (debug)
		printf("transmit: at %ld %s->%s mode %d keyid %08x len %d\n",
		    current_time, peer->dstadr ?
		    ntoa(&peer->dstadr->sin) : "-",
		    ntoa(&peer->srcadr), peer->hmode, xkeyid, sendlen);
#endif
#endif /* OPENSSL */
}


/*
 * fast_xmit - Send packet for nonpersistent association. Note that
 * neither the source or destination can be a broadcast address.
 */
static void
fast_xmit(
	struct recvbuf *rbufp,	/* receive packet pointer */
	int	rmode,		/* receive mode */
	keyid_t	xkeyid,		/* transmit key ID */
	char	*mask		/* kiss code */
	)
{
	struct pkt xpkt;	/* transmit packet structure */
	struct pkt *rpkt;	/* receive packet structure */
	l_fp	xmt_ts;		/* timestamp */
	l_fp	xmt_tx;		/* timestamp after authent */
	int	xmode;		/* transmit mode */
	int	sendlen, authlen;
#ifdef OPENSSL
	u_int32	temp32;
#endif

	/*
	 * Initialize transmit packet header fields from the receive
	 * buffer provided. We leave some fields intact as received. If
	 * the gazinta was from a multicast address, the gazoutta must
	 * go out another way.
	 */
	rpkt = &rbufp->recv_pkt;
	if (rbufp->dstadr->flags & INT_MCASTOPEN)
		rbufp->dstadr = findinterface(&rbufp->recv_srcadr);
	if (rmode == MODE_CLIENT)
		xmode = MODE_SERVER;
	else
		xmode = MODE_ACTIVE;

	/*
	 * If this is a kiss-o'-death (KoD) packet, show leap
	 * unsynchronized, stratum zero, reference ID the four-character
	 * kiss code and system root delay. Note the rate limit on these
	 * packets. Once a second initialize a bucket counter. Every
	 * packet sent decrements the counter until reaching zero. If
	 * the counter is zero, drop the kiss. Note also we don't reveal
	 * the local time, so these packets can't be used for
	 * synchronization.
	 */
	if (mask != NULL) {
		if (sys_kod == 0)
			return;

		sys_kod--;
		xpkt.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOTINSYNC,
		    PKT_VERSION(rpkt->li_vn_mode), xmode);
		xpkt.stratum = STRATUM_PKT_UNSPEC;
		memcpy(&xpkt.refid, "RATE", 4);
		xpkt.org = rpkt->xmt;
		xpkt.rec = rpkt->xmt;
		xpkt.xmt = rpkt->xmt;

	/*
	 * This is a normal packet. Use the system variables.
	 */
	} else {
		xpkt.li_vn_mode = PKT_LI_VN_MODE(sys_leap,
		    PKT_VERSION(rpkt->li_vn_mode), xmode);
		xpkt.stratum = STRATUM_TO_PKT(sys_stratum);
		xpkt.ppoll = rpkt->ppoll;
		xpkt.precision = sys_precision;
		xpkt.refid = sys_refid;
		xpkt.rootdelay = HTONS_FP(DTOFP(sys_rootdelay));
		xpkt.rootdisp = HTONS_FP(DTOUFP(sys_rootdisp));
		HTONL_FP(&sys_reftime, &xpkt.reftime);
		xpkt.org = rpkt->xmt;
		HTONL_FP(&rbufp->recv_time, &xpkt.rec);
	}

	/*
	 * If the received packet contains a MAC, the transmitted packet
	 * is authenticated and contains a MAC. If not, the transmitted
	 * packet is not authenticated.
	 */
	sendlen = LEN_PKT_NOMAC;
	if (rbufp->recv_length == sendlen) {
		if (mask == NULL) {
			get_systime(&xmt_ts);
			HTONL_FP(&xmt_ts, &xpkt.xmt);
		}
		sendpkt(&rbufp->recv_srcadr, rbufp->dstadr, 0, &xpkt,
		    sendlen);
#ifdef DEBUG
		if (debug)
			printf(
			    "transmit: at %ld %s->%s mode %d len %d\n",
			    current_time, stoa(&rbufp->dstadr->sin),
			    stoa(&rbufp->recv_srcadr), xmode, sendlen);
#endif
		return;
	}

	/*
	 * The received packet contains a MAC, so the transmitted packet
	 * must be authenticated. For symmetric key cryptography, use
	 * the predefined and trusted symmetric keys to generate the
	 * cryptosum. For autokey cryptography, use the server private
	 * value to generate the cookie, which is unique for every
	 * source-destination-key ID combination.
	 */
#ifdef OPENSSL
	if (xkeyid > NTP_MAXKEY) {
		keyid_t cookie;

		/*
		 * The only way to get here is a reply to a legitimate
		 * client request message, so the mode must be
		 * MODE_SERVER. If an extension field is present, there
		 * can be only one and that must be a command. Do what
		 * needs, but with private value of zero so the poor
		 * jerk can decode it. If no extension field is present,
		 * use the cookie to generate the session key.
		 */
		cookie = session_key(&rbufp->recv_srcadr,
		    &rbufp->dstadr->sin, 0, sys_private, 0);
		if (rbufp->recv_length >= (int)(sendlen + MAX_MAC_LEN +
		    2 * sizeof(u_int32))) {
			session_key(&rbufp->dstadr->sin,
			    &rbufp->recv_srcadr, xkeyid, 0, 2);
			temp32 = CRYPTO_RESP;
			rpkt->exten[0] |= htonl(temp32);
			crypto_xmit(&xpkt, &rbufp->recv_srcadr,
			    &sendlen, (struct exten *)rpkt->exten,
			    cookie);
		} else {
			session_key(&rbufp->dstadr->sin,
			    &rbufp->recv_srcadr, xkeyid, cookie, 2);
		}
	}
#endif /* OPENSSL */
	if (mask == NULL) {
		get_systime(&xmt_ts);
		L_ADD(&xmt_ts, &sys_authdelay);
		HTONL_FP(&xmt_ts, &xpkt.xmt);
	}
	authlen = authencrypt(xkeyid, (u_int32 *)&xpkt, sendlen);
	sendlen += authlen;
#ifdef OPENSSL
	if (xkeyid > NTP_MAXKEY)
		authtrust(xkeyid, 0);
#endif /* OPENSSL */
	get_systime(&xmt_tx);
	sendpkt(&rbufp->recv_srcadr, rbufp->dstadr, 0, &xpkt, sendlen);

	/*
	 * Calculate the encryption delay. Keep the minimum over the
	 * latest two samples.
	 */
	L_SUB(&xmt_tx, &xmt_ts);
	L_ADD(&xmt_tx, &sys_authdelay);
	sys_authdly[1] = sys_authdly[0];
	sys_authdly[0] = xmt_tx.l_uf;
	if (sys_authdly[0] < sys_authdly[1])
		sys_authdelay.l_uf = sys_authdly[0];
	else
		sys_authdelay.l_uf = sys_authdly[1];
#ifdef DEBUG
	if (debug)
		printf(
		    "transmit: at %ld %s->%s mode %d keyid %08x len %d\n",
		    current_time, ntoa(&rbufp->dstadr->sin),
		    ntoa(&rbufp->recv_srcadr), xmode, xkeyid, sendlen);
#endif
}


#ifdef OPENSSL
/*
 * key_expire - purge the key list
 */
void
key_expire(
	struct peer *peer	/* peer structure pointer */
	)
{
	int i;

	if (peer->keylist != NULL) {
		for (i = 0; i <= peer->keynumber; i++)
			authtrust(peer->keylist[i], 0);
		free(peer->keylist);
		peer->keylist = NULL;
	}
	value_free(&peer->sndval);
	peer->keynumber = 0;
	peer->flags &= ~FLAG_ASSOC;
#ifdef DEBUG
	if (debug)
		printf("key_expire: at %lu associd %d\n", current_time,
		    peer->associd);
#endif
}
#endif /* OPENSSL */


/*
 * Determine if the peer is unfit for synchronization
 *
 * A peer is unfit for synchronization if
 * > TEST10 bad leap or stratum below floor or at or above ceiling
 * > TEST11 root distance exceeded for remote peer
 * > TEST12 a direct or indirect synchronization loop would form
 * > TEST13 unreachable or noselect
 */
int				/* FALSE if fit, TRUE if unfit */
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
		rval |= TEST10;		/* stratum out of bounds */

	/*
	 * A distance error for a remote peer occurs if the root
	 * distance is greater than or equal to the distance threshold
	 * plus the increment due to one host poll interval.
	 */
	if (!(peer->flags & FLAG_REFCLOCK) && root_distance(peer) >=
	    sys_maxdist + clock_phi * ULOGTOD(peer->hpoll))
		rval |= TEST11;		/* distance exceeded */

	/*
	 * A loop error occurs if the remote peer is synchronized to the
	 * local peer of if the remote peer is synchronized to the same
	 * server as the local peer but only if the remote peer is
	 * neither a reference clock nor an orphan.
	 */
	if (peer->stratum > 1 && peer->refid != htonl(LOOPBACKADR) &&
	    (peer->refid == (peer->dstadr ? peer->dstadr->addr_refid :
	    0) || peer->refid == sys_refid))
		rval |= TEST12;		/* synch loop */

	/*
	 * An unreachable error occurs if the server is unreachable or
	 * the noselect bit is set.
	 */
	if (!peer->reach || peer->flags & FLAG_NOSELECT)
		rval |= TEST13;		/* unreachable */

	peer->flash &= ~PEER_TEST_MASK;
	peer->flash |= rval;
	return (rval);
}


/*
 * Find the precision of this particular machine
 */
#define MINSTEP 100e-9		/* minimum clock increment (s) */
#define MAXSTEP 20e-3		/* maximum clock increment (s) */
#define MINLOOPS 5		/* minimum number of step samples */

/*
 * This routine measures the system precision defined as the minimum of
 * a sequence of differences between successive readings of the system
 * clock. However, if a difference is less than MINSTEP, the clock has
 * been read more than once during a clock tick and the difference is
 * ignored. We set MINSTEP greater than zero in case something happens
 * like a cache miss.
 */
int
default_get_precision(void)
{
	l_fp	val;		/* current seconds fraction */
	l_fp	last;		/* last seconds fraction */
	l_fp	diff;		/* difference */
	double	tick;		/* computed tick value */
	double	dtemp;		/* scratch */
	int	i;		/* log2 precision */

	/*
	 * Loop to find precision value in seconds.
	 */
	tick = MAXSTEP;
	i = 0;
	get_systime(&last);
	while (1) {
		get_systime(&val);
		diff = val;
		L_SUB(&diff, &last);
		last = val;
		LFPTOD(&diff, dtemp);
		if (dtemp < MINSTEP)
			continue;

		if (dtemp < tick)
			tick = dtemp;
		if (++i >= MINLOOPS)
			break;
	}

	/*
	 * Find the nearest power of two.
	 */
	NLOG(NLOG_SYSEVENT)
	    msyslog(LOG_INFO, "precision = %.3f usec", tick * 1e6);
	for (i = 0; tick <= 1; i++)
		tick *= 2;
	if (tick - 1. > 1. - tick / 2)
		i--;
	return (-i);
}


/*
 * kod_proto - called once per second to limit kiss-of-death packets
 */
void
kod_proto(void)
{
	sys_kod = sys_kod_rate;
}


/*
 * init_proto - initialize the protocol module's data
 */
void
init_proto(void)
{
	l_fp	dummy;
	int	i;

	/*
	 * Fill in the sys_* stuff.  Default is don't listen to
	 * broadcasting, require authentication.
	 */
	sys_leap = LEAP_NOTINSYNC;
	sys_stratum = STRATUM_UNSPEC;
	sys_peer = NULL;
	sys_rootdelay = 0;
	sys_rootdisp = 0;
	L_CLR(&sys_reftime);
	sys_jitter = 0;
	sys_peer = NULL;

	memcpy(&sys_refid, "INIT", 4);
	sys_precision = (s_char)default_get_precision();
	get_systime(&dummy);
	sys_survivors = 0;
	sys_manycastserver = 0;
	sys_bclient = 0;
	sys_bdelay = 0;
	sys_authenticate = 1;
	L_CLR(&sys_authdelay);
	sys_authdly[0] = sys_authdly[1] = 0;
	sys_stattime = 0;
	proto_clr_stats();
	for (i = 0; i < MAX_TTL; i++) {
		sys_ttl[i] = (u_char)((i * 256) / MAX_TTL);
		sys_ttlmax = i;
	}
	pps_enable = 0;
	stats_control = 1;
}


/*
 * proto_config - configure the protocol module
 */
void
proto_config(
	int	item,
	u_long	value,
	double	dvalue,
	struct sockaddr_storage* svalue
	)
{
	/*
	 * Figure out what he wants to change, then do it
	 */
#ifdef DEBUG
	if (debug > 1)
		printf("proto_config: code %d value %lu dvalue %lf\n",
		    item, value, dvalue);
#endif
	switch (item) {

	/*
	 * enable and disable commands - arguments are Boolean.
	 */
	case PROTO_AUTHENTICATE: /* authentication (auth) */
		sys_authenticate = value;
		break;

	case PROTO_BROADCLIENT: /* broadcast client (bclient) */
		sys_bclient = (int)value;
		if (sys_bclient == 0)
			io_unsetbclient();
		else
			io_setbclient();
		break;

#ifdef REFCLOCK
	case PROTO_CAL:		/* refclock calibrate (calibrate) */
		cal_enable = value;
		break;
#endif /* REFCLOCK */

	case PROTO_KERNEL:	/* kernel discipline (kernel) */
		kern_enable = value;
		break;

	case PROTO_MONITOR:	/* monitoring (monitor) */
		if (value)
			mon_start(MON_ON);
		else
			mon_stop(MON_ON);
		break;

	case PROTO_NTP:		/* NTP discipline (ntp) */
		ntp_enable = value;
		break;

	case PROTO_PPS:		/* PPS discipline (pps) */
		pps_enable = value;
		break;

	case PROTO_FILEGEN:	/* statistics (stats) */
		stats_control = value;
		break;

	/*
	 * tos command - arguments are double, sometimes cast to int
	 */
	case PROTO_BEACON:	/* manycast beacon (beacon) */
		sys_beacon = (int)dvalue;
		break;

	case PROTO_BROADDELAY:	/* default broadcast delay (bdelay) */
		sys_bdelay = dvalue;
		break;

	case PROTO_CEILING:	/* stratum ceiling (ceiling) */
		sys_ceiling = (int)dvalue;
		break;

	case PROTO_MAXHOP:	/* clockhop threshold (clockhop) */
		sys_maxhop = (int)dvalue;
		break;

	case PROTO_COHORT:	/* cohort switch (cohort) */
		sys_cohort = (int)dvalue;
		break;

	case PROTO_FLOOR:	/* stratum floor (floor) */
		sys_floor = (int)dvalue;
		break;

	case PROTO_MAXCLOCK:	/* maximum candidates (maxclock) */
		sys_maxclock = (int)dvalue;
		break;

	case PROTO_MAXDIST:	/* select threshold (maxdist) */
		sys_maxdist = dvalue;
		break;

	case PROTO_CALLDELAY:	/* modem call delay (mdelay) */
		break;		/* NOT USED */

	case PROTO_MINCLOCK:	/* minimum candidates (minclock) */
		sys_minclock = (int)dvalue;
		break;

	case PROTO_MINDISP:	/* distance increment (mindist) */
		sys_mindisp = dvalue;
		break;

	case PROTO_MINSANE:	/* minimum survivors (minsane) */
		sys_minsane = (int)dvalue;
		break;

	case PROTO_ORPHAN:	/* orphan stratum (orphan) */
		sys_orphan = (int)dvalue;
		break;

	case PROTO_ADJ:		/* tick increment (tick) */
		sys_tick = dvalue;
		break;

	/*
	 * Miscellaneous commands
	 */
	case PROTO_MULTICAST_ADD: /* add group address */
		if (svalue)
		    io_multicast_add(*svalue);
		sys_bclient = 1;
		break;

	case PROTO_MULTICAST_DEL: /* delete group address */
		if (svalue)
		    io_multicast_del(*svalue);
		break;

	default:
		msyslog(LOG_INFO,
		    "proto_config: unsupported option %d", item);
	}
}


/*
 * proto_clr_stats - clear protocol stat counters
 */
void
proto_clr_stats(void)
{
	sys_stattime = current_time;
	sys_received = 0;
	sys_processed = 0;
	sys_newversion = 0;
	sys_oldversion = 0;
	sys_declined = 0;
	sys_restricted = 0;
	sys_badlength = 0;
	sys_badauth = 0;
	sys_limitrejected = 0;
}
