#include "config.h"

#include "ntp_types.h"
#include "ntp_fp.h"
#include "timespecops.h"
#include "ntp_calendar.h"

#include "unity.h"
#include "unity_fixture.h"

#include <math.h>
#include <string.h>


#define TEST_ASSERT_EQUAL_timespec(a, b) {				\
    TEST_ASSERT_EQUAL_MESSAGE(a.tv_sec, b.tv_sec, "Field tv_sec");	\
    TEST_ASSERT_EQUAL_MESSAGE(a.tv_nsec, b.tv_nsec, "Field tv_nsec");	\
}


#define TEST_ASSERT_EQUAL_l_fp(a, b) {					\
    TEST_ASSERT_EQUAL_MESSAGE(lfpsint(a), lfpsint(b), "Integral part"); \
    TEST_ASSERT_EQUAL_UINT_MESSAGE(lfpfrac(a), lfpfrac(b), "Fractional part"); \
}


static uint32_t my_tick_to_tsf(uint32_t ticks);
static uint32_t my_tsf_to_tick(uint32_t tsf);

// that's it...
struct lfpfracdata {
	long	nsec;
	uint32_t frac;
};

TEST_GROUP(timespecops);

TEST_SETUP(timespecops) {}

TEST_TEAR_DOWN(timespecops) {}

//***************************MY CUSTOM FUNCTIONS***************************

static bool timespec_isValid(struct timespec V)
{
	return V.tv_nsec >= 0 && V.tv_nsec < 1000000000;
}


static struct timespec timespec_init(time_t hi, long lo)
{
	struct timespec V;

	V.tv_sec = hi;
	V.tv_nsec = lo;

	return V;
}


static bool AssertFpClose(const l_fp m, const l_fp n, const l_fp limit)
{
	int64_t diff = (int64_t)(m - n);
	if ((l_fp)llabs(diff) <= limit)
		return true;
	else {
		printf("m_expr which is %s \nand\nn_expr which is %s\nare not close; diff=%susec\n",
                       lfptoa(m, 10), lfptoa(n, 10), lfptoa((l_fp)diff, 10));
		return false;
	}
}


static bool AssertTimespecClose(const struct timespec m, const struct timespec n,
				const struct timespec limit)
{
	struct timespec diff;

	diff = abs_tspec(sub_tspec(m, n));
	if (cmp_tspec(limit, diff) >= 0)
		return true;
	else
	{
		printf("m_expr which is %ld.%ld \nand\nn_expr which is %ld.%ld\nare not close; diff=%ld.%ldnsec\n",
		       (long)m.tv_sec, m.tv_nsec,
		       (long)n.tv_sec, n.tv_nsec,
		       (long)diff.tv_sec, diff.tv_nsec);
		return false;
	}
}

//-----------------------------------------------

static const struct lfpfracdata fdata[] = {
	{	  0, 0x00000000 }, {   2218896, 0x00916ae6 },
	{  16408100, 0x0433523d }, { 125000000, 0x20000000 },
	{ 250000000, 0x40000000 }, { 287455871, 0x4996b53d },
	{ 375000000, 0x60000000 }, { 500000000, 0x80000000 },
	{ 518978897, 0x84dbcd0e }, { 563730222, 0x90509fb3 },
	{ 563788007, 0x9054692c }, { 583289882, 0x95527c57 },
	{ 607074509, 0x9b693c2a }, { 625000000, 0xa0000000 },
	{ 645184059, 0xa52ac851 }, { 676497788, 0xad2ef583 },
	{ 678910895, 0xadcd1abb }, { 679569625, 0xadf84663 },
	{ 690926741, 0xb0e0932d }, { 705656483, 0xb4a5e73d },
	{ 723553854, 0xb93ad34c }, { 750000000, 0xc0000000 },
	{ 763550253, 0xc3780785 }, { 775284917, 0xc6791284 },
	{ 826190764, 0xd3813ce8 }, { 875000000, 0xe0000000 },
	{ 956805507, 0xf4f134a9 }, { 982570733, 0xfb89c16c }
	};


