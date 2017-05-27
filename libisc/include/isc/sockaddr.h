/*
 * Copyright (C) 2004-2007, 2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1998-2003  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_SOCKADDR_H
#define GUARD_ISC_SOCKADDR_H 1

/*! \file isc/sockaddr.h */

#include "isc/lang.h"
#include "isc/net.h"
#include "isc/types.h"

struct isc_sockaddr {
	union {
		struct sockaddr		sa;
		struct sockaddr_in	sin;
		struct sockaddr_in6	sin6;
	}				type;
	unsigned int			length;		/* XXXRTH beginning? */
	ISC_LINK(struct isc_sockaddr)	link;
};

typedef ISC_LIST(struct isc_sockaddr)	isc_sockaddrlist_t;

#define ISC_SOCKADDR_CMPADDR	  0x0001	/*%< compare the address
						 *   sin_addr/sin6_addr */
#define ISC_SOCKADDR_CMPPORT 	  0x0002	/*%< compare the port
						 *   sin_port/sin6_port */
#define ISC_SOCKADDR_CMPSCOPE     0x0004	/*%< compare the scope
						 *   sin6_scope */
#define ISC_SOCKADDR_CMPSCOPEZERO 0x0008	/*%< when comparing scopes
						 *   zero scopes always match */

#endif /* GUARD_ISC_SOCKADDR_H */
