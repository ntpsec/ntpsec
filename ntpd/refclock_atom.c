/*
 * refclock_atom - clock driver for 1-pps signals
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(REFCLOCK) && defined(CLOCK_ATOM)

#include <stdio.h>
#include <ctype.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_unixtime.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"

#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#ifdef HAVE_PPSAPI
# ifdef HAVE_TIMEPPS_H
#  include <timepps.h>
# else
#  ifdef HAVE_SYS_TIMEPPS_H
#   include <sys/timepps.h>
#  endif
# endif
#endif /* HAVE_PPSAPI */

/*
 * This driver furnishes an interface for pulse-per-second (PPS) signals
 * produced by a cesium clock, timing receiver or related equipment. It
 * can be used to remove accumulated jitter and retime a secondary
 * server when synchronized to a primary server over a congested, wide-
 * area network and before redistributing the time to local clients.
 *
 * Before this driver becomes active, the local clock must be set to
 * within +-500 ms by another means, such as a radio clock or NTP
 * itself. There are two ways to connect the PPS signal, normally at TTL
 * levels, to the computer. One is to shift to EIA levels and connect to
 * the DCD lead of a serial port. This requires a level converter and
 * may require a one-shot flipflop to lengthen the pulse. The other is
 * to connect the PS signal directly to pin 10 of a PC paralell port.
 * Both methods require a modified device driver and kernel interface
 * using the PPSAPI interface proposed to the IETF.
 *
 * Fudge Factors
 *
 * There are no special fudge factors other than the generic. The fudge
 * time1 parameter can be used to compensate for miscellaneous device
 * driver and OS delays. 
 */
/*
 * Interface definitions
 */
#ifdef HAVE_PPSAPI
extern int pps_assert;		/* selects rising or falling edge */
extern int pps_hardpps;		/* enables the kernel PPS interface */
#define DEVICE		"/dev/pps%d" /* device name and unit */
#endif /* HAVE_PPSAPI */

#define	PRECISION	(-20)	/* precision assumed (about 1 us) */
#define	REFID		"PPS\0"	/* reference ID */
#define	DESCRIPTION	"PPS Clock Discipline" /* WRU */
#define NANOSECOND	1000000000 /* one second (ns) */
#define RANGEGATE	(NANOSECOND - 500000) /* range gate (ns) */

static struct peer *pps_peer;	/* atom driver for PPS sources */

#ifdef HAVE_PPSAPI
/*
 * PPS unit control structure
 */
struct ppsunit {
	struct timespec ts;	/* last timestamp */
	int fddev;		/* pps device descriptor */
	pps_info_t pps_info;	/* pps_info control */
	pps_handle_t handle;	/* PPSAPI handlebars */
};
#endif /* HAVE_PPSAPI */

/*
 * Function prototypes
 */
static	int	atom_start	P((int, struct peer *));
static	void	atom_shutdown	P((int, struct peer *));
static	void	atom_poll	P((int, struct peer *));
#ifdef HAVE_PPSAPI
static	int	atom_pps	P((struct peer *));
#endif /* HAVE_PPSAPI */

/*
 * Transfer vector
 */
struct	refclock refclock_atom = {
	atom_start,		/* start up driver */
	atom_shutdown,		/* shut down driver */
	atom_poll,		/* transmit poll message */
	noentry,		/* not used (old atom_control) */
	noentry,		/* initialize driver */
	noentry,		/* not used (old atom_buginfo) */
	NOFLAGS			/* not used */
};


/*
 * atom_start - initialize data for processing
 */
