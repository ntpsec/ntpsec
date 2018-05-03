#include <stdarg.h>

#include "config.h"

#include "unity.h"
#include "unity_fixture.h"
#include "caltime.h"

#include "ntp.h"
#include "ntpd.h"
#include "ntp_calendar.h"
#include "ntp_leapsec.h"


TEST_GROUP(leapsec);

static time_t fixedpivot;

TEST_SETUP(leapsec) {
	fixedpivot = settime(1970, 1, 1, 0, 0, 0);
	leapsec_ut_pristine();
}

TEST_TEAR_DOWN(leapsec) {
}

static const char leap1 [] =
    "#\n"
    "#@ 	3610569600\n"
    "#\n"
    "2272060800 10	# 1 Jan 1972\n"
    "2287785600	11	# 1 Jul 1972\n"
    "2303683200	12	# 1 Jan 1973\n"
    "2335219200	13	# 1 Jan 1974\n"
    "2366755200	14	# 1 Jan 1975\n"
    "2398291200	15	# 1 Jan 1976\n"
    "2429913600	16	# 1 Jan 1977\n"
    "2461449600	17	# 1 Jan 1978\n"
    "2492985600	18	# 1 Jan 1979\n"
    "2524521600	19	# 1 Jan 1980\n"
    "   \t  \n"
    "2571782400	20	# 1 Jul 1981\n"
    "2603318400	21	# 1 Jul 1982\n"
    "2634854400	22	# 1 Jul 1983\n"
    "2698012800	23	# 1 Jul 1985\n"
    "2776982400	24	# 1 Jan 1988\n"
    "2840140800	25	# 1 Jan 1990\n"
    "2871676800	26	# 1 Jan 1991\n"
    "2918937600	27	# 1 Jul 1992\n"
    "2950473600	28	# 1 Jul 1993\n"
    "2982009600	29	# 1 Jul 1994\n"
    "3029443200	30	# 1 Jan 1996\n"
    "3076704000	31	# 1 Jul 1997\n"
    "3124137600	32	# 1 Jan 1999\n"
    "3345062400	33	# 1 Jan 2006\n"
    "3439756800	34	# 1 Jan 2009\n"
    "3550089600	35	# 1 Jul 2012\n"
    "#\n"
    "#h	dc2e6b0b 5aade95d a0587abd 4e0dacb4 e4d5049e\n"
    "#\n";

static const char leap2 [] =
    "#\n"
    "#@ 	2950473700\n"
    "#\n"
    "2272060800 10	# 1 Jan 1972\n"
    "2287785600	11	# 1 Jul 1972\n"
    "2303683200	12	# 1 Jan 1973\n"
    "2335219200	13	# 1 Jan 1974\n"
    "2366755200	14	# 1 Jan 1975\n"
    "2398291200	15	# 1 Jan 1976\n"
    "2429913600	16	# 1 Jan 1977\n"
    "2461449600	17	# 1 Jan 1978\n"
    "2492985600	18	# 1 Jan 1979\n"
    "2524521600	19	# 1 Jan 1980\n"
    "2571782400	20	# 1 Jul 1981\n"
    "2603318400	21	# 1 Jul 1982\n"
    "2634854400	22	# 1 Jul 1983\n"
    "2698012800	23	# 1 Jul 1985\n"
    "2776982400	24	# 1 Jan 1988\n"
    "2840140800	25	# 1 Jan 1990\n"
    "2871676800	26	# 1 Jan 1991\n"
    "2918937600	27	# 1 Jul 1992\n"
    "2950473600	28	# 1 Jul 1993\n"
    "#\n";

// Faked table with a leap second removal at 2009
static const char leap3 [] =
    "#\n"
    "#@ 	3610569600\n"
    "#\n"
    "2272060800 10	# 1 Jan 1972\n"
    "2287785600	11	# 1 Jul 1972\n"
    "2303683200	12	# 1 Jan 1973\n"
    "2335219200	13	# 1 Jan 1974\n"
    "2366755200	14	# 1 Jan 1975\n"
    "2398291200	15	# 1 Jan 1976\n"
    "2429913600	16	# 1 Jan 1977\n"
    "2461449600	17	# 1 Jan 1978\n"
    "2492985600	18	# 1 Jan 1979\n"
    "2524521600	19	# 1 Jan 1980\n"
    "2571782400	20	# 1 Jul 1981\n"
    "2603318400	21	# 1 Jul 1982\n"
    "2634854400	22	# 1 Jul 1983\n"
    "2698012800	23	# 1 Jul 1985\n"
    "2776982400	24	# 1 Jan 1988\n"
    "2840140800	25	# 1 Jan 1990\n"
    "2871676800	26	# 1 Jan 1991\n"
    "2918937600	27	# 1 Jul 1992\n"
    "2950473600	28	# 1 Jul 1993\n"
    "2982009600	29	# 1 Jul 1994\n"
    "3029443200	30	# 1 Jan 1996\n"
    "3076704000	31	# 1 Jul 1997\n"
    "3124137600	32	# 1 Jan 1999\n"
    "3345062400	33	# 1 Jan 2006\n"
    "3439756800	32	# 1 Jan 2009\n"
    "3550089600	33	# 1 Jul 2012\n"
    "#\n";

