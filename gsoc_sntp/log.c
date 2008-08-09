/* Check for isc/binds logging facility, probably interesting, probably worth adapting */
#include "log.h"
#include "sntp-opts.h"

int init = 0;
int filelog = 0;

FILE *log_file;


void log_msg(char *message, char type) {
	if(filelog) {
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
	if(filelog) {
		fprintf(stderr, message);
	}
	else {
		syslog(LOG_DEBUG | LOG_PERROR, message);
	}
}

void init_log(char *logfile) {
	log_file = fopen(logfile, "a");
	
	if(log_file == NULL) {
		filelog = 0;

		char error_msg[80];

		snprintf(error_msg, 80, "init_log(): Cannot open logfile %s", logfile);

		debug_msg(error_msg);

		return;
	}
	else {
		filelog = 1;
		init = 1;
		atexit(cleanup_log);
	}
}

void cleanup_log(void) {
	init = 0;
	fflush(log_file);
	fclose(log_file);
}
