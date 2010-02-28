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
 *
 * INC_MONLIST is the default allocation granularity in entries.
 * INIT_MONLIST is the default initial allocation in entries.
 */
#ifdef MONMEMINC		/* old name */
# define	INC_MONLIST	MONMEMINC
#elif !defined(INC_MONLIST)
# define	INC_MONLIST	(4 * 1024 / sizeof(mon_entry))
#endif
#ifndef INIT_MONLIST
# define	INIT_MONLIST	(16 * 1024 / sizeof(mon_entry))
#endif

/*
 * Hashing stuff
 */
#define	MON_HASH_SIZE	NTP_HASH_SIZE
#define	MON_HASH_MASK	NTP_HASH_MASK
#define	MON_HASH(addr)	NTP_HASH_ADDR(addr)

/*
 * Pointers to the hash table and the MRU list.  Memory for the hash
 * table is allocated only if monitoring is enabled.
 */
static	mon_entry *	mon_hash[MON_HASH_SIZE];
	mon_entry	mon_mru_list;	/* mru listhead */

/*
 * List of free structures structures, and counters of in-use and total
 * structures. The free structures are linked with the hash_next field.
 */
static  mon_entry *mon_free;		/* free list or null if none */
	u_int mru_alloc;		/* mru list + free list count */
	u_int mru_entries;		/* mru list count */
	u_int mru_peakentries;		/* highest mru_entries seen */
	u_int mru_initalloc = INIT_MONLIST;/* entries to preallocate */
	u_int mru_incalloc = INC_MONLIST;/* allocation batch factor */
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
	u_int	mon_enabled;		/* enable switch */
	u_int	mru_mindepth = 600;	/* preempt above this */
	int	mru_maxage = 64;	/* for entries older than */
	u_int	mru_maxdepth = 		/* MRU size hard limit */
		4 * 1024 * 1024 / sizeof(mon_entry);
	int	mon_age = 3000;		/* preemption limit */

static	void		mon_getmoremem(void);
static	void		remove_from_hash(mon_entry *);
static	inline void	mon_free_entry(mon_entry *);
static	inline void	mon_reclaim_entry(mon_entry *);


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
	INIT_DLIST(mon_mru_list, mru);
}


/*
 * remove_from_hash - removes an entry from the address hash table and
 *		      decrements mru_entries.
 */
static void
remove_from_hash(
	mon_entry *mon
	)
{
	u_int hash;
	mon_entry *punlinked;

	mru_entries--;
	hash = MON_HASH(&mon->rmtadr);
	UNLINK_SLIST(punlinked, mon_hash[hash], mon, hash_next,
		     mon_entry);
	NTP_ENSURE(punlinked == mon);
}


static inline void
mon_free_entry(
	mon_entry *m
	)
{
	memset(m, 0, sizeof(*m));
	LINK_SLIST(mon_free, m, hash_next);
}


/*
 * mon_reclaim_entry - Remove an entry from the MRU list and from the
 *		       hash array, then zero-initialize it.  Indirectly
 *		       decrements mru_entries.

 * The entry is prepared to be reused.  Before return, in
 * remove_from_hash(), mru_entries is decremented.  It is the caller's
 * responsibility to increment it again.
 */
static inline void
mon_reclaim_entry(
	mon_entry *m
	)
{
	UNLINK_DLIST(m, mru);
	remove_from_hash(m);
	memset(m, 0, sizeof(*m));
}


/*
 * mon_getmoremem - get more memory and put it on the free list
 */
static void
mon_getmoremem(void)
{
	mon_entry *chunk;
	mon_entry *mon;
	u_int entries;

	entries = (0 == mon_mem_increments)
		      ? mru_initalloc
		      : mru_incalloc;

	chunk = emalloc(entries * sizeof(*chunk));
	for (mon = chunk + entries - 1; mon >= chunk; mon--)
		mon_free_entry(mon);

	mru_alloc += entries;
	mon_mem_increments++;
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
	if (0 == mon_mem_increments)
		mon_getmoremem();

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
	mon_entry *mon;

	if (MON_OFF == mon_enabled)
		return;
	if ((mon_enabled & mode) == 0 || mode == MON_OFF)
		return;

	mon_enabled &= ~mode;
	if (mon_enabled != MON_OFF)
		return;
	
	/*
	 * Move everything on the MRU list to the free list quickly,
	 * without bothering to remove each from either the MRU list or
	 * the hash table.
	 */
	ITER_DLIST_BEGIN(mon_mru_list, mon, mru, mon_entry)
		mon_free_entry(mon);
	ITER_DLIST_END()

	/* empty the MRU list and hash table. */
	mru_entries = 0;
	INIT_DLIST(mon_mru_list, mru);
	memset(&mon_hash, 0, sizeof(mon_hash));
}

/*
 * mon_clearinterface -- remove mru entries referring to a local address
 *			 which is going away.
 */
void
mon_clearinterface(
	struct interface *lcladr
	)
{
	mon_entry *mon;

	/* iterate mon over mon_mru_list */
	ITER_DLIST_BEGIN(mon_mru_list, mon, mru, mon_entry)
		if (mon->lcladr == lcladr) {
			/* remove from mru list */
			UNLINK_DLIST(mon, mru);
			/* remove from hash list, adjust mru_entries */
			remove_from_hash(mon);
			/* put on free list */
			mon_free_entry(mon);
		}
	ITER_DLIST_END()
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
	 * The following ntp.conf "mru" knobs come into play determining
	 * the depth (or count) of the MRU list:
	 * - mru_mindepth ("mru mindepth") is a floor beneath which
	 *   entries are kept without regard to their age.  The
	 *   default is 600 which matches the longtime implementation
	 *   limit on the total number of entries.
	 * - mru_maxage ("mru maxage") is a ceiling on the age in
	 *   seconds of entries.  Entries older than this are
	 *   reclaimed once mon_mindepth is exceeded.  64s default.
	 *   Note that entries older than this can easily survive
	 *   as they are reclaimed only as needed.
	 * - mru_maxdepth ("mru maxdepth") is a hard limit on the
	 *   number of entries.
	 * - "mru maxmem" sets mru_maxdepth to the number of entries
	 *   which fit in the given number of kilobytes.  4096 default.
	 * - mru_initalloc ("mru initalloc" sets the count of the
	 *   initial allocation of MRU entries.
	 * - "mru initmem" sets mru_initalloc in units of kilobytes.
	 *   The default is 16.
	 * - mru_incalloc ("mru incalloc" sets the number of entries to
	 *   allocate on-demand each time the free list is empty.
	 * - "mru incmem" sets mru_incalloc in units of kilobytes.
	 *   The default is 4.
	 * Whichever of "mru maxmem" or "mru maxdepth" occurs last in
	 * ntp.conf controls.  Similarly for "mru initalloc" and "mru
	 * initmem", and for "mru incalloc" and "mru incmem".
	 */
	if (NULL == mon_free && mru_alloc < mru_mindepth) {
		mon_getmoremem();
		UNLINK_HEAD_SLIST(md, mon_free, hash_next);
	} else {
		tail = TAIL_DLIST(mon_mru_list, mru);
		/* note -1 is legal for mru_maxage (disables) */
		if (tail != NULL && mru_maxage < (int)(current_time -
		    tail->lasttime)) {
			mon_reclaim_entry(tail);
			md = tail;
		} else if (mon_free != NULL || mru_alloc <
			   mru_maxdepth) {
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
	mru_entries++;
	mru_peakentries = max(mru_peakentries, mru_entries);
	md->lasttime = current_time;
	md->firsttime = current_time;
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


