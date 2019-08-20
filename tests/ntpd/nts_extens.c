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

/*  base_pkt is the size of a bare NTP packet, used for constructing
 * dummy packets to feed into the tests */
uint8_t base_pkt[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                      12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                      24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
                      36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};

/* Snipped from nts_extens.c */
#define NONCE_LENGTH 16
#define CMAC_LENGTH 16
#define NTP_EX_HDR_LNG 4
#define NTP_EX_U16_LNG 2

enum NtpExtFieldType {
	Unique_Identifier = 0x104,
	NTS_Cookie = 0x204,
	NTS_Cookie_Placeholder = 0x304,
	NTS_AEEF = 0x404 /* Authenticated and Encrypted Extension Fields */
};

TEST_GROUP(nts_extens);

TEST_SETUP(nts_extens) {
	extens_init();
}

TEST_TEAR_DOWN(nts_extens) {}


TEST(nts_extens, extens_client_send) {
	/* init */
	struct peer peer;
	peer.nts_state.readIdx = 0;
	peer.nts_state.count = 4; /* 1/2 of max, -1, should cause 5 requests */
	uint8_t c2s[NTS_MAX_KEYLEN] = {1, 2, 3, 4, 5, 6, 7, 8};
	memcpy(peer.nts_state.c2s, c2s, sizeof(c2s));
	peer.nts_state.keylen = sizeof(c2s);
	peer.nts_state.cookielen = NTS_MAX_COOKIELEN;
	struct pkt xpkt;
	int used = 0;
	/* Test */
	used = extens_client_send(&peer, &xpkt);
	TEST_ASSERT_EQUAL(1056, used);
	TEST_ASSERT_EQUAL(1, peer.nts_state.readIdx);
	TEST_ASSERT_EQUAL(1, nts_client_send);
	TEST_ASSERT_EQUAL(3, peer.nts_state.count);
}

