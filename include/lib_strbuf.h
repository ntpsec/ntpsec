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
extern char *lib_getbuf(void);

#endif	/* GUARD_LIB_STRBUF_H */
