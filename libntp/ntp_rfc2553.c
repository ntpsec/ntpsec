/*
 * Compatibility shims with the rfc2553 API to simplify ntp.
 *
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 *
 * Copyright (c) 1982, 1986, 1990, 1993
 *	The Regents of the University of California.
 *
 * SPDX-License-Identifier: BSD-2-clause
 */

#include <config.h>

#include <sys/types.h>
#include <ctype.h>
#include <sys/socket.h>
#include <isc/net.h>
#include <netinet/in.h>
#include <string.h>

#include "ntp_rfc2553.h"

#include "ntpd.h"
#include "ntp_malloc.h"
#include "ntp_debug.h"


/*
 * copy_addrinfo()	- copy a single addrinfo to malloc()'d block.
 * copy_addrinfo_list() - copy an addrinfo list to malloc()'d block.
 *
 * Copies an addrinfo list and its associated data to a contiguous block
 * of storage from emalloc().  Callback routines invoked via
 * getaddrinfo_sometime() have access to the resulting addrinfo list
 * only until they return.  This routine provides an easy way to make a
 * persistent copy.  Although the list provided to gai_sometime_callback
 * routines is similarly contiguous, to keep this code usable in any
 * context where we might want to duplicate an addrinfo list, it does
 * not require the input list be contiguous.
 *
 * The returned list head pointer is passed to free() to release the
 * entire list.
 *
 * In keeping with the rest of the NTP distribution, sockaddr_u is used
 * in preference to struct sockaddr_storage, which is a member of the
 * former union and so compatible.
 */
struct addrinfo * copy_addrinfo_common(const struct addrinfo *, int
#ifdef EREALLOC_CALLSITE
								   ,
				       const char *, int
#endif
				       );


struct addrinfo *
copy_addrinfo_impl(
	const struct addrinfo *	src
#ifdef EREALLOC_CALLSITE
				   ,
	const char *		caller_file,
	int			caller_line
#endif
	)
{
	return copy_addrinfo_common(src, true
#ifdef EREALLOC_CALLSITE
					      ,
				    caller_file, caller_line
#endif
				    );
}


struct addrinfo *
copy_addrinfo_list_impl(
	const struct addrinfo *	src
#ifdef EREALLOC_CALLSITE
				   ,
	const char *		caller_file,
	int			caller_line
#endif
	)
{
	return copy_addrinfo_common(src, false
#ifdef EREALLOC_CALLSITE
					      ,
				    caller_file, caller_line
#endif
				    );
}


struct addrinfo *
copy_addrinfo_common(
	const struct addrinfo *	src,
	int			just_one
#ifdef EREALLOC_CALLSITE
					,
	const char *		caller_file,
	int			caller_line
#endif
	)
{
	const struct addrinfo *	ai_src;
	const struct addrinfo *	ai_nxt;
	struct addrinfo *	ai_cpy;
	struct addrinfo *	dst;
	sockaddr_u *		psau;
	char *			pcanon;
	u_int			elements;
	size_t			octets;
	size_t			canons_octets;
	size_t			str_octets;

	elements = 0;
	canons_octets = 0;

	for (ai_src = src; NULL != ai_src; ai_src = ai_nxt) {
		if (just_one)
			ai_nxt = NULL;
		else
			ai_nxt = ai_src->ai_next;
		++elements;
		if (NULL != ai_src->ai_canonname)
			canons_octets += 1 + strlen(ai_src->ai_canonname);
	}

	octets = elements * (sizeof(*ai_cpy) + sizeof(*psau));
	octets += canons_octets;

	dst = erealloczsite(NULL, octets, 0, true, caller_file,
			    caller_line);
	ai_cpy = dst;
	psau = (void *)(ai_cpy + elements);
	pcanon = (void *)(psau + elements);

	for (ai_src = src; NULL != ai_src; ai_src = ai_nxt) {
		if (just_one)
			ai_nxt = NULL;
		else
			ai_nxt = ai_src->ai_next;
		*ai_cpy = *ai_src;
		REQUIRE(ai_src->ai_addrlen <= sizeof(sockaddr_u));
		memcpy(psau, ai_src->ai_addr, ai_src->ai_addrlen);
		ai_cpy->ai_addr = &psau->sa;
		++psau;
		if (NULL != ai_cpy->ai_canonname) {
			ai_cpy->ai_canonname = pcanon;
			str_octets = 1 + strlen(ai_src->ai_canonname);
			memcpy(pcanon, ai_src->ai_canonname, str_octets);
			pcanon += str_octets;
		}
		if (NULL != ai_cpy->ai_next) {
			if (just_one)
				ai_cpy->ai_next = NULL;
			else
				ai_cpy->ai_next = ai_cpy + 1;
		}
		++ai_cpy;
	}
	NTP_ENSURE(pcanon == ((char *)dst + octets));

	return dst;
}


