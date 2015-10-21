/*
 * Copyright (C) 2004-2007, 2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_RANDOM_H
#define GUARD_ISC_RANDOM_H 1

#include <isc/lang.h>
#include <isc/types.h>

/*! \file isc/random.h
 * \brief Implements a random state pool which will let the caller return a
 * series of possibly non-reproducible random values.
 *
 * Note that the
 * strength of these numbers is not all that high, and should not be
 * used in cryptography functions.  It is useful for jittering values
 * a bit here and there, such as timeouts, etc.
 */

ISC_LANG_BEGINDECLS

void
isc_random_seed(uint32_t seed);
/*%<
 * Set the initial seed of the random state.
 */

void
isc_random_get(uint32_t *val);
/*%<
 * Get a random value.
 *
 * Requires:
 *	val != NULL.
 */

uint32_t
isc_random_jitter(uint32_t max, uint32_t jitter);
/*%<
 * Get a random value between (max - jitter) and (max).
 * This is useful for jittering timer values.
 */

ISC_LANG_ENDDECLS

#endif /* GUARD_ISC_RANDOM_H */
