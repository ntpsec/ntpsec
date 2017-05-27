/*
 * Copyright (C) 2004, 2005, 2007, 2008, 2012  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2003  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_NET_H
#define GUARD_ISC_NET_H 1

/*****
 ***** Module Info
 *****/

/*! \file
 * \brief
 * Basic Networking Types
 *
 * This module is responsible for defining the following basic networking
 * types:
 *
 *\li		struct in_addr
 *\li		struct in6_addr
 *\li		struct in6_pktinfo
 *\li		struct sockaddr
 *\li		struct sockaddr_in
 *\li		struct sockaddr_in6
 *\li		in_port_t
 *
 * It declares ntoh[sl]() and hton[sl]().
 *
 * MP:
 *\li	No impact.
 *
 * Reliability:
 *\li	No anticipated impact.
 *
 * Resources:
 *\li	N/A.
 *
 * Security:
 *\li	No anticipated impact.
 *
 * Standards:
 *\li	BSD Socket API
 *\li	RFC 2553
 */

/***
 *** Imports.
 ***/

#include <sys/types.h>
#include <sys/socket.h>		/* Contractual promise. */

#include <net/if.h>

#include <netinet/in.h>		/* Contractual promise. */
#include <arpa/inet.h>		/* Contractual promise. */

#include <isc/lang.h>
#include <isc/types.h>

/*% IP address. */
#define ISC__IPADDR(x)	((isc_uint32_t)htonl((isc_uint32_t)(x)))

/*% Is IP address multicast? */
#define ISC_IPADDR_ISMULTICAST(i) \
		(((isc_uint32_t)(i) & ISC__IPADDR(0xf0000000)) \
		 == ISC__IPADDR(0xe0000000))

#define ISC_IPADDR_ISEXPERIMENTAL(i) \
		(((isc_uint32_t)(i) & ISC__IPADDR(0xf0000000)) \
		 == ISC__IPADDR(0xf0000000))

/***
 *** Functions.
 ***/

ISC_LANG_BEGINDECLS

bool isc_net_probeipv4_bool(void);

isc_result_t
isc_net_probeipv4(void);
/*%<
 * Check if the system's kernel supports IPv4.
 *
 * Returns:
 *
 *\li	#ISC_R_SUCCESS		IPv4 is supported.
 *\li	#ISC_R_NOTFOUND		IPv4 is not supported.
 *\li	#ISC_R_DISABLED		IPv4 is disabled.
 *\li	#ISC_R_UNEXPECTED
 */

bool isc_net_probeipv6_bool(void);

isc_result_t
isc_net_probeipv6(void);
/*%<
 * Check if the system's kernel supports IPv6.
 *
 * Returns:
 *
 *\li	#ISC_R_SUCCESS		IPv6 is supported.
 *\li	#ISC_R_NOTFOUND		IPv6 is not supported.
 *\li	#ISC_R_DISABLED		IPv6 is disabled.
 *\li	#ISC_R_UNEXPECTED
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
 * \li	#ISC_R_SUCCESS		the option is supported.
 * \li	#ISC_R_NOTFOUND		IPv6 itself or the option is not supported.
 * \li	#ISC_R_UNEXPECTED
 */

ISC_LANG_ENDDECLS

#endif /* GUARD_ISC_NET_H */
