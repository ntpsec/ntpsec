/*
 * Copyright (c) 1997, 1998, 2003
 *	The Regents of the University of California.  All rights reserved.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-4-clause
 */

#include "config.h"
#include "ntp.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"
#include "ntp_calendar.h"

#include <stdio.h>
#include <ctype.h>

/*
 * Rockwell Jupiter GPS receiver definitions
 *
 * This is all based on the "Zodiac GPS Receiver Family Designer's
 * Guide" (dated 12/96)
 */

#define JUPITER_SYNC		0x81ff	/* sync word (book says 0xff81 !?!?) */
#define JUPITER_ALL		0xffff	/* disable all output messages */

/* Output messages (sent by the Jupiter board) */
#define JUPITER_O_GPOS		1000	/* geodetic position status */
#define JUPITER_O_EPOS		1001	/* ECEF position status */
#define JUPITER_O_CHAN		1002	/* channel summary */
#define JUPITER_O_VIS		1003	/* visible satellites */
#define JUPITER_O_DGPS		1005	/* differential GPS status */
#define JUPITER_O_MEAS		1007	/* channel measurement */
#define JUPITER_O_ID		1011	/* receiver id */
#define JUPITER_O_USER		1012	/* user-settings output */
#define JUPITER_O_TEST		1100	/* built-in test results */
#define JUPITER_O_MARK		1102	/* measurement time mark */
#define JUPITER_O_PULSE		1108	/* UTC time mark pulse output */
#define JUPITER_O_PORT		1130	/* serial port com parameters in use */
#define JUPITER_O_EUP		1135	/* EEPROM update */
#define JUPITER_O_ESTAT		1136	/* EEPROM status */

/* Input messages (sent to the Jupiter board) */
#define JUPITER_I_PVTINIT	1200	/* geodetic position and velocity */
#define JUPITER_I_USER		1210	/* user-defined datum */
#define JUPITER_I_MAPSEL	1211	/* map datum select */
#define JUPITER_I_ELEV		1212	/* satellite elevation mask control */
#define JUPITER_I_CAND		1213	/* satellite candidate select */
#define JUPITER_I_DGPS		1214	/* differential GPS control */
#define JUPITER_I_COLD		1216	/* cold start control */
#define JUPITER_I_VALID		1217	/* solution validity criteria */
#define JUPITER_I_ALT		1219	/* user-entered altitude input */
#define JUPITER_I_PLAT		1220	/* application platform control */
#define JUPITER_I_NAV		1221	/* nav configuration */
#define JUPITER_I_TEST		1300	/* perform built-in test command */
#define JUPITER_I_RESTART	1303	/* restart command */
#define JUPITER_I_PORT		1330	/* serial port com parameters */
#define JUPITER_I_PROTO		1331	/* message protocol control */
#define JUPITER_I_RDGPS		1351	/* raw DGPS RTCM SC-104 data */

struct jheader {
	u_short sync;		/* (JUPITER_SYNC) */
	u_short id;		/* message id */
	u_short len;		/* number of data short wordss (w/o cksum) */
	uint8_t reqid;		/* JUPITER_REQID_MASK bits available as id */
	uint8_t flags;		/* flags */
	u_short hsum;		/* header cksum */
};

#define JUPITER_REQID_MASK	0x3f	/* bits available as id */
#define JUPITER_FLAG_NAK	0x01	/* negative acknowledgement */
#define JUPITER_FLAG_ACK	0x02	/* acknowledgement */
#define JUPITER_FLAG_REQUEST	0x04	/* request ACK or NAK */
#define JUPITER_FLAG_QUERY	0x08	/* request one shot output message */
#define JUPITER_FLAG_LOG	0x20	/* request periodic output message */
#define JUPITER_FLAG_CONN	0x40	/* enable periodic message */
#define JUPITER_FLAG_DISC	0x80	/* disable periodic message */

#define JUPITER_H_FLAG_BITS \
    "\020\1NAK\2ACK\3REQUEST\4QUERY\5MBZ\6LOG\7CONN\10DISC"

/* Log request messages (data payload when using JUPITER_FLAG_LOG) */
struct jrequest {
	u_short trigger;		/* if 0, trigger on time trigger on
					   update (e.g. new almanac) */
	u_short interval;		/* frequency in seconds */
	u_short offset;			/* offset into minute */
	u_short dsum;			/* checksum */
};

/* JUPITER_O_GPOS (1000) */
struct jgpos {
	u_short stime[2];		/* set time (10 ms ticks) */
	u_short seq;			/* sequence number */
	u_short sseq;			/* sat measurement sequence number */
	u_short navval;			/* navigation soltuion validity */
	u_short navtype;		/* navigation solution type */
	u_short nmeas;			/* # of measurements used in solution */
	u_short polar;			/* if 1 then polar navigation */
	u_short gweek;			/* GPS week number */
	u_short sweek[2];		/* GPS seconds into week */
	u_short nsweek[2];		/* GPS nanoseconds into second */
	u_short utcday;			/* 1 to 31 */
	u_short utcmon;			/* 1 to 12 */
	u_short utcyear;		/* 1980 to 2079 */
	u_short utchour;		/* 0 to 23 */
	u_short utcmin;			/* 0 to 59 */
	u_short utcsec;			/* 0 to 59 */
	u_short utcnsec[2];		/* 0 to 999999999 */
	u_short lat[2];			/* latitude (radians) */
	u_short lon[2];			/* longitude (radians) */
	u_short height[2];		/* height (meters) */
	u_short gsep;			/* geoidal separation */
	u_short speed[2];		/* ground speed (meters/sec) */
	u_short course;			/* true course (radians) */
	u_short mvar;
	u_short climb;
	u_short mapd;
	u_short herr[2];
	u_short verr[2];
	u_short terr[2];
	u_short hverr;
	u_short bias[2];
	u_short biassd[2];
	u_short drift[2];
	u_short driftsd[2];
	u_short dsum;			/* checksum */
};
#define JUPITER_O_GPOS_NAV_NOALT	0x01	/* altitude used */
#define JUPITER_O_GPOS_NAV_NODGPS	0x02	/* no differential GPS */
#define JUPITER_O_GPOS_NAV_NOSAT	0x04	/* not enough satellites */
#define JUPITER_O_GPOS_NAV_MAXH		0x08	/* exceeded max EHPE */
#define JUPITER_O_GPOS_NAV_MAXV		0x10	/* exceeded max EVPE */

