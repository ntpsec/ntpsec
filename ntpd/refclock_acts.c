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
 * This driver requires a 9600-bps modem with a Hayes-compatible command
 * set and control over the modem data terminal ready (DTR) control
 * line. The modem setup string is hard-coded in the driver and may
 * require changes for nonstandard modems or special circumstances.
 *
 * The calling program is initiated by setting fudge flag1, either
 * manually or automatically. When flag1 is set, the calling program
 * dials each number listed in the phones command of the configuration
 * file in turn. The number is specified by the Hayes ATDT prefix
 * followed by the number itself, including the prefix and long-distance
 * digits and delay code, if necessary. The flag1 is reset and the
 * calling program terminated if (a) a valid clock update has been
 * determined, (b) no more numbers remain in the list, (c) a device
 * fault or timeout occurs or (d) fudge flag1 is reset manually.
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
 * flag3	no modem; port is directly connected to device
 * flag4	not used
 *
 * time1	offset adjustment (s)
 *
 * Ordinarily, the serial port is connected to a modem; however, it can
 * be connected directly to a device or another computer for testing and
 * calibration. In this case set fudge flag3 and the driver will send a
 * single character 'T' at each poll event. In principle, fudge flag2
 * enables port locking, allowing the modem to be shared when not in use
 * by this driver. At least on Solaris with the current NTP I/O
 * routines, this results only in lots of ugly error messages.
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
 * 47999 90-04-18 21:39:15 50 0 +.1 045.0 UTC(NIST) *
 * 47999 90-04-18 21:39:16 50 0 +.1 045.0 UTC(NIST) *
 * ...
 *
 * MJD, DST, DUT1 and UTC are not used by this driver. The "*" or "#" is
 * the on-time markers echoed by the driver and used by NIST to measure
 * and correct for the propagation delay.
 *
 * US Naval Observatory (USNO)
 *
 * Phone: (202) 762-1594 (Washington, DC); (719) 567-6742 (Boulder, CO)
 *
 * Data Format (two lines, repeating at one-second intervals)
 *
 * jjjjj nnn hhmmss UTC
 * *
 *
 * *		on-time marker
 * jjjjj	modified Julian day number (not used)
 * nnn		day of year
 * hhmmss	second of day
 * ...
 *
 * European Services (PTB, NPL, etc.)

 * PTB: +49 531 512038 (Germany)
 * NPL: 0906 851 6333 (UK only)
 *
 * Data format (see the documentation for phone numbers and formats.)
 *
 * 1995-01-23 20:58:51 MEZ  10402303260219950123195849740+40000500
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
#define LOCKFILE	"/var/spool/locks/LCK..cua%d"
#define DESCRIPTION	"Automated Computer Time Service" /* WRU */
#define REFID		"NONE"	/* default reference ID */
#define MSGCNT		20	/* max message count */
#define SMAX		80	/* max clockstats line length */

/*
 * Calling program modes
 */
#define MODE_AUTO	0	/* automatic mode */
#define MODE_PREFER	1	/* prefer mode */
#define MODE_MANUAL	2	/* manual mode */

/*
 * Service identifiers.
 */
#define REFACTS		"NIST"	/* NIST reference ID */
#define LENACTS		50	/* NIST format */
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
 * &C1	enable carrier detect
 * &D2	hang up and return to command mode on DTR transition
 * E0	modem command echo disabled
 * l1	set modem speaker volume to low level
 * M1	speaker enabled until carrier detect
 * Q0	return result codes
 * V1	return result codes as English words
 */
#define MODEM_SETUP	"ATB1&C1&D2E0L1M1Q0V1\r" /* modem setup */
#define MODEM_HANGUP	"ATH\r"	/* modem disconnect */

/*
 * Timeouts (all in seconds)
 */
#define WAIT		2	/* DTR timeout */
#define MODEM		3	/* modem timeout */
#define ANSWER		60	/* answer timeout */
#define CONNECT		10	/* first valid message timeout */
#define TIMECODE	30	/* all valid messages timeout */

/*
 * State machine codes
 */
