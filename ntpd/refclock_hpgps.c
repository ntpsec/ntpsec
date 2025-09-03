/*
 * refclock_hpgps - clock driver for HP GPS receivers
 */

#include "config.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_assert.h"
#include "ntp_calendar.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"
#include "timespecops.h"

#include <stdio.h>
#include <ctype.h>

/* Version 0.1 April  1, 1995
 *         0.2 April 25, 1995
 *             tolerant of missing timecode response prompt and sends
 *             clear status if prompt indicates error;
 *             can use either local time or UTC from receiver;
 *             can get receiver status screen via flag4
 *
 * WARNING!: This driver is UNDER CONSTRUCTION
 * Everything in here should be treated with suspicion.
 * If it looks wrong, it probably is.
 *
 * Comments and/or questions to: Dave Vitanye
 *                               Hewlett Packard Company
 *                               dave@scd.hp.com
 *                               (408) 553-2856
 *
 * Thanks to the author of the PST driver, which was the starting point for
 * this one.
 *
 * This driver supports the HP 58503A Time and Frequency Reference Receiver.
 * This receiver uses HP SmartClock (TM) to implement an Enhanced GPS receiver.
 * The receiver accuracy when locked to GPS in normal operation is better
 * than 1 usec. The accuracy when operating in holdover is typically better
 * than 10 usec. per day.
 *
 * The same driver also handles the HP Z3801A which is available surplus
 * from the cell phone industry.  It's popular with hams.
 * It needs a different line setup: 19200 baud, 7 data bits, odd parity
 * That is selected by adding "subtype 1" to the server line in ntp.conf
 * HP Z3801A code from Jeff Mock added by Hal Murray, Sep 2005
 *
 *
 * Initial driver operation: expects the receiver to be already locked
 * to GPS, configured and able to output timecode format 2 messages.
 *
 * The driver uses the poll sequence :PTIME:CODE? to get a response from
 * the receiver. The receiver responds with a timecode string of ASCII
 * printing characters, followed by a <cr><lf>, followed by a prompt string
 * issued by the receiver, in the following format:
 * T#yyyymmddhhmmssMFLRVcc<cr><lf>scpi >
 *
 * The driver processes the response at the <cr> and <lf>, so what the
 * driver sees is the prompt from the previous poll, followed by this
 * timecode. The prompt from the current poll is (usually) left unread until
 * the next poll. So (except on the very first poll) the driver sees this:
 *
 * scpi > T#yyyymmddhhmmssMFLRVcc<cr><lf>
 *
 * The T is the on-time character, at 980 msec. before the next 1PPS edge.
 * The # is the timecode format type. We look for format 2.
 * Without any of the CLK or PPS stuff, then, the receiver buffer timestamp
 * at the <cr> is 24 characters later, which is about 25 msec. at 9600 bps,
 * so the first approximation for fudge time1 is nominally -0.955 seconds.
 * This number probably needs adjusting for each machine / OS type, so far:
 *  -0.955000 on an HP 9000 Model 712/80 HP-UX 9.05
 *  -0.953175 on an HP 9000 Model 370    HP-UX 9.10
 *
 * This receiver also provides a 1PPS signal.
 * Use the ATOM driver (22) to take advantage of it.
 * That requires kernel support.
 *
 * There should always be one request in the pipeline.
 * Whenever a message is received, another one is asked for.
 * Normally, the next time messages arrives a second later.
 * The status page takes 2 seconds.  (even at 19200)
 * The timer routine will recover if a message gets dropped
 * due to an error or an unplugged cable or loss of power or ...
 * If flag4 is set, each polling interval does a dance to get
 * the status page.  The receive routine asks for status
 * rather than time, and the status collection code asks for
 * the time after it has collected the whole status message.
 */

/*
 * Fudge time1 is used to accommodate the timecode serial interface adjustment.
 * Option flag4 can be set to request a receiver status screen summary, which
 * is recorded in the clockstats file.
 */

#define HPDEBUG false

/*
 * Interface definitions
 */
#define	DEVICE		"/dev/hpgps%d" /* device name and unit */
#define	SPEED232	B9600		/* uart speed (9600 baud) */
#define	SPEED232Z	B19200		/* uart speed (19200 baud) */
#define	PRECISION	(-10)		/* precision assumed (about 1 ms) */
#define	REFID		"GPS\0"		/* reference ID */
#define NAME		"HPGPS"		/* shortname */
#define	DESCRIPTION	"HP GPS Time and Frequency Reference Receiver"


