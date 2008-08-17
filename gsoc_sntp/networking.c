#include <config.h>
#include <unistd.h>

#include "sntp-opts.h"	
#include "networking.h"
#include "header.h"
#include "utilities.h"
#include "log.h"

char adr_buf[INET6_ADDRSTRLEN];


int 
resolve_hosts (
		char **hosts, 
		int hostc, 
		struct addrinfo **res
		) 
{
	register unsigned int a, b;
	unsigned int entryc = 0; 

	if(hostc < 1) 
		return 0;
	
	struct addrinfo ***tres = (struct addrinfo ***) malloc(sizeof(struct addrinfo **) * hostc);

	for(a=0; a<hostc; a++) {
		tres[a] = (struct addrinfo **) malloc(sizeof(struct addrinfo));

#ifdef DEBUG
		printf("Starting host resolution for %s...\n", hosts[a]); 
#endif

		struct addrinfo hints, *dres;
		int error;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = PF_UNSPEC;
		hints.ai_socktype = SOCK_DGRAM;
/*		hints.ai_protocol = IPPROTO_UDP; */


		error = getaddrinfo(hosts[a], "123", &hints, tres[a]);

		if(error) {
			size_t msg_length = strlen(hosts[a]) + 21;
			char *logmsg = (char *) malloc(sizeof(char) * msg_length);

			snprintf(logmsg, msg_length, "Error looking up %s", hosts[a]);
#ifdef DEBUG
			printf("%s\n", logmsg);
#endif

			log_msg(logmsg, 1);
			res[a] = 0;
		}
		else {
			for(dres=*tres[a]; dres; dres=dres->ai_next) {
				entryc++;
#ifdef DEBUG	
				getnameinfo(dres->ai_addr, dres->ai_addrlen, adr_buf, INET6_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);
				STDLINE
				printf("Resolv No.: %i Result of getaddrinfo for %s:\n", entryc, hosts[a]);
				printf("socktype: %i ", dres->ai_socktype); 
				printf("protocol: %i ", dres->ai_protocol);
				printf("Prefered socktype: %i IP: %s\n", dres->ai_socktype, adr_buf);
				STDLINE
#endif
			}
		}
	}

#ifdef DEBUG
	printf("Retrieved %i DNS entries, continuing...\n", entryc);
#endif

	/* Make a list of the addrinfo list entries, start by counting them */
	struct addrinfo **result = (struct addrinfo **) malloc(sizeof(struct addrinfo**) * entryc);

	for(a=0, b=0; a<hostc; a++) {
		struct addrinfo *cur = *tres[a];

		if(cur->ai_next == NULL) {
			result[b] = *tres[a];
		}
		else {
			struct addrinfo *seek = *tres[a];

			for(; b<entryc && seek; b++) {
				result[b] = seek;
				seek = seek->ai_next;
			}
		}
	}

#ifdef DEBUG
	for(a=0; a<entryc; a++)
		getnameinfo(result[a]->ai_addr, result[a]->ai_addrlen, adr_buf, INET6_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);

		printf("%x: IP %s\n", (unsigned int) result[a], adr_buf); 
#endif

	*res = (struct addrinfo *) malloc(sizeof(struct addrinfo *) * entryc);

	for(a=0; a<entryc; a++)
		res[a] = result[a];

	return entryc;
}

void 
create_socket (
		SOCKET *rsock,
		struct sockaddr_storage *dest
		)
{
	*rsock = socket(dest->ss_family, SOCK_DGRAM, 0);

#ifdef DEBUG
	if(*rsock == -1)
		printf("Failed to create UDP socket with family %i\n", dest->ss_family);
#endif

}

void 
close_socket (
		SOCKET rsock
		)
{
	close(rsock);
}

