/* Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>

#include "config.h"
#include "ntpfrob.h"

void init_lib(void);

int
main(int argc, char **argv)
{
	int ch;
	iomode mode = plain_text;
	init_lib();
	while ((ch = getopt(argc, argv, "a:Acejp:rs")) != EOF) {
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
		    tickadj(mode, atoi(optarg));
#else
		    fputs("ntpfrob: no adjtimex(2) call.\n", stderr);
		    exit(0);
#endif
		    break;
		case 'c':
		    jitter(mode);
		    exit(0);
		    break;
		case 'e':
		    precision(mode);
		    exit(0);
		    break;
		case 'j':
		    mode = json;
		    break;
		case 'p':
#ifdef HAVE_SYS_TIMEPPS_H
		    ppscheck(optarg);
#else
		    fputs("ntpfrob: no PPS kernel interface.\n", stderr);
		    exit(0);
#endif
		    break;
		case 'r':
		    mode = raw;
		    break;
		case 's':
		    stepback();
		    break;
		default:
		    fputs("ntpfrob: no mode option specified.\n", stderr);
		    exit(1);
		    break;
		}
	}

    exit(0);
}
