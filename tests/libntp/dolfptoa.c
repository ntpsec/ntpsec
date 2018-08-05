#include "config.h"
#include "ntp_stdlib.h"
#include "ntp_fp.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(dolfptoa);

TEST_SETUP(dolfptoa) {}

TEST_TEAR_DOWN(dolfptoa) {}


TEST(dolfptoa, DoLfpToA) {
	l_fp in;

	// Nothingness
	in = lfpinit(0, 0);
	TEST_ASSERT_EQUAL_STRING("0", dolfptoa(in, false, 0, false));
	TEST_ASSERT_EQUAL_STRING("-0", dolfptoa(in, true, 0, false));
	TEST_ASSERT_EQUAL_STRING("0", dolfptoa(in, false, 0, true));
	TEST_ASSERT_EQUAL_STRING("-0", dolfptoa(in, true, 0, true));
	// Somthingness
	in = lfpinit(42, 0x80000000);
	TEST_ASSERT_EQUAL_STRING("42.500", dolfptoa(in, false, 3, false));
	TEST_ASSERT_EQUAL_STRING("-42.500", dolfptoa(in, true, 3, false));
	TEST_ASSERT_EQUAL_STRING("42500.000", dolfptoa(in, false, 3, true));
	TEST_ASSERT_EQUAL_STRING("-42500.000", dolfptoa(in, true, 3, true));
	// Rounded up decimals
	in = lfpinit(0xFFFFFFFF, 0xFFFFFF);
	TEST_ASSERT_EQUAL_STRING("4294967295.004", dolfptoa(in, false, 3, false));
	TEST_ASSERT_EQUAL_STRING("-4294967295.004", dolfptoa(in, true, 3, false));
	TEST_ASSERT_EQUAL_STRING("4294967295003.906", dolfptoa(in, false, 3, true));
	TEST_ASSERT_EQUAL_STRING("-4294967295003.906", dolfptoa(in, true, 3, true));
	// Rounded down decimals
	in = lfpinit(0xFFFFFFFF, 0xFF000000);
	TEST_ASSERT_EQUAL_STRING("4294967295.99609",
							 dolfptoa(in, false, 5, false));
	TEST_ASSERT_EQUAL_STRING("-4294967295.99609",
							 dolfptoa(in, true, 5, false));
	TEST_ASSERT_EQUAL_STRING("4294967295996.09375",
							 dolfptoa(in, false, 5, true));
	TEST_ASSERT_EQUAL_STRING("-4294967295996.09375",
							 dolfptoa(in, true, 5, true));
	// Extended decimals
	in = lfpinit(0xFFFFFFFF, 0xFF000000);
	TEST_ASSERT_EQUAL_STRING("4294967295.99609375000000",
							 dolfptoa(in, false, 15, false));
	TEST_ASSERT_EQUAL_STRING("-4294967295.99609375000000",
							 dolfptoa(in, true, 15, false));
	TEST_ASSERT_EQUAL_STRING("4294967295996.09375000000",
							 dolfptoa(in, false, 15, true));
	TEST_ASSERT_EQUAL_STRING("-4294967295996.09375000000",
							 dolfptoa(in, true, 15, true));
}

TEST(dolfptoa, MfpToA) {
  l_fp in;

  in = lfpinit(42, 0);
  TEST_ASSERT_EQUAL_STRING("42.000", mfptoa(in, 3));
  in = lfpinit(-42, 0);
  TEST_ASSERT_EQUAL_STRING("-42.000", mfptoa(in, 3));
}

TEST(dolfptoa, MfpToMs) {
  l_fp in;

  in = lfpinit(42, 0);
  TEST_ASSERT_EQUAL_STRING("42000.000", mfptoms(in, 3));
  in = lfpinit(-42, 0);
  TEST_ASSERT_EQUAL_STRING("-42000.000", mfptoms(in, 3));
}

TEST_GROUP_RUNNER(dolfptoa) {
	RUN_TEST_CASE(dolfptoa, DoLfpToA);
	RUN_TEST_CASE(dolfptoa, MfpToA);
	RUN_TEST_CASE(dolfptoa, MfpToMs);
}
