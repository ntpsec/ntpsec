/*
 * refclock_gpsdjson.c - clock driver as GPSD JSON client
 *	Juergen Perlinger (perlinger@ntp.org)
 *	Feb 11, 2014 for the NTP project.
 *      The contents of 'html/copyright.html' apply.
 *
 *	Heavily inspired by refclock_nmea.c
 *
 * Note: This will currently NOT work with Windows due to some
 * limitations:
 *
 *  - There is no GPSD for Windows. (There is an unofficial port to
 *    cygwin, but Windows is not officially supported.)
 *
 *  - To work properly, this driver needs PPS and TPV sentences from
 *    GPSD. I don't see how the cygwin port should deal with that.
 *
 *  - The device name matching must be done in a different way for
 *    Windows. (Can be done with COMxx matching, as done for NMEA.)
 *
 * Apart from those minor hickups, once GPSD has been fully ported to
 * Windows, there's no reason why this should not work there ;-)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ntp_types.h"

#if defined(REFCLOCK) && defined(CLOCK_GPSDJSON) && !defined(SYS_WINNT) 

/* =====================================================================
 * get the little JSMN library directly into our guts
 */
#include "../libjsmn/jsmn.c"

/* =====================================================================
 * header stuff we need
 */

#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#if defined(HAVE_SYS_POLL_H)
# include <sys/poll.h>
#elif defined(HAVE_SYS_SLECET_H)
# include <sys/select.h>
#else
# error need poll() or select()
#endif

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_unixtime.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"
#include "ntp_calendar.h"

#define	PRECISION	(-9)	/* precision assumed (about 2 ms) */
#define	PPS_PRECISION	(-20)	/* precision assumed (about 1 us) */
#define	REFID		"GPSD"	/* reference id */
#define	DESCRIPTION	"GPSD JSON client clock" /* who we are */

#define MAX_PDU_LEN	1600
#define TICKOVER_MAX	10

#ifndef BOOL
# define BOOL int
#endif
#ifndef TRUE
# define TRUE 1
#endif
#ifndef FALSE
# define FALSE 0
#endif

/* some local typedefs : The NTPD formatting style cries for short type
 * names, and we provide them locally. Note:the suffix '_t' is reserved
 * for the standard; I use a capital T instead.
 */
typedef struct peer         peerT;
typedef struct refclockproc clockprocT;
typedef struct addrinfo     addrinfoT;

/* =====================================================================
 * We use the same device name scheme as does the NMEA driver; since
 * GPSD supports the same links, we can select devices by a fixed name.
 */
static const char * s_dev_stem = "/dev/gps";

/* =====================================================================
 * forward declarations for transfer vector and the vector itself
 */

static	void	gpsd_init	(void);
static	int	gpsd_start	(int, peerT *);
static	void	gpsd_shutdown	(int, peerT *);
static	void	gpsd_receive	(struct recvbuf *);
static	void	gpsd_poll	(int, peerT *);
static	void	gpsd_control	(int, const struct refclockstat *,
				 struct refclockstat *, peerT *);
static	void	gpsd_timer	(int, peerT *);

static  int     myasprintf(char**, char const*, ...);

struct refclock refclock_gpsdjson = {
	gpsd_start,		/* start up driver */
	gpsd_shutdown,		/* shut down driver */
	gpsd_poll,		/* transmit poll message */
	gpsd_control,		/* fudge control */
	gpsd_init,		/* initialize driver */
	noentry,		/* buginfo */
	gpsd_timer		/* called once per second */
};

/* =====================================================================
 * our local clock unit and data
 */
typedef struct gpsd_unit {
	/* current line protocol version */
	uint16_t proto_major;
	uint16_t proto_minor;

	/* PPS time stamps */
	l_fp pps_stamp;	/* effective PPS time stamp */
	l_fp pps_recvt;	/* when we received the PPS message */

	/* TPV (GPS data) time stamps */
	l_fp tpv_stamp;	/* effective GPS time stamp */
	l_fp tpv_recvt;	/* when we received the TPV message */

	/* Flags to indicate available data */
	int fl_tpv : 1;	/* valid TPV seen (have time) */
	int fl_pps : 1;	/* valid pulse seen */
	int fl_ver : 1;	/* have protocol version */

	/* clock status reporting */
	int last_event;
	int next_event;

	/* admin stuff for sockets and device selection */
	int         fdt;	/* current connecting socket */
	addrinfoT * addr;	/* next address to try */
	int         tickover;	/* timeout countdown */
	char      * device;	/* device name of unit */

	/* record assemby buffer and saved length */
	int  buflen;
	char buffer[MAX_PDU_LEN];
} gpsd_unitT;

