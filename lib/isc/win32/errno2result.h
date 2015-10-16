/*
 * Copyright (C) 2004, 2005, 2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 2000, 2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_UNIX_ERRNO2RESULT_H
#define GUARD_UNIX_ERRNO2RESULT_H 1

/* XXXDCL this should be moved to lib/isc/include/isc/errno2result.h. */

#include <errno.h>		/* Provides errno. */

#include <isc/lang.h>
#include <isc/types.h>

ISC_LANG_BEGINDECLS

#define isc__errno2result(posixerrno) \
	isc__errno2resultx(posixerrno, __FILE__, __LINE__)

isc_result_t
isc__errno2resultx(int posixerrno, const char *file, int line);

ISC_LANG_ENDDECLS

#endif /* GUARD_UNIX_ERRNO2RESULT_H */
