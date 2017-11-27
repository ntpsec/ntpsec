/*
 * ntp_calendar.h - definitions for the calendar time-of-day routine
 */
#ifndef GUARD_NTP_CALENDAR_H
#define GUARD_NTP_CALENDAR_H

#include <time.h>

#include "ntp_types.h"

/* gregorian calendar date */
struct calendar {
	uint16_t year;		/* year (A.D.) */
	uint16_t yearday;	/* day of year, 1 = January 1 */
	uint8_t  month;		/* month, 1 = January */
	uint8_t  monthday;	/* day of month */
	uint8_t  hour;		/* hour of day, midnight = 0 */
	uint8_t  minute;	/* minute of hour */
	uint8_t  second;	/* second of minute */
	uint8_t  weekday;	/* 0..7, 0=Sunday */
};

/* general split representation */
typedef struct {
	int32_t hi;
	int32_t lo;
} ntpcal_split;

typedef time_t (*systime_func_ptr)(time_t *);

/*
 * We deal in a 4 year cycle starting at March 1, 1900.	 We assume
 * we will only want to deal with dates since then, and not to exceed
 * the rollover day in 2036.
 */
#define	SECSPERMIN	(60)			/* seconds per minute */
#define	MINSPERHR	(60)			/* minutes per hour */
#define	HRSPERDAY	(24)			/* hours per day */
#define	DAYSPERWEEK	(7)			/* days per week */
#define	DAYSPERYEAR	(365)			/* days per year */

#define	SECSPERHR	(SECSPERMIN * MINSPERHR)
#define	SECSPERDAY	(SECSPERHR * HRSPERDAY)
#define	SECSPERWEEK	(DAYSPERWEEK * SECSPERDAY)
#define	SECSPERYEAR	(365 * SECSPERDAY)	/* regular year */
#define	SECSPERLEAPYEAR	(366 * SECSPERDAY)	/* leap year */
#define	SECSPERAVGYEAR	31556952	/* mean year length over 400yrs */

/*
 * Get the build date & time in UTC.  This depends on the BUILD_EPOCH
 * which is fixed at configure time.
 */
extern int
ntpcal_get_build_date(struct calendar * /* jd */);

/*
 * Convert a timestamp in NTP scale to a time_t value in the UN*X
 * scale with proper epoch unfolding around a given pivot or the
 * current system time.
 */
extern time64_t
ntpcal_ntp_to_time(uint32_t /* ntp */, time_t /* pivot */);

/*
 * Convert a timestamp in NTP scale to a 64bit seconds value in the NTP
 * scale with proper epoch unfolding around a given pivot or the current
 * system time.
 * Note: The pivot must be given in UN*X time scale!
 */
extern time64_t
ntpcal_ntp_to_ntp(uint32_t /* ntp */, time_t /* pivot */);

/*
 * Split a time stamp in seconds into elapsed days and elapsed seconds
 * since midnight.
 */
extern ntpcal_split
ntpcal_daysplit(const time64_t);

/*
 * Merge a number of days and a number of seconds into seconds,
 * expressed in 64 bits to avoid overflow.
 */
extern time64_t
ntpcal_dayjoin(int32_t /* days */, int32_t /* seconds */) __attribute__((const));

/*
 * Convert elapsed years in Era into elapsed days in Era.
 */
extern int32_t
ntpcal_days_in_years(int32_t /* years */) __attribute__((const));

#define days_per_year(x)	((x) % 4 ? 365 : ((x % 400) ? ((x % 100) ? 366 : 365) : 366))

/*
 * Convert ELAPSED years/months/days of gregorian calendar to elapsed
 * days in Gregorian epoch. No range checks done here!
 */
extern int32_t
ntpcal_edate_to_eradays(int32_t /* years */, int32_t /* months */, int32_t /* mdays */);

/*
 * Convert a time spec to seconds. No range checks done here!
 */
extern int32_t
ntpcal_etime_to_seconds(int32_t /* hours */, int32_t /* minutes */,
                        int32_t /* seconds */) __attribute__((const));

/*
 * Convert the date part of a 'struct tm' (that is, year, month,
 * day-of-month) into the RataDie of that day.
 */
extern int32_t
ntpcal_tm_to_rd(const struct tm * /* utm */);

/*
 * Convert the date part of a 'struct calendar' (that is, year, month,
 * day-of-month) into the RataDie of that day.
 */
extern int32_t
ntpcal_date_to_rd(const struct calendar * /* jt */);

