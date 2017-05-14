/*
 * refclock_trimble - clock driver for the Trimble Palisade
 * Thunderbolt, and Acutime Gold timing receivers
 *
 * For detailed information on this program, please refer to the html 
 * refclock_trimble.html page accompanying the NTP distribution.
 *
 * for questions / bugs / comments, contact:
 * sven_dietrich@trimble.com
 *
 * Sven-Thorsten Dietrich
 * 645 North Mary Avenue
 * Post Office Box 3642
 * Sunnyvale, CA 94088-3642
 *
 * Version 2.45; July 14, 1999
 *
 * This software was developed by the Software and Component Technologies
 * group of Trimble Navigation, Ltd.
 * *
 * 31/03/06: Added support for Thunderbolt GPS Disciplined Clock.
 *	     Contact: Fernando Pablo Hauscarriaga
 * 	     E-mail: fernandoph@iar.unlp.edu.ar
 * 	     Home page: www.iar.unlp.edu.ar/~fernandoph
 *		  Instituto Argentino de Radioastronomia
 *			    www.iar.unlp.edu.ar
 *
 * 14/01/07: Conditinal compilation for Thunderbolt support no longer needed
 *	     now we use mode 2 for decode thunderbolt packets.
 *	     Fernando P. Hauscarriaga
 *
 * 30/08/09: Added support for Trimble Acutime Gold Receiver.
 *	     Fernando P. Hauscarriaga (fernandoph@iar.unlp.edu.ar)
 *
 * Copyright (c) 1997, 1998, 1999, 2000  Trimble Navigation Ltd.
 * All rights reserved.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-4-clause
 */

#include "config.h"

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif /* not HAVE_SYS_IOCTL_H */

#if defined HAVE_SYS_MODEM_H
#include <sys/modem.h>
#endif

#include <termios.h>
#include <sys/stat.h>

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

/*
 * I/O Definitions
 */
#ifndef ENABLE_CLASSIC_MODE
#define	DEVICE		"/dev/trimble%d" 	/* device name and unit */
#else
#define	DEVICE		"/dev/palisade%d" 	/* device name and unit */
#endif
#define	SPEED232	B9600		  	/* uart speed (9600 baud) */

/*
 * TSIP Report Definitions
 */
#define LENCODE_8F0B	74	/* Length of TSIP 8F-0B Packet & header */
#define LENCODE_NTP     22	/* Length of Trimble NTP Packet */

#define LENCODE_8FAC    68      /* Length of Thunderbolt 8F-AC Position Packet*/
#define LENCODE_8FAB    17      /* Length of Thunderbolt Primary Timing Packet*/

/* Allowed Sub-Packet ID's */
#define PACKET_8F0B	0x0B
#define PACKET_NTP	0xAD

/* Thunderbolt Packets */
#define PACKET_8FAC     0xAC	/* Supplementary Thunderbolt Time Packet */
#define PACKET_8FAB     0xAB	/* Primary Thunderbolt Time Packet */
#define PACKET_6D	0x6D	/* Supplementary Thunderbolt Tracking Stats */
#define PACKET_41	0x41	/* Thunderbolt I don't know what this packet is, it's not documented on my manual*/

/* Acutime Packets */
#define PACKET_41A      0x41    /* GPS time */
#define PACKET_46       0x46    /* Receiver Health */

#define DLE 0x10
#define ETX 0x03

/* parse states */
#define TSIP_PARSED_EMPTY       0	
#define TSIP_PARSED_FULL        1
#define TSIP_PARSED_DLE_1       2
#define TSIP_PARSED_DATA        3
#define TSIP_PARSED_DLE_2       4

/* 
 * Leap-Insert and Leap-Delete are encoded as follows:
 * 	TRIMBLE_UTC_TIME set   and TRIMBLE_LEAP_PENDING set: INSERT leap
 */

/* #define TRIMBLE_LEAP_INPROGRESS 0x08 * This is the leap flag	UNUSED */
/* #define TRIMBLE_LEAP_WARNING    0x04 * GPS Leap Warning (ICD-200) UNUSED */
#define TRIMBLE_LEAP_PENDING    0x02 /* Leap Pending (24 hours)		*/
#define TRIMBLE_UTC_TIME        0x01 /* UTC time available				*/

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
	bool 		got_time;	/* time is valid for this poll */
	char		leap_status;	/* leap second flag */
	char		rpt_status;	/* TSIP Parser State */
	size_t 		rpt_cnt;	/* TSIP packet length so far */
	char 		rpt_buf[BMAX]; 	/* packet assembly buffer */
	int		type;		/* Clock mode type */
	u_int		week;		/* GPS week number */
	u_long		TOW;		/* GPS time of week */
	int		UTC_offset;	/* GPS-UTC offset */
	struct calendar	date;		/* calendar to avoid leap early announce */
	u_int		build_week;	/* GPS week number of ntpd build date */
};

