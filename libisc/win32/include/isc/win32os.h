/*
 * Copyright (C) 2004, 2007, 2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 2002  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_WIN32OS_H
#define GUARD_ISC_WIN32OS_H 1

#include <isc/lang.h>

ISC_LANG_BEGINDECLS

/*
 * Return the number of CPUs available on the system, or 1 if this cannot
 * be determined.
 */

unsigned int
isc_win32os_majorversion(void);
/*
 * Major Version of the O/S.
 */

unsigned int
isc_win32os_minorversion(void);
/*
 * Minor Version of the O/S.
 */

unsigned int
isc_win32os_servicepackmajor(void);
/*
 * Major Version of the Service Pack for O/S.
 */

unsigned int
isc_win32os_servicepackminor(void);
/*
 * Minor Version of the Service Pack for O/S.
 */

int
isc_win32os_versioncheck(unsigned int major, unsigned int minor,
		     unsigned int updatemajor, unsigned int updateminor);

/*
 * Checks the current version of the operating system with the
 * supplied version information.
 * Returns:
 * -1	if less than the version information supplied
 *  0   if equal to all of the version information supplied
 * +1   if greater than the version information supplied
 */

ISC_LANG_ENDDECLS

#endif /* GUARD_ISC_WIN32OS_H */
