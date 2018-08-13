#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"
#include "ntp_fp.h"

TEST_GROUP(lfptostr);

TEST_SETUP(lfptostr) {}

TEST_TEAR_DOWN(lfptostr) {}

/*
 * This file contains test for both mfptoa and mfptoms (which uses dolfptoa),
 * since all these functions are very similar. It also tests ulfptoa, which is
 * a macro.
 */



static const int LFP_MAX_PRECISION = 10;
static const int LFP_MAX_PRECISION_MS = 7;

static const uint32_t ONE_FOURTH = 1073741824U;     // (1 << 30)
static const uint32_t HALF = 2147483648U;           // (1 << 31)
static const uint32_t THREE_FOURTH = 3221225472U;
static const uint32_t HALF_PROMILLE_UP = 2147484;   // slightly more than 0.0005
static const uint32_t HALF_PROMILLE_DOWN = 2147483; // slightly less than 0.0005

TEST(lfptostr, PositiveInteger) {
    l_fp test = lfpinit(200, 0); // exact 200.0000000000

	TEST_ASSERT_EQUAL_STRING("200.0000000000", mfptoa(test, LFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("200000.0000000", mfptoms(test, LFP_MAX_PRECISION_MS));
}

TEST(lfptostr, NegativeInteger) {
    l_fp test = lfpinit(-100, 0); // -100

	TEST_ASSERT_EQUAL_STRING("-100.0000000000", lfptoa(test, LFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("-100000.0000000", lfptoms(test, LFP_MAX_PRECISION_MS));
}

TEST(lfptostr, PositiveIntegerWithFraction) {
	l_fp test = lfpinit(200, ONE_FOURTH); // 200.25

	TEST_ASSERT_EQUAL_STRING("200.2500000000", lfptoa(test, LFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("200250.0000000", lfptoms(test, LFP_MAX_PRECISION_MS));
}

TEST(lfptostr, NegativeIntegerWithFraction) {
	l_fp test = lfpinit(-100, ONE_FOURTH); // -99.75

	TEST_ASSERT_EQUAL_STRING("-99.7500000000", lfptoa(test, LFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("-99750.0000000", lfptoms(test, LFP_MAX_PRECISION_MS));
}

TEST(lfptostr, RoundingDownToInteger) {
	l_fp test = lfpinit(10, ONE_FOURTH); // 10.25

	TEST_ASSERT_EQUAL_STRING("10", lfptoa(test, 0));
	TEST_ASSERT_EQUAL_STRING("10250", lfptoms(test, 0));
}

TEST(lfptostr, RoundingMiddleToInteger) {
	l_fp test = lfpinit(10, HALF); // 10.5

	TEST_ASSERT_EQUAL_STRING("11", lfptoa(test, 0));
	TEST_ASSERT_EQUAL_STRING("10500", lfptoms(test, 0));
}

TEST(lfptostr, RoundingUpToInteger) {
	l_fp test = lfpinit(5, THREE_FOURTH); // 5.75

	TEST_ASSERT_EQUAL_STRING("6", lfptoa(test, 0));
	TEST_ASSERT_EQUAL_STRING("5750", lfptoms(test, 0));
}

TEST(lfptostr, SingleDecimal) {
	l_fp test = lfpinit(8, ONE_FOURTH); // 8.25

	TEST_ASSERT_EQUAL_STRING("8.3", lfptoa(test, 1));
	TEST_ASSERT_EQUAL_STRING("8250.0", lfptoms(test, 1));
}

TEST(lfptostr, MillisecondsRoundingUp) {
	l_fp test = lfpinit(1, HALF_PROMILLE_UP); //slightly more than 1.0005

	TEST_ASSERT_EQUAL_STRING("1.0", lfptoa(test, 1));

	TEST_ASSERT_EQUAL_STRING("1000.5", lfptoms(test, 1));
	TEST_ASSERT_EQUAL_STRING("1001", lfptoms(test, 0));
}

TEST(lfptostr, MillisecondsRoundingDown) {
	l_fp test = lfpinit(1, HALF_PROMILLE_DOWN); // slightly less than 1.0005

	TEST_ASSERT_EQUAL_STRING("1.0", lfptoa(test, 1));

	TEST_ASSERT_EQUAL_STRING("1000.5", lfptoms(test, 1));
	TEST_ASSERT_EQUAL_STRING("1000", lfptoms(test, 0));
}

TEST(lfptostr, UnsignedInteger) {
	l_fp test1 = lfpinit((int32_t)3000000000LL, 0);
	l_fp test2 = lfpinit((int32_t)3000000L, 0x80000000);
	l_fp test3 = lfpinit((int32_t)13L, 0xC0000000);
	l_fp test4 = lfpinit((int32_t)13L, 0x028F5C28);

	l_fp test5 = lfpinit((int32_t)4212665562LL, 0x3C6BE7E6);
	l_fp test6 = lfpinit((int32_t)4212665562LL, 0x36222683);
	l_fp test7 = lfpinit((int32_t)4212665562LL, 0xBD3F2F5A);
	l_fp test8a = lfpinit((int32_t)1444359386LL, 0x2E0C7582);
	l_fp test8b = lfpinit((int32_t)1444359386LL, 0x2E0C7583);
	l_fp test9 = lfpinit((int32_t)3660323067LL, 0x1CD3101C);

	TEST_ASSERT_EQUAL_STRING("3000000000.0", ulfptoa(test1, 1));
	TEST_ASSERT_EQUAL_STRING("3000000000.000000", ulfptoa(test1, 6));
	TEST_ASSERT_EQUAL_STRING("3000000.5", ulfptoa(test2, 1));
	TEST_ASSERT_EQUAL_STRING("3000000.500", ulfptoa(test2, 3));
	TEST_ASSERT_EQUAL_STRING("13.750000", ulfptoa(test3, 6));
	TEST_ASSERT_EQUAL_STRING("13.010000", ulfptoa(test4, 6));

	TEST_ASSERT_EQUAL_STRING("4212665562.2360215127", ulfptoa(test5, 10));
	TEST_ASSERT_EQUAL_STRING("4212665562.2114585943", ulfptoa(test6, 10));
	TEST_ASSERT_EQUAL_STRING("4212665562.739245376", ulfptoa(test7, 9));
	TEST_ASSERT_EQUAL_STRING("1444359386.1798776095", ulfptoa(test8a, 10));
	TEST_ASSERT_EQUAL_STRING("1444359386.1798776097", ulfptoa(test8b, 10));
	TEST_ASSERT_EQUAL_STRING("3660323067.1125955647", ulfptoa(test9, 10));
}

TEST_GROUP_RUNNER(lfptostr) {
	RUN_TEST_CASE(lfptostr, PositiveInteger);
	RUN_TEST_CASE(lfptostr, NegativeInteger);
	RUN_TEST_CASE(lfptostr, PositiveIntegerWithFraction);
	RUN_TEST_CASE(lfptostr, NegativeIntegerWithFraction);
	RUN_TEST_CASE(lfptostr, RoundingDownToInteger);
	RUN_TEST_CASE(lfptostr, RoundingMiddleToInteger);
	RUN_TEST_CASE(lfptostr, RoundingUpToInteger);
	RUN_TEST_CASE(lfptostr, SingleDecimal);
	RUN_TEST_CASE(lfptostr, MillisecondsRoundingUp);
	RUN_TEST_CASE(lfptostr, MillisecondsRoundingDown);
	RUN_TEST_CASE(lfptostr, UnsignedInteger);
}
