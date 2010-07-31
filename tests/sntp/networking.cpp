#include "sntptest.h"

extern "C" {
#include "networking.h"
#include "ntp_stdlib.h"
#include "sntp-opts.h"
};

class networkingTest : public sntptest {
};

class packetProcessingTest : public networkingTest {
protected:
	void ActivateOption(const char* option, const char* argument) {
		const int ARGV_SIZE = 4;

		char* opts[ARGV_SIZE];
		
		opts[0] = estrdup("sntpopts");
		opts[1] = estrdup(option);
		opts[2] = estrdup(argument);
		opts[3] = estrdup("127.0.0.1");

		optionProcess(&sntpOptions, ARGV_SIZE, opts);
		//		ENABLE_OPT();
	}

	virtual void SetUp() {
		optionSaveState(&sntpOptions);
	}

	virtual void TearDown() {
		optionRestore(&sntpOptions);
	}
};

TEST_F(packetProcessingTest, TooShortLength) {
	pkt testpkt;
	sockaddr_u testsock;
	
	EXPECT_EQ(PACKET_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC - 1,
						  MODE_SERVER, "UnitTest"));
	EXPECT_EQ(PACKET_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC - 1,
						  MODE_BROADCAST, "UnitTest"));
}

TEST_F(packetProcessingTest, LengthNotMultipleOfFour) {
	pkt testpkt;
	sockaddr_u testsock;

	EXPECT_EQ(PACKET_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC + 6,
						  MODE_SERVER, "UnitTest"));
	EXPECT_EQ(PACKET_UNUSEABLE,
			  process_pkt(&testpkt, &testsock, LEN_PKT_NOMAC + 3,
						  MODE_BROADCAST, "UnitTest"));
}

TEST_F(packetProcessingTest, TooShortExtensionFieldLength) {
	pkt testpkt;
	sockaddr_u testsock;
	
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

	pkt testpkt;
	sockaddr_u testsock;
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

	pkt testpkt;
	sockaddr_u testsock;
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

	pkt testpkt;
	sockaddr_u testsock;

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

	pkt testpkt;
	sockaddr_u testsock;

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
