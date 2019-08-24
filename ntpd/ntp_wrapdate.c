/*
 * Calendar-oriented stuff -- perhaps a bit hard to grok.
 *
 * Used for interpreting GPS sentences and resolving wrap issues
 * around 2-digit years.  Formerly part of refclock_nmea.c,
 * dissected out so it can be used by other drivers.
 */

#include "config.h"
#include "ntp_types.h"

#include "ntp.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_stdlib.h"
#include "ntp_calendar.h"
#include "ntp_wrapdate.h"
#include "timespecops.h"

static int32_t g_gpsMinBase;
static int32_t g_gpsMinYear;

/*
 * The GPS week time scale starts on Sunday, 1980-01-06. We need the
 * rata die number of this day.
 */
#ifndef DAY_GPS_STARTS
#define DAY_GPS_STARTS 722820
#endif

/*
 * -------------------------------------------------------------------
 * nmea_init - initialise data
 *
 * calculates a few runtime constants that cannot be made compile time
 * constants.
 * -------------------------------------------------------------------
 */
void
wrapdate_init(void)
{
	struct calendar date;

	/* - calculate min. base value for GPS epoch & century unfolding
	 * This assumes that the build system was roughly in sync with
	 * the world, and that really synchronising to a time before the
	 * program was created would be unsafe or insane. If the build
	 * date cannot be established, at least use the start of GPS
	 * (1980-01-06) as minimum, because GPS can surely NOT
	 * synchronise beyond it's own big bang. We add a little safety
	 * margin for the fuzziness of the build date, which is in an
	 * undefined time zone. */
	if (ntpcal_get_build_date(&date))
		g_gpsMinBase = ntpcal_date_to_rd(&date) - 2;
	else
		g_gpsMinBase = 0;

	if (g_gpsMinBase < DAY_GPS_STARTS)
		g_gpsMinBase = DAY_GPS_STARTS;

	ntpcal_rd_to_date(&date, g_gpsMinBase);
	g_gpsMinYear  = date.year;
	g_gpsMinBase -= DAY_NTP_STARTS;
}

/*
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
 *
 * returns true on success, false on failure
 * -------------------------------------------------------------------
 */
bool
unfold_day(
	struct calendar * jd,
	uint32_t		  rec_ui
	)
{
	time64_t	     rec_qw;
	ntpcal_split rec_ds;

	/*
	 * basically this is the periodic extension of the receive
	 * time - 12hrs to the time-of-day with a period of 1 day.
	 * But we would have to execute this in 64bit arithmetic, and we
	 * cannot assume we can do this; therefore this is done
	 * in split representation.
	 */
	rec_qw = ntpcal_ntp_to_ntp(rec_ui - SECSPERDAY/2, time(NULL));
	rec_ds = ntpcal_daysplit(rec_qw);
	rec_ds.lo = ntpcal_periodic_extend(rec_ds.lo,
					   ntpcal_date_to_daysec(jd),
					   SECSPERDAY);
	rec_ds.hi += ntpcal_daysec_to_date(jd, rec_ds.lo);
	/* -1 return means calculation overflowed */
	return (ntpcal_rd_to_date(jd, rec_ds.hi + DAY_NTP_STARTS) >= 0);
}

/*
 * -------------------------------------------------------------------
 * A 2-digit year is expanded into full year spec around the year found
 * in 'jd->year'. This should be in +79/-19 years around the system time,
 * or the result will be off by 100 years.  The asymmetric behaviour was
 * chosen to enable initial sync for systems that do not have a
 * battery-backup clock and start with a date that is typically years in
 * the past.
 *
 * Since the GPS epoch starts at 1980-01-06, the resulting year will be
 * not be before 1980 in any case.
 *
 * returns true on success, false on failure
 * -------------------------------------------------------------------
 */
bool
unfold_century(
	struct calendar * jd,
	uint32_t		  rec_ui
	)
{
	struct calendar rec;
	int32_t		baseyear;

	ntpcal_ntp_to_date(&rec, rec_ui, time(NULL));
	baseyear = rec.year - 20;
	if (baseyear < g_gpsMinYear)
		baseyear = g_gpsMinYear;
	jd->year = (unsigned short)ntpcal_periodic_extend(baseyear, jd->year,
						   100);

	return ((baseyear <= jd->year) && (baseyear + 100 > jd->year));
}

/*
 * -------------------------------------------------------------------
 * A 2-digit year is expanded into a full year spec by correlation with
 * a GPS week number and the current leap second count.
 *
 * The GPS week time scale counts weeks since Sunday, 1980-01-06, modulo
 * 1024 and seconds since start of the week. The GPS time scale is based
 * on international atomic time (TAI), so the leap second difference to
 * UTC is also needed for a proper conversion.
 *
 * A brute-force analysis (that is, test for every date) shows that a
 * wrong assignment of the century can not happen between the years 1900
 * to 2399 when comparing the week signatures for different
 * centuries. (I *think* that will not happen for 400*1024 years, but I
 * have no valid proof. -*-perlinger@ntp.org-*-)
 *
 * This function is bound to work between years 1980 and 2399
 * (inclusive), which should suffice for now ;-)
 *
 * Note: This function needs a full date&time spec on input due to the
 * necessary leap second corrections!
 *
 * returns true on success, false on failure
 * -------------------------------------------------------------------
 */
