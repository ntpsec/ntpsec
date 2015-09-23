#include <config.h>

#include "main.h"

const char* Version = NTPS_VERSION_STRING " " __DATE__ " " __TIME__;

int 
main (
	int	argc,
	char **	argv
	) 
{
	return sntp_main(argc, argv, Version);
}
