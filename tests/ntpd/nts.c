#include "config.h"
#include "ntpd.h"
#include "nts.h"
#include "nts2.h"
#include "unity.h"
#include "unity_fixture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TEST_GROUP(nts);

TEST_SETUP(nts) {}

TEST_TEAR_DOWN(nts) {}

TEST(nts, nts_translate_version) {
	TEST_ASSERT_EQUAL_INT32(0, nts_translate_version(NULL));
	TEST_ASSERT_EQUAL_INT32(TLS1_3_VERSION, nts_translate_version("TLS1.3"));
	TEST_ASSERT_EQUAL_INT32(-1, nts_translate_version("blah"));
}

TEST(nts, nts_string_to_aead) {
	TEST_ASSERT_EQUAL_INT16(AEAD_AES_SIV_CMAC_256,
				nts_string_to_aead("AES_SIV_CMAC_256"));
	TEST_ASSERT_EQUAL_INT16(AEAD_AES_SIV_CMAC_384,
				nts_string_to_aead("AES_SIV_CMAC_384"));
	TEST_ASSERT_EQUAL_INT16(AEAD_AES_SIV_CMAC_512,
				nts_string_to_aead("AES_SIV_CMAC_512"));
	TEST_ASSERT_EQUAL_INT16(NO_AEAD, nts_string_to_aead("blah"));
}

TEST(nts, nts_get_key_length) {
	TEST_ASSERT_EQUAL_INT32(AEAD_AES_SIV_CMAC_256_KEYLEN,
				nts_get_key_length(AEAD_AES_SIV_CMAC_256));
	TEST_ASSERT_EQUAL_INT32(AEAD_AES_SIV_CMAC_384_KEYLEN,
				nts_get_key_length(AEAD_AES_SIV_CMAC_384));
	TEST_ASSERT_EQUAL_INT32(AEAD_AES_SIV_CMAC_512_KEYLEN,
				nts_get_key_length(AEAD_AES_SIV_CMAC_512));
	TEST_ASSERT_EQUAL_INT32(0, nts_get_key_length(-23));
}

TEST(nts, ke_append_record_null) {
	/* Setup */
	uint8_t buf[128];
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 128;
	/* Run test */
	ke_append_record_null(&cursor, 0xFA7E);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0xFA, buf[0]);
	TEST_ASSERT_EQUAL_UINT8(0x7E, buf[1]);
	TEST_ASSERT_EQUAL_UINT8(0, buf[2]);
	TEST_ASSERT_EQUAL_UINT8(0, buf[3]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[4], cursor.next);
	TEST_ASSERT_EQUAL_INT(124, cursor.left);
}

TEST(nts, ke_append_record_uint16) {
	/* Test change */
	/* Setup */
	uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	/* Run test */
	ke_append_record_uint16(&cursor, 0xCAFE, 0x1234);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0xCA, buf[0]);
	TEST_ASSERT_EQUAL_UINT8(0xFE, buf[1]);
	TEST_ASSERT_EQUAL_UINT8(0x00, buf[2]);
	TEST_ASSERT_EQUAL_UINT8(0x02, buf[3]);
	TEST_ASSERT_EQUAL_UINT8(0x12, buf[4]);
	TEST_ASSERT_EQUAL_UINT8(0x34, buf[5]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[6], cursor.next);
	TEST_ASSERT_EQUAL_INT(10, cursor.left);
	/* Test no change */
	/* Setup */
	cursor.left = 0;
	/* Run test */
	ke_append_record_uint16(&cursor, 0xCAFE, 0x1234);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0, buf[6]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[6], cursor.next);
	TEST_ASSERT_EQUAL_INT(0, cursor.left);
}

