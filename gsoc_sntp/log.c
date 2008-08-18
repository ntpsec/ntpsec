/* Add timestamps to file logging!!! */
#include "log.h"
#include "sntp-opts.h"

int init = 0;
int filelog = 0;

FILE *log_file;


void log_msg(char *message, char type) {
	if(init) {
		fprintf(log_file, message);
	}
	else {
		switch(type) {
			case 0:
				type = LOG_CONS;
				break;
			
			case 1:
				type = LOG_DEBUG | LOG_CONS;
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
		fprintf(stderr, "\n");
	}
	else {
		syslog(LOG_DEBUG | LOG_PERROR | LOG_CONS, message);
	}
}

void init_log(char *logfile) {
	printf("INIT IST KRIEEEEEG %s!!!\n", logfile);
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
