#include "libntptest.h"

class tsftomsuTest : public libntptest {
protected:
	static const u_long HALF = 2147483648UL;
	static const u_long HALF_PROMILLE_UP = 2147484; // slightly more than 0.0005
	static const u_long HALF_PROMILLE_DOWN = 2147483; // slightly less than 0.0005
	static const u_long QUARTER = 1073741824L;
};

TEST_F(tsftomsuTest, Half) {
	EXPECT_EQ(500, tsftomsu(HALF, 0));
}

TEST_F(tsftomsuTest, Quarter) {
	EXPECT_EQ(250, tsftomsu(QUARTER, 1));
}

TEST_F(tsftomsuTest, HalfPromilleRoundUp) {
	EXPECT_EQ(1, tsftomsu(HALF_PROMILLE_UP, 1));
}

TEST_F(tsftomsuTest, HalfPromilleRoundDown) {
	EXPECT_EQ(0, tsftomsu(HALF_PROMILLE_DOWN, 1));
}

TEST_F(tsftomsuTest, HalfPromilleTruncate) {
	EXPECT_EQ(0, tsftomsu(HALF_PROMILLE_UP, 0));
}
