/*
 * refclock_trimble - clock driver for the Trimble Palisade, Thunderbolt,
 * Acutime 2000, Acutime Gold and EndRun Technologies Praecis Ct/Cf/Ce/II
 * timing receivers
 *
 * For detailed information on this program, please refer to the
 * driver_trimble.html document accompanying the NTPsec distribution.
 *
 * Version 3.00; September 17, 2017
 * refer to driver_trimble.html for change log
 *
 * This software was developed by the Software and Component Technologies
 * group of Trimble Navigation, Ltd.
 *
 * Copyright (c) 1997, 1998, 1999, 2000  Trimble Navigation Ltd.
 * All rights reserved.
 * Copyright 2017 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-4-clause
 */

#include "config.h"

#if defined HAVE_SYS_MODEM_H
#include <sys/modem.h>
#endif

#include <termios.h>
#include <sys/stat.h>
#include <time.h>

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"
#include "timespecops.h"
#include "gpstolfp.h"

/*
 * GPS Definitions
 */
#define	DESCRIPTION	"Trimble Palisade/Thunderbolt/Acutime GPSes" /* Long name */
#define NAME		"TRIMBLE"	/* shortname */
#define	PRECISION	(-20)		/* precision assumed (about 1 us) */
#define	REFID		"GPS\0"		/* reference ID */
#define TRMB_MINPOLL    4		/* 16 seconds */
#define TRMB_MAXPOLL	5		/* 32 seconds */
#define MIN_SAMPLES	7		/* minimum number of samples in the median filter to allow a poll */

/*
 * I/O Definitions
 */
#ifndef ENABLE_CLASSIC_MODE
#define	DEVICE		"/dev/trimble%d" 	/* device name and unit */
#else
#define	DEVICE		"/dev/palisade%d" 	/* device name and unit */
#endif
#define	SPEED232	B9600		  	/* uart speed (9600 baud) */

/* parse consts */
#define RMAX 172 /* TSIP packet 0x58 can be 172 bytes */
#define DLE 0x10
#define ETX 0x03
#define MSG_TSIP 0
#define MSG_PRAECIS 1
#define SPSTAT_LEN 34 /* length of reply from Praecis SPSTAT message */

/* parse states */
#define TSIP_PARSED_EMPTY       0
#define TSIP_PARSED_FULL        1
#define TSIP_PARSED_DLE_1       2
#define TSIP_PARSED_DATA        3
#define TSIP_PARSED_DLE_2       4
#define TSIP_PARSED_ASCII       5
#define TSIP_PARSED_PARITY      6

#define mb(_X_) (up->rpt_buf[(_X_ + 1)]) /* shortcut for buffer access	*/

/* Conversion Definitions */
#ifdef DEBUG
# define GPS_PI 	(3.1415926535898)
# define R2D		(180.0/GPS_PI)
#endif

/*
 * Structure for build data packets for send (thunderbolt uses it only)
 * taken from Markus Prosch
 */
struct packettx
{
	short	size;
	uint8_t *data;
};

/*
 * Trimble unit control structure.
 */
struct trimble_unit {
	short		unit;		/* NTP refclock unit number */
	bool		got_pkt;	/* decoded a packet this poll */
	bool		got_time;	/* got a time packet this poll */
	int		samples;	/* samples in filter this poll */
	unsigned char	UTC_flags;	/* UTC & leap second flag */
	unsigned char	trk_status;	/* reported tracking status */
	char		rpt_status;	/* TSIP Parser State */
	size_t 		rpt_cnt;	/* TSIP packet length so far */
	char 		rpt_buf[RMAX]; 	/* packet assembly buffer */
	int		type;		/* Clock mode type */
	bool		use_event;	/* receiver has event input */
	int		MCR;		/* modem control register value at startup */
	bool		parity_chk;	/* enable parity checking */
	l_fp		p_recv_time;	/* timestamp of last received packet */
	unsigned int	week;		/* GPS week number */
	unsigned int	TOW;		/* GPS time of week */
	int		UTC_offset;	/* GPS-UTC offset */
	struct calendar	date;		/* calendar to avoid leap early announce */
	unsigned int	build_week;	/* GPS week number of ntpd build date */
};

/*
 * Function prototypes
 */
static	bool	trimble_start		(int, struct peer *);
static	void	trimble_poll		(int, struct peer *);
static	void	trimble_timer		(int, struct peer *);
static	void 	trimble_io		(struct recvbuf *);
static	void	trimble_receive		(struct peer *, int);
static	bool	TSIP_decode		(struct peer *);
static	void	HW_poll			(struct refclockproc *);
static	double	getdbl 			(uint8_t *);
static	short	getint 			(uint8_t *);
static	int32_t	getlong			(uint8_t *);
static  void	sendsupercmd		(struct packettx *buffer, int c1, int c2);
static  void	sendbyte		(struct packettx *buffer, int b);
static  void	sendint			(struct packettx *buffer, int a);
static  int	sendetx			(struct packettx *buffer, int fd);
static  void	init_thunderbolt	(int fd);

