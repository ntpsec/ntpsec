/*
 * ntp_leapsec.c - leap second processing for NTPD
 *
 * Written by Juergen Perlinger <perlinger@ntp.org> for the NTP project.
 *
 * ----------------------------------------------------------------------
 * This is an attempt to get the leap second handling into a dedicated
 * module to make the somewhat convoluted logic testable.
 *
 * Note: This code assumes that the unsigned long long return value of
 * strtoull(3) is large enough to parse any integer literal found in these
 * files, and that C will promote such values to uint64_t properly.
 *
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: NTP
 */

#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#include "ntp_types.h"
#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "ntp_calendar.h"
#include "ntp_leapsec.h"
#include "ntp.h"
#include "ntpd.h"	/* for rfc3339time() only */
#include "lib_strbuf.h"

#include <openssl/evp.h>

#define ISC_SHA1_DIGESTLENGTH 20U

static const char * const logPrefix = "leapsecond file";

/* ---------------------------------------------------------------------
 * Our internal data structure
 */
#define MAX_HIST 10	/* history of leap seconds */

struct leap_info {
	time_t   ttime;	/* transition time (after the step, POSIX epoch) */
	time_t   stime;	/* schedule limit (a month before transition)  */
	int16_t  taiof;	/* TAI offset on and after the transition      */
	bool     dynls; /* dynamic: inserted on peer/clock request     */
};
typedef struct leap_info leap_info_t;

struct leap_head {
	time_t   update; /* time of information update                 */
	time_t   expire; /* table expiration time                      */
	uint16_t size;	 /* number of infos in table	               */
	int16_t  base_tai;	/* total leaps before first entry      */
	int16_t  this_tai;	/* current TAI offset	               */
	int16_t  next_tai;	/* TAI offset after 'when'             */
	time_t   dtime;	 /* due time (current era end)                 */
	time_t   ttime;	 /* nominal transition time (next era start)   */
	time_t   stime;	 /* schedule time (when we take notice)        */
	time_t   ebase;	 /* base time of this leap era                 */
	bool     dynls;	 /* next leap is dynamic (by peer request)     */
};
typedef struct leap_head leap_head_t;

struct leap_table {
	leap_signature_t lsig;
	leap_head_t	 head;
	leap_info_t  	 info[MAX_HIST];
};

/* Where we store our tables */
static leap_table_t _ltab[2], *_lptr;
static bool  _electric;

/* Forward decls of local helpers */
static bool   add_range(leap_table_t*, const leap_info_t*);
static char * get_line(leapsec_reader, void*, char*, size_t);
static char * skipws(char*) __attribute__((pure));
static bool   parsefail(const char * cp, const char * ep);
static void   reload_limits(leap_table_t*, time_t);
static void   reset_times(leap_table_t*);
static bool   leapsec_add(leap_table_t*, time_t, int);
static bool   leapsec_raw(leap_table_t*, time_t, int, bool);

/* time_t is unsigned.  This is used for infinity in tables */
#if NTP_SIZEOF_TIME_T == 8
# define LAST_time_t 0x7fffffffffffffff
#elif NTP_SIZEOF_TIME_T == 4
# define LAST_time_t 0x7fffffff
#endif

/* =====================================================================
 * Get & Set the current leap table
 */

/* ------------------------------------------------------------------ */
leap_table_t *
leapsec_get_table(
	bool alternate)
{
	leap_table_t *p1, *p2;

	p1 = _lptr;
	if (p1 == &_ltab[0]) {
		p2 = &_ltab[1];
	} else if (p1 == &_ltab[1]) {
		p2 = &_ltab[0];
	} else {
		p1 = &_ltab[0];
		p2 = &_ltab[1];
		reset_times(p1);
		reset_times(p2);
		_lptr = p1;
	}
	if (alternate) {
		memcpy(p2, p1, sizeof(leap_table_t));
		p1 = p2;
	}

	return p1;
}

/* ------------------------------------------------------------------ */
bool
leapsec_set_table(
	leap_table_t * pt)
{
	if (pt == &_ltab[0] || pt == &_ltab[1])
		_lptr = pt;
	return _lptr == pt;
}

