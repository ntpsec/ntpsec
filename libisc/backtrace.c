/*
 * Copyright (C) 2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

/*! \file */

#include "config.h"

#include <string.h>
#include <stdlib.h>

#include "isc/backtrace.h"
#include "isc/result.h"
#include "isc/util.h"

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
#ifdef HAVE_BACKTRACE_SYMBOLS_FD
#define BACKTRACE_LIBC
#elif defined(HAVE__UNWIND_BACKTRACE)
#define BACKTRACE_UNWIND
#else
#define BACKTRACE_DISABLED
#endif  /* HAVE_BACKTRACE_SYMBOLS_FD */
#else	/* !USEBACKTRACE */
#define BACKTRACE_DISABLED
#endif	/* USEBACKTRACE */

#ifdef BACKTRACE_LIBC
#include <execinfo.h>

isc_result_t
isc_backtrace_gettrace(void **addrs, int maxaddrs, int *nframes) {
	int n;

	/*
	 * Validate the arguments: intentionally avoid using REQUIRE().
	 * See notes in backtrace.h.
	 */
	if (addrs == NULL || nframes == NULL)
		return (ISC_R_FAILURE);

	/*
	 * backtrace(3) includes this function itself in the address array,
	 * which should be eliminated from the returned sequence.
	 */
	n = backtrace(addrs, maxaddrs);
	if (n < 2)
		return (ISC_R_NOTFOUND);
	n--;
	memmove(addrs, &addrs[1], sizeof(void *) * (size_t)n);
	*nframes = n;
	return (ISC_R_SUCCESS);
}
#elif defined(BACKTRACE_UNWIND)
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

isc_result_t
isc_backtrace_gettrace(void **addrs, int maxaddrs, int *nframes) {
	trace_arg_t arg;

	/* Argument validation: see above. */
	if (addrs == NULL || nframes == NULL)
		return (ISC_R_FAILURE);

	arg.skip_count = 1;
	arg.result = addrs;
	arg.max_depth = maxaddrs;
	arg.count = 0;
	_Unwind_Backtrace(btcallback, &arg);

	*nframes = arg.count;

	return (ISC_R_SUCCESS);
}
#elif defined(BACKTRACE_DISABLED)
isc_result_t
isc_backtrace_gettrace(void **addrs, int maxaddrs, int *nframes) {
	/* Argument validation: see above. */
	if (addrs == NULL || nframes == NULL)
		return (ISC_R_FAILURE);

	UNUSED(maxaddrs);

	return (ISC_R_NOTIMPLEMENTED);
}
#endif