/* =====================================================================
 * static local helpers forward decls
 */
static void gpsd_init_socket(peerT * const peer);
static void gpsd_test_socket(peerT * const peer);
static void gpsd_stop_socket(peerT * const peer);

static void gpsd_parse(peerT * const peer,
		       const l_fp  * const rtime);
static BOOL convert_time(l_fp * fp, const char * gps_time);
static void save_ltc(clockprocT * const pp, const char * const tc);

/* =====================================================================
 * local / static stuff
 */

/* The logon string is actually the ?WATCH command of GPSD, using JSON
 * data and selecting the GPS device name we created from our unit
 * number. [Note: This is a format string!]
 */
static const char * s_logon =
    "?WATCH={\"enable\":true,\"json\":true,\"device\":\"%s\"};\r\n";

/* We keep a static list of network addresses for 'localhost:gpsd', and
 * we try to connect to them in round-robin fashion.
 */
static addrinfoT * s_gpsd_addr;

/* =====================================================================
 * the clock functions
 */

/* ---------------------------------------------------------------------
 * Init: This currently just gets the socket address for the GPS daemon
 */
static void
gpsd_init(void)
{
	addrinfoT hints;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_UNSPEC;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;

	/* just take the first configured address of localhost... */
	if (getaddrinfo("localhost", "gpsd", &hints, &s_gpsd_addr))
		s_gpsd_addr = NULL;
}

/* ---------------------------------------------------------------------
 * Start: allocate a unit pointer and set up the runtime data
 */

static int
gpsd_start(
	int     unit,
	peerT * peer)
{
	clockprocT * const pp = peer->procptr;
	gpsd_unitT * const up = emalloc_zero(sizeof(*up));

	struct stat sb;

	/* initialize the unit structure */
	up->fdt     = -1;
	up->addr    = s_gpsd_addr;
	up->last_event = CEVNT_NOMINAL;
	up->next_event = CEVNT_TIMEOUT;

	/* setup refclock processing */
	pp->unitptr = (caddr_t)up;
	pp->io.fd   = -1;
	pp->io.clock_recv = gpsd_receive;
	pp->io.srcclock   = peer;
	pp->io.datalen    = 0;
	pp->a_lastcode[0] = '\0';
	pp->lencode       = 0;
	pp->clockdesc     = DESCRIPTION;
	memcpy(&pp->refid, REFID, 4);

	/* Initialize miscellaneous variables */
	peer->precision = PRECISION;

	/* Create the device name and check for a Character Device. It's
	 * assumed that GPSD was started with the same link, so the
	 * names match. (If this is not practicable, we will have to
	 * read the symlink, if any, so we can get the true device
	 * file.)
	 */
	if (-1 == myasprintf(&up->device, "%s%u", s_dev_stem, unit)) {
	    msyslog(LOG_ERR, "%s clock device name too long",
		    refnumtoa(&peer->srcadr));
	    goto dev_fail;
	}
	if (-1 == stat(up->device, &sb) || !S_ISCHR(sb.st_mode)) {
		msyslog(LOG_ERR, "%s: '%s' is not a character device",
			refnumtoa(&peer->srcadr), up->device);
	    goto dev_fail;
	}

	LOGIF(CLOCKINFO,
	      (LOG_NOTICE, "%s: startup, device is '%s'",
	       refnumtoa(&peer->srcadr), up->device));
	return TRUE;

dev_fail:
	/* On failure, remove all UNIT ressources and declare defeat. */
	if (up) {
	    free(up->device);
	    free(up);
	}
	pp->unitptr = (caddr_t)NULL;
	return FALSE;
}

/* ------------------------------------------------------------------ */