/*
 * Function prototypes
 */

static	bool	trimble_start		(int, struct peer *);
static	void	trimble_shutdown	(int, struct peer *);
static	void	trimble_receive	(struct peer *);
static	void	trimble_poll		(int, struct peer *);
static	void 	trimble_io		(struct recvbuf *);
static	bool	TSIP_decode		(struct peer *);
static	bool	HW_poll			(struct refclockproc *);
static	double	getdbl 			(uint8_t *);
static	short	getint 			(uint8_t *);
static	int32_t	getlong			(uint8_t *);
#ifdef __UNUSED__
static  void	sendcmd			(struct packettx *buffer, int c);
#endif
static  void	sendsupercmd		(struct packettx *buffer, int c1, int c2);
static  void	sendbyte		(struct packettx *buffer, int b);
static  void	sendint			(struct packettx *buffer, int a);
static  int	sendetx			(struct packettx *buffer, int fd);
static  void	init_thunderbolt	(int fd);
static  void	init_acutime		(int fd);

#ifdef DEBUG
const char * Tracking_Status[15][15] = { 
	{ "Doing Fixes\0" }, { "Good 1SV\0" }, { "Approx. 1SV\0" },
	{"Need Time\0" }, { "Need INIT\0" }, { "PDOP too High\0" },
	{ "Bad 1SV\0" }, { "0SV Usable\0" }, { "1SV Usable\0" },
	{ "2SV Usable\0" }, { "3SV Usable\0" }, { "No Integrity\0" },
	{ "Diff Corr\0" }, { "Overdet Clock\0" }, { "Invalid\0" } };
#endif

/*
 * Transfer vector
 */
struct refclock refclock_trimble = {
	NAME,			/* basename of driver */
	trimble_start,		/* start up driver */
	trimble_shutdown,	/* shut down driver */
	trimble_poll,		/* transmit poll message */
	NULL,			/* control - not used  */
	NULL,			/* initialize driver (not used) */
	NULL			/* timer - not used */
};

/* Extract the clock type from the mode setting */
#define CLK_TYPE(x) ((int)(((x)->ttl) & 0x7F))

/* Supported clock types */
#define CLK_TRIMBLE	0	/* Trimble Palisade */
#define CLK_PRAECIS	1	/* Endrun Technologies Praecis */
#define CLK_THUNDERBOLT	2	/* Trimble Thunderbolt GPS Receiver */
#define CLK_ACUTIME     3	/* Trimble Acutime Gold */
/* #define CLK_ACUTIMEB 4	* Trimble Actutime Gold Port B UNUSED */

static bool praecis_msg;
static void praecis_parse(struct recvbuf *rbufp, struct peer *peer);

/* These routines are for sending packets to the Thunderbolt receiver
 * They are taken from Markus Prosch
 */

#ifdef __UNUSED__
/*
 * sendcmd - Build data packet for sending
 */
static void 
sendcmd (
	struct packettx *buffer,
	int c
	)
{
	*buffer->data = DLE;
	*(buffer->data + 1) = (unsigned char)c;
	buffer->size = 2;
}
#endif	/* __UNUSED_ */

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
	result = write(fd, buffer->data, (unsigned long)buffer->size);
	
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
 * init_acutime - Prepares Acutime Receiver to be used with NTP
 */
