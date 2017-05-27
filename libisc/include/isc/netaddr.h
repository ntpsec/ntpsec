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
#include "isc/lang.h"
#include "isc/net.h"
#include "isc/types.h"

ISC_LANG_BEGINDECLS

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
/*%<
 * Format a human-readable representation of the network address '*na'
 * into the character array 'array', which is of size 'size'.
 * The resulting string is guaranteed to be null-terminated.
 */

#define ISC_NETADDR_FORMATSIZE \
	sizeof("xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:XXX.XXX.XXX.XXX%SSSSSSSSSS")
/*%<
 * Minimum size of array to pass to isc_netaddr_format().
 */

void
isc_netaddr_fromsockaddr(isc_netaddr_t *netaddr, const isc_sockaddr_t *source);

void
isc_netaddr_fromin(isc_netaddr_t *netaddr, const struct in_addr *ina);

void
isc_netaddr_fromin6(isc_netaddr_t *netaddr, const struct in6_addr *ina6);

isc_result_t
isc_netaddr_frompath(isc_netaddr_t *netaddr, const char *path);

void
isc_netaddr_setzone(isc_netaddr_t *netaddr, uint32_t zone);

void
isc_netaddr_any(isc_netaddr_t *netaddr);
/*%<
 * Return the IPv4 wildcard address.
 */

void
isc_netaddr_any6(isc_netaddr_t *netaddr);
/*%<
 * Return the IPv6 wildcard address.
 */

bool
isc_netaddr_ismulticast(isc_netaddr_t *na);
/*%<
 * Returns true if the address is a multicast address.
 */

bool
isc_netaddr_isexperimental(isc_netaddr_t *na);
/*%<
 * Returns true if the address is a experimental (CLASS E) address.
 */

bool
isc_netaddr_islinklocal(isc_netaddr_t *na) __attribute__((pure));
/*%<
 * Returns #true if the address is a link local address.
 */

bool
isc_netaddr_issitelocal(isc_netaddr_t *na);
/*%<
 * Returns #true if the address is a site local address.
 */

void
isc_netaddr_fromv4mapped(isc_netaddr_t *t, const isc_netaddr_t *s);
/*%<
 * Convert an IPv6 v4mapped address into an IPv4 address.
 */

isc_result_t
isc_netaddr_prefixok(const isc_netaddr_t *na, unsigned int prefixlen);
/*
 * Test whether the netaddr 'na' and 'prefixlen' are consistent.
 * e.g. prefixlen within range.
 *      na does not have bits set which are not covered by the prefixlen.
 *
 * Returns:
 *	ISC_R_SUCCESS
 *	ISC_R_RANGE		prefixlen out of range
 *	ISC_R_NOTIMPLEMENTED	unsupported family
 *	ISC_R_FAILURE		extra bits.
 */

ISC_LANG_ENDDECLS

#endif /* GUARD_ISC_NETADDR_H */