/* JUPITER_O_CHAN (1002) */
struct jchan {
	u_short stime[2];		/* set time (10 ms ticks) */
	u_short seq;			/* sequence number */
	u_short sseq;			/* sat measurement sequence number */
	u_short gweek;			/* GPS week number */
	u_short sweek[2];		/* GPS seconds into week */
	u_short gpsns[2];		/* GPS nanoseconds from epoch */
	struct jchan2 {
		u_short flags;		/* flags */
		u_short prn;		/* satellite PRN */
		u_short chan;		/* channel number */
	} sat[12];
	u_short dsum;
};

/* JUPITER_O_VIS (1003) */
struct jvis {
	u_short stime[2];		/* set time (10 ms ticks) */
	u_short seq;			/* sequence number */
	u_short gdop;			/* best possible GDOP */
	u_short pdop;			/* best possible PDOP */
	u_short hdop;			/* best possible HDOP */
	u_short vdop;			/* best possible VDOP */
	u_short tdop;			/* best possible TDOP */
	u_short nvis;			/* number of visible satellites */
	struct jvis2 {
		u_short prn;		/* satellite PRN */
		u_short azi;		/* satellite azimuth (radians) */
		u_short elev;		/* satellite elevation (radians) */
	} sat[12];
	u_short dsum;			/* checksum */
};

/* JUPITER_O_ID (1011) */
struct jid {
	u_short stime[2];		/* set time (10 ms ticks) */
	u_short seq;			/* sequence number */
	char chans[20];			/* number of channels (ascii) */
	char vers[20];			/* software version (ascii) */
	char date[20];			/* software date (ascii) */
	char opts[20];			/* software options (ascii) */
	char reserved[20];
	u_short dsum;			/* checksum */
};

/* JUPITER_O_USER (1012) */
struct juser {
	u_short stime[2];		/* set time (10 ms ticks) */
	u_short seq;			/* sequence number */
	u_short status;			/* operatinoal status */
	u_short coldtmo;		/* cold start time-out */
	u_short dgpstmo;		/* DGPS correction time-out*/
	u_short emask;			/* elevation mask */
	u_short selcand[2];		/* selected candidate */
	u_short solflags;		/* solution validity criteria */
	u_short nsat;			/* number of satellites in track */
	u_short herr[2];		/* minimum expected horizontal error */
	u_short verr[2];		/* minimum expected vertical error */
	u_short platform;		/* application platform */
	u_short dsum;			/* checksum */
};

/* JUPITER_O_PULSE (1108) */
struct jpulse {
	u_short stime[2];		/* set time (10 ms ticks) */
	u_short seq;			/* sequence number */
	u_short reserved[5];
	u_short sweek[2];		/* GPS seconds into week */
	short offs;			/* GPS to UTC time offset (seconds) */
	u_short offns[2];		/* GPS to UTC offset (nanoseconds) */
	u_short flags;			/* flags */
	u_short dsum;			/* checksum */
};
#define JUPITER_O_PULSE_VALID		0x1	/* time mark validity */
#define JUPITER_O_PULSE_UTC		0x2	/* GPS/UTC sync */

/* JUPITER_O_EUP (1135) */
struct jeup {
	u_short stime[2];		/* set time (10 ms ticks) */
	u_short seq;			/* sequence number */
	uint8_t dataid;			/* data id */
	uint8_t prn;			/* satellite PRN */
	u_short dsum;			/* checksum */
};

/* JUPITER_I_RESTART (1303) */
struct jrestart {
	u_short seq;			/* sequence number */
	u_short flags;
	u_short dsum;			/* checksum */
};
#define JUPITER_I_RESTART_INVRAM	0x01
#define JUPITER_I_RESTART_INVEEPROM	0x02
#define JUPITER_I_RESTART_INVRTC	0x04
#define JUPITER_I_RESTART_COLD		0x80

/* JUPITER_I_PVTINIT (1200) */
struct jpvtinit {
	u_short flags;
	u_short gweek;			/* GPS week number */
	u_short sweek[2];		/* GPS seconds into week */
	u_short utcday;			/* 1 to 31 */
	u_short utcmon;			/* 1 to 12 */
	u_short utcyear;		/* 1980 to 2079 */
	u_short utchour;		/* 0 to 23 */
	u_short utcmin;			/* 0 to 59 */
	u_short utcsec;			/* 0 to 59 */
	u_short lat[2];			/* latitude (radians) */
	u_short lon[2];			/* longitude (radians) */
	u_short height[2];		/* height (meters) */
	u_short speed[2];		/* ground speed (meters/sec) */
	u_short course;			/* true course (radians) */
	u_short climb;
	u_short dsum;
};
#define JUPITER_I_PVTINIT_FORCE		0x01
#define JUPITER_I_PVTINIT_GPSVAL	0x02
#define JUPITER_I_PVTINIT_UTCVAL	0x04
#define JUPITER_I_PVTINIT_POSVAL	0x08
#define JUPITER_I_PVTINIT_ALTVAL	0x10
#define JUPITER_I_PVTINIT_SPDVAL	0x12
#define JUPITER_I_PVTINIT_MAGVAL	0x14
#define JUPITER_I_PVTINIT_CLIMBVAL	0x18

