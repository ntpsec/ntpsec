/*
 * Copyright (C) 2004, 2005, 2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1998-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

/*! \file */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "ntp_config.h"
#include "ntp_syslog.h"

#include "isc/error.h"

/*% Default unexpected callback. */
static void
default_unexpected_callback(const char *, int, const char *, va_list)
     ISC_FORMAT_PRINTF(3, 0);

/*% Default fatal callback. */
static void	library_fatal_error	(const char *, int,
					 const char *, va_list)
					ISC_FORMAT_PRINTF(3, 0)
			__attribute__	((__noreturn__));

/*% unexpected_callback */
static isc_errorcallback_t unexpected_callback ISC_FORMAT_PRINTF(3, 0) \
	 = default_unexpected_callback;
static isc_errorcallback_t fatal_callback ISC_FORMAT_PRINTF(3, 0) \
	 = library_fatal_error;

void
isc_error_setunexpected(isc_errorcallback_t cb) {
	if (cb == NULL)
		unexpected_callback = default_unexpected_callback;
	else
		unexpected_callback = cb;
}

void
isc_error_unexpected(const char *file, int line, const char *format, ...) {
	va_list args;

	va_start(args, format);
	(unexpected_callback)(file, line, format, args);
	va_end(args);
}

void
isc_error_fatal(const char *file, int line, const char *format, ...) {
	va_list args;

	va_start(args, format);
	(fatal_callback)(file, line, format, args);
	va_end(args);
	abort();
}

void
isc_error_runtimecheck(const char *file, int line, const char *expression) {
	isc_error_fatal(file, line, "RUNTIME_CHECK(%s) failed", expression);
}

static void
default_unexpected_callback(const char *file, int line, const char *format,
			    va_list args)
{
	fprintf(stderr, "%s:%d: ", file, line);
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
	fflush(stderr);
}
/*
 * library_fatal_error - Handle fatal errors from our libraries.
 */
static void
library_fatal_error(
	const char *file,
	int line,
	const char *format,
	va_list args
	)
{
	char errbuf[256];
        static recurse = 0;

        /* Avoid recursion */
        if ( recurse++ ) abort();

	msyslog(LOG_ERR, "%s:%d: fatal error:", file, line);
	vsnprintf(errbuf, sizeof(errbuf), format, args);
	msyslog(LOG_ERR, "%s", errbuf);
	msyslog(LOG_ERR, "exiting (due to fatal error in library)");

	abort();
}
