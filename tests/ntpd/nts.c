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

TEST(nts, ke_append_record_null) {
  // Setup
  uint8_t buf[128];
  BufCtl cursor;
  cursor.next = buf;
  cursor.left = 128;
  // Run test
  ke_append_record_null(&cursor, 0xFA7E);
  // Check
  TEST_ASSERT_EQUAL_UINT8(buf[0], 0xFA);
  TEST_ASSERT_EQUAL_UINT8(buf[1], 0x7E);
  TEST_ASSERT_EQUAL_UINT8(buf[2], 0);
  TEST_ASSERT_EQUAL_UINT8(buf[3], 0);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[4]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 124);
}

TEST(nts, ke_append_record_uint16) {
  // Test change
  // Setup
  uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0};
  BufCtl cursor;
  cursor.next = buf;
  cursor.left = 16;
  // Run test
  ke_append_record_uint16(&cursor, 0xCAFE, 0x1234);
  // Check
  TEST_ASSERT_EQUAL_UINT8(buf[0], 0xCA);
  TEST_ASSERT_EQUAL_UINT8(buf[1], 0xFE);
  TEST_ASSERT_EQUAL_UINT8(buf[2], 0x12);
  TEST_ASSERT_EQUAL_UINT8(buf[3], 0x34);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[4]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 12);
  // Test no change
  // Setup
  cursor.left = 0;
  // Run test
  ke_append_record_uint16(&cursor, 0xCAFE, 0x1234);
  // Check
  TEST_ASSERT_EQUAL_UINT8(buf[4], 0);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[4]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 0);
}

TEST(nts, ke_append_record_bytes) {
  // Test change
  // Setup
  uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0};
  BufCtl cursor;
  cursor.next = buf;
  cursor.left = 16;
  uint8_t data[6] = {0, 1, 2, 3, 4, 5};
  // Run test
  ke_append_record_bytes(&cursor, 0xCAFE, data, 6);
  // Check
  TEST_ASSERT_EQUAL_UINT8(buf[0], 0xCA);
  TEST_ASSERT_EQUAL_UINT8(buf[1], 0xFE);
  TEST_ASSERT_EQUAL_UINT8(buf[2], 0);
  TEST_ASSERT_EQUAL_UINT8(buf[3], 1);
  TEST_ASSERT_EQUAL_UINT8(buf[4], 2);
  TEST_ASSERT_EQUAL_UINT8(buf[5], 3);
  TEST_ASSERT_EQUAL_UINT8(buf[6], 4);
  TEST_ASSERT_EQUAL_UINT8(buf[7], 5);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[8]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 8);
  // Test no change
  // Setup
  cursor.left = 0;
  // Run test
  ke_append_record_bytes(&cursor, 0xCAFE, data, 6);
  // Check
  TEST_ASSERT_EQUAL_UINT8(buf[8], 0);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[8]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 0);
}

TEST(nts, append_header) {
  // Test change
  // Setup
  uint8_t buf[16];
  BufCtl cursor;
  cursor.next = buf;
  cursor.left = 16;
  // Run test
  append_header(&cursor, 0xFADE, 0x1234);
  // Check
  TEST_ASSERT_EQUAL_UINT8(buf[0], 0xFA);
  TEST_ASSERT_EQUAL_UINT8(buf[1], 0xDE);
  TEST_ASSERT_EQUAL_UINT8(buf[2], 0x12);
  TEST_ASSERT_EQUAL_UINT8(buf[3], 0x34);
  TEST_ASSERT_EQUAL_UINT8(buf[4], 0);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[4]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 12);
  // Test no change
  // Setup
  cursor.left = 0;
  // Run test
  append_header(&cursor, 0xFEED, 0xABCD);
  // Check
  TEST_ASSERT_EQUAL_UINT8(buf[4], 0);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[4]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 0);
}

TEST(nts, append_uint16) {
  // Test change
  // Setup
  uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0};
  BufCtl cursor;
  cursor.next = buf;
  cursor.left = 16;
  // Run test
  append_uint16(&cursor, 0x1234);
  // Check
  TEST_ASSERT_EQUAL_UINT8(buf[0], 0x12);
  TEST_ASSERT_EQUAL_UINT8(buf[1], 0x34);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[2]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 14);
  // Test no change
  // Setup
  cursor.left = 0;
  // Run test
  append_uint16(&cursor, 0x5678);
  // Check
  TEST_ASSERT_EQUAL_UINT8(buf[2], 0);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[2]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 0);
}

