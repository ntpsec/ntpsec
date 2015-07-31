extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(authkeys);

TEST_SETUP(authkeys) {}

TEST_TEAR_DOWN(authkeys) {}

/* This file contains test for both libntp/authkeys.c and libntp/authusekey.c */

#include "libntptest.h"

extern "C" {
#ifdef OPENSSL
# include "openssl/err.h"
# include "openssl/rand.h"
# include "openssl/evp.h"
#endif
#include "ntp.h"
#include "ntp_stdlib.h"
};

class authkeysTest : public libntptest {
protected:
	static const int KEYTYPE = KEY_TYPE_MD5;

	virtual void SetUp() {
		/*
		 * init_auth() is called by tests_main.cpp earlier.  It
		 * does not initialize global variables like
		 * authnumkeys, so let's reset them to zero here.
		 */
		authnumkeys = 0;

		/*
		 * Especially, empty the key cache!
		 */
		cache_keyid = 0;
		cache_type = 0;
		cache_flags = 0;
		cache_secret = NULL;
		cache_secretsize = 0;
	}

	void AddTrustedKey(keyid_t keyno) {
		/*
		 * We need to add a MD5-key in addition to setting the
		 * trust, because authhavekey() requires type != 0.
		 */
		MD5auth_setkey(keyno, KEYTYPE, NULL, 0);

		authtrust(keyno, TRUE);
	}

	void AddUntrustedKey(keyid_t keyno) {
		authtrust(keyno, FALSE);
	}
};

TEST(authkeys, AddTrustedKeys) {
	const keyid_t KEYNO1 = 5;
	const keyid_t KEYNO2 = 8;

	AddTrustedKey(KEYNO1);
	AddTrustedKey(KEYNO2);

	TEST_ASSERT_TRUE(authistrusted(KEYNO1));
	TEST_ASSERT_TRUE(authistrusted(KEYNO2));
}

TEST(authkeys, AddUntrustedKey) {
	const keyid_t KEYNO = 3;
   
	AddUntrustedKey(KEYNO);

	TEST_ASSERT_FALSE(authistrusted(KEYNO));
}

TEST(authkeys, HaveKeyCorrect) {
	const keyid_t KEYNO = 3;

	AddTrustedKey(KEYNO);

	TEST_ASSERT_TRUE(auth_havekey(KEYNO));
	TEST_ASSERT_TRUE(authhavekey(KEYNO));
}

TEST(authkeys, HaveKeyIncorrect) {
	const keyid_t KEYNO = 2;

	TEST_ASSERT_FALSE(auth_havekey(KEYNO));
	TEST_ASSERT_FALSE(authhavekey(KEYNO));
}

TEST(authkeys, AddWithAuthUseKey) {
	const keyid_t KEYNO = 5;
	const char* KEY = "52a";

	TEST_ASSERT_TRUE(authusekey(KEYNO, KEYTYPE, (u_char*)KEY));
}

TEST(authkeys, EmptyKey) {
	const keyid_t KEYNO = 3;
	const char* KEY = "";


	TEST_ASSERT_FALSE(authusekey(KEYNO, KEYTYPE, (u_char*)KEY));
}
