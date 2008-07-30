#if 0
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#include <ntp_stdlib.h>

#include <sntp-opts.h>

#include "networking.h"
#include "header.h"

#define NTP_PORT 123

int ai_fam_tmpl;

volatile int debug;


int 
main (
		char **argv, 
		int argc
		) 
{
	return sntp_main(argv, argc);
}

int  
sntp_main (
		char **argv, 
		int argc
		) 
{
	register int c;

	if (isc_net_probeipv6() != ISC_R_SUCCESS) {
		ai_fam_templ = AF_INET;
	}

	int optct = optionProcess( &sntpOptions, argc, argv );
	argc -= optct;
	argv += optct;

	/* Parse config file if declared TODO */

	/* Initialize logging system */
	if(HAVE_OPT(FILELOG)) {
		init_log(OPT_ARG(FILELOG));
	}

	/* Considering employing a variable that prevents functions of doing anything until 
	 * everything is initialized properly 
	 */
	struct addrinfo **resh;

	int resc = resolve_hosts(argv, argc, resh);
	
	if(resc < 0) {
		/* Error! Network down? */
	}

	resc = filter_reachable(resh, resc);

	if(resc < 0) {
		/* All hosts not reachable!? */
	}

	resc = filter_ntp(resh, resc);

	if(resc < 0) {
		/* No valid ntp responses */
	}

	/* Select a certain ntp server according to simple criteria? For now
	 * let's just pay attention to previous KoDs.
	 */

	for(c=0; c<resc && !sync_data_suc; c++) {
		if(!kod_entry_exists(resh[c])) {
			sync_data_suc = on_wire(resh[c]);
		}
		else {
			/* Debug/Log output */
		}
	}

	return 0;
}
