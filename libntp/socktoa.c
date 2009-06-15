/*
 * socktoa - return a numeric host name from a sockaddr_storage structure
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#ifdef ISC_PLATFORM_NEEDNTOP
#include <isc/net.h>
#endif

#include <stdio.h>

#include "ntp_fp.h"
#include "lib_strbuf.h"
#include "ntp_stdlib.h"
#include "ntp.h"

char *
socktoa(
	sockaddr_u *sock
	)
{
	register char *buffer;

	LIB_GETBUF(buffer);

	if (sock == NULL)
		strncpy(buffer, "(null)", LIB_BUFLENGTH);
	else {
		switch(AF(sock)) {

		case AF_INET :
			inet_ntop(AF_INET, PSOCK_ADDR4(sock), buffer,
			    LIB_BUFLENGTH);
			break;

		case AF_INET6 :
			inet_ntop(AF_INET6, PSOCK_ADDR6(sock), buffer,
			    LIB_BUFLENGTH);

		default:
			strncpy(buffer, "(socktoa unknown family)",
				LIB_BUFLENGTH);
		}
	}
	return buffer;
}
