/*
 *  ntp_types.h - defines how int32 and u_int32 are treated.
 *  For 64 bit systems like the DEC Alpha, they have to be defined
 *  as int and u_int.
 *  For 32 bit systems, define them as long and u_long
 */
#ifndef NTP_TYPES_H
#define NTP_TYPES_H

#include <sys/types.h>
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif

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
#define UNUSED_ARG(arg)	((void)(arg))
#endif

/*
 * COUNTOF(array) - size of array in elements
 */
#define COUNTOF(arr)	(sizeof(arr) / sizeof((arr)[0]))

/*
 * VMS DECC (v4.1), {u_char,u_short,u_long} are only in SOCKET.H,
 *			and u_int isn't defined anywhere
 */
#if defined(VMS)
#include <socket.h>
typedef unsigned int u_int;
#endif /* VMS */

#ifdef HAVE_UINT32_T
# ifndef HAVE_INT32
   typedef	int32_t		int32;
# endif
# ifndef HAVE_U_INT32
   typedef	uint32_t	u_int32;
# endif
#elif (SIZEOF_INT == 4)
# if !defined(HAVE_INT32) && !defined(int32)
   typedef	int		int32;
# endif
# if !defined(HAVE_U_INT32) && !defined(u_int32)
   typedef	unsigned	u_int32;
# endif
#else	/* SIZEOF_INT != 4 */
# if (SIZEOF_LONG == 4)
# if !defined(HAVE_INT32) && !defined(int32)
    typedef	long		int32;
#  endif
# if !defined(HAVE_U_INT32) && !defined(u_int32)
    typedef	unsigned long	u_int32;
#  endif
# else	/* SIZEOF_LONG != 4 */
#  include "Bletch: what's 32 bits on this machine?"
# endif
#endif	/* !HAVE_UINT32_T && SIZEOF_INT != 4 */

typedef u_char		ntp_u_int8_t;
typedef u_short		ntp_u_int16_t;
typedef u_int32		ntp_u_int32_t;

typedef struct ntp_uint64_t { u_int32 val[2]; } ntp_uint64_t;

typedef u_short		associd_t; /* association ID */
#define ASSOCID_MAX	USHRT_MAX
typedef u_int32 keyid_t;	/* cryptographic key ID */
#define KEYID_T_MAX	(0xffffffff)
typedef u_int32 tstamp_t;	/* NTP seconds timestamp */

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
 */
#ifndef SYS_WINNT
#define tty_open(f, a, m)	open(f, a, m)
#endif


#endif	/* NTP_TYPES_H */
