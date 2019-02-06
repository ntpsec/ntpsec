#include "nts_lib.h"
#include "unity.h"
#include "unity_fixture.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TEST_GROUP(nts_lib);

TEST_SETUP(nts_lib) {}

TEST_TEAR_DOWN(nts_lib) {}

TEST(nts_lib, record_decode_null) {
	uint8_t expected[4] = {0x80, 0, 0, 0};
	record_bits *record;

	record = calloc(1, sizeof(record_bits));
	if (NULL == record) {
		TEST_FAIL_MESSAGE("record malloc");
		return;
	}
	record->record = malloc(4);
	if (NULL == record->record) {
		TEST_FAIL_MESSAGE("record value malloc");
		return;
	}
	memcpy(record->record, expected, 4);
	record->record_length = 4;

	nts_record_parse(record);

	TEST_ASSERT_TRUE(record->critical);
	TEST_ASSERT_EQUAL_INT16(0, record->record_type);
	TEST_ASSERT_EQUAL_INT16(0, record->body_length);
	TEST_ASSERT_NULL(record->body);
	record->body = NULL;

	free(record->record);
	free(record);
}

TEST(nts_lib, record_decode_u16) {
	uint8_t expected[6] = {0, port_negotiation, 0, 2, 0, 123};
	record_bits *record;
	int lints[1];

	record = calloc(1, sizeof(record_bits));
	if (NULL == record) {
		TEST_FAIL_MESSAGE("record malloc");
		return;
	}
	record->record = malloc(6);
	if (NULL == record->record) {
		TEST_FAIL_MESSAGE("record value malloc");
		return;
	}
	memcpy(record->record, expected, 6);
	record->record_length = 6;

	nts_record_parse(record);
	if (record->body_length & 1) {
		TEST_FAIL_MESSAGE("odd byte out");
	}
	memcpy(lints, record->body, 2);

	TEST_ASSERT_FALSE(record->critical);
	TEST_ASSERT_EQUAL_INT16(port_negotiation, record->record_type);
	TEST_ASSERT_EQUAL_INT16(2, record->body_length);
	TEST_ASSERT_NOT_NULL(record->body);

	TEST_ASSERT_EQUAL_INT16(123, ntohs(lints[0]));
	record->body = NULL;

	free(record->record);
	free(record);
}

TEST(nts_lib, record_decode_u16s) {
	uint8_t expected[8] = {0, algorithm_negotiation, 0, 4,
			       0, AEAD_AES_SIV_CMAC_512, 0, AEAD_CHACHA20_POLY1305};
	record_bits *record;
	int lints[2];
	void *there;

	record = calloc(1, sizeof(record_bits));
	if (NULL == record) {
		TEST_FAIL_MESSAGE("record malloc");
		return;
	}
	record->record = malloc(8);
	if (NULL == record->record) {
		TEST_FAIL_MESSAGE("record value malloc");
		return;
	}
	memcpy(record->record, expected, 8);
	record->record_length = 8;

	nts_record_parse(record);
	there = record->body;
	if (record->body_length & 1) {
		TEST_FAIL_MESSAGE("odd byte out");
	}
	for (int count = 0; (count * 2) < record->body_length; count++) {
		there = upf(there, &lints[count], 2);
	}

	TEST_ASSERT_FALSE(record->critical);
	TEST_ASSERT_EQUAL_INT16(algorithm_negotiation, record->record_type);
	TEST_ASSERT_EQUAL_INT16(4, record->body_length);
	TEST_ASSERT_NOT_NULL(record->body);

	TEST_ASSERT_EQUAL_INT16(AEAD_AES_SIV_CMAC_512, ntohs(lints[0]));
	TEST_ASSERT_EQUAL_INT16(AEAD_CHACHA20_POLY1305, ntohs(lints[1]));
	record->body = NULL;

	free(record->record);
	free(record);
}

TEST(nts_lib, record_decode_text) {
	record_bits *record;
	const char *expserv = "asus.internal.jamesb192.com";
	uint8_t expected[31] = {0,   server_negotiation,
				0,   27,
				'a', 's', 'u', 's', '.', 'i', 'n', 't', 'e',
				'r', 'n', 'a', 'l', '.', 'j', 'a', 'm', 'e',
				's', 'b', '1', '9', '2', '.', 'c', 'o', 'm'
	};
	record = calloc(1, sizeof(record_bits));
	if (NULL == record) {
		TEST_FAIL_MESSAGE("record malloc");
		return;
	}
	record->record = malloc(31);
	if (NULL == record->record) {
		TEST_FAIL_MESSAGE("record value malloc");
		return;
	}
	memcpy(record->record, expected, 31);
	record->record_length = 31;

	nts_record_parse(record);

	TEST_ASSERT_FALSE(record->critical);
	TEST_ASSERT_EQUAL_INT16(server_negotiation, record->record_type);
	TEST_ASSERT_EQUAL_INT16(27, record->body_length);
	TEST_ASSERT_NOT_NULL(record->body);

	TEST_ASSERT_EQUAL_STRING_LEN(expserv, record->body, 27);
	record->body = NULL;

	free(record->record);
	free(record);
}

