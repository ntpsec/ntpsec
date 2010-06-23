#include "lfptest.h"

extern "C" {
#include "ntp_unixtime.h"
};

class tvtotsTest : public lfptest {
protected:
	static const u_long HALF = 2147483648UL;
	static const u_long HALF_PROMILLE_UP = 2147484; // slightly more than 0.0005
	static const u_long HALF_PROMILLE_DOWN = 2147483; // slightly less than 0.0005
};

TEST_F(tvtotsTest, Seconds) {
	timeval input = {500, 0}; // 500.0 s
	l_fp expected = {500, 0};
	l_fp actual;

	TVTOTS(&input, &actual);

	EXPECT_TRUE(IsEqual(expected, actual));
}

TEST_F(tvtotsTest, MicrosecondsRounded) {
	/* 0.0005 can not be represented exact in a l_fp structure.
	 * It would equal to 2147483,648. This means that
	 * HALF_PROMILLE_UP (which is 2147484) should be
	 * the correct rounding. */

	timeval input = {0, 500}; // 0.0005 exact
	l_fp expected = {0, HALF_PROMILLE_UP};
	l_fp actual;

	TVTOTS(&input, &actual);
	EXPECT_TRUE(IsEqual(expected, actual));
}

TEST_F(tvtotsTest, MicrosecondsExact) {
	// 0.5 can be represented exact in both l_fp and timeval.

	timeval input = {10, 500000}; // 0.5 exact
	l_fp expected = {10, HALF}; // 0.5 exact
	l_fp actual;

	TVTOTS(&input, &actual);
	EXPECT_TRUE(IsEqual(expected, actual));
}
