#include "nts_lib.h"
#include "unity.h"
#include "unity_fixture.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TEST_GROUP(nts_lib);

TEST(nts_lib, existance) {
  TEST_ASSERT_NOT_NULL(upf);
  TEST_ASSERT_NOT_NULL(nts_record_form)
  TEST_ASSERT_NOT_NULL(nts_record_parse)
  TEST_ASSERT_NOT_NULL(nts_cookie_prep);
  TEST_ASSERT_NOT_NULL(nts_cookie_clean);
  TEST_ASSERT_NOT_NULL(nts_cookie_plaintext_parse);
  TEST_ASSERT_NOT_NULL(nts_cookie_plaintext_form);
}

TEST_GROUP_RUNNER(nts_lib) { RUN_TEST_CASE(nts_lib, existance); }
