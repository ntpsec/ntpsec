/*
 * refclock_acts - clock driver for the NIST/USNO/PTB/NPL Computer Time
 *	Services
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(REFCLOCK) && (defined(CLOCK_ACTS) || defined(CLOCK_PTBACTS))

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_unixtime.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"
#include "ntp_control.h"

#include <stdio.h>
#include <ctype.h>
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif /* HAVE_SYS_IOCTL_H */

/*
 * This driver supports the US (NIST, USNO) and European (PTB, NPL,
 * etc.) modem time services, as well as Spectracom GPS and WWVB
 * receivers connected via a modem. The driver periodically dials a
 * number from a telephone list, receives the timecode data and
 * calculates the local clock correction. It is designed primarily for
 * use as backup when neither a radio clock nor connectivity to Internet
 * time servers is available.
 *
 * This driver requires a modem with a Hayes-compatible command set and
 * control over the modem data terminal ready (DTR) control line. The
 * modem setup string is hard-coded in the driver and may require
 * changes for nonstandard modems or special circumstances.
 *
 * When enabled, the calling program dials the first number in the
 * phones file. If that call fails, it dials the second number and
 * so on. The phone number is specified by the Hayes ATDT prefix
 * followed by the number itself, including the long-distance prefix
 * and delay code, if necessary. The calling program is enabled
 * when (a) fudge flag1 is set by ntpdc, (b) at each poll interval
 * when no other synchronization sources are present, and (c) at each
 * poll interval whether or not other synchronization sources are 
 * present. The calling program disconnects if (a) the called party
 * is busy or does not answer, (b) the called party disconnects
 * before a sufficient nuimber of timecodes have been received. 
 *
 * The driver is transparent to each of the modem time services and
 * Spectracom radios. It selects the parsing algorithm depending on the
 * message length. There is some hazard should the message be corrupted.
 * However, the data format is checked carefully and only if all checks
 * succeed is the message accepted. Corrupted lines are discarded
 * without complaint.
 *
 * Fudge controls
 *
 * flag1	force a call in manual mode
 * flag2	enable port locking (not verified)
 * flag3	not used
 * flag4	not used
 *
 * time1	offset adjustment (s)
 *
 * Ordinarily, the serial port is connected to a modem and the phones
 * list is defined. If no phones list is defined, the port can be 
 * connected directly to a device or another computer. In this case the
 * driver will send a single character 'T' at each poll event. If
 * fudge flag2 is enabled, port locking allows the modem to be shared
 * when not in use by this driver.
 */
/*
 * National Institute of Science and Technology (NIST)
 *
 * Phone: (303) 494-4774 (Boulder, CO); (808) 335-4721 (Hawaii)
 *
 * Data Format
 *
 * National Institute of Standards and Technology
 * Telephone Time Service, Generator 3B
 * Enter question mark "?" for HELP
 *                         D  L D
 *  MJD  YR MO DA H  M  S  ST S UT1 msADV        <OTM>
 * 47999 90-04-18 21:39:15 50 0 +.1 045.0 UTC(NIST) *<CR><LF>
 * ...
 *
 * MJD, DST, DUT1 and UTC are not used by this driver. The "*" or "#" is
 * the on-time markers echoed by the driver and used by NIST to measure
 * and correct for the propagation delay. Note: the ACTS timecode has
 * recently been changed to elminate the * on-time indicator. The reason
 * for this and the long term implications are not clear.
 *
 * US Naval Observatory (USNO)
 *
 * Phone: (202) 762-1594 (Washington, DC); (719) 567-6742 (Boulder, CO)
 *
 * Data Format (two lines, repeating at one-second intervals)
 *
 * jjjjj nnn hhmmss UTC<CR><LF>
 * *<CR><LF>
 *
 * jjjjj	modified Julian day number (not used)
 * nnn		day of year
 * hhmmss	second of day
 * *		on-time marker for previous timecode
 * ...
 *
 * USNO does not correct for the propagation delay. A fudge time1 of
 * about .06 s is advisable.
 *
 * European Services (PTB, NPL, etc.)
 *
 * PTB: +49 531 512038 (Germany)
 * NPL: 0906 851 6333 (UK only)
 *
 * Data format (see the documentation for phone numbers and formats.)
 *
 * 1995-01-23 20:58:51 MEZ  10402303260219950123195849740+40000500<CR><LF>
 *
 * Spectracom GPS and WWVB Receivers
 *
 * If a modem is connected to a Spectracom receiver, this driver will
 * call it up and retrieve the time in one of two formats. As this
 * driver does not send anything, the radio will have to either be
 * configured in continuous mode or be polled by another local driver.
 */
