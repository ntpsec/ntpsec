/*
 * Why use strlcpy()/strlcat() instead of standard strncpy()/strncat()?
 * To reduce likelihood of bugs and avoid wasteful zero fills.  See:
 * http://www.gratisoft.us/todd/papers/strlcpy.html
 */

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#include "config.h"		/* + marks local changes */
#include <sys/types.h>
#include <string.h>

#include "ntp_stdlib.h"		/* + strlcpy, strlcat prototypes */

#ifndef HAVE_STRLCPY		/* + */
/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t
strlcpy(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0) {
		while (--n != 0) {
			if ((*d++ = *s++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}

	return((size_t)(s - src - 1));	/* count does not include NUL */
}
#endif				/* + */


/* #include <sys/types.h> */	/* + */
/* #include <string.h> */	/* + */

#ifndef HAVE_STRLCAT		/* + */
/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 */
size_t
strlcat(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end */
	while (n-- != 0 && *d != '\0')
		d++;
	dlen = (size_t)(d - dst);
	n = siz - dlen;

	if (n == 0)
		return(dlen + strlen(s));
	while (*s != '\0') {
		if (n != 1) {
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';

	return(dlen + (size_t)(s - src));	/* count does not include NUL */
}
#endif				/* + */
