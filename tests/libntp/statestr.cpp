extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(statestr);

TEST_SETUP(statestr) {}

TEST_TEAR_DOWN(statestr) {}

#include "libntptest.h"

extern "C" {
#include "ntp.h" // Needed for MAX_MAC_LEN used in ntp_control.h
#include "ntp_control.h"
};

class statestrTest : public libntptest {
};

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
