/*
 * ntp_peer.c - management of data maintained for peer associations
 */
#include "config.h"

#include <stdio.h>
#include <sys/types.h>

#include "ntpd.h"
#include "ntp_lists.h"
#include "ntp_stdlib.h"
#include "ntp_auth.h"


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
static struct peer *peer_hash[NTP_HASH_SIZE];	/* peer hash table */
static int	peer_hash_count[NTP_HASH_SIZE];	/* peers in each bucket */
static struct peer *assoc_hash[NTP_HASH_SIZE];	/* association ID hash table */
static int	assoc_hash_count[NTP_HASH_SIZE];/* peers in each bucket */
struct peer *peer_list;				/* peer structures list */
static struct peer *peer_free;			/* peer structures free list */
static int	peer_free_count;		/* count of free structures */

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
static unsigned long	peer_timereset;		/* time stat counters zeroed */
static unsigned long	findpeer_calls;		/* calls to findpeer */
static unsigned long	assocpeer_calls;	/* calls to findpeerbyassoc */
static unsigned long	peer_allocations;	/* allocations from free list */
static unsigned long	peer_demobilizations;	/* structs freed to free list */
static int		total_peer_structs;	/* peer structs */
int			peer_associations;	/* mobilized associations */
static int		peer_preempt;		/* preemptable associations */
static struct peer init_peer_alloc[INIT_PEER_ALLOC]; /* init alloc */

static struct peer *	findexistingpeer_name(const char *, unsigned short,
					      struct peer *, int);
static struct peer *	findexistingpeer_addr(sockaddr_u *,
					      struct peer *, int);
