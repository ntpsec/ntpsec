#include "sntptest.h"

extern "C" {
#include "networking.h"
#include "ntp_stdlib.h"
#include "sntp-opts.h"
};

#include <sstream>
#include <string>

class networkingTest : public sntptest {
};

// Hacks into the key database.
extern key* key_ptr;
extern int key_cnt;

class packetProcessingTest : public networkingTest {
protected:
	pkt testpkt;
	sockaddr_u testsock;
	bool restoreKeyDb;

	void ActivateOption(const char* option, const char* argument) {
		const int ARGV_SIZE = 4;

		char* opts[ARGV_SIZE];
		
		opts[0] = estrdup("sntpopts");
		opts[1] = estrdup(option);
		opts[2] = estrdup(argument);
		opts[3] = estrdup("127.0.0.1");

		optionProcess(&sntpOptions, ARGV_SIZE, opts);
	}

	void PrepareAuthenticationTest(int key_id,
								  int key_len,
								  void* key_seq) {
		std::stringstream ss;
		ss << key_id;

		ActivateOption("-a", ss.str().c_str());

		key_cnt = 1;
		key_ptr = new key;
		key_ptr->next = NULL;
		key_ptr->key_id = key_id;
		key_ptr->key_len = key_len;
		memcpy(key_ptr->type, "MD5", 3);
		memcpy(key_ptr->key_seq, key_seq, key_ptr->key_len);
		restoreKeyDb = true;
	}
								  

	virtual void SetUp() {
		optionSaveState(&sntpOptions);
		restoreKeyDb = false;

		/* Initialize the test packet and socket,
		 * so they contain at least some valid data. */
		testpkt.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOWARNING, NTP_VERSION,
											MODE_SERVER);
		testpkt.stratum = STRATUM_REFCLOCK;
		memcpy(&testpkt.refid, "GPS\0", 4);
	}

	virtual void TearDown() {
		optionRestore(&sntpOptions);

		if (restoreKeyDb) {
			key_cnt = 0;
			delete key_ptr;
			key_ptr = NULL;
		}
	}
};

TEST_F(packetProcessingTest, TooShortLength) {
	EXPECT_EQ(PACKET_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC - 1,
						  MODE_SERVER, "UnitTest"));
	EXPECT_EQ(PACKET_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC - 1,
						  MODE_BROADCAST, "UnitTest"));
}

TEST_F(packetProcessingTest, LengthNotMultipleOfFour) {
	EXPECT_EQ(PACKET_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC + 6,
						  MODE_SERVER, "UnitTest"));
	EXPECT_EQ(PACKET_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC + 3,
						  MODE_BROADCAST, "UnitTest"));
}

TEST_F(packetProcessingTest, TooShortExtensionFieldLength) {
	/* The lower 16-bits are the length of the extension field.
	 * This lengths must be multiples of 4 bytes, which gives
	 * a minimum of 4 byte extension field length. */
	testpkt.exten[7] = htonl(3); // 3 bytes is too short.

	/* We send in a pkt_len of header size + 4 byte extension
	 * header + 24 byte MAC, this prevents the length error to
	 * be caught at an earlier stage */
	int pkt_len = LEN_PKT_NOMAC + 4 + 24;

	EXPECT_EQ(PACKET_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, pkt_len,
						  MODE_SERVER, "UnitTest"));
}

TEST_F(packetProcessingTest, UnauthenticatedPacketReject) {
	// Activate authentication option
	ActivateOption("-a", "123");
	ASSERT_TRUE(ENABLED_OPT(AUTHENTICATION));

	int pkt_len = LEN_PKT_NOMAC;

	// We demand authentication, but no MAC header is present.
	EXPECT_EQ(SERVER_AUTH_FAIL,
			  process_pkt(&testpkt, &testsock, pkt_len,
						  MODE_SERVER, "UnitTest"));
}

TEST_F(packetProcessingTest, CryptoNAKPacketReject) {
	// Activate authentication option
	ActivateOption("-a", "123");
	ASSERT_TRUE(ENABLED_OPT(AUTHENTICATION));

	int pkt_len = LEN_PKT_NOMAC + 4; // + 4 byte MAC = Crypto-NAK

	EXPECT_EQ(SERVER_AUTH_FAIL,
			  process_pkt(&testpkt, &testsock, pkt_len,
						  MODE_SERVER, "UnitTest"));
}

TEST_F(packetProcessingTest, AuthenticatedPacketInvalid) {
	// Activate authentication option
	ActivateOption("-a", "50");
	ASSERT_TRUE(ENABLED_OPT(AUTHENTICATION));
	
	// Hack into the key database.
	extern key* key_ptr;
	extern int key_cnt;

	key_cnt = 1;
	key_ptr = new key;
	key_ptr->next = NULL;
	key_ptr->key_id = 50;
	key_ptr->key_len = 9;
	memcpy(key_ptr->type, "MD5", 3);
	memcpy(key_ptr->key_seq, "123456789", key_ptr->key_len);

	// Prepare the packet.
	int pkt_len = LEN_PKT_NOMAC;

	testpkt.exten[0] = htonl(50);
	int mac_len = make_mac((char*)&testpkt, pkt_len,
						   MAX_MD5_LEN, key_ptr,
						   (char*)&testpkt.exten[1]);

	pkt_len += 4 + mac_len;

	// Now, alter the MAC so it becomes invalid.
	testpkt.exten[1] += 1;

	EXPECT_EQ(SERVER_AUTH_FAIL,
			  process_pkt(&testpkt, &testsock, pkt_len,
						  MODE_SERVER, "UnitTest"));

	key_cnt = 0;
	delete key_ptr;
	key_ptr = NULL;
}

