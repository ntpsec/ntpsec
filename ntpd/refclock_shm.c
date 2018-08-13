/*
 * refclock_shm - clock driver for utc via shared memory
 * - under construction -
 * To add new modes: Extend or union the shmTime-struct. Do not
 * extend/shrink size, because otherwise existing implementations
 * will specify wrong size of shared memory-segment
 * PB 18.3.97
 */

#include "config.h"
#include "ntp_types.h"
#include "ntp.h"
#include "ntpd.h"
#undef fileno
#include "ntp_io.h"
#undef fileno
#include "ntp_refclock.h"
#undef fileno
#include "timespecops.h"
#include "ntp_calendar.h"	/* for SECSPERHR */
#undef fileno
#include "ntp_stdlib.h"
#include "ntp_assert.h"

#undef fileno
#include <ctype.h>
#undef fileno

#include <sys/ipc.h>
#include <sys/shm.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#if defined(HAVE_STDATOMIC_H) && !defined(__COVERITY__)
# include <stdatomic.h>
#endif /* HAVE_STDATOMIC_H */

/*
 * This driver supports a reference clock attached through shared memory
 */

/*
 * SHM interface definitions
 */
#define PRECISION       (-30)    /* precision assumed 1 ns */
#define REFID           "SHM"   /* reference ID */
#define	NAME		"SHM"	/* shortname */
#define DESCRIPTION     "SHM/Shared memory interface"

#define NSAMPLES        3       /* stages of median filter */

/*
 * Mode flags
 */
#define SHM_MODE_PRIVATE 0x0001

/*
 * Function prototypes
 */
static  bool    shm_start       (int unit, struct peer *peer);
static  void    shm_shutdown    (struct refclockproc *peer);
static  void    shm_poll        (int unit, struct peer *peer);
static  void    shm_timer       (int unit, struct peer *peer);
static	void	shm_clockstats  (int unit, struct peer *peer);
static	void	shm_control	(int unit, const struct refclockstat * in_st,
				 struct refclockstat * out_st, struct peer *peer);

/*
 * Transfer vector
 */
struct  refclock refclock_shm = {
	NAME,			/* basename of driver */
	shm_start,              /* start up driver */
	shm_shutdown,           /* shut down driver */
	shm_poll,		/* transmit poll message */
	shm_control,		/* control settings */
	NULL,			/* not used: init */
	shm_timer,              /* once per second */
};

struct shmTime {
	int    mode; /* 0 - if valid is set:
		      *       use values,
		      *       clear valid
		      * 1 - if valid is set:
		      *       if count before and after read of values is equal,
		      *         use values
		      *       clear valid
		      */
	volatile int    count;
	time_t		clockTimeStampSec;
	int		clockTimeStampUSec;
	time_t		receiveTimeStampSec;
	int		receiveTimeStampUSec;
	int		leap;
	int		precision;
	int		nsamples;
	volatile int    valid;
	unsigned	clockTimeStampNSec;	/* Unsigned ns timestamps */
	unsigned	receiveTimeStampNSec;	/* Unsigned ns timestamps */
	int		dummy[8];
};

struct shmunit {
	struct shmTime *shm;	/* pointer to shared memory segment */
	int forall;		/* access for all UIDs?	*/

	/* debugging/monitoring counters - reset when printed */
	int ticks;		/* number of attempts to read data*/
	int good;		/* number of valid samples */
	int notready;		/* number of peeks without data ready */
	int bad;		/* number of invalid samples */
	int clash;		/* number of access clashes while reading */

	time_t max_delta;	/* difference limit */
	time_t max_delay;	/* age/stale limit */
};


static struct shmTime*
getShmTime(
	int unit,
	bool forall
	)
{
	struct shmTime *p = NULL;

	int shmid;

	/* 0x4e545030 is NTP0.
	 * Big units will give non-ascii but that's OK
	 * as long as everybody does it the same way.
	 */
	shmid=shmget(0x4e545030 + unit, sizeof (struct shmTime),
		      IPC_CREAT | (forall ? 0666 : 0600));
	if (shmid == -1) { /* error */
		msyslog(LOG_ERR, "REFCLOCK: SHM shmget (unit %d): %m", unit);
		return NULL;
	}
	p = (struct shmTime *)shmat (shmid, 0, 0);
	if (p == (struct shmTime *)-1) { /* error */
		msyslog(LOG_ERR, "REFCLOCK: SHM shmat (unit %d): %m", unit);
		return NULL;
	}

	return p;
}


/*
 * shm_start - attach to shared memory
 */
