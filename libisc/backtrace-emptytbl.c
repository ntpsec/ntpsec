/*
 * Copyright (C) 2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

/*! \file */

/*
 * This file defines an empty (default) symbol table used in backtrace.c
 * If the application wants to have a complete symbol table, it should redefine
 * isc__backtrace_symtable with the complete table in some way, and link the
 * version of the library not including this definition
 * (e.g. libisc-nosymbol.a).
 */

#include "config.h"
#include <stdio.h>

#include <isc/backtrace.h>

const int isc__backtrace_nsymbols = 0;
const isc_backtrace_symmap_t isc__backtrace_symtable[] = { { NULL, "" } };
