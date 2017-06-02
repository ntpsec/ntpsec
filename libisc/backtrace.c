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
 *    This function is available on Linux and OS X.  It is defined in the
 *    Linux Standard Base from at least version 4.1
 * 3. Otherwise, if the architecture x86 or x86_64, try to unwind the stack
 *    frame following frame pointers.  This assumes the executable binary
 *    compiled with frame pointers; this is not always true for x86_64 (rather,
 *    compiler optimizations often disable frame pointers).  The validation
 *    checks in getnextframeptr() hopefully rejects bogus values stored in
 *    the RBP register in such a case.  If the backtrace function itself crashes
 *    due to this problem, the whole package should be rebuilt with
 *    --disable-backtrace.
 */
#ifdef HAVE_BACKTRACE_SYMBOLS_FD
#define BACKTRACE_LIBC
#elif defined(HAVE__UNWIND_BACKTRACE)
#define BACKTRACE_UNWIND
#elif defined(__x86_64__) || defined(__i386__)
#define BACKTRACE_X86STACK
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
#elif defined(BACKTRACE_X86STACK)
#ifdef __x86_64__
static unsigned long
getrbp(void) {
	__asm("movq %rbp, %rax\n");
        /* much trickery with __asm()'ed return variable */
}
#endif

static void **
getnextframeptr(void **sp) {
	void **newsp = (void **)*sp;

	/*
	 * Perform sanity check for the new frame pointer, derived from
	 * google glog.  This can actually be bogus depending on compiler.
	 */

	/* prohibit the stack frames from growing downwards */
	if (newsp <= sp)
		return (NULL);

	/* A heuristics to reject "too large" frame: this actually happened. */
	if ((char *)newsp - (char *)sp > 100000)
		return (NULL);

	/*
	 * Not sure if other checks used in glog are needed at this moment.
	 * For our purposes we don't have to consider non-contiguous frames,
	 * for example.
	 */

	return (newsp);
}

isc_result_t
isc_backtrace_gettrace(void **addrs, int maxaddrs, int *nframes) {
	int i = 0;
	void **sp;

	/* Argument validation: see above. */
	if (addrs == NULL || nframes == NULL)
		return (ISC_R_FAILURE);

#ifdef __x86_64__
	sp = (void **)getrbp();
	if (sp == NULL)
		return (ISC_R_NOTFOUND);
	/*
	 * sp is the frame ptr of this function itself due to the call to
	 * getrbp(), so need to unwind one frame for consistency.
	 */
	sp = getnextframeptr(sp);
#else
	/*
	 * i386: the frame pointer is stored 2 words below the address for the
	 * first argument.  Note that the body of this function cannot be
	 * inlined since it depends on the address of the function argument.
	 */
	sp = (void **)&addrs - 2;
#endif

	while (sp != NULL && i < maxaddrs) {
		addrs[i++] = *(sp + 1);
		sp = getnextframeptr(sp);
	}

	*nframes = i;

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

