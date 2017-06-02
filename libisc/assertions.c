/*
 * Copyright (C) 2004, 2005, 2007-2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1997-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

/*! \file */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "ntp.h"
#include "ntp_debug.h"
#include "ntp_syslog.h"
#include "ntp_assert.h"
#include "isc/backtrace.h"
#include "isc/result.h"

/*%
 * Public.
 */

/*% Type to Text */
const char *
isc_assertion_typetotext(isc_assertiontype_t type) {
	const char *result;

	/*
	 * These strings have purposefully not been internationalized
	 * because they are considered to essentially be keywords of
	 * the ISC development environment.
	 */
	switch (type) {
	case isc_assertiontype_require:
		result = "REQUIRE";
		break;
	case isc_assertiontype_ensure:
		result = "ENSURE";
		break;
	case isc_assertiontype_insist:
		result = "INSIST";
		break;
	case isc_assertiontype_invariant:
		result = "INVARIANT";
		break;
	default:
		result = "(null)";
	}
	return (result);
}

/*
 * assertion_failed - Redirect assertion failed output to msyslog().
 */
void
assertion_failed(
	const char *file,
	int line,
	isc_assertiontype_t type,
	const char *cond
	)
{
        /* Is recursion an issue? */

	termlogit = true; /* insist log to terminal */

	msyslog(LOG_ERR, "%s:%d: %s(%s) failed",
		file, line, isc_assertion_typetotext(type), cond);
	msyslog(LOG_ERR, "exiting (due to assertion failure)");

	abort();
}
