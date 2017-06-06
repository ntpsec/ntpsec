/*
 * Copyright (C) 2004-2007, 2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1998-2002  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_NETADDR_H
#define GUARD_ISC_NETADDR_H 1

/*! \file isc/netaddr.h */

#include <stdbool.h>
/* for struct sockaddr on *BSD */
#include <sys/socket.h>                /* Contractual promise. */

#include <netinet/in.h>		/* Contractual promise. */
#include "isc/result.h"

/*
 * Basic Networking Types
 *
 * This module is responsible for defining the following basic networking
 * types:
 *
 *		struct in_addr
 *		struct in6_addr
 *		struct in6_pktinfo
 *		struct sockaddr
 *		struct sockaddr_in
 *		struct sockaddr_in6
 *i		in_port_t
 *
 * It declares ntoh[sl]() and hton[sl]().
 *
 * MP:
 *	No impact.
 *
 * Reliability:
 *	No anticipated impact.
 *
 * Resources:
 *	N/A.
 *
 * Security:
 *	No anticipated impact.
 *
 * Standards:
 *	BSD Socket API
 *	RFC 2553
 */

typedef struct isc_sockaddr	isc_sockaddr_t;		/* Socket Address */
typedef struct isc_netaddr	isc_netaddr_t;		/* Net Address */

/***
 *** Functions.
 ***/

bool isc_net_probeipv4_bool(void);

/*
 * Check if the system's kernel supports IPv4.
 *
 * Returns:
 *
 *	#ISC_R_SUCCESS		IPv4 is supported.
 *	#ISC_R_NOTFOUND		IPv4 is not supported.
 *	#ISC_R_DISABLED		IPv4 is disabled.
 *	#ISC_R_UNEXPECTED
 */

bool isc_net_probeipv6_bool(void);

isc_result_t
isc_net_probeipv6(void);
/*
 * Check if the system's kernel supports IPv6.
 *
 * Returns:
 *
 *	#ISC_R_SUCCESS		IPv6 is supported.
 *	#ISC_R_NOTFOUND		IPv6 is not supported.
 *	#ISC_R_DISABLED		IPv6 is disabled.
 *	#ISC_R_UNEXPECTED
 */

bool
isc_net_probe_ipv6only_bool(void);
isc_result_t
isc_net_probe_ipv6only(void);

/*
 * Check if the system's kernel supports the IPV6_(RECV)PKTINFO socket option
 * for UDP sockets.
 *
 * Returns:
 *
 * 	#ISC_R_SUCCESS		the option is supported.
 * 	#ISC_R_NOTFOUND		IPv6 itself or the option is not supported.
 * 	#ISC_R_UNEXPECTED
 */


struct isc_netaddr {
	unsigned int family;
	union {
		struct in_addr in;
		struct in6_addr in6;
	} type;
	uint32_t zone;
};

bool
isc_netaddr_eqprefix(const isc_netaddr_t *, const isc_netaddr_t *,
		     unsigned int) __attribute__((pure));
void
isc_netaddr_fromsockaddr(isc_netaddr_t *netaddr, const isc_sockaddr_t *source);

void
isc_netaddr_fromin(isc_netaddr_t *netaddr, const struct in_addr *ina);

void
isc_netaddr_fromin6(isc_netaddr_t *netaddr, const struct in6_addr *ina6);

void
isc_netaddr_setzone(isc_netaddr_t *netaddr, uint32_t zone);

bool
isc_netaddr_islinklocal(isc_netaddr_t *na) __attribute__((pure));
/*%<
 * Returns #true if the address is a link local address.
 */

#endif /* GUARD_ISC_NETADDR_H */
