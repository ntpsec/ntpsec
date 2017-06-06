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
 */

#ifdef ISC_UTIL_TRACEON
#define ISC_UTIL_TRACE(a) a
#include <stdio.h>		/* Required for fprintf/stderr when tracing. */
#else
#define ISC_UTIL_TRACE(a)
#endif

#include "isc/result.h"		/* Contractual promise. */

/*
 * Assertions
 */
#include "ntp_assert.h"	/* Contractual promise. */

/*
 * Errors
 */
#include "isc/error.h"		/* Contractual promise. */

/*% Unexpected Error */
#define UNEXPECTED_ERROR		isc_error_unexpected

/* hack to ignore GCC Unused Result */
#define ISC_IGNORE(r) do{if(r){}}while(0)

#endif /* GUARD_ISC_UTIL_H */
