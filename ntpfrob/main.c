/* Copyright 2015 by the NTPsec project contributors
   SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "config.h"

/*
 * Our methods, one per linked module
 */
extern void ppscheck(char *device);
extern void tickadj(const int tick);
extern void jitter(void);
extern void stepback(void);
extern void precision(void);

int
main(int argc, char **argv)
{
	int ch;
	while ((ch = getopt(argc, argv, "aA:cp:")) != EOF) {
		switch (ch) {
		case 'A':
#ifdef HAVE_ADJTIMEX
		    tickadj(0);
#else
		    fputs("ntpfrob: no adjtimex(2) call.\n", stderr);
		    exit(0);
#endif
		    break;
		case 'a':
#ifdef HAVE_ADJTIMEX
		    tickadj(atoi(optarg));
#else
		    fputs("ntpfrob: no adjtimex(2) call.\n", stderr);
		    exit(0);
#endif
		    break;
		case 'c':
		    jitter();
		    exit(0);
		    break;
		case 'p':
#ifdef HAVE_SYS_TIMEPPS_H
		    ppscheck(optarg);
#else
		    fputs("ntpfrob: no PPS kernel interface.\n", stderr);
		    exit(0);
#endif
		    break;
		}
	}

    exit(0);
}
