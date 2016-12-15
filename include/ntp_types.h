/*
 *  ntp_types.h - NTP basic types and macros.
 *
 * Assume C99 fixed width integer types are available: int32_t and uint32_t
 */
#ifndef GUARD_NTP_TYPES_H
#define GUARD_NTP_TYPES_H

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

/*
 * We now assume the platform supports a 64-bit scalar type (the ISC
 * library wouldn't compile otherwise).
 */

typedef uint64_t time64_t;
#define LOW32MASK	0x00000000ffffffffUL
#define HIGH32MASK	0xffffffff00000000UL
#define time64lo(n)	((uint32_t)((n) & LOW32MASK))
#define settime64lo(n, v) (n) = (((n) & HIGH32MASK) | ((v) & LOW32MASK))
#define time64s(n)        ((int64_t)(n))
#define settime64s(n,v)   (n) = ((int64_t)(v))
#define time64u(n)        (n)
#define settime64u(n,v)   (n) = (v)
#define negtime64(n)      (n = ((uint64_t)((((int64_t)(n)) * -1))))

typedef uint16_t	associd_t; /* association ID */
#define ASSOCID_MAX	USHRT_MAX
typedef uint32_t keyid_t;	/* cryptographic key ID */
#define KEYID_T_MAX	(0xffffffff)
typedef uint32_t tstamp_t;	/* NTP seconds timestamp */

/*
 * Cloning malloc()'s behavior of always returning pointers suitably
 * aligned for the strictest alignment requirement of any type is not
 * easy to do portably, as the maximum alignment required is not
 * exposed.  Use the size of a struct of the types known to represent the
 * strictest alignment on some platform. This will force the struct to 
 * have the strictest possible alignment.
 */
typedef struct max_alignment_tag {
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

typedef int SOCKET;
# define INVALID_SOCKET		(-1)
# define SOCKET_ERROR		(-1)


#endif	/* GUARD_NTP_TYPES_H */