static void
gpsd_shutdown(
	int     unit,
	peerT * peer)
{
	clockprocT * const pp = peer->procptr;
	gpsd_unitT * const up = (gpsd_unitT *)pp->unitptr;

	UNUSED_ARG(unit);

	if (up) {
	    free(up->device);
	    free(up);
	}
	pp->unitptr = (caddr_t)NULL;
	if (-1 != pp->io.fd)
		io_closeclock(&pp->io);
	pp->io.fd = -1;
	LOGIF(CLOCKINFO,
	      (LOG_NOTICE, "%s: shutdown", refnumtoa(&peer->srcadr)));
}

/* ------------------------------------------------------------------ */

static void
gpsd_receive(
	struct recvbuf * rbufp)
{
	/* declare & init control structure ptrs */
	peerT	   * const peer = rbufp->recv_peer;
	clockprocT * const pp   = peer->procptr;
	gpsd_unitT * const up   = (gpsd_unitT *)pp->unitptr;	

	const char *psrc, *esrc;
	char       *pdst, *edst, ch;

	/* Since we're getting a raw stream data, we must assemble lines
	 * in our receive buffer. We can't use neither 'refclock_gtraw'
	 * not 'refclock_gtlin' here...  We process chars until we reach
	 * an EoL (that is, line feed) but we truncate the message if it
	 * does not fit the buffer.  GPSD might truncate messages, too,
	 * so dealing with truncated buffers is necessary anyway.
	 */
	psrc = (const char*)rbufp->recv_buffer;
	esrc = psrc + rbufp->recv_length;

	pdst = up->buffer + up->buflen;
	edst = pdst + sizeof(up->buffer) - 1; /* for trailing NUL */

	while (psrc != esrc) {
		ch = *psrc++;
		if (ch == '\n') {
			/* trim trailing whitespace & terminate buffer */
			while (pdst != up->buffer && pdst[-1] <= ' ')
				--pdst;
			*pdst = '\0';
			/* process data and reset buffer */
			gpsd_parse(peer, &rbufp->recv_time);
			pdst = up->buffer;
		} else if (pdst != edst) {
			/* add next char, ignoring leading whitespace */
			if (ch > ' ' || pdst != up->buffer)
				*pdst++ = ch;
		}
	}
	up->buflen   = pdst - up->buffer;
	up->tickover = TICKOVER_MAX;
}

/* ------------------------------------------------------------------ */

static void
gpsd_poll(
	int     unit,
	peerT * peer)
{
	clockprocT * const pp = peer->procptr;
	gpsd_unitT * const up = (gpsd_unitT *)pp->unitptr;

	/* Check what state we have to report to the core */
	if (-1 == pp->io.fd) {
		/* not connected to GPSD: clearly not working! */
		up->next_event = CEVNT_FAULT;
	} else if (pp->coderecv == pp->codeproc) {
		/* no data: this is a least a timeout. */
		if (CEVNT_NOMINAL == up->next_event)
			up->next_event = CEVNT_NOMINAL;
	} else {
		/* presume all-thumbs-up if we have data */
		up->next_event = CEVNT_NOMINAL;
	}

	/* In case we're not good, remove the PPS flag and get us to our
	 * low default precision.
	*/
	if (CEVNT_NOMINAL != up->next_event) {
		peer->flags    &= ~FLAG_PPS;
		peer->precision = PRECISION;
	}

	/* eventually update the clock state. */
	if (up->last_event != up->next_event)
		refclock_report(peer, up->next_event);
	up->last_event = up->next_event;

	/* If there is data, feed it to the core and keep the stats
	 * going...
	 */
	if (pp->coderecv != pp->codeproc) {
		pp->polls++;
		pp->lastref = pp->lastrec;
		refclock_receive(peer);
	}
	record_clock_stats(&peer->srcadr, pp->a_lastcode);
}

/* ------------------------------------------------------------------ */

static void
gpsd_control(
	int                         unit,
	const struct refclockstat * in_st,
	struct refclockstat       * out_st,
	peerT                     * peer  )
{
	/* Not yet implemented -- do we need it? */
}

/* ------------------------------------------------------------------ */