#define S_IDLE		0	/* wait for poll */
#define S_DTR		1	/* wait for DTR */
#define S_OK		2	/* wait for modem */
#define S_CONNECT	3	/* wait for answer*/
#define S_FIRST		4	/* wait for first valid message */
#define S_MSG		5	/* wait for all messages */

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
static	int	acts_start	P((int, struct peer *));
static	void	acts_shutdown	P((int, struct peer *));
static	void	acts_receive	P((struct recvbuf *));
static	void	acts_message	P((struct peer *));
static	void	acts_poll	P((int, struct peer *));
static	void	acts_timeout	P((struct peer *));
static	void	acts_disc	P((struct peer *));
static	void	acts_timer	P((int, struct peer *));

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

struct	refclock refclock_ptb;

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
	if (up == NULL)
		return (0);

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

	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;
	acts_disc(peer);
	io_closeclock(&pp->io);
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
	 * interfact kicks up; so, we have to reassemble messages from
	 * arbitrary fragments. Capture the timecode at the beginning of
	 * the message and at the '*' on-time character.
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
		} else {
			*up->bufptr++ = *tptr;
			if (*tptr == '*' || *tptr == '#') {
				up->tstamp = pp->lastrec;
				write(pp->io.fd, tptr, 1);
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
	char	tbuf[SMAX];
	u_int	len;
	int	day;		/* day of the month */
	int	month;		/* month of the year */
	u_long	mjd;		/* Modified Julian Day */
	double	dut1;		/* DUT adjustment */
	double	msADV;		/* ACTS transmit advance (ms) */
	char	utc[10];	/* NIST timescale */
	int	tz;		/* WWVB timezone */
	char	flag;		/* ACTS on-time character (* or #) */
	char	synchar;	/* WWVB synchronized indicator */
	char	qualchar;	/* WWVB quality indicator */
	char	leapchar;	/* WWVB leap indicator */
	u_int	leap;		/* ACTS leap indicator */
	u_int	leapmonth;	/* PTB/NPL month of leap */
	char	leapdir;	/* USNO leap direction */
	char	dstchar;	/* WWVB daylight/savings indicator */
	u_int	dst;		/* ACTS daylight/standard time */

	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;
	len = strlen(pp->a_lastcode);
	sprintf(tbuf, "acts: (%d %d) %d %s", up->state, up->timer, len,
	    pp->a_lastcode);
#ifdef DEBUG
	if (debug)
		printf("%s\n", tbuf);
#endif
	switch(up->state) {

	/*
	 * We are not expecting anything. Probably modem disconnect
	 * noise. Go back to sleep.
	 */
	case S_IDLE:
		return;

	/*
	 * We are waiting for the OK response to the modem setup
	 * command. When this happens dial the number.
	 */
	case S_OK:
		if (strcmp(pp->a_lastcode, "OK") != 0) {
			acts_disc(peer);
			return;
		}
		sprintf(tbuf, "acts: dial %d %s", up->retry,
		    sys_phone[up->retry]);
		record_clock_stats(&peer->srcadr, tbuf);
#ifdef DEBUG
		if (debug)
			printf("%s\n", tbuf);
#endif
		len = strlen(sys_phone[up->retry]);
		if (write(pp->io.fd, sys_phone[up->retry], len)
		    < 0) {
			msyslog(LOG_ERR, "acts: *m");
			acts_disc(peer);
			return;
		}
		up->state = S_CONNECT;
		up->timer = ANSWER;
		return;

	/*
	 * We are waiting for the call to be answered. All we care about
	 * here is CONNECT as the first token in the string.
	 */
	case S_CONNECT:
		record_clock_stats(&peer->srcadr, tbuf);
		strncpy(tbuf, strtok(pp->a_lastcode, " "), SMAX);
		if (strcmp(tbuf, "CONNECT") != 0) {
			acts_disc(peer);
			return;
		}
		up->state = S_FIRST;
		up->timer = CONNECT;
		return;
	}

	/*
	 * Real yucky things here in S_FIRST and S_MSG states. Ignore
	 * everything except timecode messages, as determined by the
	 * message length. Since the data are checked carefully,
	 * occasional errors due noise are forgivable.
	 */
	pp->nsec = 0;
	switch(len) {

	/*
	 * For USNO format on-time character '*', which is on a line by
	 * itself. By sure a timecode has been received.
	 */
	case 1:
		if (*pp->a_lastcode == '*' && up->state == S_MSG) 
			break;

		return;
	
	/*
	 * ACTS format: "jjjjj yy-mm-dd hh:mm:ss ds l uuu aaaaa
	 * UTC(NIST) *"
	 */
	case LENACTS:
		if (sscanf(pp->a_lastcode,
		    "%5ld %2d-%2d-%2d %2d:%2d:%2d %2d %1d %3lf %5lf %s %c",
		    &mjd, &pp->year, &month, &day, &pp->hour,
		    &pp->minute, &pp->second, &dst, &leap, &dut1,
		    &msADV, utc, &flag) != 13) {
			refclock_report(peer, CEVNT_BADREPLY);
			return;
		}

		/*
		 * Reset the timer and wait until NIST has calculated
		 * roundtrip delay.
		 */
		if (up->state != S_MSG) {
			up->state = S_MSG;
			up->timer = TIMECODE;
		}
		if (flag != '#')
			return;

		pp->lastrec = up->tstamp;
		pp->day = ymd2yd(pp->year, month, day);
		if (leap == 1)
	    		pp->leap = LEAP_ADDSECOND;
		else if (pp->leap == 2)
	    		pp->leap = LEAP_DELSECOND;
		memcpy(&pp->refid, REFACTS, 4);
		if (up->msgcnt == 0)
			record_clock_stats(&peer->srcadr, tbuf);
		up->msgcnt++;
		break;

	/*
	 * USNO format: "jjjjj nnn hhmmss UTC"
	 */
	case LENUSNO:
		if (sscanf(pp->a_lastcode, "%5ld %3d %2d%2d%2d %s",
		    &mjd, &pp->day, &pp->hour, &pp->minute,
		    &pp->second, utc) != 6) {
			refclock_report(peer, CEVNT_BADREPLY);
			return;
		}

		/*
		 * Reset the timer and wait for the on-time character.
		 */
		if (up->state != S_MSG) {
			up->state = S_MSG;
			up->timer = TIMECODE;
		}
		pp->lastrec = up->tstamp;
		memcpy(&pp->refid, REFUSNO, 4);
		if (up->msgcnt == 0)
			record_clock_stats(&peer->srcadr, tbuf);
		up->msgcnt++;
		return;

	/*
	 * PTB/NPL format: "yyyy-mm-dd hh:mm:ss MEZ" 
	 */
	case LENPTB:
		if (sscanf(pp->a_lastcode,
		    "%*4d-%*2d-%*2d %*2d:%*2d:%2d %*5c%*12c%4d%2d%2d%2d%2d%5ld%2lf%c%2d%3lf%*15c%c",
		    &pp->second, &pp->year, &month, &day, &pp->hour,
		    &pp->minute, &mjd, &dut1, &leapdir, &leapmonth,
		    &msADV, &flag) != 12) {
			refclock_report(peer, CEVNT_BADREPLY);
			return;
		}

		/*
		 * Reset the timer and wait for the on-time character.
		 */
		if (up->state != S_MSG) {
			up->state = S_MSG;
			up->timer = TIMECODE;
		}
		pp->lastrec = up->tstamp;
		if (leapmonth == month) {
			if (leapdir == '+')
		    		pp->leap = LEAP_ADDSECOND;
			else if (leapdir == '-')
		    		pp->leap = LEAP_DELSECOND;
		}
		pp->day = ymd2yd(pp->year, month, day);
		memcpy(&pp->refid, REFPTB, 4);
		if (up->msgcnt == 0)
			record_clock_stats(&peer->srcadr, tbuf);
		up->msgcnt++;
		break;


	/*
	 * WWVB format 0: "I  ddd hh:mm:ss DTZ=nn"
	 */
	case LENWWVB0:
		if (sscanf(pp->a_lastcode,
		    "%c %3d %2d:%2d:%2d %cTZ=%2d",
		    &synchar, &pp->day, &pp->hour, &pp->minute,
		    &pp->second, &dstchar, &tz) != 7) {
			refclock_report(peer, CEVNT_BADREPLY);
			return;
		}
		pp->lastrec = up->tstamp;
		if (synchar != ' ')
			pp->leap = LEAP_NOTINSYNC;
		memcpy(&pp->refid, REFWWVB, 4);
		if (up->msgcnt == 0)
			record_clock_stats(&peer->srcadr, tbuf);
		up->msgcnt++;
		break;

	/*
	 * WWVB format 2: "IQyy ddd hh:mm:ss.mmm LD"
	 */
	case LENWWVB2:
		if (sscanf(pp->a_lastcode,
		    "%c%c%2d %3d %2d:%2d:%2d.%3ld%c%c%c",
		    &synchar, &qualchar, &pp->year, &pp->day,
		    &pp->hour, &pp->minute, &pp->second, &pp->nsec,
		    &dstchar, &leapchar, &dstchar) != 11) {
			refclock_report(peer, CEVNT_BADREPLY);
			return;
		}
		pp->lastrec = up->tstamp;
		pp->nsec *= 1000000;
		if (synchar != ' ')
			pp->leap = LEAP_NOTINSYNC;
		else if (leapchar == 'L')
			pp->leap = LEAP_ADDSECOND;
		else
			pp->leap = LEAP_NOWARNING;
		memcpy(&pp->refid, REFWWVB, 4);
		if (up->msgcnt == 0)
			record_clock_stats(&peer->srcadr, tbuf);
		up->msgcnt++;
		break;

	/*
	 * None of the above. Just forget about it and wait for the next
	 * message or timeout.
	 */
	default:
		return;
	}
	peer->refid = pp->refid;

	/*
	 * The fudge time1 value is added to each sample by the main
	 * line routines. Note that we use the median filter only when
	 * the dispersion has receeded below the threshold.
	 */
	if (refclock_process(pp)) {
		pp->lastref = pp->lastrec;
		if (peer->disp > MAXDISTANCE)
			refclock_receive(peer);
		if (up->state != S_MSG) {
			up->state = S_MSG;
			up->timer = TIMECODE;
		}
	} else {
		refclock_report(peer, CEVNT_BADTIME);
	}
	if (up->msgcnt < MSGCNT)
		return;

	acts_disc(peer);
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
	 * the state machine.
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
		/* fall through */
		break;

	/*
	 * In automatic mode the calling program runs continuously at
	 * intervals determined by the poll event or specified timeout.
	 */
	case MODE_AUTO:
		pp->sloppyclockflag |= CLK_FLAG1;
		break;

	/*
	 * In prefer mode the calling program runs continuously as long
	 * as the prefer peer is unreachable.
	 */
	case MODE_PREFER:
		if (sys_prefer != NULL) {
			if (peer->unreach >= NTP_UNREACH)
				pp->sloppyclockflag |= CLK_FLAG1;
		}
		break;
	}
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
	 * called. If flag1 is set while in S_IDLE state, force a
	 * timeout.
	 */
	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;
	if (pp->sloppyclockflag & CLK_FLAG1 && up->state == S_IDLE) {
		acts_timeout(peer);
		return;
	}
	if (up->timer == 0)
		return;

	up->timer--;
	if (up->timer == 0)
		acts_timeout(peer);
}


