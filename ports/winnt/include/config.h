/* config.h for Windows NT */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * For newer compilers we may we want newer prototypes from Windows
 * so we target _WIN32_WINNT at WINXP, but we also want our binary to
 * run on NT 4, so newer functions are runtime linked and the linker
 * /version:0x0400 * switch is used to override the .exe file minimum
 * version. For older compilers we leave it at NT 4.0.
 */
#ifndef _WIN32_WINNT
#if _MSC_VER > 1400		/* At least VS 2005 */
#define _WIN32_WINNT 0x0501
#else				/* NT 4.0 */
#define _WIN32_WINNT 0x0400 
#endif
#endif


#define _CRT_SECURE_NO_DEPRECATE 1
/*
 * ANSI C compliance enabled
 */
#define __STDC__ 1

/*
 * Enable the debug build of MS C runtime to dump leaks
 * at exit time (currently only if run under a debugger).
 */
#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

/*
 * We need to include string.h first before we override strerror
 * otherwise we can get errors during the build
 */
#include <string.h>

/*
 * We need to include stdio.h first before we #define snprintf
 * otherwise we can get errors during the build
 */
#include <stdio.h>

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
/* #define _WSPIAPI_H_ */ /* need these wrappers for ntpd.exe to load on w2k */
#endif

/*
 * On Unix struct sock_timeval is equivalent to struct timeval.
 * On Windows built with 64-bit time_t, sock_timeval.tv_sec is a long
 * as required by Windows' socket() interface timeout argument, while
 * timeval.tv_sec is time_t for the more common use as a UTC time 
 * within NTP.
 *
 * winsock.h unconditionally defines struct timeval with long tv_sec
 * instead of time_t tv_sec.  We redirect its declaration to struct 
 * sock_timeval instead of struct timeval with a #define.
 */
#define	timeval sock_timeval

/* Include Windows headers */
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#undef timeval	/* see sock_timeval #define and comment above */

/*
 * Some definitions we are using are missing in the headers
 * shipping with VC6. However, if the SDK is installed then the 
 * SDK's headers may declare the missing types. This is at least 
 * the case in the Oct 2001 SDK. That SDK and all subsequent 
 * versions also define the symbol _W64, so we can use that one
 * to determine whether some types need to be defined, or not.
 */
#ifdef _W64
/* VC6 can include wspiapi.h only if the SDK is installed */
#include <wspiapi.h>
#endif

#undef interface
#include <process.h>
#include <time.h>		/* time_t for timeval decl */

/* ---------------------------------------------------------------------
 * Above this line are #include lines and the few #define lines
 * needed before including headers.
 */

struct timeval {
	time_t	tv_sec;
	long	tv_usec;
};

/*
 * IPv6 requirements
 */
/*
 * For VS.NET most of the IPv6 types and structures are defined.
 * This should depend on the contrents of the available headers, 
 * not on the compiler version.
 */
#if defined _MSC_VER && _MSC_VER > 1200
#define HAVE_STRUCT_SOCKADDR_STORAGE
#define ISC_PLATFORM_HAVEIPV6
#define ISC_PLATFORM_HAVEIN6PKTINFO
#define NO_OPTION_NAME_WARNINGS
#endif

#ifndef _W64
/* VC6 doesn't know about socklen_t, except if the SDK is installed */
typedef int socklen_t;
#endif

#define ISC_PLATFORM_NEEDIN6ADDRANY
#define HAVE_SOCKADDR_IN6

/*
 * The type of the socklen_t defined for getnameinfo() and getaddrinfo()
 * is int for VS compilers on Windows but the type is already declared 
 */
#define GETSOCKNAME_SOCKLEN_TYPE socklen_t

#if defined _MSC_VER && _MSC_VER < 1400
/*
 * Use 32-bit time definitions for versions prior to VS 2005
 * VS 2005 defaults to 64-bit time
 */
# define SIZEOF_TIME_T 4
#else
# define SIZEOF_TIME_T 8
#endif


/*
 * An attempt to cut down the number of warnings generated during compilation.
 * All of these should be benign to disable.
 */

#pragma warning(disable: 4100) /* unreferenced formal parameter */
#pragma warning(disable: 4101) /* unreferenced local variable */
#pragma warning(disable: 4127) /* conditional expression is constant */
#pragma warning(disable: 4996) /* more secure replacement available */

/*
 * Windows NT Configuration Values
 */
#if defined _DEBUG /* Use VC standard macro definitions */
# define DEBUG 1
#endif

#define __windows__ 1
/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

#define OPEN_BCAST_SOCKET		1	/* for ntp_io.c */
#define TYPEOF_IP_MULTICAST_LOOP	BOOL
#define SETSOCKOPT_ARG_CAST		(const char *)
#define HAVE_RANDOM 
#define MAXHOSTNAMELEN			64
#define AUTOKEY

/*
 * Multimedia timer enable
 */
#define USE_MM_TIMER

/* Enable OpenSSL */
#define OPENSSL 1

/*
 * Keywords and functions that Microsoft maps
 * to other names
 */
#define inline		__inline
#define vsnprintf	_vsnprintf
#define snprintf	_snprintf
#define stricmp		_stricmp
#define strcasecmp	_stricmp
#define isascii		__isascii
#define finite		_finite
#define random		rand
#define srandom		srand
#define fdopen		_fdopen
#define read		_read
#define open		_open
#ifndef close
#define close		_close
#endif
#define write		_write
#define strdup		_strdup
#define stat		_stat		/*struct stat from  <sys/stat.h> */
#define unlink		_unlink
/*
 * punt on fchmod on Windows
 */