/* JUPITER_I_PLAT (1220) */
struct jplat {
	u_short seq;			/* sequence number */
	u_short platform;		/* application platform */
	u_short dsum;
};
#define JUPITER_I_PLAT_DEFAULT		0	/* default dynamics */
#define JUPITER_I_PLAT_LOW		2	/* pedestrian */
#define JUPITER_I_PLAT_MED		5	/* land (e.g. automobile) */
#define JUPITER_I_PLAT_HIGH		6	/* air */


#ifdef HAVE_PPSAPI
# include "ppsapi_timepps.h"
#endif

#ifdef WORDS_BIGENDIAN
#define getshort(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))
#define putshort(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))
#else
#define getshort(s) ((u_short)(s))
#define putshort(s) ((u_short)(s))
#endif

/*
 * This driver supports the Rockwell Jupiter GPS engine.
 * It requires the PPS API support.
 */

/*
 * GPS Definitions
 */
#define	DEVICE		"/dev/gps%d"	/* device name and unit */
#define	SPEED232	B9600		/* baud */

/*
 * Radio interface parameters
 */
#define	PRECISION	(-18)		/* precision assumed (about 4 us) */
#define	REFID		"GPS\0"		/* reference id */
#define	NAME		"JUPITER"	/* shortname */
#define	DESCRIPTION	"Rockwell Jupiter GPS Receiver" /* who we are */
#define	DEFFUDGETIME	0		/* default fudge time (ms) */

/* Unix timestamp for the GPS epoch: January 6, 1980 */
#define GPS_EPOCH 315964800

/* Double short to unsigned int */
#define DS2UI(p) ((getshort((p)[1]) << 16) | getshort((p)[0]))

/* Double short to signed int */
#define DS2I(p) ((getshort((p)[1]) << 16) | getshort((p)[0]))

/* One week's worth of seconds */
#define WEEKSECS (7 * 24 * 60 * 60)

/*
 * Jupiter unit control structure.
 */
struct instance {
	struct peer *peer;		/* peer */
	u_int  pollcnt;			/* poll message counter */
	bool  polled;			/* Hand in a time sample? */
#ifdef HAVE_PPSAPI
	pps_params_t pps_params;	/* pps parameters */
	pps_info_t pps_info;		/* last pps data */
	pps_handle_t pps_handle;	/* pps handle */
	bool assert;			/* pps edge to use */
	bool hardpps;			/* enable kernel mode */
	struct timespec ts;		/* last timestamp */
#endif
	l_fp limit;
	u_int gpos_gweek;		/* Current GPOS GPS week number */
	u_int gpos_sweek;		/* Current GPOS GPS seconds into week */
	u_int gweek;			/* current GPS week number */
	uint32_t lastsweek;		/* last seconds into GPS week */
	time_t timecode;		/* current ntp timecode */
	uint32_t stime;			/* used to detect firmware bug */
	bool wantid;			/* don't reconfig on channel id msg */
	u_int  moving;			/* mobile platform? */
	uint8_t sloppyclockflag;	/* driver options */
	u_short sbuf[512];		/* local input buffer */
	int ssize;			/* space used in sbuf */
};

/*
 * Function prototypes
 */
static	void	jupiter_canmsg	(struct instance *, u_int);
static	u_short	jupiter_cksum	(u_short *, u_int);
static	bool	jupiter_config	(struct instance *);
static	void	jupiter_debug	(struct peer *, const char *,
				 const char *, ...)
			__attribute__ ((format (printf, 3, 4)));
static	const char *	jupiter_parse_t	(struct instance *, u_short *);
static	const char *	jupiter_parse_gpos	(struct instance *, u_short *);
static	void	jupiter_platform	(struct instance *, u_int);
static	void	jupiter_poll	(int, struct peer *);
static	void	jupiter_control	(int, const struct refclockstat *,
				 struct refclockstat *, struct peer *);
#ifdef HAVE_PPSAPI
static	bool	jupiter_ppsapi	(struct instance *);
static	bool	jupiter_pps	(struct instance *);
#endif /* HAVE_PPSAPI */
static	int	jupiter_recv	(struct instance *);
static	void	jupiter_receive (struct recvbuf *rbufp);
static	void	jupiter_reqmsg	(struct instance *, u_int, u_int);
static	void	jupiter_reqonemsg(struct instance *, u_int);
static	char *	jupiter_send	(struct instance *, struct jheader *);
static	void	jupiter_shutdown(int, struct peer *);
static	bool	jupiter_start	(int, struct peer *);

/*
 * Transfer vector
 */
struct	refclock refclock_jupiter = {
	NAME,			/* basename of driver */
	jupiter_start,		/* start up driver */
	jupiter_shutdown,	/* shut down driver */
	jupiter_poll,		/* transmit poll message */
	jupiter_control,	/* (clock control) */
	NULL,			/* (clock init) */
	NULL			/* timer - not used */
};

/*
 * jupiter_start - open the devices and initialize data for processing
 */
