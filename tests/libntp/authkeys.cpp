#include "libntptest.h"

extern "C" {
#include "ntp.h"
#include "ntp_stdlib.h"
};

class authkeysTest : public libntptest {
protected:
	virtual void SetUp() {
		init_auth();

		/*
		 * init_auth() does not initialize global variables like authnumkeys,
		 * so let's reset them to zero here.
		 */
		authnumkeys = 0;
	}

	void AddTrustedKey(keyid_t keyno) {
		/*
		 * We need to add a MD5-key in addition to setting the
		 * trust, because authhavekey() requires type != 0.
		 */
		MD5auth_setkey(keyno, KEY_TYPE_MD5, NULL, 0);

		authtrust(keyno, 1);
	}

	void AddUntrustedKey(keyid_t keyno) {
		authtrust(keyno, 0);
	}
};

TEST_F(authkeysTest, AddTrustedKeys) {
	const keyid_t KEYNO1 = 5;
	const keyid_t KEYNO2 = 8;

	AddTrustedKey(KEYNO1);
	AddTrustedKey(KEYNO2);

	EXPECT_TRUE(authistrusted(KEYNO1));
	EXPECT_TRUE(authistrusted(KEYNO2));
}

TEST_F(authkeysTest, AddUntrustedKey) {
	const keyid_t KEYNO = 3;
   
	AddUntrustedKey(KEYNO);

	EXPECT_FALSE(authistrusted(KEYNO));
}

/*
TEST_F(authkeysTest, FindKey) {
	const keyid_t KEYNO1 = 2;
	const keyid_t KEYNO2 = 66;

	authtrust(KEYNO1, 1);
	authtrust(KEYNO2, 1);

	savekey* key1 = auth_findkey(KEYNO1);
	EXPECT_TRUE(key1 != NULL);
	EXPECT_EQ(KEYNO1, key1->keyid);
}
*/

TEST_F(authkeysTest, HaveKeyCorrect) {
	const keyid_t KEYNO = 3;

	AddTrustedKey(KEYNO);

	EXPECT_TRUE(auth_havekey(KEYNO));
	EXPECT_TRUE(authhavekey(KEYNO));
}

TEST_F(authkeysTest, HaveKeyIncorrect) {
	const keyid_t KEYNO = 2;

	EXPECT_FALSE(auth_havekey(KEYNO));
	EXPECT_FALSE(authhavekey(KEYNO));
}

/*TEST_F(authkeysTest, DeleteKeys) {
	const keyid_t KEYNO1 = 2;
	const keyid_t KEYNO2 = 5;
	const keyid_t KEYNO3 = 66;

	AddTrustedKey(KEYNO1);
	AddTrustedKey(KEYNO2);
	AddTrustedKey(KEYNO3);

	EXPECT_EQ(3, authnumkeys);

	auth_delkeys();

	EXPECT_FALSE(auth_havekey(KEYNO1));
	EXPECT_FALSE(auth_havekey(KEYNO2));
	EXPECT_FALSE(auth_havekey(KEYNO3));
}*/