/* ------------------------------------------------------------------ */
bool
leapsec_electric(
	electric_mode el)
{
	int res = _electric;
	if (el == electric_query)
		return res;

	_electric = (el == electric_on);
	if (_electric == res)
		return res;

	if (_lptr == &_ltab[0] || _lptr == &_ltab[1])
		reset_times(_lptr);

	return res;
}

/* =====================================================================
 * API functions that operate on tables
 */

/* ---------------------------------------------------------------------
 * Clear all leap second data. Use it for init & cleanup
 */
void
leapsec_clear(
	leap_table_t * pt)
{
	memset(&pt->lsig, 0, sizeof(pt->lsig));
	memset(&pt->head, 0, sizeof(pt->head));
	reset_times(pt);
}

/* ---------------------------------------------------------------------
 * Load a leap second file and check expiration on the go
 */
bool
leapsec_load(
	leap_table_t * pt  ,
	leapsec_reader func,
	void *         farg)
{
	char   *cp, *ep, linebuf[50];
	time_t ttime;
	uint64_t ntp;  /* Time from file with NTP epoch of 1900 */
	long   taiof;

	leapsec_clear(pt);

	while (get_line(func, farg, linebuf, sizeof(linebuf))) {
		cp = linebuf;
		if (*cp == '#') {
			cp++;
			if (*cp == '@') {
				cp = skipws(cp+1);
				ntp = strtoull(cp, &ep, 10);
				if (parsefail(cp, ep))
					goto fail_read;
				pt->head.expire = (time_t)(ntp - JAN_1970);
				pt->lsig.etime = pt->head.expire;
			} else if (*cp == '$') {
				cp = skipws(cp+1);
				ntp = strtoull(cp, &ep, 10);
				if (parsefail(cp, ep))
					goto fail_read;
				pt->head.update = (time_t)(ntp - JAN_1970);
			}
		} else if (isdigit((uint8_t)*cp)) {
			ntp = strtoull(cp, &ep, 10);
			if (parsefail(cp, ep))
				goto fail_read;
			ttime = (time_t)(ntp - JAN_1970);
			cp = skipws(ep);
			taiof = strtol(cp, &ep, 10);
			if (   parsefail(cp, ep)
			    || taiof > SHRT_MAX || taiof < SHRT_MIN)
				goto fail_read;
			if (!leapsec_raw(pt, ttime, taiof, false))
				goto fail_insn;
			pt->lsig.ttime = ttime;
			pt->lsig.taiof = (int16_t)taiof;
		}
	}
	return true;

fail_read:
	errno = EILSEQ;
fail_insn:
	leapsec_clear(pt);
	return false;
}

/* ---------------------------------------------------------------------
 * Dump a table in human-readable format. Use 'fprintf' and a FILE
 * pointer if you want to get it printed into a stream.
 */
void
leapsec_dump(
	const leap_table_t * pt  ,
	leapsec_dumper       func,
	void *               farg)
{
	int       idx;
	time_t    ts;
	struct tm   atb, ttb;

	ts = pt->head.expire;
	gmtime_r(&ts, &ttb);
	(*func)(farg, "leap table (%u entries) expires at %04u-%02u-%02u:\n",
		pt->head.size,
		ttb.tm_year+1900, ttb.tm_mon+1, ttb.tm_mday);
	idx = pt->head.size;
	while (idx-- != 0) {
		ts = pt->info[idx].ttime;
		gmtime_r(&ts, &ttb);
		ts = ts - pt->info[idx].stime;   // FIXME ???
		gmtime_r(&ts, &atb);

		(*func)(farg, "%04u-%02u-%02u [%c] (%04u-%02u-%02u) - %d\n",
			ttb.tm_year+1900, ttb.tm_mon+1, ttb.tm_mday,
			"-*"[pt->info[idx].dynls],
			atb.tm_year+1900, atb.tm_mon+1, atb.tm_mday,
			pt->info[idx].taiof);
	}
}

/* =====================================================================
 * usecase driven API functions
 */