/*
 * Given the number of elapsed days in the calendar era, split this
 * number into the number of elapsed years in 'res.quot' and the
 * number of elapsed days of that year in 'res.rem'.
 *
 * if 'isleapyear' is not NULL, it will receive an integer that is 0
 * for regular years and a non-zero value for leap years.
 */
extern ntpcal_split
ntpcal_split_eradays(int32_t /* days */, int32_t * /* isleapyear */);

/*
 * Given a number of elapsed days in a year and a leap year indicator,
 * split the number of elapsed days into the number of elapsed months
 * in 'res.quot' and the number of elapsed days of that month in
 * 'res.rem'.
 */
extern ntpcal_split
ntpcal_split_yeardays(int32_t /* eyd */, bool /* isleapyear */);

/*
 * Convert a RataDie number into the date part of a 'struct
 * calendar'. Return 0 if the year is regular year, 1 if the year is
 * a leap year, -1 if the calculation overflowed.
 */
extern int
ntpcal_rd_to_date(struct calendar * /* jt */, int32_t /* rd */);

/*
 * Take a value of seconds since midnight and split it into hhmmss in
 * a 'struct calendar'. Return excessive days.
 */
/* used by ntpd/refclock_nmea.c */
extern int32_t
ntpcal_daysec_to_date(struct calendar * /* jt */, int32_t /* secs */);

/*
 * Take the time part of a 'struct calendar' and return the seconds
 * since midnight.
 */
/* used by ntpd/refclock_nmea.c */
extern int32_t
ntpcal_date_to_daysec(const struct calendar *);

extern int32_t
ntpcal_tm_to_daysec(const struct tm * /* utm */);

extern int
ntpcal_time_to_date(struct calendar * /* jd */, const time64_t /* ts */);

extern int32_t
ntpcal_periodic_extend(int32_t /* pivot */, int32_t /* value */,
		       int32_t /* cycle */) __attribute__((const));

extern int
ntpcal_ntp64_to_date(struct calendar * /* jd */, const time64_t /* ntp */);

extern int
ntpcal_ntp_to_date(struct calendar * /* jd */,	uint32_t /* ntp */,
		   time_t /* pivot */);

extern time_t
ntpcal_date_to_time(const struct calendar * /* jd */);


/*
 * Additional support stuff for Ed Rheingold's calendrical calculations
 */

/*
 * Start day of NTP time as days past the imaginary date 12/1/1 BC.
 * (This is the beginning of the Christian Era, or BCE.)
 */
#define	DAY_NTP_STARTS 693596

/*
 * Start day of the UNIX epoch. This is the Rata Die of 1970-01-01.
 */
#define DAY_UNIX_STARTS 719163

/*
 * Difference between UN*X and NTP epoch (25567).
 */
#define NTP_TO_UNIX_DAYS (DAY_UNIX_STARTS - DAY_NTP_STARTS)

/*
 * Days in a normal 4 year leap year calendar cycle (1461).
 */
#define	GREGORIAN_NORMAL_LEAP_CYCLE_DAYS	(3 * 365 + 366)

/*
 * Days in a normal 100 year leap year calendar (36524).  We lose a
 * leap day in years evenly divisible by 100 but not by 400.
 */
#define	GREGORIAN_NORMAL_CENTURY_DAYS	\
			(25 * GREGORIAN_NORMAL_LEAP_CYCLE_DAYS - 1)

/*
 * The Gregorian calendar is based on a 400 year cycle. This is the
 * number of days in each cycle (146097).  We gain a leap day in years
 * divisible by 400 relative to the "normal" century.
 */
#define	GREGORIAN_CYCLE_DAYS (4 * GREGORIAN_NORMAL_CENTURY_DAYS + 1)

/*
 * Number of weeks in 400 years (20871).
 */
#define	GREGORIAN_CYCLE_WEEKS (GREGORIAN_CYCLE_DAYS / 7)

#define	is_leapyear(y)	(!((y) % 4) && !(!((y) % 100) && (y) % 400))

/*
 * Time of day conversion constant.  NTP's time scale starts in 1900,
 * Unix in 1970.  The value is 1970 - 1900 in seconds, 0x83aa7e80 or
 * 2208988800.  This is larger than 32-bit INT_MAX, so unsigned
 * type is forced.
 */
#define	JAN_1970 ((unsigned int)NTP_TO_UNIX_DAYS * (unsigned int)SECSPERDAY)

#endif
