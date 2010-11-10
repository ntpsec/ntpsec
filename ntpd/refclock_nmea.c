/*
 * refclock_nmea.c - clock driver for an NMEA GPS CLOCK
 *		Michael Petry Jun 20, 1994
 *		 based on refclock_heathn.c
 *
 * Updated to add support for Accord GPS Clock
 *		Venu Gopal Dec 05, 2007
 *		neo.venu@gmail.com, venugopal_d@pgad.gov.in
 *
 * Updated to process 'time1' fudge factor
 *		Venu Gopal May 05, 2008
 *
 * Converted to common PPSAPI code, separate PPS fudge time1
 * from serial timecode fudge time2.
 *		Dave Hart July 1, 2009
 *		hart@ntp.org, davehart@davehart.com
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(REFCLOCK) && defined(CLOCK_NMEA)

#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_unixtime.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"
#include "ntp_calendar.h"
#include "lib_strbuf.h"

#ifdef HAVE_PPSAPI
# include "ppsapi_timepps.h"
# include "refclock_atom.h"
#endif /* HAVE_PPSAPI */

#ifdef SYS_WINNT
#undef write	/* ports/winnt/include/config.h: #define write _write */
extern int async_write(int, const void *, unsigned int);
#define write(fd, data, octets)	async_write(fd, data, octets)
#endif

#ifndef TIMESPECTOTS
#define TIMESPECTOTS(ptspec, pts)					\
	do {								\
		DTOLFP((ptspec)->tv_nsec * 1.0e-9, pts);		\
		(pts)->l_ui += (u_int32)((ptspec)->tv_sec) + JAN_1970;	\
	} while (0)
#endif


/*
 * This driver supports NMEA-compatible GPS receivers
 *
 * Prototype was refclock_trak.c, Thanks a lot.
 *
 * The receiver used spits out the NMEA sentences for boat navigation.
 * And you thought it was an information superhighway.	Try a raging river
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
#define NMEA_MESSAGE_MASK	0x0F
#define NMEA_BAUDRATE_MASK	0x70
#define NMEA_BAUDRATE_SHIFT	4
#define NMEA_DELAYMEAS_MASK	0x80

#define NMEA_PROTO_IDLEN	5	/* tag name must be at least 5 chars */
#define NMEA_PROTO_MINLEN	6	/* min chars in sentence, excluding CS */
#define NMEA_PROTO_MAXLEN	80	/* max chars in sentence, excluding CS */
#define NMEA_PROTO_FIELDS	32	/* not official; limit on fields per record */

/*
 * We check the timecode format and decode its contents.  We only care
 * about a few of them, the most important being the $GPRMC format:
 *
 * $GPRMC,hhmmss,a,fddmm.xx,n,dddmmm.xx,w,zz.z,yyy.,ddmmyy,dd,v*CC
 *
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
 *  'V'	    denotes the GPS sync status : 
 *	   '0' indicates INVALID time, 
 *	   '1' indicates accuracy of +/-20 ms
 *	   '2' indicates accuracy of +/-100 ns
 */

/*
 * Definitions
 */
#define	DEVICE		"/dev/gps%d"	/* GPS serial device */
#define	PPSDEV		"/dev/gpspps%d"	/* PPSAPI device override */
#define	SPEED232	B4800	/* uart speed (4800 bps) */
#define	PRECISION	(-9)	/* precision assumed (about 2 ms) */
#define	PPS_PRECISION	(-20)	/* precision assumed (about 1 us) */
#define	REFID		"GPS\0"	/* reference id */
#define	DESCRIPTION	"NMEA GPS Clock" /* who we are */
#ifndef O_NOCTTY
#define M_NOCTTY	0
#else
#define M_NOCTTY	O_NOCTTY
#endif
#ifndef O_NONBLOCK
#define M_NONBLOCK	0
#else
#define M_NONBLOCK	O_NONBLOCK
#endif
#define PPSOPENMODE	(O_RDWR | M_NOCTTY | M_NONBLOCK)

/* NMEA sentence array indexes for those we use */
#define NMEA_GPRMC	0	/* recommended min. nav. */
#define NMEA_GPGGA	1	/* fix and quality */
#define NMEA_GPGLL	2	/* geo. lat/long */
#define NMEA_GPZDA	3	/* date/time */
/*
 * $GPZDG is a proprietary sentence that violates the spec, by not
 * using $P and an assigned company identifier to prefix the sentence
 * identifier.	When used with this driver, the system needs to be
 * isolated from other NTP networks, as it operates in GPS time, not
 * UTC as is much more common.	GPS time is >15 seconds different from
 * UTC due to not respecting leap seconds since 1970 or so.  Other
 * than the different timebase, $GPZDG is similar to $GPZDA.
 */
#define NMEA_GPZDG	4
#define NMEA_ARRAY_SIZE (NMEA_GPZDG + 1)

/*
 * Sentence selection mode bits
 */
#define USE_ALL			0	/* any/all */
#define USE_GPRMC		1
#define USE_GPGGA		2
#define USE_GPGLL		4
#define USE_GPZDA		8

/* mapping from sentence index to controlling mode bit */
static const u_char sentence_mode[NMEA_ARRAY_SIZE] =
{
	USE_GPRMC,
	USE_GPGGA,
	USE_GPGLL,
	USE_GPZDA,
	USE_GPZDA
};

/* date formats we support */
enum date_fmt {
	DATE_1_DDMMYY,	/* use 1 field	with 2-digit year */
	DATE_3_DDMMYYYY	/* use 3 fields with 4-digit year */
};

/* results for 'field_init()'
 *
 * Note: If a checksum is present, the checksum test must pass OK or the
 * sentence is tagged invalid.
 */
#define CHECK_EMPTY  -1	/* no data			*/
#define CHECK_INVALID 0	/* not a valid NMEA sentence	*/
#define CHECK_VALID   1	/* valid but without checksum	*/
#define CHECK_CSVALID 2	/* valid with checksum OK	*/

