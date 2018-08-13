/* Copyright 2018 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "config.h"
#include "ntpfrob.h"

#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_ADJTIMEX
#include <sys/timex.h>

const char * verbose[] =
{ "Clock synchronized, no leap second adjustment pending.",
  "A leap second will be added at the end of the UTC day.",
  "A leap second will be deleted at the end of the UTC day.",
  "Insertion of a leap second is in progress.",
  "A leap-second insertion or deletion has been completed.",
  "The system clock is not synchronized to a reliable server.",
  "-30-" };
const char * adjkey[] =
{ "time offset (ns)",
  "TAI offset",
  "frequency offset",
  "error max (us)",
  "error est (us)",
  "clock cmd status",
  "pll constant",
  "clock precision (us)",
  "clock tolerance",
  "tick (us)",
  "PPS frequency",
  "PPS jitter (ns)",
  "PPS interval (s)",
  "PPS stability",
  "PPS jitter limit exceed",
  "PPS calibration intervals",
  "PPS calibration errors",
  "PPS stabilitiy exceed",
  "-30-" };

void
do_dump(const iomode mode, const int force)
{
    struct timex txc;
    long adjval[18];
    int out, i, v, end = 10;

    txc.modes = 0;
    out       = adjtimex(&txc);

    switch (out) {
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

    adjval[0]  = txc.offset;
    adjval[1]  = txc.tai;
    adjval[2]  = txc.freq;
    adjval[3]  = txc.maxerror;
    adjval[4]  = txc.esterror;
    adjval[5]  = txc.status;
    adjval[6]  = txc.constant;
    adjval[7]  = txc.precision;
    adjval[8]  = txc.tolerance;
    adjval[9]  = txc.tick;
    adjval[10] = txc.ppsfreq;
    adjval[11] = txc.jitter;
    adjval[12] = txc.shift;
    adjval[13] = txc.stabil;
    adjval[14] = txc.jitcnt;
    adjval[15] = txc.calcnt;
    adjval[16] = txc.errcnt;
    adjval[17] = txc.stbcnt;

    if (((txc.status | force) & (STA_PPSFREQ | STA_PPSTIME)) != 0) {
        end = 18;
    }

    if (mode == json) {
        printf("{\"time\":%ld.%09ld, \"verbose\": \"%s\"", txc.time.tv_sec,
          txc.time.tv_usec, verbose[v]);
        for (i = 0; i < end; i++) {
            printf(", \"%s\": %ld", adjkey[i], adjval[i]);
        }
        puts("}\n");
    } else {
        printf("time = %ld.%09ld\n", txc.time.tv_sec, txc.time.tv_usec);
        printf("verbose = \"%s\"\n", verbose[v]);
        for (i = 0; i < end; i++) {
            printf("%s = %ld\n", adjkey[i], adjval[i]);
        }
    }
    exit(0);
} /* do_dump */
#endif /* HAVE_ADJTIMEX */