TEST(nts, ke_append_record_bytes) {
	/* Test change */
	/* Setup */
	uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	uint8_t data[6] = {0, 1, 2, 3, 4, 5};
	/* Run test */
	ke_append_record_bytes(&cursor, 0xCAFE, data, 6);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0xCA, buf[0]);
	TEST_ASSERT_EQUAL_UINT8(0xFE, buf[1]);
	TEST_ASSERT_EQUAL_UINT8(0x00, buf[2]);
	TEST_ASSERT_EQUAL_UINT8(0x06, buf[3]);
	TEST_ASSERT_EQUAL_UINT8(0, buf[4]);
	TEST_ASSERT_EQUAL_UINT8(1, buf[5]);
	TEST_ASSERT_EQUAL_UINT8(2, buf[6]);
	TEST_ASSERT_EQUAL_UINT8(3, buf[7]);
	TEST_ASSERT_EQUAL_UINT8(4, buf[8]);
	TEST_ASSERT_EQUAL_UINT8(5, buf[9]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[10], cursor.next);
	TEST_ASSERT_EQUAL_INT(6, cursor.left);
	/* Test no change */
	/* Setup */
	cursor.left = 0;
	/* Run test */
	ke_append_record_bytes(&cursor, 0xCAFE, data, 6);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0, buf[10]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[10], cursor.next);
	TEST_ASSERT_EQUAL_INT(0, cursor.left);
}

TEST(nts, ex_append_record_null) {
	/* Setup */
	uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	/* Run test */
	ex_append_record_null(&cursor, 0xFADE);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0xFA, buf[0]);
	TEST_ASSERT_EQUAL_UINT8(0xDE, buf[1]);
	TEST_ASSERT_EQUAL_UINT8(0, buf[2]);
	TEST_ASSERT_EQUAL_UINT8(4, buf[3]);
	TEST_ASSERT_EQUAL_UINT8(0, buf[4]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[4], cursor.next);
	TEST_ASSERT_EQUAL_INT(12, cursor.left);
}

TEST(nts, ex_append_record_uint16) {
	/* Setup */
	uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	/* Run test */
	ex_append_record_uint16(&cursor, 0xFADE, 0x1234);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0xFA, buf[0]);
	TEST_ASSERT_EQUAL_UINT8(0xDE, buf[1]);
	TEST_ASSERT_EQUAL_UINT8(0x00, buf[2]);
	TEST_ASSERT_EQUAL_UINT8(0x06, buf[3]);
	TEST_ASSERT_EQUAL_UINT8(0x12, buf[4]);
	TEST_ASSERT_EQUAL_UINT8(0x34, buf[5]);
	TEST_ASSERT_EQUAL_UINT8(0, buf[6]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[6], cursor.next);
	TEST_ASSERT_EQUAL_INT(10, cursor.left);
	/* Test no change */
	/* Setup */
	cursor.left = 0;
	/* Run test */
	ke_append_record_uint16(&cursor, 0xCAFE, 0x1234);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0, buf[6]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[6], cursor.next);
	TEST_ASSERT_EQUAL_INT(0, cursor.left);
}

TEST(nts, ex_append_record_bytes) {
	/* Test change */
	/* Setup */
	uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	uint8_t data[6] = {0, 1, 2, 3, 4, 5};
	/* Run test */
	ex_append_record_bytes(&cursor, 0xCAFE, data, 6);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0xCA, buf[0]);
	TEST_ASSERT_EQUAL_UINT8(0xFE, buf[1]);
	TEST_ASSERT_EQUAL_UINT8(0x00, buf[2]);
	TEST_ASSERT_EQUAL_UINT8(0x0A, buf[3]);
	TEST_ASSERT_EQUAL_UINT8(0, buf[4]);
	TEST_ASSERT_EQUAL_UINT8(1, buf[5]);
	TEST_ASSERT_EQUAL_UINT8(2, buf[6]);
	TEST_ASSERT_EQUAL_UINT8(3, buf[7]);
	TEST_ASSERT_EQUAL_UINT8(4, buf[8]);
	TEST_ASSERT_EQUAL_UINT8(5, buf[9]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[10], cursor.next);
	TEST_ASSERT_EQUAL_INT(6, cursor.left);
	/* Test no change */
	/* Setup */
	cursor.left = 0;
	/* Run test */
	ex_append_record_bytes(&cursor, 0xCAFE, data, 6);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0, buf[10]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[10], cursor.next);
	TEST_ASSERT_EQUAL_INT(0, cursor.left);
}