/* Size of buffer for status screen from :SYSTEM:STATUS?
 * (This code used to use :SYSTEM:PRINT?
 *  I can't find any doc for that.  Hal, 2025-Aug-19.)
 * The Z3801A manual shows 22 lines.
 * Newer versions of firmware have 23 lines of up to 79 characters.
 * Not all lines were full.    Hal Murray, Nov 2023
 */
#define LMAX		24	/* lines in status screen, plus spare */
#define SMAX            LMAX*80+1 /* characters */

#define MTCODET2        12      /* number of fields in timecode format T2 */
#define NTCODET2        21      /* number of chars to checksum in format T2 */

/*
 * Unit control structure
 */
struct hpgpsunit {
	int	idlesec;	/* seconds since last message */
	bool	didpoll;	/* poll called recently */
	unsigned int     cmndcnt;        /* collecting data */
	int     linecnt;        /* collecting text for status screen */
	char	*lastptr;	/* pointer to receiver response data */
	int	wnro;
	char    statscrn[SMAX]; /* receiver status screen buffer */
/* Statisitics since last poll: */
	int	timecnt;	/* Valid time code messages received */
	int	errorcnt;	/* Errors in received messages */
};

/*
 * Function prototypes
 */
static	bool	hpgps_start	(int, struct peer *);
static	void	hpgps_receive	(struct recvbuf *);
static	void	hpgps_poll	(int, struct peer *);
static	void	hpgps_timer	(int, struct peer *);
static	void	hpgps_write(struct peer *peer, const char *msg);
static	bool	hpgps_receive_T2(struct peer *const peer);

/*
 * Transfer vector
 */
struct	refclock refclock_hpgps = {
	NAME,			/* basename of driver */
	hpgps_start,		/* start up driver */
	NULL,			/* shut down driver in the standard way */
	hpgps_poll,		/* transmit poll message */
	NULL,			/* not used (old hpgps_control) */
	NULL,			/* initialize driver */
	hpgps_timer		/* called once per second */
};

/* commands to get extra info */
const char *commands[] = {
  ":GPS:SAT:TRAC:COUNT?\r",       /* sats being tracked */
  ":ROSC:HOLD:DUR?\r",            /* duration,state 1 if in holdover */
  ":ROSC:HOLD:TUNC:PRED?\r",      /* 1 day holdover estimate */
  ":DIAG:ROSC:EFC:ABS?\r",        /* DAC value */
  ":DIAG:ROSC:EFC:REL?\r"         /* -100 to 100 */
  };
/* Get E-230 if data not available
 * Test case is PTIME:LEAP:GPST? (hex time of next leap)
 *   PTIME:LEAP:ACC?\n:SYST:ERR?
 *     scpi > +18
 *     scpi > +0,"No error"
 *   :PTIME:LEAP:GPST?\n:SYST:ERR?
 *     scpi > E-230> -230,"Data corrupt or stale"
//":PTIME:TINT?\r*CLS\r",         /x error if no sats, no data */
//"DIAG:LIF:COUN?\n",             /x uptime in units of 3 hours */

/*
 * hpgps_start - open the devices and initialize data for processing
 */
bool hpgps_start(int unit, struct peer *peer)
{
	struct hpgpsunit *up;
	struct refclockproc *pp;
	int fd;
	unsigned int ldisc;
	unsigned int speed;
	char device[20];

	snprintf(device, sizeof(device), DEVICE, unit);
	/* refclock_open flushes junk, but a quick restart may leave
	 * a time-request in the pipeline.
	 * waits are evil, but this doesn't get called during normal operations.
	 */
	sleep(2);
	/*
	 * Open serial port. Use CLK line discipline, if available.
	 * Default is HP 58503A, mode arg selects HP Z3801A
	 */
	/* mode parameter to server config line shares ttl slot */
	/* Need mode rather than flag because this was called
	 * before following fudge line was even parsed. */
	ldisc = LDISC_STD;
	speed = SPEED232;
	/* subtype parameter to server config line shares mode slot */
	if (1 == peer->cfg.mode) {
		ldisc |= LDISC_7O1;
		speed = SPEED232Z;
	}
	fd = refclock_open(peer->cfg.path ? peer->cfg.path : device,
			   peer->cfg.baud ? peer->cfg.baud : speed, ldisc);
	if (fd <= 0)
		/* coverity[leaked_handle] */
		return false;
	/*
	 * Allocate and initialize unit structure
	 */
	up = emalloc_zero(sizeof(*up));
	pp = peer->procptr;
	pp->io.clock_recv = hpgps_receive;
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

	*up->statscrn = '\0';
	up->lastptr = up->statscrn;

	up->didpoll = false;
	up->idlesec = 0;
	up->timecnt = 0;
	up->errorcnt = 0;

	/*
	 * Get the identifier string, which is logged but otherwise ignored,
	 */
	up->linecnt = 1;
	hpgps_write(peer, "*IDN?\r");

	return true;
}


