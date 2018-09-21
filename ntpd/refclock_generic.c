/*
 * generic reference clock driver for several DCF/GPS/MSF/... receivers
 *
 * PPS notes:
 *   On systems that support PPSAPI (RFC 2783) PPSAPI is the
 *   preferred interface.
 *
 * Copyright (c) 1989-2015 by Frank Kardel <kardel@ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Note: some subtypes are obsolete and could probably stand to be removed
 * next time this code gets serious attention.  In particular, subtypes 9 and 10
 * support the Trimble SVeeSix, which was discontinued before 2003. Related
 * code in the parse library could also be dropped.
 * Also see subtypes 3 and 4, for which no information in use since 1999 and
 * 2001 respectively can be found on the web.
 *
 * WARNING: Most modes of this driver depend on the system clock for
 * year disambiguation.  They will thus not be usable for recovery if
 * the system clock is trashed. The only exceptions are the Scheitzer 240x
 * and the two Trimble devices.
 */

#include "config.h"
#include "ntp_types.h"
#include "timespecops.h"

/*
 * This driver currently provides the support for
 *   - Meinberg receiver DCF77 PZF535 (TCXO version)        (DCF)
 *   - Meinberg receiver DCF77 PZF535 (OCXO version)        (DCF)
 *   - Meinberg receiver DCF77 PZF509                       (DCF)
 *   - Meinberg receiver DCF77 AM receivers (e.g. C51)      (DCF)
 *   - IGEL CLOCK                                           (DCF)
 *   - ELV DCF7000                                          (DCF)
 *   - Schmid clock                                         (DCF)
 *   - Conrad DCF77 receiver module                         (DCF)
 *   - FAU DCF77 NTP receiver (TimeBrick)                   (DCF)
 *   - WHARTON 400A Series clock                            (DCF)
 *
 *   - Meinberg GPS receivers                               (GPS)
 *   - Trimble (TSIP and TAIP protocol)                     (GPS)
 *
 *   - RCC8000 MSF Receiver                                 (MSF)
 *   - VARITEXT clock                                       (MSF)
 */

/*
 * Meinberg receivers are usually connected via a
 * 9600/7E1 or 19200/8N1 serial line.
 *
 * The Meinberg GPS receivers also have a special NTP time stamp
 * format. The firmware release is Uni-Erlangen.
 *
 * Meinberg generic receiver setup:
 *      output time code every second
 *      Baud rate 9600 7E2S
 *
 * Meinberg GPS receiver setup:
 *      output time code every second
 *      Baudrate 19200 8N1
 *
 * This software supports the standard data formats used
 * in Meinberg receivers.
 *
 * Special software versions are only sensible for the
 * oldest GPS receiver, GPS16x. For newer receiver types
 * the output string format can be configured at the device,
 * and the device name is generally GPSxxx instead of GPS16x.
 *
 * Meinberg can be reached via: http://www.meinberg.de/
 */

#include "ntpd.h"
#include "ntp_refclock.h"
#include "ntp_calendar.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#include <unistd.h>

#include <termios.h>
#define TTY_GETATTR(_FD_, _ARG_) tcgetattr((_FD_), (_ARG_))
#define TTY_SETATTR(_FD_, _ARG_) tcsetattr((_FD_), TCSANOW, (_ARG_))

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#ifdef HAVE_PPSAPI
# include "ppsapi_timepps.h"
# include "refclock_pps.h"
#endif

#ifdef HAVE_LINUX_SERIAL_H
#  include <linux/serial.h>
#endif

#define BUFFER_SIZE(_BUF, _PTR)     ((size_t)((_BUF) + sizeof(_BUF) - (_PTR)))
#define BUFFER_SIZES(_BUF, _PTR, _SZ) ((int)((_BUF) + (_SZ) - (_PTR)))

/*
 * COND_DEF can be conditionally defined as DEF or 0. If defined as DEF
 * then some more parse-specific variables are flagged to be printed with
 * "ntpq -c cv <assid>". This can be lengthy, so by default COND_DEF
 * should be defined as 0.
 */
#if 0
# define COND_DEF   DEF   // enable this for testing
#else
# define COND_DEF   0     // enable this by default
#endif

#include "ntp_io.h"
#include "ntp_stdlib.h"

#include "parse.h"
#include "mbg_gps166.h"
#include "trimble.h"
#include "binio.h"
#include "ascii.h"
#include "recvbuff.h"

#define VERSION "4.81 2009/05/01 10:15:29"

/**===========================================================================
 ** external interface to ntp mechanism
 **/

static	bool	parse_start	(int, struct peer *);
static	void	parse_shutdown	(struct refclockproc *);
static	void	parse_poll	(int, struct peer *);
static	void	parse_control	(int, const struct refclockstat *, struct refclockstat *, struct peer *);

struct	refclock refclock_parse = {
	"GENERIC",			/* basename of driver */
	parse_start,                    /* start up driver */
	parse_shutdown,                 /* shut down driver */
	parse_poll,          		/* transmit poll message */
	parse_control,       		/* control settings */
	NULL,				/* init */
	NULL,				/* timer */
};

/*
 * Definitions
 */
#define PARSEDEVICE	"/dev/refclock-%u" /* device to open %d is unit number */
#define PARSEPPSDEVICE	"/dev/refclockpps-%u" /* optional pps device to open %d is unit number */

#undef ABS
#define ABS(_X_) (((_X_) < 0) ? -(_X_) : (_X_))

#ifdef HAVE_PPSAPI
# define PARSE_HARDPPS_DISABLE 0
# define PARSE_HARDPPS_ENABLE  1
#endif

/**===========================================================================
 ** function vector for dynamically binding io handling mechanism
 **/

struct parseunit;		/* to keep inquiring minds happy */

typedef struct bind
{
  const char *bd_description;	                                /* name of type of binding */
  int	(*bd_init)     (struct parseunit *);			/* initialize */
  void	(*bd_end)      (struct parseunit *);			/* end */
  bool  (*bd_setcs)    (struct parseunit *, parsectl_t *);	/* set character size */
  int	(*bd_disable)  (struct parseunit *);			/* disable */
  int	(*bd_enable)   (struct parseunit *);			/* enable */
  int	(*bd_getfmt)   (struct parseunit *, parsectl_t *);	/* get format */
  int	(*bd_setfmt)   (struct parseunit *, parsectl_t *);	/* setfmt */
  int	(*bd_timecode) (struct parseunit *, parsectl_t *);	/* get time code */
  void	(*bd_receive)  (struct recvbuf *);			/* receive operation */
  int	(*bd_io_input) (struct recvbuf *);			/* input operation */
} bind_t;

#define PARSE_END(_X_)			(*(_X_)->binding->bd_end)(_X_)
#define PARSE_SETCS(_X_, _CS_)		(*(_X_)->binding->bd_setcs)(_X_, _CS_)
#define PARSE_GETFMT(_X_, _DCT_)	(*(_X_)->binding->bd_getfmt)(_X_, _DCT_)
#define PARSE_SETFMT(_X_, _DCT_)	(*(_X_)->binding->bd_setfmt)(_X_, _DCT_)
#define PARSE_GETTIMECODE(_X_, _DCT_)	(*(_X_)->binding->bd_timecode)(_X_, _DCT_)

/*
 * special handling flags
 */
#define PARSE_F_PPSONSECOND	0x00000001 /* PPS pulses are on second */
#define PARSE_F_POWERUPTRUST	0x00000100 /* POWERUP state ist trusted for */
                                           /* trusttime after SYNC was seen */
/**===========================================================================
 ** error message regression handling
 **
 ** there are quite a few errors that can occur in rapid succession such as
 ** noisy input data or no data at all. in order to reduce the amount of
 ** syslog messages in such case, we are using a backoff algorithm. We limit
 ** the number of error messages of a certain class to 1 per time unit. if a
 ** configurable number of messages is displayed that way, we move on to the
 ** next time unit / count for that class. a count of messages that have been
 ** suppressed is held and displayed whenever a corresponding message is
 ** displayed. the time units for a message class will also be displayed.
 ** whenever an error condition clears we reset the error message state,
 ** thus we would still generate much output on pathological conditions
 ** where the system oscillates between OK and NOT OK states. coping
 ** with that condition is currently considered too complicated.
 **/

#define ERR_ALL	        (unsigned)~0	/* "all" errors */
#define ERR_BADDATA	(unsigned)0	/* unusable input data/conversion errors */
#define ERR_NODATA	(unsigned)1	/* no input data */
#define ERR_BADIO	(unsigned)2	/* read/write/select errors */
#define ERR_BADSTATUS	(unsigned)3	/* unsync states */
#define ERR_INTERNAL	(unsigned)5	/* internal error */
#define ERR_CNT		(unsigned)(ERR_INTERNAL+1)

#define ERR(_X_)	if (list_err(parse, (_X_)))

struct errorregression
{
	unsigned long err_count;	/* number of repetitions per class */
	unsigned long err_delay;	/* minimum delay between messages */
};

static struct errorregression
err_baddata[] =			/* error messages for bad input data */
{
	{ 1,       0 },		/* output first message immediately */
	{ 5,      60 },		/* output next five messages in 60 second intervals */
	{ 3,    SECSPERHR },	/* output next 3 messages in hour intervals */
	{ 0, 12 * SECSPERHR }	/* repeat messages only every 12 hours */
};

static struct errorregression
err_nodata[] =			/* error messages for missing input data */
{
	{ 1,       0 },		/* output first message immediately */
	{ 5,      60 },		/* output next five messages in 60 second intervals */
	{ 3,    SECSPERHR },	/* output next 3 messages in hour intervals */
	{ 0, 12 * SECSPERHR }	/* repeat messages only every 12 hours */
};

static struct errorregression
err_badstatus[] =		/* unsynchronized state messages */
{
	{ 1,       0 },		/* output first message immediately */
	{ 5,      60 },		/* output next five messages in 60 second intervals */
	{ 3,    SECSPERHR },	/* output next 3 messages in hour intervals */
	{ 0, 12 * SECSPERHR }	/* repeat messages only every 12 hours */
};

static struct errorregression
err_badio[] =			/* io failures (bad reads, selects, ...) */
{
	{ 1,       0 },		/* output first message immediately */
	{ 5,      60 },		/* output next five messages in 60 second intervals */
	{ 5,    SECSPERHR },	/* output next 3 messages in hour intervals */
	{ 0, 12 * SECSPERHR }	/* repeat messages only every 12 hours */
};

static struct errorregression
err_badevent[] =		/* non nominal events */
{
	{ 20,      0 },		/* output first message immediately */
	{ 6,      60 },		/* output next five messages in 60 second intervals */
	{ 5,    SECSPERHR },	/* output next 3 messages in hour intervals */
	{ 0, 12 * SECSPERHR }	/* repeat messages only every 12 hours */
};

static struct errorregression
err_internal[] =		/* really bad things - basically coding/OS errors */
{
	{ 0,       0 },		/* output all messages immediately */
};

static struct errorregression *
err_tbl[] =
{
	err_baddata,
	err_nodata,
	err_badio,
	err_badstatus,
	err_badevent,
	err_internal
};

struct errorinfo
{
	uptime_t	err_started;	/* begin time (ntp) of error condition */
	uptime_t	err_last;	/* last time (ntp) error occurred */
	unsigned long err_cnt;		/* number of error repetitions */
	unsigned long err_suppressed;	/* number of suppressed messages */
	struct errorregression *err_stage; /* current error stage */
};

/**===========================================================================
 ** refclock instance data
 **/

struct parseunit
{
	/*
	 * NTP management
	 */
	struct peer         *peer;		/* backlink to peer structure - refclock inactive if 0  */
	struct refclockproc *generic;		/* backlink to refclockproc structure */

	/*
	 * PARSE io
	 */
	bind_t	     *binding;	        /* io handling binding */

	/*
	 * parse state
	 */
	parse_t	      parseio;	        /* io handling structure (user level parsing) */

	/*
	 * type specific parameters
	 */
	struct parse_clockinfo   *parse_type;	        /* link to clock description */

	/*
	 * clock state handling/reporting
	 */
	uint8_t	      	      flags;	        /* flags (leap_control) */
	uptime_t	      lastchange;       /* time (ntp) when last state change accured */
	unsigned long	      statetime[CEVNT_MAX+1]; /* accumulated time of clock states */
	unsigned long         pollneeddata; 	/* current_time(!=0) for receive sample expected in PPS mode */
	unsigned short	      lastformat;       /* last format used */
	unsigned long         lastsync;		/* time (ntp) when clock was last seen fully synchronized */
        unsigned long         maxunsync;        /* max time in seconds a receiver is trusted after losing synchronisation */
        double		      ppsphaseadjust;   /* phase adjustment of PPS time stamp */
        unsigned long         lastmissed;       /* time (ntp) when poll didn't get data (powerup heuristic) */
	unsigned long         ppsserial;        /* magic cookie for ppsclock serials (avoids stale ppsclock data) */
	int	      ppsfd;	        /* fd to ise for PPS io */
#ifdef HAVE_PPSAPI
        int           hardppsstate;     /* current hard pps state */
	struct refclock_ppsctl ppsctl;      /* PPSAPI structure */
#endif
	parsetime_t   timedata;		/* last (parse module) data */
	void         *localdata;        /* optional local, receiver-specific data */
        unsigned long localstate;       /* private local state */
	struct errorinfo errors[ERR_CNT];  /* error state table for suppressing excessive error messages */
	struct ctl_var *kv;		/* additional pseudo variables */
	uptime_t	laststatistic;	/* time when staticstics where output */
};


/**===========================================================================
 ** Clockinfo section all parameter for specific clock types
 ** includes NTP parameters, TTY parameters and IO handling parameters
 **/

static	void	poll_dpoll	(struct parseunit *);
static	void	poll_poll	(struct peer *);
static	bool	poll_init	(struct parseunit *);

typedef struct poll_info
{
	unsigned long      rate;	/* poll once every "rate" seconds - 0 off */
	const char 	   *string;	/* string to send for polling */
	unsigned long      count;	/* number of characters in string */
} poll_info_t;

#define NO_CL_FLAGS	0
#define NO_POLL		0
#define NO_INIT		0
#define NO_END		0
#define NO_EVENT	0
#define NO_LCLDATA	0
#define NO_MESSAGE	0

#define DCF_ID		"DCF"	/* generic DCF */
#define DCF_A_ID	"DCFa"	/* AM demodulation */
#define DCF_P_ID	"DCFp"	/* pseudo random phase shift */
#define GPS_ID		"GPS"	/* GPS receiver */
#define MSF_ID		"MSF"	/* MSF receiver */

#define DCF_TYPE		CTL_SST_TS_LF
#define GPS_TYPE		CTL_SST_TS_UHF

/*
 * receiver specific constants
 */
#define MBG_SPEED		(B9600)
#define MBG_CFLAG		(CS7|PARENB|CREAD|CLOCAL|HUPCL|CSTOPB)
#define MBG_IFLAG		(IGNBRK|IGNPAR|ISTRIP)
#define MBG_OFLAG		0
#define MBG_LFLAG		0
#define MBG_FLAGS               PARSE_F_PPSONSECOND

/*
 * Meinberg DCF77 receivers
 */
#define	DCFUA31_ROOTDELAY	0.0  /* 0 */
#define	DCFUA31_BASEDELAY	0.010  /* 10.7421875ms: 10 ms (+/- 3 ms) */
#define DCFUA31_NAME		"MEINBERG_C51"
#define	DCFUA31_DESCRIPTION	"Meinberg DCF77 C51 or compatible"
#define DCFUA31_MAXUNSYNC       60*30       /* only trust clock for 1/2 hour */
#define DCFUA31_SPEED		MBG_SPEED
#define DCFUA31_CFLAG           MBG_CFLAG
#define DCFUA31_IFLAG           MBG_IFLAG
#define DCFUA31_OFLAG           MBG_OFLAG
#define DCFUA31_LFLAG           MBG_LFLAG
#define DCFUA31_SAMPLES		5
#define DCFUA31_KEEP		3
#define DCFUA31_FORMAT		"Meinberg Standard"

/*
 * Meinberg DCF PZF535/TCXO (FM/PZF) receiver
 */
