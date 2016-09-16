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
    .refid = 0x47505300,	/* big-endian 'GPS\0' */
#ifdef __unused__
    .reftime = 0,
    .org = 0,
    .rec = 0,
    .xmt = 0,
#endif
    .exten = {0},
};
static char *ExampleDump1 = "6:2:3:-21:0:0:47505300:0:0:0:0:nomac";

/* same as ExamplePacket1 but with 4 extension bytes */
static struct pkt ExamplePacket2 = {
    .li_vn_mode = 6,
    .stratum = 2,
    .ppoll = 3,
    .precision = -21,
    .rootdelay = 0,
    .rootdisp = 0,
    .refid = 0x47505300,	/* big-endian 'GPS\0' */
#ifdef __unused__
    .reftime = 0,
    .org = 0,
    .rec = 0,
    .xmt = 0,
#endif
    .exten = {0x01020304},
};
static char *ExampleDump2 = "6:2:3:-21:0:0:47505300:0:0:0:0:01020304";


TEST(ntpvis, PacketDump1) {
    char buf[BUFSIZ];

    packet_dump(buf, sizeof(buf), &ExamplePacket1, LEN_PKT_NOMAC);
    TEST_ASSERT_EQUAL_STRING(ExampleDump1, buf);
}

TEST(ntpvis, PacketDump2) {
    char buf[BUFSIZ];

    packet_dump(buf, sizeof(buf), &ExamplePacket2, LEN_PKT_NOMAC + 4);
    TEST_ASSERT_EQUAL_STRING(ExampleDump2, buf);
}

TEST_GROUP_RUNNER(ntpvis) {
    RUN_TEST_CASE(ntpvis, PacketDump1);
    RUN_TEST_CASE(ntpvis, PacketDump2);
}