static uint32_t my_tick_to_tsf(uint32_t ticks)
{
	// convert nanoseconds to l_fp fractional units, using double
	// precision float calculations or, if available, 64bit integer
	// arithmetic. This should give the precise fraction, rounded to
	// the nearest representation.

	return (uint32_t
	    )((( ((uint64_t)(ticks)) << 32) + 500000000) / 1000000000);
	// And before you ask: if ticks >= 1000000000, the result is
	// truncated nonsense, so don't use it out-of-bounds.
}


static uint32_t my_tsf_to_tick(uint32_t tsf)
{

	// Inverse operation: converts fraction to microseconds.
	return (uint32_t)(( ((uint64_t)(tsf)) * 1000000000 + 0x80000000) >> 32);
	// Beware: The result might be 10^9 due to rounding!
}

// ---------------------------------------------------------------------
// test support stuff -- part 1
// ---------------------------------------------------------------------

TEST(timespecops, Helpers1) {
	struct timespec x;

	for (x.tv_sec = -2; x.tv_sec < 3; x.tv_sec++) {
		x.tv_nsec = -1;
		TEST_ASSERT_FALSE(timespec_isValid(x));
		x.tv_nsec = 0;
		TEST_ASSERT_TRUE(timespec_isValid(x));
		x.tv_nsec = 999999999;
		TEST_ASSERT_TRUE(timespec_isValid(x));
		x.tv_nsec = 1000000000;
		TEST_ASSERT_FALSE(timespec_isValid(x));
	}

	return;
}

//----------------------------------------------------------------------
// test normalisation
//----------------------------------------------------------------------

TEST(timespecops, Normalise) {
	long ns;

	for ( ns = -2000000000; ns <= 2000000000; ns += 10000000) {
		struct timespec x = timespec_init(0, ns);

		x = normalize_tspec(x);
		TEST_ASSERT_TRUE(timespec_isValid(x));
	}

	return;
}

//----------------------------------------------------------------------
// test classification
//----------------------------------------------------------------------

TEST(timespecops, SignNoFrac) {
	// sign test, no fraction
	int i;

	for (i = -4; i <= 4; ++i) {
		struct timespec a = timespec_init(i, 0);
		int E = (i > 0) - (i < 0);
		int r = test_tspec_denorm(a);

		TEST_ASSERT_EQUAL(E, r);
	}

	return;
}


TEST(timespecops, SignWithFrac) {
	// sign test, with fraction
	int i;

	for (i = -4; i <= 4; ++i) {
		struct timespec a = timespec_init(i, 10);
		int E = (i >= 0) - (i < 0);
		int r = test_tspec_denorm(a);

		TEST_ASSERT_EQUAL(E, r);
	}

	return;
}

//----------------------------------------------------------------------
// test compare
//----------------------------------------------------------------------
TEST(timespecops, CmpFracEQ) {
	// fractions are equal
	int i, j;
	for (i = -4; i <= 4; ++i)
		for (j = -4; j <= 4; ++j) {
			struct timespec a = timespec_init( i , 200);
			struct timespec b = timespec_init( j , 200);
			int   E = (i > j) - (i < j);
			int   r = cmp_tspec_denorm(a, b);

			TEST_ASSERT_EQUAL(E, r);
		}

	return;
}

TEST(timespecops, CmpFracGT) {
	// fraction a bigger fraction b
	int i, j;

	for (i = -4; i <= 4; ++i)
		for (j = -4; j <= 4; ++j) {
			struct timespec a = timespec_init(i, 999999800);
			struct timespec b = timespec_init(j, 200);
			int   E = (i >= j) - (i < j);
			int   r = cmp_tspec_denorm(a, b);

			TEST_ASSERT_EQUAL(E, r);
		}

	return;
}

TEST(timespecops, CmpFracLT) {
	// fraction a less fraction b
	int i, j;

	for (i = -4; i <= 4; ++i)
		for (j = -4; j <= 4; ++j) {
			struct timespec a = timespec_init(i, 200);
			struct timespec b = timespec_init(j, 999999800);
			int   E = (i > j) - (i <= j);
			int   r = cmp_tspec_denorm(a, b);

			TEST_ASSERT_EQUAL(E, r);
		}

	return;
}

