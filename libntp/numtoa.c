/*
 * numtoa - return asciized network numbers store in local array space
 */
#include "config.h"

#include <sys/types.h>
#include <netinet/in.h>		/* ntohl */

#include <stdio.h>

#include "ntp_fp.h"
#include "lib_strbuf.h"
#include "ntp_stdlib.h"

char *
numtoa(
	uint32_t num
	)
{
	register uint32_t netnum;
	register char *buf;

	netnum = ntohl(num);
	LIB_GETBUF(buf);
	snprintf(buf, LIB_BUFLENGTH, "%lu.%lu.%lu.%lu",
		 ((unsigned long)netnum >> 24) & 0xff,
		 ((unsigned long)netnum >> 16) & 0xff,
		 ((unsigned long)netnum >> 8) & 0xff,
		 (unsigned long)netnum & 0xff);
	return buf;
}


/* Convert a refid & stratum to a string */
const char *
refid_str(
	uint32_t	refid,
	int	stratum
	)
{
	char *	text;
	size_t	tlen;

	if (stratum > 1)
		return numtoa(refid);

	LIB_GETBUF(text);
	text[0] = '.';
	memcpy(&text[1], &refid, sizeof(refid));
	text[1 + sizeof(refid)] = '\0';
	tlen = strlen(text);
	text[tlen] = '.';
	text[tlen + 1] = '\0';

	return text;
}