static void
gpsd_timer(
	int     unit,
	peerT * peer)
{
	clockprocT * const pp = peer->procptr;
	gpsd_unitT * const up = (gpsd_unitT *)pp->unitptr;
	
	/* This is used for timeout handling. Nothing that needs
	 * sub-second precison happens here, so receive/connect/retry
	 * timeouts are simply handled by a count down, and then we
	 * decide what to do by the socket values.
	 *
	 * Note that the timer stays at zero here, unless some of the
	 * functions set it to another value.
	 */
	if (up->tickover)
		--up->tickover;

	if (0 == up->tickover) {
		if (-1 != pp->io.fd)
			gpsd_stop_socket(peer);
		else if (-1 != up->fdt)
			gpsd_test_socket(peer);
		else if (NULL != s_gpsd_addr)
			gpsd_init_socket(peer);
	}
}

/* =====================================================================
 * JSON parsing stuff
 */

#define JSMN_MAXTOK	100
#define INVALID_TOKEN (-1)

typedef struct json_ctx {
	char        * buf;
	int           ntok;
	jsmntok_t     tok[JSMN_MAXTOK];
} json_ctx;

typedef int tok_ref;

#ifdef HAVE_LONG_LONG
typedef long long json_int;
 #define JSON_STRING_TO_INT strtoll
#else
typedef long json_int;
 #define JSON_STRING_TO_INT strtol
#endif

/* ------------------------------------------------------------------ */

static tok_ref
json_token_skip(
	const json_ctx * ctx,
	tok_ref          tid)
{
	int len;
	len = ctx->tok[tid].size;
	for (++tid; len; --len)
		if (tid < ctx->ntok)
			tid = json_token_skip(ctx, tid);
		else
			break;
	if (tid > ctx->ntok)
		tid = ctx->ntok;
	return tid;
}
	
/* ------------------------------------------------------------------ */

static int
json_object_lookup(
	const json_ctx * ctx,
	tok_ref          tid,
	const char     * key)
{
	int len;

	if (tid >= ctx->ntok || ctx->tok[tid].type != JSMN_OBJECT)
		return INVALID_TOKEN;
	len = ctx->ntok - tid - 1;
	if (len > ctx->tok[tid].size)
		len = ctx->tok[tid].size;
	for (tid += 1; len > 1; len-=2) {
		if (ctx->tok[tid].type != JSMN_STRING)
			continue; /* hmmm... that's an error, strictly speaking */
		if (!strcmp(key, ctx->buf + ctx->tok[tid].start))
			return tid + 1;
		tid = json_token_skip(ctx, tid + 1);
	}
	return INVALID_TOKEN;
}

/* ------------------------------------------------------------------ */

#if 0 /* currently unused */
static const char*
json_object_lookup_string(
	const json_ctx * ctx,
	tok_ref          tid,
	const char     * key)
{
	tok_ref val_ref;
	val_ref = json_object_lookup(ctx, tid, key);
	if (INVALID_TOKEN == val_ref               ||
	    JSMN_STRING   != ctx->tok[val_ref].type )
		goto cvt_error;
	return ctx->buf + ctx->tok[val_ref].start;

  cvt_error:
	errno = EINVAL;
	return NULL;
}
#endif

static const char*
json_object_lookup_string_default(
	const json_ctx * ctx,
	tok_ref          tid,
	const char     * key,
	const char     * def)
{
	tok_ref val_ref;
	val_ref = json_object_lookup(ctx, tid, key);
	if (INVALID_TOKEN == val_ref               ||
	    JSMN_STRING   != ctx->tok[val_ref].type )
		return def;
	return ctx->buf + ctx->tok[val_ref].start;
}

/* ------------------------------------------------------------------ */

static json_int
json_object_lookup_int(
	const json_ctx * ctx,
	tok_ref          tid,
	const char     * key)
{
	json_int  ret;
	tok_ref   val_ref;
	char    * ep;

	val_ref = json_object_lookup(ctx, tid, key);
	if (INVALID_TOKEN  == val_ref               ||
	    JSMN_PRIMITIVE != ctx->tok[val_ref].type )
		goto cvt_error;
	ret = JSON_STRING_TO_INT(
		ctx->buf + ctx->tok[val_ref].start, &ep, 10);
	if (*ep)
		goto cvt_error;
	return ret;

  cvt_error:
	errno = EINVAL;
	return 0;
}