#define	DCFPZF535_ROOTDELAY	0.0
#define	DCFPZF535_BASEDELAY	0.001968  /* 1.968ms +- 104us (oscilloscope) - relative to start (end of STX) */
#define	DCFPZF535_NAME		"MEINBERG_5XX"
#define	DCFPZF535_DESCRIPTION	"Meinberg DCF PZF 535/509 / TCXO"
#define DCFPZF535_MAXUNSYNC     60*60*12           /* only trust clock for 12 hours
						    * @ 5e-8df/f we have accumulated
						    * at most 2.16 ms (thus we move to
						    * NTP synchronisation */
#define DCFPZF535_SPEED		MBG_SPEED
#define DCFPZF535_CFLAG         MBG_CFLAG
#define DCFPZF535_IFLAG         MBG_IFLAG
#define DCFPZF535_OFLAG         MBG_OFLAG
#define DCFPZF535_LFLAG         MBG_LFLAG
#define DCFPZF535_SAMPLES	       5
#define DCFPZF535_KEEP		       3
#define DCFPZF535_FORMAT	"Meinberg Standard"

/*
 * Meinberg DCF PZF535/OCXO receiver
 */
#define	DCFPZF535OCXO_ROOTDELAY	0.0
#define	DCFPZF535OCXO_BASEDELAY	0.001968 /* 1.968ms +- 104us (oscilloscope) - relative to start (end of STX) */
#define	DCFPZF535OCXO_NAME	"MEINBERG_5XX"
#define	DCFPZF535OCXO_DESCRIPTION "Meinberg DCF PZF 535/509 / OCXO"
#define DCFPZF535OCXO_MAXUNSYNC     60*60*96       /* only trust clock for 4 days
						    * @ 5e-9df/f we have accumulated
						    * at most an error of 1.73 ms
						    * (thus we move to NTP synchronisation) */
#define DCFPZF535OCXO_SPEED	    MBG_SPEED
#define DCFPZF535OCXO_CFLAG         MBG_CFLAG
#define DCFPZF535OCXO_IFLAG         MBG_IFLAG
#define DCFPZF535OCXO_OFLAG         MBG_OFLAG
#define DCFPZF535OCXO_LFLAG         MBG_LFLAG
#define DCFPZF535OCXO_SAMPLES		   5
#define DCFPZF535OCXO_KEEP	           3
#define DCFPZF535OCXO_FORMAT	    "Meinberg Standard"

/*
 * Meinberg GPS receivers
 */
static	void	gps16x_message	 (struct parseunit *, parsetime_t *);
static  bool    gps16x_poll_init (struct parseunit *);

#define	GPS16X_ROOTDELAY	0.0         /* nothing here */
#define	GPS16X_BASEDELAY	0.001968         /* XXX to be fixed ! 1.968ms +- 104us (oscilloscope) - relative to start (end of STX) */
#define	GPS16X_NAME             "GPS_MEINBERG"
#define	GPS16X_DESCRIPTION      "Meinberg GPS receiver"
#define GPS16X_MAXUNSYNC        60*60*96       /* only trust clock for 4 days
						* @ 5e-9df/f we have accumulated
						* at most an error of 1.73 ms
						* (thus we move to NTP synchronisation) */
#define GPS16X_SPEED		B19200
#define GPS16X_CFLAG            (CS8|CREAD|CLOCAL|HUPCL)
#define GPS16X_IFLAG            (IGNBRK|IGNPAR)
#define GPS16X_OFLAG            MBG_OFLAG
#define GPS16X_LFLAG            MBG_LFLAG
#define GPS16X_POLLRATE	6
#define GPS16X_POLLCMD	""
#define GPS16X_CMDSIZE	0

static poll_info_t gps16x_pollinfo = { GPS16X_POLLRATE, GPS16X_POLLCMD, GPS16X_CMDSIZE };

#define GPS16X_INIT		gps16x_poll_init
#define GPS16X_POLL	        0
#define GPS16X_END		0
#define GPS16X_DATA		((void *)(&gps16x_pollinfo))
#define GPS16X_MESSAGE		gps16x_message
#define GPS16X_ID		GPS_ID
#define GPS16X_FORMAT		"Meinberg GPS Extended"
#define GPS16X_SAMPLES		5
#define GPS16X_KEEP		3

/*
 * ELV DCF7000 Wallclock-Receiver/Switching Clock (Kit)
 *
 * This is really not the hottest clock - but before you have nothing ...
 */
#define DCF7000_ROOTDELAY	0.0 /* 0 */
#define DCF7000_BASEDELAY	0.405 /* slow blow */
#define DCF7000_NAME		"ELV_DCF7000"
#define DCF7000_DESCRIPTION	"ELV DCF7000"
#define DCF7000_MAXUNSYNC	(60*5) /* sorry - but it just was not build as a clock */
#define DCF7000_SPEED		(B9600)
#define DCF7000_CFLAG           (CS8|CREAD|PARENB|PARODD|CLOCAL|HUPCL)
#define DCF7000_IFLAG		(IGNBRK)
#define DCF7000_OFLAG		0
#define DCF7000_LFLAG		0
#define DCF7000_SAMPLES		5
#define DCF7000_KEEP		3
#define DCF7000_FORMAT		"ELV DCF7000"

/*
 * Schmid DCF Receiver Kit
 *
 * When the WSDCF clock is operating optimally we want the primary clock
 * distance to come out at 300 ms.  Thus, peer.distance in the WSDCF peer
 * structure is set to 290 ms and we compute delays which are at least
 * 10 ms long.  The following are 290 ms and 10 ms expressed in unsigned fp format
 */
#define WS_POLLRATE	1	/* every second - watch interdependency with poll routine */
#define WS_POLLCMD	"\163"
#define WS_CMDSIZE	1

static poll_info_t wsdcf_pollinfo = { WS_POLLRATE, WS_POLLCMD, WS_CMDSIZE };

#define WSDCF_INIT		poll_init
#define WSDCF_POLL		poll_dpoll
#define WSDCF_END		0
#define WSDCF_DATA		((void *)(&wsdcf_pollinfo))
#define	WSDCF_ROOTDELAY		0.0	/* 0 */
#define	WSDCF_BASEDELAY	 	0.010	/*  ~  10ms */
#define WSDCF_NAME		"WSDCF"
#define WSDCF_DESCRIPTION	"WS/DCF Receiver"
#define WSDCF_FORMAT		"Schmid"
#define WSDCF_MAXUNSYNC		(60*60)	/* assume this beast hold at 1 h better than 2 ms XXX-must verify */
#define WSDCF_SPEED		(B1200)
#define WSDCF_CFLAG		(CS8|CREAD|CLOCAL)
#define WSDCF_IFLAG		0
#define WSDCF_OFLAG		0
#define WSDCF_LFLAG		0
#define WSDCF_SAMPLES		5
#define WSDCF_KEEP		3

/*
 * RAW DCF77 - input of DCF marks via RS232 - many variants
 */
#define RAWDCF_FLAGS		0
#define RAWDCF_ROOTDELAY	0.0 /* 0 */
#define RAWDCF_BASEDELAY	0.258
#define RAWDCF_FORMAT		"RAW DCF77 Timecode"
#define RAWDCF_MAXUNSYNC	(0) /* sorry - its a true receiver - no signal - no time */
#define RAWDCF_SPEED		(B50)
#define RAWDCF_CFLAG            (CS8|CREAD|CLOCAL|PARENB)
#define RAWDCF_IFLAG		(IGNPAR)
#define RAWDCF_OFLAG		0
#define RAWDCF_LFLAG		0
#define RAWDCF_SAMPLES		20
#define RAWDCF_KEEP		12
#define RAWDCF_INIT		0

/*
 * RAW DCF variants
 */
/*
 * Conrad receiver
 *
 * simplest (cheapest) DCF clock - e. g. DCF77 receiver by Conrad
 * (~40DM - roughly $30 ) followed by a level converter for RS232
 */
#define CONRAD_BASEDELAY	0.292 /* Conrad receiver @ 50 Baud on a Sun */
#define CONRAD_NAME		"RAWDCF_CONRAD"
#define CONRAD_DESCRIPTION	"RAW DCF77 CODE (Conrad DCF77 receiver module)"

/* Gude Analog- und Digitalsystem GmbH 'Expert mouseCLOCK USB v2.0' */
#define GUDE_EMC_USB_V20_SPEED            (B4800)
#define GUDE_EMC_USB_V20_BASEDELAY        0.425 /* USB serial<->USB converter FTDI232R */
#define GUDE_EMC_USB_V20_NAME             "RAWDCF_MOUSECLOCK"
#define GUDE_EMC_USB_V20_DESCRIPTION      "RAW DCF77 CODE (Expert mouseCLOCK USB v2.0)"

/*
 * TimeBrick receiver
 */
#define TIMEBRICK_BASEDELAY	0.210 /* TimeBrick @ 50 Baud on a Sun */
#define TIMEBRICK_NAME		"RAWDCF_TIMEBRICK"
#define TIMEBRICK_DESCRIPTION	"RAW DCF77 CODE (TimeBrick)"

/*
 * IGEL:clock receiver
 */
#define IGELCLOCK_BASEDELAY	0.258 /* IGEL:clock receiver */
#define IGELCLOCK_NAME		"RAWDCF_IGEL"
#define IGELCLOCK_DESCRIPTION	"RAW DCF77 CODE (IGEL:clock)"
#define IGELCLOCK_SPEED		(B1200)
#define IGELCLOCK_CFLAG		(CS8|CREAD|HUPCL|CLOCAL)

/*
 * RAWDCF receivers that need to be powered from DTR
 * (like Expert mouse clock)
 */
static	bool	rawdcf_init_1	(struct parseunit *);
#define RAWDCFDTRSET_NAME		"RAW_DCF77"
#define RAWDCFDTRSET_DESCRIPTION	"RAW DCF77 CODE (DTR SET/RTS CLR)"
#define RAWDCFDTRSET75_DESCRIPTION	"RAW DCF77 CODE (DTR SET/RTS CLR @ 75 baud)"
#define RAWDCFDTRSET_INIT 		rawdcf_init_1

/*
 * RAWDCF receivers that need to be powered from
 * DTR CLR and RTS SET
 */
static	bool	rawdcf_init_2	(struct parseunit *);
#define RAWDCFDTRCLRRTSSET_NAME		"RAW_DCF77"
#define RAWDCFDTRCLRRTSSET_DESCRIPTION	"RAW DCF77 CODE (DTR CLR/RTS SET)"
#define RAWDCFDTRCLRRTSSET75_DESCRIPTION "RAW DCF77 CODE (DTR CLR/RTS SET @ 75 baud)"
#define RAWDCFDTRCLRRTSSET_INIT	rawdcf_init_2

/*
 * Trimble GPS receivers (TAIP and TSIP protocols)
 */
#ifndef TRIM_POLLRATE
#define TRIM_POLLRATE	0	/* only true direct polling */
#endif

#define TRIM_TAIPPOLLCMD	">SRM;FR_FLAG=F;EC_FLAG=F<>QTM<"
#define TRIM_TAIPCMDSIZE	(sizeof(TRIM_TAIPPOLLCMD)-1)

static poll_info_t trimbletaip_pollinfo = { TRIM_POLLRATE, TRIM_TAIPPOLLCMD, TRIM_TAIPCMDSIZE };
static	bool	trimbletaip_init	(struct parseunit *);
static	void	trimbletaip_event	(struct parseunit *, int);

/* query time & UTC correction data */
static char tsipquery[] = { DLE, 0x21, DLE, ETX, DLE, 0x2F, DLE, ETX };

static poll_info_t trimbletsip_pollinfo = { TRIM_POLLRATE, tsipquery, sizeof(tsipquery) };
static	bool	trimbletsip_init	(struct parseunit *);
static	void	trimbletsip_end   	(struct parseunit *);
static	void	trimbletsip_message	(struct parseunit *, parsetime_t *);
static	void	trimbletsip_event	(struct parseunit *, int);

#define TRIMBLETSIP_IDLE_TIME	    (300) /* 5 minutes silence at most */
#define TRIMBLE_RESET_HOLDOFF       TRIMBLETSIP_IDLE_TIME

#define TRIMBLETAIP_SPEED	    (B4800)
#define TRIMBLETAIP_CFLAG           (CS8|CREAD|CLOCAL)
#define TRIMBLETAIP_IFLAG           (BRKINT|IGNPAR|ISTRIP|ICRNL|IXON)
#define TRIMBLETAIP_OFLAG           (OPOST|ONLCR)
#define TRIMBLETAIP_LFLAG           (0)

#define TRIMBLETSIP_SPEED	    (B9600)
#define TRIMBLETSIP_CFLAG           (CS8|CLOCAL|CREAD|PARENB|PARODD)
#define TRIMBLETSIP_IFLAG           (IGNBRK)
#define TRIMBLETSIP_OFLAG           (0)
#define TRIMBLETSIP_LFLAG           (ICANON)

#define TRIMBLETSIP_SAMPLES	    5
#define TRIMBLETSIP_KEEP	    3
#define TRIMBLETAIP_SAMPLES	    5
#define TRIMBLETAIP_KEEP	    3

#define TRIMBLETAIP_FLAGS	    (PARSE_F_PPSONSECOND)
#define TRIMBLETSIP_FLAGS	    (TRIMBLETAIP_FLAGS)

#define TRIMBLETAIP_POLL	    poll_dpoll
#define TRIMBLETSIP_POLL	    poll_dpoll

#define TRIMBLETAIP_INIT	    trimbletaip_init
#define TRIMBLETSIP_INIT	    trimbletsip_init

#define TRIMBLETAIP_EVENT	    trimbletaip_event

#define TRIMBLETSIP_EVENT	    trimbletsip_event
#define TRIMBLETSIP_MESSAGE	    trimbletsip_message

#define TRIMBLETAIP_END		    0
#define TRIMBLETSIP_END		    trimbletsip_end

#define TRIMBLETAIP_DATA	    ((void *)(&trimbletaip_pollinfo))
#define TRIMBLETSIP_DATA	    ((void *)(&trimbletsip_pollinfo))

#define TRIMBLETAIP_ID		    GPS_ID
#define TRIMBLETSIP_ID		    GPS_ID

#define TRIMBLETAIP_FORMAT	    "Trimble TAIP"
#define TRIMBLETSIP_FORMAT	    "Trimble TSIP"

#define TRIMBLETAIP_ROOTDELAY        0x0
#define TRIMBLETSIP_ROOTDELAY        0x0

#define TRIMBLETAIP_BASEDELAY        0.0
#define TRIMBLETSIP_BASEDELAY        0.020	/* GPS time message latency */

#define TRIMBLETAIP_NAME	     "GPS_TAIP"
#define TRIMBLETSIP_NAME	     "GPS_TSIP"

#define TRIMBLETAIP_DESCRIPTION      "Trimble GPS (TAIP) receiver"
#define TRIMBLETSIP_DESCRIPTION      "Trimble GPS (TSIP) receiver"

#define TRIMBLETAIP_MAXUNSYNC        0
#define TRIMBLETSIP_MAXUNSYNC        0

#define TRIMBLETAIP_EOL		    '<'

/*
 * RadioCode Clocks RCC 800 receiver
 */
#define RCC_POLLRATE   0       /* only true direct polling */
#define RCC_POLLCMD    "\r"
#define RCC_CMDSIZE    1

static poll_info_t rcc8000_pollinfo = { RCC_POLLRATE, RCC_POLLCMD, RCC_CMDSIZE };
#define RCC8000_POLL            poll_dpoll
#define RCC8000_INIT            poll_init
#define RCC8000_END             0
#define RCC8000_DATA            ((void *)(&rcc8000_pollinfo))
#define RCC8000_ROOTDELAY       0.0
#define RCC8000_BASEDELAY       0.0
#define RCC8000_ID              MSF_ID
#define RCC8000_NAME            "MSF_RCC8000"
#define RCC8000_DESCRIPTION     "RCC 8000 MSF Receiver"
#define RCC8000_FORMAT          "Radiocode RCC8000"
#define RCC8000_MAXUNSYNC       (60*60) /* should be ok for an hour */
#define RCC8000_SPEED		(B2400)
#define RCC8000_CFLAG           (CS8|CREAD|CLOCAL)
#define RCC8000_IFLAG           (IGNBRK|IGNPAR)
#define RCC8000_OFLAG           0
#define RCC8000_LFLAG           0
#define RCC8000_SAMPLES         5
#define RCC8000_KEEP	        3

/*
 * Hopf Radio clock 6021 Format
 *
 */
#define HOPF6021_ROOTDELAY	0.0
#define HOPF6021_BASEDELAY	0.0
#define HOPF6021_NAME		"HOPF_6021"
#define HOPF6021_DESCRIPTION	"HOPF 6021"
#define HOPF6021_FORMAT         "hopf Funkuhr 6021"
#define HOPF6021_MAXUNSYNC	(60*60)  /* should be ok for an hour */
#define HOPF6021_SPEED         (B9600)
#define HOPF6021_CFLAG          (CS8|CREAD|CLOCAL)
#define HOPF6021_IFLAG		(IGNBRK|ISTRIP)
#define HOPF6021_OFLAG		0
#define HOPF6021_LFLAG		0
#define HOPF6021_FLAGS          0
#define HOPF6021_SAMPLES        5
#define HOPF6021_KEEP	        3

/*
 * Diem's Computime Radio Clock Receiver
 */
#define COMPUTIME_FLAGS       0
#define COMPUTIME_ROOTDELAY   0.0
#define COMPUTIME_BASEDELAY   0.0
#define COMPUTIME_ID          DCF_ID
#define COMPUTIME_NAME        "COMPUTIME"
#define COMPUTIME_DESCRIPTION "Diem's Computime receiver"
#define COMPUTIME_FORMAT      "Diem's Computime Radio Clock"
#define COMPUTIME_TYPE        DCF_TYPE
#define COMPUTIME_MAXUNSYNC   (60*60)       /* only trust clock for 1 hour */
#define COMPUTIME_SPEED       (B9600)
#define COMPUTIME_CFLAG       (CSTOPB|CS7|CREAD|CLOCAL)
#define COMPUTIME_IFLAG       (IGNBRK|IGNPAR|ISTRIP)
#define COMPUTIME_OFLAG       0
#define COMPUTIME_LFLAG       0
#define COMPUTIME_SAMPLES     5
#define COMPUTIME_KEEP        3

/*
 * Varitext Radio Clock Receiver
 */
#define VARITEXT_FLAGS       0
#define VARITEXT_ROOTDELAY   0.0
#define VARITEXT_BASEDELAY   0.0
#define VARITEXT_ID          MSF_ID
#define VARITEXT_NAME        "VARITEXT"
#define VARITEXT_DESCRIPTION "Varitext receiver"
#define VARITEXT_FORMAT      "Varitext Radio Clock"
#define VARITEXT_TYPE        DCF_TYPE
#define VARITEXT_MAXUNSYNC   (60*60)       /* only trust clock for 1 hour */
#define VARITEXT_SPEED       (B9600)
#define VARITEXT_CFLAG       (CS7|CREAD|CLOCAL|PARENB|PARODD)
#define VARITEXT_IFLAG       (IGNPAR|IGNBRK|INPCK) /*|ISTRIP)*/
#define VARITEXT_OFLAG       0
#define VARITEXT_LFLAG       0
#define VARITEXT_SAMPLES     32
#define VARITEXT_KEEP        20

/*
 * SEL240x Satellite Sychronized Clock
 */
#define SEL240X_POLLRATE	0 /* only true direct polling */
#define SEL240X_POLLCMD		"BUB8"
#define SEL240X_CMDSIZE		4

static poll_info_t sel240x_pollinfo = { SEL240X_POLLRATE,
	                                SEL240X_POLLCMD,
					SEL240X_CMDSIZE };
#define SEL240X_FLAGS		(PARSE_F_PPSONSECOND)
#define SEL240X_POLL		poll_dpoll
#define SEL240X_INIT		poll_init
#define SEL240X_END		0
#define SEL240X_DATA            ((void *)(&sel240x_pollinfo))
#define SEL240X_ROOTDELAY	0.0
#define SEL240X_BASEDELAY	0.0
#define SEL240X_ID		GPS_ID
#define SEL240X_NAME		"SEL240X"
#define SEL240X_DESCRIPTION	"SEL240x Satellite Synchronized Clock"
#define SEL240X_FORMAT		"SEL B8"
#define SEL240X_MAXUNSYNC	60*60*12 /* only trust clock for 12 hours */
#define SEL240X_SPEED		(B9600)
#define SEL240X_CFLAG		(CS8|CREAD|CLOCAL)
#define SEL240X_IFLAG		(IGNBRK|IGNPAR)
#define SEL240X_OFLAG		(0)
#define SEL240X_LFLAG		(0)
#define SEL240X_SAMPLES		5
#define SEL240X_KEEP		3

static struct parse_clockinfo
{
	unsigned long  cl_flags;		/* operation flags (PPS interpretation, trust handling) */
  void  (*cl_poll)    (struct parseunit *);			/* active poll routine */
  bool  (*cl_init)    (struct parseunit *);			/* active poll init routine */
  void  (*cl_event)   (struct parseunit *, int);		/* special event handling (e.g. reset clock) */
  void  (*cl_end)     (struct parseunit *);			/* active poll end routine */
  void  (*cl_message) (struct parseunit *, parsetime_t *);	/* process a lower layer message */
	void   *cl_data;		/* local data area for "poll" mechanism */
	double    cl_rootdelay;		/* rootdelay */
	double    cl_basedelay;		/* current offset by which the RS232
				time code is delayed from the actual time */
	const char *cl_id;		/* ID code */
	const char *cl_name;		/* device name (tag for logging) */
	const char *cl_description;	/* device description */
	const char *cl_format;		/* fixed format */
	uint8_t  cl_type;		/* clock type (ntp control) */
	unsigned long  cl_maxunsync;	/* time to trust oscillator after losing synch */
	unsigned long  cl_speed;	/* terminal input & output baudrate */
	unsigned long  cl_cflag;        /* terminal control flags */
	unsigned long  cl_iflag;        /* terminal input flags */
	unsigned long  cl_oflag;        /* terminal output flags */
	unsigned long  cl_lflag;        /* terminal local flags */
	unsigned long  cl_samples;	/* samples for median filter */
	unsigned long  cl_keep;         /* samples for median filter to keep */
} parse_clockinfo[] =
{
	{				/* subtype 0 */
		MBG_FLAGS,
		NO_POLL,
		NO_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		DCFPZF535_ROOTDELAY,
		DCFPZF535_BASEDELAY,
		DCF_P_ID,
		DCFPZF535_NAME,
		DCFPZF535_DESCRIPTION,
		DCFPZF535_FORMAT,
		DCF_TYPE,
		DCFPZF535_MAXUNSYNC,
		DCFPZF535_SPEED,
		DCFPZF535_CFLAG,
		DCFPZF535_IFLAG,
		DCFPZF535_OFLAG,
		DCFPZF535_LFLAG,
		DCFPZF535_SAMPLES,
		DCFPZF535_KEEP
	},
	{				/* subtype 1 */
		MBG_FLAGS,
		NO_POLL,
		NO_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		DCFPZF535OCXO_ROOTDELAY,
		DCFPZF535OCXO_BASEDELAY,
		DCF_P_ID,
		DCFPZF535OCXO_NAME,
		DCFPZF535OCXO_DESCRIPTION,
		DCFPZF535OCXO_FORMAT,
		DCF_TYPE,
		DCFPZF535OCXO_MAXUNSYNC,
		DCFPZF535OCXO_SPEED,
		DCFPZF535OCXO_CFLAG,
		DCFPZF535OCXO_IFLAG,
		DCFPZF535OCXO_OFLAG,
		DCFPZF535OCXO_LFLAG,
		DCFPZF535OCXO_SAMPLES,
		DCFPZF535OCXO_KEEP
	},
	{				/* subtype 2 */
		MBG_FLAGS,
		NO_POLL,
		NO_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		DCFUA31_ROOTDELAY,
		DCFUA31_BASEDELAY,
		DCF_A_ID,
		DCFUA31_NAME,
		DCFUA31_DESCRIPTION,
		DCFUA31_FORMAT,
		DCF_TYPE,
		DCFUA31_MAXUNSYNC,
		DCFUA31_SPEED,
		DCFUA31_CFLAG,
		DCFUA31_IFLAG,
		DCFUA31_OFLAG,
		DCFUA31_LFLAG,
		DCFUA31_SAMPLES,
		DCFUA31_KEEP
	},
	{				/* subtype 3 */
		MBG_FLAGS,
		NO_POLL,
		NO_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		DCF7000_ROOTDELAY,
		DCF7000_BASEDELAY,
		DCF_A_ID,
		DCF7000_NAME,
		DCF7000_DESCRIPTION,
		DCF7000_FORMAT,
		DCF_TYPE,
		DCF7000_MAXUNSYNC,
		DCF7000_SPEED,
		DCF7000_CFLAG,
		DCF7000_IFLAG,
		DCF7000_OFLAG,
		DCF7000_LFLAG,
		DCF7000_SAMPLES,
		DCF7000_KEEP
	},
	{				/* subtype 4 */
		NO_CL_FLAGS,
		WSDCF_POLL,
		WSDCF_INIT,
		NO_EVENT,
		WSDCF_END,
		NO_MESSAGE,
		WSDCF_DATA,
		WSDCF_ROOTDELAY,
		WSDCF_BASEDELAY,
		DCF_A_ID,
		WSDCF_NAME,
		WSDCF_DESCRIPTION,
		WSDCF_FORMAT,
		DCF_TYPE,
		WSDCF_MAXUNSYNC,
		WSDCF_SPEED,
		WSDCF_CFLAG,
		WSDCF_IFLAG,
		WSDCF_OFLAG,
		WSDCF_LFLAG,
		WSDCF_SAMPLES,
		WSDCF_KEEP
	},
	{				/* subtype 5 */
		RAWDCF_FLAGS,
		NO_POLL,
		RAWDCF_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		RAWDCF_ROOTDELAY,
		CONRAD_BASEDELAY,
		DCF_A_ID,
		CONRAD_NAME,
		CONRAD_DESCRIPTION,
		RAWDCF_FORMAT,
		DCF_TYPE,
		RAWDCF_MAXUNSYNC,
		RAWDCF_SPEED,
		RAWDCF_CFLAG,
		RAWDCF_IFLAG,
		RAWDCF_OFLAG,
		RAWDCF_LFLAG,
		RAWDCF_SAMPLES,
		RAWDCF_KEEP
	},
	{				/* subtype 6 */
		RAWDCF_FLAGS,
		NO_POLL,
		RAWDCF_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		RAWDCF_ROOTDELAY,
		TIMEBRICK_BASEDELAY,
		DCF_A_ID,
		TIMEBRICK_NAME,
		TIMEBRICK_DESCRIPTION,
		RAWDCF_FORMAT,
		DCF_TYPE,
		RAWDCF_MAXUNSYNC,
		RAWDCF_SPEED,
		RAWDCF_CFLAG,
		RAWDCF_IFLAG,
		RAWDCF_OFLAG,
		RAWDCF_LFLAG,
		RAWDCF_SAMPLES,
		RAWDCF_KEEP
	},
	{				/* subtype 7 */
		MBG_FLAGS,
		GPS16X_POLL,
		GPS16X_INIT,
		NO_EVENT,
		GPS16X_END,
		GPS16X_MESSAGE,
		GPS16X_DATA,
		GPS16X_ROOTDELAY,
		GPS16X_BASEDELAY,
		GPS16X_ID,
		GPS16X_NAME,
		GPS16X_DESCRIPTION,
		GPS16X_FORMAT,
		GPS_TYPE,
		GPS16X_MAXUNSYNC,
		GPS16X_SPEED,
		GPS16X_CFLAG,
		GPS16X_IFLAG,
		GPS16X_OFLAG,
		GPS16X_LFLAG,
		GPS16X_SAMPLES,
		GPS16X_KEEP
	},
	{				/* subtype 8 */
		RAWDCF_FLAGS,
		NO_POLL,
		NO_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		RAWDCF_ROOTDELAY,
		IGELCLOCK_BASEDELAY,
		DCF_A_ID,
		IGELCLOCK_NAME,
		IGELCLOCK_DESCRIPTION,
		RAWDCF_FORMAT,
		DCF_TYPE,
		RAWDCF_MAXUNSYNC,
		IGELCLOCK_SPEED,
		IGELCLOCK_CFLAG,
		RAWDCF_IFLAG,
		RAWDCF_OFLAG,
		RAWDCF_LFLAG,
		RAWDCF_SAMPLES,
		RAWDCF_KEEP
	},
	{				/* subtype 9 */
		TRIMBLETAIP_FLAGS,
#if TRIM_POLLRATE		/* DHD940515: Allow user config */
		NO_POLL,
#else
		TRIMBLETAIP_POLL,
#endif
		TRIMBLETAIP_INIT,
		TRIMBLETAIP_EVENT,
		TRIMBLETAIP_END,
		NO_MESSAGE,
		TRIMBLETAIP_DATA,
		TRIMBLETAIP_ROOTDELAY,
		TRIMBLETAIP_BASEDELAY,
		TRIMBLETAIP_ID,
		TRIMBLETAIP_NAME,
		TRIMBLETAIP_DESCRIPTION,
		TRIMBLETAIP_FORMAT,
		GPS_TYPE,
		TRIMBLETAIP_MAXUNSYNC,
		TRIMBLETAIP_SPEED,
		TRIMBLETAIP_CFLAG,
		TRIMBLETAIP_IFLAG,
		TRIMBLETAIP_OFLAG,
		TRIMBLETAIP_LFLAG,
		TRIMBLETAIP_SAMPLES,
		TRIMBLETAIP_KEEP
	},
	{				/* subtype 10 */
		TRIMBLETSIP_FLAGS,
#if TRIM_POLLRATE		/* DHD940515: Allow user config */
		NO_POLL,
#else
		TRIMBLETSIP_POLL,
#endif
		TRIMBLETSIP_INIT,
		TRIMBLETSIP_EVENT,
		TRIMBLETSIP_END,
		TRIMBLETSIP_MESSAGE,
		TRIMBLETSIP_DATA,
		TRIMBLETSIP_ROOTDELAY,
		TRIMBLETSIP_BASEDELAY,
		TRIMBLETSIP_ID,
		TRIMBLETSIP_NAME,
		TRIMBLETSIP_DESCRIPTION,
		TRIMBLETSIP_FORMAT,
		GPS_TYPE,
		TRIMBLETSIP_MAXUNSYNC,
		TRIMBLETSIP_SPEED,
		TRIMBLETSIP_CFLAG,
		TRIMBLETSIP_IFLAG,
		TRIMBLETSIP_OFLAG,
		TRIMBLETSIP_LFLAG,
		TRIMBLETSIP_SAMPLES,
		TRIMBLETSIP_KEEP
	},
	{                             /* subtype 11 */
		NO_CL_FLAGS,
		RCC8000_POLL,
		RCC8000_INIT,
		NO_EVENT,
		RCC8000_END,
		NO_MESSAGE,
		RCC8000_DATA,
		RCC8000_ROOTDELAY,
		RCC8000_BASEDELAY,
		RCC8000_ID,
		RCC8000_NAME,
		RCC8000_DESCRIPTION,
		RCC8000_FORMAT,
		DCF_TYPE,
		RCC8000_MAXUNSYNC,
		RCC8000_SPEED,
		RCC8000_CFLAG,
		RCC8000_IFLAG,
		RCC8000_OFLAG,
		RCC8000_LFLAG,
		RCC8000_SAMPLES,
		RCC8000_KEEP
	},
	{                             /* subtype 12 */
		HOPF6021_FLAGS,
		NO_POLL,
		NO_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		HOPF6021_ROOTDELAY,
		HOPF6021_BASEDELAY,
		DCF_ID,
		HOPF6021_NAME,
		HOPF6021_DESCRIPTION,
		HOPF6021_FORMAT,
		DCF_TYPE,
		HOPF6021_MAXUNSYNC,
		HOPF6021_SPEED,
		HOPF6021_CFLAG,
		HOPF6021_IFLAG,
		HOPF6021_OFLAG,
		HOPF6021_LFLAG,
		HOPF6021_SAMPLES,
		HOPF6021_KEEP
	},
	{                            /* subtype 13 */
		COMPUTIME_FLAGS,
		NO_POLL,
		NO_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		COMPUTIME_ROOTDELAY,
		COMPUTIME_BASEDELAY,
		COMPUTIME_ID,
		COMPUTIME_NAME,
		COMPUTIME_DESCRIPTION,
		COMPUTIME_FORMAT,
		COMPUTIME_TYPE,
		COMPUTIME_MAXUNSYNC,
		COMPUTIME_SPEED,
		COMPUTIME_CFLAG,
		COMPUTIME_IFLAG,
		COMPUTIME_OFLAG,
		COMPUTIME_LFLAG,
		COMPUTIME_SAMPLES,
		COMPUTIME_KEEP
	},
	{				/* subtype 14 */
		RAWDCF_FLAGS,
		NO_POLL,
		RAWDCFDTRSET_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		RAWDCF_ROOTDELAY,
		RAWDCF_BASEDELAY,
		DCF_A_ID,
		RAWDCFDTRSET_NAME,
		RAWDCFDTRSET_DESCRIPTION,
		RAWDCF_FORMAT,
		DCF_TYPE,
		RAWDCF_MAXUNSYNC,
		RAWDCF_SPEED,
		RAWDCF_CFLAG,
		RAWDCF_IFLAG,
		RAWDCF_OFLAG,
		RAWDCF_LFLAG,
		RAWDCF_SAMPLES,
		RAWDCF_KEEP
	},
	{				/* subtype 15 */
		0,				/* operation flags (io modes) */
  		NO_POLL,			/* active poll routine */
		NO_INIT,			/* active poll init routine */
  		NO_EVENT,		        /* special event handling (e.g. reset clock) */
  		NO_END,				/* active poll end routine */
  		NO_MESSAGE,			/* process a lower layer message */
		NO_LCLDATA,			/* local data area for "poll" mechanism */
		0,				/* rootdelay */
		11.0 /* bits */ / 9600,		/* current offset by which the RS232
				           	time code is delayed from the actual time */
		DCF_ID,				/* ID code */
		"WHARTON400A",
		"WHARTON 400A Series clock",	/* device description */
		"WHARTON 400A Series clock Output Format 1",	/* fixed format */
			/* Must match a format-name in a libparse/clk_xxx.c file */
		DCF_TYPE,			/* clock type (ntp control) */
		(1*60*60),		        /* time to trust oscillator after losing synch */
		B9600,				/* terminal input & output baudrate */
		(CS8|CREAD|PARENB|CLOCAL|HUPCL),/* terminal control flags */
		0,				/* terminal input flags */
		0,				/* terminal output flags */
		0,				/* terminal local flags */
		5,				/* samples for median filter */
		3,				/* samples for median filter to keep */
	},
	{				/* subtype 16 - RAWDCF RTS set, DTR clr */
		RAWDCF_FLAGS,
		NO_POLL,
		RAWDCFDTRCLRRTSSET_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		RAWDCF_ROOTDELAY,
		RAWDCF_BASEDELAY,
		DCF_A_ID,
		RAWDCFDTRCLRRTSSET_NAME,
		RAWDCFDTRCLRRTSSET_DESCRIPTION,
		RAWDCF_FORMAT,
		DCF_TYPE,
		RAWDCF_MAXUNSYNC,
		RAWDCF_SPEED,
		RAWDCF_CFLAG,
		RAWDCF_IFLAG,
		RAWDCF_OFLAG,
		RAWDCF_LFLAG,
		RAWDCF_SAMPLES,
		RAWDCF_KEEP
	},
        {                            /* subtype 17 */
                VARITEXT_FLAGS,
                NO_POLL,
                NO_INIT,
                NO_EVENT,
                NO_END,
                NO_MESSAGE,
                NO_LCLDATA,
                VARITEXT_ROOTDELAY,
                VARITEXT_BASEDELAY,
                VARITEXT_ID,
                VARITEXT_NAME,
                VARITEXT_DESCRIPTION,
                VARITEXT_FORMAT,
                VARITEXT_TYPE,
                VARITEXT_MAXUNSYNC,
                VARITEXT_SPEED,
                VARITEXT_CFLAG,
                VARITEXT_IFLAG,
                VARITEXT_OFLAG,
                VARITEXT_LFLAG,
                VARITEXT_SAMPLES,
                VARITEXT_KEEP
        },
	{				/* subtype 18 */
		MBG_FLAGS,
		NO_POLL,
		NO_INIT,
		NO_EVENT,
		GPS16X_END,
		GPS16X_MESSAGE,
		GPS16X_DATA,
		GPS16X_ROOTDELAY,
		GPS16X_BASEDELAY,
		GPS16X_ID,
		GPS16X_NAME,
		GPS16X_DESCRIPTION,
		GPS16X_FORMAT,
		GPS_TYPE,
		GPS16X_MAXUNSYNC,
		GPS16X_SPEED,
		GPS16X_CFLAG,
		GPS16X_IFLAG,
		GPS16X_OFLAG,
		GPS16X_LFLAG,
		GPS16X_SAMPLES,
		GPS16X_KEEP
	},
	{				/* subtype 19 */
		RAWDCF_FLAGS,
		NO_POLL,
		RAWDCF_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		RAWDCF_ROOTDELAY,
		GUDE_EMC_USB_V20_BASEDELAY,
		DCF_A_ID,
		GUDE_EMC_USB_V20_NAME,
		GUDE_EMC_USB_V20_DESCRIPTION,
		RAWDCF_FORMAT,
		DCF_TYPE,
		RAWDCF_MAXUNSYNC,
		GUDE_EMC_USB_V20_SPEED,
		RAWDCF_CFLAG,
		RAWDCF_IFLAG,
		RAWDCF_OFLAG,
		RAWDCF_LFLAG,
		RAWDCF_SAMPLES,
		RAWDCF_KEEP
	},
	{				/* subtype 20, like subtype 14 but driven by 75 baud */
		RAWDCF_FLAGS,
		NO_POLL,
		RAWDCFDTRSET_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		RAWDCF_ROOTDELAY,
		RAWDCF_BASEDELAY,
		DCF_A_ID,
		RAWDCFDTRSET_NAME,
		RAWDCFDTRSET75_DESCRIPTION,
		RAWDCF_FORMAT,
		DCF_TYPE,
		RAWDCF_MAXUNSYNC,
		B75,
		RAWDCF_CFLAG,
		RAWDCF_IFLAG,
		RAWDCF_OFLAG,
		RAWDCF_LFLAG,
		RAWDCF_SAMPLES,
		RAWDCF_KEEP
	},
	{				/* subtype 21, like subtype 16 but driven by 75 baud
					 - RAWDCF RTS set, DTR clr */
		RAWDCF_FLAGS,
		NO_POLL,
		RAWDCFDTRCLRRTSSET_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		RAWDCF_ROOTDELAY,
		RAWDCF_BASEDELAY,
		DCF_A_ID,
		RAWDCFDTRCLRRTSSET_NAME,
		RAWDCFDTRCLRRTSSET75_DESCRIPTION,
		RAWDCF_FORMAT,
		DCF_TYPE,
		RAWDCF_MAXUNSYNC,
		B75,
		RAWDCF_CFLAG,
		RAWDCF_IFLAG,
		RAWDCF_OFLAG,
		RAWDCF_LFLAG,
		RAWDCF_SAMPLES,
		RAWDCF_KEEP
	},
	{				/* subtype 22 - like 2 with POWERUP trust */
		MBG_FLAGS | PARSE_F_POWERUPTRUST,
		NO_POLL,
		NO_INIT,
		NO_EVENT,
		NO_END,
		NO_MESSAGE,
		NO_LCLDATA,
		DCFUA31_ROOTDELAY,
		DCFUA31_BASEDELAY,
		DCF_A_ID,
		DCFUA31_NAME,
		DCFUA31_DESCRIPTION,
		DCFUA31_FORMAT,
		DCF_TYPE,
		DCFUA31_MAXUNSYNC,
		DCFUA31_SPEED,
		DCFUA31_CFLAG,
		DCFUA31_IFLAG,
		DCFUA31_OFLAG,
		DCFUA31_LFLAG,
		DCFUA31_SAMPLES,
		DCFUA31_KEEP
	},
	{				/* subtype 23 - like 7 with POWERUP trust */
		MBG_FLAGS | PARSE_F_POWERUPTRUST,
		GPS16X_POLL,
		GPS16X_INIT,
		NO_EVENT,
		GPS16X_END,
		GPS16X_MESSAGE,
		GPS16X_DATA,
		GPS16X_ROOTDELAY,
		GPS16X_BASEDELAY,
		GPS16X_ID,
		GPS16X_NAME,
		GPS16X_DESCRIPTION,
		GPS16X_FORMAT,
		GPS_TYPE,
		GPS16X_MAXUNSYNC,
		GPS16X_SPEED,
		GPS16X_CFLAG,
		GPS16X_IFLAG,
		GPS16X_OFLAG,
		GPS16X_LFLAG,
		GPS16X_SAMPLES,
		GPS16X_KEEP
	},
	{				/* subtype 24 */
		SEL240X_FLAGS,
		SEL240X_POLL,
		SEL240X_INIT,
		NO_EVENT,
		SEL240X_END,
		NO_MESSAGE,
		SEL240X_DATA,
		SEL240X_ROOTDELAY,
		SEL240X_BASEDELAY,
		SEL240X_ID,
		SEL240X_NAME,
		SEL240X_DESCRIPTION,
		SEL240X_FORMAT,
		GPS_TYPE,
		SEL240X_MAXUNSYNC,
		SEL240X_SPEED,
		SEL240X_CFLAG,
		SEL240X_IFLAG,
		SEL240X_OFLAG,
		SEL240X_LFLAG,
		SEL240X_SAMPLES,
		SEL240X_KEEP
	},
};

static int ncltypes = sizeof(parse_clockinfo) / sizeof(struct parse_clockinfo);

#define CLK_REALTYPE(x) ((int)(((x)->cfg.mode) & 0x7F))
/* carefull, CLK_TYPE() in refclock_trimle.c is different */
#define CLK_TYPE(x)	((CLK_REALTYPE(x) >= ncltypes) ? ~0 : CLK_REALTYPE(x))
#define CLK_PPS(x)	(((x)->cfg.mode) & 0x80)

/*
 * Other constant stuff
 */
#define	PARSEHSREFID	0x7f7f08ff	/* 127.127.8.255 refid for hi strata */

#define PARSESTATISTICS   (60*60)	        /* output state statistics every hour */

static int notice = 0;

#define PARSE_STATETIME(parse, i) ((parse->generic->currentstatus == i) ? parse->statetime[i] + current_time - parse->lastchange : parse->statetime[i])

static void parse_event   (struct parseunit *, int);
static void parse_process (struct parseunit *, parsetime_t *);
static void clear_err     (struct parseunit *, unsigned long);
static int  list_err      (struct parseunit *, unsigned long);
static char * l_mktime    (unsigned long);

/**===========================================================================
 ** implementation error message regression module
 **/
static void
clear_err(
	struct parseunit *parse,
	unsigned long    lstate
	)
{
	if (lstate == ERR_ALL)
	{
		size_t i;

		for (i = 0; i < ERR_CNT; i++)
		{
			parse->errors[i].err_stage   = err_tbl[i];
			parse->errors[i].err_cnt     = 0;
			parse->errors[i].err_last    = 0;
			parse->errors[i].err_started = 0;
			parse->errors[i].err_suppressed = 0;
		}
	}
	else
	{
		parse->errors[lstate].err_stage   = err_tbl[lstate];
		parse->errors[lstate].err_cnt     = 0;
		parse->errors[lstate].err_last    = 0;
		parse->errors[lstate].err_started = 0;
		parse->errors[lstate].err_suppressed = 0;
	}
}

static int
list_err(
	struct parseunit *parse,
	unsigned long     lstate
	)
{
	int do_it;
	struct errorinfo *err = &parse->errors[lstate];

	if (err->err_started == 0)
	{
		err->err_started = current_time;
	}

	do_it = (current_time - err->err_last) >= err->err_stage->err_delay;

	if (do_it)
	    err->err_cnt++;

	if (err->err_stage->err_count &&
	    (err->err_cnt >= err->err_stage->err_count))
	{
		err->err_stage++;
		err->err_cnt = 0;
	}

	if (!err->err_cnt && do_it)
	    msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: interval for following error message class is at least %s",
		    parse->peer->procptr->refclkunit, l_mktime(err->err_stage->err_delay));

	if (!do_it)
	    err->err_suppressed++;
	else
	    err->err_last = current_time;

	if (do_it && err->err_suppressed)
	{
		msyslog(LOG_INFO,
                    "REFCLOCK: PARSE receiver #%d: %lu message%s suppressed, error "
                    "condition class persists for %s",
		    parse->peer->procptr->refclkunit, err->err_suppressed,
                    (err->err_suppressed == 1) ? " was" : "s where",
		    l_mktime(current_time - err->err_started));
		err->err_suppressed = 0;
	}

	return do_it;
}

