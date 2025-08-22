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

#define NMEA_WRITE_SUPPORT 0 /* no write support at the moment */

#include "config.h"
#include "ntp_types.h"
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>

#include "ntp.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"
#include "timespecops.h"

#ifdef HAVE_PPSAPI
# include "ppsapi_timepps.h"
# include "refclock_pps.h"
#endif /* HAVE_PPSAPI */


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
 * The PPSAPI part of the driver understands options flag2 and flag3. If
 * flag2 is set, it will use the clear edge of the pulse. If flag3 is
 * set, kernel hardpps is enabled.
 *
 * GPS sentences other than RMC (the default) may be enabled by setting
 * the relevant bits of 'mode' in the server configuration line
 * refclock u mode X
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
#define NMEA_MESSAGE_MASK	0x0000FF0FU
#define NMEA_BAUDRATE_MASK	0x00000070U
#define NMEA_BAUDRATE_SHIFT	4

#define NMEA_DELAYMEAS_MASK	0x80
#define NMEA_EXTLOG_MASK	0x00010000U
#define NMEA_DATETRUST_MASK	0x02000000U

#define NMEA_PROTO_IDLEN	5	/* tag name must be at least 5 chars */
/* #define NMEA_PROTO_MINLEN 6 * min chars in sentence, excluding CS UNUSED */
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
 *
 * Defining PGRMF for Garmin GPS Fix Data
 * $PGRMF,WN,WS,DATE,TIME,LS,LAT,LAT_DIR,LON,LON_DIR,MODE,FIX,SPD,DIR,PDOP,TDOP
 * WN  -- GPS week number (weeks since 1980-01-06, mod 1024)
 * WS  -- GPS seconds in week
 * LS  -- GPS leap seconds, accumulated ( UTC + LS == GPS )
 * FIX -- Fix type: 0=nofix, 1=2D, 2=3D
 * DATE/TIME are standard date/time strings in UTC time scale
 *
 * The GPS time can be used to get the full century for the truncated
 * date spec.
 */

/*
 * Definitions
 */
#define	DEVICE		"/dev/gps%d"	/* GPS serial device */
#ifdef HAVE_PPSAPI
# define	PPSDEV		"/dev/gpspps%d"	/* PPSAPI device override */
# define	PPS_PRECISION	(-20)		/* precision assumed (~ 1 us) */
#endif
#ifdef ENABLE_CLASSIC_MODE
#define	SPEED232	B4800		/* uart speed (4800 bps) */
#else
#define	SPEED232	B9600		/* uart speed (9600 bps) */
#endif
#define	PRECISION	(-9)		/* precision assumed (about 2 ms) */
#define	REFID		"GPS\0"		/* reference id */
#define	NAME		"NMEA"		/* shortname */
#define	DESCRIPTION	"NMEA GPS Clock" /* who we are */

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
#define NMEA_PGRMF	5
#define NMEA_ARRAY_SIZE (NMEA_PGRMF + 1)

/*
 * Sentence selection mode bits
 */
#define USE_GPRMC		0x00000001u
#define USE_GPGGA		0x00000002u
#define USE_GPGLL		0x00000004u
#define USE_GPZDA		0x00000008u
#define USE_PGRMF		0x00000100u

