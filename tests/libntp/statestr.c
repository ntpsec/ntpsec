#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

#include <sys/timex.h>

TEST_GROUP(statestr);

TEST_SETUP(statestr) {}

TEST_TEAR_DOWN(statestr) {}


#include "ntp.h" // Needed for MAX_MAC_LEN used in ntp_control.h
#include "ntp_control.h"


// res_match_flags()
TEST(statestr, ResMatchFlags) {
	TEST_ASSERT_EQUAL_STRING("ntpport", res_match_flags(RESM_NTPONLY));
}

// res_access_flags()
TEST(statestr, ResAccessFlags) {
	TEST_ASSERT_EQUAL_STRING("notrust", res_access_flags(RES_DONTTRUST));
}

// k_st_flags()
TEST(statestr, KSTFlags) {
	TEST_ASSERT_EQUAL_STRING("ppsfreq", k_st_flags(STA_PPSFREQ));
}

// statustoa
TEST(statestr, StatusToA) {
	TEST_ASSERT_EQUAL_STRING("leap_add_sec, sync_22, 7 events, no_sys_peer",
							 statustoa(TYPE_SYS, 0x12345678));
	TEST_ASSERT_EQUAL_STRING("authenb, reach, sel_sys.peer, 7 events, access_denied",
							 statustoa(TYPE_PEER, 0x12345678));
	TEST_ASSERT_EQUAL_STRING("7 events, clk_8",
							 statustoa(TYPE_CLOCK, 0x12345678));
}

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
	RUN_TEST_CASE(statestr, ResMatchFlags);
	RUN_TEST_CASE(statestr, ResAccessFlags);
	RUN_TEST_CASE(statestr, KSTFlags);
	RUN_TEST_CASE(statestr, StatusToA);
	RUN_TEST_CASE(statestr, PeerRestart);
	RUN_TEST_CASE(statestr, SysUnspecified);
	RUN_TEST_CASE(statestr, ClockCodeExists);
	RUN_TEST_CASE(statestr, ClockCodeUnknown);
}
