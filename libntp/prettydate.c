/*
 * prettydate - convert a time stamp to something readable
 */
#include <config.h>
#include <stdio.h>
#include <stdbool.h>

#include "ntp_fp.h"
#include "lib_strbuf.h"
#include "ntp_stdlib.h"
#include "ntp_assert.h"
#include "ntp_calendar.h"

#if SIZEOF_TIME_T < 4
# error sizeof(time_t) < 4 -- this will not work!
#endif

static char *common_prettydate(l_fp *, bool);

/* Helper function to handle possible wraparound of the ntp epoch.
 *
 * Works by periodic extension of the ntp time stamp in the UN*X epoch.
 * If the 'time_t' is 32 bit, use solar cycle warping to get the value
 * in a suitable range. Also uses solar cycle warping to work around
 * really buggy implementations of 'gmtime_r()' / 'localtime_r()' that
 * cannot work with a negative time value, that is, times before
 * 1970-01-01. (MSVCRT...)
 *
 * Apart from that we're assuming that the localtime/gmtime library
 * functions have been updated so that they work...
 *
 * An explanation: The julian calendar repeats ever 28 years, because
 * it's the LCM of 7 and 1461, the week and leap year cycles. This is
 * called a 'solar cycle'. The gregorian calendar does the same as
 * long as no centennial year (divisible by 100, but not 400) goes in
 * the way. So between 1901 and 2099 (inclusive) we can warp time
 * stamps by 28 years to make them suitable for localtime_r() and
 * gmtime_r() if we have trouble. Of course this will play hubbubb with
 * the DST zone switches, so we should do it only if necessary; but as
 * we NEED a proper conversion to dates via gmtime_r() we should try to
 * cope with as many idiosyncrasies as possible.
 *
 */

/*
 * solar cycle in unsigned secs and years, and the cycle limits.
 */
#define SOLAR_CYCLE_SECS   0x34AADC80UL	/* 7*1461*86400*/
#define SOLAR_CYCLE_YEARS  28
#define MINFOLD -3
#define MAXFOLD	 3

static struct tm *
get_struct_tm(
	const	vint64 *stamp,
	int	local,
	struct	tm *tmbuf)
{
	struct tm *tm;
	int32_t	   folds = 0;
	time_t	   ts;

	int64_t tl;
	ts = tl = vint64s(*stamp);

	/*
	 * If there is chance of truncation, try to fix it. Let the
	 * compiler find out if this can happen at all.
	 */
	while (ts != tl) { /* truncation? */
		if (tl < 0) {
			if (--folds < MINFOLD)
				return NULL;
			tl += SOLAR_CYCLE_SECS;
		} else {
			if (++folds > MAXFOLD)
				return NULL;
			tl -= SOLAR_CYCLE_SECS;
		}
		ts = tl; /* next try... */
	}

	/*
	 * 'ts' should be a suitable value by now. Just go ahead, but
	 * with care:
	 *
	 * There are some pathological implementations of 'gmtime_r()'
	 * and 'localtime_r()' out there. No matter if we have 32-bit or
	 * 64-bit 'time_t', try to fix this by solar cycle warping
	 * again...
	 *
	 * At least the MSDN says that the (Microsoft) Windoze
	 * versions of 'gmtime_r()' and 'localtime_r()' will bark on time
	 * stamps < 0.
	 */
	while ((tm = (*(local ? localtime_r : gmtime_r))(&ts, tmbuf)) == NULL)
		if (ts < 0) {
			if (--folds < MINFOLD)
				return NULL;
			ts += SOLAR_CYCLE_SECS;
		} else if (ts >= (time_t)SOLAR_CYCLE_SECS) {
			if (++folds > MAXFOLD)
				return NULL;
			ts -= SOLAR_CYCLE_SECS;
		} else
			return NULL; /* That's truly pathological! */

	/* 'tm' surely not NULL here! */
	NTP_INSIST(tm != NULL);
	if (folds != 0) {
		tm->tm_year += folds * SOLAR_CYCLE_YEARS;
		if (tm->tm_year <= 0 || tm->tm_year >= 200)
			return NULL;	/* left warp range... can't help here! */
	}

	return tm;
}

static char *
common_prettydate(
	l_fp *ts,
	bool local
	)
{
	static const char pfmt[] =
	    "%08lx.%08lx %04d-%02d-%02dT%02d:%02d:%02d.%03u";

	char	    *bp;
	struct tm   *tm, tmbuf;
	u_int	     msec;
	uint32_t	     ntps;
	vint64	     sec;

	LIB_GETBUF(bp);

	/* get & fix milliseconds */
	ntps = ts->l_ui;
	msec = ts->l_uf / 4294967;	/* fract / (2 ** 32 / 1000) */
	if (msec >= 1000u) {
		msec -= 1000u;
		ntps++;
	}
	sec = ntpcal_ntp_to_time(ntps, NULL);
	tm  = get_struct_tm(&sec, local, &tmbuf);
	if (!tm) {
		/*
		 * get a replacement, but always in UTC, using
		 * ntpcal_time_to_date()
		 */
		struct calendar jd;
		ntpcal_time_to_date(&jd, &sec);
		snprintf(bp, LIB_BUFLENGTH, pfmt,
			 (u_long)ts->l_ui, (u_long)ts->l_uf,
			 jd.year, jd.month, jd.monthday,
			 jd.hour, jd.minute, jd.second, msec);
		strncat(bp, "Z",  LIB_BUFLENGTH);
	} else {
		snprintf(bp, LIB_BUFLENGTH, pfmt,
			 (u_long)ts->l_ui, (u_long)ts->l_uf,
			 1900 + tm->tm_year, tm->tm_mon+1, tm->tm_mday,
			 tm->tm_hour, tm->tm_min, tm->tm_sec, msec);
		if (!local)
			strncat(bp, "Z", LIB_BUFLENGTH);
	}
	return bp;
}


char *
prettydate(
	l_fp *ts
	)
{
	return common_prettydate(ts, true);
}


char *
rfc3339date(
	l_fp *ts
	)
{
	return common_prettydate(ts, false) + 18; /* skip past hex time */
}


char *
gmprettydate(
	l_fp *ts
	)
{
	return common_prettydate(ts, false);
}