/* Send a packet */
void
sendpkt (
	SOCKET rsock,
	struct sockaddr_storage *dest,
	struct pkt *pkt,
	int len
	)
{
#ifdef DEBUG
	pkt_output(pkt, len, stdout);
	getnameinfo((struct sockaddr *) dest, dest->ss_len, adr_buf, INET6_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);

	printf("\nSending packet to %s...\n", adr_buf);
#endif

	int cc = sendto(rsock, (char *)pkt, len, 0, (struct sockaddr *)dest, dest->ss_len);

	if (cc == SOCKET_ERROR) {
#ifdef DEBUG
		printf("Socket error: %i. Couldn't send packet!\n", cc);
#endif

		if (errno != EWOULDBLOCK && errno != ENOBUFS) {

		}
	}
#ifdef DEBUG
	else {
		printf("Packet sent.\n");
	}
#endif
}

int
recvdata (
		SOCKET rsock,
		struct sockaddr_storage *sender,
		char *rdata,
		size_t rdata_length,
		char *done
	 )
{
	socklen_t slen = SOCKLEN(&rsock);

#ifdef DEBUG
	printf("Trying to receive data from...\n");
#endif

	int recvc = recvfrom(rsock, rdata, rdata_length, 0, 
			(struct sockaddr *) sender, &slen);
#ifdef DEBUG
	printf("recvfrom returned...\n");
#endif

#ifdef DEBUG

	if(recvc > 0) {
		getnameinfo((struct sockaddr *)sender, sender->ss_len, adr_buf, INET6_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);

		printf("Received %i bytes from %s:\n", recvc, adr_buf);

		pkt_output((struct pkt *) rdata, recvc, stdout);
	}
	else {
		printf("Failure, recvc: %i\n", recvc);
	}
#endif
	/* Remove this when found a reasonable max. size. For now
	 * notify when there's data left to fetch 
	 */
	if(recvc >= 511)
		*done = 0;
	else
		*done = 1;

	return recvc;
}

/* Fetch data, check if it's data for us and whether it's useable or not. If not, return -1
 * so we can delete this server from our list and continue with another one. 
 */