/*
 * Interface definitions
 */
#define	DEVICE		"/dev/acts%d" /* device name and unit */
#define	SPEED232	B9600	/* uart speed (9600 baud) */
#define	PRECISION	(-10)	/* precision assumed (about 1 ms) */
#define LOCKFILE	"/var/spool/lock/LCK..cua%d"
#define DESCRIPTION	"Automated Computer Time Service" /* WRU */
#define REFID		"NONE"	/* default reference ID */
#define MSGCNT		20	/* max message count */
#define	MAXPHONE	10	/* max number of phone numbers */

/*
 * Calling program modes (mode)
 */
#define MODE_BACKUP	0	/* backup mode */
#define MODE_AUTO	1	/* automatic mode */
#define MODE_MANUAL	2	/* manual mode */

/*
 * Service identifiers (message length)
 */
#define REFACTS		"NIST"	/* NIST reference ID */
#define LENACTSA	50	/* NIST format A */
#define LENACTSB	48	/* NIST format B */
#define REFUSNO		"USNO"	/* USNO reference ID */
#define LENUSNO		20	/* USNO */
#define REFPTB		"PTB\0"	/* PTB/NPL reference ID */
#define LENPTB		78	/* PTB/NPL format */
#define REFWWVB		"WWVB"	/* WWVB reference ID */
#define	LENWWVB0	22	/* WWVB format 0 */
#define	LENWWVB2	24	/* WWVB format 2 */
#define LF		0x0a	/* ASCII LF */

/*
 * Modem setup strings. These may have to be changed for some modems.
 *
 * AT	command prefix
 * B1	US answer tone
 * &C0	disable carrier detect
 * &D2	hang up and return to command mode on DTR transition
 * E0	modem command echo disabled
 * L1	set modem speaker volume to low level
 * M1	speaker enabled until carrier detect
 * Q0	return result codes
 * V1	return result codes as English words
 * Y1	enable long-space disconnect
 */
#define MODEM_SETUP	"ATB1&C0&D2E0L1M1Q0V1Y1" /* modem setup */

/*
 * Timeouts (all in seconds)
 */
#define SETUP		3	/* setup timeout */
#define	REDIAL		30	/* redial timeout */
#define ANSWER		60	/* answer timeout */
#define TIMECODE	60	/* message timeout */
#define	MAXCODE		10	/* max timecodes */

/*
 * State machine codes
 */
#define S_IDLE		0	/* wait for poll */
#define	S_SETUP		1	/* send modem setup */
#define S_CONNECT	2	/* wait for answer */
#define S_MSG		3	/* wait for timecode */

/*
 * Unit control structure
 */
struct actsunit {
	int	unit;		/* unit number */
	int	state;		/* the first one was Delaware */
	int	timer;		/* timeout counter */
	int	retry;		/* retry index */
	int	msgcnt;		/* count of messages received */
	l_fp	tstamp;		/* on-time timestamp */
	char	*bufptr;	/* buffer pointer */
};

/*
 * Function prototypes
 */
static	int	acts_start	(int, struct peer *);
static	void	acts_shutdown	(int, struct peer *);
static	void	acts_receive	(struct recvbuf *);
static	void	acts_message	(struct peer *);
static	void	acts_timecode	(struct peer *, char *);
static	void	acts_poll	(int, struct peer *);
static	void	acts_timeout	(struct peer *, int);
static	void	acts_timer	(int, struct peer *);
static	void	acts_close	(struct peer *);

/*
 * Transfer vector (conditional structure name)
 */