/* mapping from sentence index to controlling mode bit */
static const uint32_t sentence_mode[NMEA_ARRAY_SIZE] =
{
	USE_GPRMC,
	USE_GPGGA,
	USE_GPGLL,
	USE_GPZDA,
	USE_GPZDA,
	USE_PGRMF
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
typedef struct {
#ifdef HAVE_PPSAPI
	struct refclock_ppsctl ppsctl; /* PPSAPI structure */
	int	ppsapi_fd;	/* fd used with PPSAPI */
	bool	ppsapi_tried;	/* attempt PPSAPI once */
	bool	ppsapi_lit;	/* time_pps_create() worked */
	bool	ppsapi_gate;	/* system is on PPS */
#endif /* HAVE_PPSAPI */
	bool	gps_time;	/* use GPS time, not UTC */
	l_fp	last_reftime;	/* last processed reference stamp */
	int 	wnro;		/* last epoch warp, for logging */
	/* tally stats, reset each poll cycle */
	struct
	{
		unsigned int total;
		unsigned int accepted;
		unsigned int rejected;   /* GPS said not enough signal */
		unsigned int malformed;  /* Bad checksum, invalid date or time */
		unsigned int filtered;   /* mode bits, not GPZDG, same second */
		unsigned int pps_used;
	}
		tally;
	/* per sentence checksum seen flag */
	uint8_t	cksum_type[NMEA_ARRAY_SIZE];
} nmea_unit;

/*
 * helper for faster field access
 */
typedef struct {
	char  *base;	/* buffer base		*/
	char  *cptr;	/* current field ptr	*/
	int    blen;	/* buffer length	*/
	int    cidx;	/* current field index	*/
} nmea_data;


/*
 * Function prototypes
 */
static	bool	nmea_start	(int, struct peer *);
static	void	nmea_shutdown	(struct refclockproc *);
static	void	nmea_receive	(struct recvbuf *);
static	void	nmea_poll	(int, struct peer *);
#ifdef HAVE_PPSAPI
static	void	nmea_control	(int, const struct refclockstat *,
				 struct refclockstat *, struct peer *);
#define		NMEA_CONTROL	nmea_control
#else
#define		NMEA_CONTROL	NULL
#endif /* HAVE_PPSAPI */
static	void	nmea_timer	(int, struct peer *);

/* parsing helpers */
static int	field_init	(nmea_data * data, char * cp, int len);
static char *	field_parse	(nmea_data * data, int fn);
static void	field_wipe	(nmea_data * data, ...);
static uint8_t	parse_qual	(nmea_data * data, int idx,
				 char tag, int inv);
static bool	parse_time	(struct timespec *dt, nmea_data *, int idx);
static bool	parse_date	(struct timespec *dt, nmea_data*,
					int idx, enum date_fmt fmt);
static bool	kludge_day	(struct timespec *dt);

static void     save_ltc        (struct refclockproc * const, const char * const,
				 size_t);

/*
 * If we want the driver to output sentences, too: re-enable the send
 * support functions by defining NMEA_WRITE_SUPPORT to non-zero...
 */
#if NMEA_WRITE_SUPPORT
static	void gps_send(int, const char *, struct peer *);
#endif /* NMEA_WRITE_SUPPORT */

/*
 * -------------------------------------------------------------------
 * Transfer vector
 * -------------------------------------------------------------------
 */
struct refclock refclock_nmea = {
	NAME,			/* basename of driver */
	nmea_start,		/* start up driver */
	nmea_shutdown,		/* shut down driver */
	nmea_poll,		/* transmit poll message */
	NMEA_CONTROL,		/* fudge control */
	NULL,			/* initialize driver */
	nmea_timer		/* called once per second */
};

/*
 * -------------------------------------------------------------------
 * nmea_start - open the GPS devices and initialize data for processing
 *
 * return false on error, true on success. Even on error the peer structures
 * must be in a state that permits 'nmea_shutdown()' to clean up all
 * resources, because it will be called immediately to do so.
 * -------------------------------------------------------------------
 */
static bool
nmea_start(
	int		unit,
	struct peer *	peer
	)
{
	struct refclockproc * const	pp = peer->procptr;
	nmea_unit * const		up = emalloc_zero(sizeof(*up));
	char				device[20], *path;
	uint32_t			rate;
	unsigned int			baudrate;
	const char *			baudtext;
        int rcode;


	/* Old style: get baudrate choice from mode byte bits 4/5/6 */
	rate = (peer->cfg.mode & NMEA_BAUDRATE_MASK) >> NMEA_BAUDRATE_SHIFT;

	/* New style: get baudrate from baud option */
	if (peer->cfg.baud)
		rate = peer->cfg.baud;

	switch (rate) {
	case 0:
	case 4800:
		baudrate = B4800;
		baudtext = "4800";
		break;
	case 1:
	case 9600:
		baudrate = B9600;
		baudtext = "9600";
		break;
	case 2:
	case 19200:
		baudrate = B19200;
		baudtext = "19200";
		break;
	case 3:
	case 38400:
		baudrate = B38400;
		baudtext = "38400";
		break;
#ifdef B57600
	case 4:
	case 57600:
		baudrate = B57600;
		baudtext = "57600";
		break;
#endif
#ifdef B115200
	case 5:
	case 115200:
		baudrate = B115200;
		baudtext = "115200";
		break;
#endif
    /* Speeds faster than 115200 are only available via the baud option. */
#ifdef B230400
        case 230400:
                baudrate = B230400;
                baudtext = "230400";
                break;
#endif
#ifdef B460800
        case 460800:
                baudrate = B460800;
                baudtext = "460800";
                break;
#endif
#ifdef B500000
        case 500000:
                baudrate = B500000;
                baudtext = "500000";
                break;
#endif
#ifdef B576000
        case 576000:
                baudrate = B576000;
                baudtext = "576000";
                break;
#endif
#ifdef B921600
        case 921600:
                baudrate = B921600;
                baudtext = "921600";
                break;
#endif
#ifdef B1000000
        case 1000000:
                baudrate = B1000000;
                baudtext = "1000000";
                break;
#endif
#ifdef B1152000
        case 1152000:
                baudrate = B1152000;
                baudtext = "1152000";
                break;
#endif
#ifdef B1500000
        case 1500000:
                baudrate = B1500000;
                baudtext = "1500000";
                break;
#endif
#ifdef B2000000
        case 2000000:
                baudrate = B2000000;
                baudtext = "2000000";
                break;
#endif
#ifdef B2500000
        case 2500000:
                baudrate = B2500000;
                baudtext = "2500000";
                break;
#endif
#ifdef B3000000
        case 3000000:
                baudrate = B3000000;
                baudtext = "3000000";
                break;
#endif
#ifdef B3500000
        case 3500000:
                baudrate = B3500000;
                baudtext = "3500000";
                break;
#endif
#ifdef B4000000
        case 4000000:
                baudrate = B4000000;
                baudtext = "4000000";
                break;
#endif
	default:
		baudrate = SPEED232;
#ifdef ENABLE_CLASSIC_MODE
		baudtext = "4800 (fallback)";
#else
		baudtext = "9600 (fallback)";
#endif
		break;
	}

	/* Allocate and initialize unit structure */
	pp->unitptr = (void *)up;
	pp->io.fd = -1;
	pp->io.clock_recv = nmea_receive;
	pp->io.srcclock = peer;
	pp->io.datalen = 0;
	/* force change detection on first valid message */
	memset(&up->last_reftime, 0xFF, sizeof(up->last_reftime));
	/* force checksum on GPRMC, see below */
	up->cksum_type[NMEA_GPRMC] = CHECK_CSVALID;
#ifdef HAVE_PPSAPI
	up->ppsapi_fd = -1;
#endif
	ZERO(up->tally);

	/* Initialize miscellaneous variables */
	peer->precision = PRECISION;
	pp->clockname = NAME;
	pp->clockdesc = DESCRIPTION;
	memcpy(&pp->refid, REFID, REFIDLEN);
	peer->sstclktype = CTL_SST_TS_UHF;

	if (peer->cfg.path)
		path = peer->cfg.path;
	else
	{
		/* build a path */
		rcode = snprintf(device, sizeof(device), DEVICE, unit);
		if ( 0 > rcode ) {
		    /* failed, set to NUL */
		    device[0] = '\0';
		}
		path = device;
        }
	/* Open serial port. */
	pp->io.fd = refclock_open(path, baudrate, LDISC_STD);

	if (0 > pp->io.fd) {
		msyslog(LOG_ERR, "REFCLOCK: %s NMEA device open(%s) failed",
		    refclock_name(peer), path);
		return false;
        }

	LOGIF(CLOCKINFO, (LOG_NOTICE, "%s serial %s open at %s bps",
			  refclock_name(peer), path, baudtext));

	/* succeed if this clock can be added */
	return io_addclock(&pp->io) != 0;
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
	struct refclockproc * pp
	)
{
	nmea_unit	    * const up = (nmea_unit *)pp->unitptr;

	if (up != NULL) {
#ifdef HAVE_PPSAPI
		if (up->ppsapi_lit)
			time_pps_destroy(up->ppsctl.handle);
		if (up->ppsapi_tried && up->ppsapi_fd != pp->io.fd)
			close(up->ppsapi_fd);
#endif
		free(up);
	}
	pp->unitptr = (void *)NULL;
	if (-1 != pp->io.fd)
		io_closeclock(&pp->io);
	pp->io.fd = -1;
}

/*
 * -------------------------------------------------------------------
 * nmea_control - configure fudge params
 * -------------------------------------------------------------------
 */
#ifdef HAVE_PPSAPI
static void
nmea_control(
	int                         unit,
	const struct refclockstat * in_st,
	struct refclockstat       * out_st,
	struct peer               * peer
	)
{
	struct refclockproc * const pp = peer->procptr;
	nmea_unit	    * const up = (nmea_unit *)pp->unitptr;

	char   device[32];

	UNUSED_ARG(in_st);
	UNUSED_ARG(out_st);

	/*
	 * PPS control
	 *
	 * If /dev/gpspps$UNIT can be opened that will be used for
	 * PPSAPI.  Otherwise, the GPS serial device /dev/gps$UNIT
	 * already opened is used for PPSAPI as well. (This might not
	 * work, in which case the PPS API remains unavailable...)
	 */

	/* Light up the PPSAPI interface if not yet attempted. */
	if ((CLK_FLAG1 & pp->sloppyclockflag) && !up->ppsapi_tried) {

		up->ppsapi_tried = true;
		if ( !peer->cfg.ppspath ) {
		    int rcode;

		    /* build a path */
		    rcode = snprintf(device, sizeof(device), PPSDEV, unit);
		    if ( 0 > rcode ) {
                        /* failed, set to NUL */
                        device[0] = '\0';
		    }
		    peer->cfg.ppspath = estrdup( device );
		}
		if ( peer->cfg.ppspath ) {
		    up->ppsapi_fd = open(peer->cfg.ppspath,
			O_RDWR | O_NOCTTY | O_NONBLOCK);
		} else {
		    up->ppsapi_fd = -1;
		}
		if ( 0 <= up->ppsapi_fd) {
		    LOGIF(CLOCKINFO, (LOG_NOTICE, "%s PPS %s opened",
			  refclock_name(peer), peer->cfg.ppspath));
                } else {
		    /* fall back to primary device */
		    up->ppsapi_fd = pp->io.fd;
		    msyslog(LOG_ERR,
			"REFCLOCK: %s PPS device open(%s) failed",
			refclock_name(peer), peer->cfg.ppspath);
                }
		if (refclock_ppsapi(up->ppsapi_fd, &up->ppsctl)) {
			/* use the PPS API for our own purposes now. */
			up->ppsapi_lit = refclock_params(
				pp->sloppyclockflag, &up->ppsctl);
			if (!up->ppsapi_lit) {
				/* failed to configure, drop PPS unit */
				time_pps_destroy(up->ppsctl.handle);
				msyslog(LOG_WARNING,
					"REFCLOCK: %s set PPSAPI params fails",
					refclock_name(peer));
			}
			/* note: the PPS I/O handle remains valid until
			 * flag1 is cleared or the clock is shut down.
			 */
		} else {
			msyslog(LOG_WARNING,
				"REFCLOCK: %s flag1 1 but PPSAPI fails",
				refclock_name(peer));
		}
	}

	/* shut down PPS API if activated */
	if (!(CLK_FLAG1 & pp->sloppyclockflag) && up->ppsapi_tried) {
		/* shutdown PPS API */
		if (up->ppsapi_lit)
			time_pps_destroy(up->ppsctl.handle);
		up->ppsctl.handle = 0;
		/* close/drop PPS fd */
		if (up->ppsapi_fd != pp->io.fd)
			close(up->ppsapi_fd);
		up->ppsapi_fd = -1;

		/* clear markers and peer items */
		up->ppsapi_gate  = false;
		up->ppsapi_lit   = false;
		up->ppsapi_tried = false;

		peer->cfg.flags &= ~FLAG_PPS;
		peer->precision = PRECISION;
	}
}
#endif	/* HAVE_PPSAPI */

/*
 * -------------------------------------------------------------------
 * nmea_timer - called once per second
 *		this only polls (older?) Oncore devices now
 *
 * Usually 'nmea_receive()' can get a timestamp every second, but at
 * least one Motorola unit needs prompting each time. Doing so in
 * 'nmea_poll()' gives only one sample per poll cycle, which actually
 * defeats the purpose of the median filter. Polling once per second
 * seems a much better idea.
 * -------------------------------------------------------------------
 */
static void
nmea_timer(
	int	      unit,
	struct peer * peer
	)
{
#if NMEA_WRITE_SUPPORT

	struct refclockproc * const pp = peer->procptr;

	UNUSED_ARG(unit);

	if (-1 != pp->io.fd) /* any mode bits to evaluate here? */
		gps_send(pp->io.fd, "$PMOTG,RMC,0000*1D\r\n", peer);
#else

	UNUSED_ARG(unit);
	UNUSED_ARG(peer);

#endif /* NMEA_WRITE_SUPPORT */
}

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
 * next PPS edge is nearer than the last. (Similar to what the PPS
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
 * preceding edge, as most units relate their sentences to the current
 * second.
 *
 * The function returns PPS_RELATE_NONE (0) if no PPS edge correlation
 * can be fixed; PPS_RELATE_EDGE (1) when a PPS edge could be fixed, but
 * the distance to the reference time stamp is too big (exceeds
 * +/-400ms) and the PPS driver PLL cannot be used to fix the phase;
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
	const struct refclockproc  * pp	    ,	/* for sanity	  */
	const struct refclock_ppsctl * ap    ,	/* for PPS io	  */
	const l_fp		   * reftime ,
	l_fp			   * rd_stamp,	/* i/o read stamp */
	double			     pp_fudge,	/* pps fudge	  */
	double			   * rd_fudge	/* i/o read fudge */
	)
{
	pps_info_t	pps_info;
	struct timespec timeout;
	l_fp		pp_stamp, pp_delta;
	double		delta, idelta;

	if (pp->leap == LEAP_NOTINSYNC)
		return PPS_RELATE_NONE; /* clock is insane, no chance */

	ZERO(timeout);
	ZERO(pps_info);
	if (time_pps_fetch(ap->handle, PPS_TSFMT_TSPEC,
			   &pps_info, &timeout) < 0)
		return PPS_RELATE_NONE; /* can't get time stamps */

	/* get last active PPS edge before receive */
	if (ap->pps_params.mode & PPS_CAPTUREASSERT)
		timeout = pps_info.assert_timestamp;
	else if (ap->pps_params.mode & PPS_CAPTURECLEAR)
		timeout = pps_info.clear_timestamp;
	else
		return PPS_RELATE_NONE; /* WHICH edge, please?!? */

	/* get delta between receive time and PPS time */
	pp_stamp = tspec_stamp_to_lfp(timeout);
	pp_delta = *rd_stamp;
	pp_delta -= pp_stamp;
	delta = lfptod(pp_delta);
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
	if (sys_vars.sys_leap == LEAP_NOTINSYNC)
		return PPS_RELATE_EDGE; /* cannot PLL with pps code */

	/* check against reftime if PPS PLL can be used */
	pp_delta = *reftime;
	pp_delta-= pp_stamp;
	delta = lfptod(pp_delta);
	delta += pp_fudge;
	if (fabs(delta) > 0.45)
		return PPS_RELATE_EDGE; /* cannot PLL with PPS code */

	/* all checks passed, gets an AAA rating here! */
	return PPS_RELATE_PHASE; /* can PLL with PPS code */
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
 * + it checks whether a sentence is known and to be used
 * + it parses the time and date data from the NMEA data string and
 *   augments the missing bits. (century in dat, whole date, ...)
 * + it rejects data that is not from the first accepted sentence in a
 *   burst
 * + it eventually replaces the receive time with the PPS edge time.
 * + it feeds the data to the internal processing stages.
 * -------------------------------------------------------------------
 */
static void
nmea_receive(
	struct recvbuf * rbufp
	)
{
	/* declare & init control structure ptrs */
	struct peer	    * const peer = rbufp->recv_peer;
	struct refclockproc * const pp = peer->procptr;
	nmea_unit	    * const up = (nmea_unit*)pp->unitptr;

	/* Use these variables to hold data until we decide its worth keeping */
	nmea_data rdata;
	char 	  rd_lastcode[BMAX];
	l_fp 	  rd_timestamp, rd_reftime;
	int	  rd_lencode;
	double	  rd_fudge;

	/* working stuff */
	struct timespec date;	/* to keep & convert the time stamp */
	/* results of sentence/date/time parsing */
	uint8_t		sentence;	/* sentence tag */
	int		checkres;
	char *		cp;
	bool		rc_date;
	bool		rc_time;

	/* make sure data has defined pristine state */
	ZERO(date);
	sentence = 0;
	rc_date = false;
	rc_time = false;
	/*
	 * Read the timecode and timestamp, then initialise field
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
		DPRINT(1, ("%s invalid data: '%s'\n",
			   refclock_name(peer), rd_lastcode));
		refclock_report(peer, CEVNT_BADREPLY);
		return;

	case CHECK_EMPTY:
		return;

	default:
		DPRINT(1, ("%s gpsread: %d '%s'\n",
			   refclock_name(peer), rd_lencode,
			   rd_lastcode));
		break;
	}
	up->tally.total++;

	/*
	 * --> below this point we have a valid NMEA sentence <--
	 *
	 * Check sentence name. Skip first 2 chars (talker ID) in most
	 * cases, to allow for $GLGGA and $GPGGA etc. Since the name
	 * field has at least 5 chars we can simply shift the field
	 * start.
	 */
	cp = field_parse(&rdata, 0);
	if      (strncmp(cp + 2, "RMC,", 4) == 0) {
		sentence = NMEA_GPRMC;
	} else if (strncmp(cp + 2, "GGA,", 4) == 0) {
		sentence = NMEA_GPGGA;
	} else if (strncmp(cp + 2, "GLL,", 4) == 0) {
		sentence = NMEA_GPGLL;
	} else if (strncmp(cp + 2, "ZDA,", 4) == 0) {
		sentence = NMEA_GPZDA;
	} else if (strncmp(cp + 2, "ZDG,", 4) == 0) {
		sentence = NMEA_GPZDG;
	} else if (strncmp(cp,   "PGRMF,", 6) == 0) {
		sentence = NMEA_PGRMF;
	} else {
		return;	/* not something we know about */
	}

	/* Eventually output delay measurement now. */
	if (peer->cfg.mode & NMEA_DELAYMEAS_MASK) {
		mprintf_clock_stats(peer, "delay %0.6f %.*s",
			 ldexp(lfpfrac(rd_timestamp), -32),
			 (int)(strchr(rd_lastcode, ',') - rd_lastcode),
			 rd_lastcode);
	}

	/* See if I want to process this message type */
	if ((peer->cfg.mode & NMEA_MESSAGE_MASK) &&
	    !(peer->cfg.mode & sentence_mode[sentence])) {
		up->tally.filtered++;
		return;
	}

	/*
	 * make sure it came in clean
	 *
	 * Apparently, older NMEA specifications (which are expensive)
	 * did not require the checksum for all sentences.  $GPRMC is
	 * the only one so far identified which has always been required
	 * to include a checksum.
	 *
	 * Today, most NMEA GPS receivers checksum every sentence.  To
	 * preserve its error-detection capabilities with modern GPSes
	 * while allowing operation without checksums on all but $GPRMC,
	 * we keep track of whether we've ever seen a valid checksum on
	 * a given sentence, and if so, reject future instances without
	 * checksum.  ('up->cksum_type[NMEA_GPRMC]' is set in
	 * 'nmea_start()' to enforce checksums for $GPRMC right from the
	 * start.)
	 */
	if (up->cksum_type[sentence] <= (uint8_t)checkres) {
		up->cksum_type[sentence] = (uint8_t)checkres;
	} else {
		DPRINT(1, ("%s checksum missing: '%s'\n",
			   refclock_name(peer), rd_lastcode));
		refclock_report(peer, CEVNT_BADREPLY);
		up->tally.malformed++;
		return;
	}

	/*
	 * $GPZDG provides GPS time not UTC, and the two mix poorly.
	 * Once have processed a $GPZDG, do not process any further UTC
	 * sentences (all but $GPZDG currently).
	 */
	if (up->gps_time && NMEA_GPZDG != sentence) {
		up->tally.filtered++;
		return;
	}

	DPRINT(1, ("%s processing %d bytes, timecode '%s'\n",
		   refclock_name(peer), rd_lencode, rd_lastcode));

	/*
	 * Grab fields depending on clock string type and possibly wipe
	 * sensitive data from the last timecode.
	 */
	switch (sentence) {

	case NMEA_GPRMC:
		/* Check quality byte, fetch data & time */
		rc_time	 = parse_time(&date, &rdata, 1);
		pp->leap = parse_qual(&rdata, 2, 'A', 0);
		rc_date	 = parse_date(&date, &rdata, 9, DATE_1_DDMMYY);
		fix_WNRO(&date, &up->wnro, peer);
		if (CLK_FLAG4 & pp->sloppyclockflag)
			field_wipe(&rdata, 3, 4, 5, 6, -1);
		break;

	case NMEA_GPGGA:
		/* Check quality byte, fetch time only */
		rc_time	 = parse_time(&date, &rdata, 1);
		pp->leap = parse_qual(&rdata, 6, '0', 1);
		rc_date	 = kludge_day(&date);
		if (CLK_FLAG4 & pp->sloppyclockflag)
			field_wipe(&rdata, 2, 4, -1);
		break;

	case NMEA_GPGLL:
		/* Check quality byte, fetch time only */
		rc_time	 = parse_time(&date, &rdata, 5);
		pp->leap = parse_qual(&rdata, 6, 'A', 0);
		rc_date	 = kludge_day(&date);
		if (CLK_FLAG4 & pp->sloppyclockflag)
			field_wipe(&rdata, 1, 3, -1);
		break;

	case NMEA_GPZDA:
		/* No quality.	Assume best, fetch time & full date */
		pp->leap = LEAP_NOWARNING;
		rc_time	 = parse_time(&date, &rdata, 1);
		rc_date	 = parse_date(&date, &rdata, 2, DATE_3_DDMMYYYY);
		fix_WNRO(&date, &up->wnro, peer);
		break;

	case NMEA_GPZDG:
		/* Check quality byte, fetch time & full date */
		rc_time	 = parse_time(&date, &rdata, 1);
		rc_date	 = parse_date(&date, &rdata, 2, DATE_3_DDMMYYYY);
		fix_WNRO(&date, &up->wnro, peer);
		pp->leap = parse_qual(&rdata, 4, '0', 1);
		date.tv_sec -= 1; /* GPZDG is following second */
		break;

	case NMEA_PGRMF:
		/* Ignore week stuff.  Use date and time fields. 
		 */
		rc_time  = parse_time(&date, &rdata, 4);
		rc_date  = parse_date(&date, &rdata, 3, DATE_1_DDMMYY);
		pp->leap = parse_qual(&rdata, 11, '0', 1);
		if (CLK_FLAG4 & pp->sloppyclockflag)
			field_wipe(&rdata, 6, 8, -1);
		break;

	default:
		INVARIANT(0);	/* Coverity 97123 */
		return;
	}

	/* Check sanity of time-of-day. */
	if (!rc_time) {	/* no time or conversion error? */
		checkres = CEVNT_BADTIME;
		up->tally.malformed++;
	}
	/* Check sanity of date. */
	else if (!rc_date) {/* no date or conversion error? */
		checkres = CEVNT_BADDATE;
		up->tally.malformed++;
	}
	/* check clock sanity; [bug 2143] */
	else if (pp->leap == LEAP_NOTINSYNC) { /* no good status? */
		checkres = CEVNT_BADREPLY;
		up->tally.rejected++;
	}
	else
		checkres = -1;

	if (checkres != -1) {
		save_ltc(pp, rd_lastcode, (size_t)rd_lencode);
		refclock_report(peer, checkres);
		return;
	}

#ifdef DEBUG
	{
	char temp[100];
	DPRINT(1, ("%s effective timecode: %s",
		   refclock_name(peer), ctime_r(&date.tv_sec, temp)));
	}
#endif

	/* Check if we must enter GPS time mode; log so if we do */
	if (!up->gps_time && (sentence == NMEA_GPZDG)) {
		msyslog(LOG_INFO,
                        "REFCLOCK: %s using GPS time as if it were UTC",
			refclock_name(peer));
		up->gps_time = true;
	}

	/*
	 * Get the reference time stamp from the calendar buffer.
	 * Process the new sample in the median filter and determine the
	 * timecode timestamp, but only if the PPS is not in control.
	 * Discard sentence if reference time did not change.
	 */
	rd_reftime = tspec_stamp_to_lfp(date);
	if (up->last_reftime == rd_reftime) {
		/* Do not touch pp->a_lastcode on purpose! */
		up->tally.filtered++;
		return;
	}
	up->last_reftime = rd_reftime;
	rd_fudge = pp->fudgetime2;

	DPRINT(1, ("%s using '%s'\n",
		   refclock_name(peer), rd_lastcode));

	/* Data will be accepted. Update stats & log data. */
	up->tally.accepted++;
	save_ltc(pp, rd_lastcode, (size_t)rd_lencode);
	pp->lastrec = rd_timestamp;

#ifdef HAVE_PPSAPI
	/*
	 * If we have PPS running, we try to associate the sentence
	 * with the last active edge of the PPS signal.
	 */
	if (up->ppsapi_lit)
		switch (refclock_ppsrelate(
				pp, &up->ppsctl, &rd_reftime, &rd_timestamp,
				pp->fudgetime1,	&rd_fudge))
		{
		case PPS_RELATE_PHASE:
			up->ppsapi_gate = true;
			peer->precision = PPS_PRECISION;
			peer->cfg.flags |= FLAG_PPS;
			DPRINT(2, ("%s PPS_RELATE_PHASE\n",
				   refclock_name(peer)));
			up->tally.pps_used++;
			break;

		case PPS_RELATE_EDGE:
			up->ppsapi_gate = true;
			peer->precision = PPS_PRECISION;
			DPRINT(2, ("%s PPS_RELATE_EDGE\n",
				   refclock_name(peer)));
			break;

		case PPS_RELATE_NONE:
		default:
			/*
			 * Resetting precision and PPS flag is done in
			 * 'nmea_poll', since it might be a glitch. But
			 * at the end of the poll cycle we know...
			 */
			DPRINT(2, ("%s PPS_RELATE_NONE\n",
				   refclock_name(peer)));
			break;
		}
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
	int           unit,
	struct peer * peer
	)
{
	struct refclockproc * const pp = peer->procptr;
	nmea_unit	    * const up = (nmea_unit *)pp->unitptr;

	UNUSED_ARG(unit);

	/*
	 * Process median filter samples. If none received, declare a
	 * timeout and keep going.
	 */
#ifdef HAVE_PPSAPI
	/*
	 * If we don't have PPS pulses and time stamps, turn PPS down
	 * for now.
	 */
	if (!up->ppsapi_gate) {
		if ( FLAG_PPS & peer->cfg.flags ) {
                    /*
                     * PPS just turned off, reset jitter to prevent
                     * thinking the NMEA is PPS precise and thus
                     * being wrongly seelcted when better time is
                     * available elsewhere
                     */
                    peer->jitter = LOGTOD(PRECISION);
                }
		peer->cfg.flags &= ~FLAG_PPS;
		peer->precision = PRECISION;
	} else {
		up->ppsapi_gate = false;
	}
#endif /* HAVE_PPSAPI */

	/*
	 * If the median filter is empty, claim a timeout. Else process
	 * the input data and keep the stats going.
	 */
	if (pp->coderecv == pp->codeproc) {
		refclock_report(peer, CEVNT_TIMEOUT);
                /* reset the jitter, to avoid bad time on recovery */
		peer->jitter = LOGTOD(PRECISION);
	} else {
		pp->polls++;
		pp->lastref = pp->lastrec;
		refclock_receive(peer);
	}

	/*
	 * If extended logging is required, write the tally stats to the
	 * clockstats file; otherwise just do a normal clock stats
	 * record. Clear the tally stats anyway.
	*/
	if (peer->cfg.mode & NMEA_EXTLOG_MASK) {
		/* Log & reset counters with extended logging */
		const char *nmea = pp->a_lastcode;
		if (*nmea == '\0') {
			nmea = "(none)";
		}
		mprintf_clock_stats(
		  peer, "%s  %u %u %u %u %u %u",
		  nmea,
		  up->tally.total, up->tally.accepted,
		  up->tally.rejected, up->tally.malformed,
		  up->tally.filtered, up->tally.pps_used);
	} else {
		record_clock_stats(peer, pp->a_lastcode);
	}
	ZERO(up->tally);
}

/*
 * -------------------------------------------------------------------
 * Save the last timecode string, making sure it's properly truncated
 * if necessary and NUL terminated in any case.
 */
static void
save_ltc(
	struct refclockproc * const pp,
	const char * const          tc,
	size_t                      len
	)
{
	if (len >= sizeof(pp->a_lastcode)) {
		len = sizeof(pp->a_lastcode) - 1;
	}
	pp->lencode = (unsigned short)len;
	memcpy(pp->a_lastcode, tc, len);
	pp->a_lastcode[len] = '\0';
}


#if NMEA_WRITE_SUPPORT
/*
 * -------------------------------------------------------------------
 *  gps_send(fd, cmd, peer)	Sends a command to the GPS receiver.
 *   as in gps_send(fd, "rqts,u", peer);
 *
 * If 'cmd' starts with a '$' it is assumed that this command is in raw
 * format, that is, starts with '$', ends with '<cr><lf>' and that any
 * checksum is correctly provided; the command will be send 'as is' in
 * that case. Otherwise the function will create the necessary frame
 * (start char, chksum, final CRLF) on the fly.
 *
 * We don't currently send any data, but would like to send RTCM SC104
 * messages for differential positioning. It should also give us better
 * time. Without a PPS output, we're Just fooling ourselves because of
 * the serial code paths
 * -------------------------------------------------------------------
 */
static void
gps_send(
	int           fd,
	const char  * cmd,
	struct peer * peer
	)
{
	/* $...*xy<CR><LF><NUL> add 7 */
	char	      buf[NMEA_PROTO_MAXLEN + 7];
	int	      len;
	uint8_t	      dcs;
	const uint8_t *beg, *end;

	if (*cmd != '$') {
		/* get checksum and length */
		beg = end = (const uint8_t*)cmd;
		dcs = 0;
		while (*end >= ' ' && *end != '*')
			dcs ^= *end++;
		len = end - beg;
		/* format into output buffer with overflow check */
		len = snprintf(buf, sizeof(buf), "$%.*s*%02X\r\n",
			       len, beg, dcs);
		if ( ( 0 > len) || ((size_t)len >= sizeof(buf))) {
			DPRINT(1,
                            ("%s gps_send: buffer overflow for command '%s'\n",
			    refclock_name(peer), cmd));
			return;	/* game over player 1 */
		}
		cmd = buf;
	} else {
		len = strlen(cmd);
	}

	DPRINT(1, ("%s gps_send: '%.*s'\n", refclock_name(peer),
		   len - 2, cmd));

	/* send out the whole stuff */
	if (write(fd, cmd, len) == -1)
		refclock_report(peer, CEVNT_FAULT);
}
#endif /* NMEA_WRITE_SUPPORT */

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
	nmea_data * data,	/* context structure		       */
	char 	  * cptr,	/* start of raw data		       */
	int	    dlen	/* data len, not counting trailing NUL */
	)
{
	uint8_t cs_l;	/* checksum local computed	*/
	uint8_t cs_r;	/* checksum remote given	*/
	char * eptr;	/* buffer end end pointer	*/
	char   tmp;	/* char buffer 			*/

	cs_l = 0;
	cs_r = 0;
	/* some basic input constraints */
	if (dlen < 0) {
		dlen = 0;
	}
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
	if (*cptr == '\0') {
		return CHECK_EMPTY;
	}
	if (*cptr++ != '$') {
		return CHECK_INVALID;
	}

	/* -*- advance context beyond start character */
	data->base++;
	data->cptr++;
	data->blen--;

	/* -*- field name: '[A-Z][A-Z0-9]{4,},' */
	if (*cptr < 'A' || *cptr > 'Z') {
		return CHECK_INVALID;
	}
	cs_l ^= *cptr++;
	while ((*cptr >= 'A' && *cptr <= 'Z') ||
	       (*cptr >= '0' && *cptr <= '9')  )
		cs_l ^= *cptr++;
	if (*cptr != ',' || (cptr - data->base) < NMEA_PROTO_IDLEN) {
		return CHECK_INVALID;
}
	cs_l ^= *cptr++;

	/* -*- data: '[^*]*' */
	while (*cptr && *cptr != '*')
		cs_l ^= *cptr++;

	/* -*- checksum field: (\*[0-9A-F]{2})?$ */
	if (*cptr == '\0') {
		return CHECK_VALID;
}
	if (*cptr != '*' || cptr != eptr - 3 ||
	    (cptr - data->base) >= NMEA_PROTO_MAXLEN) {
		return CHECK_INVALID;
}

	for (cptr++; (tmp = *cptr) != '\0'; cptr++) {
		if (tmp >= '0' && tmp <= '9') {
			cs_r = (cs_r << 4) + (tmp - '0');
		} else if (tmp >= 'A' && tmp <= 'F') {
			cs_r = (cs_r << 4) + (tmp - 'A' + 10);
		} else {
			break;
}
	}

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
	nmea_data * data,
	int 	    fn
	)
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
 * in a varargs list, preferably in ascending order, in any case
 * terminated by a negative field index.
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
	nmea_data * data,
	...
	)
{
	va_list	va;		/* vararg index list */
	int	fcnt;		/* safeguard against runaway arglist */
	int	fidx;		/* field to nuke, or -1 for checksum */
	char  * cp;		/* overwrite destination */

	fcnt = 8;
	cp = NULL;
	va_start(va, data);
	do {
		fidx = va_arg(va, int);
		if (fidx >= 0 && fidx <= NMEA_PROTO_FIELDS) {
			cp = field_parse(data, fidx);
		} else {
			cp = data->base + data->blen;
			if (data->blen >= 3 && cp[-3] == '*') {
				cp -= 2;
			}
		}
		for ( ; '\0' != *cp && '*' != *cp && ',' != *cp; cp++) {
			if ('.' != *cp) {
				*cp = '_';
			}
		}
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
 * data field (*cp points to the NUL byte) the result is LEAP_NOTINSYNC.
 * -------------------------------------------------------------------
 */
static uint8_t
parse_qual(
	nmea_data * rd,
	int         idx,
	char        tag,
	int         inv
	)
{
	static const uint8_t table[2] =
				{ LEAP_NOTINSYNC, LEAP_NOWARNING };
	char * dp;

	dp = field_parse(rd, idx);

	return table[ *dp && ((*dp == tag) == !inv) ];
}

/*
 * -------------------------------------------------------------------
 * Parse a time stamp in HHMMSS[.sss] format with error checking.
 *
 * Add result to arg
 *
 * returns true on success, false on failure
 * -------------------------------------------------------------------
 */
static bool
parse_time(
	struct timespec * dt,	/* result date+time */
	nmea_data       * rd,
	int		  idx
	)
{
	static const unsigned long weight[4] = {
		0, 100000000, 10000000, 1000000
	};

	int	rc;
	unsigned int	h;
	unsigned int	m;
	unsigned int	s;
	int	p1;
	int	p2;
	unsigned long	f;
	char  * dp;

	dp = field_parse(rd, idx);
	rc = sscanf(dp, "%2u%2u%2u%n.%3lu%n", &h, &m, &s, &p1, &f, &p2);
	if (rc < 3 || p1 != 6) {
		DPRINT(1, ("nmea: invalid time code: '%.6s'\n", dp));
		return false;
	}

	/* value sanity check */
	if (h > 23 || m > 59 || s > 60) {
		DPRINT(1, ("nmea: invalid time spec %02u:%02u:%02u\n",
			   h, m, s));
		return false;
	}

	dt->tv_sec += h*3600 + m*60 + s;
	/* if we have a fraction, scale it up to nanoseconds. */
	if (rc == 4) {
		dt->tv_nsec += (f * weight[p2 - p1 - 1]);
}

	return true;
}

/*
 * -------------------------------------------------------------------
 * Parse a date string from an NMEA sentence. This could either be a
 * partial date in DDMMYY format in one field, or DD,MM,YYYY full date
 * spec spanning three fields. This function does some extensive error
 * checking to make sure the date string was consistent.
 *
 * Add result to arg
 *
 * returns true on success, false on failure
 * -------------------------------------------------------------------
 */
static bool
parse_date(
	struct timespec * dt,	/* result pointer */
	nmea_data       * rd,
	int		  idx,
	enum date_fmt	  fmt
	)
{
	int		rc;
	unsigned int	y;
	unsigned int	m;
	unsigned int	d;
	int		p;
	char  	      * dp;
	struct tm	tm;

	dp = field_parse(rd, idx);
	switch (fmt) {

	case DATE_1_DDMMYY:
		rc = sscanf(dp, "%2u%2u%2u%n", &d, &m, &y, &p);
		if (rc != 3 || p != 6) {
			DPRINT(1, ("nmea: invalid date code: '%.6s'\n",
				   dp));
			return false;
		}
		if (y > 80) {
			/* We know the time is now > 2000 but
			 * GPS might be off by n*1024 weeks.
			 * WNRO: Week Number Roll Over, ~20 years
			 * Don't break that correction.
			 *
			 * GPS time started in 1980
			 *   anything < 80 is from the next century.
			 */
			y += 1900;
		} else {
			y += 2000;
		}
		break;

	case DATE_3_DDMMYYYY:
		rc = sscanf(dp, "%2u,%2u,%4u%n", &d, &m, &y, &p);
		if (rc != 3 || p != 10) {
			DPRINT(1, ("nmea: invalid date code: '%.10s'\n",
				   dp));
			return false;
		}
		break;

	default:
		DPRINT(1, ("nmea: invalid parse format: %u\n", fmt));
		return false;
	}

	/* value sanity check */
	if (d < 1 || d > 31 || m < 1 || m > 12) {
		DPRINT(1, ("nmea: invalid date spec (YMD) %04u:%02u:%02u\n",
			   y, m, d));
		return false;
	}

	/* timegm is non-Posix. */
	ZERO(tm);
	tm.tm_year = y-1900;
	tm.tm_mon = m-1;
	tm.tm_mday = d;
	dt->tv_sec += timegm(&tm);	/* No error checking */

	return true;
}

/* Use system time for missing date field.
 * Time from GPS is in dt.  We add a day offset.
 * This assumes the system time is within 12 hours.
 * Note the 2 interesting cases:
 *   If GPS time is late in the day and we are early in the day
 *   we are actually early in the following day.
 *   If GPS time is early in the day and we are late in the day
 *   we are actually late in the previous day.
 *
 * This code hasn't been tested yet.
 *
 */
static bool kludge_day (struct timespec *dt) {
  struct timespec now = {0, 0};

  clock_gettime(CLOCK_REALTIME, &now);
  int nowday = now.tv_sec / 86400;
  int nowsec = now.tv_sec % 86400;
  int gpssec = (int)dt->tv_sec;
  if ((gpssec-nowsec) > 12*3600) nowday -= 1;
  if ((nowsec-gpssec) > 12*3600) nowday += 1;
  dt->tv_sec += nowday*86400;

  if (LEAP_NOTINSYNC == sys_vars.sys_leap) {
    return false;
  }
  return true;
}

// end