#define PAL_TSTATS 14
#ifdef DEBUG
static const char tracking_status[PAL_TSTATS+1][16] = {
	"Doing Fixes", "Good 1SV", "Approx. 1SV", "Need Time", "Need INIT",
	"PDOP too High", "Bad 1SV", "0SV Usable", "1SV Usable", "2SV Usable",
	"3SV Usable", "No Integrity", "Diff Corr", "Overdet Clock", "Invalid"};
#endif
static const bool tracking_status_usable[PAL_TSTATS+1] = {
	true, true, false, false, false,
	false, false, false, false, false,
	false, false, false, true, false};

#define TB_DECOD_STATS 16 /* convert TB decoding status to tracking_status */
static const unsigned int tb_decod_conv[TB_DECOD_STATS+1] = {
	0, 3, 14, 5, 14, 14, 14, 14, 7, 8, 9, 10, 6, 14, 14, 14, 11};

#define TB_DISC_MODES 7
#ifdef DEBUG
static const char tb_disc_mode[TB_DISC_MODES+1][16] = {
	"normal", "power-up", "auto holdover", "manual holdover",
	"recovery", "unknown", "disabled", "invalid"};
#endif
static const bool tb_disc_in_holdover[TB_DISC_MODES+1] = {
	false, false, true, true,
	false, false, false, false};

/*
 * Transfer vector
 */
struct refclock refclock_trimble = {
	NAME,			/* basename of driver */
	trimble_start,		/* start up driver */
	NULL,			/* shut down driver in the standard way */
	trimble_poll,		/* transmit poll message */
	NULL,			/* control - not used  */
	NULL,			/* initialize driver (not used) */
	trimble_timer		/* called at 1Hz by mainloop */
};

/* Extract the clock type from the mode setting */
#define CLK_TYPE(x) ((int)(((x)->cfg.mode) & 0x7F))

/* Supported clock types */
#define CLK_PALISADE	0	/* Trimble Palisade */
#define CLK_PRAECIS	1	/* Endrun Technologies Praecis */
#define CLK_THUNDERBOLT	2	/* Trimble Thunderbolt GPS Receiver */
#define CLK_ACUTIME     3	/* Trimble Acutime Gold */

/* packet 8f-ad UTC flags */
#define UTC_AVAILABLE	0x01
#define LEAP_SCHEDULED	0x10

/*
 * sendsupercmd - Build super data packet for sending
 */
static void
sendsupercmd (
	struct packettx *buffer,
	int c1,
	int c2
	)
{
	*buffer->data = DLE;
	*(buffer->data + 1) = (unsigned char)c1;
	*(buffer->data + 2) = (unsigned char)c2;
	buffer->size = 3;
}

/*
 * sendbyte -
 */
static void
sendbyte (
	struct packettx *buffer,
	int b
	)
{
	if (b == DLE)
		*(buffer->data+buffer->size++) = DLE;
	*(buffer->data+buffer->size++) = (unsigned char)b;
}

/*
 * sendint -
 */
static void
sendint (
	struct packettx *buffer,
	int a
	)
{
	sendbyte(buffer, (unsigned char)((a>>8) & 0xff));
	sendbyte(buffer, (unsigned char)(a & 0xff));
}

/*
 * sendetx - Send packet or super packet to the device
 */
static int
sendetx (
	struct packettx *buffer,
	int fd
	)
{
	ssize_t result;

	*(buffer->data+buffer->size++) = DLE;
	*(buffer->data+buffer->size++) = ETX;
	result = write(fd, buffer->data, (size_t)buffer->size);

	if (result != -1)
		return (result);
	else
		return (-1);
}

/*
 * init_thunderbolt - Prepares Thunderbolt receiver to be used with
 *		      NTP (also taken from Markus Prosch).
 */
static void
init_thunderbolt (
	int fd
	)
{
	struct packettx tx;

	tx.size = 0;
	tx.data = (uint8_t *) malloc(100);

	/* set UTC time */
	sendsupercmd (&tx, 0x8E, 0xA2);
	sendbyte     (&tx, 0x3);
	sendetx      (&tx, fd);

	/* activate packets 0x8F-AB and 0x8F-AC */
	sendsupercmd (&tx, 0x8E, 0xA5);
	sendint      (&tx, 0x5);
	sendetx      (&tx, fd);

	free(tx.data);
}

/*
 * trimble_start - open the devices and initialize data for processing
 */
