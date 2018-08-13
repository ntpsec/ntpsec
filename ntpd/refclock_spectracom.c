/*
 * refclock_spectracom.c - clock driver for Spectracom GPS receivers
 */

#include "config.h"
#include "ntp.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_refclock.h"
#include "ntp_calendar.h"
#include "ntp_stdlib.h"

#include <stdio.h>
#include <ctype.h>

#ifdef HAVE_PPSAPI
#include "ppsapi_timepps.h"
#include "refclock_pps.h"
#endif /* HAVE_PPSAPI */

/*
 * This driver supports the "Type 2" format emitted by Spectracom time
 * servers including the 9483, 9489, and SecureSync.
 *
 * WARNING: This driver depends on the system clock for year disambiguation.
 * It will thus not be usable for recovery if the system clock is trashed.
 *
 * In former times this driver supported the Spectracom 9300 (now
 * end-of-lifed) and several models of Spectracom radio clocks that were
 * obsolesced by the WWVB modulation change at 2012-10-29T15:00:00Z.
 *
 * There are two timecode formats used by these clocks:
 *
 * Format 0 (22 ASCII printing characters):
 *
 * <cr><lf>i  ddd hh:mm:ss TZ=zz<cr><lf>
 *
 *	on-time = first <cr>
 *	hh:mm:ss = hours, minutes, seconds
 *	i = synchronization flag (' ' = in synch, '?' = out of synch)
 *
 * The alarm condition is indicated by other than ' ' at i, which occurs
 * during initial synchronization and when received signal is lost for
 * about ten hours.
 *
 * Format 2 (24 ASCII printing characters):
 *
 * <cr><lf>iqyy ddd hh:mm:ss.fff ld
 *
 *	on-time = <cr>
 *	i = synchronization flag (' ' = in synch, '?' = out of synch)
 *	q = quality indicator (' ' = locked, 'A'...'D' = unlocked)
 *	yy = year (as broadcast)
 *	ddd = day of year
 *	hh:mm:ss.fff = hours, minutes, seconds, milliseconds
 *
 * The alarm condition is indicated by other than ' ' at i, which occurs
 * during initial synchronization and when received signal is lost for
 * about ten hours. The unlock condition is indicated by other than ' '
 * at q.
 *
 * The q is normally ' ' when the time error is less than 1 ms and a
 * character in the set 'A'...'D' when the time error is less than 10,
 * 100, 500 and greater than 500 ms respectively. The l is normally ' ',
 * but is set to 'L' early in the month of an upcoming UTC leap second
 * and reset to ' ' on the first day of the following month. The d is
 * set to 'S' for standard time 'I' on the day preceding a switch to
 * daylight time, 'D' for daylight time and 'O' on the day preceding a
 * switch to standard time. The start bit of the first <cr> is
 * synchronized to the indicated time as returned.
 *
 * This driver does not need to be told which format is in use - it
 * figures out which one from the length of the message. The driver
 * makes no attempt to correct for the intrinsic jitter of the radio
 * itself, which is a known problem with the older radios.
 *
 * PPS Signal Processing
 *
 * When PPS signal processing is enabled, and when the system clock has
 * been set by this or another driver and the PPS signal offset is
 * within 0.4 s of the system clock offset, the PPS signal replaces the
 * timecode for as long as the PPS signal is active. If for some reason
 * the PPS signal fails for one or more poll intervals, the driver
 * reverts to the timecode. If the timecode fails for one or more poll
 * intervals, the PPS signal is disconnected.
 *
 * Fudge Factors
 *
 * This driver can retrieve a table of quality data maintained
 * internally by the Netclock/2 clock. If option flag4 is set to 1,
 * the driver will retrieve this table and write it to the clockstats
 * file when the first timecode message of a new day is received.
 *
 * PPS calibration fudge time 1: format 0 .003134, format 2 .004034
 */
/*
 * Interface definitions
 */
#define	DEVICE		"/dev/spectracom%d" /* device name and unit */
#define	SPEED232	B9600		/* uart speed (9600 baud) */
#define	PRECISION	(-13)		/* precision assumed (about 100 us) */
#ifdef HAVE_PPSAPI
# define PPS_PRECISION	(-13)		/* precision assumed (about 100 us) */
#endif
#define	REFID		"GPS\0"		/* reference ID */
#define NAME		"SPECTRACOM"	/* shortname */
#define	DESCRIPTION	"Spectracom GPS Receiver" /* WRU */

#define	LENTYPE0	22		/* format 0 timecode length */
#define	LENTYPE2	24		/* format 2 timecode length */
#define LENTYPE3	29		/* format 3 timecode length */
#define MONLIN		15		/* number of monitoring lines */