/*
 * hpgps_receive - receive data from the serial interface
 */
void hpgps_receive(struct recvbuf *rbufp)
{
	struct peer		* const peer = rbufp->recv_peer;
	struct refclockproc	* const pp   = peer->procptr;
	struct hpgpsunit	* const up   = pp->unitptr;

	l_fp rd_timestamp;

	/*
	 * read the receiver response
	 */
	*pp->a_lastcode = '\0';
	pp->lencode = refclock_gtlin(rbufp, pp->a_lastcode, BMAX, &rd_timestamp);

	DPRINT(1, ("hpgps: lencode: %d timecode:%s\n",
		   pp->lencode, pp->a_lastcode));

if (HPDEBUG) {
  printf("HP in:%3d %s\n", pp->lencode, pp->a_lastcode);
}
	/*
	 * If there's no characters in the reply, we can quit now
	 */
	if (pp->lencode == 0)
	    return;

	/* Strip off leading prompt to cleanup log files. */
	while (1) {
		if (strstr(pp->a_lastcode, "scpi > ") == pp->a_lastcode) {
			pp->lencode -= 7;
			memmove(pp->a_lastcode, pp->a_lastcode+7, pp->lencode+1);
			continue;
		}
		if (pp->a_lastcode[0] == 'E' &&
		    pp->a_lastcode[1] == '-' &&
		    pp->a_lastcode[5] == '>' &&
		    pp->a_lastcode[6] == ' ') {
			/* "E-nnn> " Error code */
			msyslog(LOG_ERR, "HPGPS(%d) error: %s",
				pp->refclkunit, pp->a_lastcode);
			DPRINT(0, ("hpgps: error: %s\n", pp->a_lastcode));
			hpgps_write(peer, "*CLS\r\r");
			pp->lencode -= 7;
			memmove(pp->a_lastcode, pp->a_lastcode+7, pp->lencode+1);
			continue;
		}
		break;
	}

	if (up->cmndcnt > 0) {
		/* some values are 2 part: +1.17000E+002,0
		 * split them here to avoid postprocessing before gnuplot
		 */ 
		char *comma = strchr(pp->a_lastcode, ',');
		if (NULL != comma) {
		  *comma = ' ';
		}
		*up->lastptr++ = ' ';
		memcpy(up->lastptr, pp->a_lastcode, (size_t)pp->lencode);
		up->lastptr += pp->lencode;
		if (up->cmndcnt < COUNTOF(commands)) {
		  hpgps_write(peer, commands[up->cmndcnt++]);
		  return;
		}
		*up->lastptr++ = 0;
if (HPDEBUG) {
  printf("HPlog: %s\n", up->statscrn);
}
		record_clock_stats(peer, up->statscrn);
		up->cmndcnt = 0;
		if ((pp->sloppyclockflag & CLK_FLAG4) ) {
		  up->linecnt = LMAX;
		  hpgps_write(peer, ":SYSTEM:STATUS?\r");
		} else {
		  hpgps_write(peer, ":PTIME:TCODE?\r");
		}
		return;
	}

	/*
	 * If linecnt is greater than zero, we are getting information only,
	 * such as the receiver identification string or the receiver status
	 * screen, so put the receiver response at the end of the status
	 * screen buffer. When we have the last line, write the buffer to
	 * the clockstats file and return without further processing.
	 *
	 * If linecnt is zero, we are expecting a timecode.
	 */


	if (up->linecnt > 0) {
		up->linecnt--;
		/* Silently drop whole line if it doesn't fit. */
		if ((int)(pp->lencode + 2) <= (SMAX - (up->lastptr - up->statscrn))) {
			if ( (up->lastptr != up->statscrn) || (up->linecnt > 0) )
				/* ID string stays on same line */
				*up->lastptr++ = '\n';
			memcpy(up->lastptr, pp->a_lastcode, (size_t)pp->lencode);
			up->lastptr += pp->lencode;
		}
		/* Status screen is 22 or 23 lines */
		if ( (up->linecnt == 0) ||
		     (strstr(pp->a_lastcode, "Self Test:") == pp->a_lastcode) ) {
			up->linecnt = 0;
			record_clock_stats(peer, up->statscrn);
			hpgps_write(peer, ":PTIME:TCODE?\r");
		}
		return;
	}

	pp->lastrec = rd_timestamp;

	up->idlesec = 0;

	if (hpgps_receive_T2(peer)) return;

	if (!up->didpoll) {
		/* error ?? */
		return;
	}

	REQUIRE(up->didpoll);
	up->didpoll = false;
	up->lastptr = up->statscrn;
	*up->lastptr = '\0';
	/* Do FLAG3 first.  End of FLAG3 processing starts FLAG4 */
	if (pp->sloppyclockflag & CLK_FLAG3) {
		up->lastptr += snprintf(up->statscrn, sizeof(up->statscrn),
		   "%s  %d %d ",  pp->a_lastcode, up->timecnt, up->errorcnt);
		up->timecnt = up->errorcnt = 0;
		up->cmndcnt = 0;
		hpgps_write(peer, commands[up->cmndcnt++]);
	} else if (pp->sloppyclockflag & CLK_FLAG4) {
		up->linecnt = LMAX;
		hpgps_write(peer, ":SYSTEM:STATUS?\r");
	} else {
		mprintf_clock_stats(peer,
		   "%s  %d %d",  pp->a_lastcode, up->timecnt, up->errorcnt);
		up->timecnt = up->errorcnt = 0;
		hpgps_write(peer, ":PTIME:TCODE?\r");
	}
}