static int
atom_start(
	int unit,		/* unit number (not used) */
	struct peer *peer	/* peer structure pointer */
	)
{
	struct refclockproc *pp;
#ifdef HAVE_PPSAPI
	register struct ppsunit *up;
	pps_params_t pps;
	int mode, temp;
	pps_handle_t handle;
#endif /* HAVE_PPSAPI */

	/*
	 * Allocate and initialize unit structure
	 */
	pps_peer = peer;
	pp = peer->procptr;
	peer->precision = PRECISION;
	pp->clockdesc = DESCRIPTION;
	memcpy((char *)&pp->refid, REFID, 4);
#ifdef HAVE_PPSAPI
	up = emalloc(sizeof(struct ppsunit));
	memset(up, 0, sizeof(struct ppsunit));
	pp->unitptr = (caddr_t)up;

	/*
	 * Open PPS device, if not done already. If some driver has
	 * already opened the PPS device, the fdpps has the file
	 * descriptor for it. If not and the pps_device string is non-
	 * null, then this must be the device name. If the pps_device is
	 * null, then open the canned device /dev/pps%d, or wherever the
	 * link points.
	 */
	if (fdpps > 0) {
		up->fddev = fdpps;
	} else {
		if (strlen(pps_device) == 0)
			sprintf(pps_device, DEVICE, unit);
		up->fddev = open(pps_device, O_RDWR, 0777);
		if (up->fddev <= 0) {
			msyslog(LOG_ERR,
			    "refclock_atom: %s: %m", pps_device);
			return (0);
		}
	}

	/*
	 * Light up the PPSAPI interface, select edge, enable kernel.
	 */
	memset(&pps, 0, sizeof(pps));
	if (time_pps_create(up->fddev, &handle) < 0) {
		msyslog(LOG_ERR,
		    "refclock_atom: time_pps_create failed: %m");
		return (0);
	}
	if (time_pps_getcap(handle, &mode) < 0) {
		msyslog(LOG_ERR,
		    "refclock_atom: time_pps_getcap failed: %m");
		return (0);
	}
	pps.mode = mode & PPS_CAPTUREBOTH;
	if (time_pps_setparams(handle, &pps) < 0) {
		msyslog(LOG_ERR,
		    "refclock_atom: time_pps_setparams failed: %m");
		return (0);
	}
	if (!pps_hardpps)
		temp = 0;
	else if (pps_assert)
		temp = mode & PPS_CAPTUREASSERT;
	else
		temp = mode & PPS_CAPTURECLEAR;
	if (time_pps_kcbind(handle, PPS_KC_HARDPPS, temp,
	    PPS_TSFMT_TSPEC) < 0) {
		msyslog(LOG_ERR,
		    "refclock_atom: time_pps_kcbind failed: %m");
		return (0);
	}
	(void)time_pps_getparams(handle, &pps);
	up->handle = handle;
#if DEBUG
	if (debug)
		printf(
		    "refclock_atom: %s handle %d ppsapi vers %d mode 0x%x cap 0x%x\n",
		    pps_device, up->handle, pps.api_version, pps.mode,
		    mode);
#endif
#endif /* HAVE_PPSAPI */
	return (1);
}


/*
 * atom_shutdown - shut down the clock
 */
static void
atom_shutdown(
	int unit,		/* unit number (not used) */
	struct peer *peer	/* peer structure pointer */
	)
{
#ifdef HAVE_PPSAPI
	struct refclockproc *pp;
	register struct ppsunit *up;

	pp = peer->procptr;
	up = (struct ppsunit *)pp->unitptr;
	if (up->fddev > 0)
		close(up->fddev);
	if (up->handle > 0)
		time_pps_destroy(up->handle);
	if (pps_peer == peer)
		pps_peer = 0;
	free(up);
#endif /* HAVE_PPSAPI */
}


#ifdef HAVE_PPSAPI
/*
 * atom_pps - receive data from the PPSAPI interface
 *
 * This routine is called once per second when the PPSAPI interface is
 * present. It snatches the PPS timestamp from the kernel and saves the
 * sign-extended fraction in a circular buffer for processing at the
 * next poll event.
 */
