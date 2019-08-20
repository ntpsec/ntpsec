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

TEST_GROUP(nts_server);

TEST_SETUP(nts_server) {}

TEST_TEAR_DOWN(nts_server) {}

TEST(nts_server, nts_ke_process_receive) {
	/* General init */
	struct BufCtl_t buf;
	int aead;
	bool success;
	/* ===== Test: all correct ===== */
	uint8_t buf0[] = {
		0x80, nts_next_protocol_negotiation, 0, 2, 0x00, nts_protocol_NTP,
		0x80, nts_algorithm_negotiation, 0, 2, 0x00, AEAD_AES_SIV_CMAC_256,
		0x80, nts_end_of_message, 0, 0,
	};
	buf.next = buf0;
	buf.left = sizeof(buf0);
	aead = NO_AEAD;
	/* test */
	success = nts_ke_process_receive(&buf, &aead);
	TEST_ASSERT_EQUAL(true, success);
	TEST_ASSERT_EQUAL_INT(AEAD_AES_SIV_CMAC_256, aead);
	/* ===== Test: nts_error ===== */
	uint8_t buf1[] = {
		0x80, nts_error, 0, 0,
		0x80, nts_end_of_message, 0, 0,
	};
	buf.next = buf1;
	buf.left = sizeof(buf1);
	/* test */
	success = nts_ke_process_receive(&buf, &aead);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: nts_next_protocol_negotiation, bad length ===== */
	uint8_t buf2[] = {
		0x80, nts_next_protocol_negotiation, 0, 4, 0x11, 0x22, 0x33, 0x44,
		0x80, nts_end_of_message, 0, 0,
	};
	buf.next = buf2;
	buf.left = sizeof(buf2);
	/* test */
	success = nts_ke_process_receive(&buf, &aead);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: nts_next_protocol_negotiation, bad protocol ===== */
	uint8_t buf3[] = {
		0x80, nts_next_protocol_negotiation, 0, 2, 0x11, 0x22,
		0x80, nts_end_of_message, 0, 0,
	};
	buf.next = buf3;
	buf.left = sizeof(buf3);
	/* test */
	success = nts_ke_process_receive(&buf, &aead);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: nts_end_of_message, bad length ===== */
	uint8_t buf4[] = {
		0x80, nts_end_of_message, 0, 23,
	};
	buf.next = buf4;
	buf.left = sizeof(buf4);
	/* test */
	success = nts_ke_process_receive(&buf, &aead);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: nts_end_of_message, bad critical ===== */
	uint8_t buf5[] = {
		0x00, nts_end_of_message, 0, 0,
	};
	buf.next = buf5;
	buf.left = sizeof(buf5);
	/* test */
	success = nts_ke_process_receive(&buf, &aead);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: nts_end_of_message, remaining ===== */
	uint8_t buf6[] = {
		0x00, nts_end_of_message, 0, 0,
		1, 2, 3, 4
	};
	buf.next = buf6;
	buf.left = sizeof(buf6);
	/* test */
	success = nts_ke_process_receive(&buf, &aead);
	TEST_ASSERT_EQUAL(false, success);
	/* ===== Test: default, bad critical ===== */
	uint8_t buf7[] = {
		0x80, 0xFF, 0, 0,
	};
	buf.next = buf7;
	buf.left = sizeof(buf7);
	/* test */
	success = nts_ke_process_receive(&buf, &aead);
	TEST_ASSERT_EQUAL(false, success);
}

TEST_GROUP_RUNNER(nts_server) {
	RUN_TEST_CASE(nts_server, nts_ke_process_receive);
}
