/*
 * refclock_nmea.c - clock driver for an NMEA GPS CLOCK
 *		Michael Petry Jun 20, 1994
 *		 based on refclock_heathn.c
 *
 * Updated to add support for Accord GPS Clock
 * 		Venu Gopal Dec 05, 2007
 * 		neo.venu@gmail.com, venugopal_d@pgad.gov.in
 *
 * Updated to process 'time1' fudge factor
 *		Venu Gopal May 05, 2008
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(REFCLOCK) && defined(CLOCK_NMEA)

#include <stdio.h>
#include <ctype.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_unixtime.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"

#ifdef HAVE_PPSAPI
# include "ppsapi_timepps.h"
#endif /* HAVE_PPSAPI */

#ifdef SYS_WINNT
#undef write	/* ports/winnt/include/config.h: #define write _write */
extern int async_write(int, const void *, unsigned int);
#define write(fd, data, octets)	async_write(fd, data, octets)
#endif

/*
 * This driver supports NMEA-compatible GPS receivers
 *
 * Prototype was refclock_trak.c, Thanks a lot.
 *
 * The receiver used spits out the NMEA sentences for boat navigation.
 * And you thought it was an information superhighway.  Try a raging river
 * filled with rapids and whirlpools that rip away your data and warp time.
 *
 * If HAVE_PPSAPI is defined code to use the PPSAPI will be compiled in.
 * On startup if initialization of the PPSAPI fails, it will fall back
 * to the "normal" timestamps.
 *
 * The PPSAPI part of the driver understands fudge flag2 and flag3. If
 * flag2 is set, it will use the clear edge of the pulse. If flag3 is
 * set, kernel hardpps is enabled.
 *
 * GPS sentences other than RMC (the default) may be enabled by setting
 * the relevent bits of 'mode' in the server configuration line
 * server 127.127.20.x mode X
 * 
 * bit 0 - enables RMC (1)
 * bit 1 - enables GGA (2)
 * bit 2 - enables GLL (4)
 * bit 3 - enables ZDA (8) - Standard Time & Date
 * bit 3 - enables ZDG (8) - Accord GPS Clock's custom sentence with GPS time 
 *			     very close to standard ZDA
 * 
 * Multiple sentences may be selected except when ZDG/ZDA is selected.
 *
 * bit 4/5/6 - selects the baudrate for serial port :
 *		0 for 4800 (default) 
 *		1 for 9600 
 *		2 for 19200 
 *		3 for 38400 
 *		4 for 57600 
 *		5 for 115200 
 */
#define NMEA_MESSAGE_MASK_OLD    0x07
#define NMEA_MESSAGE_MASK_SINGLE 0x08
#define NMEA_MESSAGE_MASK        (NMEA_MESSAGE_MASK_OLD | NMEA_MESSAGE_MASK_SINGLE)

#define NMEA_BAUDRATE_MASK       0x70
#define NMEA_BAUDRATE_SHIFT      4

/*
 * Definitions
 */
#ifdef SYS_WINNT
# define DEVICE "COM%d:" 	/* COM 1 - 3 supported */
#else
# define DEVICE	"/dev/gps%d"	/* name of radio device */
#endif
#define	SPEED232	B4800	/* uart speed (4800 bps) */
#define	PRECISION	(-9)	/* precision assumed (about 2 ms) */
#define	PPS_PRECISION	(-20)	/* precision assumed (about 1 us) */
#define	REFID		"GPS\0"	/* reference id */
#define	DESCRIPTION	"NMEA GPS Clock" /* who we are */
#define NANOSECOND	1000000000 /* one second (ns) */
#define RANGEGATE	500000	/* range gate (ns) */

/*
 * Unit control structure
 */
struct nmeaunit {
	int	pollcnt;	/* poll message counter */
	int	polled;		/* Hand in a sample? */
	l_fp	tstamp;		/* timestamp of last poll */
	int	gps_time;	/* 0 UTC, 1 GPS time */
#ifdef HAVE_PPSAPI
	struct timespec ts;	/* last timestamp */
	pps_params_t pps_params; /* pps parameters */
	pps_info_t pps_info;	/* last pps data */
	pps_handle_t handle;	/* pps handlebars */
#endif /* HAVE_PPSAPI */
};

