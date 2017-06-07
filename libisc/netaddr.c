/*
 * Copyright (C) 2004, 2005, 2007, 2010-2012  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2002  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

/*! \file */

#include "config.h"

#include <stdio.h>

#include "isc/netaddr.h"
#include "isc/sockaddr.h"
#include "ntp_assert.h"
#include "ntp_stdlib.h"		/* NTP change for strlcpy, strlcat */

#define ISC_TF(x) ((x) ? true : false)

bool
isc_netaddr_eqprefix(const isc_netaddr_t *a, const isc_netaddr_t *b,
		     unsigned int prefixlen)
{
	const unsigned char *pa = NULL, *pb = NULL;
	unsigned int ipabytes = 0; /* Length of whole IP address in bytes */
	unsigned int nbytes;       /* Number of significant whole bytes */
	unsigned int nbits;        /* Number of significant leftover bits */

	REQUIRE(a != NULL && b != NULL);

	if (a->family != b->family)
		return (false);

	if (a->zone != b->zone && b->zone != 0)
		return (false);

	switch (a->family) {
	case AF_INET:
		pa = (const unsigned char *) &a->type.in;
		pb = (const unsigned char *) &b->type.in;
		ipabytes = 4;
		break;
	case AF_INET6:
		pa = (const unsigned char *) &a->type.in6;
		pb = (const unsigned char *) &b->type.in6;
		ipabytes = 16;
		break;
	default:
		return (false);
	}

	/*
	 * Don't crash if we get a pattern like 10.0.0.1/9999999.
	 */
	if (prefixlen > ipabytes * 8)
		prefixlen = ipabytes * 8;

	nbytes = prefixlen / 8;
	nbits = prefixlen % 8;

	if (nbytes > 0) {
		if (memcmp(pa, pb, nbytes) != 0)
			return (false);
	}
	if (nbits > 0) {
		unsigned int bytea, byteb, mask;
		INSIST(nbytes < ipabytes);
		INSIST(nbits < 8);
		bytea = pa[nbytes];
		byteb = pb[nbytes];
		mask = (0xFF << (8-nbits)) & 0xFF;
		if ((bytea & mask) != (byteb & mask))
			return (false);
	}
	return (true);
}

void
isc_netaddr_fromin(isc_netaddr_t *netaddr, const struct in_addr *ina) {
	memset(netaddr, 0, sizeof(*netaddr));
	netaddr->family = AF_INET;
	netaddr->type.in = *ina;
}

void
isc_netaddr_fromin6(isc_netaddr_t *netaddr, const struct in6_addr *ina6) {
	memset(netaddr, 0, sizeof(*netaddr));
	netaddr->family = AF_INET6;
	netaddr->type.in6 = *ina6;
}

void
isc_netaddr_setzone(isc_netaddr_t *netaddr, uint32_t zone) {
	/* we currently only support AF_INET6. */
	REQUIRE(netaddr->family == AF_INET6);

	netaddr->zone = zone;
}

bool
isc_netaddr_islinklocal(isc_netaddr_t *na) {
	switch (na->family) {
	case AF_INET:
		return (false);
	case AF_INET6:
		return (ISC_TF(IN6_IS_ADDR_LINKLOCAL(&na->type.in6)));
	default:
		return (false);
	}
}

