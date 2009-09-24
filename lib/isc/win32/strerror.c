/*
 * Copyright (C) 2002  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* From BIND 9 lib/isc/win32/: strerror.c,v 1.5 2002/08/01 03:52:14 mayer */

/*
 * We don't need this warning message
 */
#pragma warning(disable: 4127) /* conditional expression is constant */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <isc/list.h>
#include "ntp_stdlib.h"


/*
 * Messsage list
 */
typedef struct msg_list msg_list_t;

struct msg_list {
	int code;
	char *msg;
	ISC_LINK(msg_list_t) link;
};

static ISC_LIST(msg_list_t) errormsg_list;

BOOL initialized = FALSE;

static CRITICAL_SECTION ErrorMsgLock;
# define LOCK(lock)	EnterCriticalSection(lock)
# define UNLOCK(lock)	LeaveCriticalSection(lock)

/*
 * Forward declarations
 */

char *
FormatError(int error);

char *
GetWSAErrorMessage(int errval);

static char *
isc__NTstrerror(int err);

/*
 * Initialize the error message list
 */

void
initialize() {
	ISC_LIST_INIT(errormsg_list);
	InitializeCriticalSection(&ErrorMsgLock);
	initialized = TRUE;
}

char *
NTstrerror(int errnum) {
	if(!initialized)
		initialize();
	return (isc__NTstrerror(errnum));
}
/*
 * This routine needs to free up any buffer allocated by FormatMessage
 * if that routine gets used.
 */

void
isc__strerror(int num, char *buf, size_t size) {
	char *msg;
	unsigned int unum = num;
	if(!initialized)
		initialize();

	msg = isc__NTstrerror(num);
	/*
	 * C4996 is strncpy, _snprintf may be unsafe, consider
	 * strncpy_s, _snprintf_s instead
	 * Those _s routines are not available with older MS
	 * compilers and our paranoid uses are safe.  An
	 * alternative would be to use the _s versions for
	 * newer compilers instead of disabling warning 4996.
	 */
	#if defined(_MSC_VER)
		#pragma warning(push)
		#pragma warning(disable: 4996)
	#endif /* _MSC_VER */

	/* 
	 * Both strncpy and _snprintf will not null terminate in the
	 * edge case of the message fitting but the terminator not.
	 * This seems to be a MS-specific behavior, but our paranoid
	 * approach should be harmless on less challenged runtimes.
	 */
	if (msg != NULL) {
		strncpy(buf, msg, size - 1);
	} else {
		_snprintf(buf, size - 1, "Unknown error: %u", unum);
	}
	buf[size - 1] = '\0';

	#if defined(_MSC_VER)
		#pragma warning(pop)
	#endif /* _MSC_VER */
}

/*
 * FormatError returns a message string for a given error code.
 * Callers should not attempt to free the string, as it is cached
 * and used again.
 */
char *
FormatError(int error) {
	LPVOID lpMsgBuf = NULL;

	msg_list_t *lmsg; 

	/*
	 * See if we already have the error code
	 */

	LOCK(&ErrorMsgLock);

	lmsg = ISC_LIST_HEAD(errormsg_list);
	while (lmsg != NULL) {
		if (lmsg->code == error) {
			lpMsgBuf = lmsg->msg;
			UNLOCK(&ErrorMsgLock);
			return (lpMsgBuf);
		}
		lmsg = ISC_LIST_NEXT(lmsg, link);
	}

	/*
	 * Not found
	 */

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		/* Default language */
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0,
		NULL); 

	if (lpMsgBuf) {
		lmsg = emalloc(sizeof(*lmsg));
		lmsg->code = error;
		lmsg->msg = lpMsgBuf;
		ISC_LIST_APPEND(errormsg_list, lmsg, link);
	}
	UNLOCK(&ErrorMsgLock);
	return (lpMsgBuf);
}

/*
 * Free the memory allocated for error messages during shutdown to
 * increase the utility of _DEBUG MS CRT's malloc leak checks.
 *
 * ntservice_exit() calls this routine during shutdown.  (ntservice.c)
 */
#ifdef _DEBUG
void
FormatErrorFreeMem(void) {
	msg_list_t *entry;
	/*
	 * Delete the lock used to protect the error message list as
	 * a cheap way of catching FormatError calls after this
	 * routine has torn things down for termination.
	 */
	DeleteCriticalSection(&ErrorMsgLock);

	entry = ISC_LIST_HEAD(errormsg_list);
	while (entry) {
		ISC_LIST_DEQUEUE(errormsg_list, entry, link);
		
		/* Memory from FormatMessage goes back via LocalFree */
		LocalFree(entry->msg);
		free(entry);

		entry = ISC_LIST_HEAD(errormsg_list);
	}
}
#endif /* _DEBUG */

