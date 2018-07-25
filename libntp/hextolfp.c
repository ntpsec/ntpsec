/*
 * hextolfp - convert an ascii hex string to an l_fp number
 */
#include "config.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "ntp_fp.h"
#include "ntp_stdlib.h"

bool
hextolfp(
	const char *str,
	l_fp *lfp
	)
{
	const char *cp;
	const char *cpstart;
	unsigned long dec_i;
	unsigned long dec_f;
	char *ind = NULL;
	static const char *digits = "0123456789abcdefABCDEF";

	dec_i = dec_f = 0;
	cp = str;

	/*
	 * We understand numbers of the form:
	 *
	 * [spaces]8_hex_digits[.]8_hex_digits[spaces|\n|\0]
	 */
	while (isspace((unsigned char)*cp))
	    cp++;

	cpstart = cp;
	while (*cp != '\0' && (cp - cpstart) < 8 &&
	       (ind = strchr(digits, *cp)) != NULL) {
		dec_i = dec_i << 4;	/* multiply by 16 */
		dec_i += (unsigned long)(((ind - digits) > 15) ? (ind - digits) - 6
			: (ind - digits));
		cp++;
	}

	if ((cp - cpstart) < 8 || ind == NULL)
	    return false;
	if (*cp == '.')
	    cp++;

	cpstart = cp;
	while (*cp != '\0' && (cp - cpstart) < 8 &&
	       (ind = strchr(digits, *cp)) != NULL) {
		dec_f = dec_f << 4;	/* multiply by 16 */
		dec_f += (unsigned long)(((ind - digits) > 15) ? (ind - digits) - 6
			: (ind - digits));
		cp++;
	}

	if ((cp - cpstart) < 8 || ind == NULL)
	    return false;

	if (*cp != '\0' && !isspace((unsigned char)*cp))
	    return false;

	*lfp = lfpinit_u(dec_i, dec_f);
	return true;
}
