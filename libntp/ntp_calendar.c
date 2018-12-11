/*
 * ntp_calendar.c - calendar and helper functions
 *
 * Written by Juergen Perlinger <perlinger@ntp.org> for the NTP project.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: NTP
 *
 * There is more about these types and calculations in the internals tour
 * document distributed with the code.
 */
#include "config.h"
#include <sys/types.h>

#include "ntp_types.h"
#include "ntp_calendar.h"
#include "ntp_stdlib.h"
#include "ntp_fp.h"

/*
 *---------------------------------------------------------------------
 * replacing the 'time()' function
 * --------------------------------------------------------------------
 */

static ntpcal_split
ntpcal_days_in_months(int32_t /* months */);

static  int32_t
ntpcal_edate_to_yeardays(int32_t, int32_t, int32_t);

/*
 *---------------------------------------------------------------------
 * Get the build date & time
 *---------------------------------------------------------------------
 */
int
ntpcal_get_build_date(
	struct calendar * jd
	)
{
        time_t epoch = (time_t)BUILD_EPOCH;
        struct tm epoch_tm;

	ZERO(*jd);
	jd->year     = 1970;
	jd->month    = 1;
	jd->monthday = 1;

        if (NULL == gmtime_r(&epoch, &epoch_tm)) {
            /* bad EPOCH */
	    return false;
        }
	/* good EPOCH */
	jd->year     = epoch_tm.tm_year + 1900;
	jd->yearday  = epoch_tm.tm_yday + 1;
	jd->month    = epoch_tm.tm_mon + 1;
	jd->monthday = epoch_tm.tm_mday;
	jd->hour     = epoch_tm.tm_hour;
	jd->minute   = epoch_tm.tm_min;
	jd->second   = epoch_tm.tm_sec;
	jd->weekday  = epoch_tm.tm_wday;

#if 0
        fprintf(stderr, "Build: %d %d %d %d %d %d %d %d\n",
	    (int)jd->year, (int)jd->yearday, (int)jd->month, (int)jd->monthday,
	    (int)jd->hour, (int)jd->minute, (int)jd->second, (int)jd->weekday);
#endif
        return true;
}


/*
 *---------------------------------------------------------------------
 * basic calendar stuff
 * --------------------------------------------------------------------
 */

/* month table for a year starting with March,1st */
static const uint16_t shift_month_table[13] = {
	0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337, 366
};