/*
 * This routine checks the error value and calls the WSA Windows
 * Sockets Error message function GetWSAErrorMessage below if it's
 * within that range since those messages are not available in the
 * system error messages in Windows NT 4 and earlier Windows.  With
 * Windows 2000 and later, FormatMessage will translate socket errors,
 * so GetWSAErrorMessage can be removed once Windows NT is no longer
 * supported at runtime.
 */
static char *
isc__NTstrerror(int err) {
	char *retmsg = NULL;

	/* Get the Winsock2 error messages */
	if (err >= WSABASEERR && err <= (WSABASEERR + 1015)) {
		retmsg = GetWSAErrorMessage(err);
	}
	/*
	 * C4996 here is strerror may be unsafe, consider
	 * strerror_s instead.
	 * strerror_s copies the message into a caller-supplied buffer,
	 * which is not an option when providing a strerror() wrapper.
	 */
	#if defined(_MSC_VER)
		#pragma warning(push)
		#pragma warning(disable: 4996)
	#endif /* _MSC_VER */

	if (!retmsg) {
#undef strerror
		retmsg = strerror(err);
	}

	#if defined(_MSC_VER)
		#pragma warning(pop)
	#endif /* _MSC_VER */

	if (!retmsg) {
		/*
		 * Handle a few errno values MS strerror() doesn't,
		 * since they only generate a subset of errno values
		 * in the C runtime.  These are used by RFC 2783
		 * PPSAPI timepps.h functions in ntpd on Windows.
		 */

		#ifndef ENXIO
		#define ENXIO		6
		#endif

		#ifndef EOPNOTSUPP
		#define EOPNOTSUPP	45
		#endif

		switch (err) {
		    case ENXIO:
			retmsg = "Device not configured";
			break;

		    case EOPNOTSUPP:
			retmsg = "Operation not supported";
			break;
		}
	}
	/*
	 * Handle Windows GetLastError error codes.  Note that some
	 * low-numbered Windows error codes conflict with the errno
	 * space and this routine will return the errno-related text
	 * in preference to the GetLastError/FormatMessage text for
	 * those conflicting values.
	 */
	if (!retmsg) {
		retmsg = FormatError(err);
	}

	return (retmsg);
}

/*
 * This is a replacement for perror
 */
void __cdecl
NTperror(char *errmsg) {
	/* Copy the error value first in case of other errors */
	int errval = errno; 
	char *msg;
	if(!initialized)
		initialize();

	msg = isc__NTstrerror(errval);
	if (!msg) {
		msg = "unrecognized errno";
	}
	fprintf(stderr, "%s: %s\n", errmsg, msg);
}

/*
 * Return the error string related to Winsock2 errors.
 * This function is necessary since FormatMessage knows nothing about them
 * and there is no function to get them.
 */
char *
GetWSAErrorMessage(int errval) {
	char *msg;

	switch (errval) {

	case WSAEINTR:
		msg = "Interrupted system call";
		break;

	case WSAEBADF:
		msg = "Bad file number";
		break;

	case WSAEACCES:
		msg = "Permission denied";
		break;

	case WSAEFAULT:
		msg = "Bad address";
		break;

	case WSAEINVAL:
		msg = "Invalid argument";
		break;

	case WSAEMFILE:
		msg = "Too many open sockets";
		break;

	case WSAEWOULDBLOCK:
		msg = "Operation would block";
		break;

	case WSAEINPROGRESS:
		msg = "Operation now in progress";
		break;

	case WSAEALREADY:
		msg = "Operation already in progress";
		break;

	case WSAENOTSOCK:
		msg = "Socket operation on non-socket";
		break;

	case WSAEDESTADDRREQ:
		msg = "Destination address required";
		break;

	case WSAEMSGSIZE:
		msg = "Message too long";
		break;

	case WSAEPROTOTYPE:
		msg = "Protocol wrong type for socket";
		break;

	case WSAENOPROTOOPT:
		msg = "Bad protocol option";
		break;

	case WSAEPROTONOSUPPORT:
		msg = "Protocol not supported";
		break;

	case WSAESOCKTNOSUPPORT:
		msg = "Socket type not supported";
		break;

	case WSAEOPNOTSUPP:
		msg = "Operation not supported on socket";
		break;

	case WSAEPFNOSUPPORT:
		msg = "Protocol family not supported";
		break;

	case WSAEAFNOSUPPORT:
		msg = "Address family not supported";
		break;

	case WSAEADDRINUSE:
		msg = "Address already in use";
		break;

	case WSAEADDRNOTAVAIL:
		msg = "Can't assign requested address";
		break;

	case WSAENETDOWN:
		msg = "Network is down";
		break;

	case WSAENETUNREACH:
		msg = "Network is unreachable";
		break;

	case WSAENETRESET:
		msg = "Net connection reset";
		break;

	case WSAECONNABORTED:
		msg = "Software caused connection abort";
		break;

	case WSAECONNRESET:
		msg = "Connection reset by peer";
		break;

	case WSAENOBUFS:
		msg = "No buffer space available";
		break;

	case WSAEISCONN:
		msg = "Socket is already connected";
		break;

	case WSAENOTCONN:
		msg = "Socket is not connected";
		break;

	case WSAESHUTDOWN:
		msg = "Can't send after socket shutdown";
		break;

	case WSAETOOMANYREFS:
		msg = "Too many references: can't splice";
		break;

	case WSAETIMEDOUT:
		msg = "Connection timed out";
		break;

	case WSAECONNREFUSED:
		msg = "Connection refused";
		break;

	case WSAELOOP:
		msg = "Too many levels of symbolic links";
		break;

	case WSAENAMETOOLONG:
		msg = "File name too long";
		break;

	case WSAEHOSTDOWN:
		msg = "Host is down";
		break;

	case WSAEHOSTUNREACH:
		msg = "No route to host";
		break;

	case WSAENOTEMPTY:
		msg = "Directory not empty";
		break;

	case WSAEPROCLIM:
		msg = "Too many processes";
		break;

	case WSAEUSERS:
		msg = "Too many users";
		break;

	case WSAEDQUOT:
		msg = "Disc quota exceeded";
		break;

	case WSAESTALE:
		msg = "Stale NFS file handle";
		break;

	case WSAEREMOTE:
		msg = "Too many levels of remote in path";
		break;

	case WSASYSNOTREADY:
		msg = "Network system is unavailable";
		break;

	case WSAVERNOTSUPPORTED:
		msg = "Winsock version out of range";
		break;

	case WSANOTINITIALISED:
		msg = "WSAStartup not yet called";
		break;

	case WSAEDISCON:
		msg = "Graceful shutdown in progress";
		break;
/*
	case WSAHOST_NOT_FOUND:
		msg = "Host not found";
		break;

	case WSANO_DATA:
		msg = "No host data of that type was found";
		break;
*/
	default:
		msg = NULL;
		break;
	}
	return (msg);
}