static void
init_acutime (
	int fd
	)
{
	/* Disable all outputs, Enable Event-Polling on PortA so
	   we can ask for time packets */
	struct packettx tx;

	tx.size = 0;
	tx.data = (uint8_t *) malloc(100);

	sendsupercmd(&tx, 0x8E, 0xA5);
	sendbyte(&tx, 0x02);
	sendbyte(&tx, 0x00);
	sendbyte(&tx, 0x00);
	sendbyte(&tx, 0x00);
	sendetx(&tx, fd);

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
	char gpsdev[20];
	struct termios tio;
	struct calendar build_date;

	snprintf(gpsdev, sizeof(gpsdev), DEVICE, unit);

	/*
	 * Open serial port. 
	 */
	fd = refclock_open(peer->path ? peer->path : gpsdev,
			   peer->baud ? peer->baud : SPEED232,
			   LDISC_RAW);
	if (fd <= 0) {
#ifdef DEBUG
		printf("Trimble(%d) start: open %s failed\n", unit, gpsdev);
#endif
		/* coverity[leaked_handle] */
		return false;
	}

	msyslog(LOG_NOTICE, "Trimble(%d) fd: %d dev: %s", unit, fd,
		gpsdev);

	if (tcgetattr(fd, &tio) < 0) {
		msyslog(LOG_ERR, 
			"Trimble(%d) tcgetattr(fd, &tio): %m",unit);
#ifdef DEBUG
		printf("Trimble(%d) tcgetattr(fd, &tio)\n",unit);
#endif
		close(fd);
		return false;
	}

	tio.c_cflag |= (PARENB|PARODD);
	tio.c_iflag &= (unsigned)~ICRNL;

	/*
	 * Allocate and initialize unit structure
	 */
	up = emalloc_zero(sizeof(*up));

	up->type = CLK_TYPE(peer);
	switch (up->type) {
	    case CLK_TRIMBLE:
		/* Normal mode, do nothing */
		break;
	    case CLK_PRAECIS:
		msyslog(LOG_NOTICE, "Trimble(%d) Praecis mode enabled"
			,unit);
		break;
	    case CLK_THUNDERBOLT:
		msyslog(LOG_NOTICE, "Trimble(%d) Thunderbolt mode enabled"
			,unit);
		tio.c_cflag = (CS8|CLOCAL|CREAD);
		break;
	    case CLK_ACUTIME:
		msyslog(LOG_NOTICE, "Trimble(%d) Acutime Gold mode enabled"
			,unit);
		break;
	    default:
		msyslog(LOG_NOTICE, "Trimble(%d) mode unknown",unit);
		break;
	}
	if (tcsetattr(fd, TCSANOW, &tio) == -1) {
		msyslog(LOG_ERR, "Trimble(%d) tcsetattr(fd, &tio): %m",unit);
#ifdef DEBUG
		printf("Trimble(%d) tcsetattr(fd, &tio)\n",unit);
#endif
		close(fd);
		free(up);
		return false;
	}

	pp = peer->procptr;
	pp->io.clock_recv = trimble_io;
	pp->io.srcclock = peer;
	pp->io.datalen = 0;
	pp->io.fd = fd;
	if (!io_addclock(&pp->io)) {
#ifdef DEBUG
		printf("Trimble(%d) io_addclock\n",unit);
#endif
		close(fd);
		pp->io.fd = -1;
		free(up);
		return false;
	}

	/*
	 * Initialize miscellaneous variables
	 */
	pp->unitptr = up;
	pp->clockname = NAME;
	pp->clockdesc = DESCRIPTION;

	peer->precision = PRECISION;
	peer->sstclktype = CTL_SST_TS_UHF;
	peer->minpoll = TRMB_MINPOLL;
	peer->maxpoll = TRMB_MAXPOLL;
	memcpy((char *)&pp->refid, REFID, REFIDLEN);
	
	up->leap_status = 0;
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
		msyslog(LOG_ERR, "Trimble(%d) ntpcal_get_build_date() failed: %u",
		        unit, up->build_week);
		close(fd);
		pp->io.fd = -1;
		free(up);
		return false;
	}

	if (up->type == CLK_THUNDERBOLT)
		init_thunderbolt(fd);
	if (up->type == CLK_ACUTIME)
		init_acutime(fd);

	return true;
}


/*
 * trimble_shutdown - shut down the clock
 */
static void
trimble_shutdown (
	int unit,
	struct peer *peer
	)
{
	struct trimble_unit *up;
	struct refclockproc *pp;

	UNUSED_ARG(unit);

	pp = peer->procptr;
	up = pp->unitptr;
	if (-1 != pp->io.fd)
		io_closeclock(&pp->io);
	if (NULL != up)
		free(up);
}


/* 
 * TSIP_decode - decode the TSIP data packets 
 */
