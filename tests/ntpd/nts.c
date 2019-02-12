#include "nts.h"
#include "unity.h"
#include "unity_fixture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TEST_GROUP(nts);

TEST_SETUP(nts) {}

TEST_TEAR_DOWN(nts) {}

TEST(nts, nts_append_record_null) {
  // Setup
  uint8_t buf[128];
  BufCtl cursor;
  cursor.next = buf;
  cursor.left = 128;
  // Run test
  nts_append_record_null(&cursor, 0xFA7E);
  // Check
  TEST_ASSERT_EQUAL(buf[0], 0xFA);
  TEST_ASSERT_EQUAL(buf[1], 0x7E);
  TEST_ASSERT_EQUAL(buf[2], 0);
  TEST_ASSERT_EQUAL(buf[3], 0);
  TEST_ASSERT_EQUAL(cursor.next, &buf[4]);
  TEST_ASSERT_EQUAL(cursor.left, 124);
}

TEST_GROUP_RUNNER(nts) {
  RUN_TEST_CASE(nts, nts_append_record_null);
}