/*--------------------------------------------------
 * mkreadable - make a printable ascii string (without
 * embedded quotes so that the ntpq protocol isn't
 * fooled
 */

static char *
mkreadable(
	char  *buffer,
	long  blen,
	const char  *src,
	unsigned long  srclen,
	int hex
	)
{
	static const char ellipsis[] = "...";
	char *b    = buffer;
	char *endb = NULL;

	if (blen < 4)
		return NULL;		/* don't bother with mini buffers */

	endb = buffer + blen - sizeof(ellipsis);

	blen--;			/* account for '\0' */

	while (blen && srclen--)
	{
		if (!hex &&             /* no binary only */
		    (*src != '\\') &&   /* no plain \ */
		    (*src != '"') &&    /* no " */
		    isprint((unsigned char)*src))	/* only printables */
		{			/* they are easy... */
			*buffer++ = *src++;
			blen--;
		}
		else
		{
			if (blen < 4)
			{
				while (blen--)
				{
					*buffer++ = '.';
				}
				*buffer = '\0';
				return b;
			}
			else
			{
				if (*src == '\\')
				{
					memcpy(buffer, "\\\\", 2);
					buffer += 2;
					blen   -= 2;
					src++;
				}
				else
				{
					snprintf(buffer, (size_t)blen,
                                                 "\\x%02x", (unsigned)(*src++));
					blen   -= 4;
					buffer += 4;
				}
			}
		}
		if (srclen && !blen && endb) /* overflow - set last chars to ... */
			memcpy(endb, ellipsis, sizeof(ellipsis));
	}

	*buffer = '\0';
	return b;
}


/*--------------------------------------------------
 * mkascii - make a printable ascii string
 * assumes (unless defined better) 7-bit ASCII
 */
static char *
mkascii(
	char  *buffer,
	long  blen,
	const char  *src,
	unsigned long  srclen
	)
{
	return mkreadable(buffer, blen, src, srclen, 0);
}

/**===========================================================================
 ** implementation of i/o handling methods
 ** (all STREAM, partial STREAM, user level)
 **/

static int  local_init     (struct parseunit *);
static void local_end      (struct parseunit *);
static int  local_nop      (struct parseunit *);
static bool local_setcs    (struct parseunit *, parsectl_t *);
static int  local_getfmt   (struct parseunit *, parsectl_t *);
static int  local_setfmt   (struct parseunit *, parsectl_t *);
static int  local_timecode (struct parseunit *, parsectl_t *);
static void local_receive  (struct recvbuf *);
static int  local_input    (struct recvbuf *);

static bind_t io_bindings[] =
{
	{
		"normal",
		local_init,
		local_end,
		local_setcs,
		local_nop,
		local_nop,
		local_getfmt,
		local_setfmt,
		local_timecode,
		local_receive,
		local_input,
	},
	{
		(char *)0,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
	}
};

/*--------------------------------------------------
 * local init
 */
static int
local_init(
	struct parseunit *parse
	)
{
	return parse_ioinit(&parse->parseio);
}

/*--------------------------------------------------
 * local end
 */
static void
local_end(
	struct parseunit *parse
	)
{
	parse_ioend(&parse->parseio);
}


/*--------------------------------------------------
 * local nop
 */
static int
local_nop(
	struct parseunit *parse
	)
{
	UNUSED_ARG(parse);

	return true;
}

/*--------------------------------------------------
 * local setcs
 */
static bool
local_setcs(
	struct parseunit *parse,
	parsectl_t  *tcl
	)
{
	return parse_setcs(tcl, &parse->parseio);
}

/*--------------------------------------------------
 * local getfmt
 */
static int
local_getfmt(
	struct parseunit *parse,
	parsectl_t  *tcl
	)
{
	return parse_getfmt(tcl, &parse->parseio);
}

/*--------------------------------------------------
 * local setfmt
 */
static int
local_setfmt(
	struct parseunit *parse,
	parsectl_t  *tcl
	)
{
	return parse_setfmt(tcl, &parse->parseio);
}

/*--------------------------------------------------
 * local timecode
 */
static int
local_timecode(
	struct parseunit *parse,
	parsectl_t  *tcl
	)
{
	return parse_timecode(tcl, &parse->parseio);
}


/*--------------------------------------------------
 * local input
 */
static int
local_input(
	struct recvbuf *rbufp
	)
{
	struct parseunit * parse;

	int count;
	unsigned char *s;
	timestamp_t ts;

	parse = (struct parseunit *)rbufp->recv_peer->procptr->unitptr;
	if (!parse->peer)
		return false;

	/*
	 * eat all characters, parsing then and feeding complete samples
	 */
	count = (int)rbufp->recv_length;
	s = (unsigned char *)rbufp->recv_buffer;
	ts = rbufp->recv_time;

	while (count--)
	{
		if (parse_ioread(&parse->parseio, (char)(*s++), &ts))
		{
			struct recvbuf *buf;

			/*
			 * got something good to eat
			 */
			if (!PARSE_PPS(parse->parseio.parse_dtime.parse_state))
			{
#ifdef HAVE_PPSAPI
				if (parse->flags & PARSE_PPSAPI)
				{
					struct timespec pps_timeout;
					pps_info_t      pps_info;

					pps_timeout.tv_sec  = 0;
					pps_timeout.tv_nsec = 0;

					if (time_pps_fetch(parse->ppsctl.handle, PPS_TSFMT_TSPEC, &pps_info,
							   &pps_timeout) == 0)
					{
						if (pps_info.assert_sequence + pps_info.clear_sequence != parse->ppsserial)
						{
							double dtemp;

						        struct timespec pts;
							/*
							 * add PPS time stamp if available via ppsclock module
							 * and not supplied already.
							 */
							if (parse->flags & PARSE_CLEAR)
							  pts = pps_info.clear_timestamp;
							else
							  pts = pps_info.assert_timestamp;

							setlfpuint(parse->parseio.parse_dtime.parse_ptime,
                                                                   (uint64_t)pts.tv_sec + JAN_1970);

							dtemp = (double) pts.tv_nsec * S_PER_NS;
							if (dtemp < 0.) {
								dtemp += 1;
								bumplfpuint(parse->parseio.parse_dtime.parse_ptime, -1);
							}
							if (dtemp > 1.) {
								dtemp -= 1;
								bumplfpuint(parse->parseio.parse_dtime.parse_ptime, 1);
							}
							setlfpfrac(parse->parseio.parse_dtime.parse_ptime, (uint32_t)(dtemp * FRAC));

							parse->parseio.parse_dtime.parse_state |= PARSEB_PPS|PARSEB_S_PPS;
							DPRINT(4, ("parse: local_receive: fd %d PPSAPI seq %ld - PPS %s\n",
								   rbufp->fd,
								   (long)pps_info.assert_sequence + (long)pps_info.clear_sequence,
								   lfptoa(parse->parseio.parse_dtime.parse_ptime, 6)));
						}
#ifdef DEBUG
						else
						{
							if (debug > 3) /* SPECIAL DEBUG */
							{
								printf(
								       "parse: local_receive: fd %d PPSAPI seq assert %ld, seq clear %ld - NO PPS event\n",
								       rbufp->fd,
								       (long)pps_info.assert_sequence, (long)pps_info.clear_sequence);
							}
						}
#endif
						parse->ppsserial = pps_info.assert_sequence + pps_info.clear_sequence;
					}
#ifdef DEBUG
					else
					{
						if (debug > 3) /* SPECIAL DEBUG */
						{
							printf(
							       "parse: local_receive: fd %d PPSAPI time_pps_fetch errno = %d\n",
							       rbufp->fd,
							       errno);
						}
					}
#endif
				}
#endif /* !HAVE_PPSAPI */
			}
			if (count)
			{	/* simulate receive */
// FIXME - this copy is no longer needed
// This code is the result of a simple fix for SINGLEBUFFER
// The copy used to go to add_full_recv_buffer, but that's not needed any more
// I'm not sure the local_receive below is correct
// Hal, 2018-Sep-21
				buf = get_free_recv_buffer();
				if (buf != NULL) {
					memmove((void *)buf->recv_buffer,
						(void *)&parse->parseio.parse_dtime,
						sizeof(parsetime_t));
					buf->recv_length  = sizeof(parsetime_t);
					buf->recv_time    = rbufp->recv_time;
					buf->srcadr       = rbufp->srcadr;
					buf->dstadr       = rbufp->dstadr;
					buf->fd           = rbufp->fd;
					buf->recv_peer    = rbufp->recv_peer;
					parse->generic->io.recvcount++;
					pkt_count.packets_received++;
					local_receive(buf);
					freerecvbuf(buf);
				}
				parse_iodone(&parse->parseio);
			}
			else
			{
				memmove((void *)rbufp->recv_buffer,
					(void *)&parse->parseio.parse_dtime,
					sizeof(parsetime_t));
				parse_iodone(&parse->parseio);
				rbufp->recv_length = sizeof(parsetime_t);
				return true; /* got something & in place return */
			}
		}
	}
	return false;		/* nothing to pass up */
}

/*--------------------------------------------------
 * local receive
 */
static void
local_receive(
	struct recvbuf *rbufp
	)
{
	struct parseunit * parse;
	parsetime_t parsetime;

	parse = (struct parseunit *)rbufp->recv_peer->procptr->unitptr;
	if (!parse->peer)
	    return;

	if (rbufp->recv_length != sizeof(parsetime_t))
	{
		ERR(ERR_BADIO)
			msyslog(LOG_ERR,
				"REFCLOCK: PARSE receiver #%d: local_receive: bad size "
				" (got %zu expected %zu)",
				parse->peer->procptr->refclkunit, rbufp->recv_length,
                                sizeof(parsetime_t));
		parse_event(parse, CEVNT_BADREPLY);
		return;
	}
	clear_err(parse, ERR_BADIO);

	memmove((void *)&parsetime,
		(void *)rbufp->recv_buffer,
		sizeof(parsetime_t));

	DPRINT(4, ("PARSE receiver #%d: status %06x, state %08x, time(fp) %lx.%08lx, stime(fp) %lx.%08lx, ptime(fp) %lx.%08lx\n",
		   parse->peer->procptr->refclkunit,
		   (unsigned int)parsetime.parse_status,
		   (unsigned int)parsetime.parse_state,
		   (unsigned long)lfpuint(parsetime.parse_time),
		   (unsigned long)lfpfrac(parsetime.parse_time),
		   (unsigned long)lfpuint(parsetime.parse_stime),
		   (unsigned long)lfpfrac(parsetime.parse_stime),
		   (unsigned long)lfpuint(parsetime.parse_ptime),
		   (unsigned long)lfpfrac(parsetime.parse_ptime)));

	parse_process(parse, &parsetime);
}

/*--------------------------------------------------
 * init_iobinding - find and initialize lower layers
 */
static bind_t *
init_iobinding(
	struct parseunit *parse
	)
{
  bind_t *b = io_bindings;

	while (b->bd_description != (char *)0)
	{
		if ((*b->bd_init)(parse))
		{
			return b;
		}
		b++;
	}
	return (bind_t *)0;
}

/**===========================================================================
 ** support routines
 **/

static NTP_PRINTF(4, 5) char *
ap(char *buffer, size_t len, char *pos, const char *fmt, ...)
{
	va_list va;
	int l;
	size_t rem = len - (size_t)(pos - buffer);

	if (rem == 0)
		return pos;

	va_start(va, fmt);
	l = vsnprintf(pos, rem, fmt, va);
	va_end(va);

	if (l != -1) {
		rem--;
		if (rem >= (size_t)l)
			pos += l;
		else
			pos += rem;
	}

	return pos;
}

