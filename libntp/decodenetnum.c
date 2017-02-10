/*
 * decodenetnum - convert text IP address and port to sockaddr_u
 */
#include "config.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include "ntp_stdlib.h"
#include "ntp_assert.h"

/*
 * decodenetnum		convert text IP address and port to sockaddr_u
 *
 * Returns false for failure, true for success.
 *
 * We accept:
 * IPv4
 * IPv6
 * [IPv6]
 * IPv4:port
 * [IPv6]:port
 *
 * The IP must be numeric but the port can be symbolic.
 */
bool
decodenetnum(
	const char *num,
	sockaddr_u *netnum
	)
{
	struct addrinfo hints, *ai = NULL;
	const char *ip_start, *ip_end, *port_start, *temp;
	const size_t numlen = strlen(num);
	bool have_brackets;

	char ip[INET6_ADDRSTRLEN];

	NTP_REQUIRE(num != NULL);
	/* Quickly reject empty or impossibly long inputs. */
	if(numlen == 0 ||
	   numlen > (sizeof ip - 1) + (NI_MAXSERV - 1) + (3 /* "[]:" */)) {
		return false;
	}

	/* Is this a bracketed IPv6 address? */
	have_brackets = ('[' == num[0]);
	if(have_brackets) {
		/* If it's formatted like [IPv6]:port, the port part
		   comes after the "]:". */
		if((temp = strstr(num, "]:")) != NULL) {
			ip_start = num + 1;
			ip_end = temp;
			port_start = temp + 2;
		}
		else if(num[numlen-1] == ']') {
			/* It's just [IPv6]. */
			ip_start = num + 1;
			ip_end = ip_start + numlen - 1;
			port_start = NULL;
		}
		else {
			/* Anything else must be invalid. */
			return false;
		}
	}
	/* No brackets. Searching backward, see if there's at least one
	 * colon... */
	else if((temp = strrchr(num, ':')) != NULL) {
		/* ...and then look for a second one, searching forward. */
		if(strchr(num, ':') == temp) {
			/* Searching from both directions gave us the same
			   result, so there's only one colon. What's after
			   it is the port. */
			ip_start = num;
			ip_end = temp;
			port_start = temp + 1;
		} else {
			/* Two colons and no brackets, so it has to be a bare
			   IPv6 address */
			ip_start = num;
			ip_end = ip_start + numlen;
			port_start = NULL;
		}
	} else {
		/* No colon, no brackets. */
		ip_start = num;
		ip_end = ip_start + numlen;
		port_start = NULL;
	}

	/* Now we have ip_start pointing to the start of the IP,
	   ip_end pointing past the end of the IP, and port_start
	   either NULL or pointing to the start of the port. Check
	   whether the IP is short enough to possibly be valid and
	   if so copy it into ip. */
	if(ip_end - ip_start + 1 > (int)sizeof(ip)) {
		return false;
	} else {
		memcpy(ip, ip_start, ip_end - ip_start);
		ip[ip_end - ip_start] = '\0';
	}

	ZERO(hints);
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_NUMERICHOST;
	hints.ai_protocol = IPPROTO_UDP;
	/* One final validity check: only IPv6 addresses are allowed to
	 * have brackets. */
	hints.ai_family = have_brackets ? AF_INET6 : AF_UNSPEC;

	/* If we've gotten this far, then we still don't know that
	   either the IP address or the port is well-formed, but at
	   least they're unambiguously delimited from each other.
	   Let getaddrinfo() perform all further validation. */
	if(getaddrinfo(ip, port_start == NULL ? "ntp" : port_start,
		       &hints, &ai) != 0) {
		return false;
	}

	NTP_INSIST(ai->ai_addrlen <= sizeof(*netnum));
	if(netnum) {
		ZERO(*netnum);
		memcpy(netnum, ai->ai_addr, ai->ai_addrlen);
	}
	freeaddrinfo(ai);
	return true;
}
