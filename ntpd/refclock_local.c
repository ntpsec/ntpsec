
/*
 * refclock_local - local pseudo-clock driver
 */
#include "config.h"
#include "ntp.h"
#include "ntpd.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"
#include "ntp_syscall.h"

#include <stdio.h>
#include <ctype.h>

/*
 * This is a hack to allow a machine to use its own system clock as a
 * reference clock, i.e., to free-run using no outside clock discipline
 * source. Note that the clock selection algorithm will not select this
 * driver unless all other sources of synchronization have been lost.
 * This is useful if you want to use NTP in an isolated environment
 * with no radio clock or NIST modem available. Pick a machine that you
 * figure has a good clock oscillator and configure it with this
 * driver. Set the clock using the best means available, like
 * eyeball-and-wristwatch. Then, point all the other machines at this
 * one or use broadcast (not multicast) mode to distribute time.
 *
 * Another application for this driver is if you want to use a
 * particular server's clock as the clock of last resort when all other
 * normal synchronization sources have gone away. This is especially
 * useful if that server has an ovenized oscillator. However, the
 * preferred was to do this is using orphan mode. See the documentation.
 *
 * A third application for this driver is when an external discipline
 * source is available, such as the NIST "lockclock" program, which
 * synchronizes the local clock via a telephone modem and the NIST
 * Automated Computer Time Service (ACTS), or the Digital Time
 * Synchronization Service (DTSS), which runs on DCE machines. In this
 * case the stratum should be set at zero, indicating a bona fide
 * stratum-1 source. Exercise some caution with this, since there is no
 * easy way to telegraph via NTP that something might be wrong in the
 * discipline source itself. In the case of DTSS, the local clock can
 * have a rather large jitter, depending on the interval between
 * corrections and the intrinsic frequency error of the clock
 * oscillator. In extreme cases, this can cause clients to exceed the
 * 128-ms slew window and drop off the NTP subnet.
 *
 * Fudge Factors
 *
 * None currently supported.
 */
/*
 * Local interface definitions
 */
/* #define PRECISION	(-7)	* about 10 ms precision UNUSED */
#define NAME		"LOCAL"	/* shortname */
#define DESCRIPTION	"Undisciplined local clock" /* WRU */
#define STRATUM 	5	/* default stratum */
#define DISPERSION	.01	/* default dispersion (10 ms) */

/*
 * Function prototypes
 */
static	bool	local_start (int, struct peer *);
static	void	local_poll	(int, struct peer *);

/*
 * Local variables
 */
static	unsigned long poll_time;	/* last time polled */

/*
 * Transfer vector
 */
struct	refclock refclock_local = {
	NAME,			/* basename of driver */
	local_start,		/* start up driver */
	NULL,			/* shut down driver (not used) */
	local_poll,	 	/* transmit poll message */
	NULL,			/* not used (old lcl_control) */
	NULL,			/* initialize driver (not used) */
	NULL 			/* timer - not used */
};


/*
 * local_start - start up the clock
 */
static bool
local_start(
	int unit,
	struct peer *peer
	)
{
	struct refclockproc *pp;

	UNUSED_ARG(unit);

	pp = peer->procptr;

	/*
	 * Initialize miscellaneous variables
	 */
	peer->precision = sys_vars.sys_precision;
	pp->leap = LEAP_NOTINSYNC;
	peer->stratum = STRATUM;
	pp->stratum = STRATUM;
	pp->clockname = NAME;
	pp->clockdesc = DESCRIPTION;
	memcpy(&pp->refid, "LOCL", REFIDLEN);
	peer->sstclktype = CTL_SST_TS_LOCAL;
	poll_time = current_time;
	return true;
}


/*
 * local_poll - called by the transmit procedure
 *
 * ENABLE_LOCKCLOCK: If the kernel supports the nanokernel or microkernel
 * system calls, the leap bits are extracted from the kernel. If there
 * is a kernel error or the kernel leap bits are set to 11, the NTP leap
 * bits are set to 11 and the stratum is set to infinity. Otherwise, the
 * NTP leap bits are set to the kernel leap bits and the stratum is set
 * as fudged. This behavior does not faithfully follow the
 * specification, but is probably more appropriate in a multiple-server
 * national laboratory network.
 */
static void
local_poll(
	int unit,
	struct peer *peer
	)
{
	struct refclockproc *pp;

	UNUSED_ARG(unit);

	/*
	 * Do no evil unless the house is dark or lit with our own lamp.
	 */
	if (!(sys_vars.sys_peer == NULL || sys_vars.sys_peer == peer))
		return;

	pp = peer->procptr;
	pp->polls++;

	/*
	 * Ramble through the usual filtering and grooming code, which
	 * is essentially a no-op and included mostly for pretty
	 * billboards.
	 */
	poll_time = current_time;
	refclock_process_offset(pp, pp->lastrec, pp->lastrec, 0);

	/*
	 * If another process is disciplining the system clock, we set
	 * the leap bits and quality indicators from the kernel.
	 */
#if defined(ENABLE_LOCKCLOCK)
	struct timex ntv;
	memset(&ntv,  0, sizeof ntv);
	switch (ntp_adjtime(&ntv)) {
	case TIME_OK:
		pp->leap = LEAP_NOWARNING;
		peer->stratum = pp->stratum;
		break;

	case TIME_INS:
		pp->leap = LEAP_ADDSECOND;
		peer->stratum = pp->stratum;
		break;

	case TIME_DEL:
		pp->leap = LEAP_DELSECOND;
		peer->stratum = pp->stratum;
		break;

	default:
		pp->leap = LEAP_NOTINSYNC;
		peer->stratum = STRATUM_UNSPEC;
	}
	pp->disp = 0;
	pp->jitter = 0;
#else /* ENABLE_LOCKCLOCK */
	pp->leap = LEAP_NOWARNING;
	pp->disp = DISPERSION;
	pp->jitter = 0;
#endif /* ENABLE_LOCKCLOCK */
	pp->lastref = pp->lastrec;
	refclock_receive(peer);
}