/*
 * Spectracom unit control structure
 */
struct spectracomunit {
#ifdef HAVE_PPSAPI
	struct refclock_ppsctl ppsctl; /* PPSAPI structure */
	int	ppsapi_tried;	/* attempt PPSAPI once */
	int	ppsapi_lit;	/* time_pps_create() worked */
	int	tcount;		/* timecode sample counter */
	int	pcount;		/* PPS sample counter */
#endif /* HAVE_PPSAPI */
	l_fp	laststamp;	/* last <CR> timestamp */
	bool	prev_eol_cr;	/* was last EOL <CR> (not <LF>)? */
	uint8_t	lasthour;	/* last hour (for monitor) */
	uint8_t	linect;		/* count ignored lines (for monitor */
};

/*
 * Function prototypes
 */
static	bool	spectracom_start	(int, struct peer *);
static	void	spectracom_receive	(struct recvbuf *);
static	void	spectracom_poll		(int, struct peer *);
static	void	spectracom_timer	(int, struct peer *);
#ifdef HAVE_PPSAPI
static	void	spectracom_control	(int, const struct refclockstat *,
				 struct refclockstat *, struct peer *);
#define		SPECTRACOM_CONTROL	spectracom_control
#else
#define		SPECTRACOM_CONTROL	NULL
#endif /* HAVE_PPSAPI */

/*
 * Transfer vector
 */
struct	refclock refclock_spectracom = {
	NAME,				/* basename of driver */
	spectracom_start,		/* start up driver */
	NULL,				/* shut down driver in standard way */
	spectracom_poll,		/* transmit poll message */
	SPECTRACOM_CONTROL,		/* fudge set/change notification */
	NULL,				/* initialize driver (not used) */
	spectracom_timer		/* called once per second */
};


/*
 * spectracom_start - open the devices and initialize data for processing
 */
static bool
spectracom_start(
	int unit,
	struct peer *peer
	)
{
	struct spectracomunit *up;
	struct refclockproc *pp;
	int fd;
	char device[20];

	/*
	 * Open serial port. Use CLK line discipline, if available.
	 */
	snprintf(device, sizeof(device), DEVICE, unit);
	fd = refclock_open(peer->cfg.path ? peer->cfg.path : device,
			   peer->cfg.baud ? peer->cfg.baud : SPEED232,
			   LDISC_CLK);
	if (fd <= 0)
		/* coverity[leaked_handle] */
		return false;

	/*
	 * Allocate and initialize unit structure
	 */
	up = emalloc_zero(sizeof(*up));
	pp = peer->procptr;
	pp->io.clock_recv = spectracom_receive;
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
	memcpy(&pp->refid, REFID, REFIDLEN);
	peer->sstclktype = CTL_SST_TS_LF;
	return true;
}


/*
 * spectracom_receive - receive data from the serial interface
 */
