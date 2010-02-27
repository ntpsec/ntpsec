/*
 * ntp_monitor - monitor ntpd statistics
 */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_if.h"
#include "ntp_lists.h"
#include "ntp_stdlib.h"
#include <ntp_random.h>

#include <stdio.h>
#include <signal.h>
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

/*
 * Record statistics based on source address, mode and version. The
 * receive procedure calls us with the incoming rbufp before it does
 * anything else. While at it, implement rate controls for inbound
 * traffic.
 *
 * Each entry is doubly linked into two lists, a hash table and a most-
 * recently-used (MRU) list. When a packet arrives it is looked up in
 * the hash table. If found, the statistics are updated and the entry
 * relinked at the head of the MRU list. If not found, a new entry is
 * allocated, initialized and linked into both the hash table and at the
 * head of the MRU list.
 *
 * Memory is usually allocated by grabbing a big chunk of new memory and
 * cutting it up into littler pieces. The exception to this when we hit
 * the memory limit. Then we free memory by grabbing entries off the
 * tail for the MRU list, unlinking from the hash table, and
 * reinitializing.
 */
/* INC_MONLIST is the allocation granularity in entries */
#ifdef MONMEMINC		/* old name */
# define	INC_MONLIST	MONMEMINC
#elif !defined(INC_MONLIST)
# define	INC_MONLIST	(4096 / sizeof(mon_entry))
#endif

/*
 * Hashing stuff
 */
#define	MON_HASH_SIZE	NTP_HASH_SIZE
#define	MON_HASH_MASK	NTP_HASH_MASK
#define	MON_HASH(addr)	NTP_HASH_ADDR(addr)

/*
 * Pointers to the hash table, the MRU list and the count table.  Memory
 * for the hash and count tables is only allocated if monitoring is
 * turned on.
 */
static	mon_entry *mon_hash[MON_HASH_SIZE];  /* list ptrs */
struct	mon_data mon_mru_list;

/*
 * List of free structures structures, and counters of free and total
 * structures. The free structures are linked with the hash_next field.
 */
static  mon_entry *mon_free;		/* free list or null if none */
static	u_int mon_mru_entries;		/* total structures allocated */
static	u_int mon_mem_increments;	/* times called malloc() */

/*
 * Parameters of the RES_LIMITED restriction option. We define headway
 * as the idle time between packets. A packet is discarded if the
 * headway is less than the minimum, as well as if the average headway
 * is less than eight times the increment.
 */
int	ntp_minpkt = NTP_MINPKT;	/* minimum (log 2 s) */
u_char	ntp_minpoll = NTP_MINPOLL;	/* increment (log 2 s) */

/*
 * Initialization state.  We may be monitoring, we may not.  If
 * we aren't, we may not even have allocated any memory yet.
 */
	int	mon_enabled;		/* enable switch */
	u_int	mon_mindepth = 600;	/* preempt above this */
	int	mon_maxage = 64;	/* for entries older than */
	u_int	mon_maxdepth = 		/* MRU size hard limit */
		4 * 1024 * 1024 / sizeof(mon_entry);
	int	mon_age = 3000;		/* preemption limit */
static	int	mon_have_memory;

static	void	mon_getmoremem(void);
static	void	remove_from_hash(mon_entry *);
static	void	mon_reclaim_entry(mon_entry *);


/*
 * init_mon - initialize monitoring global data
 */
void
init_mon(void)
{
	/*
	 * Don't do much of anything here.  We don't allocate memory
	 * until someone explicitly starts us.
	 */
	mon_enabled = MON_OFF;
}


/*
 * mon_start - start up the monitoring software
 */
void
mon_start(
	int mode
	)
{
	if (MON_OFF == mode)		/* MON_OFF is 0 */
		return;
	if (mon_enabled) {
		mon_enabled |= mode;
		return;
	}
	if (!mon_have_memory)
		mon_getmoremem();

	INIT_DLIST(mon_mru_list, mru);
	mon_enabled = mode;
}


/*
 * mon_stop - stop the monitoring software
 */