/* return true if all OK
 * false is error or didpoll
 */
bool hpgps_receive_T2(struct peer *const peer)
{
	struct refclockproc	* const pp   = peer->procptr;
	struct hpgpsunit	* const up   = pp->unitptr;

	l_fp rd_reftime;
	char tcodechar1;        /* identifies timecode format */
	char tcodechar2;        /* identifies timecode format */
	char timequal;          /* time figure of merit: 0-9 */
	char freqqual;          /* frequency figure of merit: 0-3 */
	char leapchar;          /* leapsecond: + or 0 or - */
	char servchar;          /* request for service: 0 = no, 1 = yes */
	char syncchar;          /* time info is invalid: 0 = no, 1 = yes */
	short expectedsm;       /* expected timecode byte checksum */
	short tcodechksm;       /* computed timecode byte checksum */
	int m, n;
	struct tm tm;		/* temp storage for parsed data */
	struct timespec date;	/* time stamp derived from serial port */
	char *tcp;              /* timecode pointer (skips over the prompt) */

	/* We get down to business:
	 * Check for a timecode reply and decode it.
	 * If we don't recognize the reply, or don't get the proper
	 * number of decoded fields, or if the timecode checksum is bad,
	 * then we declare bad format and exit.
	 *
	 * Timecode format (after removing prompt):
	 * T2yyyymmddhhmmssTFLRVcc<cr><lf>
	 *
	 */

	tcp = pp->a_lastcode;
	/* Not expected to happen. Beware of filling up log files. */
	if (*tcp == ' ') {
	    msyslog(LOG_INFO, "HPGPS(%d) Leading space: '%s'",
		pp->refclkunit, pp->a_lastcode);
	}
	while ((*tcp == ' ') || (*tcp == '\t')) tcp++;

	/*
	 * make sure we got a timecode format and
	 * then process accordingly
	 */
	m = sscanf(tcp,"%c%c", &tcodechar1, &tcodechar2);

	if (m != 2){
	        DPRINT(1, ("hpgps: no format indicator\n"));
		refclock_report(peer, CEVNT_BADREPLY);
		up->errorcnt++;
		return(false);
	}

	if ('T' != tcodechar1 || '2' != tcodechar2) {
		DPRINT(1, ("hpgps: unrecognized reply format %c%c\n",
			   tcodechar1, tcodechar2));
		refclock_report(peer, CEVNT_BADREPLY);
		up->errorcnt++;
		return(false);
	}


	m = sscanf(tcp,"%*c%*c%4d%2d%2d%2d%2d%2d%c%c%c%c%c%2hx",
		&tm.tm_year, &tm.tm_mon, &tm.tm_mday,
		&tm.tm_hour, &tm.tm_min, &tm.tm_sec,
		&timequal, &freqqual, &leapchar, &servchar, &syncchar,
		(short unsigned int*)&expectedsm);

	if (m != MTCODET2){
	        DPRINT(1, ("hpgps: only %d fields recognized in timecode\n", m));
		refclock_report(peer, CEVNT_BADREPLY);
		up->errorcnt++;
		return(false);
	}


	/*
	 * Compute and verify the checksum.
	 * Characters are summed starting at tcodechar1, ending at just
	 * before the expected checksum.  Bail out if incorrect.
	 */
	tcodechksm = 0;
	n = NTCODET2;
	while (n-- > 0) {
		tcodechksm += *tcp++;
	}
	tcodechksm &= 0x00ff;

	if (tcodechksm != expectedsm) {
	        DPRINT(1, ("hpgps: checksum %2hX doesn't match %2hX expected\n",
			   tcodechksm, expectedsm));
		refclock_report(peer, CEVNT_BADREPLY);
		up->errorcnt++;
		return(false);
	}

	if (timequal > '4') {
		DPRINT(0, ("hpgps: TFOM %c too big\n", timequal));
		refclock_report(peer, CEVNT_BADREPLY);
		up->errorcnt++;
		return(false);
	}


	/*
	 * Decode the TFLRV indicators.
	 * NEED TO FIGURE OUT how to deal with the request for service,
	 * time quality, and frequency quality indicators some day.
	 */
	if (syncchar != '0') {
		pp->leap = LEAP_NOTINSYNC;
	}
	else {
		pp->leap = LEAP_NOWARNING;
		switch (leapchar) {

		    case '0':
			break;

		    /* See http://bugs.ntp.org/1090
		     * Ignore leap announcements unless June or December.
		     * Better would be to use :GPSTime? to find the month,
		     * but that seems too likely to introduce other bugs.
		     */
		    case '+':
			if ((tm.tm_mon==6) || (tm.tm_mon==12))
			    pp->leap = LEAP_ADDSECOND;
			break;

		    case '-':
			if ((tm.tm_mon==6) || (tm.tm_mon==12))
			    pp->leap = LEAP_DELSECOND;
			break;

		    default:
			DPRINT(1, ("hpgps: unrecognized leap indicator: %c\n",
				   leapchar));
			refclock_report(peer, CEVNT_BADTIME);
			up->errorcnt++;
			return(false);
		} /* end of leapchar switch */
	}

	/*
	 * Process the new sample in the median filter and determine the
	 * reference clock offset and dispersion. We use lastrec as both
	 * the reference time and receive time in order to avoid being
	 * cute, like setting the reference time later than the receive
	 * time, which may cause a paranoid protocol module to chuck out
	 * the data.
	 */
	tm.tm_year -= 1900;
	tm.tm_mon -= 1;
	tm.tm_isdst = 0;	/* Coverity CID 584869, UNINIT */
	date.tv_nsec = 0;
	date.tv_sec = timegm(&tm);	/* No error checking */

	/* Z3801A broke 2025-Aug-17 => 2006-Jam-01 */
	fix_WNRO(&date, &up->wnro, peer);

	rd_reftime = tspec_stamp_to_lfp(date);
	refclock_process_offset(pp, rd_reftime, pp->lastrec, pp->fudgetime1);
	up->timecnt++;

	if(up->didpoll) {
		return(false);
	}

	hpgps_write(peer, ":PTIME:TCODE?\r");
	return(true);
}