// short table with good hash
static const char leap_ghash [] =
    "#\n"
    "#@ 	3610569600\n"
    "#$ 	3610566000\n"
    "#\n"
    "2272060800 10	# 1 Jan 1972\n"
    "2287785600	11	# 1 Jul 1972\n"
    "2303683200	12	# 1 Jan 1973\n"
    "2335219200	13	# 1 Jan 1974\n"
    "2366755200	14	# 1 Jan 1975\n"
    "2398291200	15	# 1 Jan 1976\n"
    "2429913600	16	# 1 Jan 1977\n"
    "2461449600	17	# 1 Jan 1978\n"
    "2492985600	18	# 1 Jan 1979\n"
    "2524521600	19	# 1 Jan 1980\n"
    "#\n"
    "#h 4b304e10 95642b3f c10b91f9 90791725 25f280d0\n"
    "#\n";

// short table with bad hash
static const char leap_bhash [] =
    "#\n"
    "#@ 	3610569600\n"
    "#$ 	3610566000\n"
    "#\n"
    "2272060800 10	# 1 Jan 1972\n"
    "2287785600	11	# 1 Jul 1972\n"
    "2303683200	12	# 1 Jan 1973\n"
    "2335219200	13	# 1 Jan 1974\n"
    "2366755200	14	# 1 Jan 1975\n"
    "2398291200	15	# 1 Jan 1976\n"
    "2429913600	16	# 1 Jan 1977\n"
    "2461449600	17	# 1 Jan 1978\n"
    "2492985600	18	# 1 Jan 1979\n"
    "2524521600	19	# 1 Jan 1980\n"
    "#\n"
    "#h	dc2e6b0b 5aade95d a0587abd 4e0dacb4 e4d5049e\n"
    "#\n";

// short table with malformed hash
static const char leap_mhash [] =
    "#\n"
    "#@ 	3610569600\n"
    "#$ 	3610566000\n"
    "#\n"
    "2272060800 10	# 1 Jan 1972\n"
    "2287785600	11	# 1 Jul 1972\n"
    "2303683200	12	# 1 Jan 1973\n"
    "2335219200	13	# 1 Jan 1974\n"
    "2366755200	14	# 1 Jan 1975\n"
    "2398291200	15	# 1 Jan 1976\n"
    "2429913600	16	# 1 Jan 1977\n"
    "2461449600	17	# 1 Jan 1978\n"
    "2492985600	18	# 1 Jan 1979\n"
    "2524521600	19	# 1 Jan 1980\n"
    "#\n"
    "#h f2349a02 788b9534 a8f2e141 f2029Q6d 4064a7ee\n"
    "#\n";

// short table with only 4 hash groups
static const char leap_shash [] =
    "#\n"
    "#@ 	3610569600\n"
    "#$ 	3610566000\n"
    "#\n"
    "2272060800 10	# 1 Jan 1972\n"
    "2287785600	11	# 1 Jul 1972\n"
    "2303683200	12	# 1 Jan 1973\n"
    "2335219200	13	# 1 Jan 1974\n"
    "2366755200	14	# 1 Jan 1975\n"
    "2398291200	15	# 1 Jan 1976\n"
    "2429913600	16	# 1 Jan 1977\n"
    "2461449600	17	# 1 Jan 1978\n"
    "2492985600	18	# 1 Jan 1979\n"
    "2524521600	19	# 1 Jan 1980\n"
    "#\n"
    "#h f2349a02 788b9534 a8f2e141 f2029Q6d\n"
    "#\n";

