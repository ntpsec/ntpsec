/*
 * Copyright (C) 2004, 2005, 2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_SYSLOG_H
#define GUARD_ISC_SYSLOG_H 1

/*! \file */

#include "isc/lang.h"
#include "isc/types.h"

ISC_LANG_BEGINDECLS

isc_result_t
isc_syslog_facilityfromstring(const char *str, int *facilityp);
/*%<
 * Convert 'str' to the appropriate syslog facility constant.
 *
 * Requires:
 *
 *\li	'str' is not NULL
 *\li	'facilityp' is not NULL
 *
 * Returns:
 * \li	#ISC_R_SUCCESS
 * \li	#ISC_R_NOTFOUND
 */

ISC_LANG_ENDDECLS

#endif /* GUARD_ISC_SYSLOG_H */