static void
spectracom_receive(
	struct recvbuf *rbufp
	)
{
	struct spectracomunit *up;
	struct refclockproc *pp;
	struct peer *peer;

	l_fp	trtmp;		/* arrival timestamp */
	int	tz;		/* time zone */
	int	day, month;	/* ddd conversion */
	int	temp;		/* int temp */
	char	syncchar;	/* synchronization indicator */
	char	qualchar;	/* quality indicator */
	char	leapchar;	/* leap indicator */
	char	dstchar;	/* daylight/standard indicator */
	char	tmpchar;	/* trashbin */

	/*
	 * Initialize pointers and read the timecode and timestamp
	 */
	peer = rbufp->recv_peer;
	pp = peer->procptr;
	up = pp->unitptr;
	temp = refclock_gtlin(rbufp, pp->a_lastcode, BMAX, &trtmp);

	/*
	 * Note we get a buffer and timestamp for both a <cr> and <lf>,
	 * but only the <cr> timestamp is retained. Note: in format 0 on
	 * a Netclock/2 or upgraded 8170 the start bit is delayed 100
	 * +-50 us relative to the pps; however, on an unmodified 8170
	 * the start bit can be delayed up to 10 ms. In format 2 the
	 * reading precision is only to the millisecond. Thus, unless
	 * you have a PPS gadget and don't have to have the year, format
	 * 0 provides the lowest jitter.
	 * Save the timestamp of each <CR> in up->laststamp.  Lines with
	 * no characters occur for every <LF>, and for some <CR>s when
	 * format 0 is used. Format 0 starts and ends each cycle with a
	 * <CR><LF> pair, format 2 starts each cycle with its only pair.
	 * The preceding <CR> is the on-time character for both formats.
	 * The timestamp provided with non-empty lines corresponds to
	 * the <CR> following the timecode, which is ultimately not used
	 * with format 0 and is used for the following timecode for
	 * format 2.
	 */
	if (temp == 0) {
		if (up->prev_eol_cr) {
			DPRINT(2, ("spectracom: <LF> @ %s\n", prettydate(trtmp)));
		} else {
			up->laststamp = trtmp;
			DPRINT(2, ("spectracom: <CR> @ %s\n", prettydate(trtmp)));
		}
		up->prev_eol_cr = !up->prev_eol_cr;
		return;
	}
	pp->lencode = temp;
	pp->lastrec = up->laststamp;
	up->laststamp = trtmp;
	up->prev_eol_cr = true;
	DPRINT(2, ("spectracom: code @ %s\n"
		   "       using %s minus one char\n",
		   prettydate(trtmp), prettydate(pp->lastrec)));
	if (pp->lastrec == 0)
		return;

	/*
	 * We get down to business, check the timecode format and decode
	 * its contents. This code uses the timecode length to determine
	 * format 0, 2 or 3. If the timecode has invalid length or is
	 * not in proper format, we declare bad format and exit.
	 */
	syncchar = qualchar = leapchar = dstchar = ' ';
	tz = 0;
	switch (pp->lencode) {

	case LENTYPE0:

		/*
		 * Timecode format 0: "I  ddd hh:mm:ss DTZ=nn"
		 */
		if (sscanf(pp->a_lastcode,
		    "%c %3d %2d:%2d:%2d%c%cTZ=%2d",
		    &syncchar, &pp->day, &pp->hour, &pp->minute,
		    &pp->second, &tmpchar, &dstchar, &tz) == 8) {
			pp->nsec = 0;
			break;
		}
		goto bad_format;

	case LENTYPE2:

		/*
		 * Timecode format 2: "IQyy ddd hh:mm:ss.mmm LD" */
		if (sscanf(pp->a_lastcode,
		    "%c%c %2d %3d %2d:%2d:%2d.%3ld %c",
		    &syncchar, &qualchar, &pp->year, &pp->day,
		    &pp->hour, &pp->minute, &pp->second, &pp->nsec,
		    &leapchar) == 9) {
			pp->nsec *= 1000000;
			break;
		}
		goto bad_format;

	case LENTYPE3:

		/*
		 * Timecode format 3: "0003I yyyymmdd hhmmss+0000SL#"
		 * WARNING: Undocumented, and the on-time character # is
		 * not yet handled correctly by this driver.  It may be
		 * as simple as compensating for an additional 1/960 s.
		 */
		if (sscanf(pp->a_lastcode,
		    "0003%c %4d%2d%2d %2d%2d%2d+0000%c%c",
		    &syncchar, &pp->year, &month, &day, &pp->hour,
		    &pp->minute, &pp->second, &dstchar, &leapchar) == 8)
		    {
			pp->day = ymd2yd(pp->year, month, day);
			pp->nsec = 0;
			break;
		}
		goto bad_format;

	default:
	bad_format:

		/*
		 * Unknown format: If dumping internal table, record
		 * stats; otherwise, declare bad format.
		 */
		if (up->linect > 0) {
			up->linect--;
			record_clock_stats(peer,
			    pp->a_lastcode);
		} else {
			refclock_report(peer, CEVNT_BADREPLY);
		}
		return;
	}

	/*
	 * Decode synchronization, quality and leap characters. If
	 * unsynchronized, set the leap bits accordingly and exit.
	 * Otherwise, set the leap bits according to the leap character.
	 * Once synchronized, the dispersion depends only on the
	 * quality character.
	 */
	switch (qualchar) {

	case ' ':
		pp->disp = .001;
		pp->lastref = pp->lastrec;
		break;

	case 'A':
		pp->disp = .01;
		break;

	case 'B':
		pp->disp = .1;
		break;

	case 'C':
		pp->disp = .5;
		break;

	case 'D':
		pp->disp = sys_maxdisp;
		break;

	default:
		pp->disp = sys_maxdisp;
		refclock_report(peer, CEVNT_BADREPLY);
		break;
	}
	if (syncchar != ' ')
		pp->leap = LEAP_NOTINSYNC;
	else if (leapchar == 'L')
		pp->leap = LEAP_ADDSECOND;
	else
		pp->leap = LEAP_NOWARNING;

	/*
	 * Process the new sample in the median filter and determine the
	 * timecode timestamp, but only if the PPS is not in control.
	 */
#ifdef HAVE_PPSAPI
	up->tcount++;
	if (peer->cfg.flags & FLAG_PPS)
		return;

#endif /* HAVE_PPSAPI */
	if (!refclock_process_f(pp, pp->fudgetime2))
		refclock_report(peer, CEVNT_BADTIME);
}


