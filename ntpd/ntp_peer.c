/*
 * ntp_peer.c - management of data maintained for peer associations
 */
#include "config.h"

#include <stdio.h>
#include <sys/types.h>

#include "ntpd.h"
#include "ntp_lists.h"
#include "ntp_stdlib.h"
#include "ntp_random.h"

/*
 *		    Table of valid association combinations
 *		    ---------------------------------------
 *
 *                             packet->mode
 * peer->mode      | UNSPEC  ACTIVE PASSIVE  CLIENT  SERVER  BCAST
 * ----------      | ---------------------------------------------
 * NO_PEER         |   e       1       0       1       1       1
 * ACTIVE          |   e       1       1       0       0       0
 * PASSIVE         |   e       1       e       0       0       0
 * CLIENT          |   e       0       0       0       1       0
 * SERVER          |   e       0       0       0       0       0
 * BCAST           |   e       0       0       0       0       0
 * BCLIENT         |   e       0       0       0       e       1
 *
 * One point to note here: a packet in BCAST mode can potentially match
 * a peer in CLIENT mode, but we that is a special case and we check for
 * that early in the decision process.  This avoids having to keep track
 * of what kind of associations are possible etc...  We actually
 * circumvent that problem by requiring that the first b(m)roadcast
 * received after the change back to BCLIENT mode sets the clock.
 */
#define AM_MODES	7	/* number of rows and columns */
#define NO_PEER		0	/* action when no peer is found */

static int AM[AM_MODES][AM_MODES] = {
/*			packet->mode					    */
/* peer { UNSPEC,   ACTIVE,     PASSIVE,    CLIENT,     SERVER,     BCAST } */
/* mode */
/*NONE*/{ AM_ERR, AM_NEWPASS, AM_NOMATCH, AM_FXMIT,   AM_MANYCAST, AM_NEWBCL},

/*A*/	{ AM_ERR, AM_PROCPKT, AM_PROCPKT, AM_NOMATCH, AM_NOMATCH,  AM_NOMATCH},

/*P*/	{ AM_ERR, AM_PROCPKT, AM_ERR,     AM_NOMATCH, AM_NOMATCH,  AM_NOMATCH},

/*C*/	{ AM_ERR, AM_NOMATCH, AM_NOMATCH, AM_NOMATCH, AM_PROCPKT,  AM_NOMATCH},

/*S*/	{ AM_ERR, AM_NOMATCH, AM_NOMATCH, AM_NOMATCH, AM_NOMATCH,  AM_NOMATCH},

/*BCST*/{ AM_ERR, AM_NOMATCH, AM_NOMATCH, AM_NOMATCH, AM_NOMATCH,  AM_NOMATCH},

/*BCL*/ { AM_ERR, AM_NOMATCH, AM_NOMATCH, AM_NOMATCH, AM_NOMATCH,  AM_PROCPKT},
};

#define MATCH_ASSOC(x, y)	AM[(x)][(y)]

/*
 * These routines manage the allocation of memory to peer structures
 * and the maintenance of three data structures involving all peers:
 *
 * - peer_list is a single list with all peers, suitable for scanning
 *   operations over all peers.
 * - peer_adr_hash is an array of lists indexed by hashed peer address.
 * - peer_aid_hash is an array of lists indexed by hashed associd.
 *
 * They also maintain a free list of peer structures, peer_free.
 *
 * The three main entry points are findpeer(), which looks for matching
 * peer structures in the peer list, newpeer(), which allocates a new
 * peer structure and adds it to the list, and unpeer(), which
 * demobilizes the association and deallocates the structure.
 */
/*
 * Peer hash tables
 */
struct peer *peer_hash[NTP_HASH_SIZE];	/* peer hash table */
int	peer_hash_count[NTP_HASH_SIZE];	/* peers in each bucket */
struct peer *assoc_hash[NTP_HASH_SIZE];	/* association ID hash table */
int	assoc_hash_count[NTP_HASH_SIZE];/* peers in each bucket */
struct peer *peer_list;			/* peer structures list */
static struct peer *peer_free;		/* peer structures free list */
int	peer_free_count;		/* count of free structures */