static void		free_peer(struct peer *);
static void		getmorepeermem(void);
static	void		peer_reset	(struct peer *);
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
	unsigned short	hname_fam,
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

	DPRINT(2, ("findexistingpeer_addr(%s, %s, %d)\n",
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
		DPRINT(3, ("%s %s %d %d 0x%x ", sockporttoa(addr),
			   sockporttoa(&peer->srcadr), mode, peer->hmode,
			   (unsigned int)peer->cast_flags));
		if ((-1 == mode || peer->hmode == mode) &&
		    ADDR_PORT_EQ(addr, &peer->srcadr)) {
			DPRINT(3, ("found.\n"));
			break;
		}
		DPRINT(3, ("\n"));
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
		return findexistingpeer_name(hostname,
					     (addr == NULL) ? AF_UNSPEC : AF(addr),
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
	struct recvbuf *rbufp
	)
{
	struct peer *	p;
	sockaddr_u *	srcadr;
	unsigned int	hash;

	findpeer_calls++;
	srcadr = &rbufp->recv_srcadr;
	hash = NTP_HASH_ADDR(srcadr);
        for (p = peer_hash[hash]; p != NULL; p = p->adr_link) {
                /* [Classic Bug 3072] ensure interface of peer matches */
                if (p->dstadr != rbufp->dstadr) continue;

                /* ensure peer source address matches */
                if (!ADDR_PORT_EQ(srcadr, &p->srcadr)) continue;

		return p;
        }

	return NULL;
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
	unsigned int hash;

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

	DPRINT(1, ("clear_all: at %u\n", current_time));
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
		if (speer->cfg.flags & FLAG_PREEMPT) {
			x = score(speer);
			if (x < temp)
				temp = x;
		}
	DPRINT(1, ("score_all: at %u score %d min %d\n",
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
	struct peer *	p
	)
{
	struct peer *	unlinked;
	int		hash;


	if ((MDF_UCAST & p->cast_flags) && !(FLAG_DNS & p->cfg.flags)) {
		hash = NTP_HASH_ADDR(&p->srcadr);
		peer_hash_count[hash]--;

		UNLINK_SLIST(unlinked, peer_hash[hash], p, adr_link,
			     struct peer);
		if (NULL == unlinked) {
			peer_hash_count[hash]++;
			msyslog(LOG_ERR, "ERR: peer %s not in address table!",
				socktoa(&p->srcadr));
		}
	}

	/* Remove him from the association hash as well. */
	hash = p->associd & NTP_HASH_MASK;
	assoc_hash_count[hash]--;

	UNLINK_SLIST(unlinked, assoc_hash[hash], p, aid_link,
		     struct peer);
	if (NULL == unlinked) {
		assoc_hash_count[hash]++;
		msyslog(LOG_ERR,
			"ERR: peer %s not in association ID table!",
			socktoa(&p->srcadr));
	}

	/* Remove him from the overall list. */
	UNLINK_SLIST(unlinked, peer_list, p, p_link,
		     struct peer);
	if (NULL == unlinked)
		msyslog(LOG_ERR, "ERR: %s not in peer list!",
			socktoa(&p->srcadr));

	if (p->hostname != NULL)
		free(p->hostname);

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
	if (FLAG_PREEMPT & peer->cfg.flags)
		peer_preempt--;
#ifdef REFCLOCK
	/*
	 * If this peer is actually a clock, shut it down first
	 */
	if (FLAG_REFCLOCK & peer->cfg.flags)
		refclock_unpeer(peer);
#endif

	free_peer(peer);
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
	if (dstadr != NULL && (FLAG_BC_VOL & p->cfg.flags) &&
	    (INT_MCASTIF & dstadr->flags) && MODE_CLIENT == p->hmode) {
		return;
	}
	if (p->dstadr != NULL) {
		p->dstadr->peercnt--;
		UNLINK_SLIST(unlinked, p->dstadr->peers, p, ilink,
			     struct peer);
		msyslog(LOG_INFO, "PROTO: %s unlink local addr %s -> %s",
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
void
peer_refresh_interface(
	struct peer *p
	)
{
	endpt *	niface;

	niface = select_peerinterface(p, &p->srcadr, NULL);

	DPRINT(4, (
		   "peer_refresh_interface: %s->%s mode %d vers %d poll %d %d flags 0x%x 0x%x mode %u key %08x: new interface: ",
		   p->dstadr == NULL ? "<null>" :
		   socktoa(&p->dstadr->sin), socktoa(&p->srcadr), p->hmode,
		   p->cfg.version, p->cfg.minpoll, p->cfg.maxpoll, p->cfg.flags, p->cast_flags,
		   p->cfg.mode, p->cfg.peerkey));
	if (niface != NULL) {
		DPRINT(4, (
			   "fd=%d, name=%.16s, flags=0x%x, ifindex=%u, sin=%s",
			   niface->fd, niface->name,
			   niface->flags, niface->ifindex,
			   socktoa(&niface->sin)));
		if (niface->flags & INT_BROADCAST)
			DPRINT(4, (", bcast=%s",
				   socktoa(&niface->bcast)));
		DPRINT(4, (", mask=%s\n", socktoa(&niface->mask)));
	} else {
		DPRINT(4, ("<NONE>\n"));
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
		if ((p->dstadr) && (p->reach & 0x3))	// Classic Bug 2849 XOR 2043
			/* either of last 2 tries with this dstadr worked */
			continue;
		if (MDF_POOL & p->cast_flags)
			continue;	/* Pool slots don't get interfaces. */
		if (FLAG_DNS & p->cfg.flags)
			continue;	/* Still doing DNS lookup. */
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
	struct peer_ctl *ctl,
	uint8_t		cast_flags,
	bool		initializing1
	)
{
	struct peer *	peer;
	unsigned int	hash;
	const char *	name;	/* for error messages */

	if (NULL != hostname)
		name = hostname;
	else
		name = socktoa(srcadr);

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
		DPRINT(2, ("newpeer(%s) found existing association\n", name));
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
	if (FLAG_PREEMPT & ctl->flags)
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

	/*
	 * Copy in the peer configuration block.
	 */
	memcpy(&peer->cfg, ctl, sizeof(peer->cfg));

	peer->cast_flags = cast_flags;
	set_peerdstadr(peer,
		       select_peerinterface(peer, srcadr, dstadr));

        if (NTP_MAXPOLL_UNK == peer->cfg.maxpoll)
	    /* not set yet, set to default */
	    peer->cfg.maxpoll = NTP_MAXDPOLL;
	/*
         * minpoll is clamped not greater than NTP_MAXPOLL
         * maxpoll is clamped not less than NTP_MINPOLL
         * minpoll is clamped not greater than maxpoll.
	 */
	peer->cfg.minpoll = min(peer->cfg.minpoll, NTP_MAXPOLL);
	peer->cfg.maxpoll = max(peer->cfg.maxpoll, NTP_MINPOLL);
	if (peer->cfg.minpoll > peer->cfg.maxpoll)
		peer->cfg.minpoll = peer->cfg.maxpoll;

	if (peer->dstadr != NULL)
		DPRINT(3, ("newpeer(%s): using fd %d and our addr %s\n",
			   socktoa(srcadr), peer->dstadr->fd,
			   socktoa(&peer->dstadr->sin)));
	else
		DPRINT(3, ("newpeer(%s): local interface currently not bound\n",
			   socktoa(srcadr)));

	/* if a key specified, verify that it will work */
	if (0 != peer->cfg.peerkey) {
		if (NULL == authlookup(peer->cfg.peerkey, false))
			msyslog(LOG_ERR, "ERR: key %u not found for server %s",
				peer->cfg.peerkey, name);
		else if (NULL == authlookup(peer->cfg.peerkey, true))
			msyslog(LOG_ERR, "ERR: key %u found but not trusted for server %s",
				peer->cfg.peerkey, name);
		}

	peer->precision = sys_vars.sys_precision;
	peer->hpoll = peer->cfg.minpoll;
	if (cast_flags & MDF_POOL)
		peer_clear(peer, "POOL", initializing1);
	else
		peer_clear(peer, "INIT", initializing1);
	if (clock_ctl.mode_ntpdate)
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
	if ((MDF_UCAST & cast_flags) && !(FLAG_DNS & ctl->flags))
		peer_update_hash(peer);
	hash = peer->associd & NTP_HASH_MASK;
	LINK_SLIST(assoc_hash[hash], peer, aid_link);
	assoc_hash_count[hash]++;
	LINK_SLIST(peer_list, peer, p_link);

	restrict_source(&peer->srcadr, false, 0);
	mprintf_event(PEVNT_MOBIL, peer, "assoc %d", peer->associd);
	DPRINT(1, ("newpeer: %s->%s mode %u vers %u poll %u %u flags 0x%x 0x%x mode %u key %08x\n",
		   latoa(peer->dstadr), socktoa(&peer->srcadr), peer->hmode,
		   peer->cfg.version, peer->cfg.minpoll, peer->cfg.maxpoll, peer->cfg.flags,
		   peer->cast_flags, peer->cfg.mode, peer->cfg.peerkey));
	return peer;
}

void peer_update_hash (struct peer *peer)
{
	unsigned int	hash;

	hash = NTP_HASH_ADDR(&peer->srcadr);
	LINK_SLIST(peer_hash[hash], peer, adr_link);
	peer_hash_count[hash]++;
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
static void
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
