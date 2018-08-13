#include "config.h"
#include "ntp.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(random);

TEST_SETUP(random) {}

TEST_TEAR_DOWN(random) {}


TEST(random, random32) {
	int i;
	uint32_t ones = 0;
	uint32_t zeros = ~0;

	/* This is just a crude sanity check.
	 * It could fail when working correctly,
	 * but the chances are pretty small.
	 * It won't be reproducable.  ;)
	 * You can test this code by making the loop count smaller.
	 */
	for (i=0; i<99; i++) {
		uint32_t sample = ntp_random();
		ones |= sample;
		zeros &= sample;
	}

	TEST_ASSERT_EQUAL_INT32(~0, ones);
	TEST_ASSERT_EQUAL_INT32(0, zeros);
}

TEST(random, random64) {
	int i;
	uint64_t ones = 0;
	uint64_t zeros = ~0;

	/* This is just a crude sanity check.
	 * It could fail when working correctly,
	 * but the chances are pretty small.
	 * It won't be reproducable.  ;)
	 * You can test this code by making the loop count smaller.
	 */
	for (i=0; i<99; i++) {
		uint64_t sample = ntp_random64();
		ones |= sample;
		zeros &= sample;
	}

	TEST_ASSERT_EQUAL_INT64(~0, ones);
	TEST_ASSERT_EQUAL_INT64(0, zeros);
}

TEST_GROUP_RUNNER(random) {
	RUN_TEST_CASE(random, random32);
	RUN_TEST_CASE(random, random64);
}
