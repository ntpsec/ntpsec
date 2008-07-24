#ifndef NETWORK_H
#define NETWORK_H

#if 0
#include <sys/types.h>	/**/
/* #include <sys/socket.h>	/**/
/* #include <netinet/in.h>	/**/
/* #include <netdb.h>	/**/
#include <stdlib.h>	/**/
#include <string.h>	/**/
#endif
#include <strings.h>	/**/
#include <errno.h>	/**/

#include "data_formats.h"

/* FIXME, see portability issue 1 */ /* irrelevant for now */
#ifndef SYS_WINNT
#define SOCKET	int
#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1
#define closesocket close
#endif

/* Check if necessary or not */
/* Maximum number of sockets... should be specified in sntp.h later */
#define MAX_AF 2


int descriptors[MAX_AF];


/* From ntpdate.c */
int is_reachable (struct addrinfo *dst);

int resolve_hosts (char **hosts, int hostc, struct addrinfo **res);

/* From ntpdate.c */
void send_pkt (struct sockaddr_storage *dest, struct pkt *pkt, int len);

int recvdata (SOCKET rsock, struct sockaddr *sender, char *rdata, size_t rdata_length, char *done);

int recvpkt (SOCKET rsock, struct pkt *rpkt, struct pkt *spkt);

int filter_reachable (struct addrinfo **res, int resc);

#endif