static bool
trimble_start (
	int unit,
	struct peer *peer
	)
{
	struct trimble_unit *up;
	struct refclockproc *pp;
	int fd;
	struct termios tio;
	struct calendar build_date;
	unsigned int cflag, iflag;
	char device[20], *path;

	pp = peer->procptr;
	pp->clockname = NAME;

	/* Open serial port. */
	if (peer->cfg.path)
	    path = peer->cfg.path;
	else
	{
	    int rcode;
	    snprintf(device, sizeof(device), DEVICE, unit);

	    /* build a path */
	    rcode = snprintf(device, sizeof(device), DEVICE, unit);
	    if ( 0 > rcode ) {
	        /* failed, set to NUL */
	        device[0] = '\0';
	    }
	    path = device;
        }
	fd = refclock_open(path,
				  peer->cfg.baud ? peer->cfg.baud : SPEED232,
				  LDISC_RAW);
	if (0 > fd) {
	        msyslog(LOG_ERR, "REFCLOCK: %s Trimble device open(%s) failed",
			refclock_name(peer), path);
		/* coverity[leaked_handle] */
		return false;
	}

	LOGIF(CLOCKINFO, (LOG_NOTICE, "%s open at %s",
			  refclock_name(peer), path));

	if (tcgetattr(fd, &tio) < 0) {
		msyslog(LOG_ERR, "REFCLOCK: %s tcgetattr failed: %m",
		        refclock_name(peer));
		close(fd);
		return false;
	}

	/* Allocate and initialize unit structure */
	up = emalloc_zero(sizeof(*up));

	up->type = CLK_TYPE(peer);
	up->parity_chk = true;
	up->use_event = true;
	pp->disp = 1000 * S_PER_NS; /* extra ~500ns for serial port delay */

	switch (up->type) {
	    case CLK_PALISADE:
		msyslog(LOG_NOTICE, "REFCLOCK: %s Palisade mode enabled",
		        refclock_name(peer));
		break;
	    case CLK_PRAECIS:
		msyslog(LOG_NOTICE, "REFCLOCK: %s Praecis mode enabled",
			refclock_name(peer));
		/* account for distance to tower */
		pp->disp = .00002;
		break;
	    case CLK_THUNDERBOLT:
		msyslog(LOG_NOTICE, "REFCLOCK: %s Thunderbolt mode enabled",
			refclock_name(peer));
		up->parity_chk = false;
		up->use_event = false;
		/*
		 * packet transmission delay varies from 9ms to 32ms depending
		 * on the number of SVs the receiver is attempting to track
		 */
		pp->disp = .023;
		break;
	    case CLK_ACUTIME:
		msyslog(LOG_NOTICE, "REFCLOCK: %s Acutime Gold mode enabled",
			refclock_name(peer));
		break;
	    default:
	        msyslog(LOG_NOTICE, "REFCLOCK: %s mode unknown",
			refclock_name(peer));
		break;
		close(fd);
		free(up);
		return false;
	}
	tio.c_cflag = (CS8|CLOCAL|CREAD);
	tio.c_iflag &= (unsigned)~ICRNL;
	if (up->parity_chk) {
		tio.c_cflag |= (PARENB|PARODD);
		tio.c_iflag &= (unsigned)~IGNPAR;
		tio.c_iflag |= (INPCK|PARMRK);
	}
	cflag = tio.c_cflag;
	iflag = tio.c_iflag;
	if (tcsetattr(fd, TCSANOW, &tio) == -1 || tcgetattr(fd, &tio) == -1 ||
	    tio.c_cflag != cflag || tio.c_iflag != iflag) {
		msyslog(LOG_ERR, "REFCLOCK: %s tcsetattr failed: wanted cflag 0x%x got 0x%x, wanted iflag 0x%x got 0x%x, return: %m",
		        refclock_name(peer), cflag, (unsigned int)tio.c_cflag,
		        iflag, (unsigned int)tio.c_iflag);
		close(fd);
		free(up);
		return false;
	}
	if (up->use_event) {
		/*
		 * The width of the RTS pulse must be either less than 5us or
		 * greater than 600ms or the Acutime 2000 may try to switch its
		 * port A baud rate because of "Auto-DGPS". The Praecis will
		 * produce unstable timestamps (-7us instead of +-40ns offsets)
		 * when pulse width is more than a few us and less than 100us.
		 * Palisade minimum puse width is specified as 1us. To satisfy
		 * these constraints the RTS pin is idled with a positive
		 * voltage and pulsed negative.
		 */
		if (ioctl(fd, TIOCMGET, &up->MCR) < 0) {
			msyslog(LOG_ERR, "REFCLOCK: %s TIOCMGET failed: %m",
			        refclock_name(peer));
			close(fd);
			free(up);
			return false;
		}
		up->MCR |= TIOCM_RTS;
		if (ioctl(fd, TIOCMSET, &up->MCR) < 0 ||
		    !(up->MCR & TIOCM_RTS)) {
			msyslog(LOG_ERR, "REFCLOCK: %s TIOCMSET failed: MCR=0x%x, return=%m",
			        refclock_name(peer), (unsigned int)up->MCR);
			close(fd);
			free(up);
			return false;
		}
	}
	pp->io.clock_recv = trimble_io;
	pp->io.srcclock = peer;
	pp->io.datalen = 0;
	pp->io.fd = fd;
	if (!io_addclock(&pp->io)) {
		msyslog(LOG_ERR, "%s io_addclock failed", refclock_name(peer));
		close(fd);
		pp->io.fd = -1;
		free(up);
		return false;
	}

	/* Initialize miscellaneous variables */
	pp->unitptr = up;
	pp->clockdesc = DESCRIPTION;

	peer->precision = PRECISION;
	peer->sstclktype = CTL_SST_TS_UHF;
	peer->cfg.minpoll = TRMB_MINPOLL;
	peer->cfg.maxpoll = TRMB_MAXPOLL;
	memcpy((char *)&pp->refid, REFID, REFIDLEN);

	up->unit = (short) unit;
	up->rpt_status = TSIP_PARSED_EMPTY;
	up->rpt_cnt = 0;

	if (ntpcal_get_build_date(&build_date)) {
		caltogps(&build_date, 0, &up->build_week, NULL);
		up->build_week -= 2; /* timezone, UTC offset, build machine clock */
	} else {
		up->build_week = 0;
	}
	if (up->build_week < MIN_BUILD_GPSWEEK || up->build_week > MAX_BUILD_GPSWEEK) {
		msyslog(LOG_ERR,
			"REFCLOCK: %s ntpcal_get_build_date() failed: %u",
		        refclock_name(peer), up->build_week);
		close(fd);
		pp->io.fd = -1;
		free(up);
		return false;
	}

	if (up->type == CLK_THUNDERBOLT)
		init_thunderbolt(fd);

	return true;
}

