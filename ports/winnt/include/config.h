/* config.h for Windows NT */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * IPv6 requirements
 */
/*
 * For VS.NET most of the IPv6 types and structures are defined
 */
#if _MSC_VER > 1200
#define HAVE_STRUCT_SOCKADDR_STORAGE
#define ISC_PLATFORM_HAVEIPV6
#define ISC_PLATFORM_HAVEIN6PKTINFO
#endif

#define ISC_PLATFORM_NEEDIN6ADDRANY
#define HAVE_SOCKADDR_IN6

/*
 * An attempt to cut down the number of warnings generated during compilation.
 * All of these should be benign to disable.
 */

#pragma warning(disable: 4100) /* unreferenced formal parameter */
#pragma warning(disable: 4101) /* unreferenced local variable */
#pragma warning(disable : 4127)

/*
 * Windows NT Configuration Values
 */
#if defined _DEBUG /* Use VC standard macro definitions */
# define DEBUG 1
#endif
#if !defined _WIN32_WINNT || _WIN32_WINNT < 0x0400
# error Please define _WIN32_WINNT in the project settings/makefile
#endif
/*
 * ANSI C compliance enabled
 */
#define __STDC__ 1
/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Prevent inclusion of winsock.h in windows.h */
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_  
#endif

#ifndef __RPCASYNC_H__
#define __RPCASYNC_H__
#endif

/*
 * VS.NET's version of wspiapi.h has a bug in it
 * where it assigns a value to a variable inside
 * an if statement. It should be comparing them.
 * We prevent inclusion since we are not using this
 * code so we don't have to see the warning messages
 */
#ifndef _WSPIAPI_H_
#define _WSPIAPI_H_
#endif

#define OPEN_BCAST_SOCKET	1 /* for	ntp_io.c */ 													
#define HAVE_RANDOM 
#define MAXHOSTNAMELEN 64
#define AUTOKEY

/* Enable OpenSSL */
#define OPENSSL 1

/*
 * Miscellaneous functions that Microsoft maps
 * to other names
 */
#define inline __inline
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#define stricmp _stricmp
#define strcasecmp _stricmp
#define isascii __isascii
#define finite _finite
#define random      rand
#define srandom     srand

/*
 * We need to include string.h first before we override strerror
 * otherwise we can get errors during the build
 */
#include <string.h>
/* Point to a local version for error string handling */
# define strerror	NTstrerror

char *NTstrerror(int errnum);

int NT_set_process_priority(void);	/* Define this function */

# define MCAST				/* Enable Multicast Support */
# define MULTICAST_NONEWSOCKET		/* Don't create a new socket for mcast address */

# define REFCLOCK			/* from ntpd.mak */

# define CLOCK_LOCAL			/* from ntpd.mak */
//# define CLOCK_PARSE 
/* # define CLOCK_ATOM */
/* # define CLOCK_SHM	*/		 /* from ntpd.mak */
# define CLOCK_HOPF_SERIAL	/* device 38, hopf DCF77/GPS serial line receiver  */
# define CLOCK_HOPF_PCI		/* device 39, hopf DCF77/GPS PCI-Bus receiver  */
# define CLOCK_NMEA
# define CLOCK_PALISADE		/* from ntpd.mak */
# define CLOCK_DUMBCLOCK
# define CLOCK_TRIMBLEDC
# define CLOCK_TRIMTSIP 1
# define CLOCK_JUPITER

# define NTP_LITTLE_ENDIAN		/* from libntp.mak */
# define NTP_POSIX_SOURCE

# define SYSLOG_FILE			/* from libntp.mak */
# define SYSV_TIMEOFDAY 		/* for ntp_unixtime.h */

# define SIZEOF_SIGNED_CHAR 1
# define SIZEOF_TIME_T 4
# define SIZEOF_INT 4			/* for ntp_types.h */

//# define HAVE_NET_IF_H
# define QSORT_USES_VOID_P
# define HAVE_SETVBUF
# define HAVE_VSPRINTF
# define HAVE_SNPRINTF
# define HAVE_VSNPRINTF
# define HAVE_PROTOTYPES		/* from ntpq.mak */
# define HAVE_MEMMOVE
# define HAVE_TERMIOS_H
# define HAVE_ERRNO_H
# define HAVE_STDARG_H
# define HAVE_NO_NICE
# define HAVE_MKTIME
# define TIME_WITH_SYS_TIME
# define HAVE_IO_COMPLETION_PORT
# define ISC_PLATFORM_NEEDNTOP
# define ISC_PLATFORM_NEEDPTON

# define NEED_S_CHAR_TYPEDEF

# define USE_PROTOTYPES 		/* for ntp_types.h */														

#define ULONG_CONST(a) a ## UL

# define NOKMEM
# define RETSIGTYPE void
# ifndef STR_SYSTEM
#  define STR_SYSTEM "WINDOWS/NT"
# endif
#define  SIOCGIFFLAGS SIO_GET_INTERFACE_LIST /* used in ntp_io.c */

/* Include Windows headers */
#include <windows.h>
#include <winsock2.h>

#endif /* __config */