TEST(nts, ex_append_header) {
	/* Test change */
	/* Setup */
	uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	/* Run test */
	ex_append_header(&cursor, 0xFADE, 0x1234);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0xFA, buf[0]);
	TEST_ASSERT_EQUAL_UINT8(0xDE, buf[1]);
	TEST_ASSERT_EQUAL_UINT8(0x12, buf[2]);
	TEST_ASSERT_EQUAL_UINT8(0x38, buf[3]);
	TEST_ASSERT_EQUAL_UINT8(0, buf[4]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[4], cursor.next);
	TEST_ASSERT_EQUAL_INT(12, cursor.left);
	/* Test no change */
	/* Setup */
	cursor.left = 0;
	/* Run test */
	ex_append_header(&cursor, 0xFEED, 0xABCD);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0, buf[4]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[4], cursor.next);
	TEST_ASSERT_EQUAL_INT(0, cursor.left);
}

TEST(nts, append_header) {
	/* Test change */
	/* Setup */
	uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	/* Run test */
	append_header(&cursor, 0xFADE, 0x1234);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0xFA, buf[0]);
	TEST_ASSERT_EQUAL_UINT8(0xDE, buf[1]);
	TEST_ASSERT_EQUAL_UINT8(0x12, buf[2]);
	TEST_ASSERT_EQUAL_UINT8(0x34, buf[3]);
	TEST_ASSERT_EQUAL_UINT8(0, buf[4]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[4], cursor.next);
	TEST_ASSERT_EQUAL_INT(12, cursor.left);
	/* Test no change */
	/* Setup */
	cursor.left = 0;
	/* Run test */
	append_header(&cursor, 0xFEED, 0xABCD);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0, buf[4]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[4], cursor.next);
	TEST_ASSERT_EQUAL_INT(0, cursor.left);
}

TEST(nts, append_uint16) {
	/* Test change */
	/* Setup */
	uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	/* Run test */
	append_uint16(&cursor, 0x1234);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0x12, buf[0]);
	TEST_ASSERT_EQUAL_UINT8(0x34, buf[1]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[2], cursor.next);
	TEST_ASSERT_EQUAL_INT(14, cursor.left);
	/* Test no change */
	/* Setup */
	cursor.left = 0;
	/* Run test */
	append_uint16(&cursor, 0x5678);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0, buf[2]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[2], cursor.next);
	TEST_ASSERT_EQUAL_INT(0, cursor.left);
}

TEST(nts, append_bytes) {
	/* Test change */
	/* Setup */
	uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	uint8_t data[6] = {0, 1, 2, 3, 4, 5};
	/* Run test */
	append_bytes(&cursor, data, 6);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0, buf[0]);
	TEST_ASSERT_EQUAL_UINT8(1, buf[1]);
	TEST_ASSERT_EQUAL_UINT8(2, buf[2]);
	TEST_ASSERT_EQUAL_UINT8(3, buf[3]);
	TEST_ASSERT_EQUAL_UINT8(4, buf[4]);
	TEST_ASSERT_EQUAL_UINT8(5, buf[5]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[6], cursor.next);
	TEST_ASSERT_EQUAL_INT(10, cursor.left);
	/* Test no change */
	/* Setup */
	cursor.left = 0;
	/* Run test */
	append_bytes(&cursor, data, 6);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0, buf[6]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[6], cursor.next);
	TEST_ASSERT_EQUAL_INT(0, cursor.left);
}

