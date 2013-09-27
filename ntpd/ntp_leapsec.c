/*
 * ntp_leapsec.c - leap second processing for NTPD
 *
 * Written by Juergen Perlinger (perlinger@ntp.org) for the NTP project.
 * The contents of 'html/copyright.html' apply.
 * ----------------------------------------------------------------------
 * This is an attempt to get the leap second handling into a dedicated
 * module to make the somewhat convoluted logic testable.
 */

#include <config.h>
#include <sys/types.h>
#include <ctype.h>

#include "ntp_types.h"
#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "ntp_calendar.h"
#include "ntp_leapsec.h"
#include "ntp.h"


/* ---------------------------------------------------------------------
 * GCC is rather sticky with its 'const' attribute. We have to it more
 * explicit than with a cast...
 */
static void* noconst(const void* ptr)
{
	union {
		const void * cp;
		void *       vp;
	} tmp;
	tmp.cp = ptr;
	return tmp.vp;
}

/* ---------------------------------------------------------------------
 * Things to put into libntp...
 */

vint64
strtouv64(
	const char * begp,
	char **      endp,
	int          base)
{
	vint64  res;
	u_char  digit;
	int     sig, num;
	const u_char *src;
	
	num = 0;
	src = (const u_char*)begp;
	while (*src && isspace(*src))
		src++;

	if (*src == '-') {
		src++;
		sig = 1;
	} else {
		sig = 0;
		src += (*src == '+');
	}

	if (base == 0) {
		base = 10;
		if (*src == '0') {
			src++;
			base = 8;
			if (toupper(*src) == 'X') {
				src++;
				base = 16;
			}
		}
	} else if (base == 16) { /* remove optional leading '0x' or '0X' */
		if (src[0] == '0' && toupper(src[1]) == 'X')
			src += 2;
	} else if (base <= 2 || base > 36) {
		memset(&res, 0xFF, sizeof(res));
		errno = ERANGE;
		return res;
	}
	
	memset(&res, 0, sizeof(res));
	while (*src) {
		if (isdigit(*src))
			digit = (u_char)*src - '0';
		else if (isupper(*src))
			digit = (u_char)*src - 'A' + 10;
		else if (islower(*src))
			digit = (u_char)*src - 'a' + 10;
		else
			break;
		if (digit >= base)
			break;
		num = 1;
#if defined(HAVE_INT64)
		res.Q_s = res.Q_s * base + digit;
#else
		/* res *= base, using 16x16->32 bit
		 * multiplication. Slow but portable.
		 */ 
		{
			u_int32 accu;
			accu       = (u_int32)res.W_s.ll * base;
			res.W_s.ll = (u_short)accu;
			accu       = (accu >> 16)
			           + (u_int32)res.W_s.lh * base;
			res.W_s.lh = (u_short)accu;
			/* the upper bits can be done in one step: */
			res.D_s.hi = res.D_s.hi * base + (accu >> 16);
		}
		M_ADD(res.D_s.hi, res.D_s.lo, 0, digit);
#endif
		src++;
	}
	if (!num)
		errno = EINVAL;
	if (endp)
		*endp = (char*)noconst(src);
	if (sig)
		M_NEG(res.D_s.hi, res.D_s.lo);
	return res;
}

int icmpv64(
	const vint64 * lhs,
	const vint64 * rhs)
{
	int res;

#if defined(HAVE_INT64)
	res = (lhs->q_s > rhs->q_s)
	    - (lhs->q_s < rhs->q_s);
#else	
	res = (lhs->d_s.hi > rhs->d_s.hi)
	    - (lhs->d_s.hi < rhs->d_s.hi);
	if ( ! res )
		res = (lhs->D_s.lo > rhs->D_s.lo)
		    - (lhs->D_s.lo < rhs->D_s.lo);
#endif

	return res;
}


