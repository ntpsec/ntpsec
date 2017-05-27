/*
 * Copyright (C) 2004, 2006, 2007, 2011, 2012  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1997-2002  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_LIST_H
#define GUARD_ISC_LIST_H 1

#define ISC_LIST(type) struct { type *head, *tail; }

#define ISC_LINK(type) struct { type *prev, *next; }

#endif /* GUARD_ISC_LIST_H */