/*
 * Association ID.  We initialize this value randomly, then assign a new
 * value every time an association is mobilized.
 */
static associd_t current_association_ID; /* association ID */
static associd_t initial_association_ID; /* association ID */

/*
 * Memory allocation watermarks.
 */
#define	INIT_PEER_ALLOC		8	/* static preallocation */
#define	INC_PEER_ALLOC		4	/* add N more when empty */

/*
 * Miscellaneous statistic counters which may be queried.
 */
u_long	peer_timereset;			/* time stat counters zeroed */
u_long	findpeer_calls;			/* calls to findpeer */
u_long	assocpeer_calls;		/* calls to findpeerbyassoc */
u_long	peer_allocations;		/* allocations from free list */
u_long	peer_demobilizations;		/* structs freed to free list */
int	total_peer_structs;		/* peer structs */
int	peer_associations;		/* mobilized associations */
int	peer_preempt;			/* preemptable associations */
static struct peer init_peer_alloc[INIT_PEER_ALLOC]; /* init alloc */

static struct peer *	findexistingpeer_name(const char *, u_short,
					      struct peer *, int);
static struct peer *	findexistingpeer_addr(sockaddr_u *,
					      struct peer *, int);
static void		free_peer(struct peer *, int);
static void		getmorepeermem(void);
static int		score(struct peer *);


/*
 * init_peer - initialize peer data structures and counters
 *
 * N.B. We use the random number routine in here. It had better be
 * initialized prior to getting here.
 */
void
init_peer(void)
{
	int i;

	/*
	 * Initialize peer free list from static allocation.
	 */
	for (i = COUNTOF(init_peer_alloc) - 1; i >= 0; i--)
		LINK_SLIST(peer_free, &init_peer_alloc[i], p_link);
	total_peer_structs = COUNTOF(init_peer_alloc);
	peer_free_count = COUNTOF(init_peer_alloc);

	/*
	 * Initialize our first association ID
	 */
	do
		current_association_ID = ntp_random() & ASSOCID_MAX;
	while (!current_association_ID);
	initial_association_ID = current_association_ID;
}


/*
 * getmorepeermem - add more peer structures to the free list
 */
static void
getmorepeermem(void)
{
	int i;
	struct peer *peers;

	peers = emalloc_zero(INC_PEER_ALLOC * sizeof(*peers));

	for (i = INC_PEER_ALLOC - 1; i >= 0; i--)
		LINK_SLIST(peer_free, &peers[i], p_link);

	total_peer_structs += INC_PEER_ALLOC;
	peer_free_count += INC_PEER_ALLOC;
}


static struct peer *
findexistingpeer_name(
	const char *	hostname,
	u_short		hname_fam,
	struct peer *	start_peer,
	int		mode
	)
{
	struct peer *p;

	if (NULL == start_peer)
		p = peer_list;
	else
		p = start_peer->p_link;
	for (; p != NULL; p = p->p_link)
		if (p->hostname != NULL
		    && (-1 == mode || p->hmode == mode)
		    && (AF_UNSPEC == hname_fam
			|| AF_UNSPEC == AF(&p->srcadr)
			|| hname_fam == AF(&p->srcadr))
		    && !strcasecmp(p->hostname, hostname))
			break;
	return p;
}