/*
 * Unit control structure
 */
struct nmeaunit {
#ifdef HAVE_PPSAPI
	struct refclock_atom atom; /* PPSAPI structure */
	int	ppsapi_tried;	/* attempt PPSAPI once */
	int	ppsapi_lit;	/* time_pps_create() worked */
	int	ppsapi_fd;	/* fd used with PPSAPI */
	int	ppsapi_gate;	/* allow edge detection processing */
	int	tcount;		/* timecode sample counter */
	int	pcount;		/* PPS sample counter */
#endif /* HAVE_PPSAPI */
	int	gps_time;	/* 0 UTC, 1 GPS time */
	int32	last_daytime;	/* last time-of-day stamp */
	/* per sentence checksum seen flag */
	u_char	cksum_type[NMEA_ARRAY_SIZE];
};

/*
 * helper for faster field access
 */
struct nmeadata {
	char  *base;	/* buffer base		*/
	char  *cptr;	/* current field ptr	*/
	int    blen;	/* buffer length	*/
	int    cidx;	/* current field index	*/
};

/*
 * Function prototypes
 */
static	int	nmea_start	(int, struct peer *);
static	void	nmea_shutdown	(int, struct peer *);
static	void	nmea_receive	(struct recvbuf *);
static	void	nmea_poll	(int, struct peer *);
#ifdef HAVE_PPSAPI
static	void	nmea_control	(int, const struct refclockstat *,
				 struct refclockstat *, struct peer *);
static	void	nmea_timer	(int, struct peer *);
#define		NMEA_CONTROL	nmea_control
#define		NMEA_TIMER	nmea_timer
#else
#define		NMEA_CONTROL	noentry
#define		NMEA_TIMER	noentry
#endif /* HAVE_PPSAPI */
static	void	gps_send	(int, const char *, struct peer *);

/* parsing helpers */
static int   field_init	(struct nmeadata *data, char *cp, int len);
static char* field_parse(struct nmeadata *data, int fn);
static void  field_wipe	(struct nmeadata *data, ...);
static int   parse_qual (const char *cp, char tag, int inv);
static int   parse_time (const char *cp, struct calendar *jd, long *nsec);
static int   parse_date (const char *cp, struct calendar *jd, enum date_fmt fmt);
/* calendar / date helpers */
static int   unfold_day	(struct calendar*, u_int32 rec_ui);

/*
 * -------------------------------------------------------------------
 * Transfer vector
 * -------------------------------------------------------------------
 */
struct	refclock refclock_nmea = {
	nmea_start,		/* start up driver */
	nmea_shutdown,		/* shut down driver */
	nmea_poll,		/* transmit poll message */
	NMEA_CONTROL,		/* fudge control */
	noentry,		/* initialize driver */
	noentry,		/* buginfo */
	NMEA_TIMER		/* called once per second */
};

/*
 * -------------------------------------------------------------------
 * nmea_start - open the GPS devices and initialize data for processing
 * -------------------------------------------------------------------
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

	pp = peer->procptr;

	/* Open serial port. Use CLK line discipline, if available. Use
	 * baudrate based on the value of bit 4/5/6
	 */
	snprintf(device, sizeof(device), DEVICE, unit);
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
		char *nmea_host, *nmea_tail;
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
		if ((nmea_tail = strtok(NULL,":")) == NULL)
			return(0);

		nmea_port = strtoul(nmea_tail, NULL, 0);

		if ((he = gethostbyname(nmea_host)) == NULL)
			return(0);
		if ((p = getprotobyname("tcp")) == NULL)
			return(0);
		memset(&so_addr, 0, sizeof(so_addr));
		so_addr.sin_family = AF_INET;
		so_addr.sin_port = htons(nmea_port);
		so_addr.sin_addr = *((struct in_addr *) he->h_addr);

		if ((fd = socket(PF_INET,SOCK_STREAM,p->p_proto)) == -1)
			return(0);
		if (connect(fd,(struct sockaddr *)&so_addr, sizeof(so_addr)) == -1) {
			close(fd);
			return (0);
		}
#else
		pp->io.fd = -1;
		return (0);
#endif
	}

	msyslog(LOG_NOTICE, "%s serial %s open at %s bps",
		refnumtoa(&peer->srcadr), device, baudtext);

	/* Allocate and initialize unit structure */
	up = emalloc(sizeof(*up));
	memset(up, 0, sizeof(*up));
	pp->io.clock_recv = nmea_receive;
	pp->io.srcclock = (caddr_t)peer;
	pp->io.datalen = 0;
	pp->io.fd = fd;
	if (!io_addclock(&pp->io)) {
		pp->io.fd = -1;
		close(fd);
		free(up);
		return (0);
	}
	pp->unitptr = (caddr_t)up;
	up->last_daytime = -1; /* force change detection on first valid message */
	up->cksum_type[NMEA_GPRMC] = CHECK_CSVALID; /* force checksum on GPRMC, see below */

	/* Initialize miscellaneous variables */
	peer->precision = PRECISION;
	pp->clockdesc = DESCRIPTION;
	memcpy(&pp->refid, REFID, 4);

	/* Seems at least one MOTOROLA unit needs to switch on
	 * periodic transmission of $PGRMC. Though this is a
	 * misconfigured device, IMHO...
	 */
	gps_send(fd, "PMOTG,RMC,0001", peer);
	
	return (1);
}


/*
 * -------------------------------------------------------------------
 * nmea_shutdown - shut down a GPS clock
 * 
 * NOTE this routine is called after nmea_start() returns failure,
 * as well as during a normal shutdown due to ntpq :config unpeer.
 * -------------------------------------------------------------------
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
	if (up != NULL) {
#ifdef HAVE_PPSAPI
		if (up->ppsapi_lit) {
			time_pps_destroy(up->atom.handle);
			if (up->ppsapi_fd != pp->io.fd)
				close(up->ppsapi_fd);
		}
#endif
		free(up);
	}
	if (-1 != pp->io.fd)
		io_closeclock(&pp->io);
}

/*
 * -------------------------------------------------------------------
 * nmea_control - configure fudge params
 * -------------------------------------------------------------------
 */