struct	refclock refclock_acts = {
	acts_start,		/* start up driver */
	acts_shutdown,		/* shut down driver */
	acts_poll,		/* transmit poll message */
	noentry,		/* not used */
	noentry,		/* not used */
	noentry,		/* not used */
	acts_timer		/* housekeeping timer */
};

/*
 * Initialize data for processing
 */
static int
acts_start (
	int	unit,
	struct peer *peer
	)
{
	struct actsunit *up;
	struct refclockproc *pp;

	/*
	 * Allocate and initialize unit structure
	 */
	up = emalloc(sizeof(struct actsunit));
	memset(up, 0, sizeof(struct actsunit));
	up->unit = unit;
	pp = peer->procptr;
	pp->unitptr = (caddr_t)up;
	pp->io.clock_recv = acts_receive;
	pp->io.srcclock = (caddr_t)peer;
	pp->io.datalen = 0;

	/*
	 * Initialize miscellaneous variables
	 */
	peer->precision = PRECISION;
	pp->clockdesc = DESCRIPTION;
	memcpy((char *)&pp->refid, REFID, 4);
	peer->sstclktype = CTL_SST_TS_TELEPHONE;
	up->bufptr = pp->a_lastcode;
	return (1);
}


/*
 * acts_shutdown - shut down the clock
 */
static void
acts_shutdown (
	int	unit,
	struct peer *peer
	)
{
	struct actsunit *up;
	struct refclockproc *pp;

	/*
	 * Warning: do this only when a call is not in progress.
	 */
	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;
	free(up);
}


/*
 * acts_receive - receive data from the serial interface
 */
static void
acts_receive (
	struct recvbuf *rbufp
	)
{
	struct actsunit *up;
	struct refclockproc *pp;
	struct peer *peer;
	char	tbuf[BMAX];
	char	*tptr;

	/*
	 * Initialize pointers and read the timecode and timestamp. Note
	 * we are in raw mode and victim of whatever the terminal
	 * interface kicks up; so, we have to reassemble messages from
	 * arbitrary fragments. Capture the timecode at the beginning of
	 * the message and at the '*' and '#' on-time characters.
	 */
	peer = (struct peer *)rbufp->recv_srcclock;
	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;
	pp->lencode = refclock_gtraw(rbufp, tbuf, BMAX - (up->bufptr -
	    pp->a_lastcode), &pp->lastrec);
	for (tptr = tbuf; *tptr != '\0'; tptr++) {
		if (*tptr == LF) {
			if (up->bufptr == pp->a_lastcode) {
				up->tstamp = pp->lastrec;
				continue;

			} else {
				*up->bufptr = '\0';
				acts_message(peer);
				up->bufptr = pp->a_lastcode;
			}
		} else if (!iscntrl(*tptr)) {
			*up->bufptr++ = *tptr;
			if (*tptr == '*' || *tptr == '#') {
				up->tstamp = pp->lastrec;
				if (write(pp->io.fd, tptr, 1) < 0)
					msyslog(LOG_ERR, "acts: write echo fails %m");
			}
		}
	}
}


/*
 * acts_message - process message
 */
