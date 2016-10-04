#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(ntpvis);

#include "ntpd.h"
#include "timespecops.h"

static struct pkt ExamplePacket1 = {
    .li_vn_mode = 6,
    .stratum = 2,
    .ppoll = 3,
    .precision = -21,
    .rootdelay = 0,
    .rootdisp = 0,
    .refid = 0x47505300,	/* big-endian 'GPS\0' */
    .exten = {0},
};
static char *ExampleDump1 = "6:2:3:-21:0:0:47505300:1474021718.5261510001:0.0000000000:0.0000000000:0.0000000000:nomac";

/* same as ExamplePacket1 but with 4 extension bytes */
static struct pkt ExamplePacket2 = {
    .li_vn_mode = 6,
    .stratum = 2,
    .ppoll = 3,
    .precision = -21,
    .rootdelay = 0,
    .rootdisp = 0,
    .refid = 0x47505300,	/* big-endian 'GPS\0' */
    .exten = {0x01020304},
};
static char *ExampleDump2 = "6:2:3:-21:0:0:47505300:1474021718.5261510001:0.0000000000:0.0000000000:0.0000000000:01020304";

TEST_SETUP(ntpvis) {
    /* becomes lfp 1474021718.5261510001 */
    struct timespec reftime = {1474021718, 526151000};
    ExamplePacket2.reftime = ExamplePacket1.reftime = tspec_intv_to_lfp(reftime);
}

TEST_TEAR_DOWN(ntpvis) {}

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

TEST(ntpvis, Roundtrip1) {
    char buf1[BUFSIZ], buf2[BUFSIZ], bin[BUFSIZ];
    size_t binlen;

    packet_dump(buf1, sizeof(buf1), &ExamplePacket1, LEN_PKT_NOMAC);
    binlen = packet_undump(bin, sizeof(bin), buf1);
    packet_dump(buf2, sizeof(buf2), (struct pkt *)bin, binlen);
    TEST_ASSERT_EQUAL_STRING(buf1, buf2);
}

TEST(ntpvis, Roundtrip2) {
    char buf1[BUFSIZ], buf2[BUFSIZ], bin[BUFSIZ];
    size_t binlen;

    packet_dump(buf1, sizeof(buf1), &ExamplePacket2, LEN_PKT_NOMAC);
    binlen = packet_undump(bin, sizeof(bin), buf1);
    packet_dump(buf2, sizeof(buf2), (struct pkt *)bin, binlen);
    TEST_ASSERT_EQUAL_STRING(buf1, buf2);
}

TEST_GROUP_RUNNER(ntpvis) {
    RUN_TEST_CASE(ntpvis, PacketDump1);
    RUN_TEST_CASE(ntpvis, PacketDump2);
    RUN_TEST_CASE(ntpvis, Roundtrip1);
    RUN_TEST_CASE(ntpvis, Roundtrip2);
}