static bool
shm_start(
	int unit,
	struct peer *peer
	)
{
	struct refclockproc * const pp = peer->procptr;
	struct shmunit *      const up = emalloc_zero(sizeof(*up));

	pp->io.clock_recv = NULL;
	pp->io.srcclock = peer;
	pp->io.datalen = 0;
	pp->io.fd = -1;

	up->forall = (unit >= 2) && !(peer->cfg.mode & SHM_MODE_PRIVATE);

	up->shm = getShmTime(unit, up->forall);

	/*
	 * Initialize miscellaneous peer variables
	 */
	memcpy((char *)&pp->refid, REFID, REFIDLEN);
	peer->sstclktype = CTL_SST_TS_UHF;
	if (up->shm != 0) {
		pp->unitptr = up;
		up->shm->precision = PRECISION;
		peer->precision = (int8_t)up->shm->precision;
		up->shm->valid = 0;
		up->shm->nsamples = NSAMPLES;
		pp->clockname = NAME;
		pp->clockdesc = DESCRIPTION;
		/* items to be changed later in 'shm_control()': */
		up->max_delay = 5;
		up->max_delta = 4 * SECSPERHR;
		return true;
	} else {
		free(up);
		pp->unitptr = NULL;
		return false;
	}
}


/*
 * shm_control - configure flag1/time2 params
 *
 * These are not yet available during 'shm_start', so we have to do any
 * pre-computations we want to avoid during regular poll/timer callbacks
 * in this callback.
 */
static void
shm_control(
	int                         unit,
	const struct refclockstat * in_st,
	struct refclockstat       * out_st,
	struct peer               * peer
	)
{
	struct refclockproc * const pp = peer->procptr;
	struct shmunit *      const up = pp->unitptr;

	UNUSED_ARG(unit);
	UNUSED_ARG(in_st);
	UNUSED_ARG(out_st);
	if (NULL == up)
		return;
	if (!(pp->sloppyclockflag & CLK_FLAG1))
		up->max_delta = 0;
	else if (pp->fudgetime2 < 1. || pp->fudgetime2 > SECSPERDAY)
		up->max_delta = 4 * SECSPERHR;
	else
		up->max_delta = (time_t)floor(pp->fudgetime2 + 0.5);
}


/*
 * shm_shutdown - shut down the clock
 */
static void
shm_shutdown(
	struct refclockproc * pp
	)
{
	struct shmunit *      const up = pp->unitptr;

	if (NULL == up)
		return;

	(void)shmdt((char *)up->shm);

	free(up);
}


/*
 * shm_poll - called by the transmit procedure
 */
static void
shm_poll(
	int unit,
	struct peer *peer
	)
{
	struct refclockproc * const pp = peer->procptr;
	struct shmunit *      const up = pp->unitptr;
	int major_error;

	pp->polls++;

	/* get dominant reason if we have no samples at all */
	major_error = max(up->notready, up->bad);
	major_error = max(major_error, up->clash);

        /*
         * Process median filter samples. If none received, see what
         * happened, tell the core and keep going.
         */
        if (pp->coderecv != pp->codeproc) {
		/* have some samples, everything OK */
		pp->lastref = pp->lastrec;
		refclock_receive(peer);
	} else if (NULL == up->shm) { /* is this possible at all? */
		/* we're out of business without SHM access */
		refclock_report(peer, CEVNT_FAULT);
	} else if (major_error == up->clash) {
		/* too many collisions is like a bad signal */
                refclock_report(peer, CEVNT_PROP);
	} else if (major_error == up->bad) {
		/* too much stale/bad/garbled data */
                refclock_report(peer, CEVNT_BADREPLY);
	} else {
		/* in any other case assume it's just a timeout */
                refclock_report(peer, CEVNT_TIMEOUT);
        }
	/* shm_clockstats() clears the tallies, so it must be last... */
	shm_clockstats(unit, peer);
}


enum segstat_t {
    OK, NO_SEGMENT, NOT_READY, BAD_MODE, CLASH
};

struct shm_stat_t {
    int status;
    int mode;
    struct timespec tvc, tvr, tvt;
    int precision;
    int leap;
};

static inline void memory_barrier(void)
{
#if defined(HAVE_STDATOMIC_H) && !defined(__COVERITY__)
    atomic_thread_fence(memory_order_seq_cst);
#endif /* HAVE_STDATOMIC_H */
}