/*
 * Function prototypes
 */
static	int	nmea_start	(int, struct peer *);
static	void	nmea_shutdown	(int, struct peer *);
#ifdef HAVE_PPSAPI
static	void	nmea_control	(int, struct refclockstat *, struct
				    refclockstat *, struct peer *);
static	int	nmea_ppsapi	(struct peer *, int, int, double *);
static	int	nmea_pps	(struct nmeaunit *, l_fp *);
#endif /* HAVE_PPSAPI */
static	void	nmea_receive	(struct recvbuf *);
static	void	nmea_poll	(int, struct peer *);
static	void	gps_send	(int, const char *, struct peer *);
static	char	*field_parse	(char *, int);
static	int	nmea_checksum_ok(const char *);

/*
 * Transfer vector
 */
struct	refclock refclock_nmea = {
	nmea_start,		/* start up driver */
	nmea_shutdown,	/* shut down driver */
	nmea_poll,		/* transmit poll message */
#ifdef HAVE_PPSAPI
	nmea_control,		/* fudge control */
#else
	noentry,		/* fudge control */
#endif /* HAVE_PPSAPI */
	noentry,		/* initialize driver */
	noentry,		/* buginfo */
	NOFLAGS			/* not used */
};

/*
 * nmea_start - open the GPS devices and initialize data for processing
 */
static int
nmea_start(
	int unit,
	struct peer *peer
	)
{
	register struct nmeaunit *up;
	struct refclockproc *pp;
	int fd;
	char device[20];
	int baudrate;
	char *baudtext;

	/*
	 * Open serial port. Use CLK line discipline, if available.
	 */
	(void)sprintf(device, DEVICE, unit);
	
	/*
	 * Opening the serial port with appropriate baudrate
	 * based on the value of bit 4/5/6
	 */
	switch ((peer->ttl & NMEA_BAUDRATE_MASK) >> NMEA_BAUDRATE_SHIFT) {
	    case 0:
	    case 6:
	    case 7:
	    default:
		baudrate = SPEED232;
		baudtext = "4800";
		break;
	    case 1:
		baudrate = B9600;
		baudtext = "9600";
		break;
	    case 2:
		baudrate = B19200;
		baudtext = "19200";
		break;
	    case 3:
		baudrate = B38400;
		baudtext = "38400";
		break;
#ifdef B57600
	    case 4:
		baudrate = B57600;
		baudtext = "57600";
		break;
#endif
#ifdef B115200
	    case 5:
		baudrate = B115200;
		baudtext = "115200";
		break;
#endif
	}

	fd = refclock_open(device, baudrate, LDISC_CLK);
	
	if (fd <= 0) {
#ifdef HAVE_READLINK
		/* nmead support added by Jon Miner (cp_n18@yahoo.com)
		 *
		 * See http://home.hiwaay.net/~taylorc/gps/nmea-server/
		 * for information about nmead
		 *
		 * To use this, you need to create a link from /dev/gpsX to
		 * the server:port where nmead is running.  Something like this:
		 *
		 * ln -s server:port /dev/gps1
		 */
		char buffer[80];
		char *nmea_host;
		int   nmea_port;
		int   len;
		struct hostent *he;
		struct protoent *p;
		struct sockaddr_in so_addr;

		if ((len = readlink(device,buffer,sizeof(buffer))) == -1)
			return(0);
		buffer[len] = 0;

		if ((nmea_host = strtok(buffer,":")) == NULL)
			return(0);

		nmea_port = atoi(strtok(NULL,":"));

		if ((he = gethostbyname(nmea_host)) == NULL)
			return(0);
		if ((p = getprotobyname("ip")) == NULL)
			return(0);
		so_addr.sin_family = AF_INET;
		so_addr.sin_port = htons(nmea_port);
		so_addr.sin_addr = *((struct in_addr *) he->h_addr);

		if ((fd = socket(PF_INET,SOCK_STREAM,p->p_proto)) == -1)
			return(0);
		if (connect(fd,(struct sockaddr *)&so_addr,SOCKLEN(&so_addr)) == -1) {
			close(fd);
			return (0);
		}
#else
		return (0);
#endif
	}

	msyslog(LOG_NOTICE, "refclock_nmea: serial %s open at %s bps",
		device, baudtext);

	/*
	 * Allocate and initialize unit structure
	 */
	up = (struct nmeaunit *)emalloc(sizeof(*up));
	if (!up) {
		close(fd);
		return (0);
	}
	memset((char *)up, 0, sizeof(struct nmeaunit));
	pp = peer->procptr;
	pp->io.clock_recv = nmea_receive;
	pp->io.srcclock = (caddr_t)peer;
	pp->io.datalen = 0;
	pp->io.fd = fd;
	if (!io_addclock(&pp->io)) {
		close(fd);
		free(up);
		return (0);
	}
	pp->unitptr = (caddr_t)up;

	/*
	 * Initialize miscellaneous variables
	 */
	peer->precision = PRECISION;
	pp->clockdesc = DESCRIPTION;
	memcpy((char *)&pp->refid, REFID, 4);
	up->pollcnt = 2;
	gps_send(pp->io.fd,"$PMOTG,RMC,0000*1D\r\n", peer);

#ifdef HAVE_PPSAPI
	/*
	 * Start the PPSAPI interface if it is there. Default to use
	 * the assert edge and do not enable the kernel hardpps.
	 */
	if (time_pps_create(fd, &up->handle) < 0) {
		up->handle = 0;
		msyslog(LOG_ERR,
		    "refclock_nmea: time_pps_create failed: %m");
		return (1);
	}
	return(nmea_ppsapi(peer, 0, 0, NULL));
#else
	return (1);
#endif /* HAVE_PPSAPI */
}

