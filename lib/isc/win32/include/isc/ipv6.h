/*
 * Copyright (C) 2004, 2005, 2007, 2011, 2012  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2002  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_IPV6_H
#define GUARD_ISC_IPV6_H 1

/*****
 ***** Module Info
 *****/

/*
 * IPv6 definitions for systems which do not support IPv6.
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
 *	RFC2553.
 */

#if _MSC_VER < 1300
#define in6_addr in_addr6
#endif

#endif /* GUARD_ISC_IPV6_H */
