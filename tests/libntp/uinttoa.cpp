#include "libntptest.h"

class uinttoaTest : public libntptest {
};

TEST_F(uinttoaTest, RegularNumber) {
	EXPECT_STREQ("39", uinttoa(39UL));
}

TEST_F(uinttoaTest, BiggestULong) {
	EXPECT_STREQ("4294967295", uinttoa(4294967295UL));
}
