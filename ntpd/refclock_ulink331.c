/*
 * refclock_ulink331 - clock driver for ultralink model 331 WWVB receiver
 * 
 */

/***********************************************************************
 *                                                                     *
 * Copyright (c) David L. Mills 1992-1998                              *
 *                                                                     *
 * Permission to use, copy, modify, and distribute this software and   *
 * its documentation for any purpose and without fee is hereby         *
 * granted, provided that the above copyright notice appears in all    *
 * copies and that both the copyright notice and this permission       *
 * notice appear in supporting documentation, and that the name        *
 * University of Delaware not be used in advertising or publicity      *
 * pertaining to distribution of the software without specific,        *
 * written prior permission. The University of Delaware makes no       *
 * representations about the suitability this software for any         *
 * purpose. It is provided "as is" without express or implied          *
 * warranty.                                                           *
 **********************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(REFCLOCK) && defined(CLOCK_ULINK331)

#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>
#include <time.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_refclock.h"
#include "ntp_calendar.h"
#include "ntp_stdlib.h"

/*
 * This driver supports ultralink model 331 WWVB radio
 * this driver was based on the refclock_wwvb.c driver
 * in the ntp distribution.
 *
 * Fudge Factors
 *
 * none
 *
 * revision history:
 *		99/9/09 j.c.lang	original edit's
 *		99/9/11 j.c.lang	changed timecode parse to 
 *                                      match what the radio actually
 *                                      sends. 
 *              99/10/11 j.c.lang       added support for continous
 *                                      time code mode (dipsw2)
 *
 * Questions, bugs, ideas send to:
 *	Joseph C. Lang
 *	tcnojl1@earthlink.net
 *
 */
/*
 * on the Ultralink model 331 decoder Dip switch 2 controls
 * polled or continous timecode 
 * define DIPSW2OFF if using polled
*/
/* #define DIPSW2OFF */ 
/*
 * Interface definitions
 */
#define	DEVICE		"/dev/wwvb%d" /* device name and unit */
#define	SPEED232	B9600	/* uart speed (9600 baud) */
#define	PRECISION	(-10)	/* precision assumed (about 10 ms) */
#define	REFID		"WWVB"	/* reference ID */
#define	DESCRIPTION	"ultra-link WWVB Receiver" /* WRU */

#define	LENCODE		32	/* timecode length */

/*
 *  unit control structure
 */
struct ulink331unit {
	u_char	tcswitch;	/* timecode switch */
	l_fp	laststamp;	/* last receive timestamp */
};

/*
 * Function prototypes
 */
static	int	ulink331_start	P((int, struct peer *));
static	void	ulink331_shutdown	P((int, struct peer *));
static	void	ulink331_receive	P((struct recvbuf *));
static	void	ulink331_poll	P((int, struct peer *));

/*
 * Transfer vector
 */
struct	refclock refclock_ulink331 = {
	ulink331_start,		/* start up driver */
	ulink331_shutdown,		/* shut down driver */
	ulink331_poll,		/* transmit poll message */
	noentry,		/* not used  */
	noentry,		/* not used  */
	noentry,		/* not used  */
	NOFLAGS			/* not used  */
};


/*
 * ulink331_start - open the devices and initialize data for processing
 */
static int
ulink331_start(
	int unit,
	struct peer *peer
	)
{
	register struct ulink331unit *up;
	struct refclockproc *pp;
	int fd;
	char device[20];

	/*
	 * Open serial port. Use CLK line discipline, if available.
	 */
	(void)sprintf(device, DEVICE, unit);
	if (!(fd = refclock_open(device, SPEED232, LDISC_CLK)))
		return (0);

	/*
	 * Allocate and initialize unit structure
	 */
	if (!(up = (struct ulink331unit *)
	      emalloc(sizeof(struct ulink331unit)))) {
		(void) close(fd);
		return (0);
	}
	memset((char *)up, 0, sizeof(struct ulink331unit));
	pp = peer->procptr;
	pp->unitptr = (caddr_t)up;
	pp->io.clock_recv = ulink331_receive;
	pp->io.srcclock = (caddr_t)peer;
	pp->io.datalen = 0;
	pp->io.fd = fd;
	if (!io_addclock(&pp->io)) {
		(void) close(fd);
		free(up);
		return (0);
	}

	/*
	 * Initialize miscellaneous variables
	 */
	peer->precision = PRECISION;
	peer->flags |= FLAG_BURST;
	peer->burst = pp->nstages;
	pp->clockdesc = DESCRIPTION;
	memcpy((char *)&pp->refid, REFID, 4);
	return (1);
}


/*
 * ulink331_shutdown - shut down the clock
 */
static void
ulink331_shutdown(
	int unit,
	struct peer *peer
	)
{
	register struct ulink331unit *up;
	struct refclockproc *pp;

	pp = peer->procptr;
	up = (struct ulink331unit *)pp->unitptr;
	io_closeclock(&pp->io);
	free(up);
}