// table with good hash and truncated/missing leading zeros
static const char leap_gthash [] = {
    "#\n"
    "#$	 3535228800\n"
    "#\n"
    "#	Updated through IERS Bulletin C46\n"
    "#	File expires on:  28 June 2014\n"
    "#\n"
    "#@	3612902400\n"
    "#\n"
    "2272060800	10	# 1 Jan 1972\n"
    "2287785600	11	# 1 Jul 1972\n"
    "2303683200	12	# 1 Jan 1973\n"
    "2335219200	13	# 1 Jan 1974\n"
    "2366755200	14	# 1 Jan 1975\n"
    "2398291200	15	# 1 Jan 1976\n"
    "2429913600	16	# 1 Jan 1977\n"
    "2461449600	17	# 1 Jan 1978\n"
    "2492985600	18	# 1 Jan 1979\n"
    "2524521600	19	# 1 Jan 1980\n"
    "2571782400	20	# 1 Jul 1981\n"
    "2603318400	21	# 1 Jul 1982\n"
    "2634854400	22	# 1 Jul 1983\n"
    "2698012800	23	# 1 Jul 1985\n"
    "2776982400	24	# 1 Jan 1988\n"
    "2840140800	25	# 1 Jan 1990\n"
    "2871676800	26	# 1 Jan 1991\n"
    "2918937600	27	# 1 Jul 1992\n"
    "2950473600	28	# 1 Jul 1993\n"
    "2982009600	29	# 1 Jul 1994\n"
    "3029443200	30	# 1 Jan 1996\n"
    "3076704000	31	# 1 Jul 1997\n"
    "3124137600	32	# 1 Jan 1999\n"
    "3345062400	33	# 1 Jan 2006\n"
    "3439756800	34	# 1 Jan 2009\n"
    "3550089600	35	# 1 Jul 2012\n"
    "#\n"
    "#h	1151a8f e85a5069 9000fcdb 3d5e5365 1d505b37"
};

static time_t lsec2009 = 3439756800u - JAN_1970; // 1 Jan 2009, 00:00:00 utc
static time_t lsec2012 = 3550089600u - JAN_1970; // 1 Jul 2012, 00:00:00 utc

static int stringreader(void* farg)
{
	const char ** cpp = (const char**)farg;
	if (**cpp)
		return *(*cpp)++;
	else
		return EOF;
}

static bool
setup_load_table(
	const char * cp)
{
	bool            rc;
	leap_table_t * pt = leapsec_get_table(0);
	rc = (pt != NULL) && leapsec_load(pt, stringreader, &cp);
	rc = rc && leapsec_set_table(pt);
	return rc;
}

static bool
setup_clear_table(void)
{
	bool            rc;
	leap_table_t * pt = leapsec_get_table(0);
	if (pt)
		leapsec_clear(pt);
	rc = leapsec_set_table(pt);
	return rc;
}


/*
std::string CalendarToString(const calendar &cal) {
	std::ostringstream ss;
	ss << cal.year << "-" << (unsigned int)cal.month << "-" << (unsigned int)cal.monthday
	   << " (" << cal.yearday << ") " << (unsigned int)cal.hour << ":"
	   << (unsigned int)cal.minute << ":" << (unsigned int)cal.second;
	return ss.str();
}
bool IsEqual(const calendar &expected, const calendar &actual) {
	if (expected.year == actual.year &&
		(expected.yearday == actual.yearday ||
		 (expected.month == actual.month &&
		  expected.monthday == actual.monthday)) &&
		expected.hour == actual.hour &&
		expected.minute == actual.minute &&
		expected.second == actual.second) {
		return true;
	} else {
		return false
			<< "expected: " << CalendarToString(expected) << " but was "
			<< CalendarToString(actual);
	}
}
*/


// =====================================================================
// VALIDATION TESTS
// =====================================================================

// ----------------------------------------------------------------------
TEST(leapsec, ValidateGood) {
	const char *cp = leap_ghash;
	int         rc = leapsec_validate(stringreader, &cp);
	TEST_ASSERT_EQUAL(LSVALID_GOODHASH, rc);
}

// ----------------------------------------------------------------------
TEST(leapsec, ValidateNoHash) {
	const char *cp = leap2;
	int         rc = leapsec_validate(stringreader, &cp);
	TEST_ASSERT_EQUAL(LSVALID_NOHASH, rc);
}

// ----------------------------------------------------------------------
TEST(leapsec, ValidateBad) {
	const char *cp = leap_bhash;
	int         rc = leapsec_validate(stringreader, &cp);
	TEST_ASSERT_EQUAL(LSVALID_BADHASH, rc);
}

// ----------------------------------------------------------------------
TEST(leapsec, ValidateMalformed) {
	const char *cp = leap_mhash;
	int         rc = leapsec_validate(stringreader, &cp);
	TEST_ASSERT_EQUAL(LSVALID_BADFORMAT, rc);
}