int ucmpv64(
	const vint64 * lhs,
	const vint64 * rhs)
{
	int res;
	
#if defined(HAVE_INT64)
	res = (lhs->Q_s > rhs->Q_s)
	    - (lhs->Q_s < rhs->Q_s);
#else	
	res = (lhs->D_s.hi > rhs->D_s.hi)
	    - (lhs->D_s.hi < rhs->D_s.hi);
	if ( ! res )
		res = (lhs->D_s.lo > rhs->D_s.lo)
		    - (lhs->D_s.lo < rhs->D_s.lo);
#endif
	return res;
}

static vint64
addv64i32(
	const vint64 * lhs,
	int32          rhs)
{
	vint64 res;

	res = *lhs;
#if defined(HAVE_INT64)
	res.q_s += rhs;
#else
	M_ADD(res.D_s.hi, res.D_s.lo,  -(rhs < 0), rhs);
#endif
	return res;
}

#if 0
static vint64
subv64i32(
	const vint64 * lhs,
	int32          rhs)
{
	vint64 res;

	res = *lhs;
#if defined(HAVE_INT64)
	res.q_s -= rhs;
#else
	M_SUB(res.D_s.hi, res.D_s.lo,  -(rhs < 0), rhs);
#endif
	return res;
}
#endif

#if 0
static vint64
addv64u32(
	const vint64 * lhs,
	u_int32        rhs)
{
	vint64 res;

	res = *lhs;
#if defined(HAVE_INT64)
	res.Q_s += rhs;
#else
	M_ADD(res.D_s.hi, res.D_s.lo, 0, rhs);
#endif
	return res;
}
#endif

static vint64
subv64u32(
	const vint64 * lhs,
	u_int32        rhs)
{
	vint64 res;

	res = *lhs;
#if defined(HAVE_INT64)
	res.Q_s -= rhs;
#else
	M_SUB(res.D_s.hi, res.D_s.lo, 0, rhs);
#endif
	return res;
}

/* ---------------------------------------------------------------------
 * Things to put into ntp_calendar... (and consequently into libntp...)
 */

/* ------------------------------------------------------------------ */
static int
ntpcal_ntp64_to_date(
	struct calendar *jd,
	const vint64    *ntp)
{
	ntpcal_split ds;
	
	ds = ntpcal_daysplit(ntp);
	ds.hi += ntpcal_daysec_to_date(jd, ds.lo);

	return ntpcal_rd_to_date(jd, ds.hi + DAY_NTP_STARTS);
}

/* ------------------------------------------------------------------ */
static vint64
ntpcal_date_to_ntp64(
	const struct calendar *jd)
{
	return ntpcal_dayjoin(ntpcal_date_to_rd(jd) - DAY_NTP_STARTS,
			      ntpcal_date_to_daysec(jd));
}


/* ---------------------------------------------------------------------
 * Our internal data structure
 */
#define MAX_HIST 10	/* history of leap seconds */

struct leap_info {
	vint64  ttime;	/* transition time (after the step, ntp scale)	*/
	u_int32 stime;	/* schedule limit (a month before transition)	*/
	short   total;	/* accumulated leap seconds from that point	*/
	u_short dynls;	/* dynamic: inserted on peer/clock request	*/
};
typedef struct leap_info leap_info_t;

struct leap_head {
	vint64  expire;	/* table expiration time          */
	u_short	size;	/* number of infos in table	  */
	int32   total;	/* total leaps before first entry */
	vint64  when;	/* begin of next leap era         */
	vint64  ttime;	/* nominal transition time        */
	vint64  stime;	/* announce leapsec 1 month ahead */
	vint64  base;	/* base of this leap era          */
	short   this_tai;	/* current TAI offset	  */
	short   next_tai;	/* TAI offset after 'when'*/
	short   dynls;	/* next leap is dynamic		  */
};
typedef struct leap_head leap_head_t;

struct leap_table {
	leap_signature_t lsig;
	leap_head_t	 head;
	leap_info_t  	 info[MAX_HIST];
};

/* Where we store our tables */
static leap_table_t _ltab[2], *_lptr;
static int/*BOOL*/  _electric;

