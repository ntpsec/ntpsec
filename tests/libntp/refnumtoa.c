#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

#include "ntp_net.h"
#include "ntp_refclock.h"

TEST_GROUP(refnumtoa);

TEST_SETUP(refnumtoa) {}

TEST_TEAR_DOWN(refnumtoa) {}


/* Might need to be updated if a new refclock gets this id. */
static const int UNUSED_REFCLOCK_ID = 250;


TEST(refnumtoa, LocalClock) {
	/* We test with a refclock address of type LOCALCLOCK.
	 * with id 8
	 */
	u_int32_t addr = REFCLOCK_ADDR;
	addr |= REFCLK_LOCALCLOCK << 8;
	addr |= 0x8;

	sockaddr_u address;
	address.sa4.sin_family = AF_INET;
	address.sa4.sin_addr.s_addr = htonl(addr);

	char expected[255];
	snprintf(expected, sizeof(expected), "%s(8)", clockname(REFCLK_LOCALCLOCK));

	TEST_ASSERT_EQUAL_STRING(expected, refnumtoa(&address));
}

TEST(refnumtoa, UnknownId) {
	/* We test with a currently unused refclock ID */
	u_int32_t addr = REFCLOCK_ADDR;
	addr |= UNUSED_REFCLOCK_ID << 8;
	addr |= 0x4;

	sockaddr_u address;
	address.sa4.sin_family = AF_INET;
	address.sa4.sin_addr.s_addr = htonl(addr);

	char expected[255];
	snprintf(expected, sizeof(expected), "REFCLK(%d,4)", UNUSED_REFCLOCK_ID);

	TEST_ASSERT_EQUAL_STRING(expected, refnumtoa(&address));
}


TEST_GROUP_RUNNER(refnumtoa) {
	RUN_TEST_CASE(refnumtoa, LocalClock);
	RUN_TEST_CASE(refnumtoa, UnknownId);
}
