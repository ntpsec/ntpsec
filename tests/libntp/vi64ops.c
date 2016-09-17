#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(vi64ops);

TEST_SETUP(vi64ops) {}

TEST_TEAR_DOWN(vi64ops) {}


#include "vint64ops.h"

bool IsEqual(const vint64 *expected, const vint64 *actual) {
	if (0 == memcmp(expected, actual, sizeof(vint64))) {
		return true;
	} else {
		printf("Expected: %04x.%04x but was: %04x.%04x\n", vint64hiu(*expected), vint64lo(*expected), vint64hiu(*actual), vint64lo(*actual));
		return false;
	}
}

// ----------------------------------------------------------------------
// test number parser
TEST(vi64ops, ParseVUI64_pos) {
	vint64 act, exp;
	const char *sp;
	char       *ep;

	sp         = "1234x";
	setvint64hiu(exp, 0);
	setvint64lo(exp, 1234);
	act        = strtouv64(sp, &ep, 0);
	TEST_ASSERT_TRUE(IsEqual(&exp, &act));
	TEST_ASSERT_EQUAL(*ep, 'x');
}

TEST(vi64ops, ParseVUI64_neg) {
	vint64 act, exp;
	const char *sp;
	char       *ep;

	sp         = "-1234x";
	setvint64hiu(exp, ~0);
	setvint64lo(exp, -1234);
	act        = strtouv64(sp, &ep, 0);
	TEST_ASSERT_TRUE(IsEqual(&exp, &act));
	TEST_ASSERT_EQUAL(*ep, 'x');
}

TEST(vi64ops, ParseVUI64_case) {
	vint64 act, exp;
	const char *sp;
	char       *ep;

	sp         = "0123456789AbCdEf";
	setvint64hiu(exp, 0x01234567);
	setvint64lo(exp, 0x89ABCDEF);
	act        = strtouv64(sp, &ep, 16);
	TEST_ASSERT_TRUE(IsEqual(&exp, &act));
	TEST_ASSERT_EQUAL(*ep, '\0');
}


TEST_GROUP_RUNNER(vi64ops) {
	RUN_TEST_CASE(vi64ops, ParseVUI64_pos);
	RUN_TEST_CASE(vi64ops, ParseVUI64_neg);
	RUN_TEST_CASE(vi64ops, ParseVUI64_case);
}
