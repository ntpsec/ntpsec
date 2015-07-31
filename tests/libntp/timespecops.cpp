extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(timespecops);

TEST_SETUP(timespecops) {}

TEST_TEAR_DOWN(timespecops) {}

#include "libntptest.h"
#include "timestructs.h"

extern "C" {
#include <math.h>
#include "timespecops.h"
}

#include <string>
#include <sstream>

using namespace timeStruct;

class timespecTest : public libntptest {
protected:
	static u_int32 my_tick_to_tsf(u_int32 ticks);
	static u_int32 my_tsf_to_tick(u_int32 tsf);

	// that's it...
	struct lfpfracdata {
		long	nsec;
		u_int32 frac;
	};
	static const lfpfracdata fdata[];
};

u_int32
timespecTest::my_tick_to_tsf(
	u_int32 ticks
	)
{
	// convert nanoseconds to l_fp fractional units, using double
	// precision float calculations or, if available, 64bit integer
	// arithmetic. This should give the precise fraction, rounded to
	// the nearest representation.
#ifdef HAVE_U_INT64
	return u_int32(((u_int64(ticks) << 32) + 500000000) / 1000000000);
#else
	return u_int32(double(ticks) * 4.294967296 + 0.5);
#endif
	// And before you ask: if ticks >= 1000000000, the result is
	// truncated nonsense, so don't use it out-of-bounds.
}

u_int32
timespecTest::my_tsf_to_tick(
	u_int32 tsf
	)
{
	// Inverse operation: converts fraction to microseconds.
#ifdef HAVE_U_INT64
	return u_int32((u_int64(tsf) * 1000000000 + 0x80000000) >> 32);
#else
	return u_int32(double(tsf) / 4.294967296 + 0.5);
#endif
	// Beware: The result might be 10^9 due to rounding!
}

const timespecTest::lfpfracdata timespecTest::fdata [] = {
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


// ---------------------------------------------------------------------
// test support stuff -- part 1
// ---------------------------------------------------------------------

TEST(timespec, Helpers1) {
	timespec_wrap x;

	for (x.V.tv_sec = -2; x.V.tv_sec < 3; x.V.tv_sec++) {
		x.V.tv_nsec = -1;
		TEST_ASSERT_FALSE(x.valid());
		x.V.tv_nsec = 0;
		TEST_ASSERT_TRUE(x.valid());
		x.V.tv_nsec = 999999999;
		TEST_ASSERT_TRUE(x.valid());
		x.V.tv_nsec = 1000000000;
		TEST_ASSERT_FALSE(x.valid());
	}
}

//----------------------------------------------------------------------
// test normalisation
//----------------------------------------------------------------------

TEST(timespec, Normalise) {
	for (long ns = -2000000000; ns <= 2000000000; ns += 10000000) {
		timespec_wrap x(0, ns);

		x = normalize_tspec(x);
		TEST_ASSERT_TRUE(x.valid());
	}
}

//----------------------------------------------------------------------
// test classification
//----------------------------------------------------------------------

TEST(timespec, SignNoFrac) {
	// sign test, no fraction
	for (int i = -4; i <= 4; ++i) {
		timespec_wrap a(i, 0);
		int E = (i > 0) - (i < 0);
		int r = test_tspec(a);

		TEST_ASSERT_EQUAL(E, r);
	}
}

TEST(timespec, SignWithFrac) {
	// sign test, with fraction
	for (int i = -4; i <= 4; ++i) {
		timespec_wrap a(i, 10);
		int E = (i >= 0) - (i < 0);
		int r = test_tspec(a);
		TEST_ASSERT_EQUAL(E, r);
	}
}

//----------------------------------------------------------------------
// test compare
//----------------------------------------------------------------------
TEST(timespec, CmpFracEQ) {
	// fractions are equal
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			timespec_wrap a( i , 200);
			timespec_wrap b( j , 200);
			int   E = (i > j) - (i < j);
			int   r = cmp_tspec_denorm(a, b);
			TEST_ASSERT_EQUAL(E, r);
		}
}

TEST(timespec, CmpFracGT) {
	// fraction a bigger fraction b
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			timespec_wrap a(i, 999999800);
			timespec_wrap b(j, 200);
			int   E = (i >= j) - (i < j);
			int   r = cmp_tspec_denorm(a, b);
			TEST_ASSERT_EQUAL(E, r);
		}
}

TEST(timespec, CmpFracLT) {
	// fraction a less fraction b
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			timespec_wrap a(i, 200);
			timespec_wrap b(j, 999999800);
			int   E = (i > j) - (i <= j);
			int   r = cmp_tspec_denorm(a, b);
			TEST_ASSERT_EQUAL(E, r);
		}
}

//----------------------------------------------------------------------
// Test addition (sum)
//----------------------------------------------------------------------

