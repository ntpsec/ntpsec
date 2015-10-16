/*
 * Copyright (C) 2004-2007, 2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_LIB_H
#define GUARD_ISC_LIB_H 1

/*! \file isc/lib.h */

#include <isc/types.h>
#include <isc/lang.h>

ISC_LANG_BEGINDECLS

void
isc_lib_register(void);
/*!<
 * \brief Register the ISC library implementations for some base services
 * such as memory or event management and handling socket or timer events.
 * An external application that wants to use the ISC library must call this
 * function very early in main().
 */

ISC_LANG_ENDDECLS

#endif /* GUARD_ISC_LIB_H */
