/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Copyright (c) 1982, 1986, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 * Compatability shims with the rfc2553 API to simplify ntp.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include "ntp_rfc2553.h"
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#include <netdb.h>

#include "ntpd.h"
#include "ntp_malloc.h"
#include "ntp_stdlib.h"
#include "ntp_string.h"

#ifndef HAVE_IPV6

#define	LOCALHOST	0x7f000001	/* 127.0.0.1, in hex, of course */

#if 0
/* XXX This is the preferred way, but for some reason the SunOS compiler
 * does not like it.
 */
const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
#else
const struct in6_addr in6addr_any;
#endif

static char *ai_errlist[] = {
	"Success",
	"Address family for hostname not supported",	/* EAI_ADDRFAMILY */
	"Temporary failure in name resolution",		/* EAI_AGAIN      */
	"Invalid value for ai_flags",		       	/* EAI_BADFLAGS   */
	"Non-recoverable failure in name resolution", 	/* EAI_FAIL       */
	"ai_family not supported",			/* EAI_FAMILY     */
	"Memory allocation failure", 			/* EAI_MEMORY     */
	"No address associated with hostname", 		/* EAI_NODATA     */
	"hostname nor servname provided, or not known",	/* EAI_NONAME     */
	"servname not supported for ai_socktype",	/* EAI_SERVICE    */
	"ai_socktype not supported", 			/* EAI_SOCKTYPE   */
	"System error returned in errno", 		/* EAI_SYSTEM     */
	"Invalid value for hints",			/* EAI_BADHINTS	  */
	"Resolved protocol is unknown",			/* EAI_PROTOCOL   */
	"Unknown error", 				/* EAI_MAX        */
};

int
getaddrinfo (const char *nodename, const char *servname,
	const struct addrinfo *hints, struct addrinfo **res)
{
	struct addrinfo *ai;
	struct hostent *hp;
	struct sockaddr_in *sockin;

	ai = calloc(sizeof(struct addrinfo), 1);
	if (ai == NULL)
		return (EAI_MEMORY);

	if (nodename != NULL) {
		hp = gethostbyname(nodename);
		if (hp == NULL) {
			if (h_errno == TRY_AGAIN)
				return (EAI_AGAIN);
			else
				return (EAI_FAIL);
		}
		ai->ai_addr = calloc(sizeof(struct sockaddr_storage), 1);
		if (ai->ai_addr == NULL)
			return (EAI_MEMORY);
		ai->ai_family = hp->h_addrtype;
		ai->ai_addrlen = sizeof(struct sockaddr_storage);
		sockin = (struct sockaddr_in *)ai->ai_addr;
		memmove(&sockin->sin_addr, hp->h_addr, hp->h_length);
		ai->ai_addr->sa_family = hp->h_addrtype;
#ifdef HAVE_SA_LEN_IN_STRUCT_SOCKADDR
		ai->ai_addr->sa_len = sizeof(struct sockaddr);
#endif
	}
	if (nodename == NULL && hints != NULL) {
		ai->ai_addr = calloc(sizeof(struct sockaddr_storage), 1);
		if (ai->ai_addr == NULL)
			return (EAI_MEMORY);
		ai->ai_family = AF_INET;
		ai->ai_addrlen = sizeof(struct sockaddr_storage);
		sockin = (struct sockaddr_in *)ai->ai_addr;
		sockin->sin_addr.s_addr = htonl(LOCALHOST);
#ifdef HAVE_SA_LEN_IN_STRUCT_SOCKADDR
		ai->ai_addr->sa_len = sizeof(struct sockaddr);
#endif
	}
	if (servname != NULL) {
		ai->ai_family = AF_INET;
		ai->ai_socktype = SOCK_DGRAM;
		if (strcmp(servname, "123") != 0)
			return (EAI_SERVICE);
		sockin = (struct sockaddr_in *)ai->ai_addr;
		sockin->sin_port = htons(NTP_PORT);
	}
	*res = ai;
	return (0);
}

void
freeaddrinfo(struct addrinfo *ai)
{
	if (ai->ai_canonname != NULL)
		free(ai->ai_canonname);
	if (ai->ai_addr != NULL)
		free(ai->ai_addr);
	free(ai);
}

int
getnameinfo (const struct sockaddr *sa, u_int salen, char *host,
	size_t hostlen, char *serv, size_t servlen, int flags)
{
	struct hostent *hp;

	if (sa->sa_family != AF_INET)
		return (EAI_FAMILY);
	hp = gethostbyaddr((const char *)sa->sa_data, 4, AF_INET);
	if (hp == NULL) {
		if (h_errno == TRY_AGAIN)
			return (EAI_AGAIN);
		else
			return (EAI_FAIL);
	}
	if (host != NULL) {
		strncpy(host, hp->h_name, hostlen);
		host[hostlen] = '\0';
	}
	return (0);
}

char *
gai_strerror(ecode)
	int ecode;
{
	if (ecode < 0 || ecode > EAI_MAX)
		ecode = EAI_MAX;
	return ai_errlist[ecode];
}


#endif /* !HAVE_IPV6 */