static bool
jupiter_start(
	int unit,
	struct peer *peer
	)
{
	struct refclockproc *pp;
	struct instance *instance;
	int fd;
	char gpsdev[20];

	/*
	 * Open serial port
	 */
	snprintf(gpsdev, sizeof(gpsdev), DEVICE, unit);
	fd = refclock_open(peer->path ? peer->path : gpsdev,
			   peer->baud ? peer->baud : SPEED232,
			   LDISC_RAW);
	if (fd <= 0) {
		jupiter_debug(peer, "jupiter_start", "open %s: %m",
			      gpsdev);
		/* coverity[leaked_handle] */
		return false;
	}

	/* Allocate unit structure */
	instance = emalloc_zero(sizeof(*instance));
	instance->peer = peer;
	pp = peer->procptr;
	pp->io.clock_recv = jupiter_receive;
	pp->io.srcclock = peer;
	pp->io.datalen = 0;
	pp->io.fd = fd;
	if (!io_addclock(&pp->io)) {
		close(fd);
		pp->io.fd = -1;
		free(instance);
		return false;
	}
	pp->unitptr = instance;

	/*
	 * Initialize miscellaneous variables
	 */
	peer->precision = PRECISION;
	pp->clockname = NAME;
	pp->clockdesc = DESCRIPTION;
	memcpy((char *)&pp->refid, REFID, REFIDLEN);
	peer->sstclktype = CTL_SST_TS_UHF;

#ifdef HAVE_PPSAPI
	instance->assert = true;
	instance->hardpps = false;
	/*
	 * Start the PPSAPI interface if it is there. Default to use
	 * the assert edge and do not enable the kernel hardpps.
	 */
	if (time_pps_create(fd, &instance->pps_handle) < 0) {
		instance->pps_handle = 0;
		msyslog(LOG_ERR,
			"refclock_jupiter: time_pps_create failed: %m");
	}
	else if (!jupiter_ppsapi(instance))
		goto clean_up;
#endif /* HAVE_PPSAPI */

	/* Ensure the receiver is properly configured */
	if (!jupiter_config(instance))
		goto clean_up;

	return true;

clean_up:
	jupiter_shutdown(unit, peer);
	pp->unitptr = 0;
	return false;
}

/*
 * jupiter_shutdown - shut down the clock
 */
static void
jupiter_shutdown(int unit, struct peer *peer)
{
	struct instance *instance;
	struct refclockproc *pp;

	UNUSED_ARG(unit);

	pp = peer->procptr;
	instance = pp->unitptr;
	if (!instance)
		return;

#ifdef HAVE_PPSAPI
	if (instance->pps_handle) {
		time_pps_destroy(instance->pps_handle);
		instance->pps_handle = 0;
	}
#endif /* HAVE_PPSAPI */

	if (pp->io.fd != -1)
		io_closeclock(&pp->io);
	free(instance);
}

/*
 * jupiter_config - Configure the receiver
 */
static bool
jupiter_config(struct instance *instance)
{
	jupiter_debug(instance->peer, __func__, "init receiver");

	/*
	 * Initialize the unit variables
	 */
	instance->sloppyclockflag = instance->peer->procptr->sloppyclockflag;
	instance->moving = !!(instance->sloppyclockflag & CLK_FLAG2);
	if (instance->moving)
		jupiter_debug(instance->peer, __func__, "mobile platform");

	instance->pollcnt     = 2;
	instance->polled      = false;
	instance->gpos_gweek = 0;
	instance->gpos_sweek = 0;
	instance->gweek = 0;
	instance->lastsweek = 2 * WEEKSECS;
	instance->timecode = 0;
	instance->stime = 0;
	instance->ssize = 0;

	/* Stop outputting all messages */
	jupiter_canmsg(instance, JUPITER_ALL);

	/* Request the receiver id so we can syslog the firmware version */
	jupiter_reqonemsg(instance, JUPITER_O_ID);

	/* Flag that this the id was requested (so we don't get called again) */
	instance->wantid = true;

	/* Request perodic time mark pulse messages */
	jupiter_reqmsg(instance, JUPITER_O_PULSE, 1);

	/* Request perodic geodetic position status */
	jupiter_reqmsg(instance, JUPITER_O_GPOS, 1);

	/* Set application platform type */
	if (instance->moving)
		jupiter_platform(instance, JUPITER_I_PLAT_MED);
	else
		jupiter_platform(instance, JUPITER_I_PLAT_LOW);

	return true;
}

#ifdef HAVE_PPSAPI
/*
 * Initialize PPSAPI
 */
bool
jupiter_ppsapi(
	struct instance *instance	/* unit structure pointer */
	)
{
	int capability;

	if (time_pps_getcap(instance->pps_handle, &capability) < 0) {
		msyslog(LOG_ERR,
		    "refclock_jupiter: time_pps_getcap failed: %m");
		return false;
	}
	memset(&instance->pps_params, 0, sizeof(pps_params_t));
	if (!instance->assert)
		instance->pps_params.mode = capability & PPS_CAPTURECLEAR;
	else
		instance->pps_params.mode = capability & PPS_CAPTUREASSERT;
	if (!(instance->pps_params.mode & (PPS_CAPTUREASSERT | PPS_CAPTURECLEAR))) {
		msyslog(LOG_ERR,
		    "refclock_jupiter: invalid capture edge %d",
		    instance->assert);
		return false;
	}
	instance->pps_params.mode |= PPS_TSFMT_TSPEC;
	if (time_pps_setparams(instance->pps_handle, &instance->pps_params) < 0) {
		msyslog(LOG_ERR,
		    "refclock_jupiter: time_pps_setparams failed: %m");
		return false;
	}
	if (instance->hardpps) {
		if (time_pps_kcbind(instance->pps_handle, PPS_KC_HARDPPS,
				    instance->pps_params.mode & ~PPS_TSFMT_TSPEC,
				    PPS_TSFMT_TSPEC) < 0) {
			msyslog(LOG_ERR,
			    "refclock_jupiter: time_pps_kcbind failed: %m");
			return false;
		}
		hardpps_enable = true;
	}
/*	instance->peer->precision = PPS_PRECISION; */

#if DEBUG
	if (debug) {
		time_pps_getparams(instance->pps_handle, &instance->pps_params);
		jupiter_debug(instance->peer, __func__,
			"pps capability 0x%x version %d mode 0x%x kern %d",
			capability, instance->pps_params.api_version,
			instance->pps_params.mode, instance->hardpps);
	}
#endif

	return true;
}

/*
 * Get PPSAPI timestamps.
 *
 * Return false on failure and true on success.
 */
