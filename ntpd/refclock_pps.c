/*
 * refclock_pps - clock driver for 1-pps signals
 */
#include "config.h"
#include <stdio.h>
#include <ctype.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"

/*
 * This driver requires the PPSAPI interface (RFC 2783)
 */
#include "ppsapi_timepps.h"
#include "refclock_pps.h"

/*
 * This driver furnishes an interface for pulse-per-second (PPS) signals
 * produced by a cesium clock, timing receiver or related equipment. It
 * can be used to remove accumulated jitter over a congested link and
 * retime a server before redistributing the time to clients. It can
 * also be used as a holdover should all other synchronization sources
 * become unreachable.
 *
 * Before this driver becomes active, the local clock must be set to
 * within +-0.4 s by another means, such as a radio clock or NTP
 * itself. There are two ways to connect the PPS signal, normally at TTL
 * levels, to the computer. One is to shift to EIA levels and connect to
 * pin 8 (DCD) of a serial port. This requires a level converter and
 * may require a one-shot flipflop to lengthen the pulse. The other is
 * to connect the PPS signal directly to pin 10 (ACK) of a PC parallel
 * port. These methods are architecture dependent.
 *
 * This driver requires the Pulse-per-Second API for Unix-like Operating
 * Systems, Version 1.0, RFC 2783 (PPSAPI). Implementations are
 * available for FreeBSD, Linux, SunOS, Solaris and Tru64. However, at
 * present only the Tru64 implementation provides the full generality of
 * the API with multiple PPS drivers and multiple handles per driver. If
 * the PPSAPI is normally implemented in the /usr/include/sys/timepps.h
 * header file and kernel support specific to each operating system.
 *
 * This driver normally uses the PLL/FLL clock discipline implemented in
 * the ntpd code. Ordinarily, this is the most accurate means, as the
 * median filter in the driver interface is much larger than in the
 * kernel. However, if the systemic clock frequency error is large (tens
 * to hundreds of PPM), it's better to used the kernel support, if
 * available.
 *
 * This driver is subject to the mitigation rules described in the
 * "mitigation rules and the prefer peer" page. However, there is an
 * important difference. If this driver becomes the PPS driver according
 * to these rules, it is active only if (a) a prefer peer other than
 * this driver is among the survivors or (b) there are no survivors and
 * the minsane option of the tos command is zero. This is intended to
 * support space missions where updates from other spacecraft are
 * infrequent, but a reliable PPS signal, such as from an Ultra Stable
 * Oscillator (USO) is available.
 *
 * Driver options
 *
 * The PPS timestamp is captured on the rising (assert) edge if flag2 is
 * dim (default) and on the falling (clear) edge if lit. If flag3 is dim
 * (default), the kernel PPS support is disabled; if lit it is enabled.
 * If flag4 is lit, each timesampt is copied to the clockstats file for
 * later analysis. This can be useful when constructing Allan deviation
 * plots. The time1 parameter can be used to compensate for
 * miscellaneous device driver and OS delays.
 */
/*
 * Interface definitions
 */
#define DEVICE		"/dev/pps%d"	/* device name and unit */
#define	PRECISION	(-30)		/* precision assumed (about 1 ns) */
#define	REFID		"PPS\0"		/* reference ID */
#define	NAME		"PPS"		/* shortname */
#define	DESCRIPTION	"PPS Clock Discipline" /* WRU */

/*
 * PPS unit control structure
 */
struct ppsunit {struct refclock_ppsctl ppsctl; /* PPS context structure pointer */
	int	fddev;		/* file descriptor */
	int	pcount;		/* PPS samples added to FIFO */
	int	scount;		/* PPS not setup */
	int	kcount;		/* PPS error from kernel */
	int	rcount;		/* PPS not ready */
};

/*
 * Function prototypes
 */
static	bool	pps_start	(int, struct peer *);
static	void	pps_shutdown	(struct refclockproc *);
static	void	pps_poll	(int, struct peer *);
static	void	pps_timer	(int, struct peer *);

/*
 * Transfer vector
 */
struct	refclock refclock_pps = {
	NAME,			/* basename of driver */
	pps_start,		/* start up driver */
	pps_shutdown,		/* shut down driver */
	pps_poll,		/* transmit poll message */
	NULL,			/* control (not used) */
	NULL,			/* initialize driver (not used) */
	pps_timer,		/* called once per second */
};


