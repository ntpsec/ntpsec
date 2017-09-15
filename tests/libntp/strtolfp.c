#include "config.h"
#include "ntp_stdlib.h"
#include "ntp_fp.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "ntp_assert.h"

#include "unity.h"
#include "unity_fixture.h"

static 	bool	mstolfp		(const char *, l_fp *);
static	bool	atolfp		(const char *, l_fp *);

/*
 * atolfp - convert an ascii string to an l_fp number
 */
/*
 * Powers of 10
 */
static unsigned long ten_to_the_n[10] = {
	0,
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	1000000000,
};


static bool
atolfp(
	const char *str,
	l_fp *lfp
	)
{
	const char *cp;
	unsigned long dec_i;
	unsigned long dec_f;
	char *ind;
	int ndec;
	bool isneg;
	static const char *digits = "0123456789";

	REQUIRE(str != NULL);

	isneg = false;
	dec_i = dec_f = 0;
	ndec = 0;
	cp = str;

	/*
	 * We understand numbers of the form:
	 *
	 * [spaces][-|+][digits][.][digits][spaces|\n|\0]
	 */
	while (isspace((unsigned char)*cp))
	    cp++;

	if (*cp == '-') {
		cp++;
		isneg = true;
	}

	if (*cp == '+')
	    cp++;

	if (*cp != '.' && !isdigit((unsigned char)*cp))
	    return false;

	while (*cp != '\0' && (ind = strchr(digits, *cp)) != NULL) {
		dec_i = (dec_i << 3) + (dec_i << 1);	/* multiply by 10 */
		dec_i += (unsigned long)(ind - digits);
		cp++;
	}

	if (*cp != '\0' && !isspace((unsigned char)*cp)) {
		if (*cp++ != '.')
		    return false;

		while (ndec < 9 && *cp != '\0'
		       && (ind = strchr(digits, *cp)) != NULL) {
			ndec++;
			dec_f = (dec_f << 3) + (dec_f << 1);	/* *10 */
			dec_f += (unsigned long)(ind - digits);
			cp++;
		}

		while (isdigit((unsigned char)*cp))
		    cp++;

		if (*cp != '\0' && !isspace((unsigned char)*cp))
		    return false;
	}

	if (ndec > 0) {
		unsigned long tmp;
		unsigned long bit;
		unsigned long ten_fact;

		ten_fact = ten_to_the_n[ndec];

		tmp = 0;
		bit = 0x80000000;
		while (bit != 0) {
			dec_f <<= 1;
			if (dec_f >= ten_fact) {
				tmp |= bit;
				dec_f -= ten_fact;
			}
			bit >>= 1;
		}
		if ((dec_f << 1) > ten_fact)
		    tmp++;
		dec_f = tmp;
	}

	if (isneg)
	{
		(dec_f) = ~(dec_f) + 1u;
		(dec_i) = ~(dec_i) + ((dec_f) == 0);
	}

	*lfp = lfpinit_u(dec_i, dec_f);
	return true;
}

/*
 * mstolfp - convert an ascii string in milliseconds to an l_fp number
 */
static bool
mstolfp(
	const char *str,
	l_fp *lfp
	)
{
	const char *cp;
	char *bp;
	const char *cpdec;
	char buf[100];

	/*
	 * We understand numbers of the form:
	 *
	 * [spaces][-][digits][.][digits][spaces|\n|\0]
	 *
	 * This is one enormous hack.  Since I didn't feel like
	 * rewriting the decoding routine for milliseconds, what
	 * is essentially done here is to make a copy of the string
	 * with the decimal moved over three places so the seconds
	 * decoding routine can be used.
	 */
	bp = buf;
	cp = str;
	while (isspace((unsigned char)*cp))
	    cp++;

	if (*cp == '-') {
		*bp++ = '-';
		cp++;
	}

	if (*cp != '.' && !isdigit((unsigned char)*cp))
	    return false;


	/*
	 * Search forward for the decimal point or the end of the string.
	 */
	cpdec = cp;
	while (isdigit((unsigned char)*cpdec))
	    cpdec++;

	/*
	 * Found something.  If we have more than three digits copy the
	 * excess over, else insert a leading 0.
	 */
	if ((cpdec - cp) > 3) {
		do {
			*bp++ = (char)*cp++;
		} while ((cpdec - cp) > 3);
	} else {
		*bp++ = '0';
	}

	/*
	 * Stick the decimal in.  If we've got less than three digits in
	 * front of the millisecond decimal we insert the appropriate number
	 * of zeros.
	 */
	*bp++ = '.';
	if ((cpdec - cp) < 3) {
		int i = 3 - (cpdec - cp);

		do {
			*bp++ = '0';
		} while (--i > 0);
	}

	/*
	 * Copy the remainder up to the millisecond decimal.  If cpdec
	 * is pointing at a decimal point, copy in the trailing number too.
	 */
	while (cp < cpdec)
	    *bp++ = (char)*cp++;

	if (*cp == '.') {
		cp++;
		while (isdigit((unsigned char)*cp))
		    *bp++ = (char)*cp++;
	}
	*bp = '\0';

	/*
	 * Check to make sure the string is properly terminated.  If
	 * so, give the buffer to the decoding routine.
	 */
	if (*cp != '\0' && !isspace((unsigned char)*cp))
	    return false;
	return atolfp(buf, lfp);
}
TEST_GROUP(strtolfp);