bool
leapsec_query(
	leap_result_t * qr,
	time_t        when)
{
	leap_table_t *   pt;
	time_t         last, next;
	bool             fired;

	/* preset things we use later on... */
	fired = false;
	pt    = leapsec_get_table(false);
	memset(qr, 0, sizeof(leap_result_t));

	if (when < pt->head.ebase) {
		/* Most likely after leap frame reset. Could also be a
		 * backstep of the system clock. Anyway, get the new
		 * leap era frame.
		 */
		reload_limits(pt, when);
	} else if (when >= pt->head.dtime) {
		/* Boundary crossed in forward direction. This might
		 * indicate a leap transition, so we prepare for that
		 * case.
		 *
		 * Some operations below are actually NOPs in electric
		 * mode, but having only one code path that works for
		 * both modes is easier to maintain.
		 */
		last = pt->head.ttime;
		// FIXME warped is only 16 bits.  ????
		qr->warped = (int16_t)(last - pt->head.dtime);
		next = when + qr->warped;
		reload_limits(pt, next);
		fired = (pt->head.ebase == last);
		if (fired) {
			when = next;
		} else {
			qr->warped = 0;
		}
	}

	qr->tai_offs = pt->head.this_tai;

	/* If before the next scheduling alert, we're done. */
	if (when < pt->head.stime)
		return fired;

	/* now start to collect the remaining data */
	qr->tai_diff  = pt->head.next_tai - pt->head.this_tai;
	qr->ttime     = pt->head.ttime;
	qr->ddist     = (uint32_t)(pt->head.dtime - when);
	qr->dynamic   = pt->head.dynls;

	qr->proximity = LSPROX_SCHEDULE;
	/* if not in the last day before transition, we're done. */

	if (when >= pt->head.dtime)
		return fired;	/* Future */

	if (when < pt->head.dtime - SECSPERDAY)
		return fired;	/* Before last day */

	qr->proximity = LSPROX_ANNOUNCE;
	if (when < pt->head.dtime - 10)
		return fired;	/* Before last 10 seconds */

	/* The last 10s before the transition. Prepare for action! */
	qr->proximity = LSPROX_ALERT;
	return fired;
}

/* ------------------------------------------------------------------ */
bool
leapsec_frame(
        leap_result_t *qr)
{
	const leap_table_t * pt;

        memset(qr, 0, sizeof(leap_result_t));
	pt = leapsec_get_table(false);
	if (pt->head.ttime <= pt->head.stime)
                return false;

	qr->tai_offs = pt->head.this_tai;
	qr->tai_diff = pt->head.next_tai - pt->head.this_tai;
	qr->ttime    = pt->head.ttime;
	qr->dynamic  = pt->head.dynls;

        return true;
}

/* ------------------------------------------------------------------ */
/* Reset the current leap frame */
void
leapsec_reset_frame(void)
{
	reset_times(leapsec_get_table(false));
}

/* ------------------------------------------------------------------ */
/* load a file from a FILE pointer. Note: If hcheck is true, load
 * only after successful signature check. The stream must be seekable
 * or this will fail.
 */
bool
leapsec_load_stream(
	FILE       * ifp  ,
	const char * fname,
	bool  logall)
{
	leap_table_t *pt;
	int           rcheck;

	if (NULL == fname)
		fname = "<unknown>";

	rcheck = leapsec_validate((leapsec_reader)getc, ifp);
	if (logall)
		switch (rcheck)
		{
		case LSVALID_GOODHASH:
			msyslog(LOG_NOTICE, "CLOCK: %s ('%s'): good hash signature",
				logPrefix, fname);
			break;

		case LSVALID_NOHASH:
			msyslog(LOG_ERR, "CLOCK: %s ('%s'): no hash signature",
				logPrefix, fname);
			break;
		case LSVALID_BADHASH:
			msyslog(LOG_ERR, "CLOCK: %s ('%s'): signature mismatch",
				logPrefix, fname);
			break;
		case LSVALID_BADFORMAT:
			msyslog(LOG_ERR, "CLOCK: %s ('%s'): malformed hash signature",
				logPrefix, fname);
			break;
		default:
			msyslog(LOG_ERR, "CLOCK: %s ('%s'): unknown error code %d",
				logPrefix, fname, rcheck);
			break;
		}
	if (rcheck < 0)
		return false;

	rewind(ifp);
	pt = leapsec_get_table(true);
	if (!leapsec_load(pt, (leapsec_reader)getc, ifp)) {
		switch (errno) {
		case EINVAL:
			msyslog(LOG_ERR, "CLOCK: %s ('%s'): bad transition time",
				logPrefix, fname);
			break;
		case ERANGE:
			msyslog(LOG_ERR, "CLOCK: %s ('%s'): times not ascending",
				logPrefix, fname);
			break;
		default:
			msyslog(LOG_ERR, "CLOCK: %s ('%s'): parsing error",
				logPrefix, fname);
			break;
		}
		return false;
	}

	if (pt->head.size)
		msyslog(LOG_NOTICE, "CLOCK: %s ('%s'): loaded, expire=%s last=%s ofs=%d",
			logPrefix, fname, rfc3339time(pt->head.expire),
			rfc3339time(pt->info[0].ttime), pt->info[0].taiof);
	else
		msyslog(LOG_NOTICE,
			"CLOCK: %s ('%s'): loaded, expire=%s ofs=%d (no entries after build date)",
			logPrefix, fname, rfc3339time(pt->head.expire),
			pt->head.base_tai);

	return leapsec_set_table(pt);
}

