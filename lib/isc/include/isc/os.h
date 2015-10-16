/*
 * Copyright (C) 2004-2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 2000, 2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_OS_H
#define GUARD_ISC_OS_H 1

/*! \file isc/os.h */

#include <isc/lang.h>

ISC_LANG_BEGINDECLS

unsigned int
isc_os_ncpus(void);
/*%<
 * Return the number of CPUs available on the system, or 1 if this cannot
 * be determined.
 */

ISC_LANG_ENDDECLS

#endif /* GUARD_ISC_OS_H */
