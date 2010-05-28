#include "libntptest.h"

extern "C" {
#include "ntp_stdlib.h"
}

class hextointTest : public libntptest {
	
};

TEST_F(hextointTest, Test1) {
	char dec8[] = "8";
	char dec31[] = "1F";
	char decmaxsigned[] = "80000000";
	char invalid1[] = "5gb";
	
	u_long res;

	ASSERT_TRUE(hextoint(dec8, &res));
	ASSERT_EQ(8, res);

	ASSERT_TRUE(hextoint(dec31, &res));
	ASSERT_EQ(31, res);

	ASSERT_TRUE(hextoint(decmaxsigned, &res));
	ASSERT_EQ(2147483648, res);

	ASSERT_FALSE(hextoint(invalid1, &res));
}