TEST(nts_lib, record_encode_null) {
	uint8_t expected[4] = {0x80, end_of_message, 0, 0};
	record_bits *record;

	record = calloc(1, sizeof(record_bits));
	if (NULL == record) {
		TEST_FAIL_MESSAGE("record malloc");
		return;
	}
	record->critical = true;
	record->body_length = 0;
	record->record_type = end_of_message;

	nts_record_form(record);

	TEST_ASSERT_EQUAL_UINT16(4, record->record_length);
	TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, record->record, 4);

	free(record->body);
	free(record);
}

TEST(nts_lib, record_encode_u16) {
	uint8_t expected[6] = {0, port_negotiation, 0, 2, 0, 123};
	uint16_t exp_port = htons(123);
	record_bits *record;

	record = calloc(1, sizeof(record_bits));
	if (NULL == record) {
		TEST_FAIL_MESSAGE("record malloc");
		return;
	}
	record->critical = false;
	record->body_length = 2;
	record->record_type = port_negotiation;
	record->body = malloc(2);
	if (NULL == record->body) {
		TEST_FAIL_MESSAGE("body malloc");
		return;
	}
	memcpy(record->body, &exp_port, 2);

	nts_record_form(record);

	TEST_ASSERT_EQUAL_UINT16(6, record->record_length);
	TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, record->record, 6);

	free(record->body);
	free(record);
}

TEST(nts_lib, record_encode_u16s) {
	uint8_t expected[8] = {0, algorithm_negotiation, 0, 4, 0, 2, 0, 4};
	uint16_t exp_algos[2] = {htons(2), htons(4)};
	record_bits *record;

	record = calloc(1, sizeof(record_bits));
	if (NULL == record) {
		TEST_FAIL_MESSAGE("record malloc");
		return;
	}
	record->critical = false;
	record->body_length = 4;
	record->record_type = algorithm_negotiation;
	record->body = malloc(4);
	if (NULL == record->body) {
		TEST_FAIL_MESSAGE("body malloc");
		return;
	}
	memcpy(record->body, &exp_algos, 4);

	nts_record_form(record);

	TEST_ASSERT_EQUAL_UINT16(8, record->record_length);
	TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, record->record, 8);

	free(record->body);
	free(record);
}

TEST(nts_lib, record_encode_text) {
	const char *expserv = "asus.internal.jamesb192.com";
	uint8_t expected[31] = {  0,  server_negotiation,
				  0,  27,
				  'a', 's', 'u', 's', '.', 'i', 'n', 't', 'e',
				  'r', 'n', 'a', 'l', '.', 'j', 'a', 'm', 'e',
				  's', 'b', '1', '9', '2', '.', 'c', 'o', 'm'
	};
	record_bits *record;

	record = calloc(1, sizeof(record_bits));
	if (NULL == record) {
		TEST_FAIL_MESSAGE("record malloc");
		return;
	}
	record->critical = false;
	record->body_length = 27;
	record->record_type = server_negotiation;
	record->body = malloc(27);
	if (NULL == record->body) {
		TEST_FAIL_MESSAGE("body calloc");
		return;
	}
	memcpy(record->body, expserv, 27);

	nts_record_form(record);

	TEST_ASSERT_EQUAL_UINT16(31, record->record_length);
	TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, record->record, 31);

	free(record->body);
	free(record);
}

/*TEST(nts_lib, existance) {
  TEST_ASSERT_NOT_NULL(upf);
  TEST_ASSERT_NOT_NULL(nts_record_form)
  TEST_ASSERT_NOT_NULL(nts_record_parse)
  TEST_ASSERT_NOT_NULL(nts_cookie_prep);
  TEST_ASSERT_NOT_NULL(nts_cookie_clean);
  TEST_ASSERT_NOT_NULL(nts_cookie_plaintext_parse);
  TEST_ASSERT_NOT_NULL(nts_cookie_plaintext_form);
}*/

TEST_GROUP_RUNNER(nts_lib) {
//  RUN_TEST_CASE(nts_lib, existance);

	RUN_TEST_CASE(nts_lib, record_decode_null);
	RUN_TEST_CASE(nts_lib, record_decode_u16);
	RUN_TEST_CASE(nts_lib, record_decode_u16s);
	RUN_TEST_CASE(nts_lib, record_decode_text);

	RUN_TEST_CASE(nts_lib, record_encode_null);
	RUN_TEST_CASE(nts_lib, record_encode_u16);
	RUN_TEST_CASE(nts_lib, record_encode_u16s);
	RUN_TEST_CASE(nts_lib, record_encode_text);
}