/*
 * nmea_shutdown - shut down a GPS clock
 */
static void
nmea_shutdown(
	int unit,
	struct peer *peer
	)
{
	register struct nmeaunit *up;
	struct refclockproc *pp;

	UNUSED_ARG(unit);

	pp = peer->procptr;
	up = (struct nmeaunit *)pp->unitptr;
#ifdef HAVE_PPSAPI
	if (NULL != up && up->handle)
		time_pps_destroy(up->handle);
#endif /* HAVE_PPSAPI */
	io_closeclock(&pp->io);
	if (NULL != up)
		free(up);
}

#ifdef HAVE_PPSAPI
/*
 * nmea_control - fudge control
 */
static void
nmea_control(
	int unit,		/* unit (not used */
	struct refclockstat *in, /* input parameters (not uded) */
	struct refclockstat *out, /* output parameters (not used) */
	struct peer *peer	/* peer structure pointer */
	)
{
	struct refclockproc *pp;

	pp = peer->procptr;
	nmea_ppsapi(peer, pp->sloppyclockflag & CLK_FLAG2,
	    pp->sloppyclockflag & CLK_FLAG3, &(pp->fudgetime1));
}


/*
 * Initialize PPSAPI
 */
int
nmea_ppsapi(
	struct peer *peer,	/* peer structure pointer */
	int enb_clear,		/* clear enable */
	int enb_hardpps,	/* hardpps enable */
	double *fudge		/* fudge value for time1 */
	)
{
	struct refclockproc *pp;
	struct nmeaunit *up;
	int capability;

	pp = peer->procptr;
	up = (struct nmeaunit *)pp->unitptr;
	if (time_pps_getcap(up->handle, &capability) < 0) {
		msyslog(LOG_ERR,
		    "refclock_nmea: time_pps_getcap failed: %m");
		return (0);
	}
	memset(&up->pps_params, 0, sizeof(pps_params_t));
	up->pps_params.api_version = PPS_API_VERS_1;
	if (enb_clear)
		up->pps_params.mode = capability & PPS_CAPTURECLEAR;
	else
		up->pps_params.mode = capability & PPS_CAPTUREASSERT;
	if (!up->pps_params.mode) {
		msyslog(LOG_ERR,
		    "refclock_nmea: invalid capture edge %d",
		    !enb_clear);
		return (0);
	}
	up->pps_params.mode |= PPS_TSFMT_TSPEC;
	
	/* Fudge time1 onto the pps in the kernel */
	if ((enb_hardpps) && (fudge != NULL )) {
		if ((!enb_clear) && (capability & PPS_OFFSETASSERT)) {
			/* Offset on assert */
			up->pps_params.mode |= PPS_OFFSETASSERT; 
			up->pps_params.assert_off_tu.tspec.tv_sec = -(*fudge);
			up->pps_params.assert_off_tu.tspec.tv_nsec = ((*fudge) - (long)(*fudge))*(-1e9);
		} else if ((enb_clear) && (capability & PPS_OFFSETCLEAR)) {
			/* Offset on clear */
			up->pps_params.mode |= PPS_OFFSETCLEAR; 
			up->pps_params.clear_off_tu.tspec.tv_sec = -(*fudge);
			up->pps_params.clear_off_tu.tspec.tv_nsec = ((*fudge) - (long)(*fudge))*(-1e9);
		}
	}
						
	if (time_pps_setparams(up->handle, &up->pps_params) < 0) {
		msyslog(LOG_ERR,
		    "refclock_nmea: time_pps_setparams failed: %m");
		return (0);
	}
	if (enb_hardpps) {
		if (time_pps_kcbind(up->handle, PPS_KC_HARDPPS,
				    up->pps_params.mode & 0x0F,
				    PPS_TSFMT_TSPEC) < 0) {
			msyslog(LOG_ERR,
			    "refclock_nmea: time_pps_kcbind failed: %m");
			return (0);
		}
		pps_enable = 1;
	}
	peer->precision = PPS_PRECISION;

#if DEBUG
	if (debug) {
		time_pps_getparams(up->handle, &up->pps_params);
		printf(
		    "refclock_ppsapi: capability 0x%x version %d mode 0x%x kern %d\n",
		    capability, up->pps_params.api_version,
		    up->pps_params.mode, enb_hardpps);
	}
#endif

	return (1);
}

