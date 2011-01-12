/*
 * lib_strbuf.h - definitions for routines which use the common string buffers
 */

#include <ntp_types.h>
#include <ntp_malloc.h>			/* for ZERO() */

/*
 * Sizes of things
 */
#define LIB_NUMBUF	16
typedef char libbufstr[128];
extern libbufstr lib_stringbuf[LIB_NUMBUF];
extern int lib_nextbuf;
extern int lib_inited;

#define	LIB_BUFLENGTH	(sizeof(lib_stringbuf) / COUNTOF(lib_stringbuf))

/*
 * Macro to get a pointer to the next buffer
 */
#define	LIB_GETBUF(bufp)					\
	do {							\
		ZERO(lib_stringbuf[lib_nextbuf]);		\
		(bufp) = &lib_stringbuf[lib_nextbuf++][0];	\
		lib_nextbuf %= COUNTOF(lib_stringbuf);		\
	} while (FALSE)
