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

/* Convert a refid & stratum to a string */
const char *
refid_str(
	uint32_t	refid,
	int	stratum
	)
{
	char *	text;
	size_t	tlen;

	if (stratum > 1) {
		struct in_addr in4;
		in4.s_addr = refid;
		return inet_ntoa(in4);
	}

	text = lib_getbuf();
	text[0] = '.';
	memcpy(&text[1], &refid, sizeof(refid));
	text[1 + sizeof(refid)] = '\0';
	tlen = strlen(text);
	text[tlen] = '.';
	text[tlen + 1] = '\0';

	return text;
}

