#include "libntptest.h"

class numtoaTest : public libntptest {
};

TEST_F(numtoaTest, Address) {
	u_int32 input = htonl(3221225472+512+1); // 192.0.2.1

	EXPECT_STREQ("192.0.2.1", numtoa(input));
}

TEST_F(numtoaTest, Netmask) {
	// 255.255.255.0
	u_int32 input = htonl(255*256*256*256 + 255*256*256 + 255*256);

	EXPECT_STREQ("255.255.255.0", numtoa(input));
}