/*
 * Get PPSAPI timestamps.
 *
 * Return 0 on failure and 1 on success.
 */
static int
nmea_pps(
	struct nmeaunit *up,
	l_fp *tsptr
	)
{
	pps_info_t pps_info;
	struct timespec timeout, ts;
	double dtemp;
	l_fp tstmp;

	/*
	 * Convert the timespec nanoseconds field to ntp l_fp units.
	 */ 
	if (up->handle == 0)
		return (0);
	timeout.tv_sec = 0;
	timeout.tv_nsec = 0;
	memcpy(&pps_info, &up->pps_info, sizeof(pps_info_t));
	if (time_pps_fetch(up->handle, PPS_TSFMT_TSPEC, &up->pps_info,
	    &timeout) < 0)
		return (0);
	if (up->pps_params.mode & PPS_CAPTUREASSERT) {
		if (pps_info.assert_sequence ==
		    up->pps_info.assert_sequence)
			return (0);
		ts = up->pps_info.assert_timestamp;
	} else if (up->pps_params.mode & PPS_CAPTURECLEAR) {
		if (pps_info.clear_sequence ==
		    up->pps_info.clear_sequence)
			return (0);
		ts = up->pps_info.clear_timestamp;
	} else {
		return (0);
	}
	if ((up->ts.tv_sec == ts.tv_sec) && (up->ts.tv_nsec == ts.tv_nsec))
		return (0);
	up->ts = ts;

	tstmp.l_ui = ts.tv_sec + JAN_1970;
	dtemp = ts.tv_nsec * FRAC / 1e9;
	tstmp.l_uf = (u_int32)dtemp;
	*tsptr = tstmp;
	return (1);
}
#endif /* HAVE_PPSAPI */

/*
 * nmea_receive - receive data from the serial interface
 */