/* ------------------------------------------------------------------ */
bool
leapsec_load_file(
	const char  * fname,
	struct stat * sb_old,
	bool   force,
	bool   logall)
{
	FILE       * fp;
	struct stat  sb_new;
	int          rc;

	/* just do nothing if there is no leap file */
	if ( !(fname && *fname) )
		return false;

	/* try to stat the leapfile */
	/* coverity[toctou] */
	if (0 != stat(fname, &sb_new)) {
		if (logall)
			msyslog(LOG_ERR, "CLOCK: %s ('%s'): stat failed: %m",
				logPrefix, fname);
		return false;
	}

	/* silently skip to postcheck if no new file found */
	if (NULL != sb_old) {
		if (!force
		 && sb_old->st_mtime == sb_new.st_mtime
		 && sb_old->st_ctime == sb_new.st_ctime
		   )
			return false;
		*sb_old = sb_new;
	}

	/* try to open the leap file, complain if that fails
	 *
	 * [perlinger@ntp.org]
	 * coverity raises a TOCTOU (time-of-check/time-of-use) issue
	 * here, which is not entirely helpful: While there is indeed a
	 * possible race condition between the 'stat()' call above and
	 * the 'fopen)' call below, I intentionally want to omit the
	 * overhead of opening the file and calling 'fstat()', because
	 * in most cases the file would have be to closed anyway without
	 * reading the contents.  I chose to disable the coverity
	 * warning instead.
	 *
	 * So unless someone comes up with a reasonable argument why
	 * this could be a real issue, I'll just try to silence coverity
	 * on that topic.
	 */
	/* coverity[toctou] */
	if ((fp = fopen(fname, "r")) == NULL) {
		if (logall)
			msyslog(LOG_ERR,
				"CLOCK: %s ('%s'): open failed: %m",
				logPrefix, fname);
		return false;
	}

	rc = leapsec_load_stream(fp, fname, logall);
	fclose(fp);
	return rc;
}

/* ------------------------------------------------------------------ */
void
leapsec_getsig(
	leap_signature_t * psig)
{
	const leap_table_t * pt;

	pt = leapsec_get_table(false);
	memcpy(psig, &pt->lsig, sizeof(leap_signature_t));
}

/* ------------------------------------------------------------------ */
bool
leapsec_expired(
	time_t       when)
{
	const leap_table_t * pt;

	pt = leapsec_get_table(false);
	return (when >= pt->head.expire);
}

/* ------------------------------------------------------------------ */
int32_t
leapsec_daystolive(
	time_t limit)
{
	const leap_table_t * pt;
	long secs_left;

	pt = leapsec_get_table(false);
	secs_left = pt->head.expire - limit;
	if (secs_left < 0)
		secs_left -= (SECSPERDAY-1);
	return secs_left/SECSPERDAY;
}

