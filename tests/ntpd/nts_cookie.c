#include "config.h"
#include "ntpd.h"
#include "nts.h"
#include "nts2.h"
#include "ntp_dns.h"
#include "unity.h"
#include "unity_fixture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes_siv.h"

extern AES_SIV_CTX* cookie_ctx;
extern uint8_t K[NTS_MAX_KEYLEN], K2[NTS_MAX_KEYLEN];
extern uint32_t I;

TEST_GROUP(nts_cookie);

TEST_SETUP(nts_cookie) {}

TEST_TEAR_DOWN(nts_cookie) {}


TEST(nts_cookie, nts_make_cookie_key) {
	/* init */
	uint8_t kStart1[NTS_MAX_KEYLEN] = {1, 2, 3, 4, 5};
	uint8_t kStart2[NTS_MAX_KEYLEN] = {10, 20, 30, 40, 50};
	uint32_t iStart = I;
	/* copy to key variables */
	memcpy(K, kStart1, sizeof(K));
	memcpy(K2, kStart2, sizeof(K2));
	/* run test */
	nts_make_cookie_key();
	/* check that K2 now equals former-K */
	TEST_ASSERT_EQUAL_UINT8_ARRAY(kStart1, K2, sizeof(K2));
	/* check that K does not equal former-K */
	/* There is no "TEST UNEQUAL", do it manually */
	bool equal = true;
	for (unsigned int i = 0; i < sizeof(K); i++) {
		if (K[i] != kStart1[i]) {
			equal = false;
			break;
		}
	}
	TEST_ASSERT_EQUAL(false, equal);
	/* Check that I does not equal former-I */
	TEST_ASSERT_NOT_EQUAL(iStart, I);
}

TEST(nts_cookie, nts_make_unpack_cookie) {
	/* init */
	uint8_t cookie[NTS_MAX_COOKIELEN];
	/* Using 16 bytes in test for ease of handling */
	uint8_t c2s[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	uint8_t s2c[16] = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
	uint8_t c2s_2[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t s2c_2[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int len;
	int keylen;
	bool ok;
	uint16_t aead; /* retrieved on unpack */
	/* Init for cookie_ctx */
	nts_cookie_init();
	/* Test */
	len = nts_make_cookie(cookie, AEAD_AES_SIV_CMAC_256, c2s, s2c, sizeof(c2s));
	TEST_ASSERT_EQUAL(72, len);
	/* Very limited in what data can be directly checked here */
	/* Reverse the test */
	ok = nts_unpack_cookie(cookie, len, &aead, c2s_2, s2c_2, &keylen);
	TEST_ASSERT_EQUAL(true, ok);
	TEST_ASSERT_EQUAL(AEAD_AES_SIV_CMAC_256, aead);
	TEST_ASSERT_EQUAL(16, keylen);
	TEST_ASSERT_EQUAL_UINT8_ARRAY(c2s, c2s_2, 16);
	TEST_ASSERT_EQUAL_UINT8_ARRAY(s2c, s2c_2, 16);
}

TEST_GROUP_RUNNER(nts_cookie) {
	RUN_TEST_CASE(nts_cookie, nts_make_unpack_cookie);
	RUN_TEST_CASE(nts_cookie, nts_make_cookie_key);
}