static enum segstat_t shm_query(volatile struct shmTime *shm_in, struct shm_stat_t *shm_stat)
/* try to grab a sample from the specified SHM segment */
{
    volatile struct shmTime shmcopy, *shm = shm_in;
    volatile int cnt;

    unsigned int cns_new, rns_new;

    /*
     * This is the main routine. It snatches the time from the shm
     * board and tacks on a local timestamp.
     */
    if (shm == NULL) {
	shm_stat->status = NO_SEGMENT;
	return NO_SEGMENT;
    }

    /*@-type@*//* splint is confused about struct timespec */
    shm_stat->tvc.tv_sec = shm_stat->tvc.tv_nsec = 0;
    {
	time_t now;

	time(&now);
	shm_stat->tvc.tv_sec = now;
    }

    /* relying on word access to be atomic here */
    if (shm->valid == 0) {
	shm_stat->status = NOT_READY;
	return NOT_READY;
    }

    cnt = shm->count;

    /*
     * This is proof against concurrency issues if either
     * (a) the memory_barrier() call works on this host, or
     * (b) memset compiles to an uninterruptible single-instruction bitblt.
     */
    memory_barrier();
    /* structure copy, to preserve volatile */
    shmcopy = *shm;
    shm->valid = 0;
    memory_barrier();

    /*
     * Clash detection in case neither (a) nor (b) was true.
     * Not supported in mode 0, and word access to the count field
     * must be atomic for this to work.
     */
    if (shmcopy.mode > 0 && cnt != shm->count) {
	shm_stat->status = CLASH;
	return (enum segstat_t)shm_stat->status;
    }

    shm_stat->status = OK;
    shm_stat->mode = shmcopy.mode;

    switch (shmcopy.mode) {
    case 0:
	shm_stat->tvr.tv_sec	= shmcopy.receiveTimeStampSec;
	shm_stat->tvr.tv_nsec	= shmcopy.receiveTimeStampUSec * 1000;
	rns_new		= shmcopy.receiveTimeStampNSec;
	shm_stat->tvt.tv_sec	= shmcopy.clockTimeStampSec;
	shm_stat->tvt.tv_nsec	= shmcopy.clockTimeStampUSec * 1000;
	cns_new		= shmcopy.clockTimeStampNSec;

	/* Since the following comparisons are between unsigned
	** variables they are always well defined, and any
	** (signed) underflow will turn into very large unsigned
	** values, well above the 1000 cutoff.
	**
	** Note: The usecs *must* be a *truncated*
	** representation of the nsecs. This code will fail for
	** *rounded* usecs, and the logic to deal with
	** wrap-arounds in the presence of rounded values is
	** much more convoluted.
	*/
	if (   ((cns_new - (unsigned)shm_stat->tvt.tv_nsec) < 1000)
	       && ((rns_new - (unsigned)shm_stat->tvr.tv_nsec) < 1000)) {
	    shm_stat->tvt.tv_nsec = (long)cns_new;
	    shm_stat->tvr.tv_nsec = (long)rns_new;
	}
	/* At this point shm_stat->tvr and shm_stat->tvt contain valid ns-level
	** timestamps, possibly generated by extending the old
	** us-level timestamps
	*/
	break;

    case 1:

	shm_stat->tvr.tv_sec	= shmcopy.receiveTimeStampSec;
	shm_stat->tvr.tv_nsec	= shmcopy.receiveTimeStampUSec * 1000;
	rns_new		= shmcopy.receiveTimeStampNSec;
	shm_stat->tvt.tv_sec	= shmcopy.clockTimeStampSec;
	shm_stat->tvt.tv_nsec	= shmcopy.clockTimeStampUSec * 1000;
	cns_new		= shmcopy.clockTimeStampNSec;

	/* See the case above for an explanation of the
	** following test.
	*/
	if (   ((cns_new - (unsigned)shm_stat->tvt.tv_nsec) < 1000)
	       && ((rns_new - (unsigned)shm_stat->tvr.tv_nsec) < 1000)) {
	    shm_stat->tvt.tv_nsec = (long)cns_new;
	    shm_stat->tvr.tv_nsec = (long)rns_new;
	}
	/* At this point shm_stat->tvr and shm_stat->tvt contains valid ns-level
	** timestamps, possibly generated by extending the old
	** us-level timestamps
	*/
	break;

    default:
	shm_stat->status = BAD_MODE;
	break;
    }
    /*@-type@*/

    /*
     * leap field is not a leap offset but a leap notification code.
     * The values are magic numbers used by NTP and set by GPSD, if at all, in
     * the subframe code.
     */
    shm_stat->leap = shmcopy.leap;
    shm_stat->precision = shmcopy.precision;

    return (enum segstat_t)shm_stat->status;
}

/*
 * shm_timer - called once every second.
 *
 * This tries to grab a sample from the SHM segment, filtering bad ones
 */
