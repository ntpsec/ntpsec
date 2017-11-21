/*
 * refclock_zyfer - clock driver for the Zyfer GPSTarplus Clock
 *
 * Harlan Stenn, Jan 2002
 */

#include "config.h"
#include "ntp.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"

#include <stdio.h>
#include <ctype.h>

#include <termios.h>

/*
 * This driver provides support for the TOD serial port of a Zyfer GPStarplus.
 * This clock also provides PPS as well as IRIG outputs.
 * Precision is limited by the serial driver, etc.
 *
 * If I was really brave I'd hack/generalize the serial driver to deal
 * with arbitrary on-time characters.  This clock *begins* the stream with
 * `!`, the on-time character, and the string is *not* EOL-terminated.
 *
 * Configure the beast for 9600, 8N1.  While I see leap-second stuff
 * in the documentation, the published specs on the TOD format only show
 * the seconds going to '59'.  I see no leap warning in the TOD format.
 *
 * The clock sends the following message once per second:
 *
 *	!TIME,2002,017,07,59,32,2,4,1
 *	      YYYY DDD HH MM SS m T O
 *
 *	!		On-time character
 *	YYYY		Year
 *	DDD	001-366	Day of Year
 *	HH	00-23	Hour
 *	MM	00-59	Minute
 *	SS	00-59	Second (probably 00-60)
 *	m	1-5	Time Mode:
 *			1 = GPS time
 *			2 = UTC time
 *			3 = LGPS time (Local GPS)
 *			4 = LUTC time (Local UTC)
 *			5 = Manual time
 *	T	4-9	Time Figure Of Merit:
 *			4         x <= 1us
 *			5   1us < x <= 10 us
 *			6  10us < x <= 100us
 *			7 100us < x <= 1ms
 *			8   1ms < x <= 10ms
 *			9  10ms < x
 *	O	0-4	Operation Mode:
 *			0 Warm-up
 *			1 Time Locked
 *			2 Coasting
 *			3 Recovering
 *			4 Manual
 *
 */

/*
 * Interface definitions
 */
#define	DEVICE		"/dev/zyfer%d" /* device name and unit */
#define	SPEED232	B9600	/* uart speed (9600 baud) */
#define	PRECISION	(-20)	/* precision assumed (about 1 us) */
#define	REFID		"GPS\0"	/* reference ID */
#define	NAME		"ZYFER"	/* shortname */
#define	DESCRIPTION	"Zyfer GPStarplus" /* WRU */

#define	LENZYFER	29	/* timecode length */

/*
 * Unit control structure
 */
struct zyferunit {
	uint8_t	Rcvbuf[LENZYFER + 1];
	uint8_t	polled;		/* poll message flag */
	int	pollcnt;
	int	Rcvptr;
};

/*
 * Function prototypes
 */
static	bool	zyfer_start	(int, struct peer *);
static	void	zyfer_receive	(struct recvbuf *);
static	void	zyfer_poll	(int, struct peer *);

/*
 * Transfer vector
 */
struct	refclock refclock_zyfer = {
	NAME,			/* basename of driver */
	zyfer_start,		/* start up driver */
	NULL,			/* shut down driver in the standard way */
	zyfer_poll,		/* transmit poll message */
	NULL,			/* not used (old zyfer_control) */
	NULL,			/* initialize driver (not used) */
	NULL			/* timer - not used */
};


/*
 * zyfer_start - open the devices and initialize data for processing
 */
static bool
zyfer_start(
	int unit,
	struct peer *peer
	)
{
	struct zyferunit *up;
	struct refclockproc *pp;
	int fd;
	char device[20];

	/*
	 * Open serial port.
	 * Something like LDISC_ACTS that looked for ! would be nice...
	 */
	snprintf(device, sizeof(device), DEVICE, unit);
	fd = refclock_open(peer->cfg.path ? peer->cfg.path : device,
			   peer->cfg.baud ? peer->cfg.baud : SPEED232,
			   LDISC_RAW);
	if (fd <= 0)
		/* coverity[leaked_handle] */
		return false;

	msyslog(LOG_NOTICE, "REFCLOCK: zyfer(%d) fd: %d", unit, fd);

	/*
	 * Allocate and initialize unit structure
	 */
	up = emalloc_zero(sizeof(struct zyferunit));
	pp = peer->procptr;
	pp->io.clock_recv = zyfer_receive;
	pp->io.srcclock = peer;
	pp->io.datalen = 0;
	pp->io.fd = fd;
	if (!io_addclock(&pp->io)) {
		close(fd);
		pp->io.fd = -1;
		free(up);
		return false;
	}
	pp->unitptr = up;

	/*
	 * Initialize miscellaneous variables
	 */
	peer->precision = PRECISION;
	pp->clockname = NAME;
	pp->clockdesc = DESCRIPTION;
	memcpy((char *)&pp->refid, REFID, REFIDLEN);
	peer->sstclktype = CTL_SST_TS_UHF;
	up->pollcnt = 2;
	up->polled = 0;		/* May not be needed... */

	return true;
}


