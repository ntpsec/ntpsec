/*
 * ntp_dns.h - client interface to DNS name resolution.
 */
#ifndef GUARD_NTP_DNS_H
#define GUARD_NTP_DNS_H

/* Get addrinfo */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/* start DNS query (unless busy) */
extern bool dns_probe(struct peer*);

/* Process answers */
extern void server_take_dns(struct peer*, struct addrinfo*);
extern void pool_take_dns(struct peer*, struct addrinfo*);

extern void dns_check(void);	/* called by main thread */

#endif	/* GUARD_NTP_DNS_H */
