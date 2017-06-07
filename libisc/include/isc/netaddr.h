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

typedef struct isc_netaddr	isc_netaddr_t;		/* Net Address */

/***
 *** Functions.
 ***/

bool isc_net_probeipv4_bool(void);
/*
 * Check if the system's kernel supports IPv4.
 *
 * Returns:
 *     True    IPv4 is supported.
 *     False   IPv4 is not supported.
 */

bool isc_net_probeipv6_bool(void);
/*
 * Check if the system's kernel supports IPv6.
 *
 * Returns:
 *     True    IPv4 is supported.
 *     False   IPv4 is not supported.
 */

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

struct isc_netaddr {
	unsigned int family;
	union {
		struct in_addr in;
		struct in6_addr in6;
	} type;
	uint32_t zone;
};

#endif /* GUARD_ISC_NETADDR_H */
