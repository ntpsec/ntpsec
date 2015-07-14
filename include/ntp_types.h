/*
 *  ntp_types.h - NTP basic types and macros.
 *
 * Assume C99 fixed width integer types are available: int32_t and uint32_t
 */
#ifndef NTP_TYPES_H
#define NTP_TYPES_H

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>

#include <limits.h>
#include "ntp_machine.h"


/*
 * This is another naming conflict.
 * On NetBSD for MAC the macro "mac" is defined as 1
 * this is fun for us as a packet structure contains an
 * optional "mac" member - severe confusion results 8-)
 * As we hopefully do not have to rely on that macro we
 * just undefine that.
 */
#ifdef mac
#undef mac
#endif

/*
 * used to quiet compiler warnings
 */
#ifndef UNUSED_ARG
#define UNUSED_ARG(arg)		((void)(arg))
#endif
#ifndef UNUSED_LOCAL
#define UNUSED_LOCAL(arg)	((void)(arg))
#endif

/*
 * COUNTOF(array) - size of array in elements
 */
#define COUNTOF(arr)	(sizeof(arr) / sizeof((arr)[0]))

#if SIZEOF_SHORT != 2
# error short is not 2 bytes -- what is 16 bit integer on this target?
#endif

/*
 * If the platform supports a 64-bit scalar, ANSI requires the types
 * int64_t and uint64_t to exist and INT64_MAX to be defined.  Thus,
 * here and later in the code we use #ifdef INT64_MAX to conditionalize
 * code that requires that scalar.
 *
 * Even on 32-bit machines we need a representation with 64 or
 * more bits. If a scalar of that size is not available, we need a struct
 * that holds the value in split representation.
 *
 * To ease the usage a bit, we alwys use a union that is in processor
 * byte order and might or might not contain a 64bit scalar.
 */

typedef union {
#   ifdef WORDS_BIGENDIAN
	struct {
		int16_t	hh; uint16_t hl; uint16_t lh; uint16_t ll;
	} w_s;
	struct {
		uint16_t hh; uint16_t hl; uint16_t lh; uint16_t ll;
	} W_s;
	struct {
		  int32_t hi; uint32_t lo;
	} d_s;
	struct {
		uint32_t hi; uint32_t lo;
	} D_s;
#   else
	struct {
		uint16_t ll; uint16_t lh; uint16_t hl;   int16_t hh;
	} w_s;
	struct {
		uint16_t ll; uint16_t lh; uint16_t hl; uint16_t hh;
	} W_s;
	struct {
		uint32_t lo;   int32_t hi;
	} d_s;
	struct {
		uint32_t lo; uint32_t hi;
	} D_s;
#   endif

#   ifdef INT64_MAX
	int64_t	q_s;	/*   signed quad scalar */
	uint64_t Q_s;	/* unsigned quad scalar */
#   endif
} vint64; /* variant int 64 */


typedef uint16_t	associd_t; /* association ID */
#define ASSOCID_MAX	USHRT_MAX
typedef uint32_t keyid_t;	/* cryptographic key ID */
#define KEYID_T_MAX	(0xffffffff)
typedef uint32_t tstamp_t;	/* NTP seconds timestamp */

/*
 * Cloning malloc()'s behavior of always returning pointers suitably
 * aligned for the strictest alignment requirement of any type is not
 * easy to do portably, as the maximum alignment required is not
 * exposed.  Use the size of a union of the types known to represent the
 * strictest alignment on some platform.
 */
typedef union max_alignment_tag {
	double		d;
} max_alignment;

#define MAXALIGN		sizeof(max_alignment)
#define ALIGN_UNITS(sz)		(((sz) + MAXALIGN - 1) / MAXALIGN)
#define ALIGNED_SIZE(sz)	(MAXALIGN * ALIGN_UNITS(sz))
#define INC_ALIGNED_PTR(b, m)	((void *)aligned_ptr((void *)(b), m))

static inline
max_alignment *
aligned_ptr(
	max_alignment *	base,
	size_t		minsize
	)
{
	return base + ALIGN_UNITS((minsize < 1) ? 1 : minsize);
}

/*
 * Macro to use in otherwise-empty source files to comply with ANSI C
 * requirement that each translation unit (source file) contain some
 * declaration.  This has commonly been done by declaring an unused
 * global variable of type int or char.  An extern reference to exit()
 * serves the same purpose without bloat.
 */
#define	NONEMPTY_TRANSLATION_UNIT	extern void exit(int);

/*
 * On Unix struct sock_timeval is equivalent to struct timeval.
 * On Windows built with 64-bit time_t, sock_timeval.tv_sec is a long
 * as required by Windows' socket() interface timeout argument, while
 * timeval.tv_sec is time_t for the more common use as a UTC time 
 * within NTP.
 */
#ifndef SYS_WINNT
#define	sock_timeval	timeval
#endif

/*
 * On Unix open() works for tty (serial) devices just fine, while on
 * Windows refclock serial devices are opened using CreateFile, a lower
 * level than the CRT-provided descriptors, because the C runtime lacks
 * tty APIs.  For refclocks which wish to use open() as well as or 
 * instead of refclock_open(), tty_open() is equivalent to open() on
 * Unix and  implemented in the Windows port similarly to
 * refclock_open().
 * Similarly, the termios emulation in the Windows code needs to know
 * about serial ports being closed, while the Posix systems do not.
 */
#ifndef SYS_WINNT
# define tty_open(f, a, m)	open(f, a, m)
# define closeserial(fd)	close(fd)
# define closesocket(fd)	close(fd)
typedef int SOCKET;
# define INVALID_SOCKET		(-1)
# define SOCKET_ERROR		(-1)
# define socket_errno()		(errno)
#else	/* SYS_WINNT follows */
# define socket_errno()		(errno = WSAGetLastError())
#endif



#endif	/* NTP_TYPES_H */
