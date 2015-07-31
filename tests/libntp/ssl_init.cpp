extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(ssl_init);

TEST_SETUP(ssl_init) {}

TEST_TEAR_DOWN(ssl_init) {}

#include "libntptest.h"

extern "C" {
#ifdef OPENSSL
# include "openssl/err.h"
# include "openssl/rand.h"
# include "openssl/evp.h"
#endif
#include "ntp.h"
};

class ssl_initTest : public libntptest {
protected:
	static const size_t TEST_MD5_DIGEST_LENGTH = 16;
	static const size_t TEST_SHA1_DIGEST_LENGTH = 20;
};

// keytype_from_text()
TEST(ssl_init, MD5KeyTypeWithoutDigestLength) {
	TEST_ASSERT_EQUAL(KEY_TYPE_MD5, keytype_from_text("MD5", NULL));
}

TEST(ssl_init, MD5KeyTypeWithDigestLength) {
	size_t digestLength;
	size_t expected = TEST_MD5_DIGEST_LENGTH;

	TEST_ASSERT_EQUAL(KEY_TYPE_MD5, keytype_from_text("MD5", &digestLength));
	TEST_ASSERT_EQUAL(expected, digestLength);
}

#ifdef OPENSSL
TEST(ssl_init, SHA1KeyTypeWithDigestLength) {
	size_t digestLength;
	size_t expected = TEST_SHA1_DIGEST_LENGTH;

	TEST_ASSERT_EQUAL(NID_sha, keytype_from_text("SHA", &digestLength));
	TEST_ASSERT_EQUAL(expected, digestLength);
}
#endif	/* OPENSSL */

// keytype_name()
TEST(ssl_init, MD5KeyName) {
	TEST_ASSERT_EQUAL_STRING("MD5", keytype_name(KEY_TYPE_MD5));
}

#ifdef OPENSSL
TEST(ssl_init, SHA1KeyName) {
	TEST_ASSERT_EQUAL_STRING("SHA", keytype_name(NID_sha));
}
#endif	/* OPENSSL */