bool
gpsfix_century(
	struct calendar  * jd,
	const gps_weektm * wd,
	unsigned short   * century
	)
{
	int32_t	days;
	int32_t	doff;
	unsigned short week;
	unsigned short year;
	int     loop;

	/* Get day offset. Assumes that the input time is in range and
	 * that the leap seconds do not shift more than +/-1 day.
	 */
	doff = ntpcal_date_to_daysec(jd) + wd->wt_leap;
	doff = (doff >= SECSPERDAY) - (doff < 0);

	/*
	 * Loop over centuries to get a match, starting with the last
	 * successful one. (Or with the 19th century if the cached value
	 * is out of range...)
	 */
	year = jd->year % 100;
	for (loop = 5; loop > 0; loop--,(*century)++) {
		if (*century < 19 || *century >= 24) {
			*century = 19;
		}
		/* Get days and week in GPS epoch */
		jd->year = year + *century * 100;
		days = ntpcal_date_to_rd(jd) - DAY_GPS_STARTS + doff;
		week = (days / 7) % 1024;
		if (days >= 0 && wd->wt_week == week)
			return true; /* matched... */
	}

	jd->year = year;
	return false; /* match failed... */
}

/*
 * -------------------------------------------------------------------
 * And now the final exercise: Considering the fact that many (most?)
 * GPS receivers cannot handle a GPS epoch wrap well, we try to
 * compensate for that problem by unwrapping a GPS epoch around the
 * receive stamp. Another exercise in periodic unfolding, of course,
 * but with enough points to take care of.
 *
 * Note: The integral part of 'tofs' is intended to handle small(!)
 * systematic offsets, as -1 for handling $GPZDG, which gives the
 * following second. (sigh...) The absolute value shall be less than a
 * day (86400 seconds).
 * -------------------------------------------------------------------
 */
l_fp
eval_gps_time(
	const char            * clockname,      /* for logging */
	const struct calendar * gpst,           /* GPS time stamp  */
	const struct timespec * tofs,           /* GPS frac second & offset */
	const bool            trusted,          /* do we fully trust dates from this GPS? */
	short                 *epoch_warp,      /* for logging */
	const l_fp            * xrecv           /* receive time stamp */
	)
{

	l_fp    retv;

	/* components of calculation */
	int32_t rcv_sec, rcv_day; /* receive ToD and day */
	int32_t gps_sec, gps_day; /* GPS ToD and day in NTP epoch */
	int32_t adj_day, weeks;   /* adjusted GPS day and week shift */

	/* some temporaries to shuffle data */
	time64_t       vi64;
	ntpcal_split rs64;

	/* evaluate time stamp from receiver. */
	gps_sec = ntpcal_date_to_daysec(gpst);
	gps_day = ntpcal_date_to_rd(gpst) - DAY_NTP_STARTS;

	/* merge in fractional offset */
	retv = tspec_intv_to_lfp(*tofs);
	gps_sec += lfpsint(retv);

	/* If we fully trust the GPS receiver, just combine days and
	 * seconds and be done. */
	if (trusted) {
		setlfpuint(retv, time64lo(ntpcal_dayjoin(gps_day, gps_sec)));
		return retv;
	}

	/* So we do not trust the GPS receiver to deliver a correct date
	 * due to the GPS epoch changes. We map the date from the
	 * receiver into the +/-512 week interval around the receive
	 * time in that case. This would be a tad easier with 64bit
	 * calculations, but again, we restrict the code to 32bit ops
	 * when possible. */

	/* - make sure the GPS fractional day is normalised
	 * Applying the offset value might have put us slightly over the
	 * edge of the allowed range for seconds-of-day. Doing a full
	 * division with floor correction is overkill here; a simple
	 * addition or subtraction step is sufficient. Using WHILE loops
	 * gives the right result even if the offset exceeds one day,
	 * which is NOT what it's intended for! */
	while (gps_sec >= SECSPERDAY) {
		gps_sec -= SECSPERDAY;
		gps_day += 1;
	}
	while (gps_sec < 0) {
		gps_sec += SECSPERDAY;
		gps_day -= 1;
	}

	/* - get unfold base: day of full recv time - 512 weeks */
	vi64 = ntpcal_ntp_to_ntp(lfpuint(*xrecv), time(NULL));
	rs64 = ntpcal_daysplit(vi64);
	rcv_sec = rs64.lo;
	rcv_day = rs64.hi - 512 * 7;

	/* - take the fractional days into account
	 * If the fractional day of the GPS time is smaller than the
	 * fractional day of the receive time, we shift the base day for
	 * the unfold by 1. */
	if (   gps_sec  < rcv_sec
	       || (gps_sec == rcv_sec && lfpfrac(retv) < lfpfrac(*xrecv)))
		rcv_day += 1;

	/* - don't warp ahead of GPS invention! */
	if (rcv_day < g_gpsMinBase)
		rcv_day = g_gpsMinBase;

	/* - let the magic happen: */
	adj_day = ntpcal_periodic_extend(rcv_day, gps_day, 1024*7);

	/* - check if we should log a GPS epoch warp */
	weeks = (adj_day - gps_day) / 7;
	if (weeks != *epoch_warp) {
		*epoch_warp = (short)weeks;
		LOGIF(CLOCKINFO, (LOG_INFO,
				  "%s Changed GPS epoch warp to %d weeks",
				  clockname, weeks));
	}

	/* - build result and be done */
	setlfpuint(retv, time64lo(ntpcal_dayjoin(adj_day, gps_sec)));
	return retv;
}

// end
