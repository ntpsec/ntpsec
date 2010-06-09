#include "libntptest.h"

extern "C" {
#include "ntp_fp.h"
};

class atolfpTest : public libntptest {
protected:
	::testing::AssertionResult IsEqual(const l_fp &expected, const l_fp &actual) {
		if (L_ISEQU(&expected, &actual)) {
			return ::testing::AssertionSuccess();
		} else {
			return ::testing::AssertionFailure()
				<< " expected: " << lfptoa(&expected, FRACTION_PREC)
				<< " but was: " << lfptoa(&actual, FRACTION_PREC);
		}
	}
};

TEST_F(atolfpTest, PositiveInteger) {
	const char *str = "500";
	l_fp expected = {500,0};
	l_fp actual;

	ASSERT_TRUE(atolfp(str, &actual));
	EXPECT_TRUE(IsEqual(expected, actual));
}

TEST_F(atolfpTest, NegativeInteger) {
	const char *str = "-300";
	l_fp expected = {-300,0};
	l_fp actual;

	ASSERT_TRUE(atolfp(str, &actual));
	EXPECT_TRUE(IsEqual(expected, actual));
}

TEST_F(atolfpTest, PositiveFraction) {
	const char *str = "+500.5";
	l_fp expected = {500, -2147483648};
	l_fp actual;

	ASSERT_TRUE(atolfp(str, &actual));
	EXPECT_TRUE(IsEqual(expected, actual));
}

TEST_F(atolfpTest, NegativeFraction) {
	const char *str = "-300.75";
	l_fp expected = {-301, 1073741824}; // Fraction is 2^32 * 1/4
	l_fp actual;

	ASSERT_TRUE(atolfp(str, &actual));
	EXPECT_TRUE(IsEqual(expected, actual));
}

TEST_F(atolfpTest, InvalidChars) {
	const char *str = "500.4a2";
	l_fp actual;

	ASSERT_FALSE(atolfp(str, &actual));
}
