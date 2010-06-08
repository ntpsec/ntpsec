#include "libntptest.h"

extern "C" {
#include "ntp.h"
#include "ntp_stdlib.h"
};

class authkeysTest : public libntptest {
protected:
	virtual void SetUp() {
		init_auth();
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

TEST_F(authkeysTest, AddTrustedKey) {
	const keyid_t KEYNO = 5;

	AddTrustedKey(KEYNO);

	EXPECT_TRUE(authistrusted(KEYNO));
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