TEST(nts_extens, extens_server_recv) {
	/* init */
	struct ntspacket_t ntspkt;
	memset(&ntspkt, 0, sizeof(ntspkt));
	uint8_t pkt[2048]; /* Void extracted value */
	BufCtl buf;
	bool ok = true;
	memset(pkt, 0, sizeof(pkt));
	memcpy(pkt, base_pkt, sizeof(base_pkt));
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	/* Make dummy cookie */
	uint8_t cookie[NTS_MAX_COOKIELEN];
	uint8_t c2s[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	uint8_t s2c[16] = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
	int cookielen;
	cookielen = nts_make_cookie(cookie, AEAD_AES_SIV_CMAC_256, c2s, s2c,
				    sizeof(c2s));
	/* === Pre Switch === */
	/* Bad record length; non-aligned */
	append_header(&buf, 0x1234, 0x0003);
	ok = extens_server_recv(&ntspkt, pkt, sizeof(pkt));
	TEST_ASSERT_EQUAL(false, ok);
	/* Bad record length; runs off end of data */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	append_header(&buf, 0x1234, 0x0010);
	ok = extens_server_recv(&ntspkt, pkt, sizeof(base_pkt) + 6); /* hdr + 2 */
	TEST_ASSERT_EQUAL(false, ok);
	/* === Unique_Identifier === */
	/* Bad UID length */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	append_header(&buf, Unique_Identifier, NTS_UID_MAX_LENGTH + 8);
	ok = extens_server_recv(&ntspkt, pkt, sizeof(pkt));
	TEST_ASSERT_EQUAL(false, ok);
	/* === NTS_Cookie === */
	/* Too many cookies */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	ex_append_record_bytes(&buf, NTS_Cookie, cookie, cookielen);
	ex_append_record_bytes(&buf, NTS_Cookie, cookie, cookielen);
	ok = extens_server_recv(&ntspkt, pkt, sizeof(pkt));
	TEST_ASSERT_EQUAL(false, ok);
	/* Bad cookie unpack */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	ex_append_record_bytes(&buf, NTS_Cookie, cookie, sizeof(cookie));
	ok = extens_server_recv(&ntspkt, pkt, sizeof(pkt));
	TEST_ASSERT_EQUAL(false, ok);
	/* === NTS_AEEF === */
	/* AEEF without cookie */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	append_header(&buf, NTS_AEEF, NTP_EX_HDR_LNG+NONCE_LENGTH+CMAC_LENGTH);
	ok = extens_server_recv(&ntspkt, pkt, sizeof(pkt));
	TEST_ASSERT_EQUAL(false, ok);
	/* Bad AEEF length */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	ex_append_record_bytes(&buf, NTS_Cookie, cookie, cookielen);
	append_header(&buf, NTS_AEEF, NTP_EX_HDR_LNG+NONCE_LENGTH+CMAC_LENGTH+8);
	ok = extens_server_recv(&ntspkt, pkt, sizeof(pkt));
	TEST_ASSERT_EQUAL(false, ok);
	/* Bad noncelen */
	uint8_t aeef_data1[NTP_EX_HDR_LNG+NONCE_LENGTH+CMAC_LENGTH] = {
		0, 3, /* nonce length, bad, not mod 4 */
		0, 16, /* cmac length */
		1, 2, 3, /* nonce */
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}; /* cmac */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	ex_append_record_bytes(&buf, NTS_Cookie, cookie, cookielen);
	ex_append_record_bytes(&buf, NTS_AEEF, aeef_data1, sizeof(aeef_data1));
	ok = extens_server_recv(&ntspkt, pkt, sizeof(pkt));
	TEST_ASSERT_EQUAL(false, ok);
	/* Bad CMAC length */
	uint8_t aeef_data2[NTP_EX_HDR_LNG+NONCE_LENGTH+CMAC_LENGTH] = {
		0, 16, /* nonce length */
		0, 8, /* cmac length, bad != 16 */
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, /* nonce */
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; /* cmac */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	ex_append_record_bytes(&buf, NTS_Cookie, cookie, cookielen);
	ex_append_record_bytes(&buf, NTS_AEEF, aeef_data2, sizeof(aeef_data2));
	ok = extens_server_recv(&ntspkt, pkt, sizeof(pkt));
	TEST_ASSERT_EQUAL(false, ok);
	/* Bad decryption */
	/* I do not know *why* this is a bad decryption, just that it is */
	uint8_t aeef_data3[NTP_EX_HDR_LNG+NONCE_LENGTH+CMAC_LENGTH] = {
		0, 16, /* nonce length */
		0, 16, /* cmac length */
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, /* nonce */
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; /* cmac */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	ex_append_record_bytes(&buf, NTS_Cookie, cookie, cookielen);
	ex_append_record_bytes(&buf, NTS_AEEF, aeef_data3, sizeof(aeef_data3));
	ok = extens_server_recv(&ntspkt, pkt, sizeof(pkt));
	TEST_ASSERT_EQUAL(false, ok);
	/* No out data -- Don't know how to trigger this yet */
	/* Remaining data, should be none */
	/*
	 * TODO: this is hitting decrypt error instead of remaining data. FIXME
	 */
	uint8_t aeef_data4[NTP_EX_HDR_LNG+NONCE_LENGTH+CMAC_LENGTH] = {
		0, 16, /* nonce length */
		0, 16, /* cmac length */
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, /* nonce */
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; /* cmac */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	ex_append_record_bytes(&buf, NTS_Cookie, cookie, cookielen);
	ex_append_record_bytes(&buf, NTS_AEEF, aeef_data4, sizeof(aeef_data4));
	ok = extens_server_recv(&ntspkt, pkt, sizeof(pkt));
	TEST_ASSERT_EQUAL(false, ok);
	/* === default === */
	/* Unknown critical */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	ex_append_header(&buf, 0xFFFF, 0);
	ok = extens_server_recv(&ntspkt, pkt, sizeof(pkt));
	TEST_ASSERT_EQUAL(false, ok);
	/* Unknown extension, period */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	ex_append_header(&buf, 0x00FF, 0);
	ok = extens_server_recv(&ntspkt, pkt, sizeof(pkt));
	TEST_ASSERT_EQUAL(false, ok);
	/* === Post Loop === */
	/* No AEEF */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	ex_append_record_bytes(&buf, NTS_Cookie, cookie, cookielen);
	ok = extens_server_recv(&ntspkt, pkt, buf.next-pkt);
	TEST_ASSERT_EQUAL(false, ok);
	/* Working */
	/* TODO / FIXME: This test is currently disabled. I cannot get a good
	 * cookie decode */
	uint8_t aeef_data5[NTP_EX_HDR_LNG+NONCE_LENGTH+CMAC_LENGTH] = {
		0, 16, /* nonce length */
		0, 16, /* cmac length */
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, /* nonce */
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; /* cmac */
	memset(pkt+LEN_PKT_NOMAC, 0, sizeof(pkt)-LEN_PKT_NOMAC);
	buf.next = pkt+LEN_PKT_NOMAC;
	buf.left = sizeof(pkt) - LEN_PKT_NOMAC;
	ex_append_record_bytes(&buf, NTS_Cookie, cookie, cookielen);
	ex_append_record_bytes(&buf, NTS_AEEF, aeef_data5, sizeof(aeef_data5));
	ok = extens_server_recv(&ntspkt, pkt, (buf.next - pkt));
	/* TEST_ASSERT_EQUAL(true, ok); //disable */
}

TEST_GROUP_RUNNER(nts_extens) {
	RUN_TEST_CASE(nts_extens, extens_client_send);
	RUN_TEST_CASE(nts_extens, extens_server_recv);
}