void
acts_message(
	struct peer *peer
	)
{
	struct actsunit *up;
	struct refclockproc *pp;
	char	tbuf[BMAX];
	int		dtr = TIOCM_DTR;

	/*
	 * What to do depends on the state and the first token in the
	 * message.
	 */
	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;

#ifdef DEBUG
	if (debug)
		printf("acts: %d %s\n", strlen(pp->a_lastcode),
		    pp->a_lastcode);
#endif

	/*
	 * Extract the first token in the line.
	 */
	strncpy(tbuf, pp->a_lastcode, BMAX);
	strtok(tbuf, " ");
	switch(up->state) {

	/*
	 * We are waiting for the OK response to the modem setup
	 * command. When this happens, dial the number followed.
	 * If anything other than OK is received, just ignore it
	 * and wait for timeoue.
	 */
	case S_SETUP:
		if (strcmp(tbuf, "OK") != 0)
			break;

		snprintf(tbuf, sizeof(tbuf), "DIAL #%d %s", up->retry,
		    sys_phone[up->retry]);
		report_event(PEVNT_CLOCK, peer, tbuf);
		ioctl(pp->io.fd, TIOCMBIS, &dtr);
		if (write(pp->io.fd, sys_phone[up->retry],
		    strlen(sys_phone[up->retry])) < 0)
			msyslog(LOG_ERR, "acts: write DIAL fails %m");
		write(pp->io.fd, "\r", 1);
		up->retry++;
		up->state = S_CONNECT;
		up->timer = ANSWER;
		return;

	/*
	 * We are waiting for the CONNECT response to the dial
	 * command. When this happens, listen for timecodes. If
	 * somthing other than CONNECT is received, like BUSY
	 * or NO CARRIER, abort the call.
	 */
	case S_CONNECT:
		if (strcmp(tbuf, "CONNECT") != 0)
			break;

		report_event(PEVNT_CLOCK, peer, pp->a_lastcode);
		up->state = S_MSG;
		up->timer = TIMECODE;
		return;

	/*
	 * We are waiting for a timecode response. Pass it to
	 * the parser. If NO CARRIER is received, save the
	 * messages and abort the call.
	 */
	case S_MSG:
		if (strcmp(tbuf, "NO") == 0)
			report_event(PEVNT_CLOCK, peer, pp->a_lastcode);
		if (up->msgcnt < MAXCODE)
			acts_timecode(peer, pp->a_lastcode);
		else
			acts_timeout(peer, S_MSG);
		return;
	}

	/*
	 * Other response. Tell us about it.
	 */
	report_event(PEVNT_CLOCK, peer, pp->a_lastcode);
	acts_close(peer);
}


/*
 * acts_timeout - called on timeout
 */
static void
acts_timeout(
	struct peer *peer,
	int	dstate
	)
{
	struct actsunit *up;
	struct refclockproc *pp;
	int	fd;
	char	device[20];
	char	lockfile[128], pidbuf[8];
	char	tbuf[BMAX];

	/*
	 * The state machine is driven by messages from the modem,
	 * when first stated and at timeout.
	 */
	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;
	switch(dstate) {

	/*
	 * System poll event. Lock the modem port, open the device
	 * and send the setup command.
	 */
	case S_IDLE:

		/*
		 * Lock the modem port. If busy, retry later. Note: if
		 * something fails between here and the close, the lock
		 * file may not be removed.
		 */
		if (pp->sloppyclockflag & CLK_FLAG2) {
			snprintf(lockfile, sizeof(lockfile), LOCKFILE,
			    up->unit);
			fd = open(lockfile, O_WRONLY | O_CREAT | O_EXCL,
			    0644);
			if (fd < 0) {
				report_event(PEVNT_CLOCK, peer, "acts: port busy");
				return;
			}
			snprintf(pidbuf, sizeof(pidbuf), "%d\n",
			    (u_int)getpid());
			if (write(fd, pidbuf, strlen(pidbuf)) < 0)
				msyslog(LOG_ERR, "acts: write lock fails %m");
			close(fd);
		}

		/*
		 * Open the device in raw mode and link the I/O.
		 */
		snprintf(device, sizeof(device), DEVICE,
		    up->unit);
		fd = refclock_open(device, SPEED232,LDISC_ACTS |
		    LDISC_RAW | LDISC_REMOTE);
		if (fd < 0) {
			msyslog(LOG_ERR, "acts: open fails %m");
			return;
		}
		pp->io.fd = fd;
		if (!io_addclock(&pp->io)) {
			msyslog(LOG_ERR, "acts: addclock fails");
			return;
		}
		up->msgcnt = 0;
		up->bufptr = pp->a_lastcode;

		/*
		 * If the port is directly connected to the device, skip
		 * the modem business and send 'T' for Spectrabum.
		 */
		if (sys_phone[up->retry] == NULL) {
			if (write(pp->io.fd, "T", 1) < 0)
				msyslog(LOG_ERR, "acts: write T fails %m");
			up->state = S_MSG;
			up->timer = TIMECODE;
			return;
		}

		/*
		 * Initialize the modem. This works with Hayes comapatible
		 * modems.
		 */
		snprintf(tbuf, sizeof(tbuf), "SETUP %s", MODEM_SETUP); 
		report_event(PEVNT_CLOCK, peer, tbuf);
		if (write(pp->io.fd, MODEM_SETUP, strlen(MODEM_SETUP)) < 0)
			msyslog(LOG_ERR, "acts: write SETUP fails %m");
		write(pp->io.fd, "\r", 1);
		up->state = S_SETUP;
		up->timer = SETUP;
		return;

	/*
	 * In SETUP state the modem did not respond to setup.
	 * the call.
	 */
	case S_SETUP:
		report_event(PEVNT_CLOCK, peer, "no modem");
		break;

	/*
	 * In CONNECT state the call did not complete. Abort
	 * the call.
	 */
	case S_CONNECT:
		report_event(PEVNT_CLOCK, peer, "no answer");
		break;

	/*
	 * In MSG states no further timecoees are expected. If any
	 * timecodes have arrived, update the clock. In any case,
	 * ternuabte the cakk.
	 */
	case S_MSG:
		if (up->msgcnt == 0)
			report_event(PEVNT_CLOCK, peer, "no timecodes");
		else
			refclock_receive(peer);
		break;
	}
	acts_close(peer);
}


