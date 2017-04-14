#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(vi64ops);

TEST_SETUP(vi64ops) {}

TEST_TEAR_DOWN(vi64ops) {}

TEST(vi64ops, SetVUI64s_pos) {
	time64_t expx = 0;

	settime64s(expx, 0x0123456789ABCDEF);
	TEST_ASSERT_EQUAL(time64s(expx), 81985529216486895);
}

TEST(vi64ops, SetVUI64s_neg) {
	time64_t expx = 0;

	settime64s(expx, 0xFEDCBA9876543210);
	TEST_ASSERT_EQUAL(time64s(expx), -81985529216486896);
}

TEST(vi64ops, SetVUI64u) {
	time64_t expx = 0;

	settime64u(expx, 0xFEDCBA9876543210);	/* sign bit is on */
	TEST_ASSERT_EQUAL(time64s(expx), 18364758544493064720UL);
}

TEST(vi64ops, NegVUI64) {
	time64_t expx = 0;

	settime64s(expx, 71985529216486896);
	TEST_ASSERT_EQUAL(negtime64(expx), -71985529216486896);
}

TEST_GROUP_RUNNER(vi64ops) {
	RUN_TEST_CASE(vi64ops, SetVUI64s_pos);
	RUN_TEST_CASE(vi64ops, SetVUI64s_neg);
	RUN_TEST_CASE(vi64ops, SetVUI64u);
	RUN_TEST_CASE(vi64ops, NegVUI64);
}
