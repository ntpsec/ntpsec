extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(lfptostr);

TEST_SETUP(lfptostr) {}

TEST_TEAR_DOWN(lfptostr) {}

/* 
 * This file contains test for both mfptoa and mfptoms (which uses dolfptoa),
 * since all these functions are very similar. It also tests ulfptoa, which is
 * a macro.
 */

#include "libntptest.h"

extern "C" {
#include "ntp_fp.h"
};

class lfptostrTest : public libntptest {
protected:
	static const int LFP_MAX_PRECISION = 10;
	static const int LFP_MAX_PRECISION_MS = 7;

	static const int ONE_FOURTH = 1073741824; // (1 << 30)
	static const int HALF = (1 << 31);
	static const int THREE_FOURTH = -ONE_FOURTH;
	static const int HALF_PROMILLE_UP = 2147484; // slightly more than 0.0005
	static const int HALF_PROMILLE_DOWN = 2147483; // slightly less than 0.0005
};

TEST(lfptostr, PositiveInteger) {
	l_fp test = {200, 0}; // exact 200.0000000000

	TEST_ASSERT_EQUAL_STRING("200.0000000000", mfptoa(test.l_ui, test.l_uf, LFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("200000.0000000", mfptoms(test.l_ui, test.l_uf, LFP_MAX_PRECISION_MS));
}

TEST(lfptostr, NegativeInteger) {
	l_fp test = {-100, 0}; // -100

	TEST_ASSERT_EQUAL_STRING("-100.0000000000", lfptoa(&test, LFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("-100000.0000000", lfptoms(&test, LFP_MAX_PRECISION_MS));
}

TEST(lfptostr, PositiveIntegerWithFraction) {
	l_fp test = {200, ONE_FOURTH}; // 200.25

	TEST_ASSERT_EQUAL_STRING("200.2500000000", lfptoa(&test, LFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("200250.0000000", lfptoms(&test, LFP_MAX_PRECISION_MS));
}

TEST(lfptostr, NegativeIntegerWithFraction) {
	l_fp test = {-100, ONE_FOURTH}; // -99.75

	TEST_ASSERT_EQUAL_STRING("-99.7500000000", lfptoa(&test, LFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("-99750.0000000", lfptoms(&test, LFP_MAX_PRECISION_MS));
}

TEST(lfptostr, RoundingDownToInteger) {
	l_fp test = {10, ONE_FOURTH}; // 10.25

	TEST_ASSERT_EQUAL_STRING("10", lfptoa(&test, 0));
	TEST_ASSERT_EQUAL_STRING("10250", lfptoms(&test, 0));
}

TEST(lfptostr, RoundingMiddleToInteger) {
	l_fp test = {10, HALF}; // 10.5

	TEST_ASSERT_EQUAL_STRING("11", lfptoa(&test, 0));
	TEST_ASSERT_EQUAL_STRING("10500", lfptoms(&test, 0));
}

TEST(lfptostr, RoundingUpToInteger) {
	l_fp test = {5, THREE_FOURTH}; // 5.75

	TEST_ASSERT_EQUAL_STRING("6", lfptoa(&test, 0));
	TEST_ASSERT_EQUAL_STRING("5750", lfptoms(&test, 0));
}

TEST(lfptostr, SingleDecimal) {
	l_fp test = {8, ONE_FOURTH}; // 8.25

	TEST_ASSERT_EQUAL_STRING("8.3", lfptoa(&test, 1));
	TEST_ASSERT_EQUAL_STRING("8250.0", lfptoms(&test, 1));
}

TEST(lfptostr, MillisecondsRoundingUp) {
	l_fp test = {1, HALF_PROMILLE_UP}; //slightly more than 1.0005

	TEST_ASSERT_EQUAL_STRING("1.0", lfptoa(&test, 1));

	TEST_ASSERT_EQUAL_STRING("1000.5", lfptoms(&test, 1));
	TEST_ASSERT_EQUAL_STRING("1001", lfptoms(&test, 0));
}

TEST(lfptostr, MillisecondsRoundingDown) {
	l_fp test = {1, HALF_PROMILLE_DOWN}; // slightly less than 1.0005

	TEST_ASSERT_EQUAL_STRING("1.0", lfptoa(&test, 1));

	TEST_ASSERT_EQUAL_STRING("1000.5", lfptoms(&test, 1));
	TEST_ASSERT_EQUAL_STRING("1000", lfptoms(&test, 0));
}

TEST(lfptostr, UnsignedInteger) {
	l_fp test = {3000000000UL, 0};

	TEST_ASSERT_EQUAL_STRING("3000000000.0", ulfptoa(&test, 1));
}