/*
 * ulink331_receive - receive data from the serial interface
 */
static void
ulink331_receive(
	struct recvbuf *rbufp
	)
{
	struct ulink331unit *up;
	struct refclockproc *pp;
	struct peer *peer;

	l_fp	trtmp;		/* arrival timestamp */
	int	qualchar;	/* quality indicator */
	int	tz;		/* time zone */
	int	temp;		/* int temp */
	char	syncchar;	/* synchronization indicator */
	char	leapchar;	/* leap indicator */
	char	dstchar;	/* daylight/standard indicator */

	/*
	 * Initialize pointers and read the timecode and timestamp
	 */
	peer = (struct peer *)rbufp->recv_srcclock;
	pp = peer->procptr;
	up = (struct ulink331unit *)pp->unitptr;
	temp = refclock_gtlin(rbufp, pp->a_lastcode, BMAX, &trtmp);

	/*
	 * Note we get a buffer and timestamp for both a <cr> and <lf>,
	 * but only the <cr> timestamp is retained. 
	 */
	if (temp == 0) {
		if (up->tcswitch == 0) {
			up->tcswitch = 1;
			up->laststamp = trtmp;
		} else
		    up->tcswitch = 0;
		return;
	}
	pp->lencode = temp;
	pp->lastrec = up->laststamp;
	up->laststamp = trtmp;
	up->tcswitch = 1;
#ifdef DEBUG
	if (debug)
		printf("ulink331: timecode %d %s\n", pp->lencode,
		    pp->a_lastcode);
#endif

	/*
	 * We get down to business, check the timecode format and decode
	 * its contents. If the timecode has invalid length or is not in
	 * proper format, we declare bad format and exit.
	 */
	syncchar = leapchar = dstchar = ' ';
	tz = 0;
	pp->msec = 0;

	if (pp->lencode != LENCODE ) {
		refclock_report(peer, CEVNT_BADREPLY);
		return;
	}

	/*
	 * Timecode format  "S9+D 00 YYYY+DDDUTCS HH:MM:SSl+5"
	 *   S sync indicator S insync N not in sync
	 *   9+ signal level 0-9 9+ if over 9 note single digit
	 *     followed by space
	 *   D data bit ???
	 *   00 hours since last sync
	 *   YYYY current year
	 *   + leap year indicator
	 *   DDD day of year
	 *   UTC timezone
	 *   S daylighe savings indicator
	 *   HH hours
	 *   MM minutes
	 *   SS seconds
	 *   L leap second flag
	 *   +5 UT1 correction
	*/
	if (sscanf(pp->a_lastcode, "%*4c %2d %4d%*c%3d%*4c %2d%c%2d:%2d%c%*2c",
	    &qualchar, &pp->year, &pp->day,
	    &pp->hour, &syncchar, &pp->minute, &pp->second, 
	    &leapchar) != 8){
		refclock_report(peer, CEVNT_BADREPLY);
		return;
	}


	/*
	 * Process the new sample in the median filter and determine the
	 * timecode timestamp.
	 */
	if (!refclock_process(pp)) {
		refclock_report(peer, CEVNT_BADTIME);
		peer->burst = 0;
		return;
	}
	if (peer->burst > 0)
		return;

	record_clock_stats(&peer->srcadr, pp->a_lastcode);

	/*
	 * Decode synchronization, and leap characters. If
	 * unsynchronized, set the leap bits accordingly and exit.
	 * Otherwise, set the leap bits according to the leap character.
	 */
	switch (qualchar) {
		case 0 :
			pp->disp=.002;
			break;
		case 1 :
			pp->disp=.02;
			break;
		case 2 :
			pp->disp=.03;
			break;
		case 3 :
			pp->disp=.04;
			break;
		default:
			pp->disp=MAXDISPERSE;
			break;
	}

	if (syncchar != ':')
		pp->leap = LEAP_NOTINSYNC;
	else if (leapchar == '+')
		pp->leap = LEAP_ADDSECOND;
	else
		pp->leap = LEAP_NOWARNING;

	refclock_receive(peer);

}


/*
 * ulink331_poll - called by the transmit procedure
 */
static void
ulink331_poll(
	int unit,
	struct peer *peer
	)
{
	register struct ulink331unit *up;
	struct refclockproc *pp;
	char pollchar;

	/*
	 * Time to poll the clock. 
	 * 'T' 
	 */
	pollchar = 'T';
	pp = peer->procptr;
	up = (struct ulink331unit *)pp->unitptr;
	if (peer->burst == 0 && peer->reach == 0)
		refclock_report(peer, CEVNT_TIMEOUT);
#ifdef DIPSW2OFF
	if (write(pp->io.fd, &pollchar, 1) != 1)
		refclock_report(peer, CEVNT_FAULT);
	else
#endif
		pp->polls++;
}

#else
int refclock_ulink331_bs;
#endif /* REFCLOCK */
