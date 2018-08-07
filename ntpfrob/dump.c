/* Copyright 2018 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "config.h"
#include "ntpfrob.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/timex.h>

const char * verbose[] = {"Clock synchronized, no leap second adjustment pending.",
			"A leap second will be added at the end of the UTC day.",
			"A leap second will be deleted at the end of the UTC day.",
			"Insertion of a leap second is in progress.",
			"A leap-second insertion or deletion has been completed.",
			"The system clock is not synchronized to a reliable server."};
const char * adjkey[] = {"time offset (ns)", "frequency offset", "error max (us)",
			"error est (us)", "clock cmd status", "pll constant",
			"clock precision (us)", "clock tolerance", "tick (us)",
			"PPS frequency", "PPS jitter (ns)", "PPS interval (s)",
			"PPS stability", "PPS jitter limit exceed", "PPS calibration intervals",
			"PPS calibration errors", "PPS stabilitiy exceed", "TAI offset"};

void do_dump(const iomode mode) {
	struct timex txc;
	long adjval[18];
	int out, i, v;

	txc.modes = 0;
	out = adjtimex(&txc);

	switch(out){
		case -1: // An error
			perror("dump.c:31 adjtimex(&txc); ");
			exit(-1);
			break;
		case TIME_OK:
			v = 0;
			break;
		case TIME_INS:
			v = 1;
			break;
		case TIME_DEL:
			v = 2;
			break;
		case TIME_OOP:
			v = 3;
			break;
		case TIME_WAIT:
			v = 4;
			break;
		case TIME_ERROR:
			v = 5;
			break;
		default:
			exit(-2);
	}
	adjval[0]  = txc.offset;	adjval[1]  = txc.freq;
	adjval[2]  = txc.maxerror;	adjval[3]  = txc.esterror;
	adjval[4]  = txc.status;	adjval[5]  = txc.constant;
	adjval[6]  = txc.precision;	adjval[7]  = txc.tolerance;
	adjval[8]  = txc.tick;		adjval[9]  = txc.ppsfreq;
	adjval[10] = txc.jitter;	adjval[11] = txc.shift;
	adjval[12] = txc.stabil;	adjval[13] = txc.jitcnt;
	adjval[14] = txc.calcnt;	adjval[15] = txc.errcnt;
	adjval[16] = txc.stbcnt;	adjval[17] = txc.tai;

	if (mode == json) {
		printf("{\"time\":%ld.%09ld, \"verbose\": \"%s\"", txc.time.tv_sec, txc.time.tv_usec, verbose[v]);
		for(i=0;i<18;i++){
			printf(", \"%s\": %ld", adjkey[i], adjval[i]);
		}
		puts("}\n");
	} else {
		printf("time = %ld.%09ld\n", txc.time.tv_sec, txc.time.tv_usec);
		printf("verbose = \"%s\"\n", verbose[v]);
		for(i=0;i<18;i++){
			printf("%s = %ld\n", adjkey[i], adjval[i]);
		}
	}
	exit(0);
}