/*
 * TSIP_decode - decode the TSIP data packets
 */
static bool
TSIP_decode (
	struct peer *peer
	)
{
	unsigned char id, decod_stat, disc_mode, timing_flags;
	long secint;
	double secs, secfrac;
	unsigned short event, m_alarms;
	uint32_t holdover_t;

	struct trimble_unit *up;
	struct refclockproc *pp;

	pp = peer->procptr;
	up = pp->unitptr;
	id = (unsigned char)up->rpt_buf[0];

	if (id == 0x8f) {
		/* Superpackets */
		event = (unsigned short) (getint((uint8_t *) &mb(1)) & 0xffff);
		if ((up->type != CLK_THUNDERBOLT) && !event)
			/* ignore auto-report */
			return false;

		switch (mb(0) & 0xff) {
		    case 0x0B:
			/*
			 * comprehensive time packet: sent after 8f-ad from
			 * Palisade and Acutime
			 */
			if (up->rpt_cnt != 74) {
				DPRINT(1, ("TSIP_decode: unit %d: 8f-0b packet length is not 74 (%d)\n",
				       up->unit, (int)up->rpt_cnt));
				refclock_report(peer, CEVNT_BADREPLY);
				return false;
			}
			up->got_time = true;
#ifdef DEBUG
			if (debug > 1) { /* SPECIAL DEBUG */
				int st, ts;
				double lat, lon, alt;
				lat = getdbl((uint8_t *) &mb(42)) * R2D;
				lon = getdbl((uint8_t *) &mb(50)) * R2D;
				alt = getdbl((uint8_t *) &mb(58));

				printf("TSIP_decode: unit %d: Latitude: %03.4f Longitude: %03.4f Alt: %05.2f m\n",
				       up->unit, lat,lon,alt);
				printf("TSIP_decode: unit %d: Sats:",
				       up->unit);
				for (st = 66, ts = 0; st <= 73; st++)
					if (mb(st)) {
						if (mb(st) > 0) ts++;
						printf(" %02d", mb(st));
					}
				printf(" : Tracking %d\n", ts);
			}
#endif
			if (!tracking_status_usable[up->trk_status]) {
				DPRINT(1, ("TSIP_decode: unit %d: unusable tracking status: %s\n",
				       up->unit,
				       tracking_status[up->trk_status]));
				return false;
			}
			up->UTC_offset = getint((uint8_t *) &mb(16));
			if (!(up->UTC_flags & UTC_AVAILABLE) ||
			    (up->UTC_offset == 0)) {
				pp->leap = LEAP_NOTINSYNC;
				DPRINT(1, ("TSIP_decode: unit %d: UTC data not available\n",
				       up->unit));
				return false;
			}

			secs = getdbl((uint8_t *) &mb(3));
			secint = (long) secs;
			secfrac = secs - secint; /* 0.0 <= secfrac < 1.0 */

			pp->nsec = (long) (secfrac * NS_PER_S);

			secint %= SECSPERDAY;    /* Only care about today */
			up->date.hour = (int)(secint / SECSPERHR);
			secint %= SECSPERHR;
			up->date.minute = (int)(secint / 60);
			secint %= 60;
			up->date.second = secint % 60;
			up->date.monthday = (uint8_t)mb(11);
			up->date.month = (uint8_t)mb(12);
			up->date.year = (uint16_t)getint((uint8_t *) &mb(13));
			up->date.yearday = 0;
			caltogps(&up->date, up->UTC_offset, &up->week, &up->TOW);
			gpsweekadj(&up->week, up->build_week);
			gpstocal(up->week, up->TOW, up->UTC_offset, &up->date);

			if ((up->UTC_flags & LEAP_SCHEDULED) &&
			    /* Avoid early announce: https://bugs.ntp.org/2773 */
			    (6 == up->date.month || 12 == up->date.month))
				pp->leap = LEAP_ADDSECOND;
			else
				pp->leap = LEAP_NOWARNING;

			DPRINT(2, ("TSIP_decode: unit %d: 8f-0b #%d %02d:%02d:%02d.%09ld %02d/%02d/%04d UTC %d\n    tracking status: %s\n",
			       up->unit, event, up->date.hour, up->date.minute,
			       up->date.second, pp->nsec, up->date.month,
			       up->date.monthday, up->date.year,
			       up->UTC_offset, tracking_status[up->trk_status]));

			/* don't reuse UTC flags or tracking status */
			up->UTC_flags = 0;
			up->trk_status = PAL_TSTATS;
			return true;
			break;

		    case 0xAD:
			/*
			 * primary UTC time packet: first packet sent after PPS
			 * from Palisade, Acutime, and Praecis
			 */
			if (up->rpt_cnt != 22) {
				DPRINT(1, ("TSIP_decode: unit %d: 8f-ad packet length is not 22 (%d)\n",
				       up->unit, (int)up->rpt_cnt));
				refclock_report(peer, CEVNT_BADREPLY);
				return false;
			}

			/* flags checked in 8f-0b for Palisade and Acutime */
			up->trk_status = (unsigned char)mb(18);
			if (up->trk_status > PAL_TSTATS)
				up->trk_status = PAL_TSTATS;
			up->UTC_flags = (unsigned char)mb(19);

			/* get timecode from 8f-0b except with Praecis */
			if (up->type != CLK_PRAECIS)
				return false;

			if (!tracking_status_usable[up->trk_status]) {
				DPRINT(1, ("TSIP_decode: unit %d: unusable tracking status: %s\n",
				       up->unit,
				       tracking_status[up->trk_status]));
				return false;
			}
			if (!(up->UTC_flags & UTC_AVAILABLE)) {
				pp->leap = LEAP_NOTINSYNC;
				DPRINT(1, ("TSIP_decode: unit %d: UTC data not available\n",
				       up->unit));
				return false;
			}

			pp->nsec = (long) (getdbl((uint8_t *) &mb(3)) * NS_PER_S);
			up->date.year = (uint16_t)getint((uint8_t *) &mb(16));
			up->date.hour = (uint8_t)mb(11);
			up->date.minute = (uint8_t)mb(12);
			up->date.second = (uint8_t)mb(13);
			up->date.month = (uint8_t)mb(15);
			up->date.monthday = (uint8_t)mb(14);
			caltogps(&up->date, 0, &up->week, &up->TOW);
			gpsweekadj(&up->week, up->build_week);
			gpstocal(up->week, up->TOW, 0, &up->date);

			if ((up->UTC_flags & LEAP_SCHEDULED) &&
			    /* Avoid early announce: https://bugs.ntp.org/2773 */
			    (6 == up->date.month || 12 == up->date.month))
				pp->leap = LEAP_ADDSECOND;
			else
				pp->leap = LEAP_NOWARNING;

			DPRINT(2, ("TSIP_decode: unit %d: 8f-ad #%d %02d:%02d:%02d.%09ld %02d/%02d/%04d UTC 0x%02x\n    tracking status: %s\n",
			       up->unit, event, up->date.hour, up->date.minute,
			       up->date.second, pp->nsec, up->date.month,
			       up->date.monthday, up->date.year,
			       up->UTC_flags, tracking_status[up->trk_status]));
			return true;
			break;

		    case 0xAC:
			/*
			 * supplemental timing packet: sent after 8f-ab from
			 * Thunderbolt
			 */
			if (up->rpt_cnt != 68) {
				DPRINT(1, ("TSIP_decode: unit %d: 8f-ac packet length is not 68 (%d)\n",
				       up->unit, (int)up->rpt_cnt));
				refclock_report(peer, CEVNT_BADREPLY);
				return false;
			}
			up->got_time = true;
#ifdef DEBUG
			if (debug > 1) { /* SPECIAL DEBUG */
				double lat, lon, alt;
				lat = getdbl((uint8_t *) &mb(36)) * R2D;
				lon = getdbl((uint8_t *) &mb(44)) * R2D;
				alt = getdbl((uint8_t *) &mb(52));
				printf("TSIP_decode: unit %d: Latitude: %03.4f Longitude: %03.4f Alt: %05.2f m\n",
				       up->unit, lat,lon,alt);
			}
#endif
			decod_stat = (unsigned char)mb(12);
			if (decod_stat > TB_DECOD_STATS)
				decod_stat = TB_DECOD_STATS;
			disc_mode = (unsigned char)mb(2);
			if (disc_mode > TB_DISC_MODES)
				disc_mode = TB_DISC_MODES;
			DPRINT(2, ("TSIP_decode: unit %d: leap=%d  decod.stat=%s  disc.mode=%s\n",
			       up->unit, pp->leap,
			       tracking_status[tb_decod_conv[decod_stat]],
			       tb_disc_mode[disc_mode]));

			m_alarms = (unsigned short)getint((uint8_t *) &mb(10));
			if (m_alarms & 0x200) {
				DPRINT(1, ("TSIP_decode: unit %d: 'position questionable' flag is set,\n    you must update the unit's stored position.\n",
				       up->unit));
				return false;
			}

			holdover_t = (uint32_t)getlong((uint8_t *) &mb(4));
			if (!tracking_status_usable[tb_decod_conv[decod_stat]])	{
				if (pp->fudgetime2 < 0.5) {
					/* holdover not enabled */
					DPRINT(1, ("TSIP_decode: unit %d: decod.stat of '%s' is unusable\n",
					       up->unit,
					       tracking_status[tb_decod_conv[decod_stat]]));
					return false;
				}else if (tb_disc_in_holdover[disc_mode] &&
				          holdover_t > pp->fudgetime2) {
					DPRINT(1, ("TSIP_decode: unit %d: unit in holdover (disc.mode=%s) with decod.stat of '%s' but holdover time of %us exceeds time2(%.fs)\n",
					       up->unit,
					       tb_disc_mode[disc_mode],
					       tracking_status[tb_decod_conv[decod_stat]],
					       holdover_t, pp->fudgetime2));
					return false;
				} else if (!tb_disc_in_holdover[disc_mode]) {
					DPRINT(1, ("TSIP_decode: unit %d: not in holdover (disc.mode=%s) and decod.stat of '%s' is unusable\n",
					       up->unit, tb_disc_mode[disc_mode],
					       tracking_status[tb_decod_conv[decod_stat]]));
					return false;
				}
			}

			if (up->UTC_flags != UTC_AVAILABLE)
				return false;

			gpsweekadj(&up->week, up->build_week);
			gpstocal(up->week, up->TOW, up->UTC_offset, &up->date);
			if ((m_alarms & 0x80) &&
			/* Avoid early announce: https://bugs.ntp.org/2773 */
			    (6 == up->date.month || 12 == up->date.month) )
				pp->leap = LEAP_ADDSECOND;  /* we ASSUME addsecond */
			else
				pp->leap = LEAP_NOWARNING;

			DPRINT(2, ("TSIP_decode: unit %d: 8f-ac TOW: %u week: %u adj.t: %02d:%02d:%02d.0 %02d/%02d/%04d\n",
			       up->unit, up->TOW, up->week,
			       up->date.hour, up->date.minute, up->date.second,
			       up->date.month, up->date.monthday, up->date.year));
			return true;
			break;

		    case 0xAB:
			/*
			 * primary timing packet: first packet sent after PPS
			 * from Thunderbolt
			 */
			if (up->rpt_cnt != 17) {
				DPRINT(1, ("TSIP_decode: unit %d: 8f-ab packet length is not 17 (%d)\n",
				       up->unit, (int)up->rpt_cnt));
				refclock_report(peer, CEVNT_BADREPLY);
				return 0;
			}
			timing_flags = (unsigned char)mb(9);
#ifdef DEBUG
			if (debug > 1) { /* SPECIAL DEBUG */
				printf("TSIP_decode: unit %d: timing flags:0x%02X=\n",
				       up->unit, timing_flags);
				if (timing_flags & 0x08) {
					printf("    timecode aligned to GPS(UTC not avail.), PPS aligned to GPS(UTC not avail.)\n");
				} else {
					if (timing_flags & 0x01)
						printf("    timecode aligned to UTC, ");
					else
						printf("    timecode aligned to GPS(misconfigured), ");
					if (timing_flags & 0x02)
						printf("PPS aligned to UTC\n");
					else
						printf("PPS aligned to GPS(misconfigured)\n");
				}
				if (timing_flags & 0x04)
					printf("    time is NOT set, ");
				else
					printf("    time is set, ");
				if (timing_flags & 0x08)
					printf("UTC is NOT available, ");
				else
					printf("UTC is available, ");
				if (timing_flags & 0x10)
					printf("test-mode timesource(misconfigured)\n");
				else
					printf("satellite timesource\n");
			}
#endif
			up->UTC_flags = 0;
			up->UTC_offset = getint((uint8_t *) &mb(7));
			if (timing_flags & 0x04 || timing_flags & 0x08 ||
			    up->UTC_offset == 0) {
				DPRINT(1, ("TSIP_decode: unit %d: time not set or UTC offset unavailable\n",
					up->unit));
				return false;
			}
			/*
			 * configuration is sent only at ntpd startup. if unit
			 * loses power it will revert to the factory default
			 * time alignment (GPS)
			 */
			if (!(timing_flags & 0x01) || !(timing_flags & 0x02) ||
			    (timing_flags & 0x10)) {
				DPRINT(1, ("TSIP_decode: unit %d: 8f-ab flags: not UTC time: unit is misconfigured (0x%02X)\n",
				       up->unit, timing_flags));
				pp->leap = LEAP_NOTINSYNC;
				refclock_report(peer, CEVNT_BADTIME);
				return false;
			}
			up->TOW = (uint32_t)getlong((uint8_t *) &mb(1));
			up->week = (uint32_t)getint((uint8_t *) &mb(5));

			pp->lastrec = up->p_recv_time;
			pp->nsec = 0;
			up->UTC_flags = UTC_AVAILABLE; /* flag for 8f-ac */
			return false;
			break;

		    default:
			break;
		} /* switch */
	}
	return false;
}