#ifdef HAVE_PPSAPI
static void
nmea_control(
	int unit,
	const struct refclockstat *in_st,
	struct refclockstat *out_st,
	struct peer *peer
	)
{
	char device[32];
	register struct nmeaunit *up;
	struct refclockproc *pp;
	int pps_fd;
	
	UNUSED_ARG(in_st);
	UNUSED_ARG(out_st);

	pp = peer->procptr;
	up = (struct nmeaunit *)pp->unitptr;

	if (!(CLK_FLAG1 & pp->sloppyclockflag)) {
		if (!up->ppsapi_tried)
			return;
		up->ppsapi_tried = 0;
		if (!up->ppsapi_lit)
			return;
		peer->flags &= ~FLAG_PPS;
		peer->precision = PRECISION;
		time_pps_destroy(up->atom.handle);
		if (up->ppsapi_fd != pp->io.fd)
			close(up->ppsapi_fd);
		up->atom.handle = 0;
		up->ppsapi_lit = 0;
		up->ppsapi_fd = -1;
		return;
	}

	/* Light up the PPSAPI interface if not yet attempted. */
	if (up->ppsapi_tried)
		return;
	up->ppsapi_tried = 1;

	/* if /dev/gpspps$UNIT can be opened that will be used for
	 * PPSAPI.  Otherwise, the GPS serial device /dev/gps$UNIT
	 * already opened is used for PPSAPI as well.
	 */
	snprintf(device, sizeof(device), PPSDEV, unit);

	pps_fd = open(device, PPSOPENMODE, S_IRUSR | S_IWUSR);

	if (-1 == pps_fd)
		pps_fd = pp->io.fd;
	
	if (refclock_ppsapi(pps_fd, &up->atom)) {
		up->ppsapi_lit = 1;
		up->ppsapi_fd = pps_fd;
		/* prepare to use the PPS API for our own purposes now. */
		refclock_params(pp->sloppyclockflag, &up->atom);
		return;
	}

	NLOG(NLOG_CLOCKINFO)
		msyslog(LOG_WARNING, "%s flag1 1 but PPSAPI fails",
			refnumtoa(&peer->srcadr));
}
#endif	/* HAVE_PPSAPI */


#ifdef HAVE_PPSAPI
/*
 * -------------------------------------------------------------------
 * nmea_timer - called once per second, fetches PPS
 *		timestamp and stuffs in median filter.
 * -------------------------------------------------------------------
 */
static void
nmea_timer(
	int		unit,
	struct peer *	peer
	)
{
	struct nmeaunit *up;
	struct refclockproc *pp;

	UNUSED_ARG(unit);

	pp = peer->procptr;
	up = (struct nmeaunit *)pp->unitptr;

	if (up->ppsapi_lit && up->ppsapi_gate &&
	    refclock_pps(peer, &up->atom, pp->sloppyclockflag) > 0) {
		up->pcount++;
		peer->flags |= FLAG_PPS;
		peer->precision = PPS_PRECISION;
	}
}
#endif	/* HAVE_PPSAPI */

#ifdef HAVE_PPSAPI
/*
 * -------------------------------------------------------------------
 * refclock_ppsrelate(...) -- correlate with PPS edge
 *
 * This function is used to correlate a receive time stamp and a
 * reference time with a PPS edge time stamp. It applies the necessary
 * fudges (fudge1 for PPS, fudge2 for receive time) and then tries to
 * move the receive time stamp to the corresponding edge. This can warp
 * into future, if a transmission delay of more than 500ms is not
 * compensated with a corresponding fudge time2 value, because then the
 * next PPS edge is nearer than the last. (Similiar to what the PPS ATOM
 * driver does, but we deal with full time stamps here, not just phase
 * shift information.) Likewise, a negative fudge time2 value must be
 * used if the reference time stamp correlates with the *following* PPS
 * pulse.
 *
 * Note that the receive time fudge value only needs to move the receive
 * stamp near a PPS edge but that close proximity is not required;
 * +/-100ms precision should be enough. But since the fudge value will
 * probably also be used to compensate the transmission delay when no
 * PPS edge can be related to the time stamp, it's best to get it as
 * close as possible.
 *
 * It should also be noted that the typical use case is matching to the
 * preceeding edge, as most units relate their sentences to the current
 * second.
 *
 * The function returns PPS_RELATE_NONE (0) if no PPS edge correlation
 * can be fixed; PPS_RELATE_EDGE (1) when a PPS edge could be fixed, but
 * the distance to the reference time stamp is too big (exceeds
 * +/-400ms) and the ATOM driver PLL cannot be used to fix the phase;
 * and PPS_RELATE_PHASE (2) when the ATOM driver PLL code can be used.
 *
 * On output, the receive time stamp is replaced with the corresponding
 * PPS edge time if a fix could be made; the PPS fudge is updated to
 * reflect the proper fudge time to apply. (This implies that
 * 'refclock_process_offset()' must be used!)
 * -------------------------------------------------------------------
 */
#define PPS_RELATE_NONE	 0	/* no pps correlation possible	  */
#define PPS_RELATE_EDGE	 1	/* recv time fixed, no phase lock */
#define PPS_RELATE_PHASE 2	/* recv time fixed, phase lock ok */