#ifdef NTP_DEAD_CODE_GetCryptErrorMessage
/*
 * These error messages are more informative about CryptAPI Errors than the
 * standard error messages
 */

char *
GetCryptErrorMessage(int errval) {
	char *msg;

	switch (errval) {

	case NTE_BAD_FLAGS:
		msg = "The dwFlags parameter has an illegal value.";
		break;
	case NTE_BAD_KEYSET:
		msg = "The Registry entry for the key container "
			"could not be opened and may not exist.";
		break;
	case NTE_BAD_KEYSET_PARAM:
		msg = "The pszContainer or pszProvider parameter "
			"is set to an illegal value.";
		break;
	case NTE_BAD_PROV_TYPE:
		msg = "The value of the dwProvType parameter is out "
			"of range. All provider types must be from "
			"1 to 999, inclusive.";
		break;
	case NTE_BAD_SIGNATURE:
		msg = "The provider DLL signature did not verify "
			"correctly. Either the DLL or the digital "
			"signature has been tampered with.";
		break;
	case NTE_EXISTS:
		msg = "The dwFlags parameter is CRYPT_NEWKEYSET, but the key"
		      " container already exists.";
		break;
	case NTE_KEYSET_ENTRY_BAD:
		msg = "The Registry entry for the pszContainer key container "
		      "was found (in the HKEY_CURRENT_USER window), but is "
		      "corrupt. See the section System Administration for "
		      " etails about CryptoAPI's Registry usage.";
		break;
	case NTE_KEYSET_NOT_DEF:
		msg = "No Registry entry exists in the HKEY_CURRENT_USER "
			"window for the key container specified by "
			"pszContainer.";
		break;
	case NTE_NO_MEMORY:
		msg = "The CSP ran out of memory during the operation.";
		break;
	case NTE_PROV_DLL_NOT_FOUND:
		msg = "The provider DLL file does not exist or is not on the "
		      "current path.";
		break;
	case NTE_PROV_TYPE_ENTRY_BAD:
		msg = "The Registry entry for the provider type specified by "
		      "dwProvType is corrupt. This error may relate to "
		      "either the user default CSP list or the machine "
		      "default CSP list. See the section System "
		      "Administration for details about CryptoAPI's "
		      "Registry usage.";
		break;
	case NTE_PROV_TYPE_NO_MATCH:
		msg = "The provider type specified by dwProvType does not "
		      "match the provider type found in the Registry. Note "
		      "that this error can only occur when pszProvider "
		      "specifies an actual CSP name.";
		break;
	case NTE_PROV_TYPE_NOT_DEF:
		msg = "No Registry entry exists for the provider type "
		      "specified by dwProvType.";
		break;
	case NTE_PROVIDER_DLL_FAIL:
		msg = "The provider DLL file could not be loaded, and "
		      "may not exist. If it exists, then the file is "
		      "not a valid DLL.";
		break;
	case NTE_SIGNATURE_FILE_BAD:
		msg = "An error occurred while loading the DLL file image, "
		      "prior to verifying its signature.";
		break;

	default:
		msg = NULL;
		break;
	}
	return msg;
}
#endif