static bool
jupiter_pps(struct instance *instance)
{
	pps_info_t pps_info;
	struct timespec timeout, ts;
	double dtemp;
	l_fp tstmp = 0;

	/*
	 * Convert the timespec nanoseconds field to ntp l_fp units.
	 */ 
	if (instance->pps_handle == 0)
		return true;
	timeout.tv_sec = 0;
	timeout.tv_nsec = 0;
	memcpy(&pps_info, &instance->pps_info, sizeof(pps_info_t));
	if (time_pps_fetch(instance->pps_handle, PPS_TSFMT_TSPEC, &instance->pps_info,
	    &timeout) < 0)
		return true;
	if (instance->pps_params.mode & PPS_CAPTUREASSERT) {
		if (pps_info.assert_sequence ==
		    instance->pps_info.assert_sequence)
			return true;
		ts = instance->pps_info.assert_timestamp;
	} else if (instance->pps_params.mode & PPS_CAPTURECLEAR) {
		if (pps_info.clear_sequence ==
		    instance->pps_info.clear_sequence)
			return true;
		ts = instance->pps_info.clear_timestamp;
	} else {
		return true;
	}
	if ((instance->ts.tv_sec == ts.tv_sec) && (instance->ts.tv_nsec == ts.tv_nsec))
		return true;
	instance->ts = ts;

	setlfpuint(tstmp, (uint32_t)ts.tv_sec + JAN_1970);
	dtemp = ts.tv_nsec * FRAC / 1e9;
	setlfpfrac(tstmp, (uint32_t)dtemp);
	instance->peer->procptr->lastrec = tstmp;
	return false;
}
#endif /* HAVE_PPSAPI */

/*
 * jupiter_poll - jupiter watchdog routine
 */
static void
jupiter_poll(int unit, struct peer *peer)
{
	struct instance *instance;
	struct refclockproc *pp;

	UNUSED_ARG(unit);

	pp = peer->procptr;
	instance = pp->unitptr;

	/*
	 * You don't need to poll this clock.  It puts out timecodes
	 * once per second.  If asked for a timestamp, take note.
	 * The next time a timecode comes in, it will be fed back.
	 */

	/*
	 * If we haven't had a response in a while, reset the receiver.
	 */
	if (instance->pollcnt > 0) {
		instance->pollcnt--;
	} else {
		refclock_report(peer, CEVNT_TIMEOUT);

		/* Request the receiver id to trigger a reconfig */
		jupiter_reqonemsg(instance, JUPITER_O_ID);
		instance->wantid = false;
	}

	/*
	 * polled every 64 seconds. Ask jupiter_receive to hand in
	 * a timestamp.
	 */
	instance->polled = true;
	pp->polls++;
}

/*
 * jupiter_control - fudge and option control
 */
static void
jupiter_control(
	int unit,		/* unit (not used) */
	const struct refclockstat *in, /* input parameters (not used) */
	struct refclockstat *out, /* output parameters (not used) */
	struct peer *peer	/* peer structure pointer */
	)
{
	struct refclockproc *pp;
	struct instance *instance;
	uint8_t sloppyclockflag;

	UNUSED_ARG(unit);
	UNUSED_ARG(in);
	UNUSED_ARG(out);

	pp = peer->procptr;
	instance = pp->unitptr;

	instance->limit = dtolfp(pp->fudgetime2);
	/* Force positive value. */
	if (L_ISNEG(instance->limit))
		L_NEG(instance->limit);

#ifdef HAVE_PPSAPI
	instance->assert = !(pp->sloppyclockflag & CLK_FLAG3);
	jupiter_ppsapi(instance);
#endif /* HAVE_PPSAPI */

	sloppyclockflag = instance->sloppyclockflag;
	instance->sloppyclockflag = pp->sloppyclockflag;
	if ((instance->sloppyclockflag & CLK_FLAG2) !=
	    (sloppyclockflag & CLK_FLAG2)) {
		jupiter_debug(peer, __func__,
		    "mode switch: reset receiver");
		jupiter_config(instance);
		return;
	}
}

/*
 * jupiter_receive - receive gps data
 * Gag me!
 */