//----------------------------------------------------------------------
// Test addition (sum)
//----------------------------------------------------------------------

TEST(timespecops, AddFullNorm) {
	int i, j;

	for (i = -4; i <= 4; ++i)
		for (j = -4; j <= 4; ++j) {
			struct timespec a = timespec_init(i, 200);
			struct timespec b = timespec_init(j, 400);
			struct timespec E = timespec_init(i + j, 200 + 400);
			struct timespec c;

			c = add_tspec(a, b);
			TEST_ASSERT_EQUAL_timespec(E, c);
		}

	return;
}


TEST(timespecops, AddFullOflow1) {
	int i, j;

	for (i = -4; i <= 4; ++i)
		for (j = -4; j <= 4; ++j) {
			struct timespec a = timespec_init(i, 200);
			struct timespec b = timespec_init(j, 999999900);
			struct timespec E = timespec_init(i + j + 1, 100);
			struct timespec c;

			c = add_tspec(a, b);
			TEST_ASSERT_EQUAL_timespec(E, c);
		}

	return;
}


TEST(timespecops, AddNsecNorm) {
	int i;

	for (i = -4; i <= 4; ++i) {
		struct timespec a = timespec_init(i, 200);
		struct timespec E = timespec_init(i, 600);
		struct timespec c;

		c = add_tspec_ns(a, 600 - 200);
		TEST_ASSERT_EQUAL_timespec(E, c);
	}

	return;
}


TEST(timespecops, AddNsecOflow1) {
	int i;

	for (i = -4; i <= 4; ++i) {
		struct timespec a = timespec_init(i, 200);
		struct timespec E = timespec_init(i + 1, 100);
		struct timespec c;

		c = add_tspec_ns(a, NS_PER_S - 100);
		TEST_ASSERT_EQUAL_timespec(E, c);
	}

	return;
}

//----------------------------------------------------------------------
// test subtraction (difference)
//----------------------------------------------------------------------

TEST(timespecops, SubFullNorm) {
	int i, j;

	for (i = -4; i <= 4; ++i)
		for (j = -4; j <= 4; ++j) {
			struct timespec a = timespec_init( i , 600);
			struct timespec b = timespec_init( j , 400);
			struct timespec E = timespec_init(i-j, 200);
			struct timespec c;

			c = sub_tspec(a, b);
			TEST_ASSERT_EQUAL_timespec(E, c);
		}

	return;
}


TEST(timespecops, SubFullOflow) {
	int i, j;

	for (i = -4; i <= 4; ++i)
		for (j = -4; j <= 4; ++j) {
			struct timespec a = timespec_init(i, 100);
			struct timespec b = timespec_init(j, 999999900);
			struct timespec E = timespec_init(i - j - 1, 200);
			struct timespec c;

			c = sub_tspec(a, b);
			TEST_ASSERT_EQUAL_timespec(E, c);
		}

	return;
}


TEST(timespecops, SubNsecNorm) {
	int i;

	for (i = -4; i <= 4; ++i) {
		struct timespec a = timespec_init(i, 600);
		struct timespec E = timespec_init(i, 200);
		struct timespec c;

		c = sub_tspec_ns(a, 600 - 200);
		TEST_ASSERT_EQUAL_timespec(E, c);
	}

	return;
}


TEST(timespecops, SubNsecOflow) {
	int i;

	for (i = -4; i <= 4; ++i) {
		struct timespec a = timespec_init( i , 100);
		struct timespec E = timespec_init(i-1, 200);
		struct timespec c;

		c = sub_tspec_ns(a, NS_PER_S - 100);
		TEST_ASSERT_EQUAL_timespec(E, c);
	}

	return;
}

//----------------------------------------------------------------------
// test negation
//----------------------------------------------------------------------


TEST(timespecops, test_Neg) {
	int i;

	for (i = -4; i <= 4; ++i) {
		struct timespec a = timespec_init(i, 100);
		struct timespec b;
		struct timespec c;

		b = neg_tspec(a);
		c = add_tspec(a, b);
		TEST_ASSERT_EQUAL(0, test_tspec(c));
	}

	return;
}