static
struct peer *		
findexistingpeer_addr(
	sockaddr_u *	addr,
	struct peer *	start_peer,
	int		mode
	)
{
	struct peer *peer;

	DPRINTF(2, ("findexistingpeer_addr(%s, %s, %d)\n",
		sockporttoa(addr),
		(start_peer)
		    ? sockporttoa(&start_peer->srcadr)
		    : "NULL",
		    mode));

	/*
	 * start_peer is included so we can locate instances of the
	 * same peer through different interfaces in the hash table.
	 * A match requires the same mode and remote
	 * address. 
	 */
	if (NULL == start_peer)
		peer = peer_hash[NTP_HASH_ADDR(addr)];
	else
		peer = start_peer->adr_link;
	
	while (peer != NULL) {
		DPRINTF(3, ("%s %s %d %d 0x%x ", sockporttoa(addr),
			sockporttoa(&peer->srcadr), mode, peer->hmode,
			(u_int)peer->cast_flags));
		if ((-1 == mode || peer->hmode == mode) &&
		    ADDR_PORT_EQ(addr, &peer->srcadr)) {
			DPRINTF(3, ("found.\n"));
			break;
		}
		DPRINTF(3, ("\n"));
		peer = peer->adr_link;
	}

	return peer;
}


/*
 * findexistingpeer - search by address and return a pointer to a peer.
 */
struct peer *
findexistingpeer(
	sockaddr_u *	addr,
	const char *	hostname,
	struct peer *	start_peer,
	int		mode)
{
	if (hostname != NULL)
		return findexistingpeer_name(hostname, AF(addr),
					     start_peer, mode);
	else
		return findexistingpeer_addr(addr, start_peer, mode);
}


/*
 * findpeer - find and return a peer match for a received datagram in
 *	      the peer_hash table.
 */
struct peer *
findpeer(
	struct recvbuf *rbufp,
	int		pkt_mode,
	int *		action
	)
{
	struct peer *	p;
	sockaddr_u *	srcadr;
	u_int		hash;
	struct pkt *	pkt;
	l_fp		pkt_org;

	findpeer_calls++;
	srcadr = &rbufp->recv_srcadr;
	hash = NTP_HASH_ADDR(srcadr);
        for (p = peer_hash[hash]; p != NULL; p = p->adr_link) {
                /* [Classic Bug 3072] ensure interface of peer matches */
                if (p->dstadr != rbufp->dstadr) continue;

                /* ensure peer source address matches */
                if (!ADDR_PORT_EQ(srcadr, &p->srcadr)) continue;

                /* If the association matching rules determine that this
                 * is not a valid combination, then look for the next
                 * valid peer association.
                 */
                *action = MATCH_ASSOC(p->hmode, pkt_mode);

                /* A response to our manycastclient solicitation might
                 * be misassociated with an ephemeral peer already spun
                 * for the server.  If the packet's org timestamp
                 * doesn't match the peer's, check if it matches the
                 * ACST prototype peer's.  If so it is a redundant
                 * solicitation response, return AM_ERR to discard it.
                 * [Classic Bug 1762]
                 */
                if (MODE_SERVER == pkt_mode && AM_PROCPKT == *action) {
                        pkt = &rbufp->recv_pkt;
                        pkt_org = ntohl_fp(pkt->org);
                        if (p->org != pkt_org && findmanycastpeer(rbufp))
                                *action = AM_ERR;
                }

                /* If an error was returned, exit back right here. */
                if (*action == AM_ERR) return NULL;

                /* If a match is found, we stop our search. */
                if (*action != AM_NOMATCH) break;
        }

	/* If no matching association is found */
	if (NULL == p) *action = MATCH_ASSOC(NO_PEER, pkt_mode);
	return p;
}

/*
 * findpeerbyassoc - find and return a peer using his association ID
 */
struct peer *
findpeerbyassoc(
	associd_t assoc
	)
{
	struct peer *p;
	u_int hash;

	assocpeer_calls++;
	hash = assoc & NTP_HASH_MASK;
	for (p = assoc_hash[hash]; p != NULL; p = p->aid_link)
		if (assoc == p->associd)
			break;
	return p;
}


/*
 * clear_all - flush all time values for all associations
 */
void
clear_all(void)
{
	struct peer *p;

	/*
	 * This routine is called when the clock is stepped, and so all
	 * previously saved time values are untrusted.
	 */
	for (p = peer_list; p != NULL; p = p->p_link)
		if (!(MDF_TXONLY_MASK & p->cast_flags))
		    peer_clear(p, "STEP", false);

	DPRINTF(1, ("clear_all: at %lu\n", current_time));
}