/*
 * zyfer_receive - receive data from the serial interface
 */
static void
zyfer_receive(
	struct recvbuf *rbufp
	)
{
	struct zyferunit *up;
	struct refclockproc *pp;
	struct peer *peer;
	int tmode;		/* Time mode */
	int tfom;		/* Time Figure Of Merit */
	int omode;		/* Operation mode */
	uint8_t *p;

	peer = rbufp->recv_peer;
	pp = peer->procptr;
	up = pp->unitptr;
	p = (uint8_t *) &rbufp->recv_space;
	/*
	 * If lencode is 0:
	 * - if *rbufp->recv_space is !
	 * - - call refclock_gtlin to get things going
	 * - else flush
	 * else stuff it on the end of lastcode
	 * If we don't have LENZYFER bytes
	 * - wait for more data
	 * Crack the beast, and if it's OK, process it.
	 *
	 * We use refclock_gtlin() because we might use LDISC_CLK.
	 *
	 * Under FreeBSD, we get the ! followed by two 14-byte packets.
	 */

	if (pp->lencode >= LENZYFER)
		pp->lencode = 0;

	if (!pp->lencode) {
		if (*p == '!')
			pp->lencode = refclock_gtlin(rbufp, pp->a_lastcode,
						     BMAX, &pp->lastrec);
		else
			return;
	} else {
		memcpy(pp->a_lastcode + pp->lencode, p, rbufp->recv_length);
		pp->lencode += (int)rbufp->recv_length;
		pp->a_lastcode[pp->lencode] = '\0';
	}

	if (pp->lencode < LENZYFER)
		return;

	record_clock_stats(peer, pp->a_lastcode);

	/*
	 * We get down to business, check the timecode format and decode
	 * its contents. If the timecode has invalid length or is not in
	 * proper format, we declare bad format and exit.
	 */

	if (pp->lencode != LENZYFER) {
		refclock_report(peer, CEVNT_BADTIME);
		return;
	}

	/*
	 * Timecode sample: "!TIME,2002,017,07,59,32,2,4,1"
	 */
	if (sscanf(pp->a_lastcode, "!TIME,%4d,%3d,%2d,%2d,%2d,%d,%d,%d",
		   &pp->year, &pp->day, &pp->hour, &pp->minute, &pp->second,
		   &tmode, &tfom, &omode) != 8) {
		refclock_report(peer, CEVNT_BADREPLY);
		return;
	}

	if (tmode != 2) {
		refclock_report(peer, CEVNT_BADTIME);
		return;
	}

	/* Should we make sure tfom is 4? */

	if (omode != 1) {
		pp->leap = LEAP_NOTINSYNC;
		return;
	}

	if (!refclock_process(pp)) {
		refclock_report(peer, CEVNT_BADTIME);
		return;
        }

	/*
	 * Good place for record_clock_stats()
	 */
	up->pollcnt = 2;

	if (up->polled) {
		up->polled = 0;
		refclock_receive(peer);
	}
}


/*
 * zyfer_poll - called by the transmit procedure
 */
static void
zyfer_poll(
	int unit,
	struct peer *peer
	)
{
	struct zyferunit *up;
	struct refclockproc *pp;

	UNUSED_ARG(unit);

	/*
	 * We don't really do anything here, except arm the receiving
	 * side to capture a sample and check for timeouts.
	 */
	pp = peer->procptr;
	up = pp->unitptr;
	if (!up->pollcnt)
		refclock_report(peer, CEVNT_TIMEOUT);
	else
		up->pollcnt--;
	pp->polls++;
	up->polled = 1;
}