/*
 * acts_close - close and prepare for next call.
 *
 * In ClOSE state no further protocol actions are required
 * other than to close and release the device and prepare to
 * dial the next number if necessary.
 */
void
acts_close(
	struct peer *peer
	)
{
	struct actsunit *up;
	struct refclockproc *pp;
	char	lockfile[128];
	int		dtr = TIOCM_DTR;

	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;
	if (pp->io.fd != 0) {
		report_event(PEVNT_CLOCK, peer, "close");
		ioctl(pp->io.fd, TIOCMBIC, &dtr);
		io_closeclock(&pp->io);
		pp->io.fd = 0;
	}
	if (pp->sloppyclockflag & CLK_FLAG2) {
		snprintf(lockfile, sizeof(lockfile),
		    LOCKFILE, up->unit);
		unlink(lockfile);
	}
	if (up->msgcnt == 0 && up->retry > 0) {
	    if (sys_phone[up->retry] != NULL) {
			up->state = S_IDLE;
			up->timer = REDIAL;
			return;
		}
	}
	up->state = S_IDLE;
	up->timer = 0;
}


/*
 * acts_poll - called by the transmit routine
 */
static void
acts_poll (
	int	unit,
	struct peer *peer
	)
{
	struct actsunit *up;
	struct refclockproc *pp;

	/*
	 * This routine is called at every system poll. All it does is
	 * set flag1 under certain conditions. The real work is done by
	 * the timeout routine and state machine.
	 */
	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;
	switch (peer->ttl) {

	/*
	 * In manual mode the calling program is activated by the ntpdc
	 * program using the enable flag (fudge flag1), either manually
	 * or by a cron job.
	 */
	case MODE_MANUAL:
		return;

	/*
	 * In automatic mode the calling program runs continuously at
	 * intervals determined by the poll event or specified timeout.
	 */
	case MODE_AUTO:
		break;

	/*
	 * In backup mode the calling program runs continuously as long
	 * as either no peers are available or this peer is selected.
	 */
	case MODE_BACKUP:
		if (!(sys_peer == NULL || sys_peer == peer))
			return;

		break;
	}
	up->retry = 0;
	acts_timeout(peer, S_IDLE);
}


/*
 * acts_timer - called at one-second intervals
 */
static void
acts_timer(
	int	unit,
	struct peer *peer
	)
{
	struct actsunit *up;
	struct refclockproc *pp;

	/*
	 * This routine implments a timeout which runs for a programmed
	 * interval. The counter is initialized by the state machine and
	 * counts down to zero. Upon reaching zero, the state machine is
	 * called. If flag1 is set while timer is zero, force a call.
	 */
	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;
	if (up->timer == 0) {
		if (pp->sloppyclockflag & CLK_FLAG1) {
			pp->sloppyclockflag &= ~CLK_FLAG1;
			acts_timeout(peer, S_IDLE);
		}
	} else {
		up->timer--;
		if (up->timer == 0)
			acts_timeout(peer, up->state);
	}
}

