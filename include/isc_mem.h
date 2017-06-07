/*
 * libntp local override of isc/mem.h to stub it out.
 *
 * include/isc is searched before any of the libisc include
 * directories and should be used only for replacement NTP headers
 * overriding headers of the same name under libisc.
 *
 * NOTE: this assumes the system malloc is thread-safe and does
 *	 not use any normal libisc locking.
 *
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#ifndef GUARD_ISC_MEM_H
#define GUARD_ISC_MEM_H

#include <stdio.h>

#include "isc_result.h"

#include "ntp_stdlib.h"
#include "ntp_types.h"

#define isc_mem_allocate(c, cnt)	isc_mem_get(c, cnt)
#define isc_mem_get(c, cnt)		\
	( UNUSED_ARG(c),	emalloc(cnt) )

#define isc_mem_reallocate(c, mem, cnt)	\
	( UNUSED_ARG(c),	erealloc((mem), cnt) )

#define isc_mem_put(c, mem, cnt)	\
	( UNUSED_ARG(cnt),	isc_mem_free(c, (mem)) )

#define isc_mem_free(c, mem)		\
	( UNUSED_ARG(c),	free(mem) )

#define isc_mem_strdup(c, str)		\
	( UNUSED_ARG(c),	estrdup(str) )

#define isc__mem_attach(src, ptgt)	do { *(ptgt) = (src); } while (0)
#define isc__mem_detach(c)		UNUSED_ARG(c)
#define isc__mem_printallactive(s)	fprintf((s), \
					"isc_mem_printallactive() stubbed.\n")

#endif /* GUARD_ISC_MEM_H */