static void
jupiter_receive(struct recvbuf *rbufp)
{
	size_t bpcnt;
	int cc, size;
	bool ppsret;
	time_t last_timecode;
	uint32_t laststime;
	const char *cp;
	uint8_t *bp;
	u_short *sp;
	struct jid *ip;
	struct jheader *hp;
	struct peer *peer;
	struct refclockproc *pp;
	struct instance *instance;
	l_fp tstamp;

	/* Initialize pointers and read the timecode and timestamp */
	peer = rbufp->recv_peer;
	pp = peer->procptr;
	instance = pp->unitptr;

	bp = (uint8_t *)rbufp->recv_buffer;
	bpcnt = rbufp->recv_length;

	/* This shouldn't happen */
	if (bpcnt > sizeof(instance->sbuf) - instance->ssize)
		bpcnt = sizeof(instance->sbuf) - instance->ssize;

	/* Append to input buffer */
	memcpy((uint8_t *)instance->sbuf + instance->ssize, bp, bpcnt);
	instance->ssize += bpcnt;

	/* While there's at least a header and we parse an intact message */
	while (instance->ssize > (int)sizeof(*hp) && (cc = jupiter_recv(instance)) > 0) {
		instance->pollcnt = 2;

		tstamp = rbufp->recv_time;
		hp = (struct jheader *)instance->sbuf;
		sp = (u_short *)(hp + 1);
		size = cc - sizeof(*hp);
		switch (getshort(hp->id)) {

		case JUPITER_O_PULSE:
			if (size != sizeof(struct jpulse)) {
				jupiter_debug(peer, __func__,
				    "pulse: len %d != %u",
				    size, (int)sizeof(struct jpulse));
				refclock_report(peer, CEVNT_BADREPLY);
				break;
			}

			/*
			 * There appears to be a firmware bug related
			 * to the pulse message; in addition to the one
			 * per second messages, we get an extra pulse
			 * message once an hour (on the anniversary of
			 * the cold start). It seems to come 200 ms
			 * after the one requested. So if we've seen a
			 * pulse message in the last 210 ms, we skip
			 * this one.
			 */
			laststime = instance->stime;
			instance->stime = DS2UI(((struct jpulse *)sp)->stime);
			if (laststime != 0 && instance->stime - laststime <= 21) {
				jupiter_debug(peer, __func__,
				"avoided firmware bug (stime %.2f, laststime %.2f)",
				(double)instance->stime * 0.01, (double)laststime * 0.01);
				break;
			}

			/* Retrieve pps timestamp */
			ppsret = jupiter_pps(instance);

			/*
			 * Add one second if msg received early
			 * (i.e. before limit, a.k.a. fudgetime2) in
			 * the second.
			 */
			if (tstamp - pp->lastrec < instance->limit)
				bumplfpuint(pp->lastrec, 1);

			/* Parse timecode (even when there's no pps) */
			last_timecode = instance->timecode;
			if ((cp = jupiter_parse_t(instance, sp)) != NULL) {
				jupiter_debug(peer, __func__,
				    "pulse: %s", cp);
				break;
			}

			/* Bail if we didn't get a pps timestamp */
			if (ppsret)
				break;

			/* Bail if we don't have the last timecode yet */
			if (last_timecode == 0)
				break;

			/* Add the new sample to a median filter */
			tstamp = lfpinit(JAN_1970 + (uint32_t)last_timecode, 0);

			refclock_process_offset(pp, tstamp, pp->lastrec, pp->fudgetime1);

			/*
			 * The clock will blurt a timecode every second
			 * but we only want one when polled.  If we
			 * havn't been polled, bail out.
			 */
			if (!instance->polled)
				break;
			instance->polled = false;

			/*
			 * It's a live one!  Remember this time.
			 */

			pp->lastref = pp->lastrec;
			refclock_receive(peer);

			/*
			 * If we get here - what we got from the clock is
			 * OK, so say so
			 */
			refclock_report(peer, CEVNT_NOMINAL);

			/*
			 * We have succeeded in answering the poll.
			 * Turn off the flag and return
			 */
			instance->polled = false;
			break;

		case JUPITER_O_GPOS:
			if (size != sizeof(struct jgpos)) {
				jupiter_debug(peer, __func__,
				    "gpos: len %d != %u",
				    size, (int)sizeof(struct jgpos));
				refclock_report(peer, CEVNT_BADREPLY);
				break;
			}

			if ((cp = jupiter_parse_gpos(instance, sp)) != NULL) {
				jupiter_debug(peer, __func__,
				    "gpos: %s", cp);
				break;
			}
			break;

		case JUPITER_O_ID:
			if (size != sizeof(struct jid)) {
				jupiter_debug(peer, __func__,
				    "id: len %d != %u",
				    size, (int)sizeof(struct jid));
				refclock_report(peer, CEVNT_BADREPLY);
				break;
			}
			/*
			 * If we got this message because the Jupiter
			 * just powered instance, it needs to be reconfigured.
			 */
			ip = (struct jid *)sp;
			jupiter_debug(peer, __func__,
			    "%s chan ver %s, %s (%s)",
			    ip->chans, ip->vers, ip->date, ip->opts);
			msyslog(LOG_DEBUG,
			    "jupiter_receive: %s chan ver %s, %s (%s)",
			    ip->chans, ip->vers, ip->date, ip->opts);
			if (instance->wantid)
				instance->wantid = false;
			else {
				jupiter_debug(peer, __func__, "reset receiver");
				jupiter_config(instance);
				/*
				 * Restore since jupiter_config() just
				 * zeroed it
				 */
				instance->ssize = cc;
			}
			break;

		default:
			jupiter_debug(peer, __func__, "unknown message id %d",
			    getshort(hp->id));
			break;
		}
		instance->ssize -= cc;
		if (instance->ssize < 0) {
			fprintf(stderr, "jupiter_recv: negative ssize!\n");
			abort();
		} else if (instance->ssize > 0)
			memcpy(instance->sbuf, (uint8_t *)instance->sbuf + cc, instance->ssize);
	}
}

