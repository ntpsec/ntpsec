/* Check for isc/binds logging facility, probably interesting, probably worth adapting */
#include "log.h"
#include "sntp-opt.h"

void log_msg(char *message, int type) {
	if(HAVE_OPT(FILELOG)) {
		if(init) {
			fprintf(log_file, message);
		}
		else {
			fprintf(stderr, "Error: Log not initialized!\n");
		}
	}
	else {
		switch(type) {
			case 0:
				type = LOG_DEBUG;
				break;
			
			case 1:
				type = LOG_WARNING;
				break;
	
			case 2: 
				type = LOG_WARNING | LOG_CONS;
				break;
		}

		syslog(type, message);
	}
}

void debug_msg(char *message) {
	if(HAVE_OPT(FILELOG)) {
		fprintf(stderr, message);
	}
	else {
		syslog(LOG_DEBUG | LOG_PERROR, message);
	}
}

void init_log(char *logfile) {
	log_file = fopen(logfile, "a");
	init = 1;
	atexit(cleanup_log);
}

void cleanup_log(void) {
	init = 0;
	fflush(log_file);
	fclose(log_file);
}
