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
	struct NTS_Key k0 = {.K={1, 2, 3, 4, 5}, .I=123};
	struct NTS_Key k1 = {.K={10, 20, 30, 40, 50}, .I=456};
	/* copy to key variables */
	nts_keys[0] = k0;
	nts_keys[1] = k1;
	/* run test */
	nts_nKeys = 2;
	nts_make_cookie_key();  /* push k0 to k1 */
	/* check that K[1] now equals former-K[0] */
	TEST_ASSERT_EQUAL_UINT8_ARRAY(nts_keys[1].K, k0.K, NTS_MAX_KEYLEN);
	TEST_ASSERT_EQUAL(nts_keys[1].I, k0.I);
	/* check that K[0] does not equal former-K[0] */
	/* There is no "TEST UNEQUAL", do it manually */
	bool equal = true;
	for (unsigned int i = 0; i < NTS_MAX_KEYLEN; i++) {
		if (nts_keys[0].K[i] != k0.K[i]) {
			equal = false;
			break;
		}
	}
	TEST_ASSERT_EQUAL(false, equal);
	/* Check that I[0] does not equal former-I[0] */
	TEST_ASSERT_NOT_EQUAL(nts_keys[0].I, k0.I);
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
	nts_nKeys = 0;
	nts_make_cookie_key();
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

TEST(nts_cookie, nts_read_write_cookies) {
	struct NTS_Key k0, k1, k2;
	bool ok;
	ntsconfig.KI = "test-cookie-keys";
	nts_nKeys = 0;
	nts_make_cookie_key();
	nts_make_cookie_key();
	nts_make_cookie_key();
	k0 = nts_keys[0];
	k1 = nts_keys[1];
	k2 = nts_keys[2];
	TEST_ASSERT_EQUAL(nts_nKeys, 3);
	ok = nts_write_cookie_keys();
	TEST_ASSERT_EQUAL(true, ok);
	nts_make_cookie_key();    /* scramble things */
	ZERO(nts_keys);
	nts_nKeys = 377;
	ok = nts_read_cookie_keys();
	TEST_ASSERT_EQUAL(true, ok);
	TEST_ASSERT_EQUAL(nts_nKeys, 3);
	TEST_ASSERT_EQUAL_UINT8_ARRAY(nts_keys[0].K, k0.K, NTS_MAX_KEYLEN);
	TEST_ASSERT_EQUAL(nts_keys[0].I, k0.I);
	TEST_ASSERT_EQUAL_UINT8_ARRAY(nts_keys[1].K, k1.K, NTS_MAX_KEYLEN);
	TEST_ASSERT_EQUAL(nts_keys[1].I, k1.I);
	TEST_ASSERT_EQUAL_UINT8_ARRAY(nts_keys[2].K, k2.K, NTS_MAX_KEYLEN);
	TEST_ASSERT_EQUAL(nts_keys[2].I, k2.I);
}

TEST_GROUP_RUNNER(nts_cookie) {
	RUN_TEST_CASE(nts_cookie, nts_make_unpack_cookie);
	RUN_TEST_CASE(nts_cookie, nts_make_cookie_key);
	RUN_TEST_CASE(nts_cookie, nts_read_write_cookies);
}