static const char *
jupiter_parse_t(struct instance *instance, u_short *sp)
{
	struct tm *tm, tmbuf;
	char *cp;
	struct jpulse *jp;
	uint32_t sweek;
	time_t last_timecode;
	u_short flags;

	jp = (struct jpulse *)sp;

	/* The timecode is presented as seconds into the current GPS week */
	sweek = DS2UI(jp->sweek) % WEEKSECS;

	/*
	 * If we don't know the current GPS week, calculate it from the
	 * current time. (It's too bad they didn't include this
	 * important value in the pulse message). We'd like to pick it
	 * up from one of the other messages like gpos or chan but they
	 * don't appear to be synchronous with time keeping and changes
	 * too soon (something like 10 seconds before the new GPS
	 * week).
	 *
	 * If we already know the current GPS week, increment it when
	 * we wrap into a new week.
	 */
	if (instance->gweek == 0) {
		if (!instance->gpos_gweek) {
			return ("jupiter_parse_t: Unknown gweek");
		}

		instance->gweek = instance->gpos_gweek;

		/*
		 * Fix warps. GPOS has GPS time and PULSE has UTC.
		 * Plus, GPOS need not be completely in synch with
		 * the PPS signal.
		 */
		if (instance->gpos_sweek >= sweek) {
			if ((instance->gpos_sweek - sweek) > WEEKSECS / 2)
				++instance->gweek;
		}
		else {
			if ((sweek - instance->gpos_sweek) > WEEKSECS / 2)
				--instance->gweek;
		}
	}
	else if (sweek == 0 && instance->lastsweek == WEEKSECS - 1) {
		++instance->gweek;
		jupiter_debug(instance->peer, __func__,
		    "NEW gps week %u", instance->gweek);
	}

	/*
	 * See if the sweek stayed the same (this happens when there is
	 * no pps pulse).
	 *
	 * Otherwise, look for time warps:
	 *
	 *   - we have stored at least one lastsweek and
	 *   - the sweek didn't increase by one and
	 *   - we didn't wrap to a new GPS week
	 *
	 * Then we warped.
	 */
	if (instance->lastsweek == sweek)
		jupiter_debug(instance->peer, __func__,
		    "gps sweek not incrementing (%d)",
		    sweek);
	else if (instance->lastsweek != 2 * WEEKSECS &&
	    instance->lastsweek + 1 != sweek &&
	    !(sweek == 0 && instance->lastsweek == WEEKSECS - 1))
		jupiter_debug(instance->peer, __func__,
		    "gps sweek jumped (was %d, now %d)",
		    instance->lastsweek, sweek);
	instance->lastsweek = sweek;

	/* This timecode describes next pulse */
	last_timecode = instance->timecode;
	instance->timecode =
	    GPS_EPOCH + (instance->gweek * WEEKSECS) + sweek;

	if (last_timecode == 0)
		/* XXX debugging */
		jupiter_debug(instance->peer, __func__,
		    "UTC <none> (gweek/sweek %u/%u)",
		    instance->gweek, sweek);
	else {
		char ascbuf[BUFSIZ];
		/* XXX debugging */
		tm = gmtime_r(&last_timecode, &tmbuf);
		cp = asctime_r(tm, ascbuf);

		jupiter_debug(instance->peer, __func__,
		    "UTC %.24s (gweek/sweek %u/%u)",
		    cp, instance->gweek, sweek);

		/* Billboard last_timecode (which is now the current time) */
		instance->peer->procptr->year   = tm->tm_year + 1900;
		instance->peer->procptr->day    = tm->tm_yday + 1;
		instance->peer->procptr->hour   = tm->tm_hour;
		instance->peer->procptr->minute = tm->tm_min;
		instance->peer->procptr->second = tm->tm_sec;
	}

	flags = getshort(jp->flags);

	/* Toss if not designated "valid" by the gps */
	if ((flags & JUPITER_O_PULSE_VALID) == 0) {
		refclock_report(instance->peer, CEVNT_BADTIME);
		return ("time mark not valid");
	}

	/* We better be sync'ed to UTC... */
	if ((flags & JUPITER_O_PULSE_UTC) == 0) {
		refclock_report(instance->peer, CEVNT_BADTIME);
		return ("time mark not sync'ed to UTC");
	}

	return (NULL);
}

static const char *
jupiter_parse_gpos(struct instance *instance, u_short *sp)
{
	struct jgpos *jg;
	time_t t;
	struct tm *tm, tmbuf;
	char *cp;
	char ascbuf[BUFSIZ];

	jg = (struct jgpos *)sp;

	if (jg->navval != 0) {
		/*
		 * Solution not valid. Use caution and refuse
		 * to determine GPS week from this message.
		 */
		instance->gpos_gweek = 0;
		instance->gpos_sweek = 0;
		return ("Navigation solution not valid");
	}

	instance->gpos_gweek = jg->gweek;
	instance->gpos_sweek = DS2UI(jg->sweek);
	/* coverity[tainted_data] */
	while(instance->gpos_sweek >= WEEKSECS) {
		instance->gpos_sweek -= WEEKSECS;
		++instance->gpos_gweek;
	}
	instance->gweek = 0;

	t = GPS_EPOCH + (instance->gpos_gweek * WEEKSECS) + instance->gpos_sweek;
	tm = gmtime_r(&t, &tmbuf);
	cp = asctime_r(tm, ascbuf);

	jupiter_debug(instance->peer, __func__,
		"GPS %.24s (gweek/sweek %u/%u)",
		cp, instance->gpos_gweek, instance->gpos_sweek);
	return (NULL);
}

/*
 * jupiter_debug - print debug messages
 */
static void
jupiter_debug(
	struct peer *	peer,
	const char *	function,
	const char *	fmt,
	...
	)
{
	char	buffer[200];
	va_list	ap;

	UNUSED_ARG(function);

	va_start(ap, fmt);
	/*
	 * Print debug message to stdout
	 * In the future, we may want to get get more creative...
	 */
	mvsnprintf(buffer, sizeof(buffer), fmt, ap);
	record_clock_stats(peer, buffer);
#ifdef DEBUG
	if (debug) {
		printf("%s: %s\n", function, buffer);
		fflush(stdout);
	}
#endif

	va_end(ap);
}

/* Checksum and transmit a message to the Jupiter */
static char *
jupiter_send(struct instance *instance, struct jheader *hp)
{
	u_int len, size;
	ssize_t cc;
	u_short *sp;
	static char errstr[132];

	size = sizeof(*hp);
	hp->hsum = putshort(jupiter_cksum((u_short *)hp,
	    (size / sizeof(u_short)) - 1));
	len = getshort(hp->len);
	if (len > 0) {
		sp = (u_short *)(hp + 1);
		sp[len] = putshort(jupiter_cksum(sp, len));
		size += (len + 1) * sizeof(u_short);
	}

	if ((cc = write(instance->peer->procptr->io.fd, (char *)hp, size)) < 0) {
		msnprintf(errstr, sizeof(errstr), "write: %m");
		return (errstr);
	} else if (cc != (int)size) {
		snprintf(errstr, sizeof(errstr), "short write (%zd != %u)", cc, size);
		return (errstr);
	}
	return (NULL);
}

