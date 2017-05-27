/*
 * Copyright (C) 2004-2009, 2012  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2003  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_TYPES_H
#define GUARD_ISC_TYPES_H 1

/*! \file isc/types.h
 */
#include <stdint.h>
#include <stdbool.h>
/*
 * XXXDCL This is just for ISC_LIST and ISC_LINK, but gets all of the other
 * list macros too.
 */
#include "isc/list.h"

/* Core Types.  Alphabetized by defined type. */

typedef struct isc_backtrace_symmap	isc_backtrace_symmap_t; /*%< Symbol Table Entry */
typedef struct isc_buffer		isc_buffer_t;		/*%< Buffer */
typedef struct isc_interface		isc_interface_t;	/*%< Interface */
typedef struct isc_interfaceiter	isc_interfaceiter_t;	/*%< Interface Iterator */
typedef struct isc_log 			isc_log_t;		/*%< Log */
typedef struct isc_logcategory		isc_logcategory_t;	/*%< Log Category */
typedef struct isc_interval		isc_interval_t;		/*%< Interval */
typedef struct isc_logconfig		isc_logconfig_t;	/*%< Log Configuration */
typedef struct isc_logmodule		isc_logmodule_t;	/*%< Log Module */
typedef struct isc_mem			isc_mem_t;		/*%< Memory */
typedef struct isc_msgcat		isc_msgcat_t;		/*%< Message Catalog */
typedef struct isc_netaddr		isc_netaddr_t;		/*%< Net Address */
typedef unsigned int			isc_result_t;		/*%< Result */
typedef struct isc_sockaddr		isc_sockaddr_t;		/*%< Socket Address */
typedef struct isc_socket		isc_socket_t;		/*%< Socket */
typedef struct isc_time			isc_time_t;		/*%< Time */

#endif /* GUARD_ISC_TYPES_H */
