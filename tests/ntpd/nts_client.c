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

void dns_take_server(struct peer *a, sockaddr_u *b);
void dns_take_status(struct peer *a, DNS_Status b);

bool nts_client_send_request_core(uint8_t *buff, int buf_size, int *used, struct peer* peer);
bool nts_client_process_response_core(uint8_t *buff, int transferred, struct peer* peer);


TEST_GROUP(nts_client);

TEST_SETUP(nts_client) {}

TEST_TEAR_DOWN(nts_client) {}


TEST(nts_client, nts_client_send_request_core) {
	bool success;
	int used;
	struct peer peer;
	uint8_t buffer[1000];
	char pAEAD[50] = "AES_SIV_CMAC_512";
	/* ===== Test: correct, peer aead ===== */
	peer.cfg.nts_cfg.aead = pAEAD;
	/* run */
	success = nts_client_send_request_core(buffer, sizeof(buffer), &used, &peer);
	TEST_ASSERT_EQUAL(true, success);
	TEST_ASSERT_EQUAL(16, used);
	TEST_ASSERT_EQUAL(0x80, buffer[0]);
	TEST_ASSERT_EQUAL(nts_next_protocol_negotiation, buffer[1]);
	TEST_ASSERT_EQUAL(0, buffer[2]);
	TEST_ASSERT_EQUAL(2, buffer[3]);
	TEST_ASSERT_EQUAL(0, buffer[4]);
	TEST_ASSERT_EQUAL(nts_protocol_NTP, buffer[5]);
	TEST_ASSERT_EQUAL(0, buffer[6]);
	TEST_ASSERT_EQUAL(nts_algorithm_negotiation, buffer[7]);
	TEST_ASSERT_EQUAL(0, buffer[8]);
	TEST_ASSERT_EQUAL(2, buffer[9]);
	TEST_ASSERT_EQUAL(0, buffer[10]);
	TEST_ASSERT_EQUAL(AEAD_AES_SIV_CMAC_512, buffer[11]);
	TEST_ASSERT_EQUAL(0x80, buffer[12]);
	TEST_ASSERT_EQUAL(nts_end_of_message, buffer[13]);
	TEST_ASSERT_EQUAL(0, buffer[14]);
	TEST_ASSERT_EQUAL(0, buffer[15]);
	/* ===== Test: correct, global config aead ===== */
	peer.cfg.nts_cfg.aead = NULL;
	char gAEAD[50] = "AES_SIV_CMAC_384";
	ntsconfig.aead = gAEAD;
	/* run */
	success = nts_client_send_request_core(buffer, sizeof(buffer), &used, &peer);
	TEST_ASSERT_EQUAL(true, success);
	TEST_ASSERT_EQUAL(16, used);
	TEST_ASSERT_EQUAL(0x80, buffer[0]);
	TEST_ASSERT_EQUAL(nts_next_protocol_negotiation, buffer[1]);
	TEST_ASSERT_EQUAL(0, buffer[2]);
	TEST_ASSERT_EQUAL(2, buffer[3]);
	TEST_ASSERT_EQUAL(0, buffer[4]);
	TEST_ASSERT_EQUAL(nts_protocol_NTP, buffer[5]);
	TEST_ASSERT_EQUAL(0, buffer[6]);
	TEST_ASSERT_EQUAL(nts_algorithm_negotiation, buffer[7]);
	TEST_ASSERT_EQUAL(0, buffer[8]);
	TEST_ASSERT_EQUAL(2, buffer[9]);
	TEST_ASSERT_EQUAL(0, buffer[10]);
	TEST_ASSERT_EQUAL(AEAD_AES_SIV_CMAC_384, buffer[11]);
	TEST_ASSERT_EQUAL(0x80, buffer[12]);
	TEST_ASSERT_EQUAL(nts_end_of_message, buffer[13]);
	TEST_ASSERT_EQUAL(0, buffer[14]);
	TEST_ASSERT_EQUAL(0, buffer[15]);
	/* ===== Test: correct, default aead ===== */
	peer.cfg.nts_cfg.aead = NULL;
	ntsconfig.aead = NULL;
	/* run */
	success = nts_client_send_request_core(buffer, sizeof(buffer), &used, &peer);
	TEST_ASSERT_EQUAL(true, success);
	TEST_ASSERT_EQUAL(16, used);
	TEST_ASSERT_EQUAL(0x80, buffer[0]);
	TEST_ASSERT_EQUAL(nts_next_protocol_negotiation, buffer[1]);
	TEST_ASSERT_EQUAL(0, buffer[2]);
	TEST_ASSERT_EQUAL(2, buffer[3]);
	TEST_ASSERT_EQUAL(0, buffer[4]);
	TEST_ASSERT_EQUAL(nts_protocol_NTP, buffer[5]);
	TEST_ASSERT_EQUAL(0, buffer[6]);
	TEST_ASSERT_EQUAL(nts_algorithm_negotiation, buffer[7]);
	TEST_ASSERT_EQUAL(0, buffer[8]);
	TEST_ASSERT_EQUAL(2, buffer[9]);
	TEST_ASSERT_EQUAL(0, buffer[10]);
	TEST_ASSERT_EQUAL(AEAD_AES_SIV_CMAC_256, buffer[11]);
	TEST_ASSERT_EQUAL(0x80, buffer[12]);
	TEST_ASSERT_EQUAL(nts_end_of_message, buffer[13]);
	TEST_ASSERT_EQUAL(0, buffer[14]);
	TEST_ASSERT_EQUAL(0, buffer[15]);
}