/*
 * acts_timeout - called by the timer interrupt
 */
static void
acts_timeout(
	struct peer *peer
	)
{
	struct actsunit *up;
	struct refclockproc *pp;
	int	dtr = TIOCM_DTR;
	int	fd;
	char	device[20];
	char	lockfile[128], pidbuf[8];

	/*
	 * The state machine is driven by messages from the modem, when
	 * first stated and at timeout.
	 */
	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;
	pp->sloppyclockflag &= ~CLK_FLAG1;
	switch(up->state) {

	/*
	 * System poll event. Lock the modem port and enable the device.
	 */
	case S_IDLE:

		/*
		 * Lock the port. If busy, retry later.
		 */
		if (pp->sloppyclockflag & CLK_FLAG2) {
			sprintf(lockfile, LOCKFILE, up->unit);
			fd = open(lockfile, O_WRONLY | O_CREAT | O_EXCL,
			    0644);
			if (fd < 0) {
				msyslog(LOG_ERR, "acts: port busy");
				break;
			}
			sprintf(pidbuf, "%d\n", (u_int)getpid());
			write(fd, pidbuf, strlen(pidbuf));
			close(fd);
		}

		/*
		 * Open device and light up a discipline if present.
		 */
		if (!pp->io.fd) {
			sprintf(device, DEVICE, up->unit);
			fd = refclock_open(device, SPEED232,
			    LDISC_ACTS | LDISC_RAW);
			if (fd < 0) {
				msyslog(LOG_ERR,
				    "acts: device open fails");
				break;
			}
			pp->io.fd = fd;
			if (!io_addclock(&pp->io)) {
				msyslog(LOG_ERR,
				    "acts: addclock fails");
				break;
			}
		}
		/*
		 * If the port is directly connected to the device, skip
		 * the modem business and send 'T' for Spectrabum.
		 */
		if (pp->sloppyclockflag & CLK_FLAG3) {
			if (write(pp->io.fd, "T", 1) < 0) {
				msyslog(LOG_ERR, "acts: write %m");
				break;
			}
			up->state = S_FIRST;
			up->timer = CONNECT;
			return;
		}

		if (ioctl(pp->io.fd, TIOCMBIS, (char *)&dtr) < 0) {
			msyslog(LOG_ERR, "acts: ioctl %m");
			break;
		}
		up->state = S_DTR;
		up->timer = WAIT;
		return;

	/*
	 * DTR timeout. Send modem setup string.
	 */
	case S_DTR:
#ifdef DEBUG
		if (debug)
			printf("acts: setup %s\n", MODEM_SETUP);
#endif
		if (write(pp->io.fd, MODEM_SETUP, sizeof(MODEM_SETUP)) <
		    0) {
			msyslog(LOG_ERR, "acts: write %m");
			break;
		}
		up->state = S_OK;
		up->timer = MODEM;
		return;
	}
	acts_disc(peer);
}


