#include "sntptest.h"

extern "C" {
#include "kod_management.h"
#include "main.h"
#include "networking.h"
#include "ntp.h"
};

class mainTest : public sntptest {
protected:
	::testing::AssertionResult LfpEquality(const l_fp &expected, const l_fp &actual) {
		if (L_ISEQU(&expected, &actual)) {
			return ::testing::AssertionSuccess();
		} else {
			return ::testing::AssertionFailure()
				<< " expected: " << lfptoa(&expected, FRACTION_PREC)
				<< " (" << expected.l_ui << "." << expected.l_uf << ")"
				<< " but was: " << lfptoa(&actual, FRACTION_PREC)
				<< " (" << actual.l_ui << "." << actual.l_uf << ")";
		}
	}
};

TEST_F(mainTest, GenerateUnauthenticatedPacket) {
	pkt testpkt;

	timeval xmt;
	GETTIMEOFDAY(&xmt, NULL);
	xmt.tv_sec += JAN_1970;

	EXPECT_EQ(LEN_PKT_NOMAC,
			  generate_pkt(&testpkt, &xmt, 0, NULL));

	EXPECT_EQ(LEAP_NOTINSYNC, PKT_LEAP(testpkt.li_vn_mode));
	EXPECT_EQ(NTP_VERSION, PKT_VERSION(testpkt.li_vn_mode));
	EXPECT_EQ(MODE_CLIENT, PKT_MODE(testpkt.li_vn_mode));

	EXPECT_EQ(STRATUM_UNSPEC, PKT_TO_STRATUM(testpkt.stratum));
	EXPECT_EQ(8, testpkt.ppoll);

	l_fp expected_xmt, actual_xmt;
	TVTOTS(&xmt, &expected_xmt);
	NTOHL_FP(&testpkt.xmt, &actual_xmt);
	EXPECT_TRUE(LfpEquality(expected_xmt, actual_xmt));
}

TEST_F(mainTest, GenerateAuthenticatedPacket) {
	key testkey;
	testkey.next = NULL;
	testkey.key_id = 30;
	testkey.key_len = 9;
	memcpy(testkey.key_seq, "123456789", testkey.key_len);
	memcpy(testkey.type, "MD5", 3);

	pkt testpkt;

	timeval xmt;
	GETTIMEOFDAY(&xmt, NULL);
	xmt.tv_sec += JAN_1970;

	const int EXPECTED_PKTLEN = LEN_PKT_NOMAC + MAX_MD5_LEN;

	EXPECT_EQ(EXPECTED_PKTLEN,
			  generate_pkt(&testpkt, &xmt, testkey.key_id, &testkey));

	EXPECT_EQ(LEAP_NOTINSYNC, PKT_LEAP(testpkt.li_vn_mode));
	EXPECT_EQ(NTP_VERSION, PKT_VERSION(testpkt.li_vn_mode));
	EXPECT_EQ(MODE_CLIENT, PKT_MODE(testpkt.li_vn_mode));

	EXPECT_EQ(STRATUM_UNSPEC, PKT_TO_STRATUM(testpkt.stratum));
	EXPECT_EQ(8, testpkt.ppoll);

	l_fp expected_xmt, actual_xmt;
	TVTOTS(&xmt, &expected_xmt);
	NTOHL_FP(&testpkt.xmt, &actual_xmt);
	EXPECT_TRUE(LfpEquality(expected_xmt, actual_xmt));

	EXPECT_EQ(testkey.key_id, ntohl(testpkt.exten[0]));
	
	char expected_mac[MAX_MD5_LEN];
	ASSERT_EQ(MAX_MD5_LEN - 4, // Remove the key_id, only keep the mac.
			  make_mac((char*)&testpkt, LEN_PKT_NOMAC, MAX_MD5_LEN, &testkey, expected_mac));
	EXPECT_TRUE(memcmp(expected_mac, (char*)&testpkt.exten[1], MAX_MD5_LEN -4) == 0);
}

