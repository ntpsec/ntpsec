/*
 * Copyright (C) 2004-2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_MAGIC_H
#define GUARD_ISC_MAGIC_H 1

/*! \file isc/magic.h */

typedef struct {
	unsigned int magic;
} isc__magic_t;


/*%
 * To use this macro the magic number MUST be the first thing in the
 * structure, and MUST be of type "unsigned int".
 * The intent of this is to allow magic numbers to be checked even though
 * the object is otherwise opaque.
 */
#define ISC_MAGIC_VALID(a,b)	(((a) != NULL) && \
				 (((const isc__magic_t *)(a))->magic == (b)))

#define ISC_MAGIC(a, b, c, d)	((a) << 24 | (b) << 16 | (c) << 8 | (d))

#endif /* GUARD_ISC_MAGIC_H */