//----------------------------------------------------------------------
// test abs value
//----------------------------------------------------------------------

TEST(timespecops, test_AbsNoFrac) {
	int i;

	for (i = -4; i <= 4; ++i) {
		struct timespec a = timespec_init(i , 0);
		struct timespec b;

		b = abs_tspec(a);
		TEST_ASSERT_EQUAL((i != 0), test_tspec(b));
	}

	return;
}


TEST(timespecops, test_AbsWithFrac) {
	int i;

	for (i = -4; i <= 4; ++i) {
		struct timespec a = timespec_init(i, 100);
		struct timespec b;

		b = abs_tspec(a);
		TEST_ASSERT_EQUAL(1, test_tspec(b));
	}

	return;
}

// ---------------------------------------------------------------------
// test support stuff -- part 2
// ---------------------------------------------------------------------

/* FIXME: temporarily disabled - spews cryptic messages into test log */
TEST(timespecops, test_Helpers2) {
	struct timespec limit = timespec_init(0, 2);
	struct timespec x, y;
	long i;

	for (x.tv_sec = -2; x.tv_sec < 3; x.tv_sec++)
		for (x.tv_nsec = 1;
		     x.tv_nsec < 1000000000;
		     x.tv_nsec += 499999999) {
			for (i = -4; i < 5; ++i) {
				y = x;
				y.tv_nsec += i;
				if (i >= -2 && i <= 2) {
					TEST_ASSERT_TRUE(AssertTimespecClose(x, y, limit));
				}
				else
				{
					TEST_ASSERT_FALSE(AssertTimespecClose(x, y, limit));
				}
			}
		}

	return;
}

//----------------------------------------------------------------------
// conversion to l_fp
//----------------------------------------------------------------------

TEST(timespecops, test_ToLFPbittest) {
	l_fp lfpClose =  lfpinit(0, 1);
	uint32_t i;

	for (i = 0; i < 1000000000; i+=1000) {
		struct timespec a = timespec_init(1, (long)i);
		l_fp E= lfpinit(1, my_tick_to_tsf(i));
		l_fp r;

		r = tspec_intv_to_lfp(a);
		TEST_ASSERT_TRUE(AssertFpClose(E, r, lfpClose));
	}

	return;
}


TEST(timespecops, test_ToLFPrelPos) {
	int i;

	for (i = 0; i < (int)COUNTOF(fdata); ++i) {
		struct timespec a = timespec_init(1, fdata[i].nsec);
		l_fp E = lfpinit(1, fdata[i].frac);
		l_fp r;

		r = tspec_intv_to_lfp(a);
		TEST_ASSERT_EQUAL_l_fp(E, r);
	}

	return;
}


TEST(timespecops, test_ToLFPrelNeg) {
	int i;

	for (i = 0; i < (int)COUNTOF(fdata); ++i) {
		struct timespec a = timespec_init(-1, fdata[i].nsec);
		l_fp E = lfpinit(~0, fdata[i].frac);
		l_fp r;

		r = tspec_intv_to_lfp(a);
		TEST_ASSERT_EQUAL_l_fp(E, r);
	}

	return;
}


TEST(timespecops, test_ToLFPabs) {
	int i;

	for (i = 0; i < (int)COUNTOF(fdata); ++i) {
		struct timespec a = timespec_init(1, fdata[i].nsec);
		l_fp E = lfpinit((int)(1 + JAN_1970), fdata[i].frac);
		l_fp r;

		r = tspec_stamp_to_lfp(a);
		TEST_ASSERT_EQUAL_l_fp(E, r);
	}

	return;
}

//----------------------------------------------------------------------
// conversion from l_fp
//----------------------------------------------------------------------

TEST(timespecops, test_FromLFPbittest) {
	struct timespec limit = timespec_init(0, 2);

	// Not *exactly* a bittest, because 2**32 tests would take a
	// really long time even on very fast machines! So we do test
	// every 1000 fractional units.
	uint32_t tsf;
	for (tsf = 0; tsf < ~((uint32_t)(1000)); tsf += 1000) {
		struct timespec E = timespec_init(1, (long)my_tsf_to_tick(tsf));
		l_fp a = lfpinit(1, tsf);
		struct timespec r;

		r = lfp_intv_to_tspec(a);
		// The conversion might be off by one nanosecond when
		// comparing to calculated value.
		TEST_ASSERT_TRUE(AssertTimespecClose(E, r, limit));
	}

	return;
}


