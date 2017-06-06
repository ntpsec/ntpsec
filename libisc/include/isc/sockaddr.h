/*
 * Copyright (C) 2004-2007, 2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1998-2003  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_SOCKADDR_H
#define GUARD_ISC_SOCKADDR_H 1

/*! \file isc/sockaddr.h */

#include "isc/netaddr.h"
#include "isc/types.h"

#define ISC_LINK(type) struct { type *prev, *next; }

struct isc_sockaddr {
	union {
		struct sockaddr		sa;
		struct sockaddr_in	sin;
		struct sockaddr_in6	sin6;
	}				type;
	unsigned int			length;		/* XXXRTH beginning? */
	ISC_LINK(struct isc_sockaddr)	link;
};

#endif /* GUARD_ISC_SOCKADDR_H */
