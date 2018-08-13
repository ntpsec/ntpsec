/*
 * socktoa.c	socktoa(), sockporttoa(), and sock_hash()
 */

#include "config.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <arpa/inet.h>
#include "isc_netaddr.h"

#include "ntp_fp.h"
#include "lib_strbuf.h"
#include "ntp_stdlib.h"
#include "ntp.h"

/*
 * socktoa - return a numeric host name from a sockaddr_storage structure
 */
const char *
socktoa(
	const sockaddr_u *sock
	)
{
	int		saved_errno;
	char *		res;
	char *		addr;
	unsigned long	scope;

	saved_errno = errno;
	res = lib_getbuf();

	if (NULL == sock) {
		strlcpy(res, "(null)", LIB_BUFLENGTH);
	} else {
		switch(AF(sock)) {

		case AF_INET:
		case AF_UNSPEC:
			inet_ntop(AF_INET, PSOCK_ADDR4(sock), res,
				  LIB_BUFLENGTH);
			break;

		case AF_INET6:
			inet_ntop(AF_INET6, PSOCK_ADDR6(sock), res,
				  LIB_BUFLENGTH);
			scope = SCOPE_VAR(sock);
			if (0 != scope && !strchr(res, '%')) {
				addr = res;
				res = lib_getbuf();
				snprintf(res, LIB_BUFLENGTH, "%s%%%lu",
					 addr, scope);
				res[LIB_BUFLENGTH - 1] = '\0';
			}
			break;

		default:
			snprintf(res, LIB_BUFLENGTH,
				 "(socktoa unknown family %d)",
				 AF(sock));
		}
	}
	errno = saved_errno;

	return res;
}


const char *
sockporttoa(
	const sockaddr_u *sock
	)
{
	int		saved_errno;
	const char *	atext;
	char *		buf;

	saved_errno = errno;
	atext = socktoa(sock);
	buf = lib_getbuf();
	snprintf(buf, LIB_BUFLENGTH,
		 (IS_IPV6(sock))
		     ? "[%s]:%hu"
		     : "%s:%hu",
		 atext, SRCPORT(sock));
	errno = saved_errno;

	return buf;
}


/*
 * sock_hash - hash a sockaddr_u structure
 */
unsigned short
sock_hash(
	const sockaddr_u *addr
	)
{
	unsigned int hashVal;
	size_t len;
	const uint8_t *pch;

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
        default:
                /* huh? */
                break;
	}

	for (unsigned int j = 0; j < len ; j++)
		hashVal = 37 * hashVal + pch[j];

	return (unsigned short)(hashVal & USHRT_MAX);
}
