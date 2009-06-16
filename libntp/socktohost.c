/*
 * socktoa - return a numeric host name from a sockaddr_storage structure
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#include <stdio.h>

#include "ntp_fp.h"
#include "lib_strbuf.h"
#include "ntp_stdlib.h"
#include "ntp.h"


char *
socktohost(
	sockaddr_u *sock
	)
{
	register char *buffer;

	LIB_GETBUF(buffer);
	if (getnameinfo(&sock->sa, SOCKLEN(sock), buffer,
	    LIB_BUFLENGTH, NULL, 0, 0))
		return stoa(sock);

	return buffer;
}
