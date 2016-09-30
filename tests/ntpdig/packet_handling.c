#include "unity.h"
#include "unity_fixture.h"

#include "ntpdigtest.h"

#include "kod_management.h"
#include "main.h"
#include "networking.h"
#include "ntp.h"

#include <sys/time.h>
#include <stdlib.h>


#define EXPECT_DOUBLE_EQ(a, b) { \
	static const double epsilon = 1e-10; \
	TEST_ASSERT_TRUE(fabs(a - b) < epsilon); \
}

static bool LfpEquality(const l_fp* expected, const l_fp* actual) {
	if (L_ISEQU(expected, actual)) {
		return true;
	} else {
		printf(" expected: %s (%u.%u) but was %s (%u.%u)",
			   lfptoa(expected, FRACTION_PREC),
			   expected->l_ui, expected->l_uf,
			   lfptoa(actual, FRACTION_PREC),
			   actual->l_ui, actual->l_uf);
		return false;
	}
}

TEST_GROUP(packetHandling);

TEST_SETUP(packetHandling) {}

TEST_TEAR_DOWN(packetHandling) {}

TEST(packetHandling, GenerateUnauthenticatedPacket) {

	struct pkt testpkt;
	struct timespec xmt;
	l_fp expected_xmt, actual_xmt;

	clock_gettime(CLOCK_REALTIME, &xmt);
	xmt.tv_sec += JAN_1970;

	TEST_ASSERT_EQUAL(LEN_PKT_NOMAC,
			  generate_pkt(&testpkt, &xmt, 0, NULL));

	TEST_ASSERT_EQUAL(LEAP_NOTINSYNC, PKT_LEAP(testpkt.li_vn_mode));
	TEST_ASSERT_EQUAL(NTP_VERSION, PKT_VERSION(testpkt.li_vn_mode));
	TEST_ASSERT_EQUAL(MODE_CLIENT, PKT_MODE(testpkt.li_vn_mode));

	TEST_ASSERT_EQUAL(STRATUM_UNSPEC, PKT_TO_STRATUM(testpkt.stratum));
	TEST_ASSERT_EQUAL(8, testpkt.ppoll);

	expected_xmt = tspec_stamp_to_lfp(xmt);
	NTOHL_FP(&testpkt.xmt, &actual_xmt);
	TEST_ASSERT_TRUE(LfpEquality(&expected_xmt, &actual_xmt));
}

TEST(packetHandling, GenerateAuthenticatedPacket) {
	struct key testkey;
	struct pkt testpkt;
	struct timespec xmt;
	const int EXPECTED_PKTLEN = LEN_PKT_NOMAC + MAX_MD5_LEN;
	l_fp expected_xmt, actual_xmt;
	char expected_mac[MAX_MD5_LEN];

	testkey.next = NULL;
	testkey.key_id = 30;
	testkey.key_len = 9;
	memcpy(testkey.key_seq, "123456789", testkey.key_len);
	memcpy(testkey.type, "MD5", 3);

	clock_gettime(CLOCK_REALTIME, &xmt);
	xmt.tv_sec += JAN_1970;

	TEST_ASSERT_EQUAL(EXPECTED_PKTLEN,
			  generate_pkt(&testpkt, &xmt, testkey.key_id, &testkey));

	TEST_ASSERT_EQUAL(LEAP_NOTINSYNC, PKT_LEAP(testpkt.li_vn_mode));
	TEST_ASSERT_EQUAL(NTP_VERSION, PKT_VERSION(testpkt.li_vn_mode));
	TEST_ASSERT_EQUAL(MODE_CLIENT, PKT_MODE(testpkt.li_vn_mode));

	TEST_ASSERT_EQUAL(STRATUM_UNSPEC, PKT_TO_STRATUM(testpkt.stratum));
	TEST_ASSERT_EQUAL(8, testpkt.ppoll);

	expected_xmt = tspec_stamp_to_lfp(xmt);
	NTOHL_FP(&testpkt.xmt, &actual_xmt);
	TEST_ASSERT_TRUE(LfpEquality(&expected_xmt, &actual_xmt));

	TEST_ASSERT_EQUAL(testkey.key_id, ntohl(testpkt.exten[0]));

	TEST_ASSERT_EQUAL(MAX_MD5_LEN - 4, // Remove the key_id, only keep the mac.
			  make_mac((char*)&testpkt, LEN_PKT_NOMAC, MAX_MD5_LEN, &testkey, expected_mac));
	TEST_ASSERT_TRUE(memcmp(expected_mac, (char*)&testpkt.exten[1], MAX_MD5_LEN -4) == 0);
}

