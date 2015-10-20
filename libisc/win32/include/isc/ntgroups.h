/*
 * Copyright (C) 2004, 2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 2001  Internet Software Consortium.
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_NTGROUPS_H
#define GUARD_ISC_NTGROUPS_H 1

#include <isc/lang.h>
#include <isc/result.h>

ISC_LANG_BEGINDECLS


isc_result_t
isc_ntsecurity_getaccountgroups(char *name, char **Groups, unsigned int maxgroups,
	     unsigned int *total);

ISC_LANG_ENDDECLS

#endif /* GUARD_ISC_NTGROUPS_H */