/*
 * acts_disc - disconnect the call and clean the place up.
 */
static void
acts_disc (
	struct peer *peer
	)
{
	struct actsunit *up;
	struct refclockproc *pp;
	int	dtr = TIOCM_DTR;
	char	tbuf[SMAX];	/* monitor buffer */
	char	lockfile[128];

	/*
	 * We get here if the call terminated successfully or if the
	 * call timed out. We drop DTR, which will reliably get the
	 * modem off the air and turn off the meter, even while the
	 * service is hammering away full tilt.
	 */
	pp = peer->procptr;
	up = (struct actsunit *)pp->unitptr;
	switch(up->state) {

	/*
	 * In OK state the modem did not respond to setup.
	 */
	case S_OK:
		msyslog(LOG_ERR, "acts: no modem");
		break;

	/*
	 * In CONNECT state the call did not complete.
	 */
	case S_CONNECT:
		msyslog(LOG_ERR, "acts: no answer");
		break;

	/*
	 * In FIRST state no messages were received.
	 */
	case S_FIRST:
		msyslog(LOG_ERR, "acts: no messages");
		break;
	}

	/*
	 * If messages have arrived, stash the data and turn off the
	 * bubble machine. If not, try the next number. If no next
	 * number, fold the tent and go home.
	 */
	if (up->msgcnt > 0) {
		refclock_receive(peer);
		up->retry = 0;
	} else {
		up->retry++;
		if (*sys_phone[up->retry] == '\0') {
			up->retry = 0;
			sprintf(tbuf, "acts: call failed");
			record_clock_stats(&peer->srcadr, tbuf);
#ifdef DEBUG
			if (debug)
				printf("%s\n", tbuf);
#endif
		}
	}

	/*
	 * If the device is open, send hangup, clear DTR, close the
	 * device and remove the lock file.
	 */
	if (pp->io.fd) {
		if (!(pp->sloppyclockflag & CLK_FLAG3)) {
			sprintf(tbuf, "acts: hangup");
			record_clock_stats(&peer->srcadr, tbuf);
#ifdef DEBUG
			if (debug)
				printf("%s\n", tbuf);
#endif
			write(pp->io.fd, MODEM_HANGUP,
			    sizeof(MODEM_HANGUP));
			ioctl(pp->io.fd, TIOCMBIC, (char *)&dtr);
		}
		if (pp->sloppyclockflag & CLK_FLAG2) {
			close(pp->io.fd);
			pp->io.fd = 0;
			sprintf(lockfile, LOCKFILE, up->unit);
			unlink(lockfile);
		}

	}

	/*
	 * If there are more numbers to dial or the modem is not in
	 * command mode, retry after a short wait.
	 */
	up->msgcnt = 0;
	up->timer = 0;
	up->bufptr = pp->a_lastcode;
	if (up->retry > 0 || up->state == S_OK)
		up->timer = MODEM;
	up->state = S_IDLE;
}

#else
int refclock_acts_bs;
#endif /* REFCLOCK */