// ----------------------------------------------------------------------
TEST(leapsec, ValidateMalformedShort) {
	const char *cp = leap_shash;
	int         rc = leapsec_validate(stringreader, &cp);
	TEST_ASSERT_EQUAL(LSVALID_BADFORMAT, rc);
}

// ----------------------------------------------------------------------
TEST(leapsec, ValidateNoLeadZero) {
	const char *cp = leap_gthash;
	int         rc = leapsec_validate(stringreader, &cp);
	TEST_ASSERT_EQUAL(LSVALID_GOODHASH, rc);
}

// =====================================================================
// BASIC FUNCTIONS
// =====================================================================

// ----------------------------------------------------------------------
// test table selection
TEST(leapsec, tableSelect) {
	leap_table_t *pt1, *pt2, *pt3;

	pt1 = leapsec_get_table(0);
	pt2 = leapsec_get_table(0);
	TEST_ASSERT_POINTERS_EQUAL(pt1, pt2);

	pt1 = leapsec_get_table(1);
	pt2 = leapsec_get_table(1);
	TEST_ASSERT_POINTERS_EQUAL(pt1, pt2);

	pt1 = leapsec_get_table(1);
	pt2 = leapsec_get_table(0);
	TEST_ASSERT_NOT_EQUAL(pt1, pt2);

	pt1 = leapsec_get_table(0);
	pt2 = leapsec_get_table(1);
	TEST_ASSERT_NOT_EQUAL(pt1, pt2);

	leapsec_set_table(pt1);
	pt2 = leapsec_get_table(0);
	pt3 = leapsec_get_table(1);
	TEST_ASSERT_POINTERS_EQUAL(pt1, pt2);
	TEST_ASSERT_NOT_EQUAL(pt2, pt3);

	pt1 = pt3;
	leapsec_set_table(pt1);
	pt2 = leapsec_get_table(0);
	pt3 = leapsec_get_table(1);
	TEST_ASSERT_POINTERS_EQUAL(pt1, pt2);
	TEST_ASSERT_NOT_EQUAL(pt2, pt3);
}

// ----------------------------------------------------------------------
// load file & check expiration
TEST(leapsec, loadFileExpire) {
	const char *cp = leap1;
	int rc;
	leap_table_t * pt = leapsec_get_table(0);

	rc =   leapsec_load(pt, stringreader, &cp)
	    && leapsec_set_table(pt);
	TEST_ASSERT_EQUAL(1, rc);
	rc = leapsec_expired(3439756800u - JAN_1970);
	TEST_ASSERT_EQUAL(0, rc);
	rc = leapsec_expired(3610569601u - JAN_1970);
	TEST_ASSERT_EQUAL(1, rc);
}

// ----------------------------------------------------------------------
// load file & check time-to-live
TEST(leapsec, loadFileTTL) {
	const char *cp = leap1;
	int rc;
	leap_table_t * pt = leapsec_get_table(0);

	const time_t limit = 3610569600u - JAN_1970;

	rc =   leapsec_load(pt, stringreader, &cp)
	    && leapsec_set_table(pt);
	TEST_ASSERT_EQUAL(1, rc);

	// exactly 1 day to live
	rc = leapsec_daystolive(limit - 86400);
	TEST_ASSERT_EQUAL( 1, rc);
	// less than 1 day to live
	rc = leapsec_daystolive(limit - 86399);
	TEST_ASSERT_EQUAL( 0, rc);
	// hit expiration exactly
	rc = leapsec_daystolive(limit);
	TEST_ASSERT_EQUAL( 0, rc);
	// expired since 1 sec
	rc = leapsec_daystolive(limit + 1);
	TEST_ASSERT_EQUAL(-1, rc);
}


// Hack to avoid compiler warnings from gcc 8.0
// We should be able to cast fprintf, but that gets:
// ../../tests/ntpd/leapsec.c: In function ‘TEST_leapsec_lsQueryPristineState_’:
// ../../tests/ntpd/leapsec.c:434:19: warning: cast between incompatible function types from ‘int (*)(FILE * restrict,  const char * restrict,  ...)’ {aka ‘int (*)(struct _IO_FILE * restrict,  const char * restrict,  ...)’} to ‘void (*)(void *, const char *, ...)’ [-Wcast-function-type]
//   leapsec_dump(pt, (leapsec_dumper)fprintf, stdout);
static void
my_fprintf(FILE *stream, const char *fmt, ...) {
	va_list  ap;

	va_start(ap, fmt);
	vfprintf(stream, fmt, ap);
	va_end(ap);

};