static void
nmea_receive(
	struct recvbuf *rbufp
	)
{
	register struct nmeaunit *up;
	struct refclockproc *pp;
	struct peer *peer;
	int month, day;
	char *cp, *dp, *msg;
	int cmdtype;
	int cmdtypezdg = 0;
	/* Use these variables to hold data until we decide its worth keeping */
	char	rd_lastcode[BMAX];
	l_fp	rd_timestamp;
	int	rd_lencode;

	/*
	 * Initialize pointers and read the timecode and timestamp
	 */
	peer = (struct peer *)rbufp->recv_srcclock;
	pp = peer->procptr;
	up = (struct nmeaunit *)pp->unitptr;

	rd_lencode = refclock_gtlin(
			rbufp, 
			rd_lastcode, 
			sizeof(rd_lastcode), 
			&rd_timestamp);

	/*
	 * There is a case that a <CR><LF> gives back a "blank" line
	 */
	if (rd_lencode == 0)
		return;

	DPRINTF(1, ("nmea: gpsread %d %s\n", rd_lencode, rd_lastcode));

	/*
	 * We check the timecode format and decode its contents. The
	 * we only care about a few of them.  The most important being
	 * the $GPRMC format
	 * $GPRMC,hhmmss,a,fddmm.xx,n,dddmmm.xx,w,zz.z,yyy.,ddmmyy,dd,v*CC
	 * mode (0,1,2,3) selects sentence ANY/ALL, RMC, GGA, GLL, ZDA
	 * $GPGLL,3513.8385,S,14900.7851,E,232420.594,A*21
	 * $GPGGA,232420.59,3513.8385,S,14900.7851,E,1,05,3.4,00519,M,,,,*3F
	 * $GPRMC,232418.19,A,3513.8386,S,14900.7853,E,00.0,000.0,121199,12.,E*77
	 *
	 * Defining GPZDA to support Standard Time & Date
	 * sentence. The sentence has the following format 
	 *  
	 *  $--ZDA,HHMMSS.SS,DD,MM,YYYY,TH,TM,*CS<CR><LF>
	 *
	 *  Apart from the familiar fields, 
	 *  'TH'    Time zone Hours
	 *  'TM'    Time zone Minutes
	 *
	 * Defining GPZDG to support Accord GPS Clock's custom NMEA 
	 * sentence. The sentence has the following format 
	 *  
	 *  $GPZDG,HHMMSS.S,DD,MM,YYYY,AA.BB,V*CS<CR><LF>
	 *
	 *  It contains the GPS timestamp valid for next PPS pulse.
	 *  Apart from the familiar fields, 
	 *  'AA.BB' denotes the signal strength( should be < 05.00 ) 
	 *  'V'     denotes the GPS sync status : 
	 *	   '0' indicates INVALID time, 
	 *	   '1' indicates accuracy of +/-20 ms
	 *	   '2' indicates accuracy of +/-100 ns
	 */
#define GPXXX		0	/* any/all */
#define GPRMC		1
#define GPGGA		2
#define GPGLL		4
#define GPZDG_ZDA	8

	cp = rd_lastcode;
	cmdtype=0;
	if (cp[0] == '$') {
		/* Allow for GLGGA and GPGGA etc. */
		msg = cp + 3;

		if (strncmp(msg, "RMC", 3) == 0)
			cmdtype = GPRMC;
		else if (strncmp(msg, "GGA", 3) == 0)
			cmdtype = GPGGA;
		else if (strncmp(msg, "GLL", 3) == 0)
			cmdtype = GPGLL;
		else if (strncmp(msg, "ZD", 2) == 0) {
			cmdtype = GPZDG_ZDA;
			if ('G' == msg[2])
				cmdtypezdg = 1;
			else if ('A' != msg[2])
				return;
		} else
			return;
	} else
		return;

	/* See if I want to process this message type */
	if (peer->ttl && !(cmdtype & (peer->ttl & NMEA_MESSAGE_MASK)))
		return;

	/* 
	 * $GPZDG provides GPS time not UTC, and the two mix poorly.
	 * Once have processed a $GPZDG, do not process any further
	 * UTC sentences (all but $GPZDG currently).
	 */
	if (up->gps_time && !cmdtypezdg)
		return;

	/* make sure it came in clean */
	if (!nmea_checksum_ok(rd_lastcode)) {
		refclock_report(peer, CEVNT_BADREPLY);
		return;
	}

	pp->lencode = (u_short) rd_lencode;
	memcpy(pp->a_lastcode, rd_lastcode, pp->lencode + 1);
	cp = pp->a_lastcode;

	pp->lastrec = up->tstamp = rd_timestamp;
	up->pollcnt = 2;

	DPRINTF(1, ("nmea: timecode %d %s\n", pp->lencode, pp->a_lastcode));

	/* Grab field depending on clock string type */
	switch (cmdtype) {

	case GPRMC:
		/*
		 * Test for synchronization.  Check for quality byte.
		 */
		dp = field_parse(cp, 2);
		if (dp[0] != 'A')
			pp->leap = LEAP_NOTINSYNC;
		else
			pp->leap = LEAP_NOWARNING;

		/* Now point at the time field */
		dp = field_parse(cp, 1);
		break;

	case GPGGA:
		/*
		 * Test for synchronization.  Check for quality byte.
		 */
		dp = field_parse(cp, 6);
		if (dp[0] == '0')
			pp->leap = LEAP_NOTINSYNC;
		else
			pp->leap = LEAP_NOWARNING;

		/* Now point at the time field */
		dp = field_parse(cp, 1);
		break;

	case GPGLL:
		/*
		 * Test for synchronization.  Check for quality byte.
		 */
		dp = field_parse(cp, 6);
		if (dp[0] != 'A')
			pp->leap = LEAP_NOTINSYNC;
		else
			pp->leap = LEAP_NOWARNING;

		/* Now point at the time field */
		dp = field_parse(cp, 5);
		break;
	
	case GPZDG_ZDA:
		/*
		 * Test for synchronization.  For $GPZDG check for validity of GPS time.
		 */
		if (cmdtypezdg) {
			dp = field_parse(cp, 6);
			if (dp[0] == '0') 
				pp->leap = LEAP_NOTINSYNC;
			else 
				pp->leap = LEAP_NOWARNING;
		} else
			pp->leap = LEAP_NOWARNING;

		/* Now point at the time field */
		dp = field_parse(cp, 1);
		break;

	default:
		return;
	}

	/*
	 * Check time code format of NMEA
	 */
	if (!isdigit((int)dp[0]) ||
	    !isdigit((int)dp[1]) ||
	    !isdigit((int)dp[2]) ||
	    !isdigit((int)dp[3]) ||
	    !isdigit((int)dp[4]) ||
	    !isdigit((int)dp[5])) {

		DPRINTF(1, ("NMEA time code %c%c%c%c%c%c non-numeric",
			    dp[0], dp[1], dp[2], dp[3], dp[4], dp[5]));
		refclock_report(peer, CEVNT_BADTIME);
		return;
	}

	/*
	 * Convert time and check values.
	 */
	pp->hour = ((dp[0] - '0') * 10) + dp[1] - '0';
	pp->minute = ((dp[2] - '0') * 10) + dp[3] -  '0';
	pp->second = ((dp[4] - '0') * 10) + dp[5] - '0';
	/* 
	 * Default to 0 milliseconds, if decimal convert milliseconds in
	 * one, two or three digits
	 */
	pp->nsec = 0; 
	if (dp[6] == '.') {
		if (isdigit((int)dp[7])) {
			pp->nsec = (dp[7] - '0') * 100000000;
			if (isdigit((int)dp[8])) {
				pp->nsec += (dp[8] - '0') * 10000000;
				if (isdigit((int)dp[9])) {
					pp->nsec += (dp[9] - '0') * 1000000;
				}
			}
		}
	}

	/*
	 * Manipulating GPS timestamp in GPZDG as the seconds field
	 * is valid for next PPS tick. Just rolling back the second,
	 * minute and hour fields appopriately
	 */
	if (cmdtypezdg) {
		if (pp->second == 0) {
			pp->second = 59;
			if (pp->minute == 0) {
				pp->minute = 59;
				if (pp->hour == 0)
					pp->hour = 23;
			}
		} else
			pp->second -= 1;
	}

	if (pp->hour > 23 || pp->minute > 59 || 
	    pp->second > 59 || pp->nsec > 1000000000) {

		DPRINTF(1, ("NMEA hour/min/sec/nsec range %02d:%02d:%02d.%09ld\n",
			    pp->hour, pp->minute, pp->second, pp->nsec));
		refclock_report(peer, CEVNT_BADTIME);
		return;
	}

	/*
	 * Convert date and check values.
	 */
	if (GPRMC == cmdtype) {

		dp = field_parse(cp,9);
		day = dp[0] - '0';
		day = (day * 10) + dp[1] - '0';
		month = dp[2] - '0';
		month = (month * 10) + dp[3] - '0';
		pp->year = dp[4] - '0';
		pp->year = (pp->year * 10) + dp[5] - '0';

	} else if (GPZDG_ZDA == cmdtype) {

		dp = field_parse(cp, 2);
		day = 10 * (dp[0] - '0') + (dp[1] - '0');
		dp = field_parse(cp, 3);
		month = 10 * (dp[0] - '0') + (dp[1] - '0');
		dp = field_parse(cp, 4);
		pp->year = /* 1000 * (dp[0] - '0') + 100 * (dp[1] - '0') + */ 10 * (dp[2] - '0') + (dp[3] - '0');

	} else {
		/* only time */
		time_t tt = time(NULL);
		struct tm * t = gmtime(&tt);
		day = t->tm_mday;
		month = t->tm_mon + 1;
		pp->year= t->tm_year + 1900;
	}

	if (month < 1 || month > 12 || day < 1) {
		refclock_report(peer, CEVNT_BADDATE);
		return;
	}

	/* pp->year will be 2 or 4 digits if read from GPS, 4 from gmtime */
	if (pp->year < 100) {
		if (pp->year < 9)	/* year of our line of code is 2009 */
			pp->year += 2100;
		else
			pp->year += 2000;
	}

	/* pp->year now 4 digits as ymd2yd requires */
	day = ymd2yd(pp->year, month, day);
	if (-1 == day) {
		refclock_report(peer, CEVNT_BADDATE);
		return;
	}
	pp->day = day;

	/*
	 * If "fudge 127.127.20.__ flag4 1" is configured in ntp.conf,
	 * remove the location and checksum from the NMEA sentence
	 * recorded as the last timecode and visible to remote users
	 * with:
	 *
	 * ntpq -c clockvar <server>
	 *
	 * Note that this also removes the location from the clockstats
	 * log (if it is enabled).  Some NTP operators monitor their
	 * NMEA GPS using the change in location in clockstats over
	 * time as as a proxy for the quality of GPS reception and
	 * thereby time reported.
	 */
	if (CLK_FLAG4 & pp->sloppyclockflag) {
		/*
		 * Start by pointing cp and dp at the fields with 
		 * longitude and latitude in the last timecode.
		 */
		switch (cmdtype) {

		case GPGLL:
			cp = field_parse(pp->a_lastcode, 1);
			dp = field_parse(cp, 2);
			break;

		case GPGGA:
			cp = field_parse(pp->a_lastcode, 2);
			dp = field_parse(cp, 2);
			break;

		case GPRMC:
			cp = field_parse(pp->a_lastcode, 3);
			dp = field_parse(cp, 2);
			break;

		case GPZDG_ZDA:
		default:
			cp = dp = NULL;
		}

		/*
		 * Blanking everything after the decimal point '.' is easy and 
		 * gives enough error for at least a few neighbors to be as 
		 * likely as you to be the one with the reflock.  We're keeping
		 * degrees and minutes but tossing the seconds (expressed as
		 * decimal fractions of a minute).  Degrees minutes seconds,
		 * not hours minutes seconds.  :)
		 */
		while (cp) {
			while (',' != *cp) {
				if ('.' != *cp)
					*cp = '_';
				cp++;
			}

			/*
			 * blank the longitude at cp then the latitude at dp
			 * then we're done.
			 */
			if (cp < dp)
				cp = dp;
			else
				cp = NULL;
		}

		/*
		 * blank the checksum, last two characters on the line
		 */
		if (dp) {
			cp = pp->a_lastcode + pp->lencode - 2;
			if (0 == cp[2])
				cp[0] = cp[1] = '_';
		}

	}

#ifdef HAVE_PPSAPI
	/*
	 * If the PPSAPI is working, rather use its timestamps.
	 * assume that the PPS occurs on the second so blow any msec
	 */
	if (nmea_pps(up, &rd_timestamp) == 1) {
		pp->lastrec = up->tstamp = rd_timestamp;
		pp->nsec = 0;
	}
#endif /* HAVE_PPSAPI */

	/*
	 * Process the new sample in the median filter and determine the
	 * reference clock offset and dispersion. We use lastrec as both
	 * the reference time and receive time, in order to avoid being
	 * cute, like setting the reference time later than the receive
	 * time, which may cause a paranoid protocol module to chuck out
	 * the data.
	 */

	if (!refclock_process(pp)) {
		refclock_report(peer, CEVNT_BADTIME);
		return;
	}

	/*
	 * Note that we're only using GPS timescale from now on.
	 */
	if (cmdtypezdg && !up->gps_time) {
		up->gps_time = 1;
		NLOG(NLOG_CLOCKINFO)
			msyslog(LOG_INFO, "%s using only $GPZDG",
				refnumtoa(&peer->srcadr));
	}

	/*
	 * Only go on if we had been polled.
	 */
	if (!up->polled)
		return;
	up->polled = 0;
	pp->lastref = pp->lastrec;
	refclock_receive(peer);

	/* If we get here - what we got from the clock is OK, so say so */
	refclock_report(peer, CEVNT_NOMINAL);

	record_clock_stats(&peer->srcadr, pp->a_lastcode);
}


