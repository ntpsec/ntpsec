#ifndef NETWORK_H
#define NETWORK_H

#include <strings.h>	
#include <errno.h>

#include <ntp_stdlib.h>

#include "data_formats.h"
/* FIXME, see portability issue 1 */ /* irrelevant for now */
#ifndef SYS_WINNT
#define SOCKET	int
#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1
#define closesocket close
#endif


/* From ntpdate.c */
int is_reachable (struct addrinfo *dst);

int resolve_hosts (char **hosts, int hostc, struct addrinfo **res);

void create_socket (SOCKET *rsock, struct sockaddr_storage *dest);

void close_socket (SOCKET rsock);

void send_pkt (SOCKET rsock, struct sockaddr_storage *dest, struct pkt *pkt, int len);

int recvdata (SOCKET rsock, struct sockaddr_storage *sender, char *rdata, size_t rdata_length, char *done);

int recvpkt (SOCKET rsock, struct pkt *rpkt, struct pkt *spkt);

#endif