// ----------------------------------------------------------------------
// test query in pristine state (bug#2745 misbehaviour)
TEST(leapsec, lsQueryPristineState) {
	int            rc;
	leap_result_t  qr;

	leap_table_t * pt = leapsec_get_table(0);
	leapsec_dump(pt, (leapsec_dumper)my_fprintf, stdout);

	rc = leapsec_query(&qr, lsec2012);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
}


// ----------------------------------------------------------------------
// ad-hoc jump: leap second at 2009.01.01 -60days
TEST(leapsec, ls2009faraway) {
  	bool           rc;
	leap_result_t  qr;

	rc = setup_load_table(leap1);
	TEST_ASSERT_EQUAL(1, rc);

	// test 60 days before leap. Nothing scheduled or indicated.
	rc = leapsec_query(&qr, lsec2009 - 60*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(33, qr.tai_offs);
	TEST_ASSERT_EQUAL(0,  qr.tai_diff);
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
}

// ----------------------------------------------------------------------
// ad-hoc jump: leap second at 2009.01.01 -1week
TEST(leapsec, ls2009weekaway) {
	bool           rc;
	leap_result_t  qr;

	rc = setup_load_table(leap1);
	TEST_ASSERT_EQUAL(1, rc);

	// test 7 days before leap. Leap scheduled, but not yet indicated.
	rc = leapsec_query(&qr, lsec2009 - 7*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(33, qr.tai_offs);
	TEST_ASSERT_EQUAL(1,  qr.tai_diff);
	TEST_ASSERT_EQUAL(LSPROX_SCHEDULE, qr.proximity);
}

// ----------------------------------------------------------------------
// ad-hoc jump: leap second at 2009.01.01 -1hr
TEST(leapsec, ls2009houraway) {
	bool           rc;
	leap_result_t  qr;

	rc = setup_load_table(leap1);
	TEST_ASSERT_EQUAL(1, rc);

	// test 1 hour before leap. 61 true seconds to go.
	rc = leapsec_query(&qr, lsec2009 - SECSPERHR);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(33, qr.tai_offs);
	TEST_ASSERT_EQUAL(1,  qr.tai_diff);
	TEST_ASSERT_EQUAL(LSPROX_ANNOUNCE, qr.proximity);
}

// ----------------------------------------------------------------------
// ad-hoc jump: leap second at 2009.01.01 -1sec
TEST(leapsec, ls2009secaway) {
	bool           rc;
	leap_result_t  qr;

	rc = setup_load_table(leap1);
	TEST_ASSERT_TRUE(rc);

	// test 1 second before leap (last boundary...) 2 true seconds to go.
	rc = leapsec_query(&qr, lsec2009 - 1);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(33, qr.tai_offs);
	TEST_ASSERT_EQUAL(1,  qr.tai_diff);
	TEST_ASSERT_EQUAL(LSPROX_ALERT, qr.proximity);
}

// ----------------------------------------------------------------------
// ad-hoc jump to leap second at 2009.01.01
TEST(leapsec, ls2009onspot) {
	bool           rc;
	leap_result_t  qr;

	rc = setup_load_table(leap1);
	TEST_ASSERT_TRUE(rc);

	// test on-spot: treat leap second as already gone.
	rc = leapsec_query(&qr, lsec2009);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(34, qr.tai_offs);
	TEST_ASSERT_EQUAL(0,  qr.tai_diff);
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
}

// ----------------------------------------------------------------------
// test handling of the leap second at 2009.01.01 without table
TEST(leapsec, ls2009nodata) {
	bool           rc;
	leap_result_t  qr;

	rc = setup_clear_table();
	TEST_ASSERT_TRUE(rc);

	// test on-spot with empty table
	rc = leapsec_query(&qr, lsec2009);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,  qr.tai_offs);
	TEST_ASSERT_EQUAL(0,  qr.tai_diff);
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
}

// ----------------------------------------------------------------------
// test handling of the leap second at 2009.01.01 with culled data
TEST(leapsec, ls2009limdata) {
	leap_table_t * pt;
	bool           rc;
	leap_result_t  qr;

	rc = setup_load_table(leap1);
	pt = leapsec_get_table(0);
	leapsec_dump(pt, (leapsec_dumper)my_fprintf, stdout);

	// FIXME
	// This used to check against build date
	// That updated the header but didn't add slots.
	// So the last slot was the only answer it could return.

	TEST_ASSERT_TRUE(rc);

	// test on-spot with limited table
	rc = leapsec_query(&qr, lsec2009+10);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(34, qr.tai_offs);
	// TEST_ASSERT_EQUAL(35, qr.tai_offs);
	TEST_ASSERT_EQUAL(0,  qr.tai_diff);
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
}

// ----------------------------------------------------------------------
// add dynamic leap second (like from peer/clock)
TEST(leapsec, addDynamic) {
	leap_table_t * pt;
	bool           rc;
	int            idx;

	/* Explicit (time_t) cast avoids warnings
	 * 32 bit FreeBSD 11.1, Nov 2017
	 * FreeBSD clang version 4.0.0
../../tests/ntpd/leapsec.c:582:3: warning: implicit conversion from 'long long' to 'time_t' (aka 'int') changes value from 2982009600 to -1312957696 [-Wconstant-conversion]
                2982009600,     //      29      # 1 Jul 1994
                ^~~~~~~~~~
*/
	static const time_t insns[] = {
		(time_t)2982009600,	//	29	# 1 Jul 1994
		(time_t)3029443200,	//	30	# 1 Jan 1996
		(time_t)3076704000,	//	31	# 1 Jul 1997
		(time_t)3124137600,	//	32	# 1 Jan 1999
		(time_t)3345062400,	//	33	# 1 Jan 2006
		(time_t)3439756800,	//	34	# 1 Jan 2009
		(time_t)3550089600,	//	35	# 1 Jul 2012
		0 // sentinel
	};

	rc = setup_load_table(leap2);
	TEST_ASSERT_TRUE(rc);

	for (idx=1; insns[idx]; ++idx) {
	    rc = leapsec_add_dyn(true,
                                 insns[idx] - (time_t)JAN_1970 -
                                 20*SECSPERDAY - 100);
	    TEST_ASSERT_TRUE(rc);
	}
	// try to slip in a previous entry
	rc = leapsec_add_dyn(true, insns[0] - (time_t)JAN_1970 -
                             20*SECSPERDAY - 100);
	TEST_ASSERT_FALSE(rc);
	//leapsec_dump(pt, (leapsec_dumper)my_fprintf, stdout);
	pt = leapsec_get_table(0);
	leapsec_dump(pt, (leapsec_dumper)my_fprintf, stdout);
}

// ----------------------------------------------------------------------
// add fixed leap seconds (like from network packet)
TEST(leapsec, addFixed) {
	bool           rc;
	int            idx;

	static const struct { time_t tt; int of; } insns[] = {
		{2982009600-JAN_1970, 29},//	# 1 Jul 1994
		{3029443200-JAN_1970, 30},//	# 1 Jan 1996
		{3076704000-JAN_1970, 31},//	# 1 Jul 1997
		{3124137600-JAN_1970, 32},//	# 1 Jan 1999
		{3345062400-JAN_1970, 33},//	# 1 Jan 2006
		{3439756800-JAN_1970, 34},//	# 1 Jan 2009
		{3550089600-JAN_1970, 35},//	# 1 Jul 2012
		{0,0} // sentinel
	};

	rc = setup_load_table(leap2);
	TEST_ASSERT_TRUE(rc);

	// try to get in BAD time stamps...
	for (idx=0; insns[idx].tt; ++idx) {
	    rc = leapsec_add_fix(
		insns[idx].of,
		insns[idx].tt - 20*SECSPERDAY - 100,
		insns[idx].tt + SECSPERDAY);
#ifdef ENABLE_LEAP_TESTING
		TEST_ASSERT_TRUE(rc);
#else
		TEST_ASSERT_FALSE(rc);
#endif
	}
	// no do it right
	for (idx=0; insns[idx].tt; ++idx) {
		rc = leapsec_add_fix(
		    insns[idx].of,
		    insns[idx].tt,
		    insns[idx].tt + SECSPERDAY);
#ifdef ENABLE_LEAP_TESTING
		TEST_ASSERT_FALSE(rc);
#else
		TEST_ASSERT_TRUE(rc);
#endif
	}
	// try to slip in a previous entry
	rc = leapsec_add_fix(
	    insns[0].of,
	    insns[0].tt,
	    insns[0].tt + SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	//leapsec_dump(pt, (leapsec_dumper)my_fprintf, stdout);
}

// =====================================================================
// SEQUENCE TESTS
// =====================================================================

// ----------------------------------------------------------------------
// leap second insert at 2009.01.01, electric mode
TEST(leapsec, ls2009seqInsElectric) {
	bool           rc;
	leap_result_t  qr;

	rc = setup_load_table(leap1);
	TEST_ASSERT_TRUE(rc);
	leapsec_electric(1);
	TEST_ASSERT_TRUE(leapsec_electric(-1));

	rc = leapsec_query(&qr, lsec2009 - 60*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - 7*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_SCHEDULE, qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - SECSPERHR);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ANNOUNCE, qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - 1);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ALERT,    qr.proximity);

	rc = leapsec_query(&qr, lsec2009);
	TEST_ASSERT_TRUE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);

	// second call, same time frame: no trigger!
	rc = leapsec_query(&qr, lsec2009);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
}

