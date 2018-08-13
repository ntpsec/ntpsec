/*
 * netof6 - return the net address part of an IPv6 address
 *  in a sockaddr_storage structure (zero out host part)
 *
 * except it does not really do that, it ASSumes /64
 *
 * returns points to a 8 position static array.  Each
 * position used in turn.
 *
 */
#include "config.h"
#include <stdio.h>
#include <syslog.h>

#include "ntp_net.h"
#include "ntp_stdlib.h"
#include "ntp.h"

sockaddr_u *
netof6(
	sockaddr_u *hostaddr
	)
{
	static sockaddr_u	netofbuf[8];
	static int		next_netofbuf;
	sockaddr_u *		netaddr;

	netaddr = &netofbuf[next_netofbuf];
	next_netofbuf = (next_netofbuf + 1) % (int)COUNTOF(netofbuf);

	memcpy(netaddr, hostaddr, sizeof(*netaddr));

	if (IS_IPV6(netaddr))
		/* assume the typical /64 subnet size */
		memset(&NSRCADR6(netaddr)[8], '\0', 8);
#ifdef DEBUG
	else {
		msyslog(LOG_ERR, "ERR: Not IPv6, AF %d", AF(netaddr));
		exit(1);
	}
#endif

	return netaddr;
}