TEST(nts, append_bytes) {
  // Test change
  // Setup
  uint8_t buf[16] = {0, 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0, 0};
  BufCtl cursor;
  cursor.next = buf;
  cursor.left = 16;
  uint8_t data[6] = {0, 1, 2, 3, 4, 5};
  // Run test
  append_bytes(&cursor, data, 6);
  // Check
  TEST_ASSERT_EQUAL_UINT8(buf[0], 0);
  TEST_ASSERT_EQUAL_UINT8(buf[1], 1);
  TEST_ASSERT_EQUAL_UINT8(buf[2], 2);
  TEST_ASSERT_EQUAL_UINT8(buf[3], 3);
  TEST_ASSERT_EQUAL_UINT8(buf[4], 4);
  TEST_ASSERT_EQUAL_UINT8(buf[5], 5);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[6]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 10);
  // Test no change
  // Setup
  cursor.left = 0;
  // Run test
  append_bytes(&cursor, data, 6);
  // Check
  TEST_ASSERT_EQUAL_UINT8(buf[6], 0);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[6]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 0);
}

TEST(nts, ke_next_record) {
  // Setup
  uint8_t buf[16] = {0xFA, 0xCE, 0, 4, 0xFF, 0xEE, 0xDD, 0xCC,
					 0, 0, 0, 0, 0, 0, 0, 0};
  BufCtl cursor;
  cursor.next = buf;
  cursor.left = 16;
  int length;
  uint16_t type;
  // Run test
  type = ke_next_record(&cursor, &length);
  // Check
  TEST_ASSERT_EQUAL_INT(length, 4);
  TEST_ASSERT_EQUAL_INT(type, 0xFACE);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[4]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 12);
}

TEST(nts, next_uint16) {
  // Setup
  uint8_t buf[16] = {0xFA, 0xCE, 0, 4, 0xFF, 0xEE, 0xDD, 0xCC,
					 0, 0, 0, 0, 0, 0, 0, 0};
  BufCtl cursor;
  cursor.next = buf;
  cursor.left = 16;
  uint16_t data;
  // Run test
  data = next_uint16(&cursor);
  // Check
  TEST_ASSERT_EQUAL_UINT8(data, 0xFACE);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[2]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 14);
}

TEST(nts, next_bytes) {
  // Setup
  uint8_t buf[16] = {0xFA, 0xCE, 0, 4, 0xFF, 0xEE, 0xDD, 0xCC,
					 0, 0, 0, 0, 0, 0, 0, 0};
  BufCtl cursor;
  cursor.next = buf;
  cursor.left = 16;
  uint8_t data[8];
  uint16_t length;
  // Run test
  length = next_bytes(&cursor, data, 8);
  // Check
  TEST_ASSERT_EQUAL_INT(length, 8);
  TEST_ASSERT_EQUAL_UINT8(data[0], 0xFA);
  TEST_ASSERT_EQUAL_UINT8(data[1], 0xCE);
  TEST_ASSERT_EQUAL_UINT8(data[2], 0);
  TEST_ASSERT_EQUAL_UINT8(data[3], 4);
  TEST_ASSERT_EQUAL_UINT8(data[4], 0xFF);
  TEST_ASSERT_EQUAL_UINT8(data[5], 0xEE);
  TEST_ASSERT_EQUAL_UINT8(data[6], 0xDD);
  TEST_ASSERT_EQUAL_UINT8(data[7], 0xCC);
  TEST_ASSERT_POINTERS_EQUAL(cursor.next, &buf[8]);
  TEST_ASSERT_EQUAL_INT(cursor.left, 8);
}

/* Hacks to keep linker happy after moving nts_init to nts.c */
bool nts_server_init (void) { return true; }
bool nts_client_init (void) { return true; }
bool nts_cookie_init (void) { return true; }
bool nts_server_init2 (void) { return true; }
bool nts_cookie_init2 (void) { return true; }
bool extens_init (void) { return true; }



TEST_GROUP_RUNNER(nts) {
  RUN_TEST_CASE(nts, ke_append_record_null);
  RUN_TEST_CASE(nts, ke_append_record_uint16);
  RUN_TEST_CASE(nts, ke_append_record_bytes);
  RUN_TEST_CASE(nts, append_header);
  RUN_TEST_CASE(nts, append_uint16);
  RUN_TEST_CASE(nts, append_bytes);
  RUN_TEST_CASE(nts, ke_next_record);
  RUN_TEST_CASE(nts, next_uint16);
  RUN_TEST_CASE(nts, next_bytes);
}