TEST_F(mainTest, OffsetCalculationPositiveOffset) {
	pkt rpkt;

	rpkt.precision = -16; // 0,000015259
	rpkt.rootdelay = HTONS_FP(DTOUFP(0.125));
	rpkt.rootdisp = HTONS_FP(DTOUFP(0.25)); //
	l_fp reftime;
	get_systime(&reftime);
	HTONL_FP(&reftime, &rpkt.reftime);

	l_fp tmp;

	// T1 - Originate timestamp
	tmp.l_ui = 1000000000UL;
	tmp.l_uf = 0UL;
	HTONL_FP(&tmp, &rpkt.org);

	// T2 - Receive timestamp
	tmp.l_ui = 1000000001UL;
	tmp.l_uf = 2147483648UL;
	HTONL_FP(&tmp, &rpkt.rec);

	// T3 - Transmit timestamp
	tmp.l_ui = 1000000002UL;
	tmp.l_uf = 0UL;
	HTONL_FP(&tmp, &rpkt.xmt);

	// T4 - Destination timestamp
	tmp.l_ui = 1000000001UL;
	tmp.l_uf = 0UL;
	timeval dst;
	TSTOTV(&tmp, &dst);

	double offset, precision, root_disp;
	offset_calculation(&rpkt, LEN_PKT_NOMAC, &dst, &offset, &precision, &root_disp);

	EXPECT_DOUBLE_EQ(1.25, offset);
	EXPECT_DOUBLE_EQ(LOGTOD(-16), precision);
	EXPECT_DOUBLE_EQ(0.25, root_disp);
}

TEST_F(mainTest, OffsetCalculationNegativeOffset) {
	pkt rpkt;

	rpkt.precision = -1;
	rpkt.rootdelay = HTONS_FP(DTOUFP(0.5));
	rpkt.rootdisp = HTONS_FP(DTOUFP(0.5));
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

	// T4 - Destination timestamp
	tmp.l_ui = 1000000003UL;
	tmp.l_uf = 0UL;
	timeval dst;
	TSTOTV(&tmp, &dst);

	double offset, precision, root_disp;
	offset_calculation(&rpkt, LEN_PKT_NOMAC, &dst, &offset, &precision, &root_disp);

	EXPECT_DOUBLE_EQ(-1, offset);
	EXPECT_DOUBLE_EQ(LOGTOD(-1), precision);
	EXPECT_DOUBLE_EQ(0.5, root_disp);
}

TEST_F(mainTest, HandleUnusableServer) {
	pkt rpkt;
	addrinfo host;
	int rpktl = SERVER_UNUSEABLE;

	EXPECT_EQ(-1, handle_pkt(rpktl, &rpkt, &host));
}

TEST_F(mainTest, HandleUnusablePacket) {
	pkt rpkt;
	addrinfo host;
	int rpktl = PACKET_UNUSEABLE;

	EXPECT_EQ(1, handle_pkt(rpktl, &rpkt, &host));
}

TEST_F(mainTest, HandleServerAuthenticationFailure) {
	pkt rpkt;
	addrinfo host;
	int rpktl = SERVER_AUTH_FAIL;

	EXPECT_EQ(1, handle_pkt(rpktl, &rpkt, &host));
}

TEST_F(mainTest, HandleKodDemobilize) {
	const char* HOSTNAME = "192.0.2.1";
	const char* REASON = "DENY";

	pkt rpkt;
	addrinfo host;
	int rpktl = KOD_DEMOBILIZE;

	host.ai_family = AF_INET;
	host.ai_addrlen = sizeof(sockaddr_in);

	sockaddr_u s;
	s.sa4.sin_family = AF_INET;
	s.sa4.sin_addr.s_addr = inet_addr(HOSTNAME);
	host.ai_addr = &s.sa;

	memcpy(&rpkt.refid, REASON, 4);

	// Test that the KOD-entry is added to the database.
	kod_init_kod_db("/dev/null");

	EXPECT_EQ(1, handle_pkt(rpktl, &rpkt, &host));

	kod_entry *entry;
	ASSERT_EQ(1, search_entry((char*)HOSTNAME, &entry));
	EXPECT_TRUE(memcmp(REASON, entry->type, 4) == 0);
}

TEST_F(mainTest, HandleKodRate) {
	pkt rpkt;
	addrinfo host;
	int rpktl = KOD_RATE;

	EXPECT_EQ(1, handle_pkt(rpktl, &rpkt, &host));
}

TEST_F(mainTest, HandleCorrectPacket) {
	// We don't want our testing code to actually change the system clock.
	ASSERT_FALSE(ENABLED_OPT(SETTOD));
	ASSERT_FALSE(ENABLED_OPT(ADJTIME));

	pkt rpkt;
	addrinfo host;
	int rpktl = LEN_PKT_NOMAC;

	l_fp now;
	get_systime(&now);
	HTONL_FP(&now, &rpkt.reftime);
	HTONL_FP(&now, &rpkt.org);
	HTONL_FP(&now, &rpkt.rec);
	HTONL_FP(&now, &rpkt.xmt);

	EXPECT_EQ(0, handle_pkt(rpktl, &rpkt, &host));
}