/*
 * acts_timecode - identify the service and parse the timecode message
 */
void
acts_timecode(
	struct peer *peer,	/* peer structure pointer */
	char	*str		/* timecode string */
	)
{
	struct actsunit *up;
	struct refclockproc *pp;
	int	day;		/* day of the month */
	int	month;		/* month of the year */
	u_long	mjd;		/* Modified Julian Day */
	double	dut1;		/* DUT adjustment */

	u_int	dst;		/* ACTS daylight/standard time */
	u_int	leap;		/* ACTS leap indicator */
	double	msADV;		/* ACTS transmit advance (ms) */
	char	utc[10];	/* ACTS timescale */
	char	flag;		/* ACTS on-time character (* or #) */

	char	synchar;	/* WWVB synchronized indicator */
	char	qualchar;	/* WWVB quality indicator */
	char	leapchar;	/* WWVB leap indicator */
	char	dstchar;	/* WWVB daylight/savings indicator */
	int	tz;		/* WWVB timezone */

	u_int	leapmonth;	/* PTB/NPL month of leap */
	char	leapdir;	/* PTB/NPL leap direction */

	/*
	 * The parser selects the modem format based on the message
	 * length. Since the data are checked carefully, occasional
	 * errors due noise are forgivable.
	 */
	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;
	pp->nsec = 0;
	switch(strlen(str)) {

	/*
	 * For USNO format on-time character '*', which is on a line by
	 * itself. Be sure a timecode has been received.
	 */
	case 1:
		if (*str == '*' && up->msgcnt > 0) 
			break;

		return;
	
	/*
	 * ACTS format A: "jjjjj yy-mm-dd hh:mm:ss ds l uuu aaaaa
	 * UTC(NIST) *".
	 */
	case LENACTSA:
		if (sscanf(str,
		    "%5ld %2d-%2d-%2d %2d:%2d:%2d %2d %1d %3lf %5lf %9s %c",
		    &mjd, &pp->year, &month, &day, &pp->hour,
		    &pp->minute, &pp->second, &dst, &leap, &dut1,
		    &msADV, utc, &flag) != 13) {
			refclock_report(peer, CEVNT_BADREPLY);
			return;
		}
		pp->day = ymd2yd(pp->year, month, day);
		pp->leap = LEAP_NOWARNING;
		if (leap == 1)
	    		pp->leap = LEAP_ADDSECOND;
		else if (leap == 2)
	    		pp->leap = LEAP_DELSECOND;
		memcpy(&pp->refid, REFACTS, 4);
		if (up->msgcnt == 0)
			record_clock_stats(&peer->srcadr, str);
		up->msgcnt++;
		if (flag != '#' && up->msgcnt < 5)
			return;

		break;
	
	/*
	 * ACTS format B: "jjjjj yy-mm-dd hh:mm:ss ds l uuu aaaaa
	 * UTC(NIST)".
	 */
	case LENACTSB:
		if (sscanf(str,
		    "%5ld %2d-%2d-%2d %2d:%2d:%2d %2d %1d %3lf %5lf %9s",
		    &mjd, &pp->year, &month, &day, &pp->hour,
		    &pp->minute, &pp->second, &dst, &leap, &dut1,
		    &msADV, utc) != 12) {
			refclock_report(peer, CEVNT_BADREPLY);
			return;
		}
		pp->day = ymd2yd(pp->year, month, day);
		pp->leap = LEAP_NOWARNING;
		if (leap == 1)
	    		pp->leap = LEAP_ADDSECOND;
		else if (leap == 2)
	    		pp->leap = LEAP_DELSECOND;
		memcpy(&pp->refid, REFACTS, 4);
		if (up->msgcnt == 0)
			record_clock_stats(&peer->srcadr, str);
		up->msgcnt++;
		break;

	/*
	 * USNO format: "jjjjj nnn hhmmss UTC"
	 */
	case LENUSNO:
		if (sscanf(str, "%5ld %3d %2d%2d%2d %3s",
		    &mjd, &pp->day, &pp->hour, &pp->minute,
		    &pp->second, utc) != 6) {
			refclock_report(peer, CEVNT_BADREPLY);
			return;
		}

		/*
		 * Wait for the on-time character, which follows in a
		 * separate message. There is no provision for leap
		 * warning.
		 */
		pp->leap = LEAP_NOWARNING;
		memcpy(&pp->refid, REFUSNO, 4);
		if (up->msgcnt == 0)
			record_clock_stats(&peer->srcadr, str);
		up->msgcnt++;
		break;

	/*
	 * PTB/NPL format: "yyyy-mm-dd hh:mm:ss MEZ" 
	 */
	case LENPTB:
		if (sscanf(str,
		    "%*4d-%*2d-%*2d %*2d:%*2d:%2d %*5c%*12c%4d%2d%2d%2d%2d%5ld%2lf%c%2d%3lf%*15c%c",
		    &pp->second, &pp->year, &month, &day, &pp->hour,
		    &pp->minute, &mjd, &dut1, &leapdir, &leapmonth,
		    &msADV, &flag) != 12) {
			refclock_report(peer, CEVNT_BADREPLY);
			return;
		}
		pp->leap = LEAP_NOWARNING;
		if (leapmonth == month) {
			if (leapdir == '+')
		    		pp->leap = LEAP_ADDSECOND;
			else if (leapdir == '-')
		    		pp->leap = LEAP_DELSECOND;
		}
		pp->day = ymd2yd(pp->year, month, day);
		memcpy(&pp->refid, REFPTB, 4);
		if (up->msgcnt == 0)
			record_clock_stats(&peer->srcadr, str);
		up->msgcnt++;
		break;


	/*
	 * WWVB format 0: "I  ddd hh:mm:ss DTZ=nn"
	 */
	case LENWWVB0:
		if (sscanf(str, "%c %3d %2d:%2d:%2d %cTZ=%2d",
		    &synchar, &pp->day, &pp->hour, &pp->minute,
		    &pp->second, &dstchar, &tz) != 7) {
			refclock_report(peer, CEVNT_BADREPLY);
			return;
		}
		pp->leap = LEAP_NOWARNING;
		if (synchar != ' ')
			pp->leap = LEAP_NOTINSYNC;
		memcpy(&pp->refid, REFWWVB, 4);
		if (up->msgcnt == 0)
			record_clock_stats(&peer->srcadr, str);
		up->msgcnt++;
		break;

	/*
	 * WWVB format 2: "IQyy ddd hh:mm:ss.mmm LD"
	 */
	case LENWWVB2:
		if (sscanf(str, "%c%c%2d %3d %2d:%2d:%2d.%3ld%c%c%c",
		    &synchar, &qualchar, &pp->year, &pp->day,
		    &pp->hour, &pp->minute, &pp->second, &pp->nsec,
		    &dstchar, &leapchar, &dstchar) != 11) {
			refclock_report(peer, CEVNT_BADREPLY);
			return;
		}
		pp->nsec *= 1000000;
		pp->leap = LEAP_NOWARNING;
		if (synchar != ' ')
			pp->leap = LEAP_NOTINSYNC;
		else if (leapchar == 'L')
			pp->leap = LEAP_ADDSECOND;
		memcpy(&pp->refid, REFWWVB, 4);
		if (up->msgcnt == 0)
			record_clock_stats(&peer->srcadr, str);
		up->msgcnt++;
		break;

	/*
	 * None of the above. Just forget about it and wait for the next
	 * message or timeout.
	 */
	default:
		return;
	}

	/*
	 * We have a valid timecode. The fudge time1 value is added to
	 * each sample by the main line routines. Note that in current
	 * telephone networks the propatation time can be different for
	 * each call and can reach 200 ms for some calls.
	 */
	peer->refid = pp->refid;
	pp->lastrec = up->tstamp;
	if (up->msgcnt == 0)
		return;

	if (!refclock_process(pp)) {
		refclock_report(peer, CEVNT_BADTIME);
		return;
	}
	pp->lastref = pp->lastrec;
}
#else
int refclock_acts_bs;
#endif /* REFCLOCK */