static json_int
json_object_lookup_int_default(
	const json_ctx * ctx,
	tok_ref          tid,
	const char     * key,
	json_int         def)
{
	json_int  retv;
	int       esave;
	
	esave = errno;
	errno = 0;
	retv  = json_object_lookup_int(ctx, tid, key);
	if (0 != errno)
		retv = def;
	errno = esave;
	return retv;
}

/* ------------------------------------------------------------------ */

static double
json_object_lookup_float(
	const json_ctx * ctx,
	tok_ref          tid,
	const char     * key)
{
	double    ret;
	tok_ref   val_ref;
	char    * ep;

	val_ref = json_object_lookup(ctx, tid, key);
	if (INVALID_TOKEN  == val_ref               ||
	    JSMN_PRIMITIVE != ctx->tok[val_ref].type )
		goto cvt_error;
	ret = strtod(ctx->buf + ctx->tok[val_ref].start, &ep);
	if (*ep)
		goto cvt_error;
	return ret;

  cvt_error:
	errno = EINVAL;
	return 0.0;
}

static double
json_object_lookup_float_default(
	const json_ctx * ctx,
	tok_ref          tid,
	const char     * key,
	double           def)
{
	double    retv;
	int       esave;
	
	esave = errno;
	errno = 0;
	retv  = json_object_lookup_float(ctx, tid, key);
	if (0 != errno)
		retv = def;
	errno = esave;
	return retv;
}

/* ------------------------------------------------------------------ */

static BOOL
json_parse_record(
	json_ctx * ctx,
	char     * buf)
{
	jsmn_parser jsm;
	int         idx, rc;

	jsmn_init(&jsm);
	rc = jsmn_parse(&jsm, buf, ctx->tok, JSMN_MAXTOK);
	ctx->buf  = buf;
	ctx->ntok = jsm.toknext;

	/* Make all tokens NUL terminated by overwriting the
	 * terminator symbol
	 */
	for (idx = 0; idx < jsm.toknext; ++idx)
		if (ctx->tok[idx].end > ctx->tok[idx].start)
			ctx->buf[ctx->tok[idx].end] = '\0';

	if (JSMN_ERROR_PART  != rc &&
	    JSMN_ERROR_NOMEM != rc &&
	    JSMN_SUCCESS     != rc  )
		return FALSE; /* not parseable - bail out */

	if (0 >= jsm.toknext || JSMN_OBJECT != ctx->tok[0].type)
		return FALSE; /* not object or no data!?! */

	return TRUE;
}


/* =====================================================================
 * static local helpers
 */

static void
process_version(
	peerT      * const peer ,
	json_ctx   * const jctx ,
	const l_fp * const rtime)
{
	clockprocT * const pp = peer->procptr;
	gpsd_unitT * const up = (gpsd_unitT *)pp->unitptr;

	int    len;
	char * buf;

	/* get protocol version number */
	errno = 0;
	up->proto_major = (uint16_t)json_object_lookup_int(
		jctx, 0, "proto_major");
	up->proto_minor = (uint16_t)json_object_lookup_int(
		jctx, 0, "proto_minor");
	if (0 == errno) {
		up->fl_ver = -1;
		msyslog(LOG_INFO, "%s: GPSD protocol version %u.%u",
			refnumtoa(&peer->srcadr),
			up->proto_major, up->proto_minor);
	}
	/*TODO: validate protocol version! */
	
	/* request watch for our GPS device
	 * Reuse the input buffer, which is no longer needed in the
	 * current cycle. Also assume that we can write the watch
	 * request in one sweep into the socket; since we do not do
	 * output otherwise, this should always work.  (Unless the
	 * TCP/IP window size gets lower than the length of the
	 * request. We handle that whe it happens.)
	 */
	snprintf(up->buffer, sizeof(up->buffer),
		 s_logon, up->device);
	buf = up->buffer;
	len = strlen(buf);
	if (len != write(pp->io.fd, buf, len)) {
		/*Note: if the server fails to read our request, the
		 * resulting data timeout will take care of the
		 * connection!
		 */
		msyslog(LOG_ERR, "%s: failed to write watch request (%m)",
			refnumtoa(&peer->srcadr));
	}
}

/* ------------------------------------------------------------------ */

