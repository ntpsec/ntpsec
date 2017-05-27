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

ISC_LANG_ENDDECLS

#endif /* GUARD_ISC_NETADDR_H */