static int
atom_pps(
	struct peer *peer	/* peer structure pointer */
	)
{
	register struct ppsunit *up;
	struct refclockproc *pp;
	struct timespec timeout, ts;
	double doffset;
	int i, rval;

	/*
	 * Convert the timeval to l_fp and save for billboards. Sign-
	 * extend the fraction and stash in the buffer. No harm is done
	 * if previous data are overwritten. If the discipline comes bum
	 * or the data grow stale, just forget it. A range gate rejects
	 * new samples if less than a jiggle time from the next second.
	 */ 
	pp = peer->procptr;
	up = (struct ppsunit *)pp->unitptr;
	if (up->handle <= 0)
		return (1);
	timeout.tv_sec = 0;
	timeout.tv_nsec = 0;
	i = up->pps_info.assert_sequence;
	rval = time_pps_fetch(up->handle, PPS_TSFMT_TSPEC,
	    &up->pps_info, &timeout);
	if (rval < 0 || i == up->pps_info.assert_sequence)
		return (1);
	if (pps_assert)
		ts = up->pps_info.assert_timestamp;
	else
		ts = up->pps_info.clear_timestamp;
	if (ts.tv_sec == up->ts.tv_sec && ts.tv_nsec < up->ts.tv_nsec +
	    RANGEGATE)
		return (1);
	up->ts = ts;
	pp->lastrec.l_ui = ts.tv_sec + JAN_1970;
	if (ts.tv_nsec > NANOSECOND / 2)
		ts.tv_nsec -= NANOSECOND;
	doffset = -ts.tv_nsec;
	SAMPLE(doffset / NANOSECOND + pp->fudgetime1);
	return (0);
}
#endif /* HAVE_PPSAPI */

/*
 * pps_sample - receive PPS data from some other clock driver
 *
 * This routine is called once per second when the external clock driver
 * processes PPS information. It processes the PPS timestamp and saves
 * the sign-extended fraction in a circular buffer for processing at the
 * next poll event. This works only for a single PPS device.
 */
int
pps_sample(
	   l_fp *offset		/* PPS offset */
	   )
{
	register struct peer *peer;
	struct refclockproc *pp;
	l_fp lftmp;
	double doffset;

	peer = pps_peer;
	if (peer == 0)		/* nobody home */
		return 1;
	pp = peer->procptr;

	/*
	 * Convert the timeval to l_fp and save for billboards. Sign-
	 * extend the fraction and stash in the buffer. No harm is done
	 * if previous data are overwritten. If the discipline comes bum
	 * or the data grow stale, just forget it.
	 */ 
	pp->lastrec = *offset;
	L_CLR(&lftmp);
	L_ADDF(&lftmp, pp->lastrec.l_f);
	LFPTOD(&lftmp, doffset);
	SAMPLE(-doffset + pp->fudgetime1);
	return (0);
}

/*
 * atom_poll - called by the transmit procedure
 *
 * This routine is called once per second when in burst mode to save PPS
 * sample offsets in the median filter. At the end of the burst period
 * the samples are processed as a heap and the clock filter updated.
 */
static void
atom_poll(
	int unit,		/* unit number (not used) */
	struct peer *peer	/* peer structure pointer */
	)
{
	struct refclockproc *pp;
#ifdef HAVE_PPSAPI
	int err;
#endif /* HAVE_PPSAPI */

	/*
	 * Accumulate samples in the median filter. At the end of each
	 * poll interval, do a little bookeeping and process the
	 * samples.
	 */
	pp = peer->procptr;
#ifdef HAVE_PPSAPI
	err = atom_pps(peer);
	if (err != 0) {
		refclock_report(peer, CEVNT_FAULT);
		return;
	}
#endif /* HAVE_PPSAPI */
	pp->polls++;
	if (peer->burst > 0)
		return;
	if (pp->coderecv == pp->codeproc) {
		refclock_report(peer, CEVNT_TIMEOUT);
		return;
	}

	/*
	 * Valid time (leap bits zero) is returned only if the prefer
	 * peer has survived the intersection algorithm and within
	 * clock_max of local time and not too long ago.  This ensures
	 * the PPS time is within +-0.5 s of the local time and the
	 * seconds numbering is unambiguous.
	 */
	if (pps_update) {
		pp->leap = LEAP_NOWARNING;
	} else {
		pp->leap = LEAP_NOTINSYNC;
		return;
	}
	refclock_receive(peer);
	peer->burst = MAXSTAGE;
}

#else
int refclock_atom_bs;
#endif /* REFCLOCK */
