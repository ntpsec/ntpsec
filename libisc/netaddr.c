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