static int
refclock_ppsrelate(
	const struct refclockproc  *pp	    ,	/* for sanity	  */
	const struct refclock_atom *ap	    ,	/* for PPS io	  */
	const l_fp		   *reftime ,
	l_fp			   *rd_stamp,	/* i/o read stamp */
	double			    pp_fudge,	/* pps fudge	  */
	double			   *rd_fudge)	/* i/o read fudge */
{
	pps_info_t	pps_info;
	struct timespec timeout;
	l_fp		pp_stamp, pp_delta;
	double		delta, idelta;

	if (pp->leap == LEAP_NOTINSYNC)
		return PPS_RELATE_NONE;	/* clock is insane, no chance */
	
	memset(&timeout, 0, sizeof(timeout));
	memset(&pps_info, 0, sizeof(pps_info_t));

	if (time_pps_fetch(ap->handle, PPS_TSFMT_TSPEC,
			   &pps_info, &timeout) < 0)
		return PPS_RELATE_NONE;

	/* get last active PPS edge before receive */
	if (ap->pps_params.mode & PPS_CAPTUREASSERT)
		timeout = pps_info.assert_timestamp;
	else if (ap->pps_params.mode & PPS_CAPTURECLEAR)
		timeout = pps_info.clear_timestamp;
	else
		return PPS_RELATE_NONE;

	/* get delta between receive time and PPS time */
	TIMESPECTOTS(&timeout, &pp_stamp);
	pp_delta = *rd_stamp;
	L_SUB(&pp_delta, &pp_stamp);
	LFPTOD(&pp_delta, delta);
	delta += pp_fudge - *rd_fudge;
	if (fabs(delta) > 1.5)
		return PPS_RELATE_NONE; /* PPS timeout control */
	
	/* eventually warp edges, check phase */
	idelta	  = floor(delta + 0.5);
	pp_fudge -= idelta;
	delta	 -= idelta;
	if (fabs(delta) > 0.45)
		return PPS_RELATE_NONE; /* dead band control */

	/* we actually have a PPS edge to relate with! */
	*rd_stamp = pp_stamp;
	*rd_fudge = pp_fudge;

	/* if whole system out-of-sync, do not try to PLL */
	if (sys_leap == LEAP_NOTINSYNC)
		return PPS_RELATE_EDGE;	/* cannot PLL with atom code */

	/* check against reftime if ATOM PLL can be used */
	pp_delta = *reftime;
	L_SUB(&pp_delta, &pp_stamp);
	LFPTOD(&pp_delta, delta);
	delta += pp_fudge;
	if (fabs(delta) > 0.45)
		return PPS_RELATE_EDGE;	/* cannot PLL with atom code */

	/* all checks passed, gets an AAA rating here! */
	return PPS_RELATE_PHASE; /* can PLL with atom code */
}
#endif	/* HAVE_PPSAPI */

/*
 * -------------------------------------------------------------------
 * nmea_receive - receive data from the serial interface
 *
 * This is the workhorse for NMEA data evaluation:
 *
 * + it checks all NMEA data, and rejects sentences that are not valid
 *   NMEA sentences
 * + it cecks whether a sentence is known and its processing is enabled
 * + it parses the time and date data from the NMEA data string and
 *   augments the missing bits. (century in dat, whole date, ...)
 * + it rejects data that is not from the first accepted sentence in a
 *   burst
 * + it evetually replaces the PPS edge time for the receive time
 * + it feeds the data to the internal processing stages.
 * -------------------------------------------------------------------
 */