/*
 * nmea_poll - called by the transmit procedure
 *
 * We go to great pains to avoid changing state here, since there may be
 * more than one eavesdropper receiving the same timecode.
 */
static void
nmea_poll(
	int unit,
	struct peer *peer
	)
{
	register struct nmeaunit *up;
	struct refclockproc *pp;

	pp = peer->procptr;
	up = (struct nmeaunit *)pp->unitptr;
	if (up->pollcnt == 0)
	    refclock_report(peer, CEVNT_TIMEOUT);
	else
	    up->pollcnt--;
	pp->polls++;
	up->polled = 1;

	/*
	 * usually nmea_receive can get a timestamp every second, 
	 * but at least one Motorola unit needs prompting each
	 * time.
	 */

	gps_send(pp->io.fd,"$PMOTG,RMC,0000*1D\r\n", peer);
}


/*
 *
 *	gps_send(fd,cmd, peer)  Sends a command to the GPS receiver.
 *	 as	gps_send(fd,"rqts,u\r", peer);
 *
 *	We don't currently send any data, but would like to send
 *	RTCM SC104 messages for differential positioning. It should
 *	also give us better time. Without a PPS output, we're
 *	Just fooling ourselves because of the serial code paths
 *
 */
static void
gps_send(
	int fd,
	const char *cmd,
	struct peer *peer
	)
{
	if (write(fd, cmd, strlen(cmd)) == -1) {
		refclock_report(peer, CEVNT_FAULT);
	}
}