/*
 * trimble__receive - receive data from the serial interface
 */
static void
trimble_receive (
	struct peer * peer,
	int type
	)
{
	struct trimble_unit *up;
	struct refclockproc *pp;

	/* Initialize pointers and read the timecode and timestamp. */
	pp = peer->procptr;
	up = pp->unitptr;

	/*
	 * Wait for fudge flags to initialize. Also, startup may have caused
	 * a spurious edge, so wait for first HW_poll()
	 */
	if (pp->polls < 1)
		return;

	up->got_pkt = true;
	if (MSG_TSIP == type) {
		if (!TSIP_decode(peer))
			return;
	} else {
		if (SPSTAT_LEN == up->rpt_cnt &&
		    up->rpt_buf[up->rpt_cnt - 1] == '\r') {
			up->rpt_buf[up->rpt_cnt - 1] = '\0';
			record_clock_stats(peer, up->rpt_buf);
		}
		return;
	}

	/* add sample to filter */
	pp->lastref = pp->lastrec;
	pp->year = up->date.year;
	pp->day = up->date.yearday;
	pp->hour = up->date.hour;
	pp->minute = up->date.minute;
	pp->second = up->date.second;
	DPRINT(2, ("trimble_receive: unit %d: %4d %03d %02d:%02d:%02d.%09ld\n",
		   up->unit, pp->year, pp->day, pp->hour, pp->minute,
		   pp->second, pp->nsec));
	if (!refclock_process(pp)) {
		refclock_report(peer, CEVNT_BADTIME);
		DPRINT(1, ("trimble_receive: unit %d: refclock_process failed!\n",
		       up->unit));
		return;
	}
	up->samples++;
}