static void
nmea_receive(
	struct recvbuf *rbufp
	)
{
	/* declare & init control structure ptrs */
	struct peer	    *const peer = rbufp->recv_peer;
	struct refclockproc *const pp	= peer->procptr;
	struct nmeaunit	    *const up	= (struct nmeaunit*)pp->unitptr;

	/* Use these variables to hold data until we decide its worth keeping */
	struct nmeadata rdata;
	char	rd_lastcode[BMAX];
	l_fp	rd_timestamp, rd_reftime;
	int	rd_lencode;
	double	rd_fudge;

	/* working stuff */
	struct calendar date;	/* to keep & convert the time stamp */
	int sentence=0, rc_date=0, rc_time=0; /* results of name/date/time parsing */
	int checkres;
	int32 daytime;
	char *cp;
	
	/* Read the timecode and timestamp, then initialise field
	 * processing. The <CR><LF> at the NMEA line end is translated
	 * to <LF><LF> by the terminal input routines on most systems,
	 * and this gives us one spurious empty read per record which we
	 * better ignore silently.
	 */
	rd_lencode = refclock_gtlin(rbufp, rd_lastcode,
				    sizeof(rd_lastcode), &rd_timestamp);
	checkres = field_init(&rdata, rd_lastcode, rd_lencode);
	switch (checkres) {
	case CHECK_INVALID:
		DPRINTF(1, ("nmea: invalid data: '%s'\n", rd_lastcode));
		refclock_report(peer, CEVNT_BADREPLY);
	case CHECK_EMPTY:
		return;
	default:
		DPRINTF(1, ("nmea: gpsread: %d '%s'\n",
			    rd_lencode, rd_lastcode));
		break;
	}
	/* --> below this point we have a valid NMEA sentence <-- */
	
	/* Check sentence name. Skip first 2 chars (talker ID), to allow
	 * for $GLGGA and $GPGGA etc. Since the name field has at least 5
	 * chars we can simply shift the field start.
	 */
	cp = field_parse(&rdata, 0) + 2;
	if (	 strncmp(cp, "RMC,", 4) == 0)
		sentence = NMEA_GPRMC;
	else if (strncmp(cp, "GGA,", 4) == 0)
		sentence = NMEA_GPGGA;
	else if (strncmp(cp, "GLL,", 4) == 0)
		sentence = NMEA_GPGLL;
	else if (strncmp(cp, "ZDA,", 4) == 0)
		sentence = NMEA_GPZDA;
	else if (strncmp(cp, "ZDG,", 4) == 0)
		sentence = NMEA_GPZDG;
	else
		return;	/* not something we know about */

	/* eventually output delay measurement now. */
	if (peer->ttl & NMEA_DELAYMEAS_MASK) {
		LIB_GETBUF(cp);
		snprintf(cp, LIB_BUFLENGTH, "delay %0.6f %.*s",
			 ldexp(rd_timestamp.l_uf, -32),
			 (int)(strchr(rd_lastcode, ',') - rd_lastcode),
			 rd_lastcode);
		record_clock_stats(&peer->srcadr, cp);
	}
	
	/* See if I want to process this message type */
	if ( (peer->ttl & NMEA_MESSAGE_MASK	 ) &&
	    !(peer->ttl & sentence_mode[sentence])  )
		return;

	/* make sure it came in clean
	 *
	 * Apparently, older NMEA specifications (which are expensive)
	 * did not require the checksum for all sentences.  $GPMRC is
	 * the only one so far identified which has always been required
	 * to include a checksum.
	 *
	 * Today, most NMEA GPS receivers checksum every sentence.  To
	 * preserve its error-detection capabilities with modern GPSes
	 * while allowing operation without checksums on all but $GPMRC,
	 * we keep track of whether we've ever seen a valid checksum on
	 * a given sentence, and if so, reject future instances without
	 * checksum.  ('up->cksum_type[NMEA_GPRMC]' is set in
	 * 'nmea_start()' to enforce checksums for $GPRMC right from the
	 * start.)
	 */
	if (up->cksum_type[sentence] <= (u_char)checkres)
		up->cksum_type[sentence] = (u_char)checkres;
	else {
		DPRINTF(1, ("nmea: checksum missing: '%s'\n", rd_lastcode));
		refclock_report(peer, CEVNT_BADREPLY);
		return;
	}

	/* $GPZDG provides GPS time not UTC, and the two mix poorly.
	 * Once have processed a $GPZDG, do not process any further UTC
	 * sentences (all but $GPZDG currently).
	 */
	if (up->gps_time && NMEA_GPZDG != sentence)
		return;

	DPRINTF(1, ("nmea: processing %d bytes, timecode '%s'\n",
		    rd_lencode, rd_lastcode));

	/* Grab fields depending on clock string type and possibly wipe
	 * sensitive data from the last timecode.
	 */
	memset(&date, 0, sizeof(date));	/* pristine state of stamp */
	switch (sentence)
	{
	case NMEA_GPRMC:
		/* Check quality byte, fetch data & time; need recv
		 * date here to augment century to date */
		ntpcal_ntp_to_date(&date, rd_timestamp.l_ui, NULL);
		rc_time	 = parse_time(field_parse(&rdata, 1),
				      &date, &pp->nsec);
		pp->leap = parse_qual(field_parse(&rdata, 2),
				      'A', 0);
		rc_date	 = parse_date(field_parse(&rdata, 9),
				      &date, DATE_1_DDMMYY);
		if (CLK_FLAG4 & pp->sloppyclockflag)
			field_wipe(&rdata, 3, 4, 5, 6, -1);
		break;

	case NMEA_GPGGA:
		/* Check quality byte, fetch time only */
		rc_time	 = parse_time(field_parse(&rdata, 1),
				      &date, &pp->nsec);
		pp->leap = parse_qual(field_parse(&rdata, 6),
				      '0', 1);
		rc_date	 = unfold_day(&date, rd_timestamp.l_ui);
		if (CLK_FLAG4 & pp->sloppyclockflag)
			field_wipe(&rdata, 2, 4, -1);
		break;

	case NMEA_GPGLL:
		/* Check quality byte, fetch time only */
		rc_time	 = parse_time(field_parse(&rdata, 5),
				      &date, &pp->nsec);
		pp->leap = parse_qual(field_parse(&rdata, 6),
				      'A', 0);
		rc_date	 = unfold_day(&date, rd_timestamp.l_ui);
		if (CLK_FLAG4 & pp->sloppyclockflag)
			field_wipe(&rdata, 1, 3, -1);
		break;
	
	case NMEA_GPZDA:
		/* No quality. Assume best, fetch time & full date */
		pp->leap = LEAP_NOWARNING;
		rc_time	 = parse_time(field_parse(&rdata, 1),
				      &date, &pp->nsec);
		rc_date	 = parse_date(field_parse(&rdata, 2),
				      &date, DATE_3_DDMMYYYY);
		break;

	case NMEA_GPZDG:
		/* Check quality byte, fetch time & full date */
		rc_time	 = parse_time(field_parse(&rdata, 1),
				      &date, &pp->nsec);
		rc_date	 = parse_date(field_parse(&rdata, 2),
				      &date, DATE_3_DDMMYYYY);
		pp->leap = parse_qual(field_parse(&rdata, 4),
				      '0', 1);
		break;
		
	default:
		return;
	}

	/* Check sanity of time-of-day. */
	if (rc_time <= 0) {	/* no time or conversion error? */
		refclock_report(peer, CEVNT_BADTIME);
		return;
	}
	/* Check sanity of date. */
	if (rc_date <= 0) {	/* no date or conversion error? */
		refclock_report(peer, CEVNT_BADDATE);
		return;
	}
	/* Discard sentence if time-of-day (in seconds) did not change */
	daytime = ntpcal_date_to_daysec(&date);
	if (up->last_daytime == daytime)
		return;
	up->last_daytime = daytime;
	
	DPRINTF(1, ("nmea_receive: effective timecode: %04u-%02u-%02u %02d:%02d:%02d\n",
		date.year, date.month, date.monthday,
		    date.hour, date.minute, date.second));

	/* Store data for statistical purposes... */
	if (rd_lencode >= sizeof(pp->a_lastcode))
		rd_lencode = sizeof(pp->a_lastcode) - 1;
	pp->lencode = (u_short)rd_lencode;
	memcpy(pp->a_lastcode, rd_lastcode, rd_lencode);
	pp->a_lastcode[rd_lencode] = '\0';
	pp->lastrec = rd_timestamp;

	/* Get the reference time stamp from the calendar buffer.
	 * Process the new sample in the median filter and determine the
	 * timecode timestamp, but only if the PPS is not in control.
	 */
	rd_fudge = pp->fudgetime2;
	DTOLFP(pp->nsec * 1.0e-9, &rd_reftime);
	rd_reftime.l_ui += caltontp(&date);
	
	/* $GPZDG postprocessing first...
	 * $GPZDG indicates the second after the *next* PPS pulse. So
	 * we remove 1 second from the reference time now. And since
	 * GPS timescale will be used from now, tell and log this fact.
	 */
	if (sentence == NMEA_GPZDG) {
		if (!up->gps_time) {
			up->gps_time = 1;
			NLOG(NLOG_CLOCKINFO)
			msyslog(LOG_INFO, "%s using only $GPZDG",
				refnumtoa(&peer->srcadr));
		}
		rd_reftime.l_ui--;
	}

#ifdef HAVE_PPSAPI
	up->tcount++;	/* received true timestamp */
	/* If we have PPS running, we try to associate the sentence
	 * with the last active edge of the PPS signal.
	 */
	if (up->ppsapi_lit)
		switch (refclock_ppsrelate(pp, &up->atom, &rd_reftime,
					  &rd_timestamp, pp->fudgetime1,
					  &rd_fudge))
		{
		case PPS_RELATE_EDGE:
			up->ppsapi_gate = 0;
			break;
		case PPS_RELATE_PHASE:
			up->ppsapi_gate = 1;
			break;
		default:
			break;
		}
	else 
		up->ppsapi_gate = 0;

	if (up->ppsapi_gate && (peer->flags & FLAG_PPS))
		return;
#endif /* HAVE_PPSAPI */

	refclock_process_offset(pp, rd_reftime, rd_timestamp, rd_fudge);
}


