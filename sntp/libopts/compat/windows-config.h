
/**
 * \file windows-config.h
 *
 *  This file contains all of the routines that must be linked into
 *  an executable to use the generated option processing.  The optional
 *  routines are in separately compiled modules so that they will not
 *  necessarily be linked in.
 *
 *  This file is part of AutoOpts, a companion to AutoGen.
 *  AutoOpts is free software.
 *  AutoOpts is Copyright (C) 1992-2015 by Bruce Korb - all rights reserved
 *
 *  AutoOpts is available under any one of two licenses.  The license
 *  in use must be one of these two and the choice is under the control
 *  of the user of the license.
 *
 *   The GNU Lesser General Public License, version 3 or later
 *      See the files "COPYING.lgplv3" and "COPYING.gplv3"
 *
 *   The Modified Berkeley Software Distribution License
 *      See the file "COPYING.mbsd"
 *
 *  These files have the following sha256 sums:
 *
 *  8584710e9b04216a394078dc156b781d0b47e1729104d666658aecef8ee32e95  COPYING.gplv3
 *  4379e7444a0e2ce2b12dd6f5a52a27a4d02d39d247901d3285c88cf0d37f477b  COPYING.lgplv3
 *  13aa749a5b0a454917a944ed8fffc530b784f5ead522b1aacaf4ec8aa55a6239  COPYING.mbsd
 */

#ifndef WINDOWS_CONFIG_HACKERY
#define WINDOWS_CONFIG_HACKERY 1

/*
 * The definitions below have been stolen from NTP's config.h for Windows.
 * However, they may be kept here in order to keep libopts independent from
 * the NTP project.
 */
#ifndef __windows__
#  define __windows__ 4
#endif

/*
 * Miscellaneous functions that Microsoft maps to other names
 */
#define snprintf _snprintf

#define SIZEOF_INT   4
#define SIZEOF_CHARP 4
#define SIZEOF_LONG  4
#define SIZEOF_SHORT 2

/*
 * VS.NET's version of wspiapi.h has a bug in it where it assigns a value
 * to a variable inside an if statement. It should be comparing them.
 * We prevent inclusion since we are not using this code so we don't have
 * to see the warning messages
 */
#ifndef _WSPIAPI_H_
#define _WSPIAPI_H_
#endif

/* Prevent inclusion of winsock.h in windows.h */
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#ifndef __RPCASYNC_H__
#define __RPCASYNC_H__
#endif

/* Include Windows headers */
#include <windows.h>
#include <winsock2.h>
#include <limits.h>

/*
 * Compatibility declarations for Windows, assuming SYS_WINNT
 * has been defined.
 */
#define strdup  _strdup
#define stat    _stat       /* struct stat from <sys/stat.h> */
#define unlink  _unlink
#define fchmod( _x, _y )
#define ssize_t SSIZE_T

#include <io.h>
#define open    _open
#define close   _close
#define read    _read
#define write   _write
#define lseek   _lseek
#define pipe    _pipe
#define dup2    _dup2

#define O_RDWR     _O_RDWR
#define O_RDONLY   _O_RDONLY
#define O_EXCL     _O_EXCL

#ifndef S_ISREG
#  define S_IFREG _S_IFREG
#  define       S_ISREG(mode)   (((mode) & S_IFREG) == S_IFREG)
#endif

#ifndef S_ISDIR
#  define S_IFDIR _S_IFDIR
#  define       S_ISDIR(mode)   (((mode) & S_IFDIR) == S_IFDIR)
#endif

/* C99 exact size integer support. */
# include <inttypes.h>

#endif /* WINDOWS_CONFIG_HACKERY */
/* windows-config.h ends here */
