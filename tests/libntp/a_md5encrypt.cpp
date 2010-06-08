#include "libntptest.h"

extern "C" {
#include "ntp.h"
#include "ntp_stdlib.h"
};

class a_md5encryptTest : public libntptest {
protected:
};

/*
 * Example packet with MD5 hash calculated manually.
 */
const int keytype = KEY_TYPE_MD5;
const char *key = "abcdefgh";
const int keylength = 8;
const char *packet = "ijklmnopqrstuvwx";
const int packetlength = 16;
const char *expectedPacket = "ijklmnopqrstuvwx0000\x0c\x0e\x84\xcf\x0b\xb7\xa8\x68\x8e\x52\x38\xdb\xbc\x1c\x39\x53";

TEST_F(a_md5encryptTest, Encrypt) {
	char *packetPtr = new char[16+4+16]; // Data + keyid + digest
	memcpy(packetPtr, packet, 16);
	memcpy(packetPtr+16, "0000", 4);

	cache_keylen = keylength;

	int length =  MD5authencrypt(KEY_TYPE_MD5, (u_char*)key, (u_int32*)packetPtr, packetlength);
	
	EXPECT_EQ(20, length);

	EXPECT_STREQ(expectedPacket, packetPtr);

	delete[] packetPtr;
}

TEST_F(a_md5encryptTest, DecryptValid) {
	cache_keylen = keylength;

	EXPECT_TRUE(MD5authdecrypt(keytype, (u_char*)key, (u_int32*)expectedPacket, packetlength, 20));
}

TEST_F(a_md5encryptTest, DecryptInvalid) {
	const char *invalidPacket = "ijklmnopqrstuvwx0000\x0c\x0e\x84\xcf\x0b\xb7\xa8\x68\x8e\x52\x38\xdb\xbc\x1c\x39\x54";
	
	EXPECT_FALSE(MD5authdecrypt(keytype, (u_char*)key, (u_int32*)invalidPacket, packetlength, 20));
}

TEST_F(a_md5encryptTest, IPv4AddressToRefId) {
	sockaddr_u addr;
	addr.sa4.sin_family = AF_INET;
	addr.sa4.sin_port = htons(80);

	u_int32 address = inet_addr("192.0.2.1");
	addr.sa4.sin_addr.s_addr = address;

	EXPECT_EQ(address, addr2refid(&addr));
}

TEST_F(a_md5encryptTest, IPv6AddressToRefId) {
	const struct in6_addr address = {
		0x20, 0x01, 0x0d, 0xb8,
        0x85, 0xa3, 0x08, 0xd3, 
        0x13, 0x19, 0x8a, 0x2e,
        0x03, 0x70, 0x73, 0x34
	};


	sockaddr_u addr;
	addr.sa6.sin6_family = AF_INET6;
	
	addr.sa6.sin6_addr = address;

	const int expected = 0x75cffd52;

	EXPECT_EQ(expected, addr2refid(&addr));
}
