#include "config.h"

#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(ssl_init);

TEST_SETUP(ssl_init) {}

TEST_TEAR_DOWN(ssl_init) {}


#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

#include "ntp.h"

// keytype_from_text()
TEST(ssl_init, MD5KeyType) {
	TEST_ASSERT_EQUAL(KEY_TYPE_MD5, keytype_from_text("MD5"));
}

TEST(ssl_init, MD5KeyTypeLegacy) {
	TEST_ASSERT_EQUAL(KEY_TYPE_MD5, keytype_from_text("M"));
}

TEST(ssl_init, SHA1KeyType) {
	TEST_ASSERT_EQUAL(NID_sha1, keytype_from_text("SHA1"));
}

TEST_GROUP_RUNNER(ssl_init) {
	RUN_TEST_CASE(ssl_init, MD5KeyType);
	RUN_TEST_CASE(ssl_init, MD5KeyTypeLegacy);

	RUN_TEST_CASE(ssl_init, SHA1KeyType);
}