TEST(timespec, AddFullNorm) {
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			timespec_wrap a(i, 200);
			timespec_wrap b(j, 400);
			timespec_wrap E(i + j, 200 + 400);
			timespec_wrap c;

			c = add_tspec(a, b);
			TEST_ASSERT_EQUAL(E, c);
		}
}

TEST(timespec, AddFullOflow1) {
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			timespec_wrap a(i, 200);
			timespec_wrap b(j, 999999900);
			timespec_wrap E(i + j + 1, 100);
			timespec_wrap c;

			c = add_tspec(a, b);
			TEST_ASSERT_EQUAL(E, c);
		}
}

TEST(timespec, AddNsecNorm) {
	for (int i = -4; i <= 4; ++i) {
		timespec_wrap a(i, 200);
		timespec_wrap E(i, 600);
		timespec_wrap c;

		c = add_tspec_ns(a, 600 - 200);
		TEST_ASSERT_EQUAL(E, c);
	}
}

TEST(timespec, AddNsecOflow1) {
	for (int i = -4; i <= 4; ++i) {
		timespec_wrap a(i, 200);
		timespec_wrap E(i + 1, 100);
		timespec_wrap c;

		c = add_tspec_ns(a, NANOSECONDS - 100);
		TEST_ASSERT_EQUAL(E, c);
	}
}

//----------------------------------------------------------------------
// test subtraction (difference)
//----------------------------------------------------------------------

TEST(timespec, SubFullNorm) {
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			timespec_wrap a( i , 600);
			timespec_wrap b( j , 400);
			timespec_wrap E(i-j, 200);
			timespec_wrap c;

			c = sub_tspec(a, b);
			TEST_ASSERT_EQUAL(E, c);
		}
}

TEST(timespec, SubFullOflow) {
	for (int i = -4; i <= 4; ++i)
		for (int j = -4; j <= 4; ++j) {
			timespec_wrap a(  i  , 100);
			timespec_wrap b(  j  , 999999900);
			timespec_wrap E(i-j-1, 200);
			timespec_wrap c;

			c = sub_tspec(a, b);
			TEST_ASSERT_EQUAL(E, c);
		}
}

TEST(timespec, SubNsecNorm) {
	for (int i = -4; i <= 4; ++i) {
		timespec_wrap a(i, 600);
		timespec_wrap E(i, 200);
		timespec_wrap c;

		c = sub_tspec_ns(a, 600 - 200);
		TEST_ASSERT_EQUAL(E, c);
	}
}

TEST(timespec, SubNsecOflow) {
	for (int i = -4; i <= 4; ++i) {
		timespec_wrap a( i , 100);
		timespec_wrap E(i-1, 200);
		timespec_wrap c;

		c = sub_tspec_ns(a, NANOSECONDS - 100);
		TEST_ASSERT_EQUAL(E, c);
	}
}

//----------------------------------------------------------------------
// test negation
//----------------------------------------------------------------------

TEST(timespec, Neg) {
	for (int i = -4; i <= 4; ++i) {
		timespec_wrap a(i, 100);
		timespec_wrap b;
		timespec_wrap c;

		b = neg_tspec(a);
		c = add_tspec(a, b);
		TEST_ASSERT_EQUAL(0, test_tspec(c));
	}
}

//----------------------------------------------------------------------
// test abs value
//----------------------------------------------------------------------

TEST(timespec, AbsNoFrac) {
	for (int i = -4; i <= 4; ++i) {
		timespec_wrap a(i , 0);
		timespec_wrap b;

		b = abs_tspec(a);
		TEST_ASSERT_EQUAL((i != 0), test_tspec(b));
	}
}

TEST(timespec, AbsWithFrac) {
	for (int i = -4; i <= 4; ++i) {
		timespec_wrap a(i, 100);
		timespec_wrap b;

		b = abs_tspec(a);
		TEST_ASSERT_EQUAL(1, test_tspec(b));
	}
}

// ---------------------------------------------------------------------
// test support stuff -- part 2
// ---------------------------------------------------------------------

TEST(timespec, Helpers2) {
	AssertTimespecClose isClose(0, 2);
	timespec_wrap x, y;

	for (x.V.tv_sec = -2; x.V.tv_sec < 3; x.V.tv_sec++)
		for (x.V.tv_nsec = 1;
		     x.V.tv_nsec < 1000000000;
		     x.V.tv_nsec += 499999999) {
			for (long i = -4; i < 5; i++) {
				y = x;
				y.V.tv_nsec += i;
				if (i >= -2 && i <= 2)
					TEST_ASSERT_TRUE(isClose(x, y)); /* ASSERT_PRED_FORMAT2 */
				else
					TEST_ASSERT_TRUE(!isClose(x, y)); /* ASSERT_PRED_FORMAT2 */
			}
		}
}

// global predicate instances we're using here
static AssertFpClose FpClose(0, 1);
static AssertTimespecClose TimespecClose(0, 2);