TEST(timespecops, test_FromLFPrelPos) {
	struct timespec limit = timespec_init(0, 2);
	int i;

	for (i = 0; i < (int)COUNTOF(fdata); ++i) {
		l_fp a = lfpinit(1, fdata[i].frac);
		struct timespec E = timespec_init(1, fdata[i].nsec);
		struct timespec r;

		r = lfp_intv_to_tspec(a);
		TEST_ASSERT_TRUE(AssertTimespecClose(E, r, limit));
	}

	return;
}


TEST(timespecops, test_FromLFPrelNeg) {
	struct timespec limit = timespec_init(0, 2);
	int i;

	for (i = 0; i < (int)COUNTOF(fdata); ++i) {
		l_fp a = lfpinit(~0, fdata[i].frac);
		struct timespec E = timespec_init(-1, fdata[i].nsec);
		struct timespec r;

		r = lfp_intv_to_tspec(a);
		TEST_ASSERT_TRUE(AssertTimespecClose(E, r, limit));
	}

	return;
}


// nsec -> frac -> nsec roundtrip, using a prime start and increment
TEST(timespecops, test_LFProundtrip) {
	int32_t t;
	uint32_t i;

	for (t = -1; t < 2; ++t)
		for (i = 4999; i < 1000000000; i += 10007) {
			struct timespec E = timespec_init(t, (long)i);
			l_fp a;
			struct timespec r;

			a = tspec_intv_to_lfp(E);
			r = lfp_intv_to_tspec(a);
			TEST_ASSERT_EQUAL_timespec(E, r);
		}

	return;
}

//----------------------------------------------------------------------
// conversion from l_fp, unsigned
//----------------------------------------------------------------------

TEST(timespecops, test_FromLFPuBittest) {
	struct timespec limit = timespec_init(0, 2);

	// Not *exactly* a bittest, because 2**32 tests would take a
	// really long time even on very fast machines! So we do test
	// every 1000 fractional units.
	uint32_t tsf;
	for (tsf = 0; tsf < ~((uint32_t)(1000)); tsf += 1000) {
		struct timespec E = timespec_init(1, (long)my_tsf_to_tick(tsf));
		l_fp a = lfpinit(1, tsf);
		struct timespec r;

		r = lfp_uintv_to_tspec(a);
		// The conversion might be off by one nanosecond when
		// comparing to calculated value.
		TEST_ASSERT_TRUE(AssertTimespecClose(E, r, limit));
	}

	return;
}


TEST(timespecops, test_FromLFPuRelPos) {
	struct timespec limit = timespec_init(0, 2);
	int i;

	for (i = 0; i < (int)COUNTOF(fdata); ++i) {
		l_fp a = lfpinit(1, fdata[i].frac);
		struct timespec E = timespec_init(1, fdata[i].nsec);
		struct timespec r;

		r = lfp_uintv_to_tspec(a);
		TEST_ASSERT_TRUE(AssertTimespecClose(E, r, limit));
	}

	return;
}


TEST(timespecops, test_FromLFPuRelNeg) {
	struct timespec limit = timespec_init(0, 2);
	int i;

	for (i = 0; i < (int)COUNTOF(fdata); ++i) {
		l_fp a = lfpinit(~0, fdata[i].frac);
		struct timespec E = timespec_init(-1, fdata[i].nsec);
		struct timespec r;

		r = lfp_uintv_to_tspec(a);
		TEST_ASSERT_TRUE(AssertTimespecClose(E, r, limit));
	}

	return;
}


