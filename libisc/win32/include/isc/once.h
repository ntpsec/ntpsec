/*
 * Copyright (C) 2004, 2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_ONCE_H
#define GUARD_ISC_ONCE_H 1

#include <isc/lang.h>
#include <isc/result.h>

ISC_LANG_BEGINDECLS

typedef struct {
	int status;
	LONG counter;
} isc_once_t;

#define ISC_ONCE_INIT_NEEDED 0
#define ISC_ONCE_INIT_DONE 1

#define ISC_ONCE_INIT { ISC_ONCE_INIT_NEEDED, 1 }

isc_result_t
isc_once_do(isc_once_t *controller, void(*function)(void));

ISC_LANG_ENDDECLS

#endif /* GUARD_ISC_ONCE_H */
