extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(modetoa);

TEST_SETUP(modetoa) {}

TEST_TEAR_DOWN(modetoa) {}

#include "libntptest.h"

class modetoaTest : public libntptest {
};

TEST(modetoa, KnownMode) {
	const int MODE = 3; // Should be "client"

	TEST_ASSERT_EQUAL_STRING("client", modetoa(MODE));
}

TEST(modetoa, UnknownMode) {
	const int MODE = 100;

	TEST_ASSERT_EQUAL_STRING("mode#100", modetoa(MODE));
}