/* month tables for years starting with January,1st; regular & leap */
static const uint16_t real_month_table[2][13] = {
	/* -*- table for regular years -*- */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	/* -*- table for leap years -*- */
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

/*
 * Some notes on the terminology:
 *
 * We use the proleptic Gregorian calendar, which is the Gregorian
 * calendar extended in both directions ad infinitum. This totally
 * disregards the fact that this calendar was invented in 1582, and
 * was adopted at various dates over the world; sometimes even after
 * the start of the NTP epoch.
 *
 * Normally date parts are given as current cycles, while time parts
 * are given as elapsed cycles:
 *
 * 1970-01-01/03:04:05 means 'IN the 1970st. year, IN the first month,
 * ON the first day, with 3hrs, 4minutes and 5 seconds elapsed.
 *
 * The basic calculations for this calendar implementation deal with
 * ELAPSED date units, which is the number of full years, full months
 * and full days before a date: 1970-01-01 would be (1969, 0, 0) in
 * that notation.
 *
 * To ease the numeric computations, month and day values outside the
 * normal range are acceptable: 2001-03-00 will be treated as the day
 * before 2001-03-01, 2000-13-32 will give the same result as
 * 2001-02-01 and so on.
 *
 * 'rd' or 'RD' is used as an abbreviation for the latin 'rata die'
 * (day number).  This is the number of days elapsed since 0000-12-31
 * in the proleptic Gregorian calendar. The begin of the Christian Era
 * (0001-01-01) is RD(1).
 *
 *
 * Some notes on the implementation:
 *
 * Calendar algorithms thrive on the division operation, which is one of
 * the slowest numerical operations in any CPU. What saves us here from
 * abysmal performance is the fact that all divisions are divisions by
 * constant numbers, and most compilers can do this by a multiplication
 * operation.  But this might not work when using the div/ldiv/lldiv
 * function family, because many compilers are not able to do inline
 * expansion of the code with following optimisation for the
 * constant-divider case.
 *
 * Also div/ldiv/lldiv are defined in terms of int/long/longlong, which
 * are inherently target dependent. Nothing that could not be cured with
 * autoconf, but still a mess...
 *
 * Furthermore, we need floor division while C demands truncation to
 * zero, so additional steps are required to make sure the algorithms
 * work.
 *
 * For all this, all divisions by constant are coded manually, even when
 * there is a joined div/mod operation: The optimiser should sort that
 * out, if possible.
 *
 * Finally, the functions do not check for overflow conditions. This
 * is a sacrifice made for execution speed; since a 32-bit day counter
 * covers +/- 5,879,610 years, this should not pose a problem here.
 */


/*
 * ==================================================================
 *
 * General algorithmic stuff
 *
 * ==================================================================
 */

/*
 *---------------------------------------------------------------------
 * Do a periodic extension of 'value' around 'pivot' with a period of
 * 'cycle'.
 *
 * The result 'res' is a number that holds to the following properties:
 *
 *   1)	 res MOD cycle == value MOD cycle
 *   2)	 pivot <= res < pivot + cycle
 *	 (replace </<= with >/>= for negative cycles)
 *
 * where 'MOD' denotes the modulo operator for FLOOR DIVISION, which
 * is not the same as the '%' operator in C: C requires division to be
 * a truncated division, where remainder and dividend have the same
 * sign if the remainder is not zero, whereas floor division requires
 * divider and modulus to have the same sign for a non-zero modulus.
 *
 * This function has some useful applications:
 *
 * + let Y be a calendar year and V a truncated 2-digit year: then
 *	periodic_extend(Y-50, V, 100)
 *   is the closest expansion of the truncated year with respect to
 *   the full year, that is a 4-digit year with a difference of less
 *   than 50 years to the year Y. ("century unfolding")
 *
 * + let T be a UN*X time stamp and V be seconds-of-day: then
 *	perodic_extend(T-43200, V, 86400)
 *   is a time stamp that has the same seconds-of-day as the input
 *   value, with an absolute difference to T of <= 12hrs.  ("day
 *   unfolding")
 *
 * + Wherever you have a truncated periodic value and a non-truncated
 *   base value and you want to match them somehow...
 *
 * Basically, the function delivers 'pivot + (value - pivot) % cycle',
 * but the implementation takes some pains to avoid internal signed
 * integer overflows in the '(value - pivot) % cycle' part and adheres
 * to the floor division convention.
 *
 * If 64bit scalars where available on all intended platforms, writing a
 * version that uses 64 bit ops would be easy; writing a general
 * division routine for 64bit ops on a platform that can only do
 * 32/16bit divisions and is still performant is a bit more
 * difficult. Since most usecases can be coded in a way that does only
 * require the 32-bit version a 64bit version is NOT provided here.
 * ---------------------------------------------------------------------
 */
int32_t
ntpcal_periodic_extend(
	int32_t pivot,
	int32_t value,
	int32_t cycle
	)
{
	uint32_t diff;
	bool	 cpl = false; /* modulo complement flag */
	bool	 neg = false; /* sign change flag	    */

	/* make the cycle positive and adjust the flags */
	if (cycle < 0) {
		cycle = - cycle;
		neg = !neg;
		cpl = !cpl;
	}
	/* guard against div by zero or one */
	if (cycle > 1) {
		/*
		 * Get absolute difference as unsigned quantity and
		 * the complement flag. This is done by always
		 * subtracting the smaller value from the bigger
		 * one. This implementation works only on a two's
		 * complement machine!
		 */
		if (value >= pivot) {
			diff = (uint32_t)value - (uint32_t)pivot;
		} else {
			diff = (uint32_t)pivot - (uint32_t)value;
			cpl = !cpl;
		}
		diff %= (uint32_t)cycle;
		if (diff) {
			if (cpl)
				diff = (uint32_t)cycle - diff;
			if (neg)
				diff = ~diff + 1;
			pivot += (int32_t)diff;
		}
	}
	return pivot;
}

/*
 *-------------------------------------------------------------------
 * Convert a timestamp in NTP scale to a 64bit seconds value in the UN*X
 * scale with proper epoch unfolding around a given pivot or the current
 * system time. This function happily accepts negative pivot values as
 * timestamps befor 1970-01-01, so be aware of possible trouble on
 * platforms with 32bit 'time_t'!
 *
 * This is also a periodic extension, but since the cycle is 2^32 and
 * the shift is 2^31, we can do some *very* fast math without explicit
 * divisions.
 *-------------------------------------------------------------------
 */
time64_t
ntpcal_ntp_to_time(
	uint32_t	ntp,
	time_t		pivot
	)
{
	time64_t res;

	settime64s(res, pivot);
	settime64u(res, time64u(res)-0x80000000);	/* unshift of half range */
	ntp	-= (uint32_t)JAN_1970;		/* warp into UN*X domain */
	ntp	-= time64lo(res);		/* cycle difference	 */
	settime64u(res, time64u(res)+(uint64_t)ntp);	/* get expanded time */

	return res;
}

/*
 *-------------------------------------------------------------------
 * Convert a timestamp in NTP scale to a 64bit seconds value in the NTP
 * scale with proper epoch unfolding around a given pivot or the current
 * system time.
 *
 * Note: The pivot must be given in the UN*X time domain!
 *
 * This is also a periodic extension, but since the cycle is 2^32 and
 * the shift is 2^31, we can do some *very* fast math without explicit
 * divisions.
 *-------------------------------------------------------------------
 */
time64_t
ntpcal_ntp_to_ntp(
	uint32_t      ntp,
	time_t	      pivot
	)
{
	time64_t res;

	settime64s(res, pivot);
	settime64u(res, time64u(res) - 0x80000000);		/* unshift of half range */
	settime64u(res, time64u(res) + (uint32_t)JAN_1970);	/* warp into NTP domain	 */

	ntp	-= time64lo(res);				/* cycle difference	 */
	settime64u(res, time64u(res) + (uint64_t)ntp);	/* get expanded time	 */

	return res;
}


/*
 * ==================================================================
 *
 * Splitting values to composite entities
 *
 * ==================================================================
 */

/*
 *-------------------------------------------------------------------
 * Split a 64bit seconds value into elapsed days in 'res.hi' and
 * elapsed seconds since midnight in 'res.lo' using explicit floor
 * division. This function happily accepts negative time values as
 * timestamps before the respective epoch start.
 * -------------------------------------------------------------------
 */
ntpcal_split
ntpcal_daysplit(
	const time64_t ts
	)
{
	ntpcal_split res;

	/* manual floor division by SECSPERDAY */
	res.hi = (int32_t)(time64s(ts) / SECSPERDAY);
	res.lo = (int32_t)(time64s(ts) % SECSPERDAY);
	if (res.lo < 0) {
		res.hi -= 1;
		res.lo += SECSPERDAY;
	}

	return res;
}

/*
 *-------------------------------------------------------------------
 * Split a 32bit seconds value into h/m/s and excessive days.  This
 * function happily accepts negative time values as timestamps before
 * midnight.
 * -------------------------------------------------------------------
 */
static int32_t
priv_timesplit(
	int32_t split[3],
	int32_t ts
	)
{
	int32_t days = 0;

	/* make sure we have a positive offset into a day */
	if (ts < 0 || ts >= SECSPERDAY) {
		days = ts / SECSPERDAY;
		ts   = ts % SECSPERDAY;
		if (ts < 0) {
			days -= 1;
			ts   += SECSPERDAY;
		}
	}

	/* get secs, mins, hours */
	split[2] = (uint8_t)(ts % SECSPERMIN);
	ts /= SECSPERMIN;
	split[1] = (uint8_t)(ts % MINSPERHR);
	split[0] = (uint8_t)(ts / MINSPERHR);

	return days;
}

/*
 * ---------------------------------------------------------------------
 * Given the number of elapsed days in the calendar era, split this
 * number into the number of elapsed years in 'res.hi' and the number
 * of elapsed days of that year in 'res.lo'.
 *
 * if 'isleapyear' is not NULL, it will receive an integer that is 0 for
 * regular years and a non-zero value for leap years.
 *---------------------------------------------------------------------
 */
ntpcal_split
ntpcal_split_eradays(
	int32_t days,
	int32_t *isleapyear
	)
{
	ntpcal_split res;
	int32_t	     n400, n100, n004, n001, yday; /* calendar year cycles */

	/*
	 * Split off calendar cycles, using floor division in the first
	 * step. After that first step, simple division does it because
	 * all operands are positive; alas, we have to be aware of the
	 * possible cycle overflows for 100 years and 1 year, caused by
	 * the additional leap day.
	 */
	n400 = days / GREGORIAN_CYCLE_DAYS;
	yday = days % GREGORIAN_CYCLE_DAYS;
	if (yday < 0) {
		n400 -= 1;
		yday += GREGORIAN_CYCLE_DAYS;
	}
	n100 = yday / GREGORIAN_NORMAL_CENTURY_DAYS;
	yday = yday % GREGORIAN_NORMAL_CENTURY_DAYS;
	n004 = yday / GREGORIAN_NORMAL_LEAP_CYCLE_DAYS;
	yday = yday % GREGORIAN_NORMAL_LEAP_CYCLE_DAYS;
	n001 = yday / DAYSPERYEAR;
	yday = yday % DAYSPERYEAR;

	/*
	 * check for leap cycle overflows and calculate the leap flag
	 * if needed
	 */
	if ((n001 | n100) > 3) {
		/* hit last day of leap year */
		n001 -= 1;
		yday += DAYSPERYEAR;
		if (isleapyear)
			*isleapyear = 1;
	} else if (isleapyear)
		*isleapyear = (n001 == 3) && ((n004 != 24) || (n100 == 3));

	/* now merge the cycles to elapsed years, using horner scheme */
	res.hi = ((4*n400 + n100)*25 + n004)*4 + n001;
	res.lo = yday;

	return res;
}

/*
 *---------------------------------------------------------------------
 * Given a number of elapsed days in a year and a leap year indicator,
 * split the number of elapsed days into the number of elapsed months in
 * 'res.hi' and the number of elapsed days of that month in 'res.lo'.
 *
 * This function will fail and return {-1,-1} if the number of elapsed
 * days is not in the valid range!
 *---------------------------------------------------------------------
 */
ntpcal_split
ntpcal_split_yeardays(
	int32_t eyd,
	bool    isleapyear
	)
{
	ntpcal_split    res;
	const uint16_t *lt;	/* month length table	*/

	/* check leap year flag and select proper table */
	lt = real_month_table[(isleapyear != 0)];
	if (0 <= eyd && eyd < lt[12]) {
		/* get zero-based month by approximation & correction step */
		res.hi = eyd >> 5;	   /* approx month; might be 1 too low */
		if (lt[res.hi + 1] <= eyd) /* fixup approximative month value  */
			res.hi += 1;
		res.lo = eyd - lt[res.hi];
	} else {
		res.lo = res.hi = -1;
	}

	return res;
}

/*
 *---------------------------------------------------------------------
 * Convert a RD into the date part of a 'struct calendar'.
 * Returns -1 on calculation overflow.
 *---------------------------------------------------------------------
 */
int
ntpcal_rd_to_date(
	struct calendar *jd,
	int32_t		 rd
	)
{
	ntpcal_split split;
	int32_t	     leaps;
	int32_t	     retv;

	leaps = 0;
	retv = 0;
	/* Get day-of-week first. Since rd is signed, the remainder can
	 * be in the range [-6..+6], but the assignment to an unsigned
	 * variable maps the negative values to positive values >=7.
	 * This makes the sign correction look strange, but adding 7
	 * causes the needed wrap-around into the desired value range of
	 * zero to six, both inclusive.
	 */
	jd->weekday = rd % 7;
	if (jd->weekday >= 7)	/* unsigned! */
		jd->weekday += 7;

	split = ntpcal_split_eradays(rd - 1, &leaps);
	retv  = (int)leaps;
	/* get year and day-of-year */
	jd->year = (uint16_t)split.hi + 1;
	if (jd->year != split.hi + 1) {
		jd->year = 0;
		retv	 = -1;	/* bletch. overflow trouble. */
	}
	jd->yearday = (uint16_t)split.lo + 1;

	/* convert to month and mday */
	split = ntpcal_split_yeardays(split.lo, leaps);
	jd->month    = (uint8_t)split.hi + 1;
	jd->monthday = (uint8_t)split.lo + 1;

	return retv ? retv : leaps;
}

/*
 *---------------------------------------------------------------------
 * Take a value of seconds since midnight and split it into hhmmss in a
 * 'struct calendar'.
 *---------------------------------------------------------------------
 */
int32_t
ntpcal_daysec_to_date(
	struct calendar *jd,
	int32_t		sec
	)
{
	int32_t days;
	int   ts[3];

	days = priv_timesplit(ts, sec);
	jd->hour   = (uint8_t)ts[0];
	jd->minute = (uint8_t)ts[1];
	jd->second = (uint8_t)ts[2];

	return days;
}

/*
 *---------------------------------------------------------------------
 * Take a UN*X time and convert to a calendar structure.
 *---------------------------------------------------------------------
 */
int
ntpcal_time_to_date(
	struct calendar	*jd,
	const time64_t ts
	)
{
	ntpcal_split ds;

	ds = ntpcal_daysplit(ts);
	ds.hi += ntpcal_daysec_to_date(jd, ds.lo);
	ds.hi += DAY_UNIX_STARTS;

	return ntpcal_rd_to_date(jd, ds.hi);
}


/*
 * ==================================================================
 *
 * merging composite entities
 *
 * ==================================================================
 */

/*
 *---------------------------------------------------------------------
 * Merge a number of days and a number of seconds into seconds,
 * expressed in 64 bits to avoid overflow.
 *---------------------------------------------------------------------
 */
time64_t
ntpcal_dayjoin(
	int32_t days,
	int32_t secs
	)
{
	time64_t res;

	settime64s(res, days);
	settime64s(res, time64s(res) * SECSPERDAY);
	settime64s(res, time64s(res) + secs);

	return res;
}

/*
 *---------------------------------------------------------------------
 * Convert elapsed years in Era into elapsed days in Era.
 *
 * To accommodate for negative values of years, floor division would be
 * required for all division operations. This can be eased by first
 * splitting the years into full 400-year cycles and years in the
 * cycle. Only this operation must be coded as a full floor division; as
 * the years in the cycle is a non-negative number, all other divisions
 * can be regular truncated divisions.
 *---------------------------------------------------------------------
 */
int32_t
ntpcal_days_in_years(
	int32_t years
	)
{
	int32_t cycle; /* full gregorian cycle */

	/* split off full calendar cycles, using floor division */
	cycle = years / 400;
	years = years % 400;
	if (years < 0) {
		cycle -= 1;
		years += 400;
	}

	/*
	 * Calculate days in cycle. years now is a non-negative number,
	 * holding the number of years in the 400-year cycle.
	 */
	return cycle * GREGORIAN_CYCLE_DAYS
	     + years * DAYSPERYEAR	/* days inregular years	*/
	     + years / 4		/* 4 year leap rule	*/
	     - years / 100;		/* 100 year leap rule	*/
	/* the 400-year rule does not apply due to full-cycle split-off */
}

/*
 *---------------------------------------------------------------------
 * Convert a number of elapsed month in a year into elapsed days in year.
 *
 * The month will be normalized, and 'res.hi' will contain the
 * excessive years that must be considered when converting the years,
 * while 'res.lo' will contain the number of elapsed days since start
 * of the year.
 *
 * This code uses the shifted-month-approach to convert month to days,
 * because then there is no need to have explicit leap year
 * information.	 The slight disadvantage is that for most month values
 * the result is a negative value, and the year excess is one; the
 * conversion is then simply based on the start of the following year.
 *---------------------------------------------------------------------
 */
static ntpcal_split
ntpcal_days_in_months(
	int32_t m
	)
{
	ntpcal_split res;

	/* normalize month into range */
	res.hi = 0;
	res.lo = m;
	if (res.lo < 0 || res.lo >= 12) {
		res.hi = res.lo / 12;
		res.lo = res.lo % 12;
		if (res.lo < 0) {
			res.hi -= 1;
			res.lo += 12;
		}
	}

	/* add 10 month for year starting with march */
	if (res.lo < 2)
		res.lo += 10;
	else {
		res.hi += 1;
		res.lo -= 2;
	}

	/* get cummulated days in year with unshift */
	res.lo = shift_month_table[res.lo] - 306;

	return res;
}

/*
 *---------------------------------------------------------------------
 * Convert ELAPSED years/months/days of gregorian calendar to elapsed
 * days in Gregorian epoch.
 *
 * If you want to convert years and days-of-year, just give a month of
 * zero.
 *---------------------------------------------------------------------
 */
int32_t
ntpcal_edate_to_eradays(
	int32_t years,
	int32_t mons,
	int32_t mdays
	)
{
	ntpcal_split tmp;
	int32_t	     res;

	if (mons) {
		tmp = ntpcal_days_in_months(mons);
		res = ntpcal_days_in_years(years + tmp.hi) + tmp.lo;
	} else
		res = ntpcal_days_in_years(years);
	res += mdays;

	return res;
}

/*
 *---------------------------------------------------------------------
 * Convert ELAPSED years/months/days of gregorian calendar to elapsed
 * days in year.
 *
 * Note: This will give the true difference to the start of the given year,
 * even if months & days are off-scale.
 *---------------------------------------------------------------------
 */
static int32_t
ntpcal_edate_to_yeardays(
	int32_t years,
	int32_t mons,
	int32_t mdays
	)
{
	ntpcal_split tmp;

	if (0 <= mons && mons < 12) {
		years += 1;
		mdays += real_month_table[is_leapyear(years)][mons];
	} else {
		tmp = ntpcal_days_in_months(mons);
		mdays += tmp.lo
		       + ntpcal_days_in_years(years + tmp.hi)
		       - ntpcal_days_in_years(years);
	}

	return mdays;
}

/*
 *---------------------------------------------------------------------
 * Convert elapsed days and the hour/minute/second information into
 * total seconds.
 *
 * If 'isvalid' is not NULL, do a range check on the time specification
 * and tell if the time input is in the normal range, permitting for a
 * single leapsecond.
 *---------------------------------------------------------------------
 */
int32_t
ntpcal_etime_to_seconds(
	int32_t hours,
	int32_t minutes,
	int32_t seconds
	)
{
	int32_t res;

	res = (hours * MINSPERHR + minutes) * SECSPERMIN + seconds;

	return res;
}

/*
 *---------------------------------------------------------------------
 * Convert the date part of a 'struct tm' (that is, year, month,
 * day-of-month) into the RD of that day.
 *---------------------------------------------------------------------
 */
int32_t
ntpcal_tm_to_rd(
	const struct tm *utm
	)
{
	return ntpcal_edate_to_eradays(utm->tm_year + 1899,
				       utm->tm_mon,
				       utm->tm_mday - 1) + 1;
}

/*
 *---------------------------------------------------------------------
 * Convert the date part of a 'struct calendar' (that is, year, month,
 * day-of-month) into the RD of that day.
 *---------------------------------------------------------------------
 */
int32_t
ntpcal_date_to_rd(
	const struct calendar *jd
	)
{
	return ntpcal_edate_to_eradays((int32_t)jd->year - 1,
				       (int32_t)jd->month - 1,
				       (int32_t)jd->monthday - 1) + 1;
}

/*
 *---------------------------------------------------------------------
 * take a 'struct calendar' and get the seconds-of-day from it.
 *---------------------------------------------------------------------
 */
int32_t
ntpcal_date_to_daysec(
	const struct calendar *jd
	)
{
	return ntpcal_etime_to_seconds(jd->hour, jd->minute,
				       jd->second);
}

/*
 *---------------------------------------------------------------------
 * take a 'struct tm' and get the seconds-of-day from it.
 *---------------------------------------------------------------------
 */
int32_t
ntpcal_tm_to_daysec(
	const struct tm *utm
	)
{
	return ntpcal_etime_to_seconds(utm->tm_hour, utm->tm_min,
				       utm->tm_sec);
}

/*
 *---------------------------------------------------------------------
 * take a 'struct calendar' and convert it to a 'time_t'
 *---------------------------------------------------------------------
 */
time_t
ntpcal_date_to_time(
	const struct calendar *jd
	)
{
	time64_t  join;
	int32_t days, secs;

	days = ntpcal_date_to_rd(jd) - DAY_UNIX_STARTS;
	secs = ntpcal_date_to_daysec(jd);
	join = ntpcal_dayjoin(days, secs);

	/* might truncate if time_t is 32 bits */
	return (time_t)join;
}


int
ntpcal_ntp64_to_date(
	struct calendar *jd,
	const time64_t  ntp
	)
{
	ntpcal_split ds;

	ds = ntpcal_daysplit(ntp);
	ds.hi += ntpcal_daysec_to_date(jd, ds.lo);

	return ntpcal_rd_to_date(jd, ds.hi + DAY_NTP_STARTS);
}

int
ntpcal_ntp_to_date(
	struct calendar *jd,
	uint32_t	 ntp,
	const time_t	piv
	)
{
	time64_t	ntp64;

	/*
	 * Unfold ntp time around current time into NTP domain. Split
	 * into days and seconds, shift days into CE domain and
	 * process the parts.
	 */
	ntp64 = ntpcal_ntp_to_ntp(ntp, piv);
	return ntpcal_ntp64_to_date(jd, ntp64);
}

/*
 * ymd2yd - compute the date in the year from y/m/d
 *
 * A thin wrapper around a more general calendar function.
 */

int
ymd2yd(
	int y,
	int m,
	int d)
{
	/*
	 * convert y/m/d to elapsed calendar units, convert that to
	 * elapsed days since the start of the given year and convert
	 * back to unity-based day in year.
	 *
	 * This does no further error checking, since the underlying
	 * function is assumed to work out how to handle the data.
	 */
	return ntpcal_edate_to_yeardays(y-1, m-1, d-1) + 1;
}
/* -*-EOF-*- */
