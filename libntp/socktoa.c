/*
 * socktoa - return a numeric host name from a sockaddr_storage structure
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#include <stdio.h>

#include "ntp_fp.h"
#include "lib_strbuf.h"
#include "ntp_stdlib.h"
#include "ntp.h"


char *
socktoa(
	struct sockaddr_storage* sock
	)
{
#ifdef HAVE_IPV6
	register char *buffer;

	LIB_GETBUF(buffer);

	if (sock == NULL) printf("null");

	switch(sock->ss_family) {

		case AF_INET :
			inet_ntop(AF_INET, &GET_INADDR(*sock), buffer,
			    LIB_BUFLENGTH);
			break;

		case AF_INET6 :
			inet_ntop(AF_INET6, &GET_INADDR6(*sock), buffer,
			    LIB_BUFLENGTH);
	}
  	return buffer;
#else
	return numtoa(GET_INADDR(*sock));
#endif
}