/*
 * hpgps_poll - called by the transmit procedure
 */
void hpgps_poll(int unit, struct peer *peer)
{
	UNUSED_ARG(unit);
	struct refclockproc *pp;
	struct hpgpsunit *up;

	pp = peer->procptr;
	up = (struct hpgpsunit *)pp->unitptr;

	pp->lastref = pp->lastrec;
	refclock_receive(peer);

	up->didpoll = true;

	pp->polls++;
}
 
/*
 * hpgps_timer - called once per second
 */
static void hpgps_timer(int unit, struct peer *peer)
{
	struct refclockproc *pp;
	struct hpgpsunit *up;

	UNUSED_ARG(unit);

	pp = peer->procptr;
	up = pp->unitptr;

	if (up->idlesec++ == 5)
	    refclock_report(peer, CEVNT_TIMEOUT);
	if (up->idlesec >= 5) {
		/* FIXME: logging (happens on some commands) */
		/* Timeout.  Poke it again.
		 * This recovers from the cable being unplugged for a while.
		 */
		hpgps_write(peer, ":PTIME:TCODE?\r");
		up->cmndcnt = 0;
		up->linecnt = 0;
	}
}


static void hpgps_write(struct peer *peer, const char *msg) {
	struct refclockproc *pp = peer->procptr;
	int len = strlen(msg);
if (HPDEBUG) {
  static int counter = 0;
  char copy[64];  /* msg ends with \r */
  strlcpy(copy, msg, sizeof(copy));
  *strstr(copy, "\r") = 0;
  printf("HPout: %d %s\n", counter++, copy);
}
	if (write(pp->io.fd, msg, len) != len)
		refclock_report(peer, CEVNT_FAULT);
}