/*
 * score_all() - determine if an association can be demobilized
 */
int
score_all(
	struct peer *peer	/* peer structure pointer */
	)
{
	struct peer *speer;
	int	temp, tamp;
	int	x;

	/*
	 * This routine finds the minimum score for all preemptible
	 * associations and returns > 0 if the association can be
	 * demobilized.
	 */
	tamp = score(peer);
	temp = 100;
	for (speer = peer_list; speer != NULL; speer = speer->p_link)
		if (speer->flags & FLAG_PREEMPT) {
			x = score(speer);
			if (x < temp)
				temp = x;
		}
	DPRINTF(1, ("score_all: at %lu score %d min %d\n",
		    current_time, tamp, temp));

	if (tamp != temp)
		temp = 0;

	return temp;
}


/*
 * score() - calculate preemption score
 */
static int
score(
	struct peer *peer	/* peer structure pointer */
	)
{
	int	temp;

	/*
	 * This routine calculates the premption score from the peer
	 * error bits and status. Increasing values are more cherished.
	 */
	temp = 0;
	if (!(peer->flash & BOGON10))
		temp++;			/* 1 good synch and stratum */
	if (!(peer->flash & BOGON13))
		temp++;			/* 2 reachable */
	if (!(peer->flash & BOGON12))
		temp++;			/* 3 no loop */
	if (!(peer->flash & BOGON11))
		temp++;			/* 4 good distance */
	if (peer->status >= CTL_PST_SEL_SELCAND)
		temp++;			/* 5 in the hunt */
	if (peer->status != CTL_PST_SEL_EXCESS)
		temp++;			/* 6 not spare tire */
	return (temp);			/* selection status */
}


/*
 * free_peer - internal routine to free memory referred to by a struct
 *	       peer and return it to the peer free list.  If unlink is
 *	       nonzero, unlink from the various lists.
 */
static void
free_peer(
	struct peer *	p,
	int		unlink_peer
	)
{
	struct peer *	unlinked;
	int		hash;

	if (unlink_peer) {
		hash = NTP_HASH_ADDR(&p->srcadr);
		peer_hash_count[hash]--;

		UNLINK_SLIST(unlinked, peer_hash[hash], p, adr_link,
			     struct peer);
		if (NULL == unlinked) {
			peer_hash_count[hash]++;
			msyslog(LOG_ERR, "peer %s not in address table!",
				socktoa(&p->srcadr));
		}

		/*
		 * Remove him from the association hash as well.
		 */
		hash = p->associd & NTP_HASH_MASK;
		assoc_hash_count[hash]--;

		UNLINK_SLIST(unlinked, assoc_hash[hash], p, aid_link,
			     struct peer);
		if (NULL == unlinked) {
			assoc_hash_count[hash]++;
			msyslog(LOG_ERR,
				"peer %s not in association ID table!",
				socktoa(&p->srcadr));
		}

		/* Remove him from the overall list. */
		UNLINK_SLIST(unlinked, peer_list, p, p_link,
			     struct peer);
		if (NULL == unlinked)
			msyslog(LOG_ERR, "%s not in peer list!",
				socktoa(&p->srcadr));
	}

	if (p->hostname != NULL)
		free(p->hostname);

	if (p->addrs != NULL)
		free(p->addrs);		/* from copy_addrinfo_list() */

	/* Add his corporeal form to peer free list */
	ZERO(*p);
	LINK_SLIST(peer_free, p, p_link);
	peer_free_count++;
}


/*
 * unpeer - remove peer structure from hash table and free structure
 */
