#include "config.h"

#include "unity.h"
#include "unity_fixture.h"

#include "ntp_filegen.h"


FILEGEN day, week, month;

TEST_GROUP(filegen);

TEST_SETUP(filegen) {
	filegen_register("/tmp/foo/", "day", &day);
	filegen_register("/tmp/foo/", "week", &week);
	filegen_register("/tmp/foo/", "month", &month);
}

TEST_TEAR_DOWN(filegen) {
#ifdef DEBUG
	filegen_unregister("day");
	filegen_unregister("week");
	filegen_unregister("month");
#endif
}


// =====================================================================
// VALIDATION TESTS
// =====================================================================

TEST(filegen, Day) {
	time_t time;
//	TEST_ASSERT_EQUAL(LSVALID_GOODHASH, rc);
}

TEST(filegen, Week) {
//	const char *cp = leap2;
//	int         rc = leapsec_validate(stringreader, &cp);
//	TEST_ASSERT_EQUAL(LSVALID_NOHASH, rc);
}

TEST(filegen, Month) {
//	const char *cp = leap_bhash;
//	int         rc = leapsec_validate(stringreader, &cp);
//	TEST_ASSERT_EQUAL(LSVALID_BADHASH, rc);
}




TEST_GROUP_RUNNER(filegen) {
	RUN_TEST_CASE(filegen, Day);
	RUN_TEST_CASE(filegen, Week);
	RUN_TEST_CASE(filegen, Month);
}
