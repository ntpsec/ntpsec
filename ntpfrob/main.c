/* Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>

#include "config.h"
#include "ntpfrob.h"

static void usage(void);

static void usage(void)
{
	fputs(
"usage:\n"
"ntpfrob [ -A] [-b bump] [ -a tick ] [ -p ppsdev ] [-c] [-e] [-r] {-h] [-?]\n"
"        -a tick    Set kernel tick\n"
"        -A         Display kernel tick\n"
"        -b bump    Bump clock by specified microseconds.\n"
"        -c         Compute and display clock jitter.\n"
"        -e         Measure clock precision.\n"
"        -j         Report in self-describing JSON.\n"
"        -p ppsdev  Look for PPS pulses on a specified device\n"
"        -r         Raw mode. Only applies to the jitter mode.\n"
"        -?         Print usage\n"
"        -h         Print usage\n"
    , stderr);

}

int
main(int argc, char **argv)
{
	int ch;
        bool got_one = false;
	iomode mode = plain_text;
	while ((ch = getopt(argc, argv, "a:Ab:cejp:rh?")) != EOF) {
                got_one = true;
		switch (ch) {
		case 'A':
#ifdef HAVE_ADJTIMEX
		    tickadj(mode==json, 0);
#else
		    fputs("ntpfrob: no adjtimex(2)\n", stderr);
#endif /* HAVE_ADJTIMEX */
		    break;
		case 'a':
#ifdef HAVE_ADJTIMEX
		    tickadj(mode, atoi(optarg));
#else
		    fputs("ntpfrob: no adjtimex(2)\n", stderr);
#endif /* HAVE_ADJTIMEX */
		    break;
		case 'b':
		    bumpclock(atoi(optarg));
		    break;
		case 'c':
		    jitter(mode);
                    /* never returns */
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
                    /* never returns */
#else
		    fputs("ntpfrob: no PPS kernel interface.\n", stderr);
		    exit(0);
#endif
		    break;
		case 'r':
		    mode = raw;
		    break;
		default:
		    fputs("ntpfrob: no mode option specified.\n", stderr);
                    /* fall through */
                case 'h':
                case '?':
                    usage();
		    exit(1);
		    break;
		}
	}
        if ( !got_one ) {
	    fputs("ntpfrob: no mode option specified.\n", stderr);
	    usage();
	    exit(1);
        }

    exit(0);
}
