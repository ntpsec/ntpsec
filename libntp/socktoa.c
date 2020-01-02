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
	char *buf = lib_getbuf();
	socktoa_r(sock, buf, LIB_BUFLENGTH);
	return buf;
}

const char *
socktoa_r(
	const sockaddr_u *sock, char *buf, size_t buflen
	)
{
	int		saved_errno;
	unsigned long	scope;

	saved_errno = errno;

	if (NULL == sock) {
		strlcpy(buf, "(null)", buflen);
	} else {
		switch(AF(sock)) {

		case AF_INET:
		case AF_UNSPEC:
			inet_ntop(AF_INET, PSOCK_ADDR4(sock), buf, buflen);
			break;

		case AF_INET6:
			inet_ntop(AF_INET6, PSOCK_ADDR6(sock), buf, buflen);
			scope = SCOPE_VAR(sock);
			if (0 != scope && !strchr(buf, '%')) {
				char buf2[LIB_BUFLENGTH];
				snprintf(buf2, sizeof(buf2), "%s%%%lu",
					 buf, scope);
				buf2[LIB_BUFLENGTH - 1] = '\0';
				strlcpy(buf, buf2, buflen);
			}
			break;

		default:
			snprintf(buf, buflen,
				 "(socktoa unknown family %d)",
				 AF(sock));
		}
	}
	errno = saved_errno;

	return buf;
}


const char *
sockporttoa(
	const sockaddr_u *sock
	)
{
	char *buf = lib_getbuf();
	sockporttoa_r(sock, buf, LIB_BUFLENGTH);
	return buf;
}

const char *
sockporttoa_r(
	const sockaddr_u *sock, char *buf, size_t buflen
	)
{
	int saved_errno;
        char buf2[LIB_BUFLENGTH];

	saved_errno = errno;
	socktoa_r(sock, buf2, sizeof(buf2));
	snprintf(buf, buflen,
		 (IS_IPV6(sock))
		     ? "[%s]:%hu"
		     : "%s:%hu",
		 buf2, SRCPORT(sock));
	errno = saved_errno;

	return buf;
}


/*
 * sock_hash - hash a sockaddr_u structure
 */
unsigned int
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

	for (unsigned int j = 0; j < len ; j++) {
		hashVal = 37 * hashVal + pch[j];
	}

	return hashVal;
}
