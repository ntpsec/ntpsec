/*
 * atolfp - convert an ascii string to an l_fp number
 */
#include "config.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "ntp_fp.h"
#include "ntp_assert.h"

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


bool
atolfp(
	const char *str,
	l_fp *lfp
	)
{
	register const char *cp;
	register unsigned long dec_i;
	register unsigned long dec_f;
	char *ind;
	int ndec;
	bool isneg;
	static const char *digits = "0123456789";

	NTP_REQUIRE(str != NULL);

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
		register unsigned long tmp;
		register unsigned long bit;
		register unsigned long ten_fact;

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

	*lfp = lfpinit(dec_i, dec_f);
	return true;
}
