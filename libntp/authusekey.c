/*
 * authusekey - decode a key from ascii and use it
 */
#include <config.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "ntp_types.h"
#include "ntp_stdlib.h"

/*
 * Types of ascii representations for keys.  "Standard" means a 64 bit
 * hex number in NBS format, i.e. with the low order bit of each byte
 * a parity bit.  "NTP" means a 64 bit key in NTP format, with the
 * high order bit of each byte a parity bit.  "Ascii" means a 1-to-8
 * character string whose ascii representation is used as the key.
 */
bool
authusekey(
	keyid_t keyno,
	int keytype,
	const u_char *str
	)
{
	int len;

	len = strlen((const char *)str);
	if (0 == len)
		return false;

	MD5auth_setkey(keyno, keytype, str, len);
	return true;
}
