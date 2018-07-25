/*
 * ntp_monitor - monitor ntpd statistics
 */

#include "config.h"

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_lists.h"
#include "ntp_stdlib.h"

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
#ifndef INC_MONLIST
# define	INC_MONLIST	(4 * 1024 / sizeof(mon_entry))
#endif
#ifndef INIT_MONLIST
# define	INIT_MONLIST	(4 * 1024 / sizeof(mon_entry))
#endif
#ifndef MRU_MAXDEPTH_DEF
# define MRU_MAXDEPTH_DEF	(1024 * 1024 / sizeof(mon_entry))
#endif

struct monitor_data mon_data = {
    .mru_mindepth = 600, /* preempt above this */
	.mru_maxage = 3600,	/* recycle if older than this */
	.mru_minage = 64,	/* recycle if full and older than this */
	.mru_maxdepth = MRU_MAXDEPTH_DEF,	/* MRU count hard limit */
	.mon_age = 3000,		/* preemption limit */
	.mru_initalloc = INIT_MONLIST, /* entries to preallocate */
	.mru_incalloc = INC_MONLIST, /* allocation batch factor */
	.mru_exists = 0,	/* slot already exists */
	.mru_new = 0,		/* allocate a new slot (2 cases) */
	.mru_recycleold = 0,	/* recycle slot: age > mru_maxage */
	.mru_recyclefull = 0,	/* recycle slot: full and age > mru_minage */
	.mru_none = 0		/* couldn't get one */
};

/*
 * List of free structures, and counters of in-use and total
 * structures. The free structures are linked with the hash_next field.
 */
static  mon_entry *mon_free;		/* free list or null if none */
static	uint64_t mru_alloc;		/* mru list + free list count */
static	uint64_t mon_mem_increments;	/* times called malloc() */

/*
 * Parameters of the RES_LIMITED restriction option. We define headway
 * as the idle time between packets. A packet is discarded if the
 * headway is less than the minimum, as well as if the average headway
 * is less than eight times the increment.
 */
int	ntp_minpkt = NTP_MINPKT;	/* minimum (log 2 s) */
uint8_t	ntp_minpoll = NTP_MINPOLL;	/* increment (log 2 s) */

static	void	mon_getmoremem(void);
static	void	remove_from_hash(mon_entry *);
static	void	mon_free_entry(mon_entry *);
static	void	mon_reclaim_entry(mon_entry *);

/*
 * init_mon - initialize monitoring global data
 */
void
init_mon(void)
{
	/*
	 * Don't do much of anything here.  We don't allocate memory
	 * until mon_start().
	 */
	mon_data.mon_enabled = MON_OFF;
	INIT_DLIST(mon_data.mon_mru_list, mru);
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
	unsigned int hash;
	mon_entry *punlinked;

	mon_data.mru_entries--;
	hash = MON_HASH(&mon->rmtadr);
	UNLINK_SLIST(punlinked, mon_data.mon_hash[hash], mon, hash_next,
		     mon_entry);
	ENSURE(punlinked == mon);
}


