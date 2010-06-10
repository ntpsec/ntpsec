#include "lfptest.h"

class hextolfpTest : public lfptest {
};

TEST_F(hextolfpTest, PositiveInteger) {
	const char *str = "00001000.00000000";
	l_fp actual;

	l_fp expected = {4096, 0}; // 16^3, no fraction part.

	ASSERT_TRUE(hextolfp(str, &actual));
	EXPECT_TRUE(IsEqual(expected, actual));
}

TEST_F(hextolfpTest, NegativeInteger) {
	const char *str = "ffffffff.00000000"; // -1 decimal
	l_fp actual;

	l_fp expected = {-1, 0};

	ASSERT_TRUE(hextolfp(str, &actual));
	EXPECT_TRUE(IsEqual(expected, actual));
}

