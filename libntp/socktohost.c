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
socktohost(
	struct sockaddr_storage* sock
	)
{
	register char *buffer;

	LIB_GETBUF(buffer);
	getnameinfo((struct sockaddr*)sock, SOCKLEN(sock), buffer, NI_MAXHOST, NULL, NULL, 0);

  	return buffer;
}
