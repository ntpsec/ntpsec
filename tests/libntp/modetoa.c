#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(modetoa);

TEST_SETUP(modetoa) {}

TEST_TEAR_DOWN(modetoa) {}



TEST(modetoa, KnownMode) {
	const int MODE = 3; // Should be "client"

	TEST_ASSERT_EQUAL_STRING("client", modetoa(MODE));
}

TEST(modetoa, UnknownMode) {
	const int MODE = 100;

	TEST_ASSERT_EQUAL_STRING("mode#100", modetoa(MODE));
}

TEST_GROUP_RUNNER(modetoa) {
	RUN_TEST_CASE(modetoa, KnownMode);
	RUN_TEST_CASE(modetoa, UnknownMode);
}