void
unpeer(
	struct peer *peer
	)
{
	mprintf_event(PEVNT_DEMOBIL, peer, "assoc %u", peer->associd);
	restrict_source(&peer->srcadr, true, 0);
	set_peerdstadr(peer, NULL);
	peer_demobilizations++;
	peer_associations--;
	if (FLAG_PREEMPT & peer->flags)
		peer_preempt--;
#ifdef REFCLOCK
	/*
	 * If this peer is actually a clock, shut it down first
	 */
	if (FLAG_REFCLOCK & peer->flags)
		refclock_unpeer(peer);
#endif

	free_peer(peer, true);
}


/*
 * setup peer dstadr field keeping it in sync with the interface
 * structures
 */
void
set_peerdstadr(
	struct peer *	p,
	endpt *		dstadr
	)
{
	struct peer *	unlinked;

	if (p == NULL || p->dstadr == dstadr)
		return;

	/*
	 * Don't accept updates to a separate multicast receive-only
	 * endpt while a BCLNT peer is running its unicast protocol.
	 */
	if (dstadr != NULL && (FLAG_BC_VOL & p->flags) &&
	    (INT_MCASTIF & dstadr->flags) && MODE_CLIENT == p->hmode) {
		return;
	}
	if (p->dstadr != NULL) {
		p->dstadr->peercnt--;
		UNLINK_SLIST(unlinked, p->dstadr->peers, p, ilink,
			     struct peer);
		msyslog(LOG_INFO, "%s local addr %s -> %s",
			socktoa(&p->srcadr), latoa(p->dstadr),
			latoa(dstadr));
	}
	p->dstadr = dstadr;
	if (dstadr != NULL) {
		LINK_SLIST(dstadr->peers, p, ilink);
		dstadr->peercnt++;
	}
}

/*
 * attempt to re-rebind interface if necessary
 */
static void
peer_refresh_interface(
	struct peer *p
	)
{
	endpt *	niface;

	niface = select_peerinterface(p, &p->srcadr, NULL);

	DPRINTF(4, (
	    "peer_refresh_interface: %s->%s mode %d vers %d poll %d %d flags 0x%x 0x%x ttl %u key %08x: new interface: ",
	    p->dstadr == NULL ? "<null>" :
	    socktoa(&p->dstadr->sin), socktoa(&p->srcadr), p->hmode,
	    p->version, p->minpoll, p->maxpoll, p->flags, p->cast_flags,
	    p->ttl, p->keyid));
	if (niface != NULL) {
		DPRINTF(4, (
		    "fd=%d, bfd=%d, name=%.16s, flags=0x%x, ifindex=%u, sin=%s",
		    niface->fd,  niface->bfd, niface->name,
		    niface->flags, niface->ifindex,
		    socktoa(&niface->sin)));
		if (niface->flags & INT_BROADCAST)
			DPRINTF(4, (", bcast=%s",
				socktoa(&niface->bcast)));
		DPRINTF(4, (", mask=%s\n", socktoa(&niface->mask)));
	} else {
		DPRINTF(4, ("<NONE>\n"));
	}

	set_peerdstadr(p, niface);
}


/*
 * refresh_all_peerinterfaces - see that all interface bindings are up
 * to date
 */
void
refresh_all_peerinterfaces(void)
{
	struct peer *p;

	/*
	 * this is called when the interface list has changed
	 * give all peers a chance to find a better interface
	 * but only if either they don't have an address already
	 * or if the one they have hasn't worked for a while.
	 */
	for (p = peer_list; p != NULL; p = p->p_link) {
		if (!(p->dstadr && (p->reach & 0x3)))	// Bug 2849 XOR 2043
			peer_refresh_interface(p);
	}
}


/*
 * newpeer - initialize a new peer association
 *
 * RETURN: a pointer to the new peer structure
 *         NULL if this would duplicate an existing peer
 */