#define fchmod(x,y)	{}
#define lseek		_lseek
#define pipe		_pipe
#define dup2		_dup2
/*
 * scale, unix sleep is seconds, Windows Sleep is msec
 */
#define sleep(x)	Sleep((unsigned)(x) * 1000)
#define fileno		_fileno
#define isatty		_isatty
#define mktemp		_mktemp
#define getpid		_getpid

typedef int pid_t;		/* PID is an int */
typedef int ssize_t;		/* ssize is an int */
typedef __int32 int32_t;	/* define a typedef for int32_t */
#define HAVE_INT32_T   1

/*
 * Map the stream to the file number
 */
#define STDOUT_FILENO	_fileno(stdout)
#define STDERR_FILENO	_fileno(stderr)

/* Point to a local version for error string handling */
# define strerror	NTstrerror
char *NTstrerror(int errnum);

# define MCAST				/* Enable Multicast Support */
# define MULTICAST_NONEWSOCKET		/* Don't create a new socket for mcast address */

# define REFCLOCK			/* from ntpd.mak */

# define CLOCK_LOCAL			/* from ntpd.mak */
/* # define CLOCK_PARSE  */
/* # define CLOCK_ATOM */
/* # define HAVE_TIMEPPS_H */
/* # define HAVE_PPSAPI */
/* # define CLOCK_SHM	*/		 /* from ntpd.mak */
# define CLOCK_HOPF_SERIAL	/* device 38, hopf DCF77/GPS serial line receiver  */
# define CLOCK_HOPF_PCI		/* device 39, hopf DCF77/GPS PCI-Bus receiver  */
# define CLOCK_NMEA
# define CLOCK_PALISADE		/* from ntpd.mak */
/* # define CLOCK_DUMBCLOCK */	/* refclock_dumbclock.c needs work to open COMx: */
# define CLOCK_TRIMBLEDC
# define CLOCK_TRIMTSIP 1
# define CLOCK_JUPITER

# define NTP_LITTLE_ENDIAN		/* from libntp.mak */
# define NTP_POSIX_SOURCE

# define SYSLOG_FILE			/* from libntp.mak */
# define HAVE_GETCLOCK

# define SIZEOF_SIGNED_CHAR	1
# define SIZEOF_INT		4	/* for ntp_types.h */

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
# define HAVE_VPRINTF

#define HAVE_LIMITS_H	1
#define HAVE_STRDUP	1
#define HAVE_STRCHR	1
#define HAVE_FCNTL_H	1
#define HAVE_SYS_RESOURCE_H
#define HAVE_BSD_NICE			/* emulate BSD setpriority() */

typedef char *caddr_t;

#ifndef _INTPTR_T_DEFINED
typedef long intptr_t;
#define _INTPTR_T_DEFINED
#endif
#define HAVE_INTPTR_T

#ifndef _UINTPTR_T_DEFINED
typedef unsigned long uintptr_t;
#define _UINTPTR_T_DEFINED
#endif
#define HAVE_UINTPTR_T

#if !defined( _W64 )
  /*
   * if DWORD_PTR needs to be defined then the build environment
   * is pure 32 bit Windows. Since DWORD_PTR and DWORD have 
   * the same size in 32 bit Windows we can safely define
   * a replacement.
   */
  typedef DWORD DWORD_PTR;
#endif

#define NEED_S_CHAR_TYPEDEF

#define USE_PROTOTYPES 		/* for ntp_types.h */

/* Directory separator, usually / or \ */
#define	DIR_SEP	'\\'


#define ULONG_CONST(a) a ## UL

#define NOKMEM
#define RETSIGTYPE void

#ifndef STR_SYSTEM
#define STR_SYSTEM "Windows"
#endif

#ifndef STR_PROCESSOR

#define STRINGIZE(arg)	#arg

#ifdef _M_IX86
#ifndef _M_IX86_FP
#define STR_PROCESSOR "x86"
#else
#if !_M_IX86_FP 
#define STR_PROCESSOR "x86"
#else 
#if _M_IX86_FP > 2
#define STR_PROCESSOR "x86-FP-" STRINGIZE(_M_IX86_FP)
#else
#if _M_IX86_FP == 2
#define STR_PROCESSOR "x86-SSE2"
#else
#define STR_PROCESSOR "x86-SSE"
#endif /* _M_IX86 == 2 */
#endif /* _M_IX86_FP > 2 */
#endif /* !_M_IX86_FP */
#endif /* !defined(_M_IX86_FP) */
#endif /* !defined(_M_IX86) */

#ifdef _M_IA64
#define STR_PROCESSOR "Itanium"
#endif

#ifdef _M_X64
#define STR_PROCESSOR "x64"
#endif

#endif /* !defined(STR_PROCESSOR) */

#define  SIOCGIFFLAGS SIO_GET_INTERFACE_LIST /* used in ntp_io.c */
/*
 * Below this line are includes which must happen after the bulk of
 * config.h is processed.  If you need to add another #include to this
 * file the preferred location is near the top, above the similar
 * line of hyphens.
 * ---------------------------------------------------------------------
 */

/*
 * Include standard stat information
 */
#include <isc/stat.h>

#endif /* __CONFIG_H */