TEST_F(packetProcessingTest, AuthenticatedPacketUnknownKey) {
	// Activate authentication option
	ActivateOption("-a", "30");
	ASSERT_TRUE(ENABLED_OPT(AUTHENTICATION));
	
	// Hack into the key database.
	extern key* key_ptr;
	extern int key_cnt;

	key_cnt = 1;
	key_ptr = new key;
	key_ptr->next = NULL;
	key_ptr->key_id = 30;
	key_ptr->key_len = 9;
	memcpy(key_ptr->type, "MD5", 3);
	memcpy(key_ptr->key_seq, "123456789", key_ptr->key_len);

	// Prepare the packet. Observe that the Key-ID expected is 30,
	// but the packet has a key id of 50.
	int pkt_len = LEN_PKT_NOMAC;

	testpkt.exten[0] = htonl(50);
	int mac_len = make_mac((char*)&testpkt, pkt_len,
						   MAX_MD5_LEN, key_ptr,
						   (char*)&testpkt.exten[1]);
	pkt_len += 4 + mac_len;

	printf("lens: %d %d\n", pkt_len, mac_len);

	EXPECT_EQ(SERVER_AUTH_FAIL,
			  process_pkt(&testpkt, &testsock, pkt_len,
						  MODE_SERVER, "UnitTest"));
}

TEST_F(packetProcessingTest, ServerVersionTooOld) {
	ASSERT_FALSE(ENABLED_OPT(AUTHENTICATION));

	testpkt.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOWARNING,
										NTP_OLDVERSION - 1,
										MODE_CLIENT);
	ASSERT_LT(PKT_VERSION(testpkt.li_vn_mode), NTP_OLDVERSION);

	int pkt_len = LEN_PKT_NOMAC;
	
	EXPECT_EQ(SERVER_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, pkt_len,
						  MODE_SERVER, "UnitTest"));
}

TEST_F(packetProcessingTest, ServerVersionTooNew) {
	ASSERT_FALSE(ENABLED_OPT(AUTHENTICATION));

	testpkt.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOWARNING,
										NTP_VERSION + 1,
										MODE_CLIENT);
	ASSERT_GT(PKT_VERSION(testpkt.li_vn_mode), NTP_VERSION);

	int pkt_len = LEN_PKT_NOMAC;

	EXPECT_EQ(SERVER_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, pkt_len,
						  MODE_SERVER, "UnitTest"));
}

TEST_F(packetProcessingTest, NonWantedMode) {
	ASSERT_FALSE(ENABLED_OPT(AUTHENTICATION));

	testpkt.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOWARNING,
										NTP_VERSION,
										MODE_CLIENT);

	// The packet has a mode of MODE_CLIENT, but process_pkt expects MODE_SERVER

	EXPECT_EQ(SERVER_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC,
						  MODE_SERVER, "UnitTest"));
}

/* Tests bug 1597 */
TEST_F(packetProcessingTest, KoDRate) {
	ASSERT_FALSE(ENABLED_OPT(AUTHENTICATION));

	testpkt.stratum = STRATUM_PKT_UNSPEC;
	memcpy(&testpkt.refid, "RATE", 4);

	EXPECT_EQ(KOD_RATE,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC,
						  MODE_SERVER, "UnitTest"));
}

TEST_F(packetProcessingTest, KoDDeny) {
	ASSERT_FALSE(ENABLED_OPT(AUTHENTICATION));

	testpkt.stratum = STRATUM_PKT_UNSPEC;
	memcpy(&testpkt.refid, "DENY", 4);

	EXPECT_EQ(KOD_DEMOBILIZE,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC,
						  MODE_SERVER, "UnitTest"));
}

TEST_F(packetProcessingTest, RejectUnsyncedServer) {
	ASSERT_FALSE(ENABLED_OPT(AUTHENTICATION));

	testpkt.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOTINSYNC,
										NTP_VERSION,
										MODE_SERVER);

	EXPECT_EQ(SERVER_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC,
						  MODE_SERVER, "UnitTest"));
}

TEST_F(packetProcessingTest, CorrectUnauthenticatedPacket) {
	ASSERT_FALSE(ENABLED_OPT(AUTHENTICATION));

	EXPECT_EQ(LEN_PKT_NOMAC,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC,
						  MODE_SERVER, "UnitTest"));
}

TEST_F(packetProcessingTest, CorrectAuthenticatedPacket) {
	ActivateOption("-a", "50");
	ASSERT_TRUE(ENABLED_OPT(AUTHENTICATION));



	EXPECT_EQ(LEN_PKT_NOMAC,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC,
						  MODE_SERVER, "UnitTest"));
}