static void
shm_timer(
	int unit,
	struct peer *peer
	)
{
	struct refclockproc * const pp = peer->procptr;
	struct shmunit *      const up = pp->unitptr;

	volatile struct shmTime *shm;

	l_fp tsrcv;
	l_fp tsref;
	int c;
	time_t tt;

	enum segstat_t status;
	struct shm_stat_t shm_stat;

	up->ticks++;
	if ((shm = up->shm) == NULL) {
		/* try to map again - this may succeed if meanwhile some-
		body has ipcrm'ed the old (unaccessible) shared mem segment */
		shm = up->shm = getShmTime(unit, up->forall);
		if (shm == NULL) {
			DPRINT(1, ("%s: no SHM segment\n",refclock_name(peer)));
			return;
		}
	}

	/* query the segment, atomically */
	status = shm_query(shm, &shm_stat);

	switch (status) {
	case OK:
	    DPRINT(2, ("%s: SHM(%d) type %d sample\n",
		       refclock_name(peer), unit, shm_stat.mode));
	    break;
	case NO_SEGMENT:
	    /* should never happen, but is harmless */
	    return;
	case NOT_READY:
	    DPRINT(1, ("%s: SHM(%d) not ready\n",refclock_name(peer), unit));
	    up->notready++;
	    return;
	case BAD_MODE:
	    DPRINT(1, ("%s: SHM(%d) type blooper, mode=%d\n",
		       refclock_name(peer), unit, shm->mode));
	    up->bad++;
	    msyslog (LOG_ERR, "SHM(%d): bad mode found in shared memory: %d",
		     unit, shm->mode);
	    return;
	case CLASH:
	    DPRINT(1, ("%s: type 1 access clash\n",
		       refclock_name(peer)));
	    msyslog (LOG_NOTICE, "SHM(%d): access clash in shared memory",
                        unit);
	    up->clash++;
	    return;
	default:
	    DPRINT(1, ("%s: internal error, unknown SHM(%d) fetch status\n",
		       refclock_name(peer), unit));
	    msyslog (LOG_NOTICE, "internal error, unknown SHM(%d) fetch status",
                        unit);
	    up->bad++;
	    return;
	}


	/*
	 * Add POSIX UTC seconds and fractional seconds as a timecode.
	 * We used to unpack this to calendar time, but it is bad
	 * practice for the driver to pretend to know calendar time;
	 * that interpretation is best left to higher levels.
	 */
	/* a_lastcode is seen as timecode with: ntpq -c cv [associd] */
	c = snprintf(pp->a_lastcode, sizeof(pp->a_lastcode), "%ld.%09ld",
		     (long)shm_stat.tvt.tv_sec, (long)shm_stat.tvt.tv_nsec);
	pp->lencode = (c < (int)sizeof(pp->a_lastcode)) ? c : 0;

	/* check 1: age control of local time stamp */
	tt = shm_stat.tvc.tv_sec - shm_stat.tvr.tv_sec;
	if (tt < 0 || tt > up->max_delay) {
		DPRINT(1, ("%s:SHM(%d) stale/bad receive time, delay=%llds\n",
			   refclock_name(peer), unit, (long long)tt));
		up->bad++;
		msyslog (LOG_ERR,
                         "SHM(%d): stale/bad receive time, delay=%llds",
			 unit, (long long)tt);
		return;
	}

	/* check 2: delta check */
	tt = shm_stat.tvr.tv_sec - shm_stat.tvt.tv_sec - (shm_stat.tvr.tv_nsec < shm_stat.tvt.tv_nsec);
	if (tt < 0)
		tt = -tt;
	if (up->max_delta > 0 && tt > up->max_delta) {
		DPRINT(1, ("%s: SHM(%d) diff limit exceeded, delta=%llds\n",
			   refclock_name(peer), unit, (long long)tt));
		up->bad++;
		msyslog (LOG_ERR,
                         "SHM(%d): difference limit exceeded, delta=%llds\n",
			 unit, (long long)tt);
		return;
	}

	/* if we really made it to this point... we're winners! */
	DPRINT(2, ("%s: SHM(%d) feeding data\n", refclock_name(peer), unit));
	tsrcv = tspec_stamp_to_lfp(shm_stat.tvr);
	tsref = tspec_stamp_to_lfp(shm_stat.tvt);
	pp->leap = (uint8_t)shm_stat.leap;
	peer->precision = (int8_t)shm_stat.precision;
	refclock_process_offset(pp, tsref, tsrcv, pp->fudgetime1);
	up->good++;
}

/*
 * shm_clockstats - dump and reset counters
 */
static void shm_clockstats(
	int unit,
	struct peer *peer
	)
{
	struct refclockproc * const pp = peer->procptr;
	struct shmunit *      const up = pp->unitptr;

	UNUSED_ARG(unit);
	if (pp->sloppyclockflag & CLK_FLAG4) {
		mprintf_clock_stats(
			peer, "%3d %3d %3d %3d %3d",
			up->ticks, up->good, up->notready,
			up->bad, up->clash);
	}
	up->ticks = up->good = up->notready = up->bad = up->clash = 0;
}