/*
 * pps_start - initialize data for processing
 */
static bool
pps_start(
	int unit,		/* unit number (not used) */
	struct peer *peer	/* peer structure pointer */
	)
{
	struct refclockproc *pp;
	struct ppsunit *up;
	char	device[80];

	/*
	 * Allocate and initialize unit structure
	 */
	pp = peer->procptr;
	peer->is_pps_driver = true;
	peer->precision = PRECISION;
	pp->clockname = NAME;
	pp->clockdesc = DESCRIPTION;
	pp->stratum = STRATUM_UNSPEC;
	memcpy((char *)&pp->refid, REFID, REFIDLEN);
	peer->sstclktype = CTL_SST_TS_ATOM;
	up = emalloc_zero(sizeof(struct ppsunit));
	pp->unitptr = up;

	/*
	 * Open PPS device. This can be any serial or parallel port and
	 * not necessarily the port used for the associated radio.
	 */
	snprintf(device, sizeof(device), DEVICE, unit);
	up->fddev = open(peer->cfg.ppspath ? peer->cfg.ppspath : device,
		O_RDWR);
	if (up->fddev <= 0) {
		msyslog(LOG_ERR, "REFCLOCK: refclock_pps: %m");
		return false;
	}

	/*
	 * Light up the PPSAPI interface.
	 */
	return (refclock_ppsapi(up->fddev, &up->ppsctl));
}


/*
 * pps_shutdown - shut down the clock
 */
static void
pps_shutdown(
	struct refclockproc *pp	/* refclock structure pointer */
	)
{
	struct ppsunit *up;

	up = pp->unitptr;
	if (up->fddev > 0)
		close(up->fddev);
	free(up);
}

/*
 * pps_timer - called once per second
 */
void
pps_timer(
	int	unit,		/* unit pointer (not used) */
	struct peer *peer	/* peer structure pointer */
	)
{
	struct	ppsunit *up;
	struct	refclockproc *pp;
	pps_status rc;

	UNUSED_ARG(unit);

	pp = peer->procptr;
	up = pp->unitptr;
	rc = refclock_catcher(peer, &up->ppsctl, pp->sloppyclockflag);
        switch (rc) {
            case PPS_OK:
                up->pcount++;
                break;
            default:
            case PPS_SETUP:
                up->scount++;
                break;
            case PPS_KERNEL:
                up->kcount++;
                break;
            case PPS_NREADY:
                up->rcount++;
                break;
        }
        if (rc != PPS_OK) return;

	peer->cfg.flags |= FLAG_PPS;

	/*
	 * If flag4 is lit, record each second offset to clockstats.
	 * That's so we can make awesome Allan deviation plots.
	 */
	if (pp->sloppyclockflag & CLK_FLAG4) {
		mprintf_clock_stats(peer, "%.9f", pp->filter[pp->coderecv]);
	}
}


/*
 * pps_poll - called by the transmit procedure
 */
static void
pps_poll(
	int unit,		/* unit number (not used) */
	struct peer *peer	/* peer structure pointer */
	)
{
	struct ppsunit *up;
	struct refclockproc *pp;

	UNUSED_ARG(unit);

	pp = peer->procptr;
	up = (struct ppsunit *)pp->unitptr;

	/*
	 * Don't wiggle the clock until some other driver has numbered
	 * the seconds.
	 */
	if (sys_vars.sys_leap == LEAP_NOTINSYNC) {
		pp->codeproc = pp->coderecv;  // xxx ??
		up->pcount = up->scount = up->kcount = up->rcount = 0;
		return;
        }

	pp->polls++;

	mprintf_clock_stats(peer,
	    "%lu %d %d %d %d",
	    up->ppsctl.sequence,
	    up->pcount, up->scount, up->kcount, up->rcount);
	up->pcount = up->scount = up->kcount = up->rcount = 0;

	if (pp->codeproc == pp->coderecv) {
		peer->cfg.flags &= ~FLAG_PPS;
		refclock_report(peer, CEVNT_TIMEOUT);
		return;
	}
	pp->lastref = pp->lastrec;
	refclock_receive(peer);
}
