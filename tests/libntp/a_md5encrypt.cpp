extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(a_md5encrypt);

TEST_SETUP(a_md5encrypt) {}

TEST_TEAR_DOWN(a_md5encrypt) {}

#include "libntptest.h"

extern "C" {
#ifdef OPENSSL
# include "openssl/err.h"
# include "openssl/rand.h"
# include "openssl/evp.h"
#endif
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
const u_short keyLength = 8;
const char *packet = "ijklmnopqrstuvwx";
const int packetLength = 16;
const int keyIdLength = 4;
const int digestLength = 16;
const int totalLength = packetLength + keyIdLength + digestLength;
const char *expectedPacket = "ijklmnopqrstuvwx\0\0\0\0\x0c\x0e\x84\xcf\x0b\xb7\xa8\x68\x8e\x52\x38\xdb\xbc\x1c\x39\x53";

TEST(a_md5encrypt, Encrypt) {
	char *packetPtr = new char[totalLength];
	memset(packetPtr+packetLength, 0, keyIdLength);
	memcpy(packetPtr, packet, packetLength);

	cache_secretsize = keyLength;

	int length =  MD5authencrypt(keytype, (u_char*)key, (u_int32*)packetPtr, packetLength);

	TEST_ASSERT_TRUE(MD5authdecrypt(keytype, (u_char*)key, (u_int32*)packetPtr, packetLength, length));

	TEST_ASSERT_EQUAL(20, length);
	TEST_ASSERT_TRUE(memcmp(expectedPacket, packetPtr, totalLength) == 0);

	delete[] packetPtr;
}

TEST(a_md5encrypt, DecryptValid) {
	cache_secretsize = keyLength;

	TEST_ASSERT_TRUE(MD5authdecrypt(keytype, (u_char*)key, (u_int32*)expectedPacket, packetLength, 20));
}

TEST(a_md5encrypt, DecryptInvalid) {
	cache_secretsize = keyLength;

	const char *invalidPacket = "ijklmnopqrstuvwx\0\0\0\0\x0c\x0e\x84\xcf\x0b\xb7\xa8\x68\x8e\x52\x38\xdb\xbc\x1c\x39\x54";

	TEST_ASSERT_FALSE(MD5authdecrypt(keytype, (u_char*)key, (u_int32*)invalidPacket, packetLength, 20));
}

TEST(a_md5encrypt, IPv4AddressToRefId) {
	sockaddr_u addr;
	addr.sa4.sin_family = AF_INET;
	addr.sa4.sin_port = htons(80);

	u_int32 address = inet_addr("192.0.2.1");
	addr.sa4.sin_addr.s_addr = address;

	TEST_ASSERT_EQUAL(address, addr2refid(&addr));
}

TEST(a_md5encrypt, IPv6AddressToRefId) {
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

	TEST_ASSERT_EQUAL(expected, addr2refid(&addr));
}
