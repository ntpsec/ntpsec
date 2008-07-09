#ifndef LOG_H
#define LOG_H

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <syslog.h>
#include <stdarg.h>

void log_msg(char *message, char type);
void debug(char *message);

char init = 0;
FILE *log_file;

void init_log(char *logfile);
void cleanup_log(void);

#endif