static void
process_tpv(
	peerT      * const peer ,
	json_ctx   * const jctx ,
	const l_fp * const rtime)
{
	clockprocT * const pp = peer->procptr;
	gpsd_unitT * const up = (gpsd_unitT *)pp->unitptr;

	const char * gps_time;
	int          gps_mode;
	double       ept, epp, epx, epy, epv;
	int          log2;
	
	gps_mode = (int)json_object_lookup_int_default(
		jctx, 0, "mode", 0);

	gps_time = json_object_lookup_string_default(
		jctx, 0, "time", NULL);

	if (gps_mode < 1 || NULL == gps_time) {
		/* receiver has no fix; tell about and avoid stale data */
		up->next_event = CEVNT_FAULT;
		/*TODO: ?better? up->next_event = CEVNT_BADREPLY; */
		up->fl_tpv = 0;
		up->fl_pps = 0;
		return;
	}

	/* save last time code to clock data */
	save_ltc(pp, gps_time);

	/* convert clock and set resulting ref time */
	if (convert_time(&up->tpv_stamp, gps_time)) {
		DPRINTF(1, (" tpv, stamp='%s', recvt='%s' mode=%u\n",
			    gmprettydate(&up->tpv_stamp),
			    gmprettydate(&up->tpv_recvt),
			    gps_mode));
		
		up->tpv_recvt = *rtime;
		up->fl_tpv = -1;
	} else {
		up->next_event = CEVNT_BADREPLY;
	}
		
	/* Set the precision from the GPSD data
	 *
	 * Since EPT has some issues, we use EPT and a home-brewed error
	 * estimation base on a sphere derived from EPX/Y/V and the
	 * speed of light. Use the better one of those two.
	 */
	ept = json_object_lookup_float_default(jctx, 0, "ept", 1.0);

	epx = json_object_lookup_float_default(jctx, 0, "epx", 1000.0);
	epy = json_object_lookup_float_default(jctx, 0, "epy", 1000.0);
	if (1 == gps_mode) {
		/* 2d-fix: extend bounding rectangle to cuboid */
		epv = max(epx, epy);
	} else {
		/* 3d-fix: get bounding cuboid */
		epv = json_object_lookup_float_default(
				jctx, 0, "epv", 1000.0);
	}

	/* get diameter of enclosing sphere of bounding cuboid as spatial
	 * error, then divide spatial error by speed of light to get
	 * another time error estimate. Add extra 100 meters as
	 * optimistic lower bound. Then use the better one of the two
	 * estimations.
	 */
	epp = 2.0 * sqrt(epx*epx + epy*epy + epv*epv);
	epp = (epp + 100.0) / 299792458.0;

	ept = min(ept, epp  );
	ept = min(ept, 0.5  );
	ept = max(ept, 1.0-9);
	ept = frexp(ept, &log2);

	peer->precision = log2;
}

/* ------------------------------------------------------------------ */

static void
process_pps(
	peerT      * const peer ,
	json_ctx   * const jctx ,
	const l_fp * const rtime)
{
	clockprocT * const pp = peer->procptr;
	gpsd_unitT * const up = (gpsd_unitT *)pp->unitptr;

	uint32_t secs;
	double   frac;
		
	errno = 0;
	secs = (uint32_t)json_object_lookup_int(jctx, 0, "real_sec");
	frac = json_object_lookup_float(jctx, 0, "real_musec");
	if (0 != errno) {
		up->next_event = CEVNT_BADTIME;
		return;
	}

	frac *= 1.0e-6; /* convert usec to seconds */
	M_DTOLFP(frac, up->pps_stamp.l_ui, up->pps_stamp.l_uf);
	up->pps_stamp.l_ui += secs;
	up->pps_stamp.l_ui += JAN_1970;

	DPRINTF(1, (" pps, stamp='%s', recvt='%s'\n", 
		    gmprettydate(&up->pps_stamp),
		    gmprettydate(&up->pps_recvt)));
	
	up->pps_recvt = *rtime;
	pp->lastrec   = up->pps_stamp;

	/* When we have a time pulse, clear the TPV flag: the
	 * PPS is only valid for the >NEXT< TPV value!
	 */
	up->fl_pps = -1;
	up->fl_tpv =  0;
}

/* ------------------------------------------------------------------ */