static bool
TSIP_decode (
	struct peer *peer
	)
{
	int st;
	unsigned short event = 0;

	struct trimble_unit *up;
	struct refclockproc *pp;

	pp = peer->procptr;
	up = pp->unitptr;

	/*
	 * Check the time packet, decode its contents. 
	 * If the timecode has invalid length or is not in
	 * proper format, declare bad format and exit.
	 */

	if ((up->type != CLK_THUNDERBOLT) & (up->type != CLK_ACUTIME)){
		if ((up->rpt_buf[0] == (char) 0x41) ||
		    (up->rpt_buf[0] == (char) 0x46) ||
		    (up->rpt_buf[0] == (char) 0x54) ||
		    (up->rpt_buf[0] == (char) 0x4B) ||
		    (up->rpt_buf[0] == (char) 0x6D)) {

			/* standard time packet - GPS time and GPS week number */
#ifdef DEBUG
			printf("Trimble Port B packets detected. Connect to Port A\n");
#endif
			return false;
		}
	}

	/*
	 * We cast both to uint8_t to as 0x8f uses the sign bit on a char
	 */
	if ((uint8_t) up->rpt_buf[0] == (uint8_t) 0x8f) {
		/* 
		 * Superpackets
		 */
		event = (unsigned short) (getint((uint8_t *) &mb(1)) & 0xffff);
		if (!((pp->sloppyclockflag & CLK_FLAG2) || event)) 
			/* Ignore Packet */
			return false;	   
	
		switch (mb(0) & 0xff) {

		    case PACKET_8F0B: 
			if (up->rpt_cnt != LENCODE_8F0B)  /* check length */
				break;

			up->UTC_offset = getint((uint8_t *) &mb(16));
			if (up->got_time)
				return false;
		
#ifdef DEBUG
			if (debug > 1) {
				int ts;
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
			if (debug) {
				if (up->UTC_offset == 0) /* Check UTC offset */ 
					printf("TSIP_decode: UTC Offset Unknown\n");
			}
#endif
			/* 
			 * except for Praecis, 8f-0b will always be sent after
			 * 8f-ad if the receiver is in a usable state, so the
			 * time isn't needed from this packet, only UTC offset
			 */
			return false;

		    case PACKET_NTP:
			/* Trimble-NTP Packet */

			if (up->rpt_cnt != LENCODE_NTP) /* check length */
				break;
	
			up->leap_status = mb(19);

			if (up->got_time)
				return false;
				
			/* Praecis produces only 8f-ad */
			if (up->type == CLK_PRAECIS) {
				up->UTC_offset = 0;
			} else if (up->UTC_offset == 0) {
#ifdef DEBUG
				printf("TSIP_decode 8f-ad: need UTC offset from 8f-0b\n");
#endif
				return false;
			}

			/* Check Tracking Status */
			st = mb(18);
			if (st < 0 || st > 14)
				st = 14;
			if ((st >= 2 && st <= 7) || st == 11 || st == 12) {
#ifdef DEBUG
				printf("TSIP_decode: Not Tracking Sats : %s\n",
				       *Tracking_Status[st]);
#endif
				refclock_report(peer, CEVNT_BADTIME);
				return false;
			}

			if ( (up->leap_status & TRIMBLE_LEAP_PENDING) &&
			/* Avoid early announce: https://bugs.ntp.org/2773 */
				(6 == up->date.month || 12 == up->date.month) ) {
				if (up->leap_status & TRIMBLE_UTC_TIME)  
					pp->leap = LEAP_ADDSECOND;
				else
					pp->leap = LEAP_DELSECOND;
			}
			else if (up->leap_status)
				pp->leap = LEAP_NOWARNING;
		
			else {  /* UTC flag is not set:
				 * Receiver may have been reset, and lost
				 * its UTC almanac data */
				pp->leap = LEAP_NOTINSYNC;
#ifdef DEBUG
				if (debug)
					printf("TSIP_decode: UTC Almanac unavailable: %d\n",
					       mb(19));	
#endif
				refclock_report(peer, CEVNT_BADTIME);
				return false;
			}

			pp->nsec = (long) (getdbl((uint8_t *) &mb(3)) * NS_PER_S);
			up->date.year = (uint16_t)getint((uint8_t *) &mb(16));
			up->date.yearday = 0;
			up->date.hour = (uint8_t)mb(11);
			up->date.minute = (uint8_t)mb(12);
			up->date.second = (uint8_t)mb(13);
			up->date.month = (uint8_t)mb(15);
			up->date.monthday = (uint8_t)mb(14);
			caltogps(&up->date, up->UTC_offset, &up->week, &up->TOW);
			gpsweekadj(&up->week, up->build_week);
			gpstocal(up->week, up->TOW, up->UTC_offset, &up->date);
			up->UTC_offset = 0; /* don't re-use offset */
#ifdef DEBUG
			if (debug > 1)
				printf("TSIP_decode: unit %d: %02X #%d %02d:%02d:%02d.%09ld %02d/%02d/%04d UTC %02x %s\n",
				       up->unit, (u_int)(mb(0) & 0xff), event,
				       up->date.hour, up->date.minute, up->date.second, pp->nsec,
				       up->date.month, up->date.monthday, up->date.year,
				       (u_int)mb(19), *Tracking_Status[st]);
#endif
			return true;

		    case PACKET_8FAC:   
			if (up->rpt_cnt != LENCODE_8FAC)/* check length */
				break;

			if (up->got_time)
				return false;
#ifdef DEBUG
			if (debug > 1) {
				double lat, lon, alt;
				lat = getdbl((uint8_t *) &mb(36)) * R2D;
				lon = getdbl((uint8_t *) &mb(44)) * R2D;
				alt = getdbl((uint8_t *) &mb(52));

				printf("TSIP_decode: unit %d: Latitude: %03.4f Longitude: %03.4f Alt: %05.2f m\n",
				       up->unit, lat,lon,alt);
				printf("TSIP_decode: unit %d\n", up->unit);
			}
#endif
			if ( (getint((uint8_t *) &mb(10)) & 0x80) &&
			/* Avoid early announce: https://bugs.ntp.org/2773 */
			    (6 == up->date.month || 12 == up->date.month) )
				pp->leap = LEAP_ADDSECOND;  /* we ASSUME addsecond */
			else 
				pp->leap = LEAP_NOWARNING;

#ifdef DEBUG
			if (debug > 1) {
				printf("TSIP_decode: unit %d: 0x%02x leap %d\n",
				       up->unit, (u_int)(mb(0) & 0xff), pp->leap);

				printf("Receiver MODE: 0x%02X\n", (uint8_t)mb(1));
				if (mb(1) == 0x00)
					printf("                AUTOMATIC\n");
				if (mb(1) == 0x01)
					printf("                SINGLE SATELLITE\n");   
				if (mb(1) == 0x03)
					printf("                HORIZONTAL(2D)\n");
				if (mb(1) == 0x04)
					printf("                FULL POSITION(3D)\n");
				if (mb(1) == 0x05)
					printf("                DGPR REFERENCE\n");
				if (mb(1) == 0x06)
					printf("                CLOCK HOLD(2D)\n");
				if (mb(1) == 0x07)
					printf("                OVERDETERMINED CLOCK\n");

				printf("\n** Disciplining MODE 0x%02X:\n", (uint8_t)mb(2));
				if (mb(2) == 0x00)
					printf("                NORMAL\n");
				if (mb(2) == 0x01)
					printf("                POWER-UP\n");
				if (mb(2) == 0x02)
					printf("                AUTO HOLDOVER\n");
				if (mb(2) == 0x03)
					printf("                MANUAL HOLDOVER\n");
				if (mb(2) == 0x04)
					printf("                RECOVERY\n");
				if (mb(2) == 0x06)
					printf("                DISCIPLINING DISABLED\n");
			}
#endif   
			return false;

		    case PACKET_8FAB:
			/* Thunderbolt Primary Timing Packet */

			if (up->rpt_cnt != LENCODE_8FAB) /* check length */
				break;

			if (up->got_time)
				return false;

			up->UTC_offset = getint((uint8_t *) &mb(7));
			if (up->UTC_offset == 0){ /* Check UTC Offset */
#ifdef DEBUG
				if (debug)
					printf("TSIP_decode: UTC Offset Unknown\n");
#endif
				return false;
			}


			if ((mb(9) & 0x1d) == 0x0) {
				/* if we know the GPS time and the UTC offset,
				   we expect UTC timing information !!! */

				pp->leap = LEAP_NOTINSYNC;
				refclock_report(peer, CEVNT_BADTIME);
				return false;
			}

			up->TOW = (uint32_t)getlong((uint8_t *) &mb(1));
			up->week = (uint32_t)getint((uint8_t *) &mb(5));
			gpsweekadj(&up->week, up->build_week);
			gpstocal(up->week, up->TOW, up->UTC_offset, &up->date);
			pp->nsec = 0;

#ifdef DEBUG		
			if (debug > 1) {
				printf("\nTiming flag value of 0x%02X decoded to:\n", (u_int)mb(9));
				if ((mb(9) & 0x01) != 0)
					printf ("	Getting UTC time\n");
				else
					printf ("	Getting GPS time\n");
				if ((mb(9) & 0x02) != 0)
					printf ("	PPS is from UTC\n");
				else
					printf ("	PPS is from GPS\n");
				if ((mb(9) & 0x04) != 0)
					printf ("	Time is not Set\n");
				else
					printf ("	Time is Set\n");
				if ((mb(9) & 0x08) != 0)
					printf("	I don't have UTC info\n");
				else
					printf ("	I have UTC info\n");
				if ((mb(9) & 0x10) != 0)
					printf ("	Time is from USER\n\n");
				else
					printf ("	Time is from GPS\n\n");	

				printf("TSIP_decode: unit %d: %02X #%d TOW: %lu  week: %u  adj.t: %02d:%02d:%02d.0 %02d/%02d/%04d ",
				        up->unit, (u_int)(mb(0) & 0xff), event, up->TOW, up->week, 
					up->date.hour, up->date.minute, up->date.second,
					up->date.month, up->date.monthday, up->date.year);
			}
#endif
			return true;

		    default:
			/* Ignore Packet */
			return false;
		} /* switch */
	} /* if 8F packets */

	else if (up->rpt_buf[0] == (uint8_t)0x42) {
#ifdef DEBUG
		if (debug > 1)
			printf("0x42\n");
#endif
		return false;
	}
	else if (up->rpt_buf[0] == (uint8_t)0x43) {
#ifdef DEBUG
		if (debug > 1)
			printf("0x43\n");
#endif
		return false;
	}
	else if ((up->rpt_buf[0] == PACKET_41) & (up->type == CLK_THUNDERBOLT)){
#ifdef DEBUG
		if (debug > 1)
			printf("Undocumented 0x41 packet on Thunderbolt\n");
#endif
		return false;
	}
	else if ((up->rpt_buf[0] == PACKET_41A) & (up->type == CLK_ACUTIME)) {
#ifdef DEBUG
		printf("GPS TOW: %ld\n", (long)getlong((uint8_t *) &mb(0)));
		printf("GPS WN: %d\n", getint((uint8_t *) &mb(4)));
		printf("GPS UTC-GPS Offser: %ld\n", (long)getlong((uint8_t *) &mb(6)));
#endif
		return false;
	}

	/* Health Status for Acutime Receiver */
	else if ((up->rpt_buf[0] == PACKET_46) & (up->type == CLK_ACUTIME)) {
#ifdef DEBUG
		if (debug > 1)
		/* Status Codes */
			switch (mb(0)) {
			    case 0x00:
				printf ("Doing Position Fixes\n");
				break;
			    case 0x01:
				printf ("Do no have GPS time yet\n");
				break;
			    case 0x03:
				printf ("PDOP is too high\n");
				break;
			    case 0x08:
				printf ("No usable satellites\n");
				break;
			    case 0x09:
				printf ("Only 1 usable satellite\n");
				break;
			    case 0x0A:
				printf ("Only 2 usable satellites\n");
				break;
			    case 0x0B:
				printf ("Only 3 usable satellites\n");
				break;
			    case 0x0C:
				printf("The Chosen satellite is unusable\n");
				break;
			    default:
				printf("(Status code is invalid)\n");
			}
#endif
		/* Error Codes */
		if (mb(1) != 0)	{
			
			refclock_report(peer, CEVNT_BADTIME);
#ifdef DEBUG
			if (debug > 1) {
				if (mb(1) & 0x01)
					printf ("Signal Processor Error, reset unit.\n");
				if (mb(1) & 0x02)
					printf ("Alignment error, channel or chip 1, reset unit.\n");
				if (mb(1) & 0x03)
					printf ("Alignment error, channel or chip 2, reset unit.\n");
				if (mb(1) & 0x04)
					printf ("Antenna feed line fault (open or short)\n");
				if (mb(1) & 0x05)
					printf ("Excessive reference frequency error, refer to packet 0x2D and packet 0x4D documentation for further information\n");
			}
#endif
		}
		return false;
	}
	else if (up->rpt_buf[0] == 0x54)
		return false;

	else if (up->rpt_buf[0] == PACKET_6D) {
#ifdef DEBUG
		int sats;

		if ((mb(0) & 0x01) && (mb(0) & 0x02))
			printf("2d Fix Dimension\n");
		if (mb(0) & 0x04)
			printf("3d Fix Dimension\n");

		if (mb(0) & 0x08)
			printf("Fix Mode is MANUAL\n");
		else
			printf("Fix Mode is AUTO\n");
	
		sats = mb(0) & 0xF0;
		sats = sats >> 4;
		printf("Tracking %d Satellites\n", sats);
#endif
		return false;
	} /* else if not super packet */
	refclock_report(peer, CEVNT_BADREPLY);
#ifdef DEBUG
	printf("TSIP_decode: unit %d: bad packet %02x-%02x event %d len %d\n", 
	       up->unit, (u_int)(up->rpt_buf[0] & 0xff), (u_int)(mb(0) & 0xff),
	       event, (int)up->rpt_cnt);
#endif
	return false;
}

/*
 * trimble__receive - receive data from the serial interface
 */

static void
trimble_receive (
	struct peer * peer
	)
{
	struct trimble_unit *up;
	struct refclockproc *pp;

	/*
	 * Initialize pointers and read the timecode and timestamp.
	 */
	pp = peer->procptr;
	up = pp->unitptr;
		
	/* startup may have caused a false edge, so wait for first HW_poll() */
	if (pp->polls <= 1 && !(pp->sloppyclockflag & CLK_FLAG2))
		return;

	if (!TSIP_decode(peer))
		return;
	
	if (up->got_time) 
		return; /* time is already saved */

	/* save time data for this poll interval */
	up->got_time = true;
	pp->year = up->date.year;
	pp->day = up->date.yearday;
	pp->hour = up->date.hour;
	pp->minute = up->date.minute;
	pp->second = up->date.second;
#ifdef DEBUG
	if (debug) 
		printf(
			"trimble_receive: unit %d: %4d %03d %02d:%02d:%02d.%09ld\n",
			up->unit, pp->year, pp->day, pp->hour, pp->minute, 
			pp->second, pp->nsec);
#endif

	/*
	 * Process the sample
	 * Generate timecode: YYYY DoY HH:MM:SS.microsec 
	 * report and process 
	 */

	snprintf(pp->a_lastcode, sizeof(pp->a_lastcode),
		 "%4d %03d %02d:%02d:%02d.%09ld",
		 pp->year, pp->day,
		 pp->hour,pp->minute, pp->second, pp->nsec); 
	pp->lencode = 24;

	if (!refclock_process(pp)) {
		refclock_report(peer, CEVNT_BADTIME);

#ifdef DEBUG
		printf("trimble_receive: unit %d: refclock_process failed!\n",
		       up->unit);
#endif
		return;
	}

	record_clock_stats(peer, pp->a_lastcode); 

#ifdef DEBUG
	if (debug)
		printf("trimble_receive: unit %d: %s\n",
		       up->unit, prettydate(pp->lastrec));
#endif
	if (pp->hour == 0 && up->week > up->build_week + 1000)
		msyslog(LOG_WARNING, "Trimble(%d) current GPS week number (%u) is more than 1000 weeks past ntpd's build date (%u), please update",
		        up->unit, up->week, up->build_week);

	pp->lastref = pp->lastrec;
	refclock_receive(peer);
}


/*
 * trimble_poll - called by the transmit procedure
 *
 */
static void
trimble_poll (
	int unit,
	struct peer *peer
	)
{
	struct trimble_unit *up;
	struct refclockproc *pp;
	
	pp = peer->procptr;
	up = pp->unitptr;

	pp->polls++;

	if (pp->polls > 2 && !up->got_time)
		refclock_report(peer, CEVNT_TIMEOUT);

	up->got_time = false;
	
#ifdef DEBUG
	if (debug)
		printf("trimble_poll: unit %d: polling %s\n", unit,
		       (pp->sloppyclockflag & CLK_FLAG2) ? 
		       "auto-report packet" : "event");
#endif 

	if (pp->sloppyclockflag & CLK_FLAG2) 
		return;  /* using auto-report packets */

	if(up->type == CLK_PRAECIS) {
		if(write(peer->procptr->io.fd,"SPSTAT\r\n",8) < 0)
			msyslog(LOG_ERR, "Trimble(%d) write: %m:",unit);
		else {
			praecis_msg = true;
			return;
		}
	}

	if (HW_poll(pp)) 
		refclock_report(peer, CEVNT_FAULT); 
}

static void
praecis_parse (
	struct recvbuf *rbufp,
	struct peer *peer
	)
{
	static char buf[100];
	static size_t p = 0;
	struct refclockproc *pp;

	pp = peer->procptr;

	memcpy(buf+p,rbufp->recv_space.X_recv_buffer, rbufp->recv_length);
	p += rbufp->recv_length;

	if(buf[p-2] == '\r' && buf[p-1] == '\n') {
		buf[p-2] = '\0';
		record_clock_stats(peer, buf);

		p = 0;
		praecis_msg = false;

		if (HW_poll(pp))
			refclock_report(peer, CEVNT_FAULT);

	}
}

static void
trimble_io (
	struct recvbuf *rbufp
	)
{
	/*
	 * Initialize pointers and read the timecode and timestamp.
	 */
	struct trimble_unit *up;
	struct refclockproc *pp;
	struct peer *peer;

	char * c, * d;

	peer = rbufp->recv_peer;
	pp = peer->procptr;
	up = pp->unitptr;

	if(up->type == CLK_PRAECIS) {
		if(praecis_msg) {
			praecis_parse(rbufp,peer);
			return;
		}
	}

	c = (char *) &rbufp->recv_space;
	d = c + rbufp->recv_length;
		
	while (c != d) {

		/* Build time packet */
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
				break;
			}
			break;

		    case TSIP_PARSED_DATA:
			if (*c == DLE)
				up->rpt_status = TSIP_PARSED_DLE_2;
			else 
				mb(up->rpt_cnt++) = *c;
			break;

		    case TSIP_PARSED_DLE_2:
			if (*c == DLE) {
				up->rpt_status = TSIP_PARSED_DATA;
				/* prevent overrun - should never happen */
				if (up->rpt_cnt < BMAX - 2)
					mb(up->rpt_cnt++) = *c;
			}
			else if (*c == ETX) 
				up->rpt_status = TSIP_PARSED_FULL;
			else 	{
				/* error: start new report packet */
				up->rpt_status = TSIP_PARSED_DLE_1;
				up->rpt_buf[0] = *c;
			}
			break;

		    case TSIP_PARSED_FULL:
		    case TSIP_PARSED_EMPTY:
		    default:
			if ( *c != DLE)
				up->rpt_status = TSIP_PARSED_EMPTY;
			else 
				up->rpt_status = TSIP_PARSED_DLE_1;
			break;
		}
		
		c++;

		if (up->rpt_status == TSIP_PARSED_DLE_1) {
			up->rpt_cnt = 0;
			if (pp->sloppyclockflag & CLK_FLAG2) 
				/* stamp it */
				get_systime(&pp->lastrec);
		}
		else if (up->rpt_status == TSIP_PARSED_EMPTY)
			up->rpt_cnt = 0;

		else if (up->rpt_cnt > sizeof(up->rpt_buf)) 
			up->rpt_status =TSIP_PARSED_EMPTY;

		if (up->rpt_status == TSIP_PARSED_FULL) 
			trimble_receive(peer);

	} /* while chars in buffer */
}


