/*
 * decodenetnum - return a net number (this is crude, but careful)
 */
#include <sys/types.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "ntp_stdlib.h"
#include "ntp_assert.h"

int
decodenetnum(
	const char *num,
	struct sockaddr_storage *netnum
	)
{
	struct addrinfo hints, *ai = NULL;
	register int err;
	register const char *cp;
	char name[80];
	char *np;

	NTP_REQUIRE(num != NULL);
	NTP_REQUIRE(strlen(num) < sizeof(name));

	if ('[' != num[0]) 
		np = name;
	else {
		cp = num + 1;
		np = name; 
		while (*cp && ']' != *cp)
			*np++ = *cp++;
		*np = 0;
		np = name; 
	}
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_NUMERICHOST;
	err = getaddrinfo(np, NULL, &hints, &ai);
	if (err != 0)
		return 0;
	memcpy(netnum, ai->ai_addr, ai->ai_addrlen); 
	freeaddrinfo(ai);
	return 1;
}
