#include "config.h"
#include "ntp_stdlib.h"
#include "ntp_auth.h"

// Note that NTP doesn't use RFCs 1321 or 4631

// RFC 4493: The AES-CMAC Algorithm
//   https://www.rfc-editor.org/rfc/rfc4493


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

/* Need #define to avoid VLA (variable length array) */
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
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	auth.digest = EVP_MD_fetch(NULL, "MD5", "fips=no");
#else
	auth.digest = EVP_get_digestbyname("MD5");
#endif
#if OPENSSL_VERSION_NUMBER > 0x20000000L
	auth.mac_ctx = NULL;
#else
	auth.cipher = NULL;
#endif
	auth.key = (uint8_t *)MD5key;
	auth.key_size = (unsigned short)strlen(MD5key);

	TEST_ASSERT_NOT_NULL(auth.digest);

	int length = digest_encrypt(&auth,
				    (uint32_t*)packetPtr, packetLength);

	TEST_ASSERT_EQUAL(4+16, length);

	TEST_ASSERT_TRUE(digest_decrypt(&auth,
					(uint32_t*)packetPtr, packetLength, length));

	if (0) {
		printf("\n");
		for (int i = 0; i< totalLength; i++) {
			printf("%02x ", (unsigned int)expectedMD5Packet[i] & 0xff);
		}
		printf("\n");
		for (int i = 0; i< totalLength; i++) {
			printf("%02x ", (unsigned int)packetPtr[i] & 0xff);
		}
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
	auth.key = (uint8_t *)CMACkey;
	auth.key_size = (unsigned short)strlen(CMACkey);
#if OPENSSL_VERSION_NUMBER > 0x20000000L
	auth.mac_ctx = Setup_MAC_CTX("AES-128-CBC", auth.key, auth.key_size);
	TEST_ASSERT_NOT_NULL(auth.mac_ctx);
#else
	auth.cipher = EVP_get_cipherbyname("AES-128-CBC");
	TEST_ASSERT_NOT_NULL(auth.cipher);
#endif

	int length = cmac_encrypt(&auth,
				  (uint32_t*)packetPtr, packetLength);

	TEST_ASSERT_EQUAL(4+16, length);            /* AES-128 */

	TEST_ASSERT_TRUE(cmac_decrypt(&auth,
				      (uint32_t*)packetPtr, packetLength, length));

	if (0) {
		printf("\n");
		for (int i = 0; i< totalLength; i++) {
			printf("%02x ", (unsigned int)expectedCMACPacket[i] & 0xff);
		}
		printf("\n");
		for (int i = 0; i< totalLength; i++) {
			printf("%02x ", (unsigned int)packetPtr[i] & 0xff);
		}
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

TEST(macencrypt, null_trunc) {
	unsigned char key[] = {
		0x0f, 0xd2, 0x28, 0x7c, 0x1e, 0x97, 0xa5, 0x0c,
		0xb9, 0xd3, 0xcb, 0x9f, 0x80, 0xde, 0xbc, 0xb6,
	};
	unsigned char sample[96] = {
		0xe3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xe6, 0x13, 0x6a, 0x71, 0x2d, 0x8c, 0x48, 0x00,
	};
	unsigned char expected_sample[] = {
		0xe3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xe6, 0x13, 0x6a, 0x71, 0x2d, 0x8c, 0x48, 0x00,

		0x00, 0x00, 0x00, 0x64, // this line is ignored

		0x01, 0xc1, 0x67, 0xa4, 0xdf, 0x28, 0xf2, 0x5a,
		0x47, 0x94, 0xbf, 0x00, 0xc8, 0xb3, 0x54, 0x69
	};
	int len_pack = 48;
	auth.keyid = 100;
	auth.type = AUTH_CMAC;
	auth.digest = NULL;
	auth.key = (uint8_t *)key;
	auth.key_size = (unsigned short)strlen(CMACkey);

#if OPENSSL_VERSION_NUMBER > 0x20000000L
	auth.mac_ctx = Setup_MAC_CTX("AES-128-CBC", auth.key, auth.key_size);
	TEST_ASSERT_NOT_NULL(auth.mac_ctx);
#else
	auth.cipher = EVP_get_cipherbyname("AES-128-CBC");
	TEST_ASSERT_NOT_NULL(auth.cipher);
#endif

	int length = cmac_encrypt(&auth,
				  (uint32_t*)sample, len_pack);

	TEST_ASSERT_EQUAL(4+16, length);            /* AES-128 */

	TEST_ASSERT_EQUAL_MEMORY(
	    &(expected_sample[52]),
	    &(sample[52]),
	    digestLength
	);
}

/* Test vectors from RFC 4493
 * Note: All tests use the same message.
 * Shorter tests only use the front part of it.
 * The message gets copied into buffer because cmac_encrypt
 * trashes the buffer by appending the MAC.
 */
TEST(macencrypt, CMAC_TestVectors) {
	const unsigned char M[64] = {
		0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
		0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
		0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
		0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
		0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
		0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
		0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
		0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
	};
	unsigned char key[16] = {
		0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
		0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
	};
	const unsigned char mac0[16] = {
//    AES-CMAC       bb1d6929 e9593728 7fa37d12 9b756746
		0xbb, 0x1d, 0x69, 0x29,  0xe9, 0x59, 0x37, 0x28,
		0x7f, 0xa3, 0x7d, 0x12,  0x9b, 0x75, 0x67, 0x46
	};
	const unsigned char mac16[16] = {
//    AES-CMAC       070a16b4 6b4d4144 f79bdd9d d04a287c
		0x07, 0x0a, 0x16, 0xb4,  0x6b, 0x4d, 0x41, 0x44,
		0xf7, 0x9b, 0xdd, 0x9d,  0xd0, 0x4a, 0x28, 0x7c
	};
	const unsigned char mac40[16] = {
//    AES-CMAC       dfa66747 de9ae630 30ca3261 1497c827
		0xdf, 0xa6, 0x67, 0x47,  0xde, 0x9a, 0xe6, 0x30,
		0x30, 0xca, 0x32, 0x61,  0x14, 0x97, 0xc8, 0x27
	};
	const unsigned char mac64[16] = {
//    AES-CMAC       51f0bebf 7e3b9d92 fc497417 79363cfe
		0x51, 0xf0, 0xbe, 0xbf,  0x7e, 0x3b, 0x9d, 0x92,
		0xfc, 0x49, 0x74, 0x17,  0x79, 0x36, 0x3c, 0xfe
	};
	unsigned char buffer[64+4+16];
	int length;

	auth.keyid = 100;
	auth.type = AUTH_CMAC;
	auth.digest = NULL;
	auth.key = (uint8_t *)key;
	auth.key_size = (unsigned short)sizeof(key);

#if OPENSSL_VERSION_NUMBER > 0x20000000L
	auth.mac_ctx = Setup_MAC_CTX("AES-128-CBC", auth.key, auth.key_size);
	TEST_ASSERT_NOT_NULL(auth.mac_ctx);
#else
	auth.cipher = EVP_get_cipherbyname("AES-128-CBC");
	TEST_ASSERT_NOT_NULL(auth.cipher);
#endif

	memcpy(buffer, M, 0);
	length = cmac_encrypt(&auth, (uint32_t*)buffer, 0);
	TEST_ASSERT_EQUAL(4+16, length);
	TEST_ASSERT_EQUAL_MEMORY(mac0, buffer+0+4, 16);

	memcpy(buffer, M, 16);
	length = cmac_encrypt(&auth, (uint32_t*)buffer, 16);
	TEST_ASSERT_EQUAL(4+16, length);
	TEST_ASSERT_EQUAL_MEMORY(mac16, buffer+16+4, 16);

	memcpy(buffer, M, 40);
	length = cmac_encrypt(&auth, (uint32_t*)buffer, 40);
	TEST_ASSERT_EQUAL(4+16, length);
	TEST_ASSERT_EQUAL_MEMORY(mac40, buffer+40+4, 16);

	memcpy(buffer, M, 64);
	length = cmac_encrypt(&auth, (uint32_t*)buffer, 64);
	TEST_ASSERT_EQUAL(4+16, length);
	TEST_ASSERT_EQUAL_MEMORY(mac64, buffer+64+4, 16);
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
	RUN_TEST_CASE(macencrypt, CMAC_Encrypt);
	RUN_TEST_CASE(macencrypt, DecryptValidCMAC);
	RUN_TEST_CASE(macencrypt, DecryptInvalidCMAC);
	RUN_TEST_CASE(macencrypt, IPv4AddressToRefId);
	RUN_TEST_CASE(macencrypt, IPv6AddressToRefId);
	RUN_TEST_CASE(macencrypt, null_trunc);
	RUN_TEST_CASE(macencrypt, CMAC_TestVectors);
}