/*
 * Trigger the Trimble's event input, which is driven off the RTS
 *
 * Take a system time stamp to match the GPS time stamp.
 *
 */
static bool
HW_poll (
	struct refclockproc * pp 	/* pointer to unit structure */
	)
{	
	int x;	/* state before & after RTS set */
	struct trimble_unit *up;

	up = pp->unitptr;

	/* read the current status, so we put things back right */
	if (ioctl(pp->io.fd, TIOCMGET, &x) < 0) {
		DPRINTF(1, ("Trimble HW_poll: unit %d: GET %m\n",
			up->unit));
		msyslog(LOG_ERR, "Trimble(%d) HW_poll: ioctl(fd,GET): %m", 
			up->unit);
		return true;
	}
  
	x |= TIOCM_RTS;        /* turn on RTS  */

	/* Edge trigger */
	if (up->type == CLK_ACUTIME)
		IGNORE(write (pp->io.fd, "", 1));
		
	if (ioctl(pp->io.fd, TIOCMSET, &x) < 0) { 
#ifdef DEBUG
		if (debug)
			printf("Trimble HW_poll: unit %d: SET \n", up->unit);
#endif
		msyslog(LOG_ERR,
			"Trimble(%d) HW_poll: ioctl(fd, SET, RTS_on): %m", 
			up->unit);
		return true;
	}

	x &= ~TIOCM_RTS;        /* turn off RTS  */
	
	/* poll timestamp */
	get_systime(&pp->lastrec);

	if (ioctl(pp->io.fd, TIOCMSET, &x) == -1) {
#ifdef DEBUG
		if (debug)
			printf("Trimble HW_poll: unit %d: UNSET \n", up->unit);
#endif
		msyslog(LOG_ERR,
			"Trimble(%d) HW_poll: ioctl(fd, UNSET, RTS_off): %m", 
			up->unit);
		return true;
	}

	return false;
}

/*
 * copy/swap a big-endian palisade double into a host double
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
 * copy/swap a big-endian palisade short into a host short
 */
static short
getint (
	uint8_t *bp
	)
{
	u_short us;

	memcpy(&us, bp, sizeof(us));
	return (short)ntohs(us);
}


/*
 * copy/swap a big-endian palisade 32-bit int into a host 32-bit int
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