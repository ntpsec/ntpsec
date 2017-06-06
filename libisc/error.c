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
static void	library_unexpected_error(const char *, int,
					 const char *, va_list)
					ISC_FORMAT_PRINTF(3, 0);

/*% Default fatal callback. */
static void	library_fatal_error	(const char *, int,
					 const char *, va_list)
					ISC_FORMAT_PRINTF(3, 0)
			__attribute__	((__noreturn__));

void
isc_error_unexpected(const char *file, int line, const char *format, ...) {
	va_list args;

	va_start(args, format);
	library_unexpected_error(file, line, format, args);
	va_end(args);
}

void
isc_error_fatal(const char *file, int line, const char *format, ...) {
	va_list args;

	va_start(args, format);
	library_fatal_error(file, line, format, args);
	va_end(args);
	abort();
}

void
isc_error_runtimecheck(const char *file, int line, const char *expression) {
	isc_error_fatal(file, line, "RUNTIME_CHECK(%s) failed", expression);
}

/*
 * library_unexpected_error - Handle non fatal errors from our libraries.
 */
#define MAX_UNEXPECTED_ERRORS 100
static void
library_unexpected_error(
	const char *file,
	int line,
	const char *format,
	va_list args
	)
{
	char errbuf[256];
	static int unexpected_error_cnt = 0;

	if (unexpected_error_cnt >= MAX_UNEXPECTED_ERRORS)
		return;	/* avoid clutter in log */

	msyslog(LOG_ERR, "%s:%d: unexpected error:", file, line);
	vsnprintf(errbuf, sizeof(errbuf), format, args);
	msyslog(LOG_ERR, "%s", errbuf);

	if (++unexpected_error_cnt == MAX_UNEXPECTED_ERRORS)
		msyslog(LOG_ERR, "Too many errors.  Shutting up.");

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
        static int recurse = 0;

        /* Avoid recursion */
        if ( recurse++ ) abort();

	msyslog(LOG_ERR, "%s:%d: fatal error:", file, line);
	vsnprintf(errbuf, sizeof(errbuf), format, args);
	msyslog(LOG_ERR, "%s", errbuf);
	msyslog(LOG_ERR, "exiting (due to fatal error in library)");

	abort();
}
