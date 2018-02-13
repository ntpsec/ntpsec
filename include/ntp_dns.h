/*
 * ntp_dns.h - client interface to DNS name resolution.
 */
#ifndef GUARD_NTP_DNS_H
#define GUARD_NTP_DNS_H

#include "ntp_net.h"

typedef enum {DNS_good, DNS_temp, DNS_error} DNS_Status;

/* start DNS query (unless busy) */
extern bool dns_probe(struct peer*);

/* called by main thread to do callbacks */
extern void dns_check(void);

/* Callbacks to process answers */
extern void dns_take_server(struct peer*, sockaddr_u*);
extern void dns_take_pool(struct peer*, sockaddr_u*);
extern void dns_take_status(struct peer*, DNS_Status);

/* New interface has appeared - try again */
extern void dns_new_interface(void);

#endif	/* GUARD_NTP_DNS_H */