int 
recvpkt (
		SOCKET rsock,
		struct pkt *rpkt,
		struct pkt *spkt
	)
{
	register int a;
	int has_mac;
	int is_authentic;

	l_fp tmp;
	l_fp org;
	l_fp rec;


	struct sockaddr_storage sender;
	char *rdata, done;

	/* Much space, just to be sure */
	rdata = (char *) malloc(sizeof(char) * 512);

	int pkt_length = recvdata(rsock, &sender, rdata, 512, &done);

	if(!done) {
		/* Do something about it, first check for a maximum length of ntp packets,
		 * probably that's something we can avoid 
		 */
	}
	
	/* Some checks to see if that packet is intended for us */

	if(pkt_length == LEN_PKT_NOMAC)
		has_mac = 0;

	else if(pkt_length > LEN_PKT_NOMAC) 
		has_mac = pkt_length - LEN_PKT_NOMAC;
	
	else
		if(debug) {
			fprintf(stderr, "recvpkt: Funny packet length: %i. Discarding package.\n", pkt_length);
			return -1;
		}

	if(has_mac) {
		if(has_mac > MAX_MAC_LEN || has_mac % 4 != 0) {
			return -1;
							}
		/* Do auth stuff */
	}

	/*struct pkt *tmp_rpkt = (struct pkt *)rdata;
	rpkt->li_vn_mode = tmp_rpkt->li_vn_mode; 
	rpkt->stratum = tmp_rpkt->stratum;
	rpkt->ppoll = tmp_rpkt->ppoll;
	rpkt->precision = tmp_rpkt->precision;
	rpkt->rootdelay = tmp_rpkt->rootdelay;
	rpkt->rootdisp = tmp_rpkt->rootdisp;
	rpkt->refid = tmp_rpkt->refid;
	rpkt->reftime = tmp_rpkt->reftime;
	rpkt->org = tmp_rpkt->org;
	rpkt->rec = tmp_rpkt->rec;
	rpkt->xmt = tmp_rpkt->xmt; */

	if(pkt_length > LEN_PKT_MAC) {
		if(ENABLED_OPT(NORMALVERBOSE))
			printf("sntp recvpkt: Received packet is too big (%i bytes), trying again to get a useable packet\n", 
					pkt_length);

		return -2;
	}
	
	for(a=0; a<pkt_length; a++) 
		((unsigned char *) rpkt)[a] = rdata[a];

	printf("%x %x\n", PKT_VERSION(rpkt->li_vn_mode), NTP_OLDVERSION);
	if (PKT_VERSION(rpkt->li_vn_mode) < NTP_OLDVERSION ||
		PKT_VERSION(rpkt->li_vn_mode) > NTP_VERSION) {
		return -1;
	} 

	if (PKT_MODE(rpkt->li_vn_mode) != MODE_SERVER
		 && PKT_MODE(rpkt->li_vn_mode) != MODE_PASSIVE) {
#ifdef DEBUG
			printf("sntp recvpkt: mode %d stratum %i\n",
			   PKT_MODE(rpkt->li_vn_mode), rpkt->stratum);
#endif
		return -1;
	}

	if(rpkt->stratum == STRATUM_PKT_UNSPEC) {
		if(ENABLED_OPT(NORMALVERBOSE))
			printf("sntp recvpkt: Unusable packet, discarding. (stratum: %i)\n", rpkt->stratum);
		
		return -1;
	}

	if(PKT_LEAP(rpkt->li_vn_mode) == LEAP_NOTINSYNC) {
		if(ENABLED_OPT(NORMALVERBOSE)) 
			printf("sntp recvpkt: Server not in sync, skipping this server\n");

		return -1;
	}


	/*
	 * Decode the org timestamp and make sure we're getting a response
	 * to our last request. 
	 */
	NTOHL_FP(&rpkt->org, &org);
	if (!L_ISEQU(&org, &spkt->xmt)) {
		if (debug)
			printf("receive: pkt.org and peer.xmt differ\n");
		return -1;
	}


	/* Left for now, finishin other stuff. I think I might want that somewhere else,
	 * don't want this function to do on-wire tasks. Sanity checks are right here I think
	 *
	server->leap = PKT_LEAP(rpkt->li_vn_mode);
	server->stratum = PKT_TO_STRATUM(rpkt->stratum);
	server->precision = rpkt->precision;
	server->rootdelay = ntohl(rpkt->rootdelay);
	server->rootdisp = ntohl(rpkt->rootdisp);
	server->refid = rpkt->refid;
	NTOHL_FP(&rpkt->reftime, &server->reftime);
	NTOHL_FP(&rpkt->rec, &rec);
	NTOHL_FP(&rpkt->xmt, &server->org);

	if (L_ISZERO(&rec) || !L_ISHIS(&server->org, &rec)) 
		return -1; */

	return pkt_length;
}

/*
 * is_reachable - check to see if we have a route to given destination
 *		    (non-blocking).
 */
int
is_reachable (
		struct addrinfo *dst
		)
{
	SOCKET sockfd;

	sockfd = socket(dst->ai_family, SOCK_DGRAM, 0);

	if (sockfd == -1) {
#ifdef DEBUG
		printf("is_reachable: Couldn't create socket\n");
#endif
		return 0;
	}

	if(connect(sockfd, dst->ai_addr, SOCKLEN(dst->ai_addr))) {
		closesocket(sockfd);
		return 0;
	}

	closesocket(sockfd);
	return 1;
}

int 
filter_reachable (
		struct addrinfo **res,
		int resc
		)
{
	register int a, b;
	int filter_elements = 0;

	int *index = (int *) malloc(sizeof(int) * resc);
	
	for(a=0, b=0; a<resc; a++) {
		if(is_reachable(res[a])) {
			index[b] = a;
			b++;
		}
	}

	struct addrinfo **cpyres = (struct addrinfo **) malloc(sizeof(struct addrinfo *) * b);

	filter_elements = b+1;

	for(a=0; a<filter_elements; a++) 
		cpyres[a] = res[index[a]];


	for(a=0, b=0; a<resc; a++) {
		if(a == index[b]) 
			b++;
		else 
			freeaddrinfo(res[a]);
	}

	res = cpyres;

	return filter_elements;
}