// ----------------------------------------------------------------------
// leap second insert at 2009.01.01, dumb mode
TEST(leapsec, ls2009seqInsDumb) {
	bool           rc;
	leap_result_t  qr;

	rc = setup_load_table(leap1);
	TEST_ASSERT_TRUE(rc);
	TEST_ASSERT_FALSE(leapsec_electric(-1));

	rc = leapsec_query(&qr, lsec2009 - 60*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - 7*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_SCHEDULE, qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - SECSPERHR);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ANNOUNCE, qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - 1);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ALERT,    qr.proximity);

	rc = leapsec_query(&qr, lsec2009);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ALERT,    qr.proximity);

	rc = leapsec_query(&qr, lsec2009+1);
	TEST_ASSERT_TRUE(rc)
	TEST_ASSERT_EQUAL(-1,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);

	// second call, same time frame: no trigger!
	rc = leapsec_query(&qr, lsec2009);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
}


// ----------------------------------------------------------------------
// fake leap second remove at 2009.01.01, electric mode
TEST(leapsec, ls2009seqDelElectric) {
	bool           rc;
	leap_result_t  qr;

	rc = setup_load_table(leap3);
	TEST_ASSERT_TRUE(rc);
	leapsec_electric(1);
	TEST_ASSERT_TRUE(leapsec_electric(-1));

	rc = leapsec_query(&qr, lsec2009 - 60*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - 7*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_SCHEDULE, qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - SECSPERHR);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ANNOUNCE, qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - 1);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ALERT,    qr.proximity);

	rc = leapsec_query(&qr, lsec2009);
	TEST_ASSERT_TRUE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);

	// second call, same time frame: no trigger!
	rc = leapsec_query(&qr, lsec2009);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
}