/*
 * spectracom_timer - called once per second by the transmit procedure
 */
static void
spectracom_timer(
	int unit,
	struct peer *peer
	)
{
	UNUSED_ARG(unit);

	struct spectracomunit *up;
	struct refclockproc *pp;
	char	pollchar;	/* character sent to clock */
#ifdef DEBUG
	l_fp	now;
#endif

	/*
	 * Time to poll the clock. The Spectracom clock responds to a
	 * 'T' by returning a timecode in the format(s) specified above.
	 * Note there is no checking on state, since this may not be the
	 * only customer reading the clock. Only one customer need poll
	 * the clock; all others just listen in.
	 */
	pp = peer->procptr;
	up = pp->unitptr;
	if (up->linect > 0)
		pollchar = 'R';
	else
		pollchar = 'T';
	if (write(pp->io.fd, &pollchar, 1) != 1)
		refclock_report(peer, CEVNT_FAULT);
#ifdef DEBUG
	get_systime(&now);
	if (debug) /* SPECIAL DEBUG */
		printf("%c poll at %s\n", pollchar, prettydate(now));
#endif
#ifdef HAVE_PPSAPI
	if (up->ppsapi_lit &&
	    refclock_catcher(peer, &up->ppsctl, pp->sloppyclockflag) > 0) {
		up->pcount++,
		peer->cfg.flags |= FLAG_PPS;
		peer->precision = PPS_PRECISION;
	}
#endif /* HAVE_PPSAPI */
}


/*
 * spectracom_poll - called by the transmit procedure
 */
static void
spectracom_poll(
	int unit,
	struct peer *peer
	)
{
	struct spectracomunit *up;
	struct refclockproc *pp;

	UNUSED_ARG(unit);

	/*
	 * Sweep up the samples received since the last poll. If none
	 * are received, declare a timeout and keep going.
	 */
	pp = peer->procptr;
	up = pp->unitptr;
	pp->polls++;

	/*
	 * If the monitor flag is set (flag4), we dump the internal
	 * quality table at the first timecode beginning the day.
	 */
	if (pp->sloppyclockflag & CLK_FLAG4 && pp->hour <
	    (int)up->lasthour)
		up->linect = MONLIN;
	up->lasthour = (uint8_t)pp->hour;

	/*
	 * Process median filter samples. If none received, declare a
	 * timeout and keep going.
	 */
#ifdef HAVE_PPSAPI
	if (up->pcount == 0) {
		peer->cfg.flags &= ~FLAG_PPS;
		peer->precision = PRECISION;
	}
	if (up->tcount == 0) {
		pp->coderecv = pp->codeproc;
		refclock_report(peer, CEVNT_TIMEOUT);
		return;
	}
	up->pcount = up->tcount = 0;
#else /* HAVE_PPSAPI */
	if (pp->coderecv == pp->codeproc) {
		refclock_report(peer, CEVNT_TIMEOUT);
		return;
	}
#endif /* HAVE_PPSAPI */
	refclock_receive(peer);
	record_clock_stats(peer, pp->a_lastcode);
	DPRINT(1, ("spectracom: timecode %d %s\n", pp->lencode,
		   pp->a_lastcode));
}


/*
 * spectracom_control - fudge parameters have been set or changed
 */
#ifdef HAVE_PPSAPI
static void
spectracom_control(
	int unit,
	const struct refclockstat *in_st,
	struct refclockstat *out_st,
	struct peer *peer
	)
{
	struct spectracomunit *up;
	struct refclockproc *pp;

	UNUSED_ARG(unit);
	UNUSED_ARG(in_st);
	UNUSED_ARG(out_st);

	pp = peer->procptr;
	up = pp->unitptr;

	if (!(pp->sloppyclockflag & CLK_FLAG1)) {
		if (!up->ppsapi_tried)
			return;
		up->ppsapi_tried = 0;
		if (!up->ppsapi_lit)
			return;
		peer->cfg.flags &= ~FLAG_PPS;
		peer->precision = PRECISION;
		time_pps_destroy(up->ppsctl.handle);
		up->ppsctl.handle = 0;
		up->ppsapi_lit = 0;
		return;
	}

	if (up->ppsapi_tried)
		return;
	/*
	 * Light up the PPSAPI interface.
	 */
	up->ppsapi_tried = 1;
	if (refclock_ppsapi(pp->io.fd, &up->ppsctl)) {
		up->ppsapi_lit = 1;
		return;
	}

	msyslog(LOG_WARNING, "REFCLOCK %s flag1 1 but PPSAPI fails",
		refclock_name(peer));
}
#endif	/* HAVE_PPSAPI */

