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

/* called by main thread to do callbacks */
extern void dns_check(void);

/* Callbacks to process answers */
extern void server_take_dns(struct peer*, struct addrinfo*);
extern void pool_take_dns(struct peer*, struct addrinfo*);

#endif	/* GUARD_NTP_DNS_H */
