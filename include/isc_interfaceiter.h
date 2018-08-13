/*
 * Copyright (C) 2004-2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_INTERFACEITER_H
#define GUARD_ISC_INTERFACEITER_H 1

/*****
 ***** Module Info
 *****/

/* isc_interfaceiter.h
 * Iterates over the list of network interfaces.
 *
 * Interfaces whose address family is not supported are ignored and never
 * returned by the iterator.  Interfaces whose netmask, interface flags,
 * or similar cannot be obtained are also ignored, and the failure is logged.
 *
 * Standards:
 *	The API for scanning varies greatly among operating systems.
 *	This module attempts to hide the differences.
 */

/***
 *** Imports
 ***/

#include "isc_result.h"

typedef struct isc_interfaceiter isc_interfaceiter_t;  /* Interface Iterator */
typedef struct isc_mem           isc_mem_t;	       /* Memory */

/* Net Address */
typedef struct isc_netaddr {
	unsigned int family;
	union {
		struct in_addr in;
		struct in6_addr in6;
	} type;
	uint32_t zone;
} isc_netaddr_t;


/*!
 * \brief Public structure describing a network interface.
 */

typedef struct isc_interface {
	char name[32];		   /* Interface name, null-terminated. */
	unsigned int af;	   /* Address family. */
	isc_netaddr_t address;	   /* Local address. */
	isc_netaddr_t netmask;	   /* Network mask. */
	isc_netaddr_t broadcast;   /* Broadcast address. */
	isc_netaddr_t dstaddress;  /* Destination address (point-to-point only). */
	uint32_t flags;		   /* Flags; see INTERFACE flags. */
	unsigned int ifindex;	   /* Interface index for IP(V6)_MULTICAST_IF. */
} isc_interface_t;

/*@{*/
/*! Interface flags. */

#define INTERFACE_F_UP			0x00000001U
#define INTERFACE_F_POINTTOPOINT	0x00000002U
#define INTERFACE_F_LOOPBACK		0x00000004U
#define INTERFACE_F_BROADCAST		0x00000008U
#define INTERFACE_F_MULTICAST		0x00000010U
#define INTERFACE_F_PRIVACY		0x00000020U	/* RFC 4941 */
/*@}*/

/***
 *** Functions
 ***/

bool
isc_interfaceiter_create_bool(isc_mem_t *mctx, isc_interfaceiter_t **iterp);
isc_result_t
isc_interfaceiter_create(isc_mem_t *mctx, isc_interfaceiter_t **iterp);
/*!<
 * \brief Create an iterator for traversing the operating system's list
 * of network interfaces.
 *
 * Returns:
 *\li	#ISC_R_SUCCESS
 * \li	#ISC_R_NOMEMORY
 *\li	Various network-related errors
 */

bool
isc_interfaceiter_first_bool(isc_interfaceiter_t *iter);
isc_result_t
isc_interfaceiter_first(isc_interfaceiter_t *iter);
/*!<
 * \brief Position the iterator on the first interface.
 *
 * Returns:
 *\li	#ISC_R_SUCCESS		Success.
 *\li	#ISC_R_NOMORE		There are no interfaces.
 */

bool
isc_interfaceiter_current_bool(isc_interfaceiter_t *iter,
			  isc_interface_t *ifdata);
isc_result_t
isc_interfaceiter_current(isc_interfaceiter_t *iter,
			  isc_interface_t *ifdata);
/*!<
 * \brief Get information about the interface the iterator is currently
 * positioned at and store it at *ifdata.
 *
 * Requires:
 *\li 	The iterator has been successfully positioned using
 * 	isc_interface_iter_first() / isc_interface_iter_next().
 *
 * Returns:
 *\li	#ISC_R_SUCCESS		Success.
 */

bool
isc_interfaceiter_next_bool(isc_interfaceiter_t *iter);
isc_result_t
isc_interfaceiter_next(isc_interfaceiter_t *iter);
/*!<
 * \brief Position the iterator on the next interface.
 *
 * Requires:
 * \li	The iterator has been successfully positioned using
 * 	isc_interface_iter_first() / isc_interface_iter_next().
 *
 * Returns:
 *\li	#ISC_R_SUCCESS		Success.
 *\li	#ISC_R_NOMORE		There are no more interfaces.
 */

void
isc_interfaceiter_destroy(isc_interfaceiter_t **iterp);
/*!<
 * \brief Destroy the iterator.
 */

#endif /* GUARD_ISC_INTERFACEITER_H */