// ----------------------------------------------------------------------
// fake leap second remove at 2009.01.01. dumb mode
TEST(leapsec, ls2009seqDelDumb) {
	bool           rc;
	leap_result_t  qr;

	rc = setup_load_table(leap3);
	TEST_ASSERT_TRUE(rc);
	TEST_ASSERT_FALSE(leapsec_electric(-1));

	rc = leapsec_query(&qr, lsec2009 - 60*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - 7*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_SCHEDULE, qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - SECSPERHR);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ANNOUNCE, qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - 2);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ALERT,    qr.proximity);

	rc = leapsec_query(&qr, lsec2009 - 1);
	TEST_ASSERT_TRUE(rc);
	TEST_ASSERT_EQUAL(1,              qr.warped);
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);

	// second call, same time frame: no trigger!
	rc = leapsec_query(&qr, lsec2009);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
}

// ----------------------------------------------------------------------
// leap second insert at 2012.07.01, electric mode
TEST(leapsec, ls2012seqInsElectric) {
	bool           rc;
	leap_result_t  qr;

	rc = setup_load_table(leap1);
	TEST_ASSERT_TRUE(rc);
	leapsec_electric(1);
	TEST_ASSERT_TRUE(leapsec_electric(-1));

	rc = leapsec_query(&qr, lsec2012 - 60*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);

	rc = leapsec_query(&qr, lsec2012 - 7*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_SCHEDULE, qr.proximity);

	rc = leapsec_query(&qr, lsec2012 - SECSPERHR);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ANNOUNCE, qr.proximity);

	rc = leapsec_query(&qr, lsec2012 - 1);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ALERT,    qr.proximity);

	rc = leapsec_query(&qr, lsec2012);
	TEST_ASSERT_TRUE(rc);
	TEST_ASSERT_EQUAL(0,            qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);

	// second call, same time frame: no trigger!
	rc = leapsec_query(&qr, lsec2012);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
}

