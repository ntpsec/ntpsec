/*
 * Copyright (C) 2006  Internet Systems Consortium, Inc. ("ISC")
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This module uses Windows lookup facilities to get the address information
 * wherever it resides. This avoids calling the Internet standard funcction
 * gethostbyname and gives us more control over the results since the
 * Microsoft implementation seems to return the wrong error code for some
 * conditions.
 */

#include <config.h>
#include <ws2tcpip.h>
#include <nspapi.h>
#include <svcguid.h>
#include <ntp_rfc2553.h>
#include <malloc.h>
#include <ntp_stdlib.h>
#include <syslog.h>

/*
 * Set this Macro to force retries even if it fails
 * the lookup
 */
#ifdef FORCE_DNSRETRY

#undef EAI_NONAME
#define EAI_NONAME EAI_AGAIN

#endif

typedef struct hostent hostent_t;

int ReturnCode(int errcode)
{
	int retcode;

	switch (errcode)
	{
	case 0:
		return (0);
	case WSAEINVAL:
		return (EAI_BADFLAGS);
	case WSANO_DATA:
		return (EAI_NONAME);
	case WSANOTINITIALISED:
	case WSASERVICE_NOT_FOUND:
		return (EAI_FAIL);
	case WSA_NOT_ENOUGH_MEMORY:
		return (EAI_MEMORY);
	default:
		return (EAI_FAIL);
	}
}

int
AddToAddresses(char **Addresses, int *cnt, CSADDR_INFO *csaddr)
{

	int csize;
	struct in_addr *sinaddr;
	char *addr;
	struct in_addr *addr_list;
	struct sockaddr_in *sin;
	sin = (struct sockaddr_in *) csaddr->RemoteAddr.lpSockaddr;
	if (*Addresses != NULL)
	{
		csize = _msize(*Addresses);
		addr_list = realloc(*Addresses, csize + sizeof(struct in_addr));
	}
	else
	{
		csize = 0;
		addr_list = malloc(sizeof(struct in_addr));
	}
	addr = (char *) addr_list;
	sinaddr = &((struct in_addr*) addr)[(*cnt)];
	memset(sinaddr, 0, sizeof(sinaddr));
	memcpy(sinaddr, &sin->sin_addr, sizeof(struct in_addr));
	
	(*cnt)++;
	*Addresses = addr;
	return 0;
}

int
DNSlookup_name(
	const char *name,
	int ai_family,
	struct hostent **Addresses
)
{
	char buffer[sizeof(WSAQUERYSET) + 2048];
	WSAQUERYSET query;
	hostent_t *addr = NULL;
	char *bufaddr = NULL;
	char ** addrlist = &bufaddr;
	int addrcnt = 0;
	WSAQUERYSET *results = (WSAQUERYSET *) buffer;
	GUID	HostnameGUID = SVCID_INET_HOSTADDRBYNAME;
	HANDLE	handle;
	DWORD dwLength;
	int err = 0;
	int retcode = 0;
	int errcode = 0;
	DWORD i;

	/*
	 * First we must create a query set
	 */
	memset(&query, 0, sizeof(query));
	query.dwSize = sizeof(query);
	query.lpszServiceInstanceName = (char *)name;
	query.dwNameSpace = NS_DNS;
	query.lpServiceClassId = &HostnameGUID;

	err = WSALookupServiceBegin(&query,
                                 LUP_RETURN_NAME | LUP_RETURN_BLOB | LUP_RETURN_ADDR,
                                 &handle);

	if(err == SOCKET_ERROR)
	{
		/*
		 * Convert the error code and return
		 */
		return (ReturnCode(WSAGetLastError()));
	}

	/*
	 * Initially none
	 * Change if we get something
	 */
	retcode = EAI_NONAME;
	dwLength = sizeof(buffer);
	*addrlist = NULL;

	while(err == 0)		/* Drop out when error */
	{
		memset(&buffer, 0, dwLength);
		err = WSALookupServiceNext(
			                handle,
			                0,
			                &dwLength,
			                results);
		errcode = WSAGetLastError();
		if (results->dwNumberOfCsAddrs > 0)
		{
			if (addr == NULL)
			{
				addr = (struct hostent *) malloc(sizeof(struct hostent));
				memset(addr, 0, sizeof(struct hostent));
				addr->h_addrtype = (short) results->lpcsaBuffer->iSocketType;
				addr->h_length = (short) results->lpcsaBuffer->RemoteAddr.iSockaddrLength;
			}
			for (i = 0; i < results->dwNumberOfCsAddrs; i++)
			{
				AddToAddresses(addrlist, &addrcnt, &results->lpcsaBuffer[i]);
			}
		}

	}
	if (addr != NULL)
	{
		addr->h_name = (char *) name;
		addr->h_addr_list = addrlist;
		retcode = 0;
	}
	else
	{
#ifdef FORCE_DNSRETRY
		/*
		 * We do this or the error would never be logged
		 */
		if (errcode == WSANO_DATA)
			msyslog(LOG_ERR, "Address not found for %s", name);
#endif
		retcode = ReturnCode(errcode);
	}
	*Addresses = addr;
	WSALookupServiceEnd(handle);
	return (retcode);
}