struct peer *
newpeer(
	sockaddr_u *	srcadr,
	const char *	hostname,
	endpt *		dstadr,
	uint8_t		hmode,
	uint8_t		version,
	uint8_t		minpoll,
	uint8_t		maxpoll,
	u_int		flags,
	uint8_t		cast_flags,
	uint32_t		ttl,
	keyid_t		key,
	bool		initializing1
	)
{
	struct peer *	peer;
	u_int		hash;

	/*
	 * For now only pool associations have a hostname.
	 */
	NTP_INSIST(NULL == hostname || (MDF_POOL & cast_flags));

	/*
	 * First search from the beginning for an association with given
	 * remote address and mode. If an interface is given, search
	 * from there to find the association which matches that
	 * destination. If the given interface is "any", track down the
	 * actual interface, because that's what gets put into the peer
	 * structure.
	 */
	if (dstadr != NULL) {
		peer = findexistingpeer(srcadr, hostname, NULL, hmode);
		while (peer != NULL) {
			if (peer->dstadr == dstadr ||
			    ((MDF_BCLNT & cast_flags) &&
			     (MDF_BCLNT & peer->cast_flags)))
				break;

			if (dstadr == ANY_INTERFACE_CHOOSE(srcadr) &&
			    peer->dstadr == findinterface(srcadr))
				break;

			peer = findexistingpeer(srcadr, hostname, peer,	hmode);
		}
	} else {
		/* no endpt address given */
		peer = findexistingpeer(srcadr, hostname, NULL, hmode);
	}

	/*
	 * If a peer is found, this would be a duplicate and we don't
	 * allow that. This avoids duplicate ephemeral (broadcast/
	 * multicast) and preemptible (manycast and pool) client
	 * associations.
	 */
	if (peer != NULL) {
		DPRINTF(2, ("newpeer(%s) found existing association\n",
			(hostname)
			    ? hostname
			    : socktoa(srcadr)));
		return NULL;
	}

	/*
	 * Allocate a new peer structure. Some dirt here, since some of
	 * the initialization requires knowledge of our system state.
	 */
	if (peer_free_count == 0)
		getmorepeermem();
	UNLINK_HEAD_SLIST(peer, peer_free, p_link);
	peer_free_count--;
	peer_associations++;
	if (FLAG_PREEMPT & flags)
		peer_preempt++;

	/*
	 * Assign an association ID and increment the system variable.
	 */
	peer->associd = current_association_ID;
	if (++current_association_ID == 0)
		++current_association_ID;

	peer->srcadr = *srcadr;
	if (hostname != NULL)
		peer->hostname = estrdup(hostname);
	peer->hmode = hmode;
	peer->version = version;
	peer->flags = flags;
	peer->cast_flags = cast_flags;
	set_peerdstadr(peer, 
		       select_peerinterface(peer, srcadr, dstadr));

        if (NTP_MAXPOLL_UNK == maxpoll)
	    /* not set yet, set to default */
	    maxpoll = NTP_MAXDPOLL;
	/*
         * minpoll is clamped not greater than NTP_MAXPOLL
         * maxpoll is clamped not less than NTP_MINPOLL
         * minpoll is clamped not greater than maxpoll.
	 */
	peer->minpoll = min(minpoll, NTP_MAXPOLL);
	peer->maxpoll = max(maxpoll, NTP_MINPOLL);
	if (peer->minpoll > peer->maxpoll)
		peer->minpoll = peer->maxpoll;

	if (peer->dstadr != NULL)
		DPRINTF(3, ("newpeer(%s): using fd %d and our addr %s\n",
			socktoa(srcadr), peer->dstadr->fd,
			socktoa(&peer->dstadr->sin)));
	else
		DPRINTF(3, ("newpeer(%s): local interface currently not bound\n",
			socktoa(srcadr)));

	/*
	 * Broadcast needs the socket enabled for broadcast
	 */
	if ((MDF_BCAST & cast_flags) && peer->dstadr != NULL)
		enable_broadcast(peer->dstadr, srcadr);

	peer->ttl = ttl;
	peer->keyid = key;
	peer->precision = sys_precision;
	peer->hpoll = peer->minpoll;
	if (cast_flags & MDF_POOL)
		peer_clear(peer, "POOL", initializing1);
	else if (cast_flags & MDF_BCAST)
		peer_clear(peer, "BCST", initializing1);
	else
		peer_clear(peer, "INIT", initializing1);
	if (mode_ntpdate)
		peer_ntpdate++;

	/*
	 * Note time on statistics timers.
	 */
	peer->timereset = current_time;
	peer->timereachable = current_time;
	peer->timereceived = current_time;

	/*
	 * Put the new peer in the hash tables.
	 */
	hash = NTP_HASH_ADDR(&peer->srcadr);
	LINK_SLIST(peer_hash[hash], peer, adr_link);
	peer_hash_count[hash]++;
	hash = peer->associd & NTP_HASH_MASK;
	LINK_SLIST(assoc_hash[hash], peer, aid_link);
	assoc_hash_count[hash]++;
	LINK_SLIST(peer_list, peer, p_link);

	restrict_source(&peer->srcadr, false, 0);
	mprintf_event(PEVNT_MOBIL, peer, "assoc %d", peer->associd);
	DPRINTF(1, ("newpeer: %s->%s mode %u vers %u poll %u %u flags 0x%x 0x%x ttl %u key %08x\n",
	    latoa(peer->dstadr), socktoa(&peer->srcadr), peer->hmode,
	    peer->version, peer->minpoll, peer->maxpoll, peer->flags,
	    peer->cast_flags, peer->ttl, peer->keyid));
	return peer;
}


