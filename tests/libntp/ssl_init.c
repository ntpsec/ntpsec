#include "config.h"

#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(ssl_init);

TEST_SETUP(ssl_init) {}

TEST_TEAR_DOWN(ssl_init) {}


#ifdef HAVE_OPENSSL
# include "openssl/err.h"
# include "openssl/rand.h"
# include "openssl/evp.h"
#endif
#include "ntp.h"

static const size_t TEST_MD5_DIGEST_LENGTH = 16;

#ifdef HAVE_OPENSSL
static const size_t TEST_SHA1_DIGEST_LENGTH = 20;
#endif

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

#ifdef HAVE_OPENSSL
TEST(ssl_init, SHA1KeyTypeWithDigestLength) {
	size_t digestLength;
	size_t expected = TEST_SHA1_DIGEST_LENGTH;

	TEST_ASSERT_EQUAL(NID_sha1, keytype_from_text("SHA1", &digestLength));
	TEST_ASSERT_EQUAL(expected, digestLength);
}
#endif	/* HAVE_OPENSSL */

// keytype_name()
TEST(ssl_init, MD5KeyName) {
	TEST_ASSERT_EQUAL_STRING("MD5", keytype_name(KEY_TYPE_MD5));
}

#ifdef HAVE_OPENSSL
TEST(ssl_init, SHA1KeyName) {
	TEST_ASSERT_EQUAL_STRING("SHA1", keytype_name(NID_sha1));
}
#endif	/* HAVE_OPENSSL */

TEST_GROUP_RUNNER(ssl_init) {
	RUN_TEST_CASE(ssl_init, MD5KeyTypeWithoutDigestLength);
	RUN_TEST_CASE(ssl_init, MD5KeyTypeWithDigestLength);
	RUN_TEST_CASE(ssl_init, MD5KeyName);

#ifdef HAVE_OPENSSL
	RUN_TEST_CASE(ssl_init, SHA1KeyTypeWithDigestLength);
	RUN_TEST_CASE(ssl_init, SHA1KeyName);
#endif
}