/*--------------------------------------------------
 * convert a flag field to a string
 */
static char *
parsestate(
	unsigned long lstate,
	char *buffer,
	int size
	)
{
	static struct bits
	{
		unsigned long      bit;
		const char *name;
	} flagstrings[] =
	  {
		  { PARSEB_ANNOUNCE,   "DST SWITCH WARNING" },
		  { PARSEB_POWERUP,    "NOT SYNCHRONIZED" },
		  { PARSEB_NOSYNC,     "TIME CODE NOT CONFIRMED" },
		  { PARSEB_DST,        "DST" },
		  { PARSEB_UTC,        "UTC DISPLAY" },
		  { PARSEB_LEAPADD,    "LEAP ADD WARNING" },
		  { PARSEB_LEAPDEL,    "LEAP DELETE WARNING" },
		  { PARSEB_LEAPSECOND, "LEAP SECOND" },
		  { PARSEB_CALLBIT,    "CALL BIT" },
		  { PARSEB_TIMECODE,   "TIME CODE" },
		  { PARSEB_PPS,        "PPS" },
		  { PARSEB_POSITION,   "POSITION" },
		  { 0,		       NULL }
	  };

	static struct sbits
	{
		unsigned long      bit;
		const char *name;
	} sflagstrings[] =
	  {
		  { PARSEB_S_LEAP,     "LEAP INDICATION" },
		  { PARSEB_S_PPS,      "PPS SIGNAL" },
		  { PARSEB_S_CALLBIT,  "CALLBIT" },
		  { PARSEB_S_POSITION, "POSITION" },
		  { 0,		       NULL }
	  };
	int i;
	char *s, *t;

	*buffer = '\0';
	s = t = buffer;

	i = 0;
	while (flagstrings[i].bit)
	{
		if (flagstrings[i].bit & lstate)
		{
			if (s != t)
				t = ap(buffer, (size_t)size, t, "; ");
			t = ap(buffer, (size_t)size, t, "%s",
                               flagstrings[i].name);
		}
		i++;
	}

	if (lstate & (PARSEB_S_LEAP|PARSEB_S_CALLBIT|PARSEB_S_PPS|PARSEB_S_POSITION))
	{
		if (s != t)
			t = ap(buffer, (size_t)size, t, "; ");

		t = ap(buffer, (size_t)size, t, "(");

		s = t;

		i = 0;
		while (sflagstrings[i].bit)
		{
			if (sflagstrings[i].bit & lstate)
			{
				if (t != s)
				{
					t = ap(buffer, (size_t)size, t, "; ");
				}

				t = ap(buffer, (size_t)size, t, "%s",
				    sflagstrings[i].name);
			}
			i++;
		}
		ap(buffer, (size_t)size, t, ")");
	}
	return buffer;
}

/*--------------------------------------------------
 * convert a status flag field to a string
 */
static char *
parsestatus(
	unsigned long lstate,
	char *buffer,
	int size
	)
{
	static struct bits
	{
		unsigned long      bit;
		const char *name;
	} flagstrings[] =
	  {
		  { CVT_OK,      "CONVERSION SUCCESSFUL" },
		  { CVT_NONE,    "NO CONVERSION" },
		  { CVT_FAIL,    "CONVERSION FAILED" },
		  { CVT_BADFMT,  "ILLEGAL FORMAT" },
		  { CVT_BADDATE, "DATE ILLEGAL" },
		  { CVT_BADTIME, "TIME ILLEGAL" },
		  { CVT_ADDITIONAL, "ADDITIONAL DATA" },
		  { 0,		 NULL }
	  };
	int i;
	char *t;

	t = buffer;
	*buffer = '\0';

	i = 0;
	while (flagstrings[i].bit)
	{
		if (flagstrings[i].bit & lstate)
		{
			if (t != buffer)
				t = ap(buffer, (size_t)size, t, "; ");
			t = ap(buffer, (size_t)size, t, "%s",
                               flagstrings[i].name);
		}
		i++;
	}

	return buffer;
}

/*--------------------------------------------------
 * convert a clock status flag field to a string
 */
static const char *
clockstatus(
	unsigned long lstate
	)
{
	static char buffer[20];
	static struct status
	{
		unsigned long      value;
		const char *name;
	} flagstrings[] =
	  {
		  { CEVNT_NOMINAL, "NOMINAL" },
		  { CEVNT_TIMEOUT, "NO RESPONSE" },
		  { CEVNT_BADREPLY,"BAD FORMAT" },
		  { CEVNT_FAULT,   "FAULT" },
		  { CEVNT_PROP,    "PROPAGATION DELAY" },
		  { CEVNT_BADDATE, "ILLEGAL DATE" },
		  { CEVNT_BADTIME, "ILLEGAL TIME" },
		  { (unsigned)~0L, NULL }
	  };
	int i;

	i = 0;
	while (flagstrings[i].value != (unsigned int)~0)
	{
		if (flagstrings[i].value == lstate)
		{
			return flagstrings[i].name;
		}
		i++;
	}

	snprintf(buffer, sizeof(buffer), "unknown #%lu", lstate);

	return buffer;
}


/*--------------------------------------------------
 * l_mktime - make representation of a relative time
 */
static char *
l_mktime(
	unsigned long delta
	)
{
	unsigned long tmp, m, s;
	static char buffer[40];
	char *t;

	buffer[0] = '\0';
	t = buffer;

	if ((tmp = delta / (60*60*24)) != 0)
	{
		t = ap(buffer, sizeof(buffer), t, "%lud+", tmp);
		delta -= tmp * 60*60*24;
	}

	s = delta % 60;
	delta /= 60;
	m = delta % 60;
	delta /= 60;

	ap(buffer, sizeof(buffer), t, "%02d:%02d:%02d",
	     (int)delta, (int)m, (int)s);

	return buffer;
}


/*--------------------------------------------------
 * parse_statistics - list summary of clock states
 */
static void
parse_statistics(
	struct parseunit *parse
	)
{
	int i;

	NLOG(NLOG_CLOCKSTATIST) /* conditional if clause for conditional syslog */
		{
			msyslog(LOG_INFO,
				"REFCLOCK: PARSE receiver #%d: running time: %s",
				parse->peer->procptr->refclkunit,
				l_mktime(current_time - parse->generic->timestarted));

			msyslog(LOG_INFO,
				"REFCLOCK: PARSE receiver #%d: current status: %s",
				parse->peer->procptr->refclkunit,
				clockstatus(parse->generic->currentstatus));

			for (i = 0; i <= CEVNT_MAX; i++)
			{
				unsigned long s_time;
				unsigned long percent, d = current_time - parse->generic->timestarted;

				percent = s_time = PARSE_STATETIME(parse, i);

				while (((unsigned long)(~0) / 10000) < percent)
				{
					percent /= 10;
					d       /= 10;
				}

				if (d)
				    percent = (percent * 10000) / d;
				else
				    percent = 10000;

				if (s_time)
				    msyslog(LOG_INFO,
                                        "REFCLOCK: PARSE receiver #%d: state %18s: "
                                        "%13s (%3lu.%02lu%%)",
					    parse->peer->procptr->refclkunit,
					    clockstatus((unsigned int)i),
					    l_mktime(s_time),
					    percent / 100, percent % 100);
			}
		}
}

/*--------------------------------------------------
 * cparse_statistics - wrapper for statistics call
 */
static void
cparse_statistics(
        struct parseunit *parse
	)
{
	if (parse->laststatistic + PARSESTATISTICS < current_time)
		parse_statistics(parse);
	parse->laststatistic = current_time;
}

/**===========================================================================
 ** ntp interface routines
 **/

/*--------------------------------------------------
 * parse_shutdown - shut down a PARSE clock
 */