//----------------------------------------------------------------------
// conversion to l_fp
//----------------------------------------------------------------------

TEST(timespec, ToLFPbittest) {
	for (u_int32 i = 0; i < 1000000000; i+=1000) {
		timespec_wrap a(1, i);
		l_fp_wrap     E(1, my_tick_to_tsf(i));
		l_fp_wrap     r;

		r = tspec_intv_to_lfp(a);
		TEST_ASSERT_TRUE(FpClose(E, r)); /* ASSERT_PRED_FORMAT2 */
	}
}

TEST(timespec, ToLFPrelPos) {
	for (int i = 0; i < COUNTOF(fdata); i++) {
		timespec_wrap a(1, fdata[i].nsec);
		l_fp_wrap     E(1, fdata[i].frac);
		l_fp_wrap     r;

		r = tspec_intv_to_lfp(a);
		TEST_ASSERT_EQUAL(E, r);
	}
}

TEST(timespec, ToLFPrelNeg) {
	for (int i = 0; i < COUNTOF(fdata); i++) {
		timespec_wrap a(-1, fdata[i].nsec);
		l_fp_wrap     E(~0, fdata[i].frac);
		l_fp_wrap     r;

		r = tspec_intv_to_lfp(a);
		TEST_ASSERT_EQUAL(E, r);
	}
}

TEST(timespec, ToLFPabs) {
	for (int i = 0; i < COUNTOF(fdata); i++) {
		timespec_wrap a(1, fdata[i].nsec);
		l_fp_wrap     E(1 + JAN_1970, fdata[i].frac);
		l_fp_wrap     r;

		r = tspec_stamp_to_lfp(a);
		TEST_ASSERT_EQUAL(E, r);
	}
}

//----------------------------------------------------------------------
// conversion from l_fp
//----------------------------------------------------------------------
TEST(timespec, FromLFPbittest) {
	// Not *exactly* a bittest, because 2**32 tests would take a
	// really long time even on very fast machines! So we do test
	// every 1000 fractional units.
	for (u_int32 tsf = 0; tsf < ~u_int32(1000); tsf += 1000) {
		timespec_wrap E(1, my_tsf_to_tick(tsf));
		l_fp_wrap     a(1, tsf);
		timespec_wrap r;

		r = lfp_intv_to_tspec(a);
		// The conversion might be off by one nanosecond when
		// comparing to calculated value.
		TEST_ASSERT_TRUE(TimespecClose(E, r)); /* ASSERT_PRED_FORMAT2 */
	}
}

TEST(timespec, FromLFPrelPos) {
	for (int i = 0; i < COUNTOF(fdata); i++) {
		l_fp_wrap     a(1, fdata[i].frac);
		timespec_wrap E(1, fdata[i].nsec);
		timespec_wrap r;

		r = lfp_intv_to_tspec(a);
		TEST_ASSERT_TRUE(TimespecClose(E, r)); /* ASSERT_PRED_FORMAT2 */
	}
}

TEST(timespec, FromLFPrelNeg) {
	for (int i = 0; i < COUNTOF(fdata); i++) {
		l_fp_wrap     a(~0, fdata[i].frac);
		timespec_wrap E(-1, fdata[i].nsec);
		timespec_wrap r;

		r = lfp_intv_to_tspec(a);
		TEST_ASSERT_TRUE(TimespecClose(E, r)); /* ASSERT_PRED_FORMAT2 */
	}
}


// nsec -> frac -> nsec roundtrip, using a prime start and increment
TEST(timespec, LFProundtrip) {
	for (int32_t t = -1; t < 2; ++t)
		for (u_int32 i = 4999; i < 1000000000; i+=10007) {
			timespec_wrap E(t, i);
			l_fp_wrap     a;
			timespec_wrap r;

			a = tspec_intv_to_lfp(E);
			r = lfp_intv_to_tspec(a);
			TEST_ASSERT_EQUAL(E, r);
		}
}

//----------------------------------------------------------------------
// string formatting
//----------------------------------------------------------------------

TEST(timespec, ToString) {
	static const struct {
		time_t		sec;
		long		nsec;
		const char *	repr;
	} data [] = {
		{ 0, 0,	 "0.000000000" },
		{ 2, 0,	 "2.000000000" },
		{-2, 0, "-2.000000000" },
		{ 0, 1,	 "0.000000001" },
		{ 0,-1,	"-0.000000001" },
		{ 1,-1,	 "0.999999999" },
		{-1, 1, "-0.999999999" },
		{-1,-1, "-1.000000001" },
	};
	for (int i = 0; i < COUNTOF(data); i++) {
		timespec_wrap a(data[i].sec, data[i].nsec);
		std::string E(data[i].repr);
		std::string r(tspectoa(a));
		TEST_ASSERT_EQUAL(E, r);
	}
}

// -*- EOF -*-