static void
gpsd_parse(
	peerT      * const peer,
	const l_fp * const rtime)
{
	clockprocT * const pp = peer->procptr;
	gpsd_unitT * const up = (gpsd_unitT *)pp->unitptr;

	json_ctx     jctx;
	const char * clsid;

        DPRINTF(2, ("gpsd_parse: time %s '%s'\n",
                    ulfptoa(rtime, 6), up->buffer));

	/* See if we can grab anything potentially useful */
	if (!json_parse_record(&jctx, up->buffer))
		return;

	/* Now dispatch over the objects we know */
	clsid = json_object_lookup_string_default(
		&jctx, 0, "class", "-bad-repy-");

	if (!strcmp("VERSION", clsid))
		process_version(peer, &jctx, rtime);
	else if (!strcmp("TPV", clsid))
		process_tpv(peer, &jctx, rtime);
	else if (!strcmp("PPS", clsid))
		process_pps(peer, &jctx, rtime);
	else
		return; /* nothing we know about... */

	/* Bail out unless we have a pulse and a time stamp */
	if (!(up->fl_pps && up->fl_tpv))
		return;

	/* Check if the pulse and the time came close enough to be
	 * correlated. Ignore this pair if the difference is more than a
	 * second.
	 */
	L_SUB(&up->tpv_recvt, &up->pps_recvt);
	if (0 == up->tpv_recvt.l_ui) {
		peer->flags |= FLAG_PPS;
		/*TODO: fudge processing */
		refclock_process_offset(pp, up->tpv_stamp, up->pps_stamp,
					pp->fudgetime1);
	} else {
		LOGIF(CLOCKINFO,
		      (LOG_WARNING, "%s: disaarded data due to delay",
		       refnumtoa(&peer->srcadr)));
	}

	/* mark pulse and time as consumed */
	up->fl_pps = 0;
	up->fl_tpv = 0;
}

/* ------------------------------------------------------------------ */

static void
gpsd_stop_socket(
	peerT * const peer)
{
	clockprocT * const pp = peer->procptr;
	gpsd_unitT * const up = (gpsd_unitT *)pp->unitptr;

	if (-1 != pp->io.fd)
		io_closeclock(&pp->io);
	pp->io.fd = -1;
	msyslog(LOG_INFO, "%s: closing socket to GPSD",
		refnumtoa(&peer->srcadr));
	up->tickover = TICKOVER_MAX;
}

/* ------------------------------------------------------------------ */

