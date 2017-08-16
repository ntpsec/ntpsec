/*
 * Created: Sun Jul 13 09:12:02 1997
 *
 * Copyright (c) 1997-2005 by Frank Kardel <kardel <AT> ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "config.h"

#include <stdio.h>
#include "ntp_stdlib.h"
#include "ntp_fp.h"
#include "ieee754io.h"

static uint64_t get_byte (unsigned char *, offsets_t, int *);

#ifdef DEBUG_PARSELIB

#include "lib_strbuf.h"

static char *
fmt_blong(
          uint64_t val,
          int cnt
          )
{
    char *buf, *s;
    int i = cnt;

    val <<= 32 - cnt;
    buf = lib_getbuf();
    s = buf;

    while (i--) {
        if (val & 0x80000000) {
            *s++ = '1';
        } else {
            *s++ = '0';
        }
        val <<= 1;
    }
    *s = '\0';
    return buf;
}

static char *
fmt_flt(
        bool sign,
        uint64_t ml,
        uint64_t ch,
        int length
        )
{
        char *buf;

        buf = lib_getbuf();
        if ( 8 == length ) {
            snprintf(buf, LIB_BUFLENGTH, "%c %s %s %s", sign ? '-' : '+',
                     fmt_blong(ch, 11),
                     fmt_blong(ml >> 32, 20),
                     fmt_blong(ml & 0x0FFFFFFFFULL, 32));
        } else {
            snprintf(buf, LIB_BUFLENGTH, "%c %s %s", sign ? '-' : '+',
                     fmt_blong(ch, 8),
                     fmt_blong(ml, 23));
        }

        return buf;
}

static char *
fmt_hex(
        unsigned char *bufp,
        int length
        )
{
        char *  buf;
        char    hex[4];
        int     i;

        buf = lib_getbuf();
        buf[0] = '\0';
        for (i = 0; i < length; i++) {
                snprintf(hex, sizeof(hex), "%02x", bufp[i]);
                strlcat(buf, hex, LIB_BUFLENGTH);
        }

        return buf;
}

#endif

static uint64_t
get_byte(
         unsigned char *bufp,
         offsets_t offset,
         int *fieldindex
         )
{
    unsigned char val;

    val = *(bufp + offset[*fieldindex]);
    (*fieldindex)++;
    return val;
}

/*
 * make conversions to and from external IEEE754 formats and internal
 * NTP FP format.
 */
int
fetch_ieee754(
              unsigned char **buffpp,
              int size,
              l_fp *lfpp,
              offsets_t offsets
              )
{
    unsigned char *bufp = *buffpp;
    bool sign;
    int bias;                       /* bias 127 or 1023 */
    int maxexp;
    int mbits;                      /* length of mantissa, 23 or 52 */
    uint64_t mantissa;              /* mantissa, 23 or 52 bits used, +1 */
    int characteristic;             /* biased exponent, 0 to 255 or 2047 */
    int exponent;                   /* unbiased exponent */
    int maxexp_lfp;                 /* maximum exponent that fits in an l_fp */
    unsigned char val;
    int fieldindex = 0;             /* index into bufp */
    int fudge;                      /* shift difference of l_fp and IEEE */
    int shift;                      /* amount to shift IEEE to get l_fp */


    *lfpp = 0;          /* return zero for all errors: NAN, +INF, -INF, etc. */

    /* fetch sign byte & first part of characteristic */
    val = (unsigned char)get_byte(bufp, offsets, &fieldindex);

    sign = (val & 0x80) != 0;
    characteristic = (val & 0x7F);

    /* fetch rest of characteristic and start of mantissa */
    val = (unsigned char)get_byte(bufp, offsets, &fieldindex);

    switch (size) {
    case IEEE_DOUBLE:
        fudge = -20;
        maxexp_lfp = 31;
        mbits  = 52;
        bias   = 1023;
        maxexp = 2047;
        characteristic <<= 4;
        /* grab lower characteristic bits */
        characteristic  |= (val & 0xF0) >> 4;

        mantissa  = (val & 0x0FULL) << 48;
        mantissa |= get_byte(bufp, offsets, &fieldindex) << 40;
        mantissa |= get_byte(bufp, offsets, &fieldindex) << 32;

        mantissa |= get_byte(bufp, offsets, &fieldindex) << 24;
        mantissa |= get_byte(bufp, offsets, &fieldindex) << 16;
        mantissa |= get_byte(bufp, offsets, &fieldindex) << 8;
        mantissa |= get_byte(bufp, offsets, &fieldindex);
        break;

    case IEEE_SINGLE:
        fudge = 9;
        maxexp_lfp = 30;
        mbits  = 23;
        bias   = 127;
        maxexp = 255;
        characteristic <<= 1;
        /* grab last characteristic bit from 2nd byte */
        characteristic |= (val & 0x80) ? 1 : 0 ;

        mantissa   = (val & 0x7FU) << 16;
        mantissa  |= get_byte(bufp, offsets, &fieldindex) << 8;
        mantissa  |= get_byte(bufp, offsets, &fieldindex);
        break;

    default:
        return IEEE_BADCALL;
    }

    exponent = characteristic - bias;
    shift = exponent + fudge;

#ifdef DEBUG_PARSELIB
    if ( debug > 4) { /* SPECIAL DEBUG */
        int length = 8;
        if ( IEEE_SINGLE == size ) {
            length = 4;
        }

        printf("\nfetchieee754: FP: %s -> %s\n", fmt_hex(*buffpp, length),
               fmt_flt(sign, mantissa, characteristic, length));
        printf("fetchieee754: Char: %d, Exp: %d, mbits %d, shift %d\n",
               characteristic, exponent, mbits, shift);
    }
#endif

    *buffpp += fieldindex;

    /* detect funny numbers */
    if (characteristic == maxexp) {
        /* NaN or Infinity */
        if (mantissa) {
            /* NaN */
            return IEEE_NAN;
        }
        /* +Inf or -Inf */
        return sign ? IEEE_NEGINFINITY : IEEE_POSINFINITY;
    }

    /* check for overflows */
    if (exponent > maxexp_lfp) {
        /*
        * sorry an l_fp only so long
        * overflow only in respect to NTP-FP representation
        */
        return sign ? IEEE_NEGOVERFLOW : IEEE_POSOVERFLOW;
    }

    if (characteristic == 0) {
        /* de-normalized or tiny number - fits only as 0 */
        return IEEE_OK;
    }

    /* build the real number */

    /* add in implied 1 */
    mantissa  |= 1ULL << mbits;

    if ( 0 == shift ) {
        /* no shift */
        *lfpp = mantissa;
    } else if ( 0 > shift ) {
        /* right shift */
        *lfpp = mantissa >> -shift;
    } else {
        /* left shift */
        *lfpp = mantissa << shift;
    }

    /* adjust for sign */
    if (sign) {
      L_NEG(*lfpp);
    }

    return IEEE_OK;
}