/* ------------------------------------------------------------------ */
bool
leapsec_add_fix(
	int          tai_offset,
	time_t       ttime,
	time_t       etime)
{
	leap_table_t * pt;

	pt   = leapsec_get_table(true);

	if ((etime <= pt->head.expire)
	   || !leapsec_raw(pt, ttime, tai_offset, false) )
		return false;

	pt->lsig.etime = etime;
	pt->lsig.ttime = ttime;
	pt->lsig.taiof = (int16_t)tai_offset;

	pt->head.expire = etime;

	return leapsec_set_table(pt);
}

/* ------------------------------------------------------------------ */
bool
leapsec_add_dyn(
	bool         insert,
	time_t       when)
{
	leap_table_t * pt;

	pt = leapsec_get_table(true);
	return (leapsec_add(pt, when, insert)
		&& leapsec_set_table(pt));
}

/* =====================================================================
 * internal helpers
 */

/* [internal] Reset / init the time window in the leap processor to
 * force reload on next query. Since a leap transition cannot take place
 * at an odd second, the value chosen avoids spurious leap transition
 * triggers. Making all three times equal forces a reload. Using the
 * maximum value for unsigned 64 bits makes finding the next leap frame
 * a bit easier.
 */
static void
reset_times(
	leap_table_t * pt)
{
	pt->head.ebase = LAST_time_t;
	pt->head.stime = pt->head.ebase;
	pt->head.ttime = pt->head.ebase;
	pt->head.dtime = pt->head.ebase;
}

/* [internal] Add raw data to the table, removing old entries on the
 * fly. This cannot fail currently.
 */
static bool
add_range(
	leap_table_t *      pt,
	const leap_info_t * pi)
{
	/* If the table is full, make room by throwing out the oldest
	 * entry. But remember the accumulated leap seconds!
	 */
	if (pt->head.size >= MAX_HIST) {
		pt->head.size     = MAX_HIST - 1;
		pt->head.base_tai = pt->info[pt->head.size].taiof;
	}

	/* make room in lower end and insert item */
	memmove(pt->info+1, pt->info, pt->head.size*sizeof(*pt->info));
	pt->info[0] = *pi;
	pt->head.size++;

	/* invalidate the cached limit data -- we might have news ;-)
	 *
	 * This blocks a spurious transition detection. OTOH, if you add
	 * a value after the last query before a leap transition was
	 * expected to occur, this transition trigger is lost. But we
	 * can probably live with that.
	 */
	reset_times(pt);
	return true;
}

/* [internal] given a reader function, read characters into a buffer
 * until either EOL or EOF is reached. Makes sure that the buffer is
 * always NUL terminated, but silently truncates excessive data. The
 * EOL-marker ('\n') is *not* stored in the buffer.
 *
 * Returns the pointer to the buffer, unless EOF was reached when trying
 * to read the first character of a line.
 */
static char *
get_line(
	leapsec_reader func,
	void *         farg,
	char *         buff,
	size_t         size)
{
	int   ch;
	char *ptr;

	/* if we cannot even store the delimiter, declare failure */
	if (buff == NULL || size == 0)
		return NULL;

	ptr = buff;
	while (EOF != (ch = (*func)(farg)) && '\n' != ch)
		if (size > 1) {
			size--;
			*ptr++ = (char)ch;
		}
	/* discard trailing whitespace */
	while (ptr != buff && isspace((uint8_t)ptr[-1]))
		ptr--;
	*ptr = '\0';
	return (ptr == buff && ch == EOF) ? NULL : buff;
}

/* [internal] skips whitespace characters from a character buffer. */
static char *
skipws(
	char *ptr)
{
	while (isspace((uint8_t)*ptr))
		ptr++;
	return ptr;
}

/* [internal] check if a strtoXYZ ended at EOL or whitespace and
 * converted something at all. Return true if something went wrong.
 */
static bool
parsefail(
	const char * cp,
	const char * ep)
{
	return (cp == ep)
	    || (*ep && *ep != '#' && !isspace((uint8_t)*ep));
}

/* [internal] reload the table limits around the given time stamp. This
 * is where the real work is done when it comes to table lookup and
 * evaluation. Some care has been taken to have correct code for dealing
 * with boundary conditions and empty tables.
 *
 * In electric mode, transition and trip time are the same. In dumb
 * mode, the difference of the TAI offsets must be taken into account
 * and trip time and transition time become different. The difference
 * becomes the warping distance when the trip time is reached.
 */
