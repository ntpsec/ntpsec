#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(statestr);

TEST_SETUP(statestr) {}

TEST_TEAR_DOWN(statestr) {}


#include "ntp.h" // Needed for MAX_MAC_LEN used in ntp_control.h
#include "ntp_control.h"


// eventstr()
TEST(statestr, PeerRestart) {
	TEST_ASSERT_EQUAL_STRING("restart", eventstr(PEVNT_RESTART));
}

TEST(statestr, SysUnspecified) {
	TEST_ASSERT_EQUAL_STRING("unspecified", eventstr(EVNT_UNSPEC));
}

// ceventstr()
TEST(statestr, ClockCodeExists) {
	TEST_ASSERT_EQUAL_STRING("clk_unspec", ceventstr(CTL_CLK_OKAY));
}

TEST(statestr, ClockCodeUnknown) {
	TEST_ASSERT_EQUAL_STRING("clk_-1", ceventstr(-1));
}

TEST_GROUP_RUNNER(statestr) {
	RUN_TEST_CASE(statestr, PeerRestart);
	RUN_TEST_CASE(statestr, SysUnspecified);
	RUN_TEST_CASE(statestr, ClockCodeExists);
	RUN_TEST_CASE(statestr, ClockCodeUnknown);
}
