/* Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>

#include "config.h"

/*
 * Our methods, one per linked module
 */
extern void ppscheck(char *device);
extern void tickadj(const bool json, const int tick);
extern void jitter(const bool json);
extern void stepback(void);
extern void precision(const bool json);

int
main(int argc, char **argv)
{
	int ch;
	bool json = false;
	while ((ch = getopt(argc, argv, "a:Acejp:")) != EOF) {
		switch (ch) {
		case 'A':
#ifdef HAVE_ADJTIMEX
		    tickadj(json, 0);
#else
		    fputs("ntpfrob: no adjtimex(2) call.\n", stderr);
		    exit(0);
#endif
		    break;
		case 'a':
#ifdef HAVE_ADJTIMEX
		    tickadj(json, atoi(optarg));
#else
		    fputs("ntpfrob: no adjtimex(2) call.\n", stderr);
		    exit(0);
#endif
		    break;
		case 'c':
		    jitter(json);
		    exit(0);
		    break;
		case 'e':
		    precision(json);
		    exit(0);
		    break;
		case 'j':
		    json = true;
		    break;
		case 'p':
#ifdef HAVE_SYS_TIMEPPS_H
		    ppscheck(optarg);
#else
		    fputs("ntpfrob: no PPS kernel interface.\n", stderr);
		    exit(0);
#endif
		    break;
		default:
		    fputs("ntpfrob: no mode option specified.\n", stderr);
		    exit(1);
		    break;
		}
	}

    exit(0);
}