// ----------------------------------------------------------------------
// leap second insert at 2012.07.01, dumb mode
TEST(leapsec, ls2012seqInsDumb) {
	bool           rc;
	leap_result_t  qr;

	rc = setup_load_table(leap1);
	TEST_ASSERT_TRUE(rc);
	TEST_ASSERT_FALSE(leapsec_electric(-1));

	rc = leapsec_query(&qr, lsec2012 - 60*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);

	rc = leapsec_query(&qr, lsec2012 - 7*SECSPERDAY);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_SCHEDULE, qr.proximity);

	rc = leapsec_query(&qr, lsec2012 - SECSPERHR);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ANNOUNCE, qr.proximity);

	rc = leapsec_query(&qr, lsec2012 - 1);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,               qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ALERT,    qr.proximity);

	// This is just 1 sec before transition!
	rc = leapsec_query(&qr, lsec2012);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,            qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_ALERT, qr.proximity);

	// NOW the insert/backwarp must happen
	rc = leapsec_query(&qr, lsec2012+1);
	TEST_ASSERT_TRUE(rc);
	TEST_ASSERT_EQUAL(-1,            qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);

	// second call with transition time: no trigger!
	rc = leapsec_query(&qr, lsec2012);
	TEST_ASSERT_FALSE(rc);
	TEST_ASSERT_EQUAL(0,             qr.warped   );
	TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
}

// ----------------------------------------------------------------------
// test repeated query on empty table in dumb mode
TEST(leapsec, lsEmptyTableDumb) {
	bool           rc;
	leap_result_t  qr;
	time_t       t;
	const time_t t0   = lsec2012 - 10;
	const time_t tE   = lsec2012 + 10;

	TEST_ASSERT_FALSE(leapsec_electric(-1));

	for (t = t0; t != tE; ++t) {
		rc = leapsec_query(&qr, t);
		TEST_ASSERT_FALSE(rc);
		TEST_ASSERT_EQUAL(0,             qr.warped   );
		TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
	}
}

// ----------------------------------------------------------------------
// test repeated query on empty table in electric mode
TEST(leapsec, lsEmptyTableElectric) {
	bool           rc;
	leap_result_t  qr;
	time_t       t;

	leapsec_electric(electric_on);
	TEST_ASSERT(electric_on == leapsec_electric(electric_query));

	const time_t   t0 = lsec2012 - 10;
	const time_t   tE = lsec2012 + 10;

	for (t = t0; t != tE; ++t) {
		rc = leapsec_query(&qr, t);
		TEST_ASSERT_FALSE(rc);
		TEST_ASSERT_EQUAL(0,             qr.warped   );
		TEST_ASSERT_EQUAL(LSPROX_NOWARN, qr.proximity);
	}
}

TEST_GROUP_RUNNER(leapsec) {
	RUN_TEST_CASE(leapsec, ValidateGood);
	RUN_TEST_CASE(leapsec, ValidateNoHash);
	RUN_TEST_CASE(leapsec, ValidateBad);
	RUN_TEST_CASE(leapsec, ValidateMalformed);
	RUN_TEST_CASE(leapsec, ValidateMalformedShort);
	RUN_TEST_CASE(leapsec, ValidateNoLeadZero);
	RUN_TEST_CASE(leapsec, tableSelect);
	RUN_TEST_CASE(leapsec, loadFileExpire);
	RUN_TEST_CASE(leapsec, loadFileTTL);
	RUN_TEST_CASE(leapsec, lsQueryPristineState);
	RUN_TEST_CASE(leapsec, ls2009faraway);
	RUN_TEST_CASE(leapsec, ls2009weekaway);
	RUN_TEST_CASE(leapsec, ls2009houraway);
	RUN_TEST_CASE(leapsec, ls2009secaway);
	RUN_TEST_CASE(leapsec, ls2009onspot);
	RUN_TEST_CASE(leapsec, ls2009nodata);
	RUN_TEST_CASE(leapsec, ls2009limdata);
	RUN_TEST_CASE(leapsec, addDynamic);
	RUN_TEST_CASE(leapsec, addFixed);
	RUN_TEST_CASE(leapsec, ls2009seqInsElectric);
	RUN_TEST_CASE(leapsec, ls2009seqInsDumb);
	RUN_TEST_CASE(leapsec, ls2009seqDelElectric);
	RUN_TEST_CASE(leapsec, ls2009seqDelDumb);
	RUN_TEST_CASE(leapsec, ls2012seqInsElectric);
	RUN_TEST_CASE(leapsec, ls2012seqInsDumb);
	RUN_TEST_CASE(leapsec, lsEmptyTableDumb);
	RUN_TEST_CASE(leapsec, lsEmptyTableElectric);
}