static void
parse_shutdown(
	struct refclockproc *pp
	)
{
	struct parseunit *parse = NULL;

	if (pp)
		parse = pp->unitptr;

	if (!parse)
	{
		/* nothing to clean up */
		return;
	}

	if (!parse->peer)
	{
		msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: INTERNAL ERROR - unit already inactive - `shutdown ignored", pp->refclkunit);
		return;
	}

#ifdef HAVE_PPSAPI
	if (parse->flags & PARSE_PPSAPI)
	{
		(void)time_pps_destroy(parse->ppsctl.handle);
	}
#endif
	if (parse->generic->io.fd != parse->ppsfd && parse->ppsfd != -1)
		(void)close(parse->ppsfd);  /* close separate PPS source */

	/*
	 * print statistics a last time and
	 * stop statistics machine
	 */
	parse_statistics(parse);

	if (parse->parse_type->cl_end)
	{
		parse->parse_type->cl_end(parse);
	}

	/*
	 * cleanup before leaving this world
	 */
	if (parse->binding)
	    PARSE_END(parse);

	/*
	 * Tell the I/O module to turn us off.  We're history.
	 */
	io_closeclock(&parse->generic->io);

	free_varlist(parse->kv);

	NLOG(NLOG_CLOCKINFO) /* conditional if clause for conditional syslog */
		msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: reference clock \"%s\" removed",
			parse->peer->procptr->refclkunit, parse->parse_type->cl_description);

	parse->peer = (struct peer *)0; /* unused now */
	pp->unitptr = (void *)0;
	free(parse);
}

#ifdef HAVE_PPSAPI
/*----------------------------------------
 * set up HARDPPS via PPSAPI
 */
static void
parse_hardpps(
	      struct parseunit *parse,
	      int mode
	      )
{
        if (parse->hardppsstate == mode)
	        return;

	if (CLK_PPS(parse->peer) && (parse->flags & PARSE_PPSKERNEL)) {
		int	i = 0;

		if (mode == PARSE_HARDPPS_ENABLE)
		        {
			        if (parse->flags & PARSE_CLEAR)
				        i = PPS_CAPTURECLEAR;
				else
				        i = PPS_CAPTUREASSERT;
			}

		if (time_pps_kcbind(parse->ppsctl.handle, PPS_KC_HARDPPS, i,
		    PPS_TSFMT_TSPEC) < 0) {
		        msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: time_pps_kcbind failed: %m",
				parse->peer->procptr->refclkunit);
		} else {
		        NLOG(NLOG_CLOCKINFO)
		                msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: kernel PPS synchronisation %sabled",
					parse->peer->procptr->refclkunit, (mode == PARSE_HARDPPS_ENABLE) ? "en" : "dis");
			/*
			 * tell the rest, that we have a kernel PPS source, iff we ever enable HARDPPS
			 */
			if (mode == PARSE_HARDPPS_ENABLE)
			        clock_ctl.hardpps_enable = true;
		}
	}

	parse->hardppsstate = mode;
}

/*----------------------------------------
 * set up PPS via PPSAPI
 */
static bool
parse_ppsapi(
	     struct parseunit *parse
	)
{
	int cap, mode_ppsoffset;
	const char *cp;

	parse->flags &= (uint8_t) (~PARSE_PPSAPI);

	/*
	 * collect PPSAPI offset capability - should move into generic handling
	 */
	if (time_pps_getcap(parse->ppsctl.handle, &cap) < 0) {
		msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: parse_ppsapi: time_pps_getcap failed: %m",
			parse->peer->procptr->refclkunit);

		return false;
	}

	/*
	 * initialize generic PPSAPI interface
	 *
	 * we leave out CLK_FLAG3 as time_pps_kcbind()
	 * is handled here for now. Ideally this should also
	 * be part of the generic PPSAPI interface
	 */
	if (!refclock_params(parse->flags & (CLK_FLAG1|CLK_FLAG2|CLK_FLAG4), &parse->ppsctl))
		return false;

	/* nb. only turn things on, if someone else has turned something
	 *	on before we get here, leave it alone!
	 */

	if (parse->flags & PARSE_CLEAR) {
		cp = "CLEAR";
		mode_ppsoffset = PPS_OFFSETCLEAR;
	} else {
		cp = "ASSERT";
		mode_ppsoffset = PPS_OFFSETASSERT;
	}

	msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: initializing PPS to %s",
		parse->peer->procptr->refclkunit, cp);

	if (!(mode_ppsoffset & cap)) {
	  msyslog(LOG_WARNING,
		  "REFCLOCK: PARSE receiver #%u: Cannot set PPS_%sCLEAR, "
		  " this will increase jitter (PPS API capabilities=0x%x)",
		  parse->peer->procptr->refclkunit, cp, (unsigned)cap);
		mode_ppsoffset = 0;
	} else {
		if (mode_ppsoffset == PPS_OFFSETCLEAR)
			{
				parse->ppsctl.pps_params.clear_offset.tv_sec = (time_t)(-parse->ppsphaseadjust);
				parse->ppsctl.pps_params.clear_offset.tv_nsec = (long)(-1e9*(parse->ppsphaseadjust - (double)(long)parse->ppsphaseadjust));
			}

		if (mode_ppsoffset == PPS_OFFSETASSERT)
			{
				parse->ppsctl.pps_params.assert_offset.tv_sec = (time_t)(-parse->ppsphaseadjust);
				parse->ppsctl.pps_params.assert_offset.tv_nsec = (long)(-1e9*(parse->ppsphaseadjust - (double)(long)parse->ppsphaseadjust));
			}
	}

	parse->ppsctl.pps_params.mode |= mode_ppsoffset;

	if (time_pps_setparams(parse->ppsctl.handle, &parse->ppsctl.pps_params) < 0) {
	  msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: FAILED set PPS parameters: %m",
		  parse->peer->procptr->refclkunit);
		return false;
	}

	parse->flags |= PARSE_PPSAPI;
	return true;
}
#else
#define parse_hardpps(_PARSE_, _MODE_) /* empty */
#endif

/*--------------------------------------------------
 * parse_start - open the PARSE devices and initialize data for processing
 */
static bool
parse_start(
	int sysunit,
	struct peer *peer
	)
{
	unsigned int unit;
	int fd232;
	struct termios tio;		/* NEEDED FOR A LONG TIME ! */
	struct parseunit * parse;
	char parsedev[sizeof(PARSEDEVICE)+20];
	char parseppsdev[sizeof(PARSEPPSDEVICE)+20];
	parsectl_t tmp_ctl;
	unsigned int type;
	char *path, *ppspath;

	UNUSED_ARG(sysunit);

	/*
	 * get out Copyright information once
	 */
	if (!notice)
        {
		NLOG(NLOG_CLOCKINFO) /* conditional if clause for conditional syslog */
			msyslog(LOG_INFO, "REFCLOCK: NTP PARSE support: Copyright (c) 1989-2015, Frank Kardel");
		notice = 1;
	}

#ifdef ENABLE_CLASSIC_MODE
	peer->cfg.mode = (peer->procptr->refclkunit & ~0x80) >> 2;
	peer->procptr->refclkunit = peer->procptr->refclkunit & 0x03;
#endif /* ENABLE_CLASSIC_MODE */

	type = (unsigned int)CLK_TYPE(peer);
	unit = peer->procptr->refclkunit;

	if ((type == (unsigned int)~0) || (parse_clockinfo[type].cl_description == (char *)0))
	{
		msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%u: parse_start: "
                                 "unsupported clock type %d (max %d)",
			unit, CLK_REALTYPE(peer), ncltypes-1);
		return false;
	}

	/*
	 * Unit okay, attempt to open the device.
	 */
	if (peer->cfg.path)
	    path = peer->cfg.path;
	else {
	    (void) snprintf(parsedev, sizeof(parsedev), PARSEDEVICE, unit);
	    path = parsedev;
	}
	if (peer->cfg.ppspath)
	    ppspath = peer->cfg.ppspath;
	else {
	    (void) snprintf(parseppsdev, sizeof(parsedev), PARSEPPSDEVICE, unit);
	    ppspath = parseppsdev;
	}

	fd232 = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK, 0777);

	if (fd232 == -1)
	{
		msyslog(LOG_ERR,
                        "REFCLOCK: PARSE receiver #%u: parse_start: open of %s failed: %m",
                        unit, path);
		return false;
	}

	parse = emalloc_zero(sizeof(*parse));

	parse->generic = peer->procptr;	 /* link up */
	parse->generic->unitptr = (void *)parse; /* link down */

	/*
	 * Set up the structures
	 */
	parse->generic->timestarted    = current_time;
	parse->lastchange     = current_time;

	parse->flags          = 0;
	parse->pollneeddata   = 0;
	parse->laststatistic  = current_time;
	parse->lastformat     = (unsigned short)~0;	/* assume no format known */
	parse->timedata.parse_status = (unsigned short)~0;	/* be sure to mark initial status change */
	parse->lastmissed     = 0;	/* assume got everything */
	parse->ppsserial      = 0;
	parse->ppsfd	      = -1;
	parse->localdata      = (void *)0;
	parse->localstate     = 0;
	parse->kv             = (struct ctl_var *)0;

	clear_err(parse, ERR_ALL);

	parse->parse_type     = &parse_clockinfo[type];

	parse->maxunsync      = parse->parse_type->cl_maxunsync;

	parse->generic->fudgetime1 = parse->parse_type->cl_basedelay;

	parse->generic->fudgetime2 = 0.0;
	parse->ppsphaseadjust = parse->generic->fudgetime2;

	parse->generic->clockname  = parse->parse_type->cl_name;
	parse->generic->clockdesc  = parse->parse_type->cl_description;

	peer->rootdelay       = parse->parse_type->cl_rootdelay;
	peer->sstclktype      = parse->parse_type->cl_type;
	peer->precision       = sys_vars.sys_precision;

	peer->stratum         = STRATUM_REFCLOCK;

	if (peer->stratum <= 1)
	    memmove((char *)&parse->generic->refid, parse->parse_type->cl_id, REFIDLEN);
	else
	    parse->generic->refid = htonl(PARSEHSREFID);

	parse->generic->io.fd = fd232;

	parse->peer = peer;		/* marks it also as busy */

	/*
	 * configure terminal line
	 */
	if (TTY_GETATTR(fd232, &tio) == -1)
	{
		msyslog(LOG_ERR,
                    "REFCLOCK: PARSE receiver #%u: parse_start: tcgetattr(%d, &tio): %m",
                    unit, fd232);
		/* let our cleaning staff do the work */
		parse_shutdown(peer->procptr);
		return false;
	}
	else
	{
#ifndef _PC_VDISABLE
		memset((char *)tio.c_cc, 0, sizeof(tio.c_cc));
#else
		int disablec;
		errno = 0;		/* pathconf can deliver -1 without changing errno ! */

		disablec = fpathconf(parse->generic->io.fd, _PC_VDISABLE);
		if (disablec == -1 && errno)
		{
			msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: parse_start: fpathconf(fd, _PC_VDISABLE): %m", parse->peer->procptr->refclkunit);
			memset((char *)tio.c_cc, 0, sizeof(tio.c_cc)); /* best guess */
		}
		else
		    if (disablec != -1)
			memset((char *)tio.c_cc, disablec, sizeof(tio.c_cc));
#endif

		if ((parse_clockinfo[type].cl_lflag & ICANON) == 0)
		{
			tio.c_cc[VMIN]   = 1;
			tio.c_cc[VTIME]  = 0;
		}

		tio.c_cflag = (tcflag_t) parse_clockinfo[type].cl_cflag;
		tio.c_iflag = (tcflag_t) parse_clockinfo[type].cl_iflag;
		tio.c_oflag = (tcflag_t) parse_clockinfo[type].cl_oflag;
		tio.c_lflag = (tcflag_t) parse_clockinfo[type].cl_lflag;


		if ((cfsetospeed(&tio, (speed_t) parse_clockinfo[type].cl_speed) == -1) ||
		    (cfsetispeed(&tio, (speed_t) parse_clockinfo[type].cl_speed) == -1))
		{
		    msyslog(LOG_ERR,
			    "REFCLOCK: PARSE receiver #%u: parse_start: "
			    " tcset{i,o}speed(&tio, speed): %m",
			    unit);
		    /* let our cleaning staff do the work */
		    parse_shutdown(peer->procptr);
		    return false;
		}

		/*
		 * set up pps device
		 * if the PARSEPPSDEVICE can be opened that will be used
		 * for PPS else PARSEDEVICE will be used
		 */
		parse->ppsfd = open(ppspath, O_RDWR | O_NOCTTY | O_NONBLOCK, 0777);

		if (parse->ppsfd == -1)
		{
			parse->ppsfd = fd232;
		}

/*
 * Linux PPS - the old way
 */
#if defined(HAVE_LINUX_SERIAL_H)	/* Linux hack: define PPS interface */
		{
			struct serial_struct	ss;
			if (ioctl(parse->ppsfd, TIOCGSERIAL, &ss) < 0 ||
			    (
#ifdef ASYNC_LOW_LATENCY
			     ss.flags |= (int)ASYNC_LOW_LATENCY,
#endif
#ifndef HAVE_PPSAPI
#ifdef ASYNC_PPS_CD_NEG
			     ss.flags |= (int)ASYNC_PPS_CD_NEG,
#endif
#endif
			     ioctl(parse->ppsfd, TIOCSSERIAL, &ss)) < 0) {
				msyslog(LOG_NOTICE,
					"REFCLOCK: refclock_parse: TIOCSSERIAL fd %d, %m", parse->ppsfd);
				msyslog(LOG_NOTICE,
					"REFCLOCK: refclock_parse: optional PPS processing not available");
			} else {
				parse->flags    |= PARSE_PPSAPI;
#ifdef ASYNC_PPS_CD_NEG
				NLOG(NLOG_CLOCKINFO)
				  msyslog(LOG_INFO,
					  "REFCLOCK: refclock_parse: PPS detection on");
#endif
			}
		}
#endif

/*
 * PPS via PPSAPI
 */
#if defined(HAVE_PPSAPI)
		parse->hardppsstate = PARSE_HARDPPS_DISABLE;
		if (CLK_PPS(parse->peer))
		{
		  if (!refclock_ppsapi(parse->ppsfd, &parse->ppsctl))
		    {
		      msyslog(LOG_NOTICE, "REFCLOCK: PARSE receiver #%d: parse_start: could not set up PPS: %m", parse->peer->procptr->refclkunit);
		    }
		  else
		    {
		      parse_ppsapi(parse);
		    }
		}
#endif

		if (TTY_SETATTR(fd232, &tio) == -1) {
		    msyslog(LOG_ERR,
		      "REFCLOCK: PARSE receiver #%u: parse_start: tcsetattr(%d, &tio): %m",
                      unit, fd232);
		    /* let our cleaning staff do the work */
		    parse_shutdown(peer->procptr);
		    return false;
		}
	}

	/*
	 * pick correct input machine
	 */
	parse->generic->io.srcclock = peer;
	parse->generic->io.datalen = 0;

	parse->binding = init_iobinding(parse);

	if (parse->binding == (bind_t *)0)
		{
			msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: parse_start: io sub system initialisation failed.", parse->peer->procptr->refclkunit);
			parse_shutdown(peer->procptr); /* let our cleaning staff do the work */
			return false;			/* well, ok - special initialisation broke */
		}

	parse->generic->io.clock_recv = parse->binding->bd_receive; /* pick correct receive routine */
	parse->generic->io.io_input   = parse->binding->bd_io_input; /* pick correct input routine */

	/*
	 * as we always(?) get 8 bit chars we want to be
	 * sure, that the upper bits are zero for less
	 * than 8 bit I/O - so we pass that information on.
	 * note that there can be only one bit count format
	 * per file descriptor
	 */

	switch (tio.c_cflag & CSIZE)
	{
	    case CS5:
		tmp_ctl.parsesetcs.parse_cs = PARSE_IO_CS5;
		break;

	    case CS6:
		tmp_ctl.parsesetcs.parse_cs = PARSE_IO_CS6;
		break;

	    case CS7:
		tmp_ctl.parsesetcs.parse_cs = PARSE_IO_CS7;
		break;

	    case CS8:
		tmp_ctl.parsesetcs.parse_cs = PARSE_IO_CS8;
		break;

            default:
                /* huh? */
                break;
	}

	if (!PARSE_SETCS(parse, &tmp_ctl))
	{
		msyslog(LOG_ERR,
                    "REFCLOCK: PARSE receiver #%u: parse_start: parse_setcs() FAILED.",
                    unit);
		parse_shutdown(peer->procptr);	/* let our cleaning staff do the work */
		return false;			/* well, ok - special initialisation broke */
	}

	strlcpy(tmp_ctl.parseformat.parse_buffer, parse->parse_type->cl_format, sizeof(tmp_ctl.parseformat.parse_buffer));
	tmp_ctl.parseformat.parse_count = (unsigned short) strlen(tmp_ctl.parseformat.parse_buffer);

	if (!PARSE_SETFMT(parse, &tmp_ctl))
	{
		msyslog(LOG_ERR,
                    "REFCLOCK: PARSE receiver #%u: parse_start: parse_setfmt() FAILED.",
                    unit);
		parse_shutdown(peer->procptr);	/* let our cleaning staff do the work */
		return false;			/* well, ok - special initialisation broke */
	}

	/*
	 * get rid of all IO accumulated so far
	 */
	(void) tcflush(parse->generic->io.fd, TCIOFLUSH);

	/*
	 * try to do any special initializations
	 */
	if (parse->parse_type->cl_init)
		{
			if (parse->parse_type->cl_init(parse))
				{
					parse_shutdown(peer->procptr); /* let our cleaning staff do the work */
					return false;		/* well, ok - special initialisation broke */
				}
		}

	/*
	 * Insert in async io device list.
	 */
	if (!io_addclock(&parse->generic->io))
        {
		msyslog(LOG_ERR,
			"REFCLOCK: PARSE receiver #%d: parse_start: addclock %s fails (ABORT - clock type requires async io)", parse->peer->procptr->refclkunit, parsedev);
		parse_shutdown(peer->procptr); /* let our cleaning staff do the work */
		return false;
	}

	/*
	 * print out configuration
	 */
	NLOG(NLOG_CLOCKINFO)
		{
			/* conditional if clause for conditional syslog */
			msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: reference clock \"%s\" (I/O device %s, PPS device %s) added",
				parse->peer->procptr->refclkunit,
				parse->parse_type->cl_description, parsedev,
				(parse->ppsfd != parse->generic->io.fd) ? parseppsdev : parsedev);

			msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: Stratum %d, trust time %s, precision %d",
				parse->peer->procptr->refclkunit,
				parse->peer->stratum,
				l_mktime(parse->maxunsync), parse->peer->precision);

			msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: rootdelay %.6f s, phase adjustment %.6f s, PPS phase adjustment %.6f s, %s IO handling",
				parse->peer->procptr->refclkunit,
				parse->parse_type->cl_rootdelay,
				parse->generic->fudgetime1,
				parse->ppsphaseadjust,
                                parse->binding->bd_description);

			msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: Format recognition: %s", parse->peer->procptr->refclkunit,
				parse->parse_type->cl_format);
                        msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: %sPPS support",
				parse->peer->procptr->refclkunit,
				CLK_PPS(parse->peer) ? "" : "NO ");
		}

	return true;
}

/*--------------------------------------------------
 * parse_ctl - process changes on flags/time values
 */
static void
parse_ctl(
	    struct parseunit *parse,
	    const struct refclockstat *in
	    )
{
        if (in)
	{
		if (in->haveflags & (CLK_HAVEFLAG1|CLK_HAVEFLAG2|CLK_HAVEFLAG3|CLK_HAVEFLAG4))
		{
		  uint8_t mask = CLK_FLAG1|CLK_FLAG2|CLK_FLAG3|CLK_FLAG4;
		  parse->flags = (parse->flags & (uint8_t)(~mask)) | (in->flags & mask);
#if defined(HAVE_PPSAPI)
		  if (CLK_PPS(parse->peer))
		    {
		      parse_ppsapi(parse);
		    }
#endif
		}

		if (in->haveflags & CLK_HAVETIME1)
                {
		  parse->generic->fudgetime1 = in->fudgetime1;
		  msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: new phase adjustment %.6f s",
			  parse->peer->procptr->refclkunit,
			  parse->generic->fudgetime1);
		}

		if (in->haveflags & CLK_HAVETIME2)
                {
		  parse->generic->fudgetime2 = in->fudgetime2;
		  if (parse->flags & PARSE_TRUSTTIME)
		    {
		      parse->maxunsync = (unsigned long)ABS(in->fudgetime2);
		      msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: new trust time %s",
			      parse->peer->procptr->refclkunit,
			      l_mktime(parse->maxunsync));
		    }
		  else
		    {
		      parse->ppsphaseadjust = in->fudgetime2;
		      msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: new PPS phase adjustment %.6f s",
			  parse->peer->procptr->refclkunit,
			      parse->ppsphaseadjust);
#if defined(HAVE_PPSAPI)
		      if (CLK_PPS(parse->peer))
		      {
			      parse_ppsapi(parse);
		      }
#endif
		    }
		}
	}
}

/*--------------------------------------------------
 * parse_poll - called by the transmit procedure
 */
static void
parse_poll(
	int unit,
	struct peer *peer
	)
{
	struct parseunit *parse = peer->procptr->unitptr;

	if (peer != parse->peer)
	{
		msyslog(LOG_ERR,
			"REFCLOCK: PARSE receiver #%d: poll: INTERNAL: peer incorrect",
			unit);
		return;
	}

	/*
	 * Update clock stat counters
	 */
	parse->generic->polls++;

	if (parse->pollneeddata &&
	    ((int)(current_time - parse->pollneeddata) > (1<<(max(min(parse->peer->hpoll, parse->peer->ppoll), parse->peer->cfg.minpoll)))))
	{
		/*
		 * start worrying when exceeding a poll interval
		 * bad news - didn't get a response last time
		 */
		parse->lastmissed = current_time;
		parse_event(parse, CEVNT_TIMEOUT);

		ERR(ERR_NODATA)
			msyslog(LOG_WARNING, "REFCLOCK: PARSE receiver #%d: no data from device within poll interval (check receiver / wiring)", parse->peer->procptr->refclkunit);
	}

	/*
	 * we just mark that we want the next sample for the clock filter
	 */
	parse->pollneeddata = current_time;

	if (parse->parse_type->cl_poll)
	{
		parse->parse_type->cl_poll(parse);
	}

	cparse_statistics(parse);

	return;
}

#define LEN_STATES 300		/* length of state string */

/*--------------------------------------------------
 * parse_control - set fudge factors, return statistics
 */
static void
parse_control(
	int unit,
	const struct refclockstat *in,
	struct refclockstat *out,
	struct peer *peer
	)
{
	struct parseunit *parse = peer->procptr->unitptr;
	parsectl_t tmpctl;

	static char outstatus[400];	/* status output buffer */

	if (out)
	{
		out->lencode       = 0;
		out->p_lastcode    = 0;
		out->kv_list       = (struct ctl_var *)0;
	}

	if (!parse || !parse->peer)
	{
		msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: parse_control: unit invalid (UNIT INACTIVE)",
			unit);
		return;
	}

	unit = parse->peer->procptr->refclkunit;

	/*
	 * handle changes
	 */
	parse_ctl(parse, in);

	/*
	 * supply data
	 */
	if (out)
	{
		unsigned long sum = 0;
		char *tt, *start;
		int i;

		outstatus[0] = '\0';

		/*
		 * keep fudgetime2 in sync with TRUSTTIME/MAXUNSYNC flag1
		 */
		parse->generic->fudgetime2 = (parse->flags & PARSE_TRUSTTIME) ? (double)parse->maxunsync : parse->ppsphaseadjust;

		/*
		 * figure out skew between PPS and RS232 - just for informational
		 * purposes
		 */
		if (PARSE_SYNC(parse->timedata.parse_state))
		{
			if (PARSE_PPS(parse->timedata.parse_state) && PARSE_TIMECODE(parse->timedata.parse_state))
			{
				l_fp off;

				/*
				 * we have a PPS and RS232 signal - calculate the skew
				 * WARNING: assumes on TIMECODE == PULSE (timecode after pulse)
				 */
				off = parse->timedata.parse_stime;
				off -= parse->timedata.parse_ptime; /* true offset */
				tt = add_var(&out->kv_list, 80, RO);
				snprintf(tt, 80, "refclock_ppsskew=%s", lfptoms(off, 6));
			}
		}

		if (PARSE_PPS(parse->timedata.parse_state))
		{
			tt = add_var(&out->kv_list, 80, RO|DEF);
			snprintf(tt, 80, "refclock_ppstime=\"%s\"", prettydate(parse->timedata.parse_ptime));
		}

		start = tt = add_var(&out->kv_list, 128, RO|DEF);
		tt = ap(start, 128, tt, "refclock_time=\"");

		if (lfpuint(parse->timedata.parse_time) == 0)
		{
			ap(start, 128, tt, "<UNDEFINED>\"");
		}
		else
		{
			ap(start, 128, tt, "%s\"",
			    prettydate(parse->timedata.parse_time));
		}

		if (!PARSE_GETTIMECODE(parse, &tmpctl))
		{
			ERR(ERR_INTERNAL)
				msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: parse_control: parse_timecode() FAILED", unit);
		}
		else
		{
			start = tt = add_var(&out->kv_list, 512, RO|DEF);
			tt = ap(start, 512, tt, "refclock_status=\"");

			/*
			 * copy PPS flags from last read transaction (informational only)
			 */
			tmpctl.parsegettc.parse_state |= parse->timedata.parse_state &
				(PARSEB_PPS|PARSEB_S_PPS);

			(void)parsestate(tmpctl.parsegettc.parse_state, tt, BUFFER_SIZES(start, tt, 512));

			tt += strlen(tt);

			ap(start, 512, tt, "\"");

			if (tmpctl.parsegettc.parse_count)
			    mkascii(outstatus+strlen(outstatus), (int)(sizeof(outstatus)- strlen(outstatus) - 1),
				    tmpctl.parsegettc.parse_buffer, (unsigned)(tmpctl.parsegettc.parse_count));

		}

		tmpctl.parseformat.parse_format = tmpctl.parsegettc.parse_format;

		if (!PARSE_GETFMT(parse, &tmpctl))
		{
			ERR(ERR_INTERNAL)
				msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: parse_control: parse_getfmt() FAILED", unit);
		}
		else
		{
			int count = tmpctl.parseformat.parse_count - 1;

			start = tt = add_var(&out->kv_list, 80, RO|DEF);
			tt = ap(start, 80, tt, "refclock_format=\"");

			if (count > 0) {
				tt = ap(start, 80, tt, "%*.*s",
			        	count,
			        	count,
			        	tmpctl.parseformat.parse_buffer);
			}

			ap(start, 80, tt, "\"");
		}

		/*
		 * gather state statistics
		 */

		start = tt = add_var(&out->kv_list, LEN_STATES, RO|DEF);
		tt = ap(start, LEN_STATES, tt, "refclock_states=\"");

		for (i = 0; i <= CEVNT_MAX; i++)
		{
			unsigned long s_time;
			unsigned long d = current_time - parse->generic->timestarted;
			unsigned long percent;

			percent = s_time = PARSE_STATETIME(parse, i);

			while (((unsigned long)(~0) / 10000) < percent)
			{
				percent /= 10;
				d       /= 10;
			}

			if (d)
			    percent = (percent * 10000) / d;
			else
			    percent = 10000;

			if (s_time)
			{
				char item[80];
				int count;

				snprintf(item, 80, "%s%s%s: %s (%d.%02d%%)",
					sum ? "; " : "",
					(parse->generic->currentstatus == i) ? "*" : "",
					clockstatus((unsigned int)i),
					l_mktime(s_time),
					(int)(percent / 100), (int)(percent % 100));
				if ((count = (int) strlen(item)) < (LEN_STATES - 40 - (tt - start)))
					{
						tt = ap(start, LEN_STATES, tt,
						    "%s", item);
					}
				sum += s_time;
			}
		}

		tt = ap(start, LEN_STATES, tt,
		    "; running time: %s\"", l_mktime(sum));

		add_var(&out->kv_list, 32, RO);
		snprintf(tt, 32,  "refclock_id=\"%s\"", parse->parse_type->cl_id);

		add_var(&out->kv_list, 80, RO);
		snprintf(tt, 80,  "refclock_iomode=\"%s\"", parse->binding->bd_description);

		add_var(&out->kv_list, 128, RO);
		snprintf(tt, 128, "refclock_driver_version=\"%s\"", VERSION);

		{
			struct ctl_var *k;

			k = parse->kv;
			while (k && !(k->flags & EOV))
			{
				set_var(&out->kv_list, k->text, strlen(k->text)+1, k->flags);
				k++;
			}
		}

		out->lencode       = (unsigned short) strlen(outstatus);
		out->p_lastcode    = outstatus;
	}
}

/**===========================================================================
 ** processing routines
 **/

/*--------------------------------------------------
 * event handling - note that nominal events will also be posted
 * keep track of state dwelling times
 */
static void
parse_event(
	struct parseunit *parse,
	int event
	)
{
	if (parse->generic->currentstatus != (uint8_t) event)
	{
		parse->statetime[parse->generic->currentstatus] += current_time - parse->lastchange;
		parse->lastchange              = current_time;

		if (parse->parse_type->cl_event)
		    parse->parse_type->cl_event(parse, event);

		if (event == CEVNT_NOMINAL)
		{
			NLOG(NLOG_CLOCKSTATUS)
				msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: SYNCHRONIZED",
					parse->peer->procptr->refclkunit);
		}

		refclock_report(parse->peer, event);
	}
}

/*--------------------------------------------------
 * process a PARSE time sample
 */
static void
parse_process(
	struct parseunit *parse,
	parsetime_t      *parsetime
	)
{
	l_fp off, rectime = 0, reftime = 0;
	double fudge;

	/* silence warning: integral part may be used uninitialized in this function */
	ZERO(off);

	/*
	 * check for changes in conversion status
	 * (only one for each new status !)
	 */
	if (((parsetime->parse_status & CVT_MASK) != CVT_OK) &&
	    ((parsetime->parse_status & CVT_MASK) != CVT_NONE) &&
	    (parse->timedata.parse_status != parsetime->parse_status))
	{
		char buffer[400];

		NLOG(NLOG_CLOCKINFO) /* conditional if clause for conditional syslog */
			msyslog(LOG_WARNING, "REFCLOCK: PARSE receiver #%d: conversion status \"%s\"",
				parse->peer->procptr->refclkunit, parsestatus(parsetime->parse_status, buffer, sizeof(buffer)));

		if ((parsetime->parse_status & CVT_MASK) == CVT_FAIL)
		{
			/*
			 * tell more about the story - list time code
			 * there is a slight change for a race condition and
			 * the time code might be overwritten by the next packet
			 */
			parsectl_t tmpctl;

			if (!PARSE_GETTIMECODE(parse, &tmpctl))
			{
				ERR(ERR_INTERNAL)
					msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: parse_process: parse_timecode() FAILED", parse->peer->procptr->refclkunit);
			}
			else
			{
				ERR(ERR_BADDATA)
					msyslog(LOG_WARNING, "REFCLOCK: PARSE receiver #%d: FAILED TIMECODE: \"%s\" (check receiver configuration / wiring)",
						parse->peer->procptr->refclkunit, mkascii(buffer, sizeof buffer, tmpctl.parsegettc.parse_buffer, (unsigned)(tmpctl.parsegettc.parse_count - 1)));
			}
			/* copy status to show only changes in case of failures */
			parse->timedata.parse_status = parsetime->parse_status;
		}
	}

	/*
	 * examine status and post appropriate events
	 */
	if ((parsetime->parse_status & CVT_MASK) != CVT_OK)
	{
		/*
		 * got bad data - tell the rest of the system
		 */
		switch (parsetime->parse_status & CVT_MASK)
		{
		case CVT_NONE:
			if ((parsetime->parse_status & CVT_ADDITIONAL) &&
			    parse->parse_type->cl_message)
				parse->parse_type->cl_message(parse, parsetime);
			/*
			 * save PPS information that comes piggyback
			 */
			if (PARSE_PPS(parsetime->parse_state))
			  {
			    parse->timedata.parse_state |= PARSEB_PPS|PARSEB_S_PPS;
			    parse->timedata.parse_ptime  = parsetime->parse_ptime;
			  }
			break; 		/* well, still waiting - timeout is handled at higher levels */

		case CVT_FAIL:
			if (parsetime->parse_status & CVT_BADFMT)
			{
				parse_event(parse, CEVNT_BADREPLY);
			}
			else if (parsetime->parse_status & CVT_BADDATE)
			{
				parse_event(parse, CEVNT_BADDATE);
			}
			else if (parsetime->parse_status & CVT_BADTIME)
			{
				parse_event(parse, CEVNT_BADTIME);
			}
			else
			{
                                /* for the lack of something better */
				parse_event(parse, CEVNT_BADREPLY);
			}
                        break;
                default:
                        /* huh? */
                        break;
		}
		return;			/* skip the rest - useless */
	}

	/*
	 * check for format changes
	 * (in case somebody has swapped clocks 8-)
	 */
	if (parse->lastformat != parsetime->parse_format)
	{
		parsectl_t tmpctl;

		tmpctl.parseformat.parse_format = parsetime->parse_format;

		if (!PARSE_GETFMT(parse, &tmpctl))
		{
			ERR(ERR_INTERNAL)
				msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: parse_getfmt() FAILED", parse->peer->procptr->refclkunit);
		}
		else
		{
			NLOG(NLOG_CLOCKINFO) /* conditional if clause for conditional syslog */
				msyslog(LOG_INFO, "REFCLOCK: PARSE receiver #%d: packet format \"%s\"",
					parse->peer->procptr->refclkunit, tmpctl.parseformat.parse_buffer);
		}
		parse->lastformat = parsetime->parse_format;
	}

	/*
	 * now, any changes ?
	 */
	if ((parse->timedata.parse_state ^ parsetime->parse_state) &
	    ~(unsigned)(PARSEB_PPS|PARSEB_S_PPS))
	{
		char tmp1[200];
		char tmp2[200];
		/*
		 * something happened - except for PPS events
		 */

		(void) parsestate(parsetime->parse_state, tmp1, sizeof(tmp1));
		(void) parsestate(parse->timedata.parse_state, tmp2, sizeof(tmp2));

		NLOG(NLOG_CLOCKINFO) /* conditional if clause for conditional syslog */
			msyslog(LOG_INFO,"REFCLOCK: PARSE receiver #%d: STATE CHANGE: %s -> %s",
				parse->peer->procptr->refclkunit, tmp2, tmp1);
	}

	/*
	 * carry on PPS information if still usable
	 */
	if (PARSE_PPS(parse->timedata.parse_state) && !PARSE_PPS(parsetime->parse_state))
        {
	        parsetime->parse_state |= PARSEB_PPS|PARSEB_S_PPS;
		parsetime->parse_ptime  = parse->timedata.parse_ptime;
	}

	/*
	 * remember for future
	 */
	parse->timedata = *parsetime;

	/*
	 * check to see, whether the clock did a complete powerup or lost PZF signal
	 * and post correct events for current condition
	 */
	if (PARSE_POWERUP(parsetime->parse_state))
	{
		/*
		 * this is bad, as we have completely lost synchronisation
		 * well this is a problem with the receiver here
		 * for PARSE Meinberg DCF77 receivers the lost synchronisation
		 * is true as it is the powerup state and the time is taken
		 * from a crude real time clock chip
		 * for the PZF/GPS series this is only partly true, as
		 * PARSE_POWERUP only means that the pseudo random
		 * phase shift sequence cannot be found. this is only
		 * bad, if we have never seen the clock in the SYNC
		 * state, where the PHASE and EPOCH are correct.
		 * for reporting events the above business does not
		 * really matter, but we can use the time code
		 * even in the POWERUP state after having seen
		 * the clock in the synchronized state (PZF class
		 * receivers) unless we have had a telegram disruption
		 * after having seen the clock in the SYNC state. we
		 * thus require having seen the clock in SYNC state
		 * *after* having missed telegrams (noresponse) from
		 * the clock. one problem remains: we might use erroneously
		 * POWERUP data if the disruption is shorter than 1 polling
		 * interval. fortunately powerdowns last usually longer than 64
		 * seconds and the receiver is at least 2 minutes in the
		 * POWERUP or NOSYNC state before switching to SYNC
		 * for GPS receivers this can mean antenna problems and other causes.
		 * the additional grace period can be enables by a clock
		 * mode having the PARSE_F_POWERUPTRUST flag in cl_flag set.
		 */
		parse_event(parse, CEVNT_FAULT);
		NLOG(NLOG_CLOCKSTATUS)
			ERR(ERR_BADSTATUS)
			msyslog(LOG_ERR,"REFCLOCK: PARSE receiver #%d: NOT SYNCHRONIZED/RECEIVER PROBLEMS",
				parse->peer->procptr->refclkunit);
	}
	else
	{
		/*
		 * we have two states left
		 *
		 * SYNC:
		 *  this state means that the EPOCH (timecode) and PHASE
		 *  information has be read correctly (at least two
		 *  successive PARSE timecodes were received correctly)
		 *  this is the best possible state - full trust
		 *
		 * NOSYNC:
		 *  The clock should be on phase with respect to the second
		 *  signal, but the timecode has not been received correctly within
		 *  at least the last two minutes. this is a sort of half baked state
		 *  for PARSE Meinberg DCF77 clocks this is bad news (clock running
		 *  without timecode confirmation)
		 *  PZF 535 has also no time confirmation, but the phase should be
		 *  very precise as the PZF signal can be decoded
		 */

		if (PARSE_SYNC(parsetime->parse_state))
		{
			/*
			 * currently completely synchronized - best possible state
			 */
			parse->lastsync = current_time;
			clear_err(parse, ERR_BADSTATUS);
		}
		else
		{
			/*
			 * we have had some problems receiving the time code
			 */
			parse_event(parse, CEVNT_PROP);
			NLOG(NLOG_CLOCKSTATUS)
				ERR(ERR_BADSTATUS)
				msyslog(LOG_ERR,"REFCLOCK: PARSE receiver #%d: TIMECODE NOT CONFIRMED",
					parse->peer->procptr->refclkunit);
		}
	}

	fudge = parse->generic->fudgetime1; /* standard RS232 Fudgefactor */

	if (PARSE_TIMECODE(parsetime->parse_state))
	{
		rectime = parsetime->parse_stime;
		off = reftime = parsetime->parse_time;

		off -= rectime; /* prepare for PPS adjustments logic */

		DPRINT(4, ("REFCLOCK: PARSE receiver #%d: Reftime %s, Recvtime %s - initial offset %s\n",
			   parse->peer->procptr->refclkunit,
			   prettydate(reftime),
			   prettydate(rectime),
			   lfptoa(off,6)));
	}

	if (PARSE_PPS(parsetime->parse_state) && CLK_PPS(parse->peer))
	{
		l_fp offset;
		double ppsphaseadjust = parse->ppsphaseadjust;

#ifdef HAVE_PPSAPI
		/*
		 * set fudge = 0.0 if already included in PPS time stamps
		 */
		if (parse->ppsctl.pps_params.mode & (PPS_OFFSETCLEAR|PPS_OFFSETASSERT))
		        {
			        ppsphaseadjust = 0.0;
			}
#endif

		/*
		 * we have a PPS signal - much better than the RS232 stuff (we hope)
		 */
		offset = parsetime->parse_ptime;

		DPRINT(4, ("REFCLOCK: PARSE receiver #%d: PPStime %s\n",
			   parse->peer->procptr->refclkunit,
			   prettydate(offset)));
		if (PARSE_TIMECODE(parsetime->parse_state))
		{
			if (fabsl(lfptod(off)) <= 0.5)
			{
				fudge = ppsphaseadjust; /* pick PPS fudge factor */

				/*
				 * RS232 offsets within [-0.5..0.5] - take PPS offsets
				 */

				if (parse->parse_type->cl_flags & PARSE_F_PPSONSECOND)
				{
				    reftime = off = offset;
				    if (lfpfrac(reftime) & 0x80000000)
					    bumplfpuint(reftime, 1);
				    setlfpfrac(reftime, 0);


				    /*
				     * implied on second offset
				     */
                                    /* map [0.5..1[ -> [-0.5..0[ */
				    setlfpfrac(off, ~lfpfrac(off));
                                    /* sign extend */
				    setlfpuint(off,
                                      (unsigned long int)(
                                          (lfpfrac(off) & 0x80000000) ?
                                          -1 : 0));
				}
				else
				{
					/*
					 * time code describes pulse
					 */
					reftime = off = parsetime->parse_time;

					off -= offset; /* true offset */
				}
			}
			/*
			 * take RS232 offset when PPS when out of bounds
			 */
		}
		else
		{
			fudge = ppsphaseadjust; /* pick PPS fudge factor */
			/*
			 * Well, no time code to guide us - assume on second pulse
			 * and pray, that we are within [-0.5..0.5[
			 */
			off = offset;
			reftime = offset;
			if (lfpfrac(reftime) & 0x80000000)
				bumplfpuint(reftime, 1);
			setlfpfrac(reftime, 0);
			/*
			 * implied on second offset
			 */
                        /* map [0.5..1[ -> [-0.5..0[ */
			setlfpfrac(off, ~lfpfrac(off));
                        /* sign extend */
			setlfpuint(off, ((lfpfrac(off) & 0x80000000) ?
                                         (unsigned long int)-1 : 0));
		}
	}
	else
	{
		if (!PARSE_TIMECODE(parsetime->parse_state))
		{
			/*
			 * Well, no PPS, no TIMECODE, no more work ...
			 */
			if ((parsetime->parse_status & CVT_ADDITIONAL) &&
			    parse->parse_type->cl_message)
				parse->parse_type->cl_message(parse, parsetime);
			return;
		}
	}

	DPRINT(4, ("REFCLOCK: PARSE receiver #%d: Reftime %s, Recvtime %s - final offset %s\n",
		   parse->peer->procptr->refclkunit,
		   prettydate(reftime),
		   prettydate(rectime),
		   lfptoa(off,6)));


	rectime = reftime;
	rectime -= off;	/* just to keep the ntp interface happy */

	DPRINT(4, ("REFCLOCK: PARSE receiver #%d: calculated Reftime %s, Recvtime %s\n",
		   parse->peer->procptr->refclkunit,
		   prettydate(reftime),
		   prettydate(rectime)));

	if ((parsetime->parse_status & CVT_ADDITIONAL) &&
	    parse->parse_type->cl_message)
		parse->parse_type->cl_message(parse, parsetime);

	if (PARSE_SYNC(parsetime->parse_state))
	{
		/*
		 * log OK status
		 */
		parse_event(parse, CEVNT_NOMINAL);
	}

	clear_err(parse, ERR_BADIO);
	clear_err(parse, ERR_BADDATA);
	clear_err(parse, ERR_NODATA);
	clear_err(parse, ERR_INTERNAL);

	/*
	 * and now stick it into the clock machine
	 * samples are only valid iff lastsync is not too old and
	 * we have seen the clock in sync at least once
	 * after the last time we didn't see an expected data telegram
	 * at startup being not in sync is also bad just like
	 * POWERUP state unless PARSE_F_POWERUPTRUST is set
	 * see the clock states section above for more reasoning
	 */
	if (((current_time - parse->lastsync) > parse->maxunsync)           ||
	    (parse->lastsync < parse->lastmissed)                           ||
	    ((parse->lastsync == 0) && !PARSE_SYNC(parsetime->parse_state)) ||
	    (((parse->parse_type->cl_flags & PARSE_F_POWERUPTRUST) == 0) &&
	     PARSE_POWERUP(parsetime->parse_state)))
	{
		parse->generic->leap = LEAP_NOTINSYNC;
		parse->lastsync = 0;	/* wait for full sync again */
	}
	else
	{
		if (PARSE_LEAPADD(parsetime->parse_state))
		{
			/*
			 * we pick this state also for time code that pass leap warnings
			 * without direction information (as earth is currently slowing
			 * down).
			 */
			parse->generic->leap = (parse->flags & PARSE_LEAP_DELETE) ? LEAP_DELSECOND : LEAP_ADDSECOND;
		}
		else
		    if (PARSE_LEAPDEL(parsetime->parse_state))
		    {
			    parse->generic->leap = LEAP_DELSECOND;
		    }
		    else
		    {
			    parse->generic->leap = LEAP_NOWARNING;
		    }
	}

	if (parse->generic->leap != LEAP_NOTINSYNC)
	{
	        /*
		 * only good/trusted samples are interesting
		 */
	        DPRINT(3, ("REFCLOCK: PARSE receiver #%d: refclock_process_offset(reftime=%s, rectime=%s, Fudge=%f)\n",
			   parse->peer->procptr->refclkunit,
			   prettydate(reftime),
			   prettydate(rectime),
			   fudge));
		parse->generic->lastref = reftime;

		refclock_process_offset(parse->generic, reftime, rectime, fudge);

#ifdef HAVE_PPSAPI
		/*
		 * pass PPS information on to PPS clock
		 */
		if (PARSE_PPS(parsetime->parse_state) && CLK_PPS(parse->peer))
			{
				parse->peer->cfg.flags |= (FLAG_PPS | FLAG_TSTAMP_PPS);
				parse_hardpps(parse, PARSE_HARDPPS_ENABLE);
			}
#endif
	} else {
		parse_hardpps(parse, PARSE_HARDPPS_DISABLE);
		parse->peer->cfg.flags &= ~(FLAG_PPS | FLAG_TSTAMP_PPS);
	}

	/*
	 * ready, unless the machine wants a sample or
	 * we are in fast startup mode (peer->dist > MAXDISTANCE)
	 */
	if (!parse->pollneeddata && parse->peer->disp <= MAXDISTANCE)
	    return;

	parse->pollneeddata = 0;

	parse->timedata.parse_state &= ~(unsigned)(PARSEB_PPS|PARSEB_S_PPS);

	refclock_receive(parse->peer);
}

/**===========================================================================
 ** special code for special clocks
 **/

static void
mk_utcinfo(
	   char *t,  // pointer to the output string buffer
	   int wnt,
	   int wnlsf,
	   int dn,
	   int dtls,
	   int dtlsf,
	   int size  // size of the output string buffer
	   )
{
	/*
	 * The week number transmitted by the GPS satellites for the leap date
	 * is truncated to 8 bits only. If the nearest leap second date is off
	 * the current date by more than +/- 128 weeks then conversion to a
	 * calendar date is ambiguous. On the other hand, if a leap second is
	 * currently being announced (i.e. dtlsf != dtls) then the week number
	 * wnlsf is close enough, and we can unambiguously determine the date
	 * for which the leap second is scheduled.
	 */
	if ( dtlsf != dtls )
	{
		time_t t_ls;
		struct tm tmbuf, *tm;
		int n = 0;

		if (wnlsf < GPSWRAP)
			wnlsf += GPSWEEKS;

		if (wnt < GPSWRAP)
			wnt += GPSWEEKS;

		t_ls = (time_t)(wnlsf * SECSPERWEEK +
			        dn * SECSPERDAY +
			        (int)GPS_SEC_BIAS - 1);

		tm = gmtime_r( &t_ls, &tmbuf );
		if (tm == NULL)  // gmtime_r() failed
		{
			snprintf( t, (size_t)size,
                                 "** (gmtime_r() failed in mk_utcinfo())" );
			return;
		}

		n += snprintf( t, (size_t)size,
                  "UTC offset transition from %is to %is due to leap second %s",
		  dtls, dtlsf, ( dtls < dtlsf ) ? "insertion" : "deletion" );
		n += snprintf(t + n, (size_t)(size - n),
                              " at UTC midnight at the end of %04i-%02i-%02i",
			      tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
	}
	else
		snprintf( t, (size_t)size,
                    "UTC offset parameter: %is, no leap second announced.\n",
                    dtls );

}

#ifdef CLOCK_MEINBERG
/**===========================================================================
 ** Meinberg GPS receiver support
 **/

/*------------------------------------------------------------
 * gps16x_message - process messages from Meinberg GPS receiver
 */
static void
gps16x_message(
	       struct parseunit *parse,
	       parsetime_t      *parsetime
	       )
{
	if (parse->timedata.parse_msglen && parsetime->parse_msg[0] == SOH)
	{
		GPS_MSG_HDR header;
		unsigned char *bufp = (unsigned char *)parsetime->parse_msg + 1;

#ifdef DEBUG
		if (debug > 2) /* SPECIAL DEBUG */
		{
			char msgbuffer[600];

			mkreadable(msgbuffer, sizeof(msgbuffer), (char *)parsetime->parse_msg, parsetime->parse_msglen, 1);
			printf("REFCLOCK: PARSE receiver #%d: received message (%d bytes) >%s<\n",
				parse->peer->procptr->refclkunit,
				parsetime->parse_msglen,
				msgbuffer);
		}
#endif
		get_mbg_header(&bufp, &header);
		if (header.hdr_csum == mbg_csum(parsetime->parse_msg + 1, 6) &&
		    (header.len == 0 ||
		     (header.len < sizeof(parsetime->parse_msg) &&
		      header.data_csum == mbg_csum(bufp, header.len))))
		{
			/*
			 * clean message
			 */
			switch (header.cmd)
			{
			case GPS_SW_REV:
			    {
				char buffer[64];
				SW_REV gps_sw_rev;

				get_mbg_sw_rev(&bufp, &gps_sw_rev);
				snprintf(buffer, sizeof(buffer),
				    "meinberg_gps_version=\"%x.%02x%s%s\"",
				    (unsigned)((gps_sw_rev.code >> 8) & 0xFF),
				    (unsigned)(gps_sw_rev.code & 0xFF),
				    gps_sw_rev.name[0] ? " " : "",
				    gps_sw_rev.name);
				set_var(&parse->kv, buffer, strlen(buffer)+1,
                                        RO|DEF);
			    }
			break;

			case GPS_BVAR_STAT:
				{
					static struct state
					{
						BVAR_STAT flag; /* status flag */
						const char *string; /* bit name */
					} states[] =
					  {
						  { BVAR_CFGH_INVALID,     "Configuration/Health" },
						  { BVAR_ALM_NOT_COMPLETE, "Almanachs" },
						  { BVAR_UTC_INVALID,      "UTC Correction" },
						  { BVAR_IONO_INVALID,     "Ionospheric Correction" },
						  { BVAR_RCVR_POS_INVALID, "Receiver Position" },
						  { 0, "" }
					  };
					BVAR_STAT status;
					struct state *s = states;
					char buffer[512];
					char *p, *b;

					status = (BVAR_STAT)get_lsb_uint16(&bufp);
					p = b = buffer;
					p = ap(buffer, sizeof(buffer), p,
					    "meinberg_gps_status=\"[0x%04x] ",
					    status);

					if (status)
					{
						p = ap(buffer, sizeof(buffer), p, "incomplete buffered data: ");
						b = p;
						while (s->flag)
						{
							if (status & s->flag)
							{
								if (p != b)
								{
									p = ap(buffer, sizeof(buffer), p, ", ");
								}

								p = ap(buffer, sizeof(buffer), p, "%s", (const char *)s->string);
							}
							s++;
						}
						ap(buffer, sizeof(buffer), p, "\"");
					}
					else
					{
						ap(buffer, sizeof(buffer), p, "<all buffered data complete>\"");
					}

					set_var(&parse->kv, buffer, strlen(buffer)+1, RO|DEF);
				}
			break;

			case GPS_POS_XYZ:
				{
					XYZ xyz;
					char buffer[256];

					get_mbg_xyz(&bufp, xyz);
					snprintf(buffer, sizeof(buffer), "gps_position(XYZ)=\"%s m, %s m, %s m\"",
						 mfptoa(xyz[XP], 1),
						 mfptoa(xyz[YP], 1),
						 mfptoa(xyz[ZP], 1));

					set_var(&parse->kv, buffer, sizeof(buffer), RO|DEF);
				}
			break;

			case GPS_POS_LLA:
				{
					LLA lla;
					char buffer[256];

					get_mbg_lla(&bufp, lla);

					snprintf(buffer, sizeof(buffer), "gps_position(LLA)=\"%s deg, %s deg, %s m\"",
						 mfptoa(lla[LAT], 4),
						 mfptoa(lla[LON], 4),
						 mfptoa(lla[ALT], 1));

					set_var(&parse->kv, buffer, sizeof(buffer), RO|DEF);
				}
			break;

			case GPS_TZDL:
				break;

			case GPS_PORT_PARM:
				break;

			case GPS_SYNTH:
				break;

			case GPS_ANT_INFO:
				{
					ANT_INFO antinfo;
					char buffer[512];
					char *p, *q;

					get_mbg_antinfo(&bufp, &antinfo);
					p = buffer;
					p = ap(buffer, sizeof(buffer), p, "meinberg_antenna_status=\"");
					switch (antinfo.status)
					{
					case ANT_INVALID: // No other fields valid since antenna has not yet been disconnected
						p = ap(buffer, sizeof(buffer),
						    p, "<OK>");
						break;

					case ANT_DISCONN: // Antenna is disconnected, tm_reconn and delta_t not yet set
						q = ap(buffer, sizeof(buffer),
						    p, "DISCONNECTED since ");
						NLOG(NLOG_CLOCKSTATUS)
							ERR(ERR_BADSTATUS)
							msyslog(LOG_ERR,"REFCLOCK: PARSE receiver #%d: ANTENNA FAILURE: %s",
								parse->peer->procptr->refclkunit, p);

						p = q;
						mbg_tm_str(&p, &antinfo.tm_disconn, BUFFER_SIZE(buffer, p), 0);
						*p = '\0';
						break;

					case ANT_RECONN: // Antenna had been disconnect, but receiver sync. after reconnect, so all fields valid
						p = ap(buffer, sizeof(buffer),
						    p, "SYNC AFTER RECONNECT on ");
						mbg_tm_str(&p, &antinfo.tm_reconn, BUFFER_SIZE(buffer, p), 0);
						p = ap(buffer, sizeof(buffer),
							p, ", clock offset at reconnect %c%ld.%07ld s, disconnect time ",
							(antinfo.delta_t < 0) ? '-' : '+',
							(long) ABS(antinfo.delta_t) / 10000,
							(long) ABS(antinfo.delta_t) % 10000);
						mbg_tm_str(&p, &antinfo.tm_disconn, BUFFER_SIZE(buffer, p), 0);
						*p = '\0';
						break;

					default:
						p = ap(buffer, sizeof(buffer),
						    p, "bad status 0x%04x",
						    (unsigned)antinfo.status);
						break;
					}

					ap(buffer, sizeof(buffer), p, "\"");

					set_var(&parse->kv, buffer, sizeof(buffer), RO|DEF);
				}
			break;

			case GPS_UCAP:
				break;

			case GPS_CFGH:
				{
					CFGH cfgh;
					char buffer[512];
					char *p;

					get_mbg_cfgh(&bufp, &cfgh);
					if (cfgh.valid)
					{
						const char *cp;
						uint16_t tmp_val;
						int i;

						p = buffer;
						p = ap(buffer, sizeof(buffer),
						    p, "gps_tot_51=\"");
						mbg_tgps_str(&p, &cfgh.tot_51, BUFFER_SIZE(buffer, p));
						ap(buffer, sizeof(buffer),
						    p, "\"");
						set_var(&parse->kv, buffer, sizeof(buffer), RO|COND_DEF);

						p = buffer;
						p = ap(buffer, sizeof(buffer),
						    p, "gps_tot_63=\"");
						mbg_tgps_str(&p, &cfgh.tot_63, BUFFER_SIZE(buffer, p));
						ap(buffer, sizeof(buffer),
						    p, "\"");
						set_var(&parse->kv, buffer, sizeof(buffer), RO|COND_DEF);

						p = buffer;
						p = ap(buffer, sizeof(buffer),
						    p, "gps_t0a=\"");
						mbg_tgps_str(&p, &cfgh.t0a, BUFFER_SIZE(buffer, p));
						ap(buffer, sizeof(buffer),
						    p, "\"");
						set_var(&parse->kv, buffer, sizeof(buffer), RO|COND_DEF);

						for (i = 0; i < N_SVNO_GPS; i++)
						{
							p = buffer;
							p = ap(buffer, sizeof(buffer), p, "sv_info[%d]=\"PRN%d", i, i + N_SVNO_GPS);

							tmp_val = cfgh.health[i];  /* a 6 bit SV health code */
							p = ap(buffer, sizeof(buffer), p, "; health=0x%02x (", tmp_val);
							/* "All Ones" has a special meaning" */
							if (tmp_val == 0x3F) /* satellite is unusable or doesn't even exist */
								cp = "SV UNAVAILABLE";
							else {
								/* The MSB contains a summary of the 3 MSBs of the 8 bit health code,
								 * indicating if the data sent by the satellite is OK or not. */
								p = ap(buffer, sizeof(buffer), p, "DATA %s, ", (tmp_val & 0x20) ? "BAD" : "OK" );

								/* The 5 LSBs contain the status of the different signals sent by the satellite. */
								switch (tmp_val & 0x1F)
								{
									case 0x00: cp = "SIGNAL OK";              break;
									/* codes 0x01 through 0x1B indicate that one or more
									 * specific signal components are weak or dead.
									 * We don't decode this here in detail. */
									case 0x1C: cp = "SV IS TEMP OUT";         break;
									case 0x1D: cp = "SV WILL BE TEMP OUT";    break;
									default:   cp = "TRANSMISSION PROBLEMS";  break;
								}
							}
							p = ap(buffer, sizeof(buffer), p, "%s)", cp );

							tmp_val = cfgh.cfg[i];  /* a 4 bit SV configuration/type code */
							p = ap(buffer, sizeof(buffer), p, "; cfg=0x%02x (", tmp_val);
							switch (tmp_val & 0x7)
							{
								case 0x00:  cp = "(reserved)";        break;
								case 0x01:  cp = "BLOCK II/IIA/IIR";  break;
								case 0x02:  cp = "BLOCK IIR-M";       break;
								case 0x03:  cp = "BLOCK IIF";         break;
								case 0x04:  cp = "BLOCK III";         break;
								default:   cp = "unknown SV type";   break;
							}
							p = ap(buffer, sizeof(buffer), p, "%s", cp );
							if (tmp_val & 0x08)  /* A-S is on, P-code is encrypted */
								p = ap( buffer, sizeof(buffer), p, ", A-S on" );

						        ap(buffer, sizeof(buffer), p, ")\"");
							set_var(&parse->kv, buffer, sizeof(buffer), RO|COND_DEF);
						}
					}
				}
			break;

			case GPS_ALM:
				break;

			case GPS_EPH:
				break;

			case GPS_UTC:
				{
					UTC utc;
					char buffer[512];
					char *p;

					p = buffer;

					get_mbg_utc(&bufp, &utc);

					if (utc.valid)
					{
						p = ap(buffer, sizeof(buffer), p, "gps_utc_correction=\"");
						mk_utcinfo(p, utc.t0t.wn, utc.WNlsf, utc.DNt,
                                                           utc.delta_tls, utc.delta_tlsf,
                                                           (int)BUFFER_SIZE(buffer, p));
						p += strlen(p);
						ap(buffer, sizeof(buffer), p, "\"");
					}
					else
					{
						ap(buffer, sizeof(buffer), p, "gps_utc_correction=\"<NO UTC DATA>\"");
					}
					set_var(&parse->kv, buffer, sizeof(buffer), RO|DEF);
				}
			break;

			case GPS_IONO:
				break;

			case GPS_ASCII_MSG:
				{
					ASCII_MSG gps_ascii_msg;
					char buffer[128];

					get_mbg_ascii_msg(&bufp, &gps_ascii_msg);
					strlcpy(buffer, "gps_message=", sizeof(buffer));
					if (gps_ascii_msg.valid)
						{
							char buffer1[128];
							mkreadable(buffer1, sizeof(buffer1), gps_ascii_msg.s, strlen(gps_ascii_msg.s), (int)0);
							strlcat(buffer, buffer1, sizeof(buffer));
						}
					else
						strlcat(buffer, "<None>", sizeof(buffer));

					set_var(&parse->kv, buffer, sizeof(buffer), RO|DEF);
				}

			break;

			default:
				break;
			}
		}
		else
		{
			msyslog(LOG_DEBUG, "REFCLOCK: PARSE receiver #%d: gps16x_message: message checksum error: hdr_csum = 0x%x (expected 0x%x), "
			                   "data_len = %d, data_csum = 0x%x (expected 0x%x)",
				parse->peer->procptr->refclkunit,
				header.hdr_csum, mbg_csum(parsetime->parse_msg + 1, 6),
				header.len,
				header.data_csum, mbg_csum(bufp, (unsigned)((header.len < sizeof(parsetime->parse_msg)) ? header.len : 0)));
		}
	}

	return;
}

/*------------------------------------------------------------
 * gps16x_poll - query the receiver periodically
 */
static void
gps16x_poll(
	    struct peer *peer
	    )
{
	struct parseunit *parse = peer->procptr->unitptr;

	static GPS_MSG_HDR sequence[] =
	{
		{ GPS_SW_REV,          0, 0, 0 },
		{ GPS_BVAR_STAT,       0, 0, 0 },
		{ GPS_UTC,             0, 0, 0 },
		{ GPS_ASCII_MSG,       0, 0, 0 },
		{ GPS_ANT_INFO,        0, 0, 0 },
		{ GPS_CFGH,            0, 0, 0 },
		{ GPS_POS_XYZ,         0, 0, 0 },
		{ GPS_POS_LLA,         0, 0, 0 },
		{ (unsigned short)~0,  0, 0, 0 }
	};

	int rtc;
	unsigned char cmd_buffer[64];
	unsigned char *outp = cmd_buffer;
	GPS_MSG_HDR *header;

	if (((poll_info_t *)parse->parse_type->cl_data)->rate)
	{
		parse->peer->procptr->nextaction = current_time + ((poll_info_t *)parse->parse_type->cl_data)->rate;
	}

	if (sequence[parse->localstate].cmd == (unsigned short)~0)
		parse->localstate = 0;

	header = sequence + parse->localstate++;

	*outp++ = SOH;		/* start command */

	put_mbg_header(&outp, header);
	outp = cmd_buffer + 1;

	header->hdr_csum = (CSUM)mbg_csum(outp, 6);
	put_mbg_header(&outp, header);

#ifdef DEBUG
	if (debug > 2) /* SPECIAL DEBUG */
	{
	    char buffer[128];

	    mkreadable(buffer, sizeof(buffer), (char *)cmd_buffer, (unsigned)(outp - cmd_buffer), 1);
	    printf(
                "REFCLOCK: PARSE receiver #%d: transmitted message #%lu (%d bytes) >%s<\n",
		   parse->peer->procptr->refclkunit,
		   parse->localstate - 1,
		   (int)(outp - cmd_buffer),
		   buffer);
	}
#endif

	rtc = (int) write(parse->generic->io.fd, cmd_buffer, (unsigned long)(outp - cmd_buffer));

	if (rtc < 0)
	{
		ERR(ERR_BADIO)
			msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: gps16x_poll: failed to send cmd to clock: %m", parse->peer->procptr->refclkunit);
	}
	else
	if (rtc != outp - cmd_buffer)
	{
		ERR(ERR_BADIO)
			msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: gps16x_poll: failed to send cmd incomplete (%d of %d bytes sent)", parse->peer->procptr->refclkunit, rtc, (int)(outp - cmd_buffer));
	}

	clear_err(parse, ERR_BADIO);
	return;
}

/*--------------------------------------------------
 * init routine - setup timer
 */
static bool
gps16x_poll_init(
	struct parseunit *parse
	)
{
	if (((poll_info_t *)parse->parse_type->cl_data)->rate)
	{
		parse->peer->procptr->action = gps16x_poll;
		gps16x_poll(parse->peer);
	}

	return false;
}

#else
static void
gps16x_message(
	       struct parseunit *parse,
	       parsetime_t      *parsetime
	       )
{}
static bool
gps16x_poll_init(
	struct parseunit *parse
	)
{
	return true;
}
#endif /* CLOCK_MEINBERG */

/**===========================================================================
 ** clock polling support
 **/

/*--------------------------------------------------
 * direct poll routine
 */
static void
poll_dpoll(
	struct parseunit *parse
	)
{
	long rtc;
	const char *ps = ((poll_info_t *)parse->parse_type->cl_data)->string;
	long ct = (long)(((poll_info_t *)parse->parse_type->cl_data)->count);

	rtc = write(parse->generic->io.fd, ps, (size_t)ct);
	if (rtc < 0)
	{
		ERR(ERR_BADIO)
			msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: poll_dpoll: failed to send cmd to clock: %m", parse->peer->procptr->refclkunit);
	}
	else
	    if (rtc != ct)
	    {
		    ERR(ERR_BADIO)
			    msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: poll_dpoll: failed to send cmd incomplete (%ld of %ld bytes sent)", parse->peer->procptr->refclkunit, rtc, ct);
	    }
	clear_err(parse, ERR_BADIO);
}

/*--------------------------------------------------
 * periodic poll routine
 */
static void
poll_poll(
	struct peer *peer
	)
{
	struct parseunit *parse = peer->procptr->unitptr;

	if (parse->parse_type->cl_poll)
		parse->parse_type->cl_poll(parse);

	if (((poll_info_t *)parse->parse_type->cl_data)->rate)
	{
		parse->peer->procptr->nextaction = current_time + ((poll_info_t *)parse->parse_type->cl_data)->rate;
	}
}

/*--------------------------------------------------
 * init routine - setup timer
 */
static bool
poll_init(
	struct parseunit *parse
	)
{
	if (((poll_info_t *)parse->parse_type->cl_data)->rate)
	{
		parse->peer->procptr->action = poll_poll;
		poll_poll(parse->peer);
	}

	return false;
}

/**===========================================================================
 ** Trimble support
 **/

/*-------------------------------------------------------------
 * trimble TAIP init routine - setup EOL and then do poll_init.
 */
static bool
trimbletaip_init(
	struct parseunit *parse
	)
{
	struct termios tio;
	/*
	 * configure terminal line for trimble receiver
	 */
	if (TTY_GETATTR(parse->generic->io.fd, &tio) == -1)
	{
		msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: trimbletaip_init: tcgetattr(fd, &tio): %m", parse->peer->procptr->refclkunit);
		return false;
	}
	else
	{
		tio.c_cc[VEOL] = TRIMBLETAIP_EOL;

		if (TTY_SETATTR(parse->generic->io.fd, &tio) == -1)
		{
			msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: trimbletaip_init: tcsetattr(fd, &tio): %m", parse->peer->procptr->refclkunit);
			return false;
		}
	}
	return poll_init(parse);
}

/*--------------------------------------------------
 * trimble TAIP event routine - reset receiver upon data format trouble
 */
static const char *taipinit[] = {
	">FPV00000000<",
	">SRM;ID_FLAG=F;CS_FLAG=T;EC_FLAG=F;FR_FLAG=T;CR_FLAG=F<",
	">FTM00020001<",
	(char *)0
};

static void
trimbletaip_event(
	struct parseunit *parse,
	int event
	)
{
	switch (event)
	{
	    case CEVNT_BADREPLY:	/* reset on garbled input */
	    case CEVNT_TIMEOUT:		/* reset on no input */
		    {
			    const char **iv;

			    iv = taipinit;
			    while (*iv)
			    {
				    int rtc = (int) write(parse->generic->io.fd, *iv, strlen(*iv));
				    if (rtc < 0)
				    {
					    msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: trimbletaip_event: failed to send cmd to clock: %m", parse->peer->procptr->refclkunit);
					    return;
				    }
				    else
				    {
					    if (rtc != (int)strlen(*iv))
					    {
						    msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: trimbletaip_event: failed to send cmd incomplete (%d of %d bytes sent)",
							    parse->peer->procptr->refclkunit, rtc, (int)strlen(*iv));
						    return;
					    }
				    }
				    iv++;
			    }

			    NLOG(NLOG_CLOCKINFO)
				    ERR(ERR_BADIO)
				    msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: trimbletaip_event: RECEIVER INITIALIZED",
					    parse->peer->procptr->refclkunit);
		    }
		    break;

	    default:			/* ignore */
		break;
	}
}

/*
 * This driver supports the Trimble SVee Six Plus GPS receiver module.
 * It should support other Trimble receivers which use the Trimble Standard
 * Interface Protocol (see below).
 *
 * The module has a serial I/O port for command/data and a 1 pulse-per-second
 * output, about 1 microsecond wide. The leading edge of the pulse is
 * coincident with the change of the GPS second. This is the same as
 * the change of the UTC second +/- ~1 microsecond. Some other clocks
 * specifically use a feature in the data message as a timing reference, but
 * the SVee Six Plus does not do this. In fact there is considerable jitter
 * on the timing of the messages, so this driver only supports the use
 * of the PPS pulse for accurate timing. Where it is determined that
 * the offset is way off, when first starting up ntpd for example,
 * the timing of the data stream is used until the offset becomes low enough
 * (|offset| < CLOCK_MAX), at which point the pps offset is used.
 *
 * It can use either option for receiving PPS information - the 'ppsclock'
 * stream pushed onto the serial data interface to timestamp the Carrier
 * Detect interrupts, where the 1PPS connects to the CD line. This only
 * works on SunOS 4.1.x currently. To select this, define PPSPPS in
 * Config.local. The other option is to use a pulse-stretcher/level-converter
 * to convert the PPS pulse into a RS232 start pulse & feed this into another
 * tty port. To use this option, define PPSCLK in Config.local. The pps input,
 * by whichever method, is handled in ntp_loopfilter.c
 *
 * The receiver uses a serial message protocol called Trimble Standard
 * Interface Protocol (it can support others but this driver only supports
 * TSIP). Messages in this protocol have the following form:
 *
 * <DLE><id> ... <data> ... <DLE><ETX>
 *
 * Any bytes within the <data> portion of value 10 hex (<DLE>) are doubled
 * on transmission and compressed back to one on reception. Otherwise
 * the values of data bytes can be anything. The serial interface is RS-422
 * asynchronous using 9600 baud, 8 data bits with odd party (**note** 9 bits
 * in total!), and 1 stop bit. The protocol supports byte, integer, single,
 * and double datatypes. Integers are two bytes, sent most significant first.
 * Singles are IEEE754 single precision floating point numbers (4 byte) sent
 * sign & exponent first. Doubles are IEEE754 double precision floating point
 * numbers (8 byte) sent sign & exponent first.
 * The receiver supports a large set of messages, only a small subset of
 * which are used here. From driver to receiver the following are used:
 *
 *  ID    Description
 *
 *  21    Request current time
 *  22    Mode Select
 *  2C    Set/Request operating parameters
 *  2F    Request UTC info
 *  35    Set/Request I/O options

 * From receiver to driver the following are recognised:
 *
 *  ID    Description
 *
 *  41    GPS Time
 *  44    Satellite selection, PDOP, mode
 *  46    Receiver health
 *  4B    Machine code/status
 *  4C    Report operating parameters (debug only)
 *  4F    UTC correction data (used to get leap second warnings)
 *  55    I/O options (debug only)
 *
 * All others are accepted but ignored.
 *
 */

#define PI		3.1415926535898	/* lots of sig figs */
#define D2R		PI/180.0

/*-------------------------------------------------------------------
 * sendcmd, sendbyte, sendetx, sendflt implement the command
 * interface to the receiver.
 *
 * CAVEAT: the sendflt routine is byte order dependent and
 * float implementation dependent - these must be converted to portable
 * versions !
 *
 * CURRENT LIMITATION: float implementation. This runs only on systems
 * with IEEE754 floats as native floats
 */

typedef struct trimble
{
	unsigned long last_msg;	/* last message received */
	unsigned long last_reset;	/* last time a reset was issued */
	uint8_t qtracking;	/* query tracking status */
	unsigned long ctrack;		/* current tracking set */
	unsigned long ltrack;		/* last tracking set */
} trimble_t;

union uval {
	uint8_t  bd[8];
	int     iv;
	float   fv;
	double  dv;
};

struct txbuf
{
	short idx;			/* index to first unused byte */
	uint8_t *txt;			/* pointer to actual data buffer */
};

static void	sendcmd		(struct txbuf *buf, int c);
static void	sendbyte	(struct txbuf *buf, int b);
static void	sendetx		(struct txbuf *buf, struct parseunit *parse);
static void	sendflt		(struct txbuf *buf, double a);

void
sendcmd(
	struct txbuf *buf,
	int c
	)
{
	buf->txt[0] = DLE;
	buf->txt[1] = (uint8_t)c;
	buf->idx = 2;
}

void
sendbyte(
	struct txbuf *buf,
	int b
	)
{
	if (b == DLE)
	    buf->txt[buf->idx++] = DLE;
	buf->txt[buf->idx++] = (uint8_t)b;
}

void
sendetx(
	struct txbuf *buf,
	struct parseunit *parse
	)
{
	buf->txt[buf->idx++] = DLE;
	buf->txt[buf->idx++] = ETX;

	if (write(parse->generic->io.fd, buf->txt, (size_t)buf->idx) != buf->idx)
	{
		ERR(ERR_BADIO)
			msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: sendetx: failed to send cmd to clock: %m", parse->peer->procptr->refclkunit);
	}
	else
	{
#ifdef DEBUG
	  if (debug > 2) /* SPECIAL DEBUG */
	  {
		  char buffer[256];

		  mkreadable(buffer, sizeof(buffer), (char *)buf->txt,
                             (unsigned long)buf->idx, 1);
		  printf("REFCLOCK: PARSE receiver #%d: transmitted message (%d bytes) >%s<\n",
			 parse->peer->procptr->refclkunit,
			 buf->idx, buffer);
	  }
#endif
		clear_err(parse, ERR_BADIO);
	}
}

void
sendflt(
	struct txbuf *buf,
	double a
	)
{
	int i;
	union uval uval;

	uval.fv = (float) a;
#ifdef WORDS_BIGENDIAN
	for (i=0; i<=3; i++)
#else
	    for (i=3; i>=0; i--)
#endif
		sendbyte(buf, uval.bd[i]);
}

#define TRIM_POS_OPT	0x13	/* output position with high precision */
#define TRIM_TIME_OPT	0x03	/* use UTC time stamps, on second */

/*--------------------------------------------------
 * trimble TSIP setup routine
 */
static bool
trimbletsip_setup(
		  struct parseunit *parse,
		  const char *reason
		  )
{
	uint8_t buffer[256];
	struct txbuf buf;
	trimble_t *t = parse->localdata;

	if (t && t->last_reset &&
	    ((t->last_reset + TRIMBLE_RESET_HOLDOFF) > current_time)) {
		return true;	/* not yet */
	}

	if (t)
		t->last_reset = current_time;

	buf.txt = buffer;

	sendcmd(&buf, CMD_CVERSION);	/* request software versions */
	sendetx(&buf, parse);

	sendcmd(&buf, CMD_COPERPARAM);	/* set operating parameters */
	sendbyte(&buf, 4);	/* static */
	sendflt(&buf, 5.0*D2R);	/* elevation angle mask = 10 deg XXX */
	sendflt(&buf, 4.0);	/* s/n ratio mask = 6 XXX */
	sendflt(&buf, 12.0);	/* PDOP mask = 12 */
	sendflt(&buf, 8.0);	/* PDOP switch level = 8 */
	sendetx(&buf, parse);

	sendcmd(&buf, CMD_CMODESEL);	/* fix mode select */
	sendbyte(&buf, 1);	/* time transfer mode */
	sendetx(&buf, parse);

	sendcmd(&buf, CMD_CMESSAGE);	/* request system message */
	sendetx(&buf, parse);

	sendcmd(&buf, CMD_CSUPER);	/* superpacket fix */
	sendbyte(&buf, 0x2);	/* binary mode */
	sendetx(&buf, parse);

	sendcmd(&buf, CMD_CIOOPTIONS);	/* set I/O options */
	sendbyte(&buf, TRIM_POS_OPT);	/* position output */
	sendbyte(&buf, 0x00);	/* no velocity output */
	sendbyte(&buf, TRIM_TIME_OPT);	/* UTC, compute on seconds */
	sendbyte(&buf, 0x00);	/* no raw measurements */
	sendetx(&buf, parse);

	sendcmd(&buf, CMD_CUTCPARAM);	/* request UTC correction data */
	sendetx(&buf, parse);

	NLOG(NLOG_CLOCKINFO)
		ERR(ERR_BADIO)
		msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: trimbletsip_setup: RECEIVER RE-INITIALIZED (%s)", parse->peer->procptr->refclkunit, reason);

	return false;
}

/*--------------------------------------------------
 * TRIMBLE TSIP check routine
 */
static void
trimble_check(
	      struct peer *peer
	      )
{
	struct parseunit *parse = peer->procptr->unitptr;
	trimble_t *t = parse->localdata;
	uint8_t buffer[256];
	struct txbuf buf;
	buf.txt = buffer;

	if (t)
	{
		if (current_time > t->last_msg + TRIMBLETSIP_IDLE_TIME)
			(void)trimbletsip_setup(parse, "message timeout");
	}

	poll_poll(parse->peer);	/* emit query string and re-arm timer */

	if (t && t->qtracking)
	{
		unsigned long oldsats = t->ltrack & ~t->ctrack;

		t->qtracking = 0;
		t->ltrack = t->ctrack;

		if (oldsats)
		{
			int i;

			for (i = 0; oldsats; i++) {
				if (oldsats & (1U << i))
					{
						sendcmd(&buf, CMD_CSTATTRACK);
						sendbyte(&buf, i+1);	/* old sat */
						sendetx(&buf, parse);
					}
				oldsats &= ~(1U << i);
			}
		}

		sendcmd(&buf, CMD_CSTATTRACK);
		sendbyte(&buf, 0x00);	/* current tracking set */
		sendetx(&buf, parse);
	}
}

/*--------------------------------------------------
 * TRIMBLE TSIP end routine
 */
static void
trimbletsip_end(
	      struct parseunit *parse
	      )
{	trimble_t *t = parse->localdata;

	if (t)
	{
		free(t);
		parse->localdata = NULL;
	}
	parse->peer->procptr->nextaction = 0;
	parse->peer->procptr->action = NULL;
}

/*--------------------------------------------------
 * TRIMBLE TSIP init routine
 */
static bool
trimbletsip_init(
	struct parseunit *parse
	)
{
#if defined(VEOL) || defined(VEOL2)
	struct termios tio;		/* NEEDED FOR A LONG TIME ! */
	unsigned int type;
	/*
	 * allocate local data area
	 */
	if (!parse->localdata)
	{
		trimble_t *t;

		t = emalloc_zero(sizeof(trimble_t));
		parse->localdata = t;
		t->last_msg = current_time;
	}

	parse->peer->procptr->action     = trimble_check;
	parse->peer->procptr->nextaction = current_time;

	/*
	 * configure terminal line for ICANON mode with VEOL characters
	 */
	if (TTY_GETATTR(parse->generic->io.fd, &tio) == -1)
	{
		msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: trimbletsip_init: tcgetattr(%d, &tio): %m", parse->peer->procptr->refclkunit, parse->generic->io.fd);
		return false;
	}
	else
	{
		type = (unsigned int)CLK_TYPE(parse->peer);
		if ( (type != (unsigned int)~0) &&
                     (parse_clockinfo[type].cl_lflag & ICANON))
		{
#ifdef VEOL
			tio.c_cc[VEOL]  = ETX;
#endif
#ifdef VEOL2
			tio.c_cc[VEOL2]  = DLE;
#endif
		}

		if (TTY_SETATTR(parse->generic->io.fd, &tio) == -1)
		{
			msyslog(LOG_ERR, "REFCLOCK: PARSE receiver #%d: trimbletsip_init: tcsetattr(%d, &tio): %m", parse->peer->procptr->refclkunit, parse->generic->io.fd);
			return false;
		}
	}
#endif
	return trimbletsip_setup(parse, "initial startup");
}

/*------------------------------------------------------------
 * trimbletsip_event - handle Trimble events
 * simple evente handler - attempt to re-initialize receiver
 */
static void
trimbletsip_event(
	struct parseunit *parse,
	int event
	)
{
	switch (event)
	{
	    case CEVNT_BADREPLY:	/* reset on garbled input */
	    case CEVNT_TIMEOUT:		/* reset on no input */
		    (void)trimbletsip_setup(parse, "event BAD_REPLY/TIMEOUT");
		    break;

	    default:			/* ignore */
		break;
	}
}

/*
 * getflt, getint convert fields in the incoming data into the
 * appropriate type of item
 *
 * CAVEAT: these routines are currently definitely byte order dependent
 * and assume Representation(float) == IEEE754
 * These functions MUST be converted to portable versions (especially
 * converting the float representation into ntp_fp formats in order
 * to avoid floating point operations at all!
 */

static double
getflt(
	uint8_t *bp
	)
{
	union uval uval;

#ifdef WORDS_BIGENDIAN
	uval.bd[0] = *bp++;
	uval.bd[1] = *bp++;
	uval.bd[2] = *bp++;
	uval.bd[3] = *bp;
#else  /* ! WORDS_BIGENDIAN */
	uval.bd[3] = *bp++;
	uval.bd[2] = *bp++;
	uval.bd[1] = *bp++;
	uval.bd[0] = *bp;
#endif /* ! WORDS_BIGENDIAN */
	return (double)uval.fv;
}

static double
getdbl(
	uint8_t *bp
	)
{
	union uval uval;

#ifdef WORDS_BIGENDIAN
	uval.bd[0] = *bp++;
	uval.bd[1] = *bp++;
	uval.bd[2] = *bp++;
	uval.bd[3] = *bp++;
	uval.bd[4] = *bp++;
	uval.bd[5] = *bp++;
	uval.bd[6] = *bp++;
	uval.bd[7] = *bp;
#else  /* ! WORDS_BIGENDIAN */
	uval.bd[7] = *bp++;
	uval.bd[6] = *bp++;
	uval.bd[5] = *bp++;
	uval.bd[4] = *bp++;
	uval.bd[3] = *bp++;
	uval.bd[2] = *bp++;
	uval.bd[1] = *bp++;
	uval.bd[0] = *bp;
#endif /* ! WORDS_BIGENDIAN */
	return uval.dv;
}

/*--------------------------------------------------
 * trimbletsip_message - process trimble messages
 */
#define RTOD (180.0 / 3.1415926535898)
#define mb(_X_) (buffer[2+(_X_)]) /* shortcut for buffer access */

static void
trimbletsip_message(
		    struct parseunit *parse,
		    parsetime_t      *parsetime
		    )
{
	unsigned char *buffer = parsetime->parse_msg;
	unsigned int   size   = parsetime->parse_msglen;

	if ((size < 4) ||
	    (buffer[0]      != DLE) ||
	    (buffer[size-1] != ETX) ||
	    (buffer[size-2] != DLE))
	{
#ifdef DEBUG
		if (debug > 2) { /* SPECIAL DEBUG */
			size_t i;

			printf("TRIMBLE BAD packet, size %u:\n	", size);
			for (i = 0; i < size; i++) {
				printf ("%2.2x, ", buffer[i]&0xff);
				if (i%16 == 15) printf("\n\t");
			}
			printf("\n");
		}
#endif
		return;
	}
	else
	{
		unsigned short var_flag;
		trimble_t *tr = parse->localdata;
		unsigned int cmd = buffer[1];
		char pbuffer[200];
		char *t = pbuffer;
		cmd_info_t *s;

#ifdef DEBUG
		if (debug > 3) { /* SPECIAL DEBUG */
			size_t i;

			printf("TRIMBLE packet 0x%02x, size %u:\n	",
                               cmd, size);
			for (i = 0; i < size; i++) {
				printf ("%2.2x, ", buffer[i]&0xff);
				if (i%16 == 15) printf("\n\t");
			}
			printf("\n");
		}
#endif

		if (tr)
			tr->last_msg = current_time;

		s = trimble_convert(cmd, trimble_rcmds);

		if (s)
		{
			t = ap(pbuffer, sizeof(pbuffer), t, "%s=\"", s->varname);
		}
		else
		{
			DPRINT(1, ("TRIMBLE UNKNOWN COMMAND 0x%02x\n", cmd));
			return;
		}

		var_flag = (unsigned short) s->varmode;

		switch(cmd)
		{
		case CMD_RCURTIME:
			t = ap(pbuffer, sizeof(pbuffer), t, "%f, %d, %f",
				 getflt((unsigned char *)&mb(0)),
                                        getmsb_short(&mb(4)),
				 getflt((unsigned char *)&mb(6)));
			break;

		case CMD_RBEST4:
			t = ap(pbuffer, sizeof(pbuffer), t, "mode: ");
			switch (mb(0) & 0xF)
			{
			default:
				t = ap(pbuffer, sizeof(pbuffer), t,
				    "0x%x", (unsigned)(mb(0) & 0x7));
				break;

			case 1:
				t = ap(pbuffer, sizeof(pbuffer), t, "0D");
				break;

			case 3:
				t = ap(pbuffer, sizeof(pbuffer), t, "2D");
				break;

			case 4:
				t = ap(pbuffer, sizeof(pbuffer), t, "3D");
				break;
			}
			if (mb(0) & 0x10)
				t = ap(pbuffer, sizeof(pbuffer), t, "-MANUAL, ");
			else
				t = ap(pbuffer, sizeof(pbuffer), t, "-AUTO, ");

			t = ap(pbuffer, sizeof(pbuffer), t, "satellites %02d %02d %02d %02d, PDOP %.2f, HDOP %.2f, VDOP %.2f, TDOP %.2f",
				mb(1), mb(2), mb(3), mb(4),
				getflt((unsigned char *)&mb(5)),
				getflt((unsigned char *)&mb(9)),
				getflt((unsigned char *)&mb(13)),
				getflt((unsigned char *)&mb(17)));

			break;

		case CMD_RVERSION:
			t = ap(pbuffer, sizeof(pbuffer), t, "%d.%d (%d/%d/%d)",
				mb(0)&0xff, mb(1)&0xff, 1900+(mb(4)&0xff), mb(2)&0xff, mb(3)&0xff);
			break;

		case CMD_RRECVHEALTH:
		{
			static const char *msgs[] =
			{
				"Battery backup failed",
				"Signal processor error",
				"Alignment error, channel or chip 1",
				"Alignment error, channel or chip 2",
				"Antenna feed line fault",
				"Excessive ref freq. error",
				"<BIT 6>",
				"<BIT 7>"
			};

			int i, bits;

			switch (mb(0) & 0xFF)
			{
			default:
				t = ap(pbuffer, sizeof(pbuffer), t, "illegal value 0x%02x", mb(0) & 0xFF);
				break;
			case 0x00:
				t = ap(pbuffer, sizeof(pbuffer), t, "doing position fixes");
				break;
			case 0x01:
				t = ap(pbuffer, sizeof(pbuffer), t, "no GPS time yet");
				break;
			case 0x03:
				t = ap(pbuffer, sizeof(pbuffer), t, "PDOP too high");
				break;
			case 0x08:
				t = ap(pbuffer, sizeof(pbuffer), t, "no usable satellites");
				break;
			case 0x09:
				t = ap(pbuffer, sizeof(pbuffer), t, "only ONE usable satellite");
				break;
			case 0x0A:
				t = ap(pbuffer, sizeof(pbuffer), t, "only TWO usable satellites");
				break;
			case 0x0B:
				t = ap(pbuffer, sizeof(pbuffer), t, "only THREE usable satellites");
				break;
			case 0x0C:
				t = ap(pbuffer, sizeof(pbuffer), t, "the chosen satellite is unusable");
				break;
			}

			bits = mb(1) & 0xFF;

			for (i = 0; i < 8; i++)
				if (bits & (0x1<<i))
				{
					t = ap(pbuffer, sizeof(pbuffer), t, ", %s", msgs[i]);
				}
		}
		break;

		case CMD_RMESSAGE:
			mkreadable(t, (int)BUFFER_SIZE(pbuffer, t),
                                   (char *)&mb(0),
                                   (unsigned)(size - 2u -
                                              (unsigned)(&mb(0) - buffer)),
                                   0);
			break;

		case CMD_RMACHSTAT:
		{
			static const char *msgs[] =
			{
				"Synthesizer Fault",
				"Battery Powered Time Clock Fault",
				"A-to-D Converter Fault",
				"The almanac stored in the receiver is not complete and current",
				"<BIT 4>",
				"<BIT 5",
				"<BIT 6>",
				"<BIT 7>"
			};

			int i, bits;

			t = ap(pbuffer, sizeof(pbuffer), t, "machine id 0x%02x", mb(0) & 0xFF);
			bits = mb(1) & 0xFF;

			for (i = 0; i < 8; i++)
				if (bits & (0x1<<i))
				{
					t = ap(pbuffer, sizeof(pbuffer), t, ", %s", msgs[i]);
				}

			t = ap(pbuffer, sizeof(pbuffer), t, ", Superpackets %ssupported", (mb(2) & 0xFF) ? "" :"un" );
		}
		break;

		case CMD_ROPERPARAM:
			t = ap(pbuffer, sizeof(pbuffer), t, "%2x %.1f %.1f %.1f %.1f",
				mb(0), getflt((unsigned char *)&mb(1)), getflt((unsigned char *)&mb(5)),
				getflt((unsigned char *)&mb(9)), getflt((unsigned char *)&mb(13)));
			break;

		case CMD_RUTCPARAM:
		{
			float t0t = getflt((unsigned char *)&mb(14));
			short wnt = getmsb_short(&mb(18));
			short dtls = getmsb_short(&mb(12));
			short wnlsf = getmsb_short(&mb(20));
			short dn = getmsb_short(&mb(22));
			short dtlsf = getmsb_short(&mb(24));

			if ((int)t0t != 0)
			{
				mk_utcinfo(t, wnt, wnlsf, dn, dtls, dtlsf,
                                           (int)BUFFER_SIZE(pbuffer, t));
			}
			else
			{
			        t = ap(pbuffer, sizeof(pbuffer), t, "<NO UTC DATA>");
			}
		}
		break;

		case CMD_RSAT1BIAS:
			t = ap(pbuffer, sizeof(pbuffer), t, "%.1fm %.2fm/s at %.1fs",
				getflt(&mb(0)), getflt(&mb(4)), getflt(&mb(8)));
			break;

		case CMD_RIOOPTIONS:
		{
			t = ap(pbuffer, sizeof(pbuffer), t, "%02x %02x %02x %02x",
				mb(0), mb(1), mb(2), mb(3));
			if (mb(0) != TRIM_POS_OPT ||
			    mb(2) != TRIM_TIME_OPT)
			{
				(void)trimbletsip_setup(parse, "bad io options");
			}
		}
		break;

		case CMD_RSPOSXYZ:
		{
			double x = getflt((unsigned char *)&mb(0));
			double y = getflt((unsigned char *)&mb(4));
			double z = getflt((unsigned char *)&mb(8));
			double f = getflt((unsigned char *)&mb(12));

			if (f > 0.0)
			  t = ap(pbuffer, sizeof(pbuffer), t, "x= %.1fm, y= %.1fm, z= %.1fm, time_of_fix= %f sec",
				  x, y, z,
				  f);
			else
				return;
		}
		break;

		case CMD_RSLLAPOS:
		{
			double lat = getflt((unsigned char *)&mb(0));
			double lng = getflt((unsigned char *)&mb(4));
			double f   = getflt((unsigned char *)&mb(12));

			if (f > 0.0)
			  t = ap(pbuffer, sizeof(pbuffer), t, "lat %f %c, long %f %c, alt %.2fm",
				  ((lat < 0.0) ? (-lat) : (lat))*RTOD, (lat < 0.0 ? 'S' : 'N'),
				  ((lng < 0.0) ? (-lng) : (lng))*RTOD, (lng < 0.0 ? 'W' : 'E'),
				  getflt((unsigned char *)&mb(8)));
			else
				return;
		}
		break;

		case CMD_RDOUBLEXYZ:
		{
			double x = getdbl((unsigned char *)&mb(0));
			double y = getdbl((unsigned char *)&mb(8));
			double z = getdbl((unsigned char *)&mb(16));
			t = ap(pbuffer, sizeof(pbuffer), t, "x= %.1fm, y= %.1fm, z= %.1fm",
				x, y, z);
		}
		break;

		case CMD_RDOUBLELLA:
		{
			double lat = getdbl((unsigned char *)&mb(0));
			double lng = getdbl((unsigned char *)&mb(8));
			t = ap(pbuffer, sizeof(pbuffer), t, "lat %f %c, lon %f %c, alt %.2fm",
				((lat < 0.0) ? (-lat) : (lat))*RTOD, (lat < 0.0 ? 'S' : 'N'),
				((lng < 0.0) ? (-lng) : (lng))*RTOD, (lng < 0.0 ? 'W' : 'E'),
				getdbl((unsigned char *)&mb(16)));
		}
		break;

		case CMD_RALLINVIEW:
		{
			int i, sats;

			t = ap(pbuffer, sizeof(pbuffer), t, "mode: ");
			switch (mb(0) & 0x7)
			{
			default:
				t = ap(pbuffer, sizeof(pbuffer), t, "0x%x",
                                       (unsigned)(mb(0) & 0x7));
				break;

			case 3:
				t = ap(pbuffer, sizeof(pbuffer), t, "2D");
				break;

			case 4:
				t = ap(pbuffer, sizeof(pbuffer), t, "3D");
				break;
			}
			if (mb(0) & 0x8)
				t = ap(pbuffer, sizeof(pbuffer), t, "-MANUAL, ");
			else
				t = ap(pbuffer, sizeof(pbuffer), t, "-AUTO, ");

			sats = (mb(0)>>4) & 0xF;

			t = ap(pbuffer, sizeof(pbuffer), t, "PDOP %.2f, HDOP %.2f, VDOP %.2f, TDOP %.2f, %d satellite%s in view: ",
				getflt((unsigned char *)&mb(1)),
				getflt((unsigned char *)&mb(5)),
				getflt((unsigned char *)&mb(9)),
				getflt((unsigned char *)&mb(13)),
				sats, (sats == 1) ? "" : "s");

			for (i=0; i < sats; i++)
			{
				t = ap(pbuffer, sizeof(pbuffer), t, "%s%02d", i ? ", " : "", mb(17+i));
				if (tr)
					tr->ctrack |= (1U << (mb(17+i)-1));
			}

			if (tr)
			{	/* mark for tracking status query */
				tr->qtracking = 1;
			}
		}
		break;

		case CMD_RSTATTRACK:
		{
			t = ap(pbuffer, sizeof(pbuffer), t-2, "[%02d]=\"", mb(0)); /* add index to var name */
			if (getflt((unsigned char *)&mb(4)) < 0.0)
			{
				t = ap(pbuffer, sizeof(pbuffer), t, "<NO MEASUREMENTS>");
				var_flag &= (unsigned short)(~DEF);
			}
			else
			{
				t = ap(pbuffer, sizeof(pbuffer), t, "ch=%d, acq=%s, eph=%d, signal_level= %5.2f, elevation= %5.2f, azimuth= %6.2f",
					(mb(1) & 0xFF)>>3,
					mb(2) ? ((mb(2) == 1) ? "ACQ" : "SRCH") : "NEVER",
					mb(3),
					getflt((unsigned char *)&mb(4)),
					getflt((unsigned char *)&mb(12)) * RTOD,
					getflt((unsigned char *)&mb(16)) * RTOD);
				if (mb(20))
				{
					var_flag &= (unsigned short)(~DEF);
					t = ap(pbuffer, sizeof(pbuffer), t, ", OLD");
				}
				if (mb(22))
				{
					if (mb(22) == 1)
						t = ap(pbuffer, sizeof(pbuffer), t, ", BAD PARITY");
					else
						if (mb(22) == 2)
							t = ap(pbuffer, sizeof(pbuffer), t, ", BAD EPH HEALTH");
				}
				if (mb(23))
					t = ap(pbuffer, sizeof(pbuffer), t, ", collecting data");
			}
		}
		break;

		default:
			t = ap(pbuffer, sizeof(pbuffer), t, "<UNDECODED>");
			break;
		}

		ap(pbuffer, sizeof(pbuffer), t, "\"");
		set_var(&parse->kv, pbuffer, sizeof(pbuffer), var_flag);
	}
}


/**============================================================
 ** RAWDCF support
 **/

/*--------------------------------------------------
 * rawdcf_init_1 - set up modem lines for RAWDCF receivers
 * SET DTR line
 */
#if defined(TIOCMSET) && (defined(TIOCM_DTR) || defined(CIOCM_DTR))
static bool
rawdcf_init_1(
	struct parseunit *parse
	)
{
	/* fixed 2000 for using with Linux by Wolfram Pienkoss <wp@bszh.de> */
	/*
	 * You can use the RS232 to supply the power for a DCF77 receiver.
	 * Here a voltage between the DTR and the RTS line is used. Unfortunately
	 * the name has changed from CIOCM_DTR to TIOCM_DTR recently.
	 */
	int sl232;

	if (ioctl(parse->generic->io.fd, TIOCMGET, (void *)&sl232) == -1)
	{
		msyslog(LOG_NOTICE, "REFCLOCK: PARSE receiver #%d: rawdcf_init_1: WARNING: ioctl(fd, TIOCMGET, [C|T]IOCM_DTR): %m", parse->peer->procptr->refclkunit);
		return 0;
	}

#ifdef TIOCM_DTR
	sl232 = (sl232 & ~TIOCM_RTS) | TIOCM_DTR;	/* turn on DTR, clear RTS for power supply */
#else
	sl232 = (sl232 & ~CIOCM_RTS) | CIOCM_DTR;	/* turn on DTR, clear RTS for power supply */
#endif

	if (ioctl(parse->generic->io.fd, TIOCMSET, (void *)&sl232) == -1)
	{
		msyslog(LOG_NOTICE, "REFCLOCK: PARSE receiver #%d: rawdcf_init_1: WARNING: ioctl(fd, TIOCMSET, [C|T]IOCM_DTR): %m", parse->peer->procptr->refclkunit);
	}
	return 0;
}
#else
static int
rawdcfdtr_init_1(
	struct parseunit *parse
	)
{
	msyslog(LOG_NOTICE, "REFCLOCK: PARSE receiver #%d: rawdcf_init_1: WARNING: OS interface incapable of setting DTR to power DCF modules", parse->peer->procptr->refclkunit);
	return 0;
}
#endif  /* DTR initialisation type */

/*--------------------------------------------------
 * rawdcf_init_2 - set up modem lines for RAWDCF receivers
 * CLR DTR line, SET RTS line
 */
#if defined(TIOCMSET) &&  (defined(TIOCM_RTS) || defined(CIOCM_RTS))
static bool
rawdcf_init_2(
	struct parseunit *parse
	)
{
	/* fixed 2000 for using with Linux by Wolfram Pienkoss <wp@bszh.de> */
	/*
	 * You can use the RS232 to supply the power for a DCF77 receiver.
	 * Here a voltage between the DTR and the RTS line is used. Unfortunately
	 * the name has changed from CIOCM_DTR to TIOCM_DTR recently.
	 */
	int sl232;

	if (ioctl(parse->generic->io.fd, TIOCMGET, (void *)&sl232) == -1)
	{
		msyslog(LOG_NOTICE, "REFCLOCK: PARSE receiver #%d: rawdcf_init_2: WARNING: ioctl(fd, TIOCMGET, [C|T]IOCM_RTS): %m", parse->peer->procptr->refclkunit);
		return 0;
	}

#ifdef TIOCM_RTS
	sl232 = (sl232 & ~TIOCM_DTR) | TIOCM_RTS;	/* turn on RTS, clear DTR for power supply */
#else
	sl232 = (sl232 & ~CIOCM_DTR) | CIOCM_RTS;	/* turn on RTS, clear DTR for power supply */
#endif

	if (ioctl(parse->generic->io.fd, TIOCMSET, (void *)&sl232) == -1)
	{
		msyslog(LOG_NOTICE, "REFCLOCK: PARSE receiver #%d: rawdcf_init_2: WARNING: ioctl(fd, TIOCMSET, [C|T]IOCM_RTS): %m", parse->peer->procptr->refclkunit);
	}
	return 0;
}
#else
static int
rawdcf_init_2(
	struct parseunit *parse
	)
{
	msyslog(LOG_NOTICE, "REFCLOCK: PARSE receiver #%d: rawdcf_init_2: WARNING: OS interface incapable of setting RTS to power DCF modules", parse->peer->procptr->refclkunit);
	return 0;
}
#endif  /* DTR initialisation type */

/*
 * History:
 *
 * refclock_parse.c,v
 * Revision 4.81  2009/05/01 10:15:29  kardel
 * use new refclock_ppsapi interface
 *
 * Revision 4.80  2007/08/11 12:06:29  kardel
 * update comments wrt/ to PPS
 *
 * Revision 4.79  2007/08/11 11:52:23  kardel
 * - terminate io bindings before io_closeclock() will close our file descriptor
 *
 * Revision 4.78  2006/12/22 20:08:27  kardel
 * Bug 746 (RFE): add configuration for Expert mouseCLOCK USB v2.0 as mode 19
 *
 * Revision 4.77  2006/08/05 07:44:49  kardel
 * support optionally separate PPS devices via /dev/refclockpps-{0..3}
 *
 * Revision 4.76  2006/06/22 18:40:47  kardel
 * clean up signedness (gcc 4)
 *
 * Revision 4.75  2006/06/22 16:58:10  kardel
 * Bug #632: call parse_ppsapi() in parse_ctl() when updating
 * the PPS offset. Fix sign of offset passed to kernel.
 *
 * Revision 4.74  2006/06/18 21:18:37  kardel
 * NetBSD Coverity CID 3796: possible NULL deref
 *
 * Revision 4.73  2006/05/26 14:23:46  kardel
 * cleanup of copyright info
 *
 * Revision 4.72  2006/05/26 14:19:43  kardel
 * cleanup of ioctl cruft
 *
 * Revision 4.71  2006/05/26 14:15:57  kardel
 * delay adding refclock to async refclock io after all initializations
 *
 * Revision 4.70  2006/05/25 18:20:50  kardel
 * bug #619
 * terminate parse io engine after de-registering
 * from refclock io engine
 *
 * Revision 4.69  2006/05/25 17:28:02  kardel
 * complete refclock io structure initialization *before* inserting it into the
 * refclock input machine (avoids null pointer deref) (bug #619)
 *
 * Revision 4.68  2006/05/01 17:02:51  kardel
 * copy receiver method also for newlwy created receive buffers
 *
 * Revision 4.67  2006/05/01 14:37:29  kardel
 * If an input buffer parses into more than one message do insert the
 * parsed message in a new input buffer instead of processing it
 * directly. This avoids deed complicated processing in signal
 * handling.
 *
 * Revision 4.66  2006/03/18 00:45:30  kardel
 * coverity fixes found in NetBSD coverity scan
 *
 * Revision 4.65  2006/01/26 06:08:33  kardel
 * output errno on PPS setup failure
 *
 * Revision 4.64  2005/11/09 20:44:47  kardel
 * utilize full PPS timestamp resolution from PPS API
 *
 * Revision 4.63  2005/10/07 22:10:25  kardel
 * bounded buffer implementation
 *
 * Revision 4.62.2.2  2005/09/25 10:20:16  kardel
 * avoid unexpected buffer overflows due to sprintf("%f") on strange floats:
 * replace almost all str* and *printf functions be their buffer bounded
 * counterparts
 *
 * Revision 4.62.2.1  2005/08/27 16:19:27  kardel
 * limit re-set rate of trimble clocks
 *
 * Revision 4.62  2005/08/06 17:40:00  kardel
 * cleanup size handling wrt/ to buffer boundaries
 *
 * Revision 4.61  2005/07/27 21:16:19  kardel
 * fix a long (> 11 years) misconfiguration wrt/ Meinberg cflag factory
 * default setup. CSTOPB was missing for the 7E2 default data format of
 * the DCF77 clocks.
 *
 * Revision 4.60  2005/07/17 21:14:44  kardel
 * change contents of version string to include the RCS/CVS Id
 *
 * Revision 4.59  2005/07/06 06:56:38  kardel
 * syntax error
 *
 * Revision 4.58  2005/07/04 13:10:40  kardel
 * fix bug 455: tripping over NULL pointer on cleanup
 * fix shadow storage logic for ppsphaseadjust and trustime wrt/ time2
 * fix compiler warnings for some platforms wrt/ printf formatstrings and
 *     varying structure element sizes
 * reorder assignment in binding to avoid tripping over NULL pointers
 *
 * Revision 4.57  2005/06/25 09:25:19  kardel
 * sort out log output sequence
 *
 * Revision 4.56  2005/06/14 21:47:27  kardel
 * collect samples only if samples are ok (sync or trusted flywheel)
 * propagate pps phase adjustment value to kernel via PPSAPI to help HARDPPS
 * en- and dis-able HARDPPS in correlation to receiver sync state
 *
 * Revision 4.55  2005/06/02 21:28:31  kardel
 * clarify trust logic
 *
 * Revision 4.54  2005/06/02 17:06:49  kardel
 * change status reporting to use fixed refclock_report()
 *
 * Revision 4.53  2005/06/02 16:33:31  kardel
 * fix acceptance of clocks unsync clocks right at start
 *
 * Revision 4.52  2005/05/26 21:55:06  kardel
 * cleanup status reporting
 *
 * Revision 4.51  2005/05/26 19:19:14  kardel
 * implement fast refclock startup
 *
 * Revision 4.50  2005/04/16 20:51:35  kardel
 * set hardpps_enable = 1 when binding a kernel PPS source
 *
 * Revision 4.49  2005/04/16 17:29:26  kardel
 * add non polling clock type 18 for just listenning to Meinberg clocks
 *
 * Revision 4.48  2005/04/16 16:22:27  kardel
 * bk sync 20050415 ntp-dev
 *
 * Revision 4.47  2004/11/29 10:42:48  kardel
 * bk sync ntp-dev 20041129
 *
 * Revision 4.46  2004/11/29 10:26:29  kardel
 * keep fudgetime2 in sync with trusttime/ppsphaseadjust depending in flag1
 *
 * Revision 4.45  2004/11/14 20:53:20  kardel
 * clear PPS flags after using them
 *
 * Revision 4.44  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.43  2001/05/26 22:53:16  kardel
 * 20010526 reconciliation
 *
 * Revision 4.42  2000/05/14 15:31:51  kardel
 * PPSAPI && RAWDCF modemline support
 *
 * Revision 4.41  2000/04/09 19:50:45  kardel
 * fixed rawdcfdtr_init() -> rawdcf_init_1
 *
 * Revision 4.40  2000/04/09 15:27:55  kardel
 * modem line fiddle in rawdcf_init_2
 *
 * Revision 4.39  2000/03/18 09:16:55  kardel
 * PPSAPI integration
 *
 * Revision 4.38  2000/03/05 20:25:06  kardel
 * support PPSAPI
 *
 * Revision 4.37  2000/03/05 20:11:14  kardel
 * 4.0.99g reconciliation
 *
 * Revision 4.36  1999/11/28 17:18:20  kardel
 * disabled burst mode
 *
 * Revision 4.35  1999/11/28 09:14:14  kardel
 * RECON_4_0_98F
 *
 * Revision 4.34  1999/05/14 06:08:05  kardel
 * store current_time in a suitable container (unsigned long)
 *
 * Revision 4.33  1999/05/13 21:48:38  kardel
 * double the no response timeout interval
 *
 * Revision 4.32  1999/05/13 20:09:13  kardel
 * complain only about missing polls after a full poll interval
 *
 * Revision 4.31  1999/05/13 19:59:32  kardel
 * add clock type 16 for RTS set DTR clr in RAWDCF
 *
 * Revision 4.30  1999/02/28 20:36:43  kardel
 * fixed printf fmt
 *
 * Revision 4.29  1999/02/28 19:58:23  kardel
 * updated copyright information
 *
 * Revision 4.28  1999/02/28 19:01:50  kardel
 * improved debug out on sent Meinberg messages
 *
 * Revision 4.27  1999/02/28 18:05:55  kardel
 * no linux/ppsclock.h stuff
 *
 * Revision 4.26  1999/02/28 15:27:27  kardel
 * wharton clock integration
 *
 * Revision 4.25  1999/02/28 14:04:46  kardel
 * added missing double quotes to UTC information string
 *
 * Revision 4.24  1999/02/28 12:06:50  kardel
 * (parse_control): using gmprettydate instead of prettydate()
 * (mk_utcinfo): new function for formatting GPS derived UTC information
 * (gps16x_message): changed to use mk_utcinfo()
 * (trimbletsip_message): changed to use mk_utcinfo()
 * ignoring position information in unsynchronized mode
 * (parse_start): augument linux support for optional ASYNC_LOW_LATENCY
 *
 * Revision 4.23  1999/02/23 19:47:53  kardel
 * fixed #endifs
 * (stream_receive): fixed formats
 *
 * Revision 4.22  1999/02/22 06:21:02  kardel
 * use new autoconfig symbols
 *
 * Revision 4.21  1999/02/21 12:18:13  kardel
 * 4.91f reconciliation
 *
 * Revision 4.20  1999/02/21 10:53:36  kardel
 * initial Linux PPSkit version
 *
 * Revision 4.19  1999/02/07 09:10:45  kardel
 * clarify STREAMS mitigation rules in comment
 *
 * Revision 4.18  1998/12/20 23:45:34  kardel
 * fix types and warnings
 *
 * Revision 4.17  1998/11/15 21:24:51  kardel
 * cannot access mbg_ routines when CLOCK_MEINBERG
 * is not defined
 *
 * Revision 4.16  1998/11/15 20:28:17  kardel
 * Release 4.0.73e13 reconciliation
 *
 * Revision 4.15  1998/08/22 21:56:08  kardel
 * fixed IO handling for non-STREAM IO
 *
 * Revision 4.14  1998/08/16 19:00:48  kardel
 * (gps16x_message): reduced UTC parameter information (dropped A0,A1)
 * made uval a local variable (killed one of the last globals)
 * (sendetx): added logging of messages when in debug mode
 * (trimble_check): added periodic checks to facilitate re-initialization
 * (trimbletsip_init): made use of EOL character if in non-kernel operation
 * (trimbletsip_message): extended message interpretation
 * (getdbl): fixed data conversion
 *
 * Revision 4.13  1998/08/09 22:29:13  kardel
 * Trimble TSIP support
 *
 * Revision 4.12  1998/07/11 10:05:34  kardel
 * Release 4.0.73d reconciliation
 *
 * Revision 4.11  1998/06/14 21:09:42  kardel
 * Sun acc cleanup
 *
 * Revision 4.10  1998/06/13 12:36:45  kardel
 * signed/unsigned, name clashes
 *
 * Revision 4.9  1998/06/12 15:30:00  kardel
 * prototype fixes
 *
 * Revision 4.8  1998/06/12 11:19:42  kardel
 * added direct input processing routine for refclocks in
 * order to avaiod that single character io gobbles up all
 * receive buffers and drops input data. (Problem started
 * with fast machines so a character a buffer was possible
 * one of the few cases where faster machines break existing
 * allocation algorithms)
 *
 * Revision 4.7  1998/06/06 18:35:20  kardel
 * (parse_start): added BURST mode initialisation
 *
 * Revision 4.6  1998/05/27 06:12:46  kardel
 * RAWDCF_BASEDELAY default added
 * old comment removed
 * casts for ioctl()
 *
 * Revision 4.5  1998/05/25 22:05:09  kardel
 * RAWDCF_SETDTR option removed
 * clock type 14 attempts to set DTR for
 * power supply of RAWDCF receivers
 *
 * Revision 4.4  1998/05/24 16:20:47  kardel
 * updated comments referencing Meinberg clocks
 * added RAWDCF clock with DTR set option as type 14
 *
 * Revision 4.3  1998/05/24 10:48:33  kardel
 * calibrated CONRAD RAWDCF default fudge factor
 *
 * Revision 4.2  1998/05/24 09:59:35  kardel
 * corrected version information (ntpq support)
 *
 * Revision 4.1  1998/05/24 09:52:31  kardel
 * use fixed format only (new IO model)
 * output debug to stdout instead of msyslog()
 * don't include >"< in ASCII output in order not to confuse
 * ntpq parsing
 *
 * Revision 4.0  1998/04/10 19:52:11  kardel
 * Start 4.0 release version numbering
 *
 * Revision 1.2  1998/04/10 19:28:04  kardel
 * initial NTP VERSION 4 integration of PARSE with GPS166 binary support
 * derived from 3.105.1.2 from V3 tree
 *
 * Revision information 3.1 - 3.105 from log deleted 1998/04/10 kardel
 *
 */
