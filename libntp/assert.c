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
#include "isc_result.h"

#ifdef USEBACKTRACE

/*
 * Getting a back trace of a running process is tricky and highly platform
 * dependent.  Our current approach is as follows:
 * 1. If the system library supports the "backtrace()" function, use it.
 *    OS X support this starting at with SDK 10.5.  glibc since version 2.1
 * 2. Otherwise, if unwind.h exists then use the __Unwind_Backtrace() function.
 *    This function is available on Linux, OS X, and FreeBSD.  It is defined
 *    in Linux Standard Base since version 4.1
 * 3. Otherwise, tough luck.
 */

/*
 * The maximum number of stack frames to dump on assertion failure.
 */
#ifndef BACKTRACE_MAXFRAME
#define BACKTRACE_MAXFRAME 128
#endif


# ifdef HAVE_BACKTRACE_SYMBOLS_FD
#include <execinfo.h>

void
backtrace_log(void) {
	int nptrs;
	void *buffer[BACKTRACE_MAXFRAME];
	char **strings;

	nptrs = backtrace(buffer, BACKTRACE_MAXFRAME);
	strings = backtrace_symbols(buffer, nptrs);
	msyslog(LOG_ERR, "ERR: Stack trace:\n");
	if (strings) {
	   /* skip trace of this shim function */
	   for (int j = 1; j < nptrs; j++)
	       msyslog(LOG_ERR, "ERR:  %s\n", strings[j]);

	   free(strings);
	}
}
# elif defined(HAVE__UNWIND_BACKTRACE)
#include <unwind.h>

typedef struct {
	void **result;
	int max_depth;
	int skip_count;
	int count;
} trace_arg_t;

static _Unwind_Reason_Code
btcallback(struct _Unwind_Context *uc, void *opq) {
	trace_arg_t *arg = (trace_arg_t *)opq;

	if (arg->skip_count > 0)
		arg->skip_count--;
	else
		arg->result[arg->count++] = (void *)_Unwind_GetIP(uc);
	if (arg->count == arg->max_depth)
		return (5); /* _URC_END_OF_STACK */

	return (0); /* _URC_NO_REASON */
}


void
backtrace_log(void) {
	trace_arg_t arg;
	void *buffer[BACKTRACE_MAXFRAME];
	int i;

	arg.skip_count = 1;
	arg.result = buffer;
	arg.max_depth = BACKTRACE_MAXFRAME;
	arg.count = 0;
	_Unwind_Backtrace(btcallback, &arg);

	msyslog(LOG_ERR, "ERR: Stack trace:\n");
	/* skip trace of this shim function */
	for (i = 1; i < arg.count; i++) {
	    msyslog(LOG_ERR, "ERR: #%d %p in ??\n", i, buffer[i]);
	}

}
#endif /* HAVE__UNWIND_BACKTRACE */

#endif /* USEBACKTRACE */

/*% Type to Text */
static const char *
assertion_typetotext(assertiontype_t type) {
	const char *result;

	/*
	 * These strings have purposefully not been internationalized
	 * because they are considered to essentially be keywords of
	 * the ISC development environment.
	 */
	switch (type) {
	case assertiontype_require:
		result = "REQUIRE";
		break;
	case assertiontype_ensure:
		result = "ENSURE";
		break;
	case assertiontype_insist:
		result = "INSIST";
		break;
	case assertiontype_invariant:
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
	assertiontype_t type,
	const char *cond
	)
{
        /* Is recursion an issue? */

	termlogit = true; /* insist log to terminal */

	msyslog(LOG_ERR, "ERR: %s:%d: %s(%s) failed",
		file, line, assertion_typetotext(type), cond);
#ifndef BACKTRACE_DISABLED
        backtrace_log();
#endif
	msyslog(LOG_ERR, "ERR: exiting (due to assertion failure)");

	abort();
}
