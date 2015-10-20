/*
 * Copyright (C) 2004-2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 2000, 2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_FORMATCHECK_H
#define GUARD_ISC_FORMATCHECK_H 1

/*! \file isc/formatcheck.h */

/*%
 * ISC_FORMAT_PRINTF().
 *
 * \li fmt is the location of the format string parameter.
 * \li args is the location of the first argument (or 0 for no argument checking).
 * 
 * Note:
 * \li The first parameter is 1, not 0.
 */
#ifdef __GNUC__
#define ISC_FORMAT_PRINTF(fmt, args) __attribute__((__format__(__printf__, fmt, args)))
#else
#define ISC_FORMAT_PRINTF(fmt, args)
#endif

#endif /* GUARD_ISC_FORMATCHECK_H */