/*
 * trimble_poll - called by the transmit procedure
 */
static void
trimble_poll (
	int unit,
	struct peer *peer
	)
{
	struct trimble_unit *up;
	struct refclockproc *pp;
	int cl;
	bool err;

	UNUSED_ARG(unit);

	pp = peer->procptr;
	up = pp->unitptr;

	/* samples are not taken until second poll */
	if (++pp->polls < 2)
		return;

	/* check status for the previous poll interval */
	err = (up->samples < MIN_SAMPLES);
	if (err) {
		refclock_report(peer, CEVNT_TIMEOUT);
		if (!up->got_pkt) {
			DPRINT(1, ("trimble_poll: unit %d: no packets found\n",
			       up->unit));
		} else if (!up->got_time) {
			DPRINT(1, ("trimble_poll: unit %d: packet(s) found but none were usable.\nVerify unit isn't connected to Port B and flag3 is correct for Palisade/Acutime\n",
			       up->unit));
		} else {
			DPRINT(1, ("trimble_poll: unit %d: not enough samples (%d, min %d), skipping poll\n",
			       up->unit, up->samples, MIN_SAMPLES));
			pp->codeproc = pp->coderecv; /* reset filter */
		}
	}
	up->got_time = false;
	up->got_pkt = false;
	up->samples = 0;
	if (err)
		return;

	/* ask Praecis for its signal status */
	if(up->type == CLK_PRAECIS) {
		if(write(peer->procptr->io.fd,"SPSTAT\r\n",8) < 0)
			msyslog(LOG_ERR, "REFCLOCK: %s write: %m:",
			        refclock_name(peer));
	}

	/* record clockstats */
	cl = snprintf(pp->a_lastcode, sizeof(pp->a_lastcode),
		 "%4d %03d %02d:%02d:%02d.%09ld",
		 pp->year, pp->day, pp->hour,pp->minute, pp->second, pp->nsec);
	pp->lencode = (cl < (int)sizeof(pp->a_lastcode)) ? cl : 0;
	record_clock_stats(peer, pp->a_lastcode);

	DPRINT(2, ("trimble_poll: unit %d: %s\n",
	       up->unit, prettydate(pp->lastrec)));

	if (pp->hour == 0 && up->week > up->build_week + 1000)
		msyslog(LOG_WARNING, "REFCLOCK: %s current GPS week number (%u) is more than 1000 weeks past ntpd's build date (%u), please update",
		        refclock_name(peer), up->week, up->build_week);

	/* process samples in filter */
	refclock_receive(peer);
}