TEST(nts_client, nts_client_process_response_core) {
	/* General init */
	bool success;
	struct peer peer;
	peer.nts_state.aead = 42; /* Dummy init values */
	peer.nts_state.cookielen = 0;
	peer.nts_state.writeIdx = 0;
	peer.nts_state.count = 0;
	/* Coverity barfed on uninitialized peer.srcadr, 2022-Mar-16
	 * ** CID 349664:  Uninitialized variables  (UNINIT)
	 * So initialize it with something. */
	peer.srcadr.sa4.sin_family = AF_INET;
	peer.srcadr.sa4.sin_port = htons(9999);
	peer.srcadr.sa4.sin_addr.s_addr= htonl(0x04030201);
	/* ===== Test: all correct ===== */
	/* data */
	uint8_t buf0[] = {
		0x80, nts_next_protocol_negotiation, 0, 2,
			0, nts_protocol_NTP,
		0x80, nts_algorithm_negotiation, 0, 2,
			0, AEAD_AES_SIV_CMAC_256,
		0x80, nts_new_cookie, 0, 8,
			1, 2, 3, 4, 5, 6, 7, 8,
		/* server_negotiation skipped due to getaddrinfo() containment breach */
		0x80, nts_port_negotiation, 0, 2,
			0, 123,
		0x80, nts_end_of_message, 0, 0
	};
	/* run */
	success = nts_client_process_response_core(buf0, sizeof(buf0), &peer);
	/* check */
	TEST_ASSERT_EQUAL(true, success);
	TEST_ASSERT_EQUAL_INT16(AEAD_AES_SIV_CMAC_256, peer.nts_state.aead);
	TEST_ASSERT_EQUAL_INT32(8, peer.nts_state.cookielen);
	TEST_ASSERT_EQUAL_INT8(1, peer.nts_state.cookies[0][0]);
	TEST_ASSERT_EQUAL_INT8(2, peer.nts_state.cookies[0][1]);
	TEST_ASSERT_EQUAL_INT8(3, peer.nts_state.cookies[0][2]);
	TEST_ASSERT_EQUAL_INT8(4, peer.nts_state.cookies[0][3]);
	TEST_ASSERT_EQUAL_INT8(5, peer.nts_state.cookies[0][4]);
	TEST_ASSERT_EQUAL_INT8(6, peer.nts_state.cookies[0][5]);
	TEST_ASSERT_EQUAL_INT8(7, peer.nts_state.cookies[0][6]);
	TEST_ASSERT_EQUAL_INT8(8, peer.nts_state.cookies[0][7]);
	TEST_ASSERT_EQUAL_INT32(1, peer.nts_state.writeIdx);
	TEST_ASSERT_EQUAL_INT32(1, peer.nts_state.count);
	/* ===== Test: nts_error ===== */
	/* data */
	uint8_t buf1[] = {
		0x80, nts_error, 0, 2, 0x11, 0x22,
		0x80, nts_end_of_message, 0, 0
	};
	/* run */
	success = nts_client_process_response_core(buf1, sizeof(buf1), &peer);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: nts_next_protocol, wrong data length ===== */
	/* data */
	uint8_t buf2[] = {
		0x80, nts_error, 0, 4, 0x11, 0x22, 0x33, 0x44,
		0x80, nts_end_of_message, 0, 0
	};
	/* run */
	success = nts_client_process_response_core(buf2, sizeof(buf2), &peer);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: nts_next_protocol, wrong data ===== */
	/* data */
	uint8_t buf3[] = {
		0x80, nts_error, 0, 2, 0x11, 0x22,
		0x80, nts_end_of_message, 0, 0
	};
	/* run */
	success = nts_client_process_response_core(buf3, sizeof(buf3), &peer);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: nts_algorithm_negotiation, wrong length ===== */
	/* data */
	uint8_t buf4[] = {
		0x80, nts_algorithm_negotiation, 0, 4, 0x11, 0x22, 0x33, 0x44,
		0x80, nts_end_of_message, 0, 0
	};
	/* run */
	success = nts_client_process_response_core(buf4, sizeof(buf4), &peer);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test:nts_algorithm_negotiation, bad AEAN type ===== */
	/* data */
	uint8_t buf5[] = {
		0x80, nts_algorithm_negotiation, 0, 4, 0x11, 0x22,
		0x80, nts_end_of_message, 0, 0
	};
	/* run */
	success = nts_client_process_response_core(buf5, sizeof(buf5), &peer);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: nts_new_cookie, over max cookie length ===== */
	/* data */
	uint8_t buf6[] = {
		0x80, nts_new_cookie, 0, 4, 0x11, 0x22, /* NTS_MAX_COOKIELEN == 192 */
		0x80, nts_end_of_message, 0, 0
	};
	/* run */
	success = nts_client_process_response_core(buf6, sizeof(buf6), &peer);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: nts_new_cookie, cookie doesn't equal peer cookie size ===== */
	/* data */
	peer.nts_state.cookielen = 8;
	uint8_t buf7[] = {
		0x80, nts_new_cookie, 0, 4, 0, 9,
		0x80, nts_end_of_message, 0, 0
	};
	/* run */
	success = nts_client_process_response_core(buf7, sizeof(buf7), &peer);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: nts_new_cookie, have max cookies ===== */
	/* data */
	uint8_t buf8[] = {
		0x80, nts_new_cookie, 0, 4, 0, 8, 10, 20, 30, 40, 50, 60, 70, 80,
		0x80, nts_end_of_message, 0, 0
	};
	peer.nts_state.writeIdx = 0;
	peer.nts_state.count = NTS_MAX_COOKIES;
	/* run */
	success = nts_client_process_response_core(buf8, sizeof(buf8), &peer);
	/* check */
	TEST_ASSERT_EQUAL(false, success);
	TEST_ASSERT_EQUAL(0, peer.nts_state.writeIdx);
	TEST_ASSERT_NOT_EQUAL(10, peer.nts_state.cookies[0][0]);
	/* ===== Test: nts_end_of_message, wrong length ===== */
	/* data */
	uint8_t buf9[] = {
		0x80, nts_end_of_message, 0, 4
	};
	/* run */
	success = nts_client_process_response_core(buf9, sizeof(buf9), &peer);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: nts_end_of_message, data remaining ===== */
	/* data */
	uint8_t buf10[] = {
		0x80, nts_end_of_message, 0, 0,
		42
	};
	/* run */
	success = nts_client_process_response_core(buf10, sizeof(buf10), &peer);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: weird type, critical ===== */
	/* data */
	uint8_t buf11[] = {
		0x80, 0xFF, 0, 0,
		0x80, nts_end_of_message, 0, 0,
	};
	/* run */
	success = nts_client_process_response_core(buf11, sizeof(buf11), &peer);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: no cookies ===== */
	/* data */
	peer.nts_state.count = 0;
	uint8_t buf12[] = {
		0x80, nts_end_of_message, 0, 0
	};
	/* run */
	success = nts_client_process_response_core(buf12, sizeof(buf12), &peer);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: no aead ===== */
	/* data */
	peer.nts_state.count = 8; /* So this doesn't trigger an error */
	peer.nts_state.aead = NO_AEAD;
	uint8_t buf13[] = {
		0x80, nts_end_of_message, 0, 0
	};
	/* run */
	success = nts_client_process_response_core(buf13, sizeof(buf13), &peer);
	TEST_ASSERT_EQUAL(false, success);
}

/* Hacks to keep linker happy */

#ifdef HAVE_SECCOMP_H
void setup_SIGSYS_trap(void) {
        return;         /* dummy to keep linker happy */
}
#endif

void dns_take_server(struct peer *a, sockaddr_u *b) {
	UNUSED_ARG(a);
	UNUSED_ARG(b);
	return;
}

void dns_take_status(struct peer *a, DNS_Status b) {
	UNUSED_ARG(a);
	UNUSED_ARG(b);
	return;
}

TEST_GROUP_RUNNER(nts_client) {
	RUN_TEST_CASE(nts_client, nts_client_send_request_core);
	RUN_TEST_CASE(nts_client, nts_client_process_response_core);
}
