/*
 * Copyright (C) 2004-2007, 2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1998-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_ERROR_H
#define GUARD_ISC_ERROR_H 1

/* isc_error.h */

/*
 * ISC_FORMAT_PRINTF().
 *
 * fmt is the location of the format string parameter.
 * args is the location of the first argument (or 0 for no argument checking).
 *
 * Note:
 * The first parameter is 1, not 0.
 */
#ifdef __GNUC__
#define ISC_FORMAT_PRINTF(fmt, args) __attribute__((__format__(__printf__, fmt, args)))
#else
#define ISC_FORMAT_PRINTF(fmt, args)
#endif


/* unexpected error */
void
isc_error_unexpected(const char *, int, const char *, ...)
     ISC_FORMAT_PRINTF(3, 4);

/* Unexpected Error */
#define UNEXPECTED_ERROR(fmt, ...) \
	isc_error_unexpected(__FILE__, __LINE__, fmt, __VA_ARGS__)

#endif /* GUARD_ISC_ERROR_H */