TEST(nts, ke_next_record) {
	/* Setup */
	uint8_t buf[16] = {0xFA, 0xCE, 0, 4, 0xFF, 0xEE, 0xDD, 0xCC,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	int length;
	uint16_t type;
	/* Run test */
	type = ke_next_record(&cursor, &length);
	/* Check */
	TEST_ASSERT_EQUAL_INT(4, length);
	TEST_ASSERT_EQUAL_INT(0xFACE, type);
	TEST_ASSERT_POINTERS_EQUAL(&buf[4], cursor.next);
	TEST_ASSERT_EQUAL_INT(12, cursor.left);
}

TEST(nts, ex_next_record) {
	/* Setup */
	uint8_t buf[16] = {0xFA, 0xCE, 0, 8, 0xFF, 0xEE, 0xDD, 0xCC,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	int length;
	uint16_t type;
	/* Run test */
	type = ex_next_record(&cursor, &length);
	/* Check */
	TEST_ASSERT_EQUAL_INT(4, length);
	TEST_ASSERT_EQUAL_INT(0xFACE, type);
	TEST_ASSERT_POINTERS_EQUAL(&buf[4], cursor.next);
	TEST_ASSERT_EQUAL_INT(12, cursor.left);
}

TEST(nts, next_uint16) {
	/* Setup */
	uint8_t buf[16] = {0xFA, 0xCE, 0, 4, 0xFF, 0xEE, 0xDD, 0xCC,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	uint16_t data;
	/* Run test */
	data = next_uint16(&cursor);
	/* Check */
	TEST_ASSERT_EQUAL_UINT8(0xFACE, data);
	TEST_ASSERT_POINTERS_EQUAL(&buf[2], cursor.next);
	TEST_ASSERT_EQUAL_INT(14, cursor.left);
}

TEST(nts, next_bytes) {
	/* Setup */
	uint8_t buf[16] = {0xFA, 0xCE, 0, 4, 0xFF, 0xEE, 0xDD, 0xCC,
		0, 0, 0, 0, 0, 0, 0, 0};
	BufCtl cursor;
	cursor.next = buf;
	cursor.left = 16;
	uint8_t data[8];
	uint16_t length;
	/* Run test */
	length = next_bytes(&cursor, data, 8);
	/* Check */
	TEST_ASSERT_EQUAL_INT(8, length);
	TEST_ASSERT_EQUAL_UINT8(0xFA, data[0]);
	TEST_ASSERT_EQUAL_UINT8(0xCE, data[1]);
	TEST_ASSERT_EQUAL_UINT8(0, data[2]);
	TEST_ASSERT_EQUAL_UINT8(4, data[3]);
	TEST_ASSERT_EQUAL_UINT8(0xFF, data[4]);
	TEST_ASSERT_EQUAL_UINT8(0xEE, data[5]);
	TEST_ASSERT_EQUAL_UINT8(0xDD, data[6]);
	TEST_ASSERT_EQUAL_UINT8(0xCC, data[7]);
	TEST_ASSERT_POINTERS_EQUAL(&buf[8], cursor.next);
	TEST_ASSERT_EQUAL_INT(8, cursor.left);
}

TEST_GROUP_RUNNER(nts) {
	RUN_TEST_CASE(nts, nts_translate_version);
	RUN_TEST_CASE(nts, nts_string_to_aead);
	RUN_TEST_CASE(nts, nts_get_key_length);
	RUN_TEST_CASE(nts, ke_append_record_null);
	RUN_TEST_CASE(nts, ke_append_record_uint16);
	RUN_TEST_CASE(nts, ke_append_record_bytes);
	RUN_TEST_CASE(nts, ex_append_record_null);
	RUN_TEST_CASE(nts, ex_append_record_uint16);
	RUN_TEST_CASE(nts, ex_append_record_bytes);
	RUN_TEST_CASE(nts, ex_append_header);
	RUN_TEST_CASE(nts, append_header);
	RUN_TEST_CASE(nts, append_uint16);
	RUN_TEST_CASE(nts, append_bytes);
	RUN_TEST_CASE(nts, ke_next_record);
	RUN_TEST_CASE(nts, ex_next_record);
	RUN_TEST_CASE(nts, next_uint16);
	RUN_TEST_CASE(nts, next_bytes);
}
