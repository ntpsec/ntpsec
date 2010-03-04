/*
 * socktoa.c	socktoa(), sockporttoa(), and sock_hash()
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <arpa/inet.h>

#ifdef ISC_PLATFORM_NEEDNTOP
#include <isc/net.h>
#endif

#include <stdio.h>

#include "ntp_fp.h"
#include "lib_strbuf.h"
#include "ntp_stdlib.h"
#include "ntp.h"

/*
 * socktoa - return a numeric host name from a sockaddr_storage structure
 */
char *
socktoa(
	const sockaddr_u *sock
	)
{
	register char *buffer;

	LIB_GETBUF(buffer);

	if (NULL == sock)
		strncpy(buffer, "(null)", LIB_BUFLENGTH);
	else {
		switch(AF(sock)) {

		case AF_INET:
		case AF_UNSPEC:
			inet_ntop(AF_INET, PSOCK_ADDR4(sock), buffer,
				  LIB_BUFLENGTH);
			break;

		case AF_INET6:
			inet_ntop(AF_INET6, PSOCK_ADDR6(sock), buffer,
				  LIB_BUFLENGTH);
			break;

		default:
			snprintf(buffer, LIB_BUFLENGTH, 
				 "(socktoa unknown family %d)", 
				 AF(sock));
		}
	}
	return buffer;
}


char *
sockporttoa(
	const sockaddr_u *sock
	)
{
	const char *atext;
	char *buf;

	atext = socktoa(sock);
	LIB_GETBUF(buf);
	if (IS_IPV4(sock))
		snprintf(buf, LIB_BUFLENGTH, "%s:%hu", atext, SRCPORT(sock));
	else
		snprintf(buf, LIB_BUFLENGTH, "[%s]:%hu", atext, SRCPORT(sock));

	return buf;
}


/*
 * sock_hash - hash a sockaddr_u structure
 */
u_short
sock_hash(
	const sockaddr_u *addr
	)
{
	u_int hashVal;
	u_int j;
	size_t len;
	const u_char *pch;

	hashVal = 0;
	len = 0;

	/*
	 * We can't just hash the whole thing because there are hidden
	 * fields in sockaddr_in6 that might be filled in by recvfrom(),
	 * so just use the family, port and address.
	 */
	pch = (const void *)&AF(addr);
	hashVal = 37 * hashVal + *pch;
	if (sizeof(AF(addr)) > 1) {
		pch++;
		hashVal = 37 * hashVal + *pch;
	}
	switch(AF(addr)) {
	case AF_INET:
		pch = (const void *)&SOCK_ADDR4(addr);
		len = sizeof(SOCK_ADDR4(addr));
		break;

	case AF_INET6:
		pch = (const void *)&SOCK_ADDR6(addr);
		len = sizeof(SOCK_ADDR6(addr));
		break;
	}

	for (j = 0; j < len ; j++)
		hashVal = 37 * hashVal + pch[j];

	return (u_short)(hashVal & USHRT_MAX);
}