/*
 * -------------------------------------------------------------------
 * nmea_poll - called by the transmit procedure
 *
 * Does the necessary bookkeeping stuff to keep the reported state of
 * the clock in sync with reality.
 *
 * We go to great pains to avoid changing state here, since there may
 * be more than one eavesdropper receiving the same timecode.
 * -------------------------------------------------------------------
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

# if 0
	/*
	 * usually nmea_receive can get a timestamp every second, but
	 * at least one Motorola unit needs prompting each time. And
	 * since we may bail out early, we do this immediately.
	 */
	gps_send(pp->io.fd,"PMOTG,RMC,0000", peer);
#endif
	
	/*
	 * Process median filter samples. If none received, declare a
	 * timeout and keep going.
	 */
#ifdef HAVE_PPSAPI
	/*
	 * If we don't have PPS pulses and time stamps, turn PPS down
	 * for now.
	 */
	if (up->pcount == 0 || up->tcount == 0) {
		peer->flags &= ~FLAG_PPS;
		peer->precision = PRECISION;
		up->ppsapi_gate = 0;
	}
	/*
	 * If we don't have real time stamps flush the median filter;
	 * it might contain stale PPS time stamps otherwise.
	 */
	if (up->tcount == 0) 
		pp->coderecv = pp->codeproc;
	/*
	 * reset counters for next cycle.
	 */
	up->pcount = up->tcount = 0;
#endif /* HAVE_PPSAPI */
	/*
	 * If the median filter is empty, claim a timeout and leave
	 */
	if (pp->coderecv == pp->codeproc) {
		refclock_report(peer, CEVNT_TIMEOUT);
		return;
	}

	/* keep stats going */
	pp->polls++;
	pp->lastref = pp->lastrec;
	refclock_receive(peer);
	record_clock_stats(&peer->srcadr, pp->a_lastcode);
}

/*
 * -------------------------------------------------------------------
 *	gps_send(fd,cmd, peer)	Sends a command to the GPS receiver.
 *	 as	gps_send(fd, "rqts,u", peer);
 *
 * The function will create the necessary frame (start char, chksum,
 * final CRLF) on the fly.
 *
 *	We don't currently send any data, but would like to send
 *	RTCM SC104 messages for differential positioning. It should
 *	also give us better time. Without a PPS output, we're
 *	Just fooling ourselves because of the serial code paths
 * -------------------------------------------------------------------
 */
static void
gps_send(
	int fd,
	const char *cmd,
	struct peer *peer
	)
{
	char   buf[NMEA_PROTO_MAXLEN + 7]; /* $...*xy<CR><LF><NUL> add 7 */
	u_char len = NMEA_PROTO_MAXLEN;
	u_char dcs = 0;
	char  *dst = buf;
	char  *end = buf + sizeof(buf) - 1; /* last char */

	/*
	 * copy data to buffer, creating checksum and frame on the fly
	 */
	if (*cmd == '$')
		cmd++;
	*dst++ = '$';
	while (len-- && *cmd && *cmd != '*')
		dcs ^= *dst++ = *cmd++;
	snprintf(dst, end - dst, "*%02X\r\n", dcs);
	*end = '\0';
	dst += strlen(dst);
	len = dst - buf;
	DPRINTF(1, ("nmea: gps_send: '%.*s'\n", len-2, buf));

	/* send out the whole stuff */
	if (write(fd, buf, len) == -1) {
		refclock_report(peer, CEVNT_FAULT);
	}
}

/*
 * -------------------------------------------------------------------
 * helpers for faster field splitting
 * -------------------------------------------------------------------
 *
 * set up a field record, check syntax and verify checksum
 *
 * format is $XXXXX,1,2,3,4*ML
 *
 * 8-bit XOR of characters between $ and * noninclusive is transmitted
 * in last two chars M and L holding most and least significant nibbles
 * in hex representation such as:
 *
 *   $GPGLL,5057.970,N,00146.110,E,142451,A*27
 *   $GPVTG,089.0,T,,,15.2,N,,*7F
 *
 * Some other constraints:
 * + The field name must at least 5 upcase characters or digits and must
 *   start with a character.
 * + The checksum (if present) must be uppercase hex digits.
 * + The length of a sentence is limited to 80 characters (not including
 *   the final CR/LF nor the checksum, but including the leading '$')
 *
 * Return values:
 *  + CHECK_INVALID
 *	The data does not form a valid NMEA sentence or a checksum error
 *	occurred.
 *  + CHECK_VALID
 *	The data is a valid NMEA sentence but contains no checksum.
 *  + CHECK_CSVALID
 *	The data is a valid NMEA sentence and passed the checksum test.
 * -------------------------------------------------------------------
 */
