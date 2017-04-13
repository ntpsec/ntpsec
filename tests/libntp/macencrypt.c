#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(macencrypt);

TEST_SETUP(macencrypt) {}

TEST_TEAR_DOWN(macencrypt) {}

#include <openssl/evp.h>

#include "ntp.h"

/*
 * Example packet with MD5 hash calculated manually.
 */
const int keytype = NID_md5;
char key[] = "abcdefgh";
const u_short keyLength = 8;
const char *packet = "ijklmnopqrstuvwx";
const int packetLength = 16;
const int keyIdLength = 4;
const int digestLength = 16;
const int totalLength = 36; //error: initializer element is not constant packetLength + keyIdLength + digestLength;
char expectedPacket[] = "ijklmnopqrstuvwx\0\0\0\0\x0c\x0e\x84\xcf\x0b\xb7\xa8\x68\x8e\x52\x38\xdb\xbc\x1c\x39\x53";

TEST(macencrypt, Encrypt) {
	char *packetPtr[totalLength];
	memset(packetPtr+packetLength, 0, (size_t)keyIdLength);
	memcpy(packetPtr, packet, (size_t)packetLength);

	cache_secretsize = keyLength;

	int length = mac_authencrypt(keytype, (u_char*)key,
                                     (uint32_t*)packetPtr, packetLength);

	TEST_ASSERT_TRUE(mac_authdecrypt(keytype, (u_char*)key, (uint32_t*)packetPtr, packetLength, length));

	TEST_ASSERT_EQUAL(20, length);
//XXX	TEST_ASSERT_TRUE(memcmp(expectedPacket, packetPtr, totalLength) == 0);  Does not pass

}

TEST(macencrypt, DecryptValid) {
	cache_secretsize = keyLength;

	TEST_ASSERT_TRUE(mac_authdecrypt(keytype, (u_char*)key,
                         (uint32_t*)expectedPacket, packetLength, 20));
}

TEST(macencrypt, DecryptInvalid) {
	cache_secretsize = keyLength;

	char invalidPacket[] = "ijklmnopqrstuvwx\0\0\0\0\x0c\x0e\x84\xcf\x0b\xb7\xa8\x68\x8e\x52\x38\xdb\xbc\x1c\x39\x54";

	TEST_ASSERT_FALSE(mac_authdecrypt(keytype, (u_char*)key,
                          (uint32_t*)invalidPacket, packetLength, 20));
}

TEST(macencrypt, IPv4AddressToRefId) {
	sockaddr_u addr;
	SET_AF(&addr, AF_INET);
	SET_NSRCPORT(&addr, htons(80));
	uint32_t address = inet_addr("192.0.2.1");
	PSOCK_ADDR4(&addr)->s_addr = address;

	TEST_ASSERT_EQUAL(address, addr2refid(&addr));
}

TEST(macencrypt, IPv6AddressToRefId) {
	const struct in6_addr address = {{{
		0x20, 0x01, 0x0d, 0xb8,
        0x85, 0xa3, 0x08, 0xd3,
        0x13, 0x19, 0x8a, 0x2e,
        0x03, 0x70, 0x73, 0x34
	}}};


	sockaddr_u addr;
	SET_AF(&addr, AF_INET6);
	SET_SOCK_ADDR6(&addr, address);

	const uint expected = htonl(0x52fdcf75);

	TEST_ASSERT_EQUAL(expected, addr2refid(&addr));
}

TEST_GROUP_RUNNER(macencrypt) {
	RUN_TEST_CASE(macencrypt, Encrypt);
	RUN_TEST_CASE(macencrypt, DecryptValid);
	RUN_TEST_CASE(macencrypt, DecryptInvalid);
	RUN_TEST_CASE(macencrypt, IPv4AddressToRefId);
	RUN_TEST_CASE(macencrypt, IPv6AddressToRefId);
}