TEST(packetHandling, OffsetCalculationPositiveOffset) {
	struct pkt rpkt;
	l_fp reftime;
	l_fp tmp;
	struct timespec dst;
	double offset, precision, synch_distance;

	rpkt.precision = -16; // 0,000015259
	rpkt.rootdelay = HTONS_FP(DTOUFP(0.125));
	rpkt.rootdisp = HTONS_FP(DTOUFP(0.25));
	/* Synch Distance: (0.125+0.25)/2.0 == 0.1875 */
	get_systime(&reftime);
	HTONL_FP(&reftime, &rpkt.reftime);

	/* T1 - Originate timestamp */
	tmp.l_ui = 1000000000UL;
	tmp.l_uf = 0UL;
	HTONL_FP(&tmp, &rpkt.org);

	/* T2 - Receive timestamp */
	tmp.l_ui = 1000000001UL;
	tmp.l_uf = 2147483648UL;
	HTONL_FP(&tmp, &rpkt.rec);

	/* T3 - Transmit timestamp */
	tmp.l_ui = 1000000002UL;
	tmp.l_uf = 0UL;
	HTONL_FP(&tmp, &rpkt.xmt);

	/* T4 - Destination timestamp as standard timeval */
	tmp.l_ui = 1000000001UL;
	tmp.l_uf = 0UL;
	dst = lfp_stamp_to_tspec(tmp, NULL);
	dst.tv_sec -= JAN_1970;

	offset_calculation(&rpkt, LEN_PKT_NOMAC, &dst, &offset, &precision, &synch_distance);

	EXPECT_DOUBLE_EQ(1.25, offset);
	EXPECT_DOUBLE_EQ(1. / ULOGTOD(16), precision);
	/* 1.1250150000000001 ? */
	EXPECT_DOUBLE_EQ(1.125015, synch_distance);
}

TEST(packetHandling, OffsetCalculationNegativeOffset) {
	struct pkt rpkt;
	struct timespec dst;

	rpkt.precision = -1;
	rpkt.rootdelay = HTONS_FP(DTOUFP(0.5));
	rpkt.rootdisp = HTONS_FP(DTOUFP(0.5));
	// Synch Distance is (0.5+0.5)/2.0, or 0.5
	l_fp reftime;
	get_systime(&reftime);
	HTONL_FP(&reftime, &rpkt.reftime);

	l_fp tmp;

	// T1 - Originate timestamp
	tmp.l_ui = 1000000001UL;
	tmp.l_uf = 0UL;
	HTONL_FP(&tmp, &rpkt.org);

	// T2 - Receive timestamp
	tmp.l_ui = 1000000000UL;
	tmp.l_uf = 2147483648UL;
	HTONL_FP(&tmp, &rpkt.rec);

	// T3 - Transmit timestamp
	tmp.l_ui = 1000000001UL;
	tmp.l_uf = 2147483648UL;
	HTONL_FP(&tmp, &rpkt.xmt);

	// T4 - Destination timestamp as standard timeval
	tmp.l_ui = 1000000003UL;
	tmp.l_uf = 0UL;
	dst = lfp_stamp_to_tspec(tmp, NULL);
	dst.tv_sec -= JAN_1970;

	double offset, precision, synch_distance;
	offset_calculation(&rpkt, LEN_PKT_NOMAC, &dst, &offset, &precision, &synch_distance);

	EXPECT_DOUBLE_EQ(-1, offset);
	EXPECT_DOUBLE_EQ(1. / ULOGTOD(1), precision);
	EXPECT_DOUBLE_EQ(1.3333483333333334, synch_distance);
}