static int
field_init(
	struct nmeadata *data,	/* context structure			*/
	char		*cptr,	/* start of raw data			*/
	int		 dlen)	/* data len, not counting trailing NUL	*/
{
	u_char cs_l=0, cs_r=0; /* checksum local computed / remote given */
	char *eptr, tmp;       /* end ptr and char buffer */
	
	/* some basic input constraints */
	if (dlen < 0)
		dlen = 0;
	eptr = cptr + dlen;
	*eptr = '\0';
	
	/* load data context */	
	data->base = cptr;
	data->cptr = cptr;
	data->cidx = 0;
	data->blen = dlen;

	/* syntax check follows here. check allowed character
	 * sequences, updating the local computed checksum as we go.
	 *
	 * regex equiv: '^\$[A-Z][A-Z0-9]{4,}[^*]*(\*[0-9A-F]{2})?$'
	 */

	/* -*- start character: '^\$' */
	if (*cptr == '\0')
		return CHECK_EMPTY;
	if (*cptr++ != '$')
		return CHECK_INVALID;

	/* -*- advance context beyond start character */
	data->base++;
	data->cptr++;
	data->blen--;
	
	/* -*- field name: '[A-Z][A-Z0-9]{4,},' */
	if (*cptr < 'A' || *cptr > 'Z')
		return CHECK_INVALID;
	cs_l ^= *cptr++;
	while ((*cptr >= 'A' && *cptr <= 'Z') ||
	       (*cptr >= '0' && *cptr <= '9')  )
		cs_l ^= *cptr++;
	if (*cptr != ',' || (cptr - data->base) < NMEA_PROTO_IDLEN)
		return CHECK_INVALID;
	cs_l ^= *cptr++;

	/* -*- data: '[^*]*' */
	while (*cptr && *cptr != '*')
		cs_l ^= *cptr++;
	
	/* -*- checksum field: (\*[0-9A-F]{2})?$ */
	if (*cptr == '\0')
		return CHECK_VALID;
	if (*cptr != '*' || cptr != eptr - 3 ||
	    (cptr - data->base) >= NMEA_PROTO_MAXLEN)
		return CHECK_INVALID;

	for (cptr++; (tmp = *cptr) != '\0'; cptr++)
		if (tmp >= '0' && tmp <= '9')
			cs_r = (cs_r << 4) + (tmp - '0');
		else if (tmp >= 'A' && tmp <= 'F')
			cs_r = (cs_r << 4) + (tmp - 'A' + 10);
		else
			break;

	/* -*- make sure we are at end of string and csum matches */
	if (cptr != eptr || cs_l != cs_r)
		return CHECK_INVALID;

	return CHECK_CSVALID;
}

/*
 * -------------------------------------------------------------------
 * fetch a data field by index, zero being the name field. If this
 * function is called repeatedly with increasing indices, the total load
 * is O(n), n being the length of the string; if it is called with
 * decreasing indices, the total load is O(n^2). Try not to go backwards
 * too often.
 * -------------------------------------------------------------------
 */
static char *
field_parse(
	struct nmeadata *data,
	int		 fn  )
{
	char tmp;

	if (fn < data->cidx) {
		data->cidx = 0;
		data->cptr = data->base;
	}
	while ((fn > data->cidx) && (tmp = *data->cptr) != '\0') {
		data->cidx += (tmp == ',');
		data->cptr++;
	}
	return data->cptr;
}

/*
 * -------------------------------------------------------------------
 * Wipe (that is, overwrite with '_') data fields and the checksum in
 * the last timecode.  The list of field indices is given as integers
 * in a varargs list, preferrably in ascending order and terminated by
 * a negative field index.
 *
 * A maximum number of 8 fields can be overwritten at once to guard
 * against runaway (that is, unterminated) argument lists.
 *
 * This function affects what a remote user can see with
 *
 * ntpq -c clockvar <server>
 *
 * Note that this also removes the wiped fields from any clockstats
 * log.	 Some NTP operators monitor their NMEA GPS using the change in
 * location in clockstats over time as as a proxy for the quality of
 * GPS reception and thereby time reported.
 * -------------------------------------------------------------------
 */
static void
field_wipe(
	struct nmeadata *data,
	... ) /* this must be a list of ints, terminated by a value less
	       * than zero */
{
	va_list va;		/* vararg index list */
	int	fcnt = 8;	/* safeguard against runaway arglist */
	int	fidx;		/* field to nuke, or -1 for checksum */
	char   *cp   = NULL;	/* overwrite destination */
	
	va_start(va, data);
	do {
		fidx = va_arg(va, int);
		if (fidx >= 0 && fidx <= NMEA_PROTO_FIELDS) {
			cp = field_parse(data, fidx);
		} else {
			cp = data->base + data->blen;
			if (data->blen >= 3 && cp[-3] == '*')
				cp -= 2;
		}
		for (/*NOP*/; '\0' != *cp && '*' != *cp && ',' != *cp;	cp++)
			if ('.' != *cp)
				*cp = '_';
	} while (fcnt-- && fidx >= 0);
	va_end(va);	
}

/*
 * -------------------------------------------------------------------
 * PARSING HELPERS
 * -------------------------------------------------------------------
 *
 * Check sync status
 *
 * If the character at the data field start matches the tag value,
 * return LEAP_NOWARNING and LEAP_NOTINSYNC otherwise. If the 'inverted'
 * flag is given, just the opposite value is returned. If there is no
 * data field (*cp points to the NUL byte) the reult is LEAP_NOTINSYNC.
 * -------------------------------------------------------------------
 */
