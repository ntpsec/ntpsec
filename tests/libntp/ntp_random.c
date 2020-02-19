#include "config.h"
#include "ntp.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(random);

TEST_SETUP(random) {}

TEST_TEAR_DOWN(random) {}

/* leftover from testing ntp_random()
 * random(3) is 31 bits.
 */
TEST(random, random32) {
	uint32_t ones = 0;
	uint32_t zeros = RAND_MAX;

	/* This is just a crude sanity check.
	 * It could fail when working correctly,
	 * but the chances are pretty small.
	 * It won't be reproducible.  ;)
	 * You can test this code by making the loop count smaller.
	 */
	for (int i=0; i<99; i++) {
		uint32_t sample = random();
		ones |= sample;
		zeros &= sample;
	}

	/* RAND_MAX on FreeBSD is 0x7ffffffd */
	TEST_ASSERT_EQUAL_INT32(0x7fffffff, ones);
	TEST_ASSERT_EQUAL_INT32(0, zeros);
}

TEST(random, random_bytes) {
#define BYTES 100
	unsigned char zeros[BYTES];  /* collected zeros */
	unsigned char ones[BYTES];   /* collected ones */
	unsigned char clear[BYTES];  /* expected all zeros */
	unsigned char full[BYTES];   /* expected all ones */

	for (int j=0; j<BYTES; j++) {
		zeros[j] = ~0;
		ones[j] = 0;
		clear[j] = 0;
		full[j] = ~0;
	}

	/* This is just a crude sanity check.
	 * It could fail when working correctly,
	 * but the chances are pretty small.
	 * It won't be reproducible.  ;)
	 * You can test this code by making the loop count smaller.
	 */
	for (int i=0; i<99; i++) {
		unsigned char sample[BYTES];
		ntp_RAND_bytes(&sample[0], BYTES);
		for (int j=0; j<BYTES; j++) {
			zeros[j] &= ~sample[j];
			ones[j] |= sample[j];
		}
	}

	TEST_ASSERT_EQUAL_MEMORY(full, ones, BYTES);
	TEST_ASSERT_EQUAL_MEMORY(clear, zeros, BYTES);
}

TEST_GROUP_RUNNER(random) {
	RUN_TEST_CASE(random, random32);
	RUN_TEST_CASE(random, random_bytes);
}
