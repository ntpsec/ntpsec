#include "config.h"
#include "ntp_stdlib.h"
#include "ntp_auth.h"

#include "unity.h"
#include "unity_fixture.h"

#include <openssl/evp.h>

#include "ntp.h"

TEST_GROUP(authkeys);

TEST_SETUP(authkeys) {
	/*
	 * init_auth() is called by tests_main.cpp earlier.  It
	 * does not initialize global variables like
	 * authnumkeys, so let's reset them to zero here.
	 */
	authnumkeys = 0;

}

TEST_TEAR_DOWN(authkeys) {}

/* This file contains test for libntp/authkeys.c */



static void AddTrustedKey(keyid_t);
static void AddUntrustedKey(keyid_t);

static void AddTrustedKey(keyid_t keyno) {
	/*
	 * We need to add a type and key in addition to setting the
	 * trust, because authlookup() requires type != AUTH_NONE.
	 */
	auth_setkey(keyno, AUTH_DIGEST, "MD5", NULL, 0);
	authtrust(keyno, true);
}

static void AddUntrustedKey(keyid_t keyno) {
	authtrust(keyno, false);
}


TEST(authkeys, AddTrustedKeys) {
	const keyid_t KEYNO1 = 5;
	const keyid_t KEYNO2 = 8;

	AddTrustedKey(KEYNO1);
	AddTrustedKey(KEYNO2);

	TEST_ASSERT_NOT_NULL(authlookup(KEYNO1, true));
	TEST_ASSERT_NOT_NULL(authlookup(KEYNO1, false));
	TEST_ASSERT_NOT_NULL(authlookup(KEYNO2, true));
	TEST_ASSERT_NOT_NULL(authlookup(KEYNO2, false));
}

TEST(authkeys, AddUntrustedKey) {
	const keyid_t KEYNO = 3;

	AddUntrustedKey(KEYNO);  /* gets type of AUTH_NULL */

	TEST_ASSERT_NULL(authlookup(KEYNO, true));
	TEST_ASSERT_NULL(authlookup(KEYNO, false));

	auth_setkey(KEYNO, AUTH_DIGEST, "MD5", NULL, 0);

	TEST_ASSERT_NULL(authlookup(KEYNO, true));
	TEST_ASSERT_NOT_NULL(authlookup(KEYNO, false));
}

TEST(authkeys, HaveKeyCorrect) {
	const keyid_t KEYNO = 3;

	AddTrustedKey(KEYNO);

	TEST_ASSERT_NOT_NULL(authlookup(KEYNO, true));
	TEST_ASSERT_NOT_NULL(authlookup(KEYNO, false));
}

TEST(authkeys, HaveKeyIncorrect) {
	const keyid_t KEYNO = 2;

	TEST_ASSERT_NULL(authlookup(KEYNO, true));
	TEST_ASSERT_NULL(authlookup(KEYNO, false));
}

TEST_GROUP_RUNNER(authkeys) {
	RUN_TEST_CASE(authkeys, AddTrustedKeys);
	RUN_TEST_CASE(authkeys, AddUntrustedKey);
	RUN_TEST_CASE(authkeys, HaveKeyCorrect);
	RUN_TEST_CASE(authkeys, HaveKeyIncorrect);
}
