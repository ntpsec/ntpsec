#include "config.h"

#include "unity.h"
#include "unity_fixture.h"

#include <time.h>

#include "timevalops.h"

TEST_GROUP(tstotv);

TEST_SETUP(tstotv) {}

TEST_TEAR_DOWN(tstotv) {}

u_long      current_time;

static bool IsEqual(const struct timeval *expected, const struct timeval *actual) {
	if (expected->tv_sec == actual->tv_sec &&
		expected->tv_usec == actual->tv_usec) {
		// Success
		return true;
	} else {
		printf("Expected: %ld.%ld but was %ld.%ld\n", expected->tv_sec, expected->tv_usec, actual->tv_sec, actual->tv_usec);
		return false;
	}
}

static const u_long HALF = 2147483648UL;


TEST(tstotv, Seconds) {
	const l_fp input = {{50}, 0}; // 50.0 s
	const struct timeval expected = {50, 0};
	struct timeval actual;

	TSTOTV(&input, &actual);

	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));
}

TEST(tstotv, MicrosecondsExact) {
	const l_fp input = {{50}, HALF}; // 10.5 s
	const struct timeval expected = {50, 500000};
	struct timeval actual;

	TSTOTV(&input, &actual);

	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));

}

TEST(tstotv, MicrosecondsRounding) {
	const l_fp input = {{50}, 3865471UL}; // Should round to 50.0009
	const struct timeval expected = {50, 900};
	struct timeval actual;

	TSTOTV(&input, &actual);

	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));
}

TEST_GROUP_RUNNER(tstotv) {
	RUN_TEST_CASE(tstotv, Seconds);
	RUN_TEST_CASE(tstotv, MicrosecondsExact);
	RUN_TEST_CASE(tstotv, MicrosecondsRounding);
}