/*
 * trimble_io - create TSIP packets or ASCII strings from serial data stream
 */
static void
trimble_io (
	struct recvbuf *rbufp
	)
{
	struct trimble_unit *up;
	struct refclockproc *pp;
	struct peer *peer;

	char * c, * d;

	peer = rbufp->recv_peer;
	pp = peer->procptr;
	up = pp->unitptr;

	c = (char *) &rbufp->recv_space;
	d = c + rbufp->recv_length;

	while (c != d) {
		switch (up->rpt_status) {
		    case TSIP_PARSED_DLE_1:
			switch (*c)
			{
			    case 0:
			    case DLE:
			    case ETX:
				up->rpt_status = TSIP_PARSED_EMPTY;
				break;

			    default:
				up->rpt_status = TSIP_PARSED_DATA;
				/* save packet ID */
				up->rpt_buf[0] = *c;
				/* save packet receive time */
				up->p_recv_time = rbufp->recv_time;
				break;
			}
			break;

		    case TSIP_PARSED_DATA:
			if (*c == DLE)
				up->rpt_status = TSIP_PARSED_DLE_2;
			else if (up->parity_chk && *c == '\377')
				up->rpt_status = TSIP_PARSED_PARITY;
			else
				mb(up->rpt_cnt++) = *c;
			break;

		    case TSIP_PARSED_PARITY:
			if (*c == '\377') {
				up->rpt_status = TSIP_PARSED_DATA;
				mb(up->rpt_cnt++) = *c;
			} else {
				msyslog(LOG_ERR, "REFCLOCK: %s: detected serial parity error or receive buffer overflow",
					refclock_name(peer));
				up->rpt_status = TSIP_PARSED_EMPTY;
			}
			break;

		    case TSIP_PARSED_DLE_2:
			if (*c == DLE) {
				up->rpt_status = TSIP_PARSED_DATA;
				mb(up->rpt_cnt++) = *c;
			} else if (*c == ETX) {
				up->rpt_status = TSIP_PARSED_FULL;
				trimble_receive(peer, MSG_TSIP);
			} else {
				/* error: start new report packet */
				up->rpt_status = TSIP_PARSED_DLE_1;
				up->rpt_buf[0] = *c;
			}
			break;

		    case TSIP_PARSED_ASCII:
			if (*c == '\n') {
				mb(up->rpt_cnt++) = *c;
				up->rpt_status = TSIP_PARSED_FULL;
				trimble_receive(peer, MSG_PRAECIS);
			} else if (up->parity_chk && *c == '\377') {
				up->rpt_status = TSIP_PARSED_PARITY;
			} else {
				mb(up->rpt_cnt++) = *c;
			}
			break;

		    case TSIP_PARSED_FULL:
		    case TSIP_PARSED_EMPTY:
		    default:
			up->rpt_cnt = 0;
			if (*c == DLE) {
				up->rpt_status = TSIP_PARSED_DLE_1;
			} else if (up->type == CLK_PRAECIS && NULL != strchr("6L789ADTP", *c)) {
				/* Praecis command reply */
				up->rpt_buf[0] = *c;
				up->rpt_status = TSIP_PARSED_ASCII;
			} else {
 				up->rpt_status = TSIP_PARSED_EMPTY;
			}
			break;
		}
		c++;
		if (up->rpt_cnt > RMAX - 2) {/* additional byte for ID */
			up->rpt_status = TSIP_PARSED_EMPTY;
			DPRINT(1, ("trimble_io: unit %d: oversize serial message (%luB) 0x%02x discarded\n",
			        up->unit, (unsigned long)up->rpt_cnt,
				(uint8_t)up->rpt_buf[0]));
		}
	} /* while chars in buffer */
}