/* Request periodic message output */
static struct {
	struct jheader jheader;
	struct jrequest jrequest;
} reqmsg = {
	{ putshort(JUPITER_SYNC), 0,
	    putshort((sizeof(struct jrequest) / sizeof(u_short)) - 1),
	    0, JUPITER_FLAG_REQUEST | JUPITER_FLAG_NAK |
	    JUPITER_FLAG_CONN | JUPITER_FLAG_LOG, 0 },
	{ 0, 0, 0, 0 }
};

/* An interval of zero means to output on trigger */
static void
jupiter_reqmsg(struct instance *instance, u_int id,
    u_int interval)
{
	struct jheader *hp;
	struct jrequest *rp;
	char *cp;

	hp = &reqmsg.jheader;
	hp->id = putshort(id);
	rp = &reqmsg.jrequest;
	rp->trigger = putshort(interval == 0);
	rp->interval = putshort(interval);
	if ((cp = jupiter_send(instance, hp)) != NULL)
		jupiter_debug(instance->peer, __func__, "%u: %s", id, cp);
}

/* Cancel periodic message output */
static struct jheader canmsg = {
	putshort(JUPITER_SYNC), 0, 0, 0,
	JUPITER_FLAG_REQUEST | JUPITER_FLAG_NAK | JUPITER_FLAG_DISC,
	0
};

static void
jupiter_canmsg(struct instance *instance, u_int id)
{
	struct jheader *hp;
	char *cp;

	hp = &canmsg;
	hp->id = putshort(id);
	if ((cp = jupiter_send(instance, hp)) != NULL)
		jupiter_debug(instance->peer, __func__, "%u: %s", id, cp);
}

/* Request a single message output */
static struct jheader reqonemsg = {
	putshort(JUPITER_SYNC), 0, 0, 0,
	JUPITER_FLAG_REQUEST | JUPITER_FLAG_NAK | JUPITER_FLAG_QUERY,
	0
};

static void
jupiter_reqonemsg(struct instance *instance, u_int id)
{
	struct jheader *hp;
	char *cp;

	hp = &reqonemsg;
	hp->id = putshort(id);
	if ((cp = jupiter_send(instance, hp)) != NULL)
		jupiter_debug(instance->peer, __func__, "%u: %s", id, cp);
}

/* Set the platform dynamics */
static struct {
	struct jheader jheader;
	struct jplat jplat;
} platmsg = {
	{ putshort(JUPITER_SYNC), putshort(JUPITER_I_PLAT),
	    putshort((sizeof(struct jplat) / sizeof(u_short)) - 1), 0,
	    JUPITER_FLAG_REQUEST | JUPITER_FLAG_NAK, 0 },
	{ 0, 0, 0 }
};

static void
jupiter_platform(struct instance *instance, u_int platform)
{
	struct jheader *hp;
	struct jplat *pp;
	char *cp;

	hp = &platmsg.jheader;
	pp = &platmsg.jplat;
	pp->platform = putshort(platform);
	if ((cp = jupiter_send(instance, hp)) != NULL)
		jupiter_debug(instance->peer, __func__, "%u: %s", platform, cp);
}

/* Checksum "len" shorts */
static u_short
jupiter_cksum(u_short *sp, u_int len)
{
	u_short sum, x;

	sum = 0;
	while (len-- > 0) {
		x = *sp++;
		sum += getshort(x);
	}
	return (~sum + 1);
}

/* Return the size of the next message (or zero if we don't have it all yet) */
static int
jupiter_recv(struct instance *instance)
{
	int n, len, size, cc;
	struct jheader *hp;
	uint8_t *bp;
	u_short *sp;

	/* Must have at least a header's worth */
	cc = sizeof(*hp);
	size = instance->ssize;
	if (size < cc)
		return (0);

	/* Search for the sync short if missing */
	sp = instance->sbuf;
	hp = (struct jheader *)sp;
	if (getshort(hp->sync) != JUPITER_SYNC) {
		/* Wasn't at the front, sync up */
		jupiter_debug(instance->peer, __func__, "syncing");
		bp = (uint8_t *)sp;
		n = size;
		while (n >= 2) {
			if (bp[0] != (JUPITER_SYNC & 0xff)) {
				/*
				jupiter_debug(instance->peer, __func__,
				    "{0x%x}", bp[0]);
				*/
				++bp;
				--n;
				continue;
			}
			if (bp[1] == ((JUPITER_SYNC >> 8) & 0xff))
				break;
			/*
			jupiter_debug(instance->peer, __func__,
			    "{0x%x 0x%x}", bp[0], bp[1]);
			*/
			bp += 2;
			n -= 2;
		}
		/*
		jupiter_debug(instance->peer, __func__, "\n");
		*/
		/* Shuffle data to front of input buffer */
		if (n > 0)
			memcpy(sp, bp, n);
		size = n;
		instance->ssize = size;
		if (size < cc || hp->sync != JUPITER_SYNC)
			return (0);
	}

	if (jupiter_cksum(sp, (cc / sizeof(u_short) - 1)) !=
	    getshort(hp->hsum)) {
	    jupiter_debug(instance->peer, __func__, "bad header checksum!");
		/* This is drastic but checksum errors should be rare */
		instance->ssize = 0;
		return (0);
	}

	/* Check for a payload */
	len = getshort(hp->len);
	if (len > 0) {
		n = (len + 1) * sizeof(u_short);
		/* Not enough data yet */
		if (size < cc + n)
			return (0);

		/* Check payload checksum */
		sp = (u_short *)(hp + 1);
		if (jupiter_cksum(sp, len) != getshort(sp[len])) {
			jupiter_debug(instance->peer,
			    __func__, "bad payload checksum!");
			/* This is drastic but checksum errors should be rare */
			instance->ssize = 0;
			return (0);
		}
		cc += n;
	}
	return (cc);
}