TEST(packetHandling, HandleUnusableServer) {
	struct pkt		rpkt;
	sockaddr_u	host;
	int		rpktl;

	ZERO(rpkt);
	ZERO(host);
	rpktl = SERVER_UNUSEABLE;
	TEST_ASSERT_EQUAL(-1, handle_pkt(rpktl, &rpkt, &host, ""));
}

TEST(packetHandling, HandleUnusablePacket) {
	struct pkt		rpkt;
	sockaddr_u	host;
	int		rpktl;

	ZERO(rpkt);
	ZERO(host);
	rpktl = PACKET_UNUSEABLE;
	TEST_ASSERT_EQUAL(1, handle_pkt(rpktl, &rpkt, &host, ""));
}

TEST(packetHandling, HandleServerAuthenticationFailure) {
	struct pkt		rpkt;
	sockaddr_u	host;
	int		rpktl;

	ZERO(rpkt);
	ZERO(host);
	rpktl = SERVER_AUTH_FAIL;
	TEST_ASSERT_EQUAL(1, handle_pkt(rpktl, &rpkt, &host, ""));
}

TEST(packetHandling, HandleKodDemobilize) {
	const char *	HOSTNAME = "192.0.2.1";
	const char *	REASON = "DENY";
	struct pkt		rpkt;
	sockaddr_u	host;
	int		rpktl;
	struct kod_entry *	entry;

	rpktl = KOD_DEMOBILIZE;
	ZERO(rpkt);
	memcpy(&rpkt.refid, REASON, 4);
	ZERO(host);
	host.sa4.sin_family = AF_INET;
	host.sa4.sin_addr.s_addr = inet_addr(HOSTNAME);

	// Test that the KOD-entry is added to the database.
	kod_init_kod_db("/dev/null", true);

	TEST_ASSERT_EQUAL(1, handle_pkt(rpktl, &rpkt, &host, HOSTNAME));

	TEST_ASSERT_EQUAL(1, search_entry(HOSTNAME, &entry));
	TEST_ASSERT_TRUE(memcmp(REASON, entry->type, 4) == 0);
	/* coverity[leaked_storage] */
}

TEST(packetHandling, HandleKodRate) {
	struct pkt		rpkt;
	sockaddr_u	host;
	int		rpktl;

	ZERO(rpkt);
	ZERO(host);
	rpktl = KOD_RATE;
	TEST_ASSERT_EQUAL(1, handle_pkt(rpktl, &rpkt, &host, ""));
}

TEST(packetHandling, HandleCorrectPacket) {
	struct pkt		rpkt;
	sockaddr_u	host;
	int		rpktl;
	l_fp		now;

	// We don't want our testing code to actually change the system clock.
//XXX: needs updating.	TEST_ASSERT_FALSE(ENABLED_OPT(STEP));
//XXX: needs updating	TEST_ASSERT_FALSE(ENABLED_OPT(SLEW));

	get_systime(&now);
	HTONL_FP(&now, &rpkt.reftime);
	HTONL_FP(&now, &rpkt.org);
	HTONL_FP(&now, &rpkt.rec);
	HTONL_FP(&now, &rpkt.xmt);
	rpktl = LEN_PKT_NOMAC;
	ZERO(host);
	AF(&host) = AF_INET;

	TEST_ASSERT_EQUAL(0, handle_pkt(rpktl, &rpkt, &host, ""));
}

TEST_GROUP_RUNNER(packetHandling) {
	RUN_TEST_CASE(packetHandling, GenerateUnauthenticatedPacket);
	RUN_TEST_CASE(packetHandling, GenerateAuthenticatedPacket);
	RUN_TEST_CASE(packetHandling, OffsetCalculationPositiveOffset);
	RUN_TEST_CASE(packetHandling, OffsetCalculationNegativeOffset);
	RUN_TEST_CASE(packetHandling, HandleUnusableServer);
	RUN_TEST_CASE(packetHandling, HandleUnusablePacket);
	RUN_TEST_CASE(packetHandling, HandleServerAuthenticationFailure);
	RUN_TEST_CASE(packetHandling, HandleKodDemobilize);
	RUN_TEST_CASE(packetHandling, HandleKodRate);
	RUN_TEST_CASE(packetHandling, HandleCorrectPacket);
}
