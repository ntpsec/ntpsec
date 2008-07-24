#include <ntp_fp.h>

#include "sntp-opts.h"
#include "networking.h"
#include "log.h"


int 
resolve_hosts (
		char **hosts, 
		int hostc, 
		struct addrinfo **res
	) 
{
	register unsigned int a, b;
	unsigned int entryc = 0; 

	res = (struct addrinfo **) malloc(sizeof(struct addrinfo) * hostc);
	
	
	for(a=0; a<hostc; a++) {
		struct addrinfo hints, **res, *res0;
		int error;
		const char *cause = NULL;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = PF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_UDP;


		error = getaddrinfo(hosts[a], "ntp", &hints, &res[a]);

		if(error) {
			size_t msg_length = strlen(hosts[a]) + 21;
			char *logmsg = (char *) malloc(sizeof(char) * msg_length);

			snprintf(logmsg, msg_length, "Error looking up %s", hosts[a]);
			log_msg(logmsg, 1);
		}
	}

	/* Make a list of the addrinfo list entries, start by counting them */
	for(a=0; a<hostc; a++) {
		entryc++;

		if(res[a]->ai_next != NULL) {
			while(res[a]->ai_next != NULL) 
				entryc++;
		}
	}

	struct addrinfo **result = (struct addrinfo **) malloc(sizeof(struct addrinfo) * entryc);

	for(a=0,b=0; a<entryc; b++) {
		if(res[b]->ai_next != NULL) {
			memcpy(result[a], res[b], sizeof(struct addrinfo));
			a++;
		}
		else {
			struct addrinfo *seek = res[b]->ai_next;

			for(; a<entryc && seek != NULL; a++) {
				memcpy(result[a], seek, sizeof(struct addrinfo));
				seek = seek->ai_next;
			}
		}
	}

	return entryc;
}

/* Send a packet */
void
sendpkt (
		struct sockaddr_storage *dest,
		struct pkt *pkt,
		int len
	)
{
	int sock;

	int cc = sendto(sock, (char *)pkt, len, 0, (struct sockaddr *)dest,
			SOCKLEN(dest));

	if (cc == SOCKET_ERROR) {
		if (errno != EWOULDBLOCK && errno != ENOBUFS) {

		}
	}

}

int
recvdata (
		SOCKET rsock,
		struct sockaddr *sender,
		char *rdata,
		size_t rdata_length,
		char *done
	 )
{
	socklen_t slen = SOCKLEN(rsock);

	int recvc = recvfrom(rsock, rdata, rdata_length, MSG_DONTWAIT, 
			sender, &slen);

	
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
 * so we can delete this server from our list and continue with another one. KOD package handling
 * is not done here. 
 */
int 
recvpkt (
		SOCKET rsock,
		struct pkt *rpkt,
		struct pkt *spkt
	)
{
	int has_mac;
	int is_authentic;

	l_fp ts;
	l_fp t10, t23, tmp;
	l_fp org;
	l_fp rec;
	l_fp ci;


	struct sockaddr sender;
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
		has_mac = 1;
	
	else
		if(debug) {
			fprintf(stderr, "recvpkt: Funny packet length: %i. Discarding package.\n", pkt_length);
			return -1;
		}

	rpkt = (struct pkt *)rdata;

	if (PKT_VERSION(rpkt->li_vn_mode) < NTP_OLDVERSION ||
		PKT_VERSION(rpkt->li_vn_mode) > NTP_VERSION) {
		return -1;
	}

	if ((PKT_MODE(rpkt->li_vn_mode) != MODE_SERVER
		 && PKT_MODE(rpkt->li_vn_mode) != MODE_PASSIVE)
		|| rpkt->stratum >= STRATUM_UNSPEC) {
		if (debug)
			fprintf(stderr, "receive: mode %d stratum %d\n",
			   PKT_MODE(rpkt->li_vn_mode), rpkt->stratum);
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

	/* Do authentication stuff here */

	/* Left for now, finishin other stuff. I think I might want that somewhere else,
	 * don't want this function to do on-wire stuff. Sanity checks are right here I think
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

/* Will be handled by recvpkt */
int
filter_ntp (
		struct addrinfo **res,
		int resc
	   )
{
	return 0;
}
