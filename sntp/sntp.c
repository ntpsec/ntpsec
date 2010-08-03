#include "main.h"

char *progname = "sntp";	/* for msyslog */
volatile int debug;

int 
main (
	int argc,
	char **argv
	) 
{
	return sntp_main(argc, argv);
}