TEST_SETUP(strtolfp) {}

TEST_TEAR_DOWN(strtolfp) {}

#include "lfptest.h"

/* This class tests both atolfp and mstolfp */

TEST(strtolfp, PositiveInteger) {
	const char *str = "500";
	const char *str_ms = "500000";

	l_fp expected = lfpinit(500, 0);
	l_fp actual, actual_ms;

	TEST_ASSERT_TRUE(atolfp(str, &actual));
	TEST_ASSERT_TRUE(mstolfp(str_ms, &actual_ms));

	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));
	TEST_ASSERT_TRUE(IsEqual(&expected, &actual_ms));
}

TEST(strtolfp, NegativeInteger) {
	const char *str = "-300";
	const char *str_ms = "-300000";

	l_fp expected = lfpinit(-300, 0);
	l_fp actual, actual_ms;

	TEST_ASSERT_TRUE(atolfp(str, &actual));
	TEST_ASSERT_TRUE(mstolfp(str_ms, &actual_ms));

	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));
	TEST_ASSERT_TRUE(IsEqual(&expected, &actual_ms));
}

TEST(strtolfp, PositiveFraction) {
	const char *str = "+500.5";
	const char *str_ms = "500500.0";

	l_fp expected = lfpinit(500, HALF);
	l_fp actual, actual_ms;

	TEST_ASSERT_TRUE(atolfp(str, &actual));
	TEST_ASSERT_TRUE(mstolfp(str_ms, &actual_ms));

	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));
	TEST_ASSERT_TRUE(IsEqual(&expected, &actual_ms));
}

TEST(strtolfp, NegativeFraction) {
	const char *str = "-300.75";
	const char *str_ms = "-300750";

	l_fp expected = lfpinit(-301, QUARTER);
	l_fp actual, actual_ms;

	TEST_ASSERT_TRUE(atolfp(str, &actual));
	TEST_ASSERT_TRUE(mstolfp(str_ms, &actual_ms));

	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));
	TEST_ASSERT_TRUE(IsEqual(&expected, &actual_ms));
}

TEST(strtolfp, PositiveMsFraction) {
	const char *str = "300.00025";
	const char *str_ms = "300000.25";

	l_fp expected = lfpinit(300, QUARTER_PROMILLE_APPRX);
	l_fp actual, actual_ms;

	TEST_ASSERT_TRUE(atolfp(str, &actual));
	TEST_ASSERT_TRUE(mstolfp(str_ms, &actual_ms));

	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));
	TEST_ASSERT_TRUE(IsEqual(&expected, &actual_ms));
}

TEST(strtolfp, NegativeMsFraction) {
	const char *str = "-199.99975";
	const char *str_ms = "-199999.75";

	l_fp expected = lfpinit(-200, QUARTER_PROMILLE_APPRX);
	l_fp actual, actual_ms;

	TEST_ASSERT_TRUE(atolfp(str, &actual));
	TEST_ASSERT_TRUE(mstolfp(str_ms, &actual_ms));

	TEST_ASSERT_TRUE(IsEqual(&expected, &actual));
	TEST_ASSERT_TRUE(IsEqual(&expected, &actual_ms));
}

TEST(strtolfp, InvalidChars) {
	const char *str = "500.4a2";
	l_fp actual, actual_ms;

	TEST_ASSERT_FALSE(atolfp(str, &actual));
	TEST_ASSERT_FALSE(mstolfp(str, &actual_ms));
}

TEST_GROUP_RUNNER(strtolfp) {
	RUN_TEST_CASE(strtolfp, PositiveInteger);
	RUN_TEST_CASE(strtolfp, NegativeInteger);
	RUN_TEST_CASE(strtolfp, PositiveFraction);
	RUN_TEST_CASE(strtolfp, NegativeFraction);
	RUN_TEST_CASE(strtolfp, PositiveMsFraction);
	RUN_TEST_CASE(strtolfp, NegativeMsFraction);
	RUN_TEST_CASE(strtolfp, InvalidChars);
}
