#include "libntptest.h"

extern "C" {
#include "ntp.h"
};

class ssl_initTest : public libntptest {
protected:
	static const size_t MD5_DIGEST_LENGTH = 16;
	static const size_t SHA1_DIGEST_LENGTH = 20;
};

// keytype_from_text()
TEST_F(ssl_initTest, MD5KeyTypeWithoutDigestLength) {
	ASSERT_EQ(KEY_TYPE_MD5, keytype_from_text("MD5", NULL));
}

TEST_F(ssl_initTest, MD5KeyTypeWithDigestLength) {
	size_t digestLength;
	size_t expected = MD5_DIGEST_LENGTH;

	EXPECT_EQ(KEY_TYPE_MD5, keytype_from_text("MD5", &digestLength));
	EXPECT_EQ(expected, digestLength);
}

TEST_F(ssl_initTest, SHA1KeyTypeWithDigestLength) {
	size_t digestLength;
	size_t expected = SHA1_DIGEST_LENGTH;

	EXPECT_EQ(NID_sha, keytype_from_text("SHA", &digestLength));
	EXPECT_EQ(expected, digestLength);
}

// keytype_name()
TEST_F(ssl_initTest, MD5KeyName) {
	EXPECT_STREQ("MD5", keytype_name(KEY_TYPE_MD5));
}

TEST_F(ssl_initTest, SHA1KeyName) {
	EXPECT_STREQ("SHA", keytype_name(NID_sha));
}
