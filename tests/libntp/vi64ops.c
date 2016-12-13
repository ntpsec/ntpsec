#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(vi64ops);

TEST_SETUP(vi64ops) {}

TEST_TEAR_DOWN(vi64ops) {}

TEST(vi64ops, HiLoVUI64uh) {
	time64_t exp = 0;

	settime64hiu(exp, 0x01234567);
	settime64lo(exp, 0x89ABCDEF);
	TEST_ASSERT_EQUAL(time64hiu(exp), 0x01234567);
}

TEST(vi64ops, HiLoVUI64ul) {
	time64_t exp = 0;

	settime64hiu(exp, 0x01234567);
	settime64lo(exp, 0x89ABCDEF);
	TEST_ASSERT_EQUAL(time64lo(exp), 0x89ABCDEF);
}

TEST(vi64ops, SetVUI64s_pos) {
	time64_t exp = 0;

	settime64s(exp, 0x0123456789ABCDEF);
	TEST_ASSERT_EQUAL(time64s(exp), 81985529216486895);
}

TEST(vi64ops, SetVUI64s_neg) {
	time64_t exp = 0;

	settime64s(exp, 0xFEDCBA9876543210);
	TEST_ASSERT_EQUAL(time64s(exp), -81985529216486896);
}

TEST(vi64ops, SetVUI64u) {
	time64_t exp = 0;

	settime64u(exp, 0xFEDCBA9876543210);	/* sign bit is on */
	TEST_ASSERT_EQUAL(time64s(exp), 18364758544493064720UL);
}

TEST(vi64ops, NegVUI64) {
	time64_t exp = 0;

	settime64s(exp, 71985529216486896);
	TEST_ASSERT_EQUAL(negtime64(exp), -71985529216486896);
}

TEST_GROUP_RUNNER(vi64ops) {
	RUN_TEST_CASE(vi64ops, HiLoVUI64uh);
	RUN_TEST_CASE(vi64ops, HiLoVUI64ul);
	RUN_TEST_CASE(vi64ops, SetVUI64s_pos);
	RUN_TEST_CASE(vi64ops, SetVUI64s_neg);
	RUN_TEST_CASE(vi64ops, SetVUI64u);
	RUN_TEST_CASE(vi64ops, NegVUI64);
}