void
mon_stop(
	int mode
	)
{
	mon_entry *md;
	int i;

	if (MON_OFF == mon_enabled)
		return;
	if ((mon_enabled & mode) == 0 || mode == MON_OFF)
		return;

	mon_enabled &= ~mode;
	if (mon_enabled != MON_OFF)
		return;
	
	/*
	 * Put everything back on the free list
	 */
	for (i = 0; i < MON_HASH_SIZE; i++) {
		while (mon_hash[i] != NULL) {
			UNLINK_HEAD_SLIST(md, mon_hash[i], hash_next);
			memset(md, 0, sizeof(*md));
			LINK_SLIST(mon_free, md, hash_next);
		}
	}
	INIT_DLIST(mon_mru_list, mru);
}


void
mon_clearinterface(
	struct interface *lcladr
	)
{
	mon_entry *mon;

	/* iterate mon over mon_mru_list */
	ITER_DLIST_BEGIN(mon_mru_list, mon, mru, mon_entry)

		if (mon->lcladr == lcladr) {
			/* remove from mru list and hash */
			UNLINK_DLIST(mon, mru);
			remove_from_hash(mon);
			/* put on free list */
			memset(mon, 0, sizeof(*mon));
			LINK_SLIST(mon_free, mon, hash_next);
		}

	ITER_DLIST_END()
}


static void
mon_reclaim_entry(
	mon_entry *m
	)
{
	UNLINK_DLIST(m, mru);
	remove_from_hash(m);
	memset(m, 0, sizeof(*m));
}


/*
 * ntp_monitor - record stats about this packet
 *
 * Returns flags
 */