/* Forward decls of local helpers */
static int    add_range(leap_table_t*, const leap_info_t*);
static char * get_line(leapsec_reader, void*, char*, size_t);
static char * skipws(const char*);
static void   reload_limits(leap_table_t*, const vint64*);
static int    betweenu32(u_int32, u_int32, u_int32);
static void   reset_times(leap_table_t*);
static int    leapsec_add(leap_table_t*, const vint64*, int);
static int    leapsec_raw(leap_table_t*, const vint64 *, int, int);

/* =====================================================================
 * Get & Set the current leap table
 */

/* ------------------------------------------------------------------ */
leap_table_t *
leapsec_get_table(
	int alternate)
{
	leap_table_t *p1, *p2;

	p1 = _lptr;
	p1 = &_ltab[p1 == &_ltab[1]];
	p2 = &_ltab[p1 == &_ltab[0]];
	if (alternate) {
		memcpy(p2, p1, sizeof(leap_table_t));
		p1 = p2;
	}

	return p1;
}

/* ------------------------------------------------------------------ */
int/*BOOL*/
leapsec_set_table(
	leap_table_t * pt)
{
	if (pt == &_ltab[0] || pt == &_ltab[1])
		_lptr = pt;
	return _lptr == pt;
}

/* ------------------------------------------------------------------ */
int/*BOOL*/
leapsec_electric(
	int/*BOOL*/ on)
{
	int res = _electric;
	if (on < 0)
		return res;

	_electric = on != 0;
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
 * Check if expired at a given time
 */
int/*BOOL*/
leapsec_is_expired(
	leap_table_t * pt  ,
	u_int32        when,
	const time_t * tpiv)
{
	vint64 limit;

	limit = ntpcal_ntp_to_ntp(when, tpiv);
	return ucmpv64(&limit, &pt->head.expire) >= 0;
}

/* ---------------------------------------------------------------------
 * Load a leap second file and check expiration on the go
 */
int/*BOOL*/
leapsec_load(
	leap_table_t * pt  ,
	leapsec_reader func,
	void *         farg,
	int            use_build_limit)
{
	char   *cp, *ep, linebuf[50];
	vint64 tt, limit;
	int    al;
	struct calendar build;

	leapsec_clear(pt);
	if (use_build_limit && ntpcal_get_build_date(&build))
		limit = ntpcal_date_to_ntp64(&build);
	else
		memset(&limit, 0, sizeof(limit));

	while (get_line(func, farg, linebuf, sizeof(linebuf))) {
		cp = skipws(linebuf);
		if (*cp == '#') {
			cp++;
			if (*cp == '@' || *cp == '$') {
				cp = skipws(cp+1);
				pt->head.expire = strtouv64(cp, &ep, 10);
				if (ep == cp || *ep > ' ')
					goto fail_read;
				pt->lsig.etime = pt->head.expire.D_s.lo;
			}		    
		} else if (isdigit(*cp)) {
			tt = strtouv64(cp, &ep, 10);
			if (ep == cp || *ep > ' ')
				goto fail_read;
			cp = skipws(ep);
			al = strtol(cp, &ep, 10);
			if (ep == cp || *ep > ' ')
				goto fail_read;
			cp = skipws(ep);
			if (ucmpv64(&tt, &limit) >= 0) {
				if (!leapsec_raw(pt, &tt, al, FALSE))
					goto fail_insn;
			} else {
				pt->head.total = al;
			}
			pt->lsig.ttime = tt.D_s.lo;
			pt->lsig.taiof = al;
		}
	}
	return TRUE;

fail_read:
	errno = EILSEQ;
fail_insn:
	leapsec_clear(pt);
	return FALSE;
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
	int             idx;
	vint64          ts;
	struct calendar atb, ttb;

	ntpcal_ntp64_to_date(&ttb, &pt->head.expire);
	(*func)(farg, "leap table (%u entries) expires at %04u-%02u-%02u:\n",
		pt->head.size,
		ttb.year, ttb.month, ttb.monthday);
	idx = pt->head.size;
	while (idx-- != 0) {
		ts = pt->info[idx].ttime;
		ntpcal_ntp64_to_date(&ttb, &ts);
		ts = subv64u32(&ts, pt->info[idx].stime);
		ntpcal_ntp64_to_date(&atb, &ts);

		(*func)(farg, "%04u-%02u-%02u [%c] (%04u-%02u-%02u) - %d\n",
			ttb.year, ttb.month, ttb.monthday,
			"-*"[pt->info[idx].dynls != 0],
			atb.year, atb.month, atb.monthday,
			pt->info[idx].total);
	}
}

/* =====================================================================
 * usecase driven API functions
 */

int/*BOOL*/
leapsec_query(
	leap_result_t * qr   ,
	u_int32         ts32 ,
	const time_t *  pivot)
{
	leap_table_t *   pt;
	vint64           ts64, last;
	u_int32          when32;
	int              fired;

	/* preset things we use later on... */
	fired = FALSE;
	ts64  = ntpcal_ntp_to_ntp(ts32, pivot);
	pt    = leapsec_get_table(FALSE);
	memset(qr, 0, sizeof(leap_result_t));

	if (ucmpv64(&ts64, &pt->head.base) < 0) {
		/* Ooops? Clock step backward? Oh, well... */ 
		reload_limits(pt, &ts64);
	} else if (ucmpv64(&ts64, &pt->head.when) >= 0)	{
		/* Boundary crossed in forward direction. This might
		 * indicate a leap transition, so we prepare for that
		 * case.
		 */
		last = pt->head.ttime;
		qr->warped = last.D_s.lo - pt->head.when.D_s.lo;
		reload_limits(pt, &ts64);
		if (ucmpv64(&pt->head.base, &last) == 0)
			fired = TRUE;
		else
			qr->warped = 0;
	}

	qr->tai_offs = pt->head.this_tai;

	/* If before the next schedulung alert, we're done. */
	if (ucmpv64(&ts64, &pt->head.stime) < 0)
		return fired;

	/* now start to collect the remaing data */
	when32 = pt->head.when.D_s.lo;

	qr->tai_diff = pt->head.next_tai - pt->head.this_tai;
	qr->when     = pt->head.when;
	qr->dist     = when32 - ts32;
	qr->dynamic  = pt->head.dynls;
	qr->proximity= LSPROX_SCHEDULE;

	/* if not in the last day before transition, we're done. */
	if (!betweenu32(when32-SECSPERDAY, ts32, when32))
		return fired;

	qr->proximity = LSPROX_ANNOUNCE;
	if (!betweenu32(when32-10, ts32, when32))
		return fired;

	qr->proximity = LSPROX_ALERT;
	return fired;
}

/* ------------------------------------------------------------------ */
/* Reset the current leap frame */
void
leapsec_reset_frame(void)
{
	reset_times(leapsec_get_table(FALSE));
}

/* ------------------------------------------------------------------ */
int/*BOOL*/
leapsec_load_file(
	FILE * ifp   ,
	int    blimit)
{
	leap_table_t * pt;

	pt = leapsec_get_table(TRUE);
	return leapsec_load(pt, (leapsec_reader)getc, ifp, blimit)
	    && leapsec_set_table(pt);
}

/* ------------------------------------------------------------------ */
void
leapsec_getsig(
	leap_signature_t * psig)
{
	const leap_table_t * pt;

	pt = leapsec_get_table(FALSE);
	memcpy(psig, &pt->lsig, sizeof(leap_signature_t));
}

/* ------------------------------------------------------------------ */
int/*BOOL*/
leapsec_expired(
	u_int32        when,
	const time_t * tpiv)
{
	return leapsec_is_expired(leapsec_get_table(FALSE), when, tpiv);
}

/* ------------------------------------------------------------------ */
int/*BOOL*/
leapsec_add_fix(
	u_int32        ttime,
	u_int32        etime,
	int            total,
	const time_t * pivot)
{
	time_t         tpiv;
	leap_table_t * pt;
	vint64         tt64, et64;

	if (pivot == NULL) {
		time(&tpiv);
		pivot = &tpiv;
	}
	
	et64 = ntpcal_ntp_to_ntp(etime, pivot);
	tt64 = ntpcal_ntp_to_ntp(ttime, pivot);
	pt   = leapsec_get_table(TRUE);

	if (ucmpv64(&et64, &pt->head.expire) <= 0)
		return FALSE;
	if ( ! leapsec_raw(pt, &tt64, total, FALSE))
		return FALSE;

	pt->lsig.etime = etime;
	pt->lsig.ttime = ttime;
	pt->lsig.taiof = total;

	pt->head.expire = et64;

	return leapsec_set_table(pt);
}

/* ------------------------------------------------------------------ */
int/*BOOL*/
leapsec_add_dyn(
	u_int32        ntpnow,
	int            insert,
	const time_t * pivot )
{
	leap_table_t * pt;
	vint64         now64;

	pt = leapsec_get_table(TRUE);
	now64 = ntpcal_ntp_to_ntp(ntpnow, pivot);
	return leapsec_add(pt, &now64, (insert != 0))
	    && leapsec_set_table(pt);
}

/* =====================================================================
 * internal helpers
 */

/* Reset / init the time window in the leap processor to force reload on
 * next query.
 */
static void
reset_times(
	leap_table_t * pt)
{
	pt->head.base.D_s.hi = 0;
	pt->head.base.D_s.lo = 1;
	pt->head.stime = pt->head.base;
	pt->head.ttime = pt->head.base;
	pt->head.when  = pt->head.base;
}

/* [internal] Add raw data to the table, removing old entries on the
 * fly. This cannot fail currently.
 */
static int/*BOOL*/
add_range(
	leap_table_t *      pt,
	const leap_info_t * pi)
{
	/* If the table is full, make room by throwing out the oldest
	 * entry. But remember the accumulated leap seconds!
	 */
	if (pt->head.size >= MAX_HIST) {
		pt->head.size  = MAX_HIST - 1;
		pt->head.total = pt->info[pt->head.size].total;
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
	return TRUE;
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
	char *ptr = buff;

	while (EOF != (ch = (*func)(farg)) && '\n' != ch)
		if (size > 1) {
			size--;
			*ptr++ = (char)ch;
		}
	if (size)
		*ptr = '\0';
	return (ptr == buff && ch == EOF) ? NULL : buff;
}

/* [internal] skips whitespace characters from a character buffer. */
static char *
skipws(
	const char *ptr)
{
	const u_char * src;

	src = (const u_char*)ptr;
	while (isspace(*src))
		src++;
	return (char*)noconst(src);
}

/* [internal] reload the table limits around the given time stamp. This
 * is where the real work is done when it comes to table lookup and
 * evaluation. Some care has been taken to have correct code for dealing
 * with boundary conditions and empty tables.
 */
static void
reload_limits(
	leap_table_t * pt,
	const vint64 * ts)
{
	int idx;

	/* Get full time and search the true lower bound. Use a
	 * simple loop here, since the number of entries does
	 * not warrant a binary search. This also works for an empty
	 * table, so there is no shortcut for that case.
	 */
	for (idx = 0; idx != pt->head.size; idx++)
		if (ucmpv64(ts, &pt->info[idx].ttime) >= 0)
			break;

	/* get time limits with proper bound conditions. Note that the
	 * bounds of the table will be observed even if the table is
	 * empty -- no undefined condition must arise from this code.
	 */
	if (idx >= pt->head.size) {
		memset(&pt->head.base, 0x00, sizeof(vint64));
		pt->head.this_tai = pt->head.total;
	} else {
		pt->head.base     = pt->info[idx].ttime;
		pt->head.this_tai = pt->info[idx].total;
	}
	if (--idx >= 0) {
		pt->head.next_tai = pt->info[idx].total;
		pt->head.dynls    = pt->info[idx].dynls;
		pt->head.ttime    = pt->info[idx].ttime;

		if (!_electric)
			pt->head.when = addv64i32(
				&pt->head.ttime,
				pt->head.next_tai - pt->head.this_tai);
		else
			pt->head.when = pt->head.ttime;
		
		pt->head.stime = subv64u32(
			&pt->head.ttime, pt->info[idx].stime);

	} else {
		memset(&pt->head.ttime, 0xFF, sizeof(vint64));
		pt->head.stime    = pt->head.ttime;
		pt->head.when     = pt->head.ttime;
		pt->head.next_tai = pt->head.this_tai;
		pt->head.dynls    = 0;
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
static int/*BOOL*/
leapsec_add(
	leap_table_t*  pt    ,
	const vint64 * now64 ,
	int            insert)
{
	vint64		ttime, stime;
	struct calendar	fts;
	leap_info_t	li;

	/* Check against the table expiration and the lates available
	 * leap entry. Do not permit inserts, only appends, and only if
	 * the extend the table beyond the expiration!
	 */
	if (   ucmpv64(now64, &pt->head.expire) < 0
	   || (pt->head.size && ucmpv64(now64, &pt->info[0].ttime) <= 0)) {
		errno = ERANGE;
		return FALSE;
	}

	ntpcal_ntp64_to_date(&fts, now64);
	/* To guard against dangling leap flags: do not accept leap
	 * second request on the 1st hour of the 1st day of the month.
	 */
	if (fts.monthday == 1 && fts.hour == 0) {
		errno = EINVAL;
		return FALSE;
	}

	/* Ok, do the remaining calculations */
	fts.monthday = 1;
	fts.hour     = 0;
	fts.minute   = 0;
	fts.second   = 0;
	stime = ntpcal_date_to_ntp64(&fts);
	fts.month++;
	ttime = ntpcal_date_to_ntp64(&fts);

	li.ttime = ttime;
	li.stime = ttime.D_s.lo - stime.D_s.lo;
	li.total = (pt->head.size ? pt->info[0].total : pt->head.total)
	         + (insert ? 1 : -1);
	li.dynls = 1;
	return add_range(pt, &li);
}

/* [internal] Given a time stamp for a leap insertion (the exact begin
 * of the new leap era), create new leap frame and put it into the
 * table. This is the work horse for reading a leap file and getting a
 * leap second update via authenticated network packet.
 */
int/*BOOL*/
leapsec_raw(
	leap_table_t * pt,
	const vint64 * ttime,
	int            total,
	int            dynls)
{
	vint64		stime;
	struct calendar	fts;
	leap_info_t	li;

	/* Check that we only extend the table. Paranoia rulez! */
	if (pt->head.size && ucmpv64(ttime, &pt->info[0].ttime) <= 0) {
		errno = ERANGE;
		return FALSE;
	}

	ntpcal_ntp64_to_date(&fts, ttime);
	/* If this does not match the exact month start, bail out. */
	if (fts.monthday != 1 || fts.hour || fts.minute || fts.second) {
		errno = EINVAL;
		return FALSE;
	}
	fts.month--; /* was in range 1..12, no overflow here! */
	stime    = ntpcal_date_to_ntp64(&fts);
	li.ttime = *ttime;
	li.stime = ttime->D_s.lo - stime.D_s.lo;
	li.total = (short)total;
	li.dynls = (dynls != 0);
	return add_range(pt, &li);
}

/* [internal] Do a wrap-around save range inclusion check.
 * Returns TRUE if x in [lo,hi[ (intervall open on right side) with full
 * handling of an overflow / wrap-around.
 */
static int/*BOOL*/
betweenu32(
	u_int32 lo,
	u_int32 x,
	u_int32 hi)
{
	int rc;
	if (lo <= hi)
		rc = (lo <= x) && (x < hi);
	else
		rc = (lo <= x) || (x < hi);
	return rc;
}

/* -*- that's all folks! -*- */