static void
reload_limits(
	leap_table_t * pt,
	time_t ts)
{
	int idx;

	/* Get full time and search the true lower bound. Use a
	 * simple loop here, since the number of entries does
	 * not warrant a binary search. This also works for an empty
	 * table, so there is no shortcut for that case.
	 */
	for (idx = 0; idx != pt->head.size; idx++)
		if (ts >= pt->info[idx].ttime)
			break;

	/* get time limits with proper bound conditions. Note that the
	 * bounds of the table will be observed even if the table is
	 * empty -- no undefined condition must arise from this code.
	 */
	if (idx >= pt->head.size) {
		pt->head.ebase = 0;
		pt->head.this_tai = pt->head.base_tai;
	} else {
		pt->head.ebase    = pt->info[idx].ttime;
		pt->head.this_tai = pt->info[idx].taiof;
	}
	if (--idx >= 0) {
		pt->head.next_tai = pt->info[idx].taiof;
		pt->head.dynls    = pt->info[idx].dynls;
		pt->head.ttime    = pt->info[idx].ttime;

		if (_electric)
			pt->head.dtime = pt->head.ttime;
                else
			pt->head.dtime = pt->head.ttime +
				pt->head.next_tai - pt->head.this_tai;

		pt->head.stime =  pt->head.ttime - pt->info[idx].stime;

	} else {
		pt->head.ttime = LAST_time_t;
		pt->head.stime    = pt->head.ttime;
		pt->head.dtime    = pt->head.ttime;
		pt->head.next_tai = pt->head.this_tai;
		pt->head.dynls    = false;
	}
}

/* [internal] Take a time stamp and create a leap second frame for
 * it. This will schedule a leap second for the beginning of the next
 * month, midnight UTC. The 'insert' argument tells if a leap second is
 * added (!=0) or removed (==0). We do not handle multiple inserts
 * (yet?)
 *
 * Returns 1 if the insert worked, 0 otherwise. (It's not possible to
 * insert a leap second into the current history -- only appending
 * towards the future is allowed!)
 */
static bool
leapsec_add(
	leap_table_t*  pt,
	const time_t   now,
	int            insert)
{
	time_t		ttime, starttime;
	struct tm	fts;
	leap_info_t	li;

	/* Check against the table expiration and the latest available
	 * leap entry. Do not permit inserts, only appends, and only if
	 * the extend the table beyond the expiration!
	 */
	if ((now < pt->head.expire)
	    || (pt->head.size && (now <= pt->info[0].ttime))) {
		errno = ERANGE;
		return false;
	}

	gmtime_r(&now, &fts);
	/* To guard against dangling leap flags: do not accept leap
	 * second request on the 1st hour of the 1st day of the month.
	 */
#ifndef ENABLE_LEAP_TESTING
	if (fts.tm_mday == 1 && fts.tm_hour == 0) {
		errno = EINVAL;
		return false;
	}
#endif

	/* Ok, do the remaining calculations */
	fts.tm_hour = 0;
	fts.tm_min  = 0;
	fts.tm_sec  = 0;
	starttime = mktime(&fts);
	fts.tm_mon++;
	if (fts.tm_mon == 12) {
		fts.tm_mon = 0;
		fts.tm_year++;
	}
	ttime = mktime(&fts);
	li.ttime = ttime;
	li.stime = ttime - starttime;
	li.taiof = (pt->head.size ? pt->info[0].taiof : pt->head.base_tai)
	         + (insert ? 1 : -1);
	li.dynls = 1;
	return add_range(pt, &li);
}

/* [internal] Given a time stamp for a leap insertion (the exact begin
 * of the new leap era), create new leap frame and put it into the
 * table. This is the work horse for reading a leap file and getting a
 * leap second update via authenticated network packet.
 */
