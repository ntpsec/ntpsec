/*
 * lib_strbuf.h - definitions for routines which use the common string buffers
 */
#ifndef GUARD_LIB_STRBUF_H
#define GUARD_LIB_STRBUF_H

#include "ntp_types.h"
#include "ntp_malloc.h"			/* for ZERO() */

/*
 * Sizes of things
 */
#define LIB_NUMBUF	16
#define	LIB_BUFLENGTH	128

typedef char libbufstr[LIB_BUFLENGTH];
extern libbufstr lib_stringbuf[LIB_NUMBUF];
extern int lib_nextbuf;

/*
 * Macro to get a pointer to the next buffer
 */
#define	LIB_GETBUF(bufp)					\
	do {							\
		ZERO(lib_stringbuf[lib_nextbuf]);		\
		(bufp) = &lib_stringbuf[lib_nextbuf++][0];	\
		lib_nextbuf %= COUNTOF(lib_stringbuf);		\
	} while (false)

#endif	/* GUARD_LIB_STRBUF_H */
