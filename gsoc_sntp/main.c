#if 0
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#include <ntp_stdlib.h>
#include <ntp_unixtime.h>
#include <l_stdlib.h>
#include <isc/result.h>
#include <stdio.h>

#include <sntp-opts.h>

#include "networking.h"
/*#include "header.h"*/

#define NTP_PORT 123

#define DEBUG

int ai_fam_templ;
volatile int debug;

/* tOptions sntpOptions; */


int 
main (
		int argc,
		char **argv
		) 
{
	return sntp_main(argc, argv);
}

int  
sntp_main (
		int argc, 
		char **argv
		) 
{
	char *kod_file, *log_file;
	register int c;

#ifdef DEBUG
	printf("Starting %s with %i arguments...\n", argv[0], argc);
#endif 

	if (isc_net_probeipv6() != ISC_R_SUCCESS) {
		ai_fam_templ = AF_INET;
#ifdef DEBUG
		printf("No ipv6 support available, forcing ipv4\n");
#endif
	}

#ifndef NO_DISK
#endif

	int optct = optionProcess(&sntpOptions, argc, argv);
	argc -= optct;
	argv += optct; 

	for(c=0; c<argc; c++) 
		printf("%s\n", argv[c]);


	/* Parse config file if declared TODO */

	/* Initialize logging system */
	if(HAVE_OPT(FILELOG)) {
		init_log(OPT_ARG(FILELOG));
	}

	if(HAVE_OPT(KOD)) {
		kod_init_kod_db(OPT_ARG(KOD));
	}

	kod_init_kod_db("test.kod");



	/* Considering employing a variable that prevents functions of doing anything until 
	 * everything is initialized properly 
	 */
	struct addrinfo **resh = (struct addrinfo **) malloc(sizeof(struct addrinfo **));

	int resc = resolve_hosts(argv, argc, resh);
	
	if(resc < 1) {
		/* Error! Network down? */
	}

	/* Select a certain ntp server according to simple criteria? For now
	 * let's just pay attention to previous KoDs.
	 */
	int sync_data_suc = 0;
	for(c=0; c<resc && !sync_data_suc; c++) {
		if(!kod_entry_exists(inet_ntoa(resh[c]->ai_addr))) {
			int ow_ret = on_wire(resh[c]);
			sync_data_suc = 1;
		}
		else {
			struct kod_entry **reason;
			int kodc = search_entry(resh[c], reason);

			printf("KoD package exists for %s, stopping any further communication.", inet_ntoa(resh[c]->ai_addr));

		}
	}

	return 0;
}

int
on_wire (
		struct addrinfo *host
					)
{
	SOCKET sock;

	struct pkt *x_pkt = (struct pkt *) malloc(sizeof(struct pkt));
	struct pkt *r_pkt = (struct pkt *) malloc(sizeof(struct pkt));

	struct timeval tv_t1, tv_t2;
	struct timezone tz;
	l_fp t1, t4;

	int error = GETTIMEOFDAY(&tv_t1, &tz);

#ifdef DEBUG
	printf("tv_t1 sec: %i msec: %i\n", tv_t1.tv_sec, tv_t1.tv_usec);
#endif

	TVTOTS(&tv_t1, &(x_pkt->org));

/* Data from a valid timestamp */
/*	unsigned char *tptr = &(x_pkt->org);

	tptr[0] = 0xcc;
	tptr[1] = 0x44;
	tptr[2] = 0x87;
	tptr[3] = 0x45;*/

	x_pkt->stratum = 15;
	x_pkt->ppoll = 8;
	x_pkt->li_vn_mode = 0x1b;


	
	/*in_addr_t foo = inet_addr("127.0.0.1"); */
	create_socket(&sock, host->ai_addr);

	int rsock = sendpkt(sock, host->ai_addr, x_pkt, 48);

	int rpktl = recvpkt(sock, r_pkt, x_pkt);

	close_socket(sock);

	NTOHL_FP(&(r_pkt->rec), &(r_pkt->rec));
	TSTOTV(&(r_pkt->rec), &tv_t2);

	double time = 0.0;

	LFPTOD(&(r_pkt->rec), time);

	printf("\nrec: %d\n", time);



	return 0;
}









	