static int
parse_qual(
	const char *dp , 
	char	    tag,
	int	    inv)
{
	static const int table[2] = { LEAP_NOTINSYNC, LEAP_NOWARNING };

	return table[ *dp && ((*dp == tag) == !inv) ];
}

/*
 * -------------------------------------------------------------------
 * Parse a time stamp in HHMMSS[.sss] format with error checking.
 *
 * returns 1 on success, -1 on failure
 * -------------------------------------------------------------------
 */
static int
parse_time(
	const char	*dp, 
	struct calendar *jd,	/* result pointer */
	long		*ns)	/* optional storage for nsec fraction */
{
	static const unsigned long weight[4] = {
		0, 100000000, 10000000, 1000000
	};

	int	      rc;
	unsigned int  h, m, s, p1, p2;
	unsigned long f;

	rc = sscanf(dp, "%2u%2u%2u%n.%3lu%n", &h, &m, &s, &p1, &f, &p2);
	if (rc < 3 || p1 != 6) {
		DPRINTF(1, ("nmea: invalid time code: '%.6s'\n", dp));
		return -1;
	}
	
	/* value sanity check */
	if (h > 23 || m > 59 || s > 60) {
		DPRINTF(1, ("nmea: invalid time spec %02u:%02u:%02u\n",
			    h, m, s));
		return -1;
	}

	jd->hour   = h;
	jd->minute = m;
	jd->second = s;
	/* if we have and need a fraction, scale it up to nanoseconds. */
	if (ns) {
		if (rc == 4)
			*ns = f * weight[p2 - p1 - 1];
		else
			*ns = 0;
	}

	return 1;
}

/*
 * -------------------------------------------------------------------
 * Parse a date string from an NMEA sentence. This could either be a
 * partial date in DDMMYY format in one field, or DD,MM,YYYY full date
 * spec spanning three fields. This function does some extensive error
 * checking to make sure the date string was consistent.
 *
 * A 2-digit year is expanded into full year spec around the year found
 * in 'jd->year'. This should be in -79/+19 years around the true time,
 * or the result will be off by 100 years.  The assymetric behaviour was
 * chosen to enable inital sync for systems that do not have a
 * battery-backup clock and start with a date that is typically years in
 * the past.
 *
 * returns 1 on success, -1 on failure
 * -------------------------------------------------------------------
 */
static int
parse_date(
	const char	*dp, 
	struct calendar *jd   ,	/* result pointer, may contain a year */
	enum date_fmt	 fmt  )
{
	int	     rc, ybase;
	unsigned int y, m, d, p;
	
	switch (fmt) {
	case DATE_1_DDMMYY:
		rc = sscanf(dp, "%2u%2u%2u%n", &d, &m, &y, &p);
		if (rc != 3 || p != 6) {
			DPRINTF(1, ("nmea: invalid date code: '%.6s'\n",
				    dp));
			return -1;
		}
		/* augment century, based on year in 'jd-year' */
		ybase = (int)(jd->year ? jd->year : 1990) - 20;
		y = ntpcal_periodic_extend(ybase, y, 100);
		break;

	case DATE_3_DDMMYYYY:
		rc = sscanf(dp, "%2u,%2u,%4u%n", &d, &m, &y, &p);
		if (rc != 3 || p != 10) {
			DPRINTF(1, ("nmea: invalid date code: '%.10s'\n",
			    dp));
			return -1;
		}
		break;

	default:
		DPRINTF(1, ("nmea: invalid parse format: %d\n", fmt));
		return -1;
	}

	/* value sanity check */
	if (d < 1 || d > 31 || m < 1 || m > 12) {
		DPRINTF(1, ("nmea: invalid date spec (YMD) %04u:%02u:%02u\n",
			    y, m, d));
		return -1;
	}
	
	/* store results */
	jd->monthday = d;
	jd->month    = m;
	jd->year     = y;

	return 1;
}

/*
 * -------------------------------------------------------------------
 * funny calendar-oriented stuff -- perhaps a bit hard to grok.
 * -------------------------------------------------------------------
 *
 * Unfold a time-of-day (seconds since midnight) around the current
 * system time in a manner that guarantees an absolute difference of
 * less than 12hrs.
 *
 * This function is used for NMEA sentences that contain no date
 * information. This requires the system clock to be in +/-12hrs
 * around the true time, or the clock will synchronize the system 1day
 * off if not augmented with a time sources that also provide the
 * necessary date information.
 *
 * The function updates the calendar structure it also uses as
 * input to fetch the time from.
 * -------------------------------------------------------------------
 */
static int
unfold_day(
	struct calendar *jd    ,
	u_int32		 rec_ui)
{
	vint64	     rec_qw;
	ntpcal_split rec_ds;
	int	     cvtres;

	/*
	 * basically this is the peridiodic extension of the receive
	 * time - 12hrs to the time-of-day with a period of 1 day.
	 * But we would have to execute this in 64bit arithmetic, and we
	 * cannot assume we can do this; therefore this is done
	 * manually.
	 *
	 * Caveat: The time spec in '*jd' must be normalised; the time
	 * parsing function takes care of this.
	 */
	rec_qw = ntpcal_ntp_to_ntp(rec_ui, NULL);
	rec_ds = ntpcal_daysplit(&rec_qw);
	
	rec_ds.lo = ntpcal_date_to_daysec(jd) - rec_ds.lo;
	if (rec_ds.lo < -SECSPERDAY/2)
		rec_ds.hi++;
	if (rec_ds.lo >= SECSPERDAY/2)
		rec_ds.hi--;
	
	cvtres = ntpcal_rd_to_date(jd, rec_ds.hi + DAY_NTP_STARTS);
	return (cvtres >= 0) ? 1 : -1;
}
#else
int refclock_nmea_bs;
#endif /* REFCLOCK && CLOCK_NMEA */
