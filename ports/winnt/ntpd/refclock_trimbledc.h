/*
 * refclock_trimbledc - clock driver for the Trimble Data Collector compatible
 * GPS receivers (4000, 4700, 4800, 7400, ...)
 *
 * Greg Brackley (greg@trimble.co.nz)
 */

#ifndef _REFCLOCK_TRIMBLEDC_H
#define _REFCLOCK_TRIMBLEDC_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined HAVE_SYS_MODEM_H
#include <sys/modem.h>
#define TIOCMSET MCSETA
#define TIOCMGET MCGETA
#define TIOCM_RTS MRTS
#endif

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_control.h"
#include "ntp_refclock.h"
#include "ntp_unixtime.h"
#include "ntp_stdlib.h"

/*
 * GPS Definitions
 */
#define	DESCRIPTION	"Trimble Data Collector GPS" /* Long name */
#define	PRECISION	(-20)	/* precision assumed (about 1 us) */
#define	REFID		"GPS\0"	/* reference ID */
#define NSAMPLES		1
#define NSKEEP			1
#define TRIMBLEDC_MINPOLL	4
#define TRIMBLEDC_MAXPOLL	4

/*
 * I/O Definitions
 */
#if !defined SYS_WINNT
#define	DEVICE		"/dev/trimbledc%d" 	/* device name and unit */
#else
#define	DEVICE		"\\\\.\\COM%d" 		/* device name and unit */
#endif
#define	SPEED232	B9600		  	/* uart speed (9600 baud) */

#define POLL_AVG	10  /* number of samples to average code delay */
#define DC_IO_BUFSIZE	512 /* IO buffer */
#define DC_MAXLEN	260  /* maximum length TSIP packet */

/* 
 * Leap-Insert and Leap-Delete are encoded as follows:
 * 	PALISADE_UTC_TIME set   and PALISADE_LEAP_PENDING set: INSERT leap
 * 	PALISADE_UTC_TIME clear and PALISADE_LEAP_PENDING set: DELETE leap
 */

#define PALISADE_LEAP_WARNING    0x04 /* GPS Leap Warning (see ICD-200) */
#define PALISADE_LEAP_PENDING    0x02 /* Leap Pending (24 hours) */
#define PALISADE_UTC_TIME        0x01 /* UTC time available */

#define mb(_X_) (up->rpt_buf[(_X_ + 1)]) /* shortcut for buffer access */

/* Conversion Definitions */
#define GPS_PI 		(3.1415926535898)
#define	R2D		(180.0/GPS_PI)

typedef enum TrimbleDcParseState {

  TrimbleParseStateAck,
  TrimbleParseStateNak,
  TrimbleParseStateData,

} TrimbleDcParseState;

struct trimbledc_unit {
	int		unit;		/* NTP refclock unit number */
	int 		polled;		/* flag to detect noreplies */
	char		leap_status;	/* leap second flag */
	TrimbleDcParseState		rpt_status;	
	short 		rpt_cnt;	
	char 		rpt_buf[DC_MAXLEN]; 	
	char		io_buf[DC_IO_BUFSIZE];
	short 		io_ptr[2];	/* 0 = head, 1 = tail */
};

#endif /* _REFCLOCK_TRIMBLEDC_H */
