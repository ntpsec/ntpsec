#ifndef NETUTILS_H
#define NETUTILS_H

#include <sys/socket.h>
#include <string.h>

int async_resolve (int hnamec, char *hnamev[], char *adr_ptr[]);
int retrv_dnsadr (char *adr_ptr[]);

#endif
