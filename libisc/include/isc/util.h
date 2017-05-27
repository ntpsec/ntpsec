/*
 * Copyright (C) 2004-2007, 2010-2012  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1998-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_UTIL_H
#define GUARD_ISC_UTIL_H 1

/*! \file isc/util.h
 * NOTE:
 *
 * This file is not to be included from any <isc/???.h> (or other) library
 * files.
 *
 * \brief
 * Including this file puts several macros in your name space that are
 * not protected (as all the other ISC functions/macros do) by prepending
 * ISC_ or isc_ to the name.
 */

/***
 *** General Macros.
 ***/

/*%
 * Use this to hide unused function arguments.
 * \code
 * int
 * foo(char *bar)
 * {
 *	UNUSED(bar);
 * }
 * \endcode
 */
#define UNUSED(x)      (void)(x)

/*%
 * We use macros instead of calling the routines directly because
 * the capital letters make the locking stand out.
 * We RUNTIME_CHECK for success since in general there's no way
 * for us to continue if they fail.
 */

#ifdef ISC_UTIL_TRACEON
#define ISC_UTIL_TRACE(a) a
#include <stdio.h>		/* Required for fprintf/stderr when tracing. */
#else
#define ISC_UTIL_TRACE(a)
#endif

#include "isc/result.h"		/* Contractual promise. */

/*
 * List Macros.
 */
#include "isc/list.h"		/* Contractual promise. */


/*
 * Assertions
 */
#include "isc/assertions.h"	/* Contractual promise. */

/*% Require Assertion */
#define REQUIRE(e)			ISC_REQUIRE(e)
/*% Ensure Assertion */
#define ENSURE(e)			ISC_ENSURE(e)
/*% Insist Assertion */
#define INSIST(e)			ISC_INSIST(e)
/*% Invariant Assertion */
#define INVARIANT(e)			ISC_INVARIANT(e)

/*
 * Errors
 */
#include "isc/error.h"		/* Contractual promise. */

/*% Unexpected Error */
#define UNEXPECTED_ERROR		isc_error_unexpected
/*% Runtime Check */
#define RUNTIME_CHECK(cond)		ISC_ERROR_RUNTIMECHECK(cond)

/* hack to ignore GCC Unused Result */
#define ISC_IGNORE(r) do{if(r){}}while(0)

/*%
 * Time
 */
#define TIME_NOW(tp) 	RUNTIME_CHECK(isc_time_now((tp)) == ISC_R_SUCCESS)

#endif /* GUARD_ISC_UTIL_H */