int
ntp_monitor(
	struct recvbuf *rbufp,
	int	flags
	)
{
	struct pkt *	pkt;
	mon_entry *	md;
	mon_entry *	tail;
	sockaddr_u	addr;
	u_int		hash;
	u_char		mode;
	u_char		version;
	int		interval;

	if (mon_enabled == MON_OFF)
		return (flags);

	pkt = &rbufp->recv_pkt;
	memcpy(&addr, &rbufp->recv_srcadr, sizeof(addr));
	hash = MON_HASH(&addr);
	mode = PKT_MODE(pkt->li_vn_mode);
	version = PKT_VERSION(pkt->li_vn_mode);
	md = mon_hash[hash];
	while (md != NULL) {
		int	head;		/* headway increment */
		int	leak;		/* new headway */
		int	limit;		/* average threshold */

		/*
		 * Match address only to conserve MRU size.
		 */
		if (SOCK_EQ(&md->rmtadr, &addr)) {
			interval = current_time - md->lasttime;
			md->lasttime = current_time;
			md->count++;
			md->flags = flags;
			md->vn_mode = VN_MODE(version, mode);

			/*
			 * Shuffle to the head of the MRU list.
			 */
			UNLINK_DLIST(md, mru);
			LINK_DLIST(mon_mru_list, md, mru);

			/*
			 * At this point the most recent arrival is
			 * first in the MRU list. Decrease the counter
			 * by the headway, but not less than zero.
			 */
			md->leak -= interval;
			if (md->leak < 0)
				md->leak = 0;
			head = 1 << ntp_minpoll;
			leak = md->leak + head;
			limit = NTP_SHIFT * head;
#ifdef DEBUG
			if (debug > 1)
				printf("restrict: interval %d headway %d limit %d\n",
				    interval, leak, limit);
#endif

			/*
			 * If the minimum and average thresholds are not
			 * exceeded, douse the RES_LIMITED and RES_KOD
			 * bits and increase the counter by the headway
			 * increment. Note that we give a 1-s grace for
			 * the minimum threshold and a 2-s grace for the
			 * headway increment. If one or both thresholds
			 * are exceeded and the old counter is less than
			 * the average threshold, set the counter to the
			 * average threshold plus the inrcrment and
			 * leave the RES_KOD bit lit. Othewise, leave
			 * the counter alone and douse the RES_KOD bit.
			 * This rate-limits the KoDs to no less than the
			 * average headway.
			 */
			if (interval + 1 >= (1 << ntp_minpkt) &&
			    leak < limit) {
				md->leak = leak - 2;
				md->flags &= ~(RES_LIMITED | RES_KOD);
			} else if (md->leak < limit)
				md->leak = limit + head;
			else
				md->flags &= ~RES_KOD;

			return md->flags;
		}
		md = md->hash_next;
	}

	/*
	 * If we got here, this is the first we've heard of this
	 * guy.  Get him some memory, either from the free list
	 * or from the tail of the MRU list.
	 * Four ntp.conf "mru" knobs come into play determining the
	 * depth (or count) of the MRU list:
	 * - mon_mindepth ("mru mindepth") is a floor beneath which
	 *   entries are kept without regard to their age.  The
	 *   default is 600 which matches the longtime implementation
	 *   limit on the total number of entries.
	 * - mon_maxage ("mru maxage") is a ceiling on the age in
	 *   seconds of entries.  Entries older than this are
	 *   reclaimed once mon_mindepth is exceeded.  64s default.
	 * - mon_maxdepth ("mru maxdepth") is a hard limit on the
	 *   number of entries.
	 * - "mru maxmem" sets mon_maxdepth to the number of entries
	 *   which fit in the given number of kilobytes.  4096 default.
	 * Whichever of "mru maxmem" or "mru maxdepth" occurs last in
	 * ntp.conf controls.
	 */
	if (NULL == mon_free && mon_mru_entries < mon_mindepth) {
		mon_getmoremem();
		UNLINK_HEAD_SLIST(md, mon_free, hash_next);
	} else {
		tail = TAIL_DLIST(mon_mru_list, mru);
		/* note -1 is legal for mon_maxage (disables) */
		if (tail != NULL && mon_maxage < (int)(current_time -
		    tail->lasttime)) {
			mon_reclaim_entry(tail);
			md = tail;
		} else if (mon_free != NULL || mon_mru_entries <
			   mon_maxdepth) {
			if (NULL == mon_free)
				mon_getmoremem();
			UNLINK_HEAD_SLIST(md, mon_free, hash_next);
		/*
		 * Preempt from the MRU list if old enough.
		 */
		} else if (tail->count == 1 || ntp_random() / (2. *
			   FRAC) > (double)(current_time -
			   tail->lasttime) / mon_age)
			return ~RES_LIMITED & flags;
		else {
			mon_reclaim_entry(tail);
			md = tail;
		}
	}

	/*
	 * Got one, initialize it
	 */
	md->lasttime = md->firsttime = current_time;
	md->count = 1;
	md->flags = flags & ~RES_LIMITED;
	md->leak = 0;
	memcpy(&md->rmtadr, &addr, sizeof(md->rmtadr));
	md->vn_mode = VN_MODE(version, mode);
	md->lcladr = rbufp->dstadr;
	md->cast_flags = (u_char)(((rbufp->dstadr->flags &
	    INT_MCASTOPEN) && rbufp->fd == md->lcladr->fd) ?
	    MDF_MCAST: rbufp->fd == md->lcladr->bfd ? MDF_BCAST :
	    MDF_UCAST);

	/*
	 * Drop him into front of the hash table. Also put him on top of
	 * the MRU list.
	 */
	LINK_SLIST(mon_hash[hash], md, hash_next);
	LINK_DLIST(mon_mru_list, md, mru);

	return md->flags;
}


/*
 * mon_getmoremem - get more memory and put it on the free list
 */
static void
mon_getmoremem(void)
{
	mon_entry *md;
	int i;

	md = emalloc(INC_MONLIST * sizeof(*md));
	memset(md, 0, INC_MONLIST * sizeof(*md));

	for (i = INC_MONLIST - 1; i >= 0; i--)
		LINK_SLIST(mon_free, &md[i], hash_next);

	mon_mru_entries += INC_MONLIST;
	mon_mem_increments++;
	mon_have_memory = 1;
}


static void
remove_from_hash(
	mon_entry *mon
	)
{
	u_int hash;
	mon_entry *punlinked;

	hash = MON_HASH(&mon->rmtadr);
	UNLINK_SLIST(punlinked, mon_hash[hash], mon, hash_next,
		     mon_entry);
	NTP_ENSURE(punlinked == mon);
}