/*
 * trimble_timer - trigger an event at 1Hz
 */
static void
trimble_timer(
	int unit,
	struct peer * peer
	)
{
	struct trimble_unit *up;
	struct refclockproc *pp;

	UNUSED_ARG(unit);

	pp = peer->procptr;
	up = pp->unitptr;

	if (up->use_event)
		HW_poll(pp);
}

/*
 * HW_poll - trigger the event input
 */
static void
HW_poll (
	struct refclockproc * pp
	)
{
	struct trimble_unit *up;
	static const struct timespec ts = {0, 13 * NS_PER_MS};

	up = pp->unitptr;

	/* Edge trigger */
	if (pp->sloppyclockflag & CLK_FLAG3) {
		IGNORE(write (pp->io.fd, "", 1));
	} else {
		up->MCR &= ~TIOCM_RTS; /* set RTS low from high idle state */
		IGNORE(ioctl(pp->io.fd, TIOCMSET, &up->MCR));

		/*
		 * The Acutime 2000 will occasionally transmit with parity
		 * errors if the low state is held for less than 1ms, and the
		 * Praecis will produce unstable timestamps if the low state is
		 * held for less than 12ms.
		 */
		nanosleep(&ts, NULL);

		up->MCR |= TIOCM_RTS;  /* make edge / restore idle */
		IGNORE(ioctl(pp->io.fd, TIOCMSET, &up->MCR));
	}

	/* get timestamp after triggering since RAND_bytes is slow */
	get_systime(&pp->lastrec);
}

/*
 * getdbl - copy/swap a big-endian palisade double into a host double
 */
static double
getdbl (
	uint8_t *bp
	)
{
#ifdef WORDS_BIGENDIAN
	double out;

	memcpy(&out, bp, sizeof(out));
	return out;
#else
	union {
		uint8_t ch[8];
		uint32_t u32[2];
	} ui;

	union {
		double out;
		uint32_t u32[2];
	} uo;

	memcpy(ui.ch, bp, sizeof(ui.ch));
	/* least-significant 32 bits of double from swapped bp[4] to bp[7] */
	uo.u32[0] = ntohl(ui.u32[1]);
	/* most-significant 32 bits from swapped bp[0] to bp[3] */
	uo.u32[1] = ntohl(ui.u32[0]);

	return uo.out;
#endif
}

/*
 * getint - copy/swap a big-endian palisade short into a host short
 */
static short
getint (
	uint8_t *bp
	)
{
	unsigned short us;

	memcpy(&us, bp, sizeof(us));
	return (short)ntohs(us);
}

/*
 * getlong -copy/swap a big-endian palisade 32-bit int into a host 32-bit int
 */
static int32_t
getlong(
	uint8_t *bp
	)
{
	uint32_t u32;

	memcpy(&u32, bp, sizeof(u32));
	return (int32_t)(uint32_t)ntohl(u32);
}