/*
 * peer_clr_stats - clear peer module statistics counters
 */
void
peer_clr_stats(void)
{
	findpeer_calls = 0;
	assocpeer_calls = 0;
	peer_allocations = 0;
	peer_demobilizations = 0;
	peer_timereset = current_time;
}


/*
 * peer_reset - reset statistics counters
 */
void
peer_reset(
	struct peer *peer
	)
{
	if (peer == NULL)
		return;

	peer->timereset = current_time;
	peer->sent = 0;
	peer->received = 0;
	peer->processed = 0;
	peer->badauth = 0;
	peer->bogusorg = 0;
	peer->oldpkt = 0;
	peer->seldisptoolarge = 0;
	peer->selbroken = 0;
}


/*
 * peer_all_reset - reset all peer statistics counters
 */
void
peer_all_reset(void)
{
	struct peer *peer;

	for (peer = peer_list; peer != NULL; peer = peer->p_link)
		peer_reset(peer);
}


/*
 * findmanycastpeer - find and return a manycastclient or pool
 *		      association matching a received response.
 */
struct peer *
findmanycastpeer(
	struct recvbuf *rbufp	/* receive buffer pointer */
	)
{
	struct peer *peer;
	struct pkt *pkt;
	l_fp p_org;

	/*
	 * This routine is called upon arrival of a server-mode response
	 * to a manycastclient multicast solicitation, or to a pool
	 * server unicast solicitation.  Search the peer list for a
	 * manycastclient association where the last transmit timestamp
	 * matches the response packet's originate timestamp.  There can
	 * be multiple manycastclient associations, or multiple pool
	 * solicitation assocations, so this assumes the transmit
	 * timestamps are unique for such.
	 */
	pkt = &rbufp->recv_pkt;
	for (peer = peer_list; peer != NULL; peer = peer->p_link)
		if (MDF_SOLICIT_MASK & peer->cast_flags) {
			p_org = ntohl_fp(pkt->org);
			if (p_org == peer->org)
				break;
		}

	return peer;
}

/* peer_cleanup - clean peer list prior to shutdown */
void peer_cleanup(void)
{
        struct peer *peer;
        associd_t assoc;

        for (assoc = initial_association_ID; assoc != current_association_ID; assoc++) {
            if (assoc != 0U) {
                peer = findpeerbyassoc(assoc);
                if (peer != NULL)
                    unpeer(peer);
            }
        }
        peer = findpeerbyassoc(current_association_ID);
        if (peer != NULL)
            unpeer(peer);
}