bool
leapsec_raw(
	leap_table_t * pt,
	const time_t   ttime,
	int            taiof,
	bool           dynls)
{
	time_t		starttime;
	struct tm	fts;
	leap_info_t	li;

	/* Check that we only extend the table. Paranoia rulez! */
	if (pt->head.size && (ttime <= pt->info[0].ttime)) {
		errno = ERANGE;
		return false;
	}

	gmtime_r(&ttime, &fts);
#ifndef ENABLE_LEAP_TESTING
	/* If this does not match the exact month start, bail out. */
	if (fts.tm_mday != 1 || fts.tm_hour || fts.tm_min || fts.tm_sec) {
		errno = EINVAL;
		return false;
	}
#endif
	/* Start 28 days earler.  Avoids month arithmetic.  */
	starttime = ttime - 28*SECSPERDAY;
	li.ttime = ttime;
	li.stime = ttime - starttime;
	li.taiof = (int16_t)taiof;
	li.dynls = dynls;
	return add_range(pt, &li);
}


/* =====================================================================
 * validation stuff
 */

typedef struct {
	unsigned char hv[ISC_SHA1_DIGESTLENGTH];
} sha1_digest;

/* [internal] parse a digest line to get the hash signature
 * The NIST code creating the hash writes them out as 5 hex integers
 * without leading zeros. This makes reading them back as hex-encoded
 * BLOB impossible, because there might be less than 40 hex digits.
 *
 * The solution is to read the values back as integers, and then do the
 * byte twiddle necessary to get it into an array of 20 chars. The
 * drawback is that it permits any acceptable number syntax provided by
 * 'scanf()' and 'strtoul()', including optional signs and '0x'
 * prefixes.
 */
static bool
do_leap_hash(
	sha1_digest * mac,
	char const  * cp )
{
	int wi, di, num, len;
	unsigned long tmp[5];

	memset(mac, 0, sizeof(*mac));
	num = sscanf(cp, " %lx %lx %lx %lx %lx%n",
		     &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4],
		     &len);
	if (num != 5 || cp[len] > ' ')
		return false;

	/* now do the byte twiddle */
	for (wi=0; wi < 5; ++wi)
		for (di=3; di >= 0; --di) {
			mac->hv[wi*4 + di] =
				(unsigned char)(tmp[wi] & 0x0FF);
			tmp[wi] >>= 8;
		}
	return true;
}

/* [internal] add the digits of a data line to the hash, stopping at the
 * next hash ('#') character.
 */
static void
do_hash_data(
	EVP_MD_CTX * mdctx,
	char const * cp   )
{
	unsigned char  text[32]; // must be power of two!
	unsigned int   tlen =  0;
	unsigned char  ch;

	while ('\0' != (ch = (unsigned char)(*cp++)) && '#' != ch)
		if (isdigit(ch)) {
			text[tlen++] = ch;
			tlen &= (sizeof(text)-1);
			if (0 == tlen)
				EVP_DigestUpdate(
				    mdctx, (const void *)text, sizeof(text));
		}

	if (0 < tlen)
		EVP_DigestUpdate(mdctx, (const void *)text, (size_t)tlen);
}

/* given a reader and a reader arg, calculate and validate the hash
 * signature of a NIST leap second file.
 */
int
leapsec_validate(
	leapsec_reader func,
	void *         farg)
{
	EVP_MD_CTX     *mdctx;
	sha1_digest    rdig, ldig; /* remote / local digests */
	char           line[50];
	int            hlseen = -1;

	mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL);
	while (get_line(func, farg, line, sizeof(line))) {
		if (!strncmp(line, "#h", 2))
			hlseen = do_leap_hash(&rdig, line+2);
		else if (!strncmp(line, "#@", 2))
			do_hash_data(mdctx, line+2);
		else if (!strncmp(line, "#$", 2))
			do_hash_data(mdctx, line+2);
		else if (isdigit((unsigned char)line[0]))
			do_hash_data(mdctx, line);
	}
	EVP_DigestFinal_ex(mdctx, ldig.hv, NULL);
	EVP_MD_CTX_destroy(mdctx);

	if (0 > hlseen)
		return LSVALID_NOHASH;
	if (0 == hlseen)
		return LSVALID_BADFORMAT;
	if (0 != memcmp(&rdig, &ldig, sizeof(sha1_digest)))
		return LSVALID_BADHASH;
	return LSVALID_GOODHASH;
}

/* reset the global state for unit tests */
void
leapsec_ut_pristine(void)
{
	memset(_ltab, 0, sizeof(_ltab));
	_lptr     = NULL;
	_electric = 0;
}



/* -*- that's all folks! -*- */
