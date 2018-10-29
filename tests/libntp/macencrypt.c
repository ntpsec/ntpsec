#include "config.h"
#include "ntp_stdlib.h"
#include "ntp_auth.h"

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
char MD5key[] = "abcdefgh";
char CMACkey[] = "0123456789abcdef";  /* AES-128 needs 16 bytes */
const char *packet = "ijklmnopqrstuvwx";
const int packetLength = 16;
const int keyIdLength = 4;
const int digestLength = 16;

/* Need #define to avoie VLA (variable length array) */
#define totalLength 36

char expectedMD5Packet[] = "ijklmnopqrstuvwx\0\0\0\0\x0c\x0e\x84\xcf\x0b\xb7\xa8\x68\x8e\x52\x38\xdb\xbc\x1c\x39\x53";
char expectedCMACPacket[] = "ijklmnopqrstuvwx\0\0\0\0\xb0\xa1\xcf\xd2\x7f\x69\x0c\x43\xa7\x5d\x6c\x55\x91\x4b\x15\x14";

auth_info auth;

TEST(macencrypt, Encrypt) {
	char packetPtr[totalLength];
	memset(packetPtr+packetLength, 0, (size_t)keyIdLength);
	memcpy(packetPtr, packet, (size_t)packetLength);

	/* FIXME: Initialization stuff. Needed by other digest tests. */
	auth.keyid = 123;
	auth.type = AUTH_DIGEST;
	auth.digest = EVP_get_digestbyname("MD5");
	auth.cipher = NULL;
	auth.key = (uint8_t *)MD5key;
	auth.key_size = (unsigned short)strlen(MD5key);

	TEST_ASSERT_NOT_NULL(auth.digest);

	int length = digest_encrypt(&auth,
		(uint32_t*)packetPtr, packetLength);

	TEST_ASSERT_EQUAL(4+16, length);

	TEST_ASSERT_TRUE(digest_decrypt(&auth,
		(uint32_t*)packetPtr, packetLength, length));

if (0) {
	int i;
        printf("\n");
	for (i = 0; i< totalLength; i++)
	  printf("%02x ", (unsigned int)expectedMD5Packet[i] & 0xff);
        printf("\n");
	for (i = 0; i< totalLength; i++)
	  printf("%02x ", (unsigned int)packetPtr[i] & 0xff);
        printf("\n");
}
	TEST_ASSERT_TRUE(memcmp(expectedMD5Packet, packetPtr, totalLength) == 0);

}
TEST(macencrypt, CMAC_Encrypt) {
	char packetPtr[totalLength];
	memset(packetPtr+packetLength, 0, (size_t)keyIdLength);
	memcpy(packetPtr, packet, (size_t)packetLength);

	/* FIXME: Initialization stuff. Needed by other CMAC tests. */
	auth.keyid = 1234;
	auth.type = AUTH_CMAC;
	auth.digest = NULL;
	auth.cipher = EVP_get_cipherbyname("AES-128-CBC");
	auth.key = (uint8_t *)CMACkey;
	auth.key_size = (unsigned short)strlen(CMACkey);

	TEST_ASSERT_NOT_NULL(auth.cipher);

	int length = cmac_encrypt(&auth,
		(uint32_t*)packetPtr, packetLength);

	TEST_ASSERT_EQUAL(4+16, length);            /* AES-128 */

	TEST_ASSERT_TRUE(cmac_decrypt(&auth,
		(uint32_t*)packetPtr, packetLength, length));

if (0) {
	int i;
        printf("\n");
	for (i = 0; i< totalLength; i++)
	  printf("%02x ", (unsigned int)expectedCMACPacket[i] & 0xff);
        printf("\n");
	for (i = 0; i< totalLength; i++)
	  printf("%02x ", (unsigned int)packetPtr[i] & 0xff);
        printf("\n");
}
	TEST_ASSERT_TRUE(memcmp(expectedCMACPacket, packetPtr, totalLength) == 0);

}

TEST(macencrypt, DecryptValid) {
	TEST_ASSERT_TRUE(digest_decrypt(&auth,
		(uint32_t*)expectedMD5Packet, packetLength, 20));
}

TEST(macencrypt, DecryptValidCMAC) {
	TEST_ASSERT_TRUE(cmac_decrypt(&auth,
		(uint32_t*)expectedCMACPacket, packetLength, 20));
}

TEST(macencrypt, DecryptInvalid) {
	char invalidPacket[] = "ijklmnopqrstuvwx\0\0\0\0\x0c\x0e\x84\xcf\x0b\xb7\xa8\x68\x8e\x52\x38\xdb\xbc\x1c\x39\x54";

	TEST_ASSERT_FALSE(digest_decrypt(&auth,
		(uint32_t*)invalidPacket, packetLength, 20));
}

TEST(macencrypt, DecryptInvalidCMAC) {
	char invalidPacket[] = "ijklmnopqrstuvwx\0\0\0\0\x0c\x0e\x84\xcf\x0b\xb7\xa8\x68\x8e\x52\x38\xdb\xbc\x1c\x39\x54";

	TEST_ASSERT_FALSE(cmac_decrypt(&auth,
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

	const unsigned int expected = htonl(0x52fdcf75);

	TEST_ASSERT_EQUAL(expected, addr2refid(&addr));
}

/* Both digest and CMAC tests share some global variables
 * that get setup by Encrypt or CMAC_Encrypt
 * Thus the tests must be run in the right order.
 */
TEST_GROUP_RUNNER(macencrypt) {
	RUN_TEST_CASE(macencrypt, Encrypt);
	RUN_TEST_CASE(macencrypt, DecryptValid);
	RUN_TEST_CASE(macencrypt, DecryptInvalid);
	RUN_TEST_CASE(macencrypt, CMAC_Encrypt);
	RUN_TEST_CASE(macencrypt, DecryptValidCMAC);
	RUN_TEST_CASE(macencrypt, DecryptInvalidCMAC);
	RUN_TEST_CASE(macencrypt, IPv4AddressToRefId);
	RUN_TEST_CASE(macencrypt, IPv6AddressToRefId);
}
