#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(sfptostr);

TEST_SETUP(sfptostr) {}

TEST_TEAR_DOWN(sfptostr) {}

/* 
 * This file contains test for both fptoa and fptoms (which uses dofptoa),
 * since all these functions are very similar.
 */


#include "ntp_fp.h"

static const int SFP_MAX_PRECISION = 6;

TEST(sfptostr, PositiveInteger) {
	s_fp test = 300 << 16; // exact 300.000000

	TEST_ASSERT_EQUAL_STRING("300.000000", fptoa(test, SFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("300000.000", fptoms(test, SFP_MAX_PRECISION));
}

TEST(sfptostr, NegativeInteger) {
	/* Mask to avoid warnings.  See issue #71 */
	s_fp test = (-200 & 0xffff)  << 16; // exact -200.000000

	TEST_ASSERT_EQUAL_STRING("-200.000000", fptoa(test, SFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("-200000.000", fptoms(test, SFP_MAX_PRECISION));
}

TEST(sfptostr, PositiveIntegerPositiveFraction) {
	s_fp test = (300 << 16) + (1 << 15); // 300 + 0.5

	TEST_ASSERT_EQUAL_STRING("300.500000", fptoa(test, SFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("300500.000", fptoms(test, SFP_MAX_PRECISION));
}

TEST(sfptostr, NegativeIntegerNegativeFraction) {
	s_fp test = ((-200 & 0xffff) << 16) - (1 << 15); // -200 - 0.5

	TEST_ASSERT_EQUAL_STRING("-200.500000", fptoa(test, SFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("-200500.000", fptoms(test, SFP_MAX_PRECISION));
}

TEST(sfptostr, PositiveIntegerNegativeFraction) {
	s_fp test = (300 << 16) - (1 << 14); // 300 - 0.25

	TEST_ASSERT_EQUAL_STRING("299.750000", fptoa(test, SFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("299750.000", fptoms(test, SFP_MAX_PRECISION));
}

TEST(sfptostr, NegativeIntegerPositiveFraction) {
	s_fp test = ((-200 & 0xffff) << 16) + (1 << 14)*3; // -200 + 0.75

	TEST_ASSERT_EQUAL_STRING("-199.250000", fptoa(test, SFP_MAX_PRECISION));
	TEST_ASSERT_EQUAL_STRING("-199250.000", fptoms(test, SFP_MAX_PRECISION));
}

TEST(sfptostr, SingleDecimalInteger) {
	s_fp test = 300 << 16; // 300

	TEST_ASSERT_EQUAL_STRING("300.0", fptoa(test, 1));
	TEST_ASSERT_EQUAL_STRING("300000.0", fptoms(test, 1));
}

TEST(sfptostr, SingleDecimalRounding) {
	s_fp test = (2 << 16) + (1 << 14)*3; // 2 + 0.25*3 = 2.75

	TEST_ASSERT_EQUAL_STRING("2.8", fptoa(test, 1));
	TEST_ASSERT_EQUAL_STRING("2750.0", fptoms(test, 1));
}

TEST_GROUP_RUNNER(sfptostr) {
	RUN_TEST_CASE(sfptostr, PositiveInteger);
	RUN_TEST_CASE(sfptostr, NegativeInteger);
	RUN_TEST_CASE(sfptostr, PositiveIntegerPositiveFraction);
	RUN_TEST_CASE(sfptostr, NegativeIntegerNegativeFraction);
	RUN_TEST_CASE(sfptostr, PositiveIntegerNegativeFraction);
	RUN_TEST_CASE(sfptostr, NegativeIntegerPositiveFraction);
	RUN_TEST_CASE(sfptostr, SingleDecimalInteger);
	RUN_TEST_CASE(sfptostr, SingleDecimalRounding);
}
