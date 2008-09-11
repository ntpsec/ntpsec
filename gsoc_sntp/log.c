/*
 * Copyright (C) 2008  Johannes Maximilian Kühn
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "log.h"
#include "sntp-opts.h"

int init = 0;
int filelog = 0;

FILE *log_file;


void log_msg(char *message, char type) {
	if(init) {
		time_t cur_time = time(NULL);
		char *timestamp = ctime(&cur_time);

		fprintf(log_file, "%s: %s\n", timestamp, message);
		fflush(log_file);
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
		time_t cur_time = time(NULL);
		char *timestamp = ctime(&cur_time);

		fprintf(stderr, "%s: %s\n", timestamp, message);
	}
	else {
		syslog(LOG_DEBUG
#ifdef LOG_PERROR
			| LOG_PERROR
#endif
			| LOG_CONS, message);
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