static void
mon_free_entry(
	mon_entry *m
	)
{
	ZERO(*m);
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
static void
mon_reclaim_entry(
	mon_entry *m
	)
{
	INSIST(NULL != m);

	UNLINK_DLIST(m, mru);
	remove_from_hash(m);
	ZERO(*m);
}


/*
 * mon_getmoremem - get more memory and put it on the free list
 */
static void
mon_getmoremem(void)
{
	mon_entry *chunk;
	unsigned int entries;

	entries = (0 == mon_mem_increments)
		      ? mon_data.mru_initalloc
		      : mon_data.mru_incalloc;

	if (entries) {
		chunk = eallocarray(entries, sizeof(*chunk));
		mru_alloc += entries;
		for (chunk += entries; entries; entries--)
			mon_free_entry(--chunk);

		mon_mem_increments++;
	}
        /* chunk not free()ed, chunk added to free list */
	/* coverity[leaked_storage] */
}


/*
 * mon_start - start up the monitoring software
 */
void
mon_start(
	int mode
	)
{
	size_t octets;
	unsigned int min_hash_slots;

	if (MON_OFF == mode)		/* MON_OFF is 0 */
		return;
	if (mon_data.mon_enabled) {
		mon_data.mon_enabled |= (unsigned int)mode;
		return;
	}
	if (0 == mon_mem_increments)
		mon_getmoremem();
	/*
	 * Select the MRU hash table size to limit the average count
	 * per bucket at capacity (mru_maxdepth) to 8, if possible
	 * given our hash is limited to 16 bits.
	 */
	min_hash_slots = (mon_data.mru_maxdepth / 8) + 1;
	mon_data.mon_hash_bits = 0;
	while (min_hash_slots >>= 1)
		mon_data.mon_hash_bits++;
	mon_data.mon_hash_bits = max(4, mon_data.mon_hash_bits);
	mon_data.mon_hash_bits = min(16, mon_data.mon_hash_bits);
	octets = sizeof(*mon_data.mon_hash) * MON_HASH_SIZE;
	mon_data.mon_hash = erealloc_zero(mon_data.mon_hash, octets, 0);

	mon_data.mon_enabled = (unsigned int)mode;
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

	if (MON_OFF == mon_data.mon_enabled)
		return;
	if ((mon_data.mon_enabled & (unsigned int)mode) == 0 || mode == MON_OFF)
		return;

	mon_data.mon_enabled &= (unsigned int)~mode;
	if (mon_data.mon_enabled != MON_OFF)
		return;

	/*
	 * Move everything on the MRU list to the free list quickly,
	 * without bothering to remove each from either the MRU list or
	 * the hash table.
	 */
	ITER_DLIST_BEGIN(mon_data.mon_mru_list, mon, mru, mon_entry)
		mon_free_entry(mon);
	ITER_DLIST_END()

	/* empty the MRU list and hash table. */
	mon_data.mru_entries = 0;
	INIT_DLIST(mon_data.mon_mru_list, mru);
	memset(mon_data.mon_hash, '\0', sizeof(*mon_data.mon_hash) * MON_HASH_SIZE);
}


/*
 * mon_clearinterface -- remove mru entries referring to a local address
 *			 which is going away.
 */
void
mon_clearinterface(
	endpt *lcladr
	)
{
	mon_entry *mon;

	/* iterate mon over mon_mru_list */
	ITER_DLIST_BEGIN(mon_data.mon_mru_list, mon, mru, mon_entry)
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

int mon_get_oldest_age(l_fp now)
{
    mon_entry *	oldest;
    if (mon_data.mru_entries == 0)
	return 0;
    oldest = TAIL_DLIST(mon_data.mon_mru_list, mru);
    now -= oldest->last;
    /* add one-half second to round up */
    now += 0x80000000;
    return lfpsint(now);
}
/*
 * ntp_monitor - record stats about this packet
 *
 * Returns supplied restriction flags, with RES_LIMITED and RES_KOD
 * cleared unless the packet should not be responded to normally
 * (RES_LIMITED) and possibly should trigger a KoD response (RES_KOD).
 * The returned flags are saved in the MRU entry, so that it reflects
 * whether the last packet from that source triggered rate limiting,
 * and if so, possible KoD response.  This implies you can not tell
 * whether a given address is eligible for rate limiting/KoD from the
 * monlist restrict bits, only whether or not the last packet triggered
 * such responses.  ntpq -c reslist lets you see whether RES_LIMITED
 * or RES_KOD is lit for a particular address before ntp_monitor()'s
 * typical dousing.
 */
unsigned short
ntp_monitor(
	struct recvbuf *rbufp,
	unsigned short	flags
	)
{
	l_fp		interval_fp;
	mon_entry *	mon;
	mon_entry *	oldest;
	int		oldest_age;
	unsigned int	hash;
	unsigned short	restrict_mask;
	uint8_t		mode;
	uint8_t		version;
	uint8_t     li_vn_mode;
	int		interval;
	int		head;		/* headway increment */
	int		leak;		/* new headway */
	int		limit;		/* average threshold */

	if (mon_data.mon_enabled == MON_OFF)
		return ~(RES_LIMITED | RES_KOD) & flags;

	hash = MON_HASH(&rbufp->recv_srcadr);
	li_vn_mode = rbufp->recv_buffer[0];
	mode = PKT_MODE(li_vn_mode);
	version = PKT_VERSION(li_vn_mode);
	mon = mon_data.mon_hash[hash];
	/*
	 * We keep track of all traffic for a given IP in one entry,
	 * otherwise cron'ed ntpdate or similar evades RES_LIMITED.
	 */

	for (; mon != NULL; mon = mon->hash_next)
		if (SOCK_EQ(&mon->rmtadr, &rbufp->recv_srcadr))
			break;

	if (mon != NULL) {
		mon_data.mru_exists++;
		interval_fp = rbufp->recv_time;
		interval_fp -= mon->last;
		/* add one-half second to round up */
		interval_fp += 0x80000000;
		interval = lfpsint(interval_fp);
		mon->last = rbufp->recv_time;
		NSRCPORT(&mon->rmtadr) = NSRCPORT(&rbufp->recv_srcadr);
		mon->count++;
		restrict_mask = flags;
		mon->vn_mode = VN_MODE(version, mode);

		/* Shuffle to the head of the MRU list. */
		UNLINK_DLIST(mon, mru);
		LINK_DLIST(mon_data.mon_mru_list, mon, mru);

		/*
		 * At this point the most recent arrival is first in the
		 * MRU list.  Decrease the counter by the headway, but
		 * not less than zero.
		 */
		mon->leak -= interval;
		mon->leak = max(0, mon->leak);
		head = 1 << ntp_minpoll;
		leak = mon->leak + head;
		limit = NTP_SHIFT * head;

		DPRINT(2, ("MRU: interval %d headway %d limit %d\n",
			   interval, leak, limit));

		/*
		 * If the minimum and average thresholds are not
		 * exceeded, douse the RES_LIMITED and RES_KOD bits and
		 * increase the counter by the headway increment.  Note
		 * that we give a 1-s grace for the minimum threshold
		 * and a 2-s grace for the headway increment.  If one or
		 * both thresholds are exceeded and the old counter is
		 * less than the average threshold, set the counter to
		 * the average threshold plus the increment and leave
		 * the RES_LIMITED and RES_KOD bits lit. Otherwise,
		 * leave the counter alone and douse the RES_KOD bit.
		 * This rate-limits the KoDs to no less than the average
		 * headway.
		 */
		if (interval + 1 >= ntp_minpkt && leak < limit) {
			mon->leak = leak - 2;
			restrict_mask &= ~(RES_LIMITED | RES_KOD);
		} else if (mon->leak < limit)
			mon->leak = limit + head;
		else
			restrict_mask &= ~RES_KOD;

		mon->flags = restrict_mask;

		return mon->flags;
	}

	/*
	 * If we got here, this is the first we've heard of this
	 * guy.  Get him some memory, either from the free list
	 * or from the tail of the MRU list.
	 *
	 * The following ntp.conf "mru" knobs come into play determining
	 * the depth (or count) of the MRU list:
	 * - mru_mindepth ("mru mindepth") is a floor beneath which
	 *   entries are kept without regard to their age.  The
	 *   default is 600 which matches the longtime implementation
	 *   limit on the total number of entries.
	 * - mru_maxage ("mru maxage") is a ceiling on the age in
	 *   seconds of entries.  Entries older than this are
	 *   reclaimed once mon_mindepth is exceeded.  3600s default.
	 *   Note that entries older than this can easily survive
	 *   as they are reclaimed only as needed.
	 * - mru_maxdepth ("mru maxdepth") is a hard limit on the
	 *   number of entries.
	 * - "mru maxmem" sets mru_maxdepth to the number of entries
	 *   which fit in the given number of kilobytes.  The default is
	 *   1024, or 1 megabyte.
	 * - mru_initalloc ("mru initalloc" sets the count of the
	 *   initial allocation of MRU entries.
	 * - "mru initmem" sets mru_initalloc in units of kilobytes.
	 *   The default is 4.
	 * - mru_incalloc ("mru incalloc" sets the number of entries to
	 *   allocate on-demand each time the free list is empty.
	 * - "mru incmem" sets mru_incalloc in units of kilobytes.
	 *   The default is 4.
	 * Whichever of "mru maxmem" or "mru maxdepth" occurs last in
	 * ntp.conf controls.  Similarly for "mru initalloc" and "mru
	 * initmem", and for "mru incalloc" and "mru incmem".
	 */
	if (mon_data.mru_entries < mon_data.mru_mindepth) {
		mon_data.mru_new++;
		if (NULL == mon_free)
			mon_getmoremem();
		UNLINK_HEAD_SLIST(mon, mon_free, hash_next);
	} else {
		oldest = TAIL_DLIST(mon_data.mon_mru_list, mru);
		oldest_age = mon_get_oldest_age(rbufp->recv_time);
		if (mon_data.mru_maxage < oldest_age) {
			mon_data.mru_recycleold++;
			mon_reclaim_entry(oldest);
			mon = oldest;
		} else if (mon_free != NULL || mru_alloc < mon_data.mru_maxdepth) {
			mon_data.mru_new++;
			if (NULL == mon_free)
				mon_getmoremem();
			UNLINK_HEAD_SLIST(mon, mon_free, hash_next);
		} else if (oldest_age < mon_data.mru_minage) {
			mon_data.mru_none++;
			return ~(RES_LIMITED | RES_KOD) & flags;
		} else {
			mon_data.mru_recyclefull++;
			/* coverity[var_deref_model] */
			mon_reclaim_entry(oldest);
			mon = oldest;
		}
	}

	/*
	 * Got one, initialize it
	 */
	REQUIRE(mon != NULL);
	mon_data.mru_entries++;
	mon_data.mru_peakentries = max(mon_data.mru_peakentries,
								   mon_data.mru_entries);
	mon->last = rbufp->recv_time;
	mon->first = mon->last;
	mon->count = 1;
	mon->flags = ~(RES_LIMITED | RES_KOD) & flags;
	mon->leak = 0;
	memcpy(&mon->rmtadr, &rbufp->recv_srcadr, sizeof(mon->rmtadr));
	mon->vn_mode = VN_MODE(version, mode);
	mon->lcladr = rbufp->dstadr;

	/*
	 * Drop him into front of the hash table. Also put him on top of
	 * the MRU list.
	 */
	LINK_SLIST(mon_data.mon_hash[hash], mon, hash_next);
	LINK_DLIST(mon_data.mon_mru_list, mon, mru);

	return mon->flags;
}


