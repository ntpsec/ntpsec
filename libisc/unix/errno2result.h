/*
 * Copyright (C) 2004, 2005, 2007, 2011, 2012  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 2000, 2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_UNIX_ERRNO2RESULT_H
#define GUARD_UNIX_ERRNO2RESULT_H 1

/*! \file */

/* XXXDCL this should be moved to lib/isc/include/isc/errno2result.h. */

#include <errno.h>		/* Provides errno. */

#include <isc/lang.h>
#include <isc/types.h>

ISC_LANG_BEGINDECLS

#define isc__errno2result(x) isc___errno2result(x, __FILE__, __LINE__)

isc_result_t
isc___errno2result(int posixerrno, const char *file, unsigned int line);

ISC_LANG_ENDDECLS

#endif /* GUARD_UNIX_ERRNO2RESULT_H */