static char *
field_parse(
	char *cp,
	int fn
	)
{
	char *tp;
	int i = fn;

	for (tp = cp; i && *tp; tp++)
		if (*tp == ',')
			i--;

	return tp;
}


/*
 * nmea_checksum_ok verifies 8-bit XOR checksum is correct then returns 1
 *
 * format is $XXXXX,1,2,3,4*ML
 *
 * 8-bit XOR of characters between $ and * noninclusive is transmitted
 * in last two chars M and L holding most and least significant nibbles
 * in hex representation such as:
 *
 *   $GPGLL,5057.970,N,00146.110,E,142451,A*27
 *   $GPVTG,089.0,T,,,15.2,N,,*7F
 */
int
nmea_checksum_ok(
	const char *sentence
	)
{
	u_char my_cs;
	u_long input_cs;
	const char *p;

	my_cs = 0;
	p = sentence;

	if ('$' != *p++)
		return 0;

	for ( ; *p && '*' != *p; p++) {

		my_cs ^= *p;
	}

	if ('*' != *p++)
		return 0;

	if (0 == p[0] || 0 == p[1] || 0 != p[2])
		return 0;

	if (0 == hextoint(p, &input_cs))
		return 0;

	if (my_cs != input_cs)
		return 0;

	return 1;
}
#else
int refclock_nmea_bs;
#endif /* REFCLOCK */