static void
gpsd_init_socket(
	peerT * const peer)
{
	clockprocT * const pp = peer->procptr;
	gpsd_unitT * const up = (gpsd_unitT *)pp->unitptr;
	addrinfoT  * ai;
	int          rc;

	/* draw next address to try */
	if (NULL == up->addr)
		up->addr = s_gpsd_addr;
	ai = up->addr;
	up->addr = ai->ai_next;

	/* try to create a matching socket */
	up->fdt = socket(
		ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (-1 == up->fdt) {
		msyslog(LOG_ERR, "%s: cannot create GPSD socket: %m",
			refnumtoa(&peer->srcadr));
		goto no_socket;
	}
	
	/* make sure the socket is non-blocking */
	rc = fcntl(up->fdt, F_SETFL, O_NONBLOCK, 1);
	if (-1 == rc) {
		msyslog(LOG_ERR, "%s: cannot set GPSD socket to non-blocking: %m",
			refnumtoa(&peer->srcadr));
		goto no_socket;
	}
	/* start a non-blocking connect */
	rc = connect(up->fdt, ai->ai_addr, ai->ai_addrlen);
	if (-1 == rc && errno != EINPROGRESS) {
		msyslog(LOG_ERR, "%s: cannot connect GPSD socket: %m",
			refnumtoa(&peer->srcadr));
		goto no_socket;
	}

	return;
  
  no_socket:
	if (-1 != up->fdt)
		close(up->fdt);
	up->fdt = -1;
}

/* ------------------------------------------------------------------ */

static void
gpsd_test_socket(
	peerT * const peer)
{
	clockprocT * const pp = peer->procptr;
	gpsd_unitT * const up = (gpsd_unitT *)pp->unitptr;

	int       error;
	int       rc;
	socklen_t erlen;

	/* Check if the non-blocking connect was finished by testing the
	 * socket for writeability. Use the 'poll()' API if available
	 * and 'select()' otherwise.
	 */
#if defined(HAVE_SYS_POLL_H)
	{
		struct pollfd pfd;

		pfd.events = POLLIN;
		pfd.fd     = up->fdt;
		if (1 != poll(&pfd, 1, 0))
			return;
	}
#elif defined(HAVE_SYS_SELECT_H)
	{
		struct timeval tout;
		fd_set         fset;

		memset(&tout, 0, sizeof(tout));
		FD_ZERO(&fset);
		FD_SET(up->fdt, &fset);
		if (1 != select(up->fdt+1, NULL, &fset, NULL, &tout))
			return;
	}
#else
# error Blooper! That should have been found earlier!
#endif

	/* next timeout is a full one... */
	up->tickover = TICKOVER_MAX;

	/* check for socket error */
	error = 0;
	erlen = sizeof(error);
	rc    = getsockopt(up->fdt, SOL_SOCKET, SO_ERROR, &error, &erlen);
	if (0 != rc || 0 != error) {
		msyslog(LOG_ERR, "%s: cannot connect GPSD socket: %m",
			refnumtoa(&peer->srcadr));
		goto no_socket;
	}	
	/* swap socket FDs, and make sure the clock was added */
	pp->io.fd = up->fdt;
	up->fdt   = -1;
	if (0 == io_addclock(&pp->io)) {
		msyslog(LOG_ERR, "%s: failed to register with I/O engine",
			refnumtoa(&peer->srcadr));
		goto no_socket;
	}

	up->tickover = TICKOVER_MAX;
	return;
	
  no_socket:
	if (-1 != up->fdt)
		close(up->fdt);
	up->fdt = -1;
}

/* =====================================================================
 * helper stuff
 */
static BOOL
convert_time(
	l_fp       * fp      ,
	const char * gps_time)
{
	char     *ep, *sp;
	struct tm gd;
	double    frac;

	/* Use 'strptime' to take the brunt of the work, then use 'strtoul'
	 * to read in any fractional part.
	 */
	ep = strptime(gps_time, "%Y-%m-%dT%H:%M:%S", &gd);
	if (*ep == '.') {
		errno = 0;
		frac = strtoul((sp = ep + 1), &ep, 10);
		if (errno && ep != sp)
			return FALSE;
		for (/*NOP*/; sp != ep; ++sp)
			frac *= 0.1;
	} else {
		frac = 0.0;
	}
	if (ep[0] != 'Z' || ep[1] != '\0')
		return FALSE;

	/* now convert the whole thing into a 'l_fp' */
	/*TODO: refactor the calendar stuff into 'ntp_calendar.c' */
	M_DTOLFP(frac, fp->l_ui, fp->l_uf);
	fp->l_ui += (ntpcal_tm_to_rd(&gd) - DAY_NTP_STARTS) * SECSPERDAY;
	fp->l_ui +=  ntpcal_tm_to_daysec(&gd);

	return TRUE;
}

/*
 * -------------------------------------------------------------------
 * Save the last timecode string, making sure it's properly truncated
 * if necessary and NUL terminated in any case.
 */
static void
save_ltc(
	clockprocT * const pp,
	const char * const tc)
{
	size_t len;

	len = (tc) ? strlen(tc) : 0;
	if (len >= sizeof(pp->a_lastcode))
		len = sizeof(pp->a_lastcode) - 1;
	pp->lencode = (u_short)len;
	memcpy(pp->a_lastcode, tc, len);
	pp->a_lastcode[len] = '\0';
}

/*
 * -------------------------------------------------------------------
 * asprintf replacement... it's not available everywhere...
 */
static int
myasprintf(
	char      ** spp,
	char const * fmt,
	...             )
{
	size_t alen, plen;

	alen = 32;
	*spp = NULL;
	do {
		va_list va;

		alen += alen;
		free(*spp);
		*spp = (char*)malloc(alen);
		if (NULL == *spp)
			return -1;

		va_start(va, fmt);
		plen = (size_t)vsnprintf(*spp, alen, fmt, va);
		va_end(va);
	} while (plen >= alen);

	return (int)plen;
}

#else
NONEMPTY_TRANSLATION_UNIT
#endif /* REFCLOCK && CLOCK_GPSDJSON */