// nsec -> frac -> nsec roundtrip, using a prime start and increment
TEST(timespecops, test_LFPuRoundtrip) {
	int32_t t;
	uint32_t i;

	for (t = -1; t < 2; ++t)
		for (i = 4999; i < 1000000000; i += 10007) {
			struct timespec E = timespec_init(t, (long)i);
			l_fp a;
			struct timespec r;

			a = tspec_intv_to_lfp(E);
			r = lfp_uintv_to_tspec(a);
			TEST_ASSERT_EQUAL_timespec(E, r);
		}

	return;
}

//----------------------------------------------------------------------
// conversion from double
//----------------------------------------------------------------------

TEST(timespecops, DToTspec) {
	struct timespec res;

	res = d_to_tspec(42.25);
	TEST_ASSERT_EQUAL(42, res.tv_sec);
	TEST_ASSERT_EQUAL(250000000, res.tv_nsec);
}

//----------------------------------------------------------------------
// conversion from lfp stamp
//----------------------------------------------------------------------

TEST(timespecops, LfpStampToTspec) {
	struct timespec res;

	res = lfp_stamp_to_tspec(86400, 100000);
	TEST_ASSERT_EQUAL(2085978496, res.tv_sec);
	TEST_ASSERT_EQUAL(20117, res.tv_nsec);
}

//----------------------------------------------------------------------
// conversion from tval
//----------------------------------------------------------------------

TEST(timespecops, TvalToTspec) {
	struct timespec res;
	struct timeval in;

	in.tv_sec = 42;
	in.tv_usec = 23;
	res = tval_to_tspec(in);
	TEST_ASSERT_EQUAL(42, res.tv_sec);
	TEST_ASSERT_EQUAL(23000, res.tv_nsec);
}

//----------------------------------------------------------------------
// string formatting
//----------------------------------------------------------------------

TEST_GROUP_RUNNER(timespecops) {
	RUN_TEST_CASE(timespecops, Helpers1);
	RUN_TEST_CASE(timespecops, Normalise);
	RUN_TEST_CASE(timespecops, SignNoFrac);
	RUN_TEST_CASE(timespecops, SignWithFrac);
	RUN_TEST_CASE(timespecops, CmpFracEQ);
	RUN_TEST_CASE(timespecops, CmpFracGT);
	RUN_TEST_CASE(timespecops, CmpFracLT);
	RUN_TEST_CASE(timespecops, AddFullNorm);
	RUN_TEST_CASE(timespecops, AddFullOflow1);
	RUN_TEST_CASE(timespecops, AddNsecNorm);
	RUN_TEST_CASE(timespecops, AddNsecOflow1);
	RUN_TEST_CASE(timespecops, SubFullNorm);
	RUN_TEST_CASE(timespecops, SubFullOflow);
	RUN_TEST_CASE(timespecops, SubNsecNorm);
	RUN_TEST_CASE(timespecops, SubNsecOflow);
	RUN_TEST_CASE(timespecops, test_Neg);
	RUN_TEST_CASE(timespecops, test_AbsNoFrac);
	RUN_TEST_CASE(timespecops, test_AbsWithFrac);
	/* RUN_TEST_CASE(timespecops, test_Helpers2); */
	RUN_TEST_CASE(timespecops, test_ToLFPbittest);
	RUN_TEST_CASE(timespecops, test_ToLFPrelPos);
	RUN_TEST_CASE(timespecops, test_ToLFPrelNeg);
	RUN_TEST_CASE(timespecops, test_ToLFPabs);
	RUN_TEST_CASE(timespecops, test_FromLFPbittest);
	RUN_TEST_CASE(timespecops, test_FromLFPrelPos);
	RUN_TEST_CASE(timespecops, test_FromLFPrelNeg);
	RUN_TEST_CASE(timespecops, test_LFProundtrip);
	RUN_TEST_CASE(timespecops, test_FromLFPuBittest);
	RUN_TEST_CASE(timespecops, test_FromLFPuRelPos);
	RUN_TEST_CASE(timespecops, test_FromLFPuRelNeg);
	RUN_TEST_CASE(timespecops, test_LFPuRoundtrip);
	RUN_TEST_CASE(timespecops, DToTspec);
	RUN_TEST_CASE(timespecops, LfpStampToTspec);
	RUN_TEST_CASE(timespecops, TvalToTspec);
}


// -*- EOF -*-
