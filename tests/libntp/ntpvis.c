#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(ntpvis);

TEST_SETUP(ntpvis) {}

TEST_TEAR_DOWN(ntpvis) {}

#include "ntpd.h"

static struct pkt ExamplePacket1 = {
    .li_vn_mode = 6,
    .stratum = 2,
    .ppoll = 3,
    .precision = -21,
    .rootdelay = 0,
    .rootdisp = 0,
    .refid = 0x47506300,	/* big-endian 'GPS' */
#ifdef __unused__
    .reftime = 0,
    .org = 0,
    .rec = 0,
    .xmt = 0,
#endif
    .exten = {0},
};
static char *ExampleDump1 = "6:2:3:-21:0:0:1196450560:0:0:0:0:nomac";

TEST(ntpvis, PacketDump) {
    char buf[BUFSIZ];

    packet_dump(buf, sizeof(buf), &ExamplePacket1, LEN_PKT_NOMAC);
    TEST_ASSERT_TRUE(strcmp(buf, ExampleDump1) == 0);
}

TEST_GROUP_RUNNER(ntpvis) {
    RUN_TEST_CASE(ntpvis, PacketDump);
}
