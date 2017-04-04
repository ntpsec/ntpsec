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

static unsigned char get_byte (unsigned char *, offsets_t, int *);
#if defined(DEBUG) && defined(DEBUG_PARSELIB)
static int put_ieee754 (unsigned char **bufpp, int size, l_fp *lfpp,
                        offsets_t offsets);
#endif
#ifdef __UNUSED__
static void put_byte (unsigned char *, offsets_t, int *, unsigned char);
#endif

#ifdef DEBUG_PARSELIB

#include "lib_strbuf.h"

static char *
fmt_blong(
	  unsigned long val,
	  int cnt
	  )
{
  char *buf, *s;
  int i = cnt;

  val <<= 32 - cnt;
  LIB_GETBUF(buf);
  s = buf;
  
  while (i--)
    {
      if (val & 0x80000000)
	{
	  *s++ = '1';
	}
      else
	{
	  *s++ = '0';
	}
      val <<= 1;
    }
  *s = '\0';
  return buf;
}

static char *
fmt_flt(
	unsigned int sign,
	unsigned long mh,
	unsigned long ml,
	unsigned long ch
	)
{
	char *buf;

	LIB_GETBUF(buf);
	snprintf(buf, LIB_BUFLENGTH, "%c %s %s %s", sign ? '-' : '+',
		 fmt_blong(ch, 11),
		 fmt_blong(mh, 20),
		 fmt_blong(ml, 32));

	return buf;
}

static char *
fmt_hex(
	unsigned char *bufp,
	int length
	)
{
	char *	buf;
	char	hex[4];
	int	i;

	LIB_GETBUF(buf);
	buf[0] = '\0';
	for (i = 0; i < length; i++) {
		snprintf(hex, sizeof(hex), "%02x", bufp[i]);
		strlcat(buf, hex, LIB_BUFLENGTH);
	}

	return buf;
}

#endif

static unsigned char
get_byte(
	 unsigned char *bufp,
	 offsets_t offset,
	 int *fieldindex
	 )
{
  unsigned char val;

  val     = *(bufp + offset[*fieldindex]);
#ifdef DEBUG_PARSELIB
  if (debug > 4)
    printf("fetchieee754: getbyte(0x%08x, %d) = 0x%02x\n", (unsigned int)(bufp)+offset[*fieldindex], *fieldindex, val);
#endif
  (*fieldindex)++;
  return val;
}

#ifdef __UNUSED__
static void
put_byte(
	 unsigned char *bufp,
	 offsets_t offsets,
	 int *fieldindex,
	 unsigned char val
	 )
{
  *(bufp + offsets[*fieldindex]) = val;
  (*fieldindex)++;
}
#endif

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
  unsigned int sign;
  unsigned int bias;
  unsigned int maxexp;
  int mbits;
  unsigned long mantissa_low;
  unsigned long mantissa_high;
  unsigned long characteristic;
  long exponent;
  unsigned int maxexp_lfp;  /* maximum exponent that fits in an l_fp */
#ifdef DEBUG_PARSELIB
  int length;
#endif
  unsigned char val;
  int fieldindex = 0;
  

  *lfpp = 0;           /* return zero for all errors: NAN, +INF, -INF, etc. */

  switch (size)
    {
    case IEEE_DOUBLE:
#ifdef DEBUG_PARSELIB
      length = 8;
#endif
      maxexp_lfp = 31;
      mbits  = 52;
      bias   = 1023;
      maxexp = 2047;
      break;

    case IEEE_SINGLE:
#ifdef DEBUG_PARSELIB
      length = 4;
#endif
      maxexp_lfp = 127;
      mbits  = 23;
      bias   = 127;
      maxexp = 255;
      break;

    default:
      return IEEE_BADCALL;
    }
  
  val = get_byte(bufp, offsets, &fieldindex); /* fetch sign byte & first part of characteristic */
  
  sign     = (val & 0x80) != 0;
  characteristic = (val & 0x7F);

  val = get_byte(bufp, offsets, &fieldindex); /* fetch rest of characteristic and start of mantissa */
  
  switch (size)
    {
    case IEEE_SINGLE:
      characteristic <<= 1;
      characteristic  |= (val & 0x80) != 0; /* grab last characteristic bit */

      mantissa_high  = 0;

      mantissa_low   = (val &0x7F) << 16;
      mantissa_low  |= (unsigned long)get_byte(bufp, offsets, &fieldindex) << 8;
      mantissa_low  |= get_byte(bufp, offsets, &fieldindex);
      break;
      
    case IEEE_DOUBLE:
      characteristic <<= 4;
      characteristic  |= (val & 0xF0) >> 4; /* grab lower characteristic bits */

      mantissa_high  = (val & 0x0F) << 16;
      mantissa_high |= (unsigned long)get_byte(bufp, offsets, &fieldindex) << 8;
      mantissa_high |= get_byte(bufp, offsets, &fieldindex);

      mantissa_low   = (unsigned long)get_byte(bufp, offsets, &fieldindex) << 24;
      mantissa_low  |= (unsigned long)get_byte(bufp, offsets, &fieldindex) << 16;
      mantissa_low  |= (unsigned long)get_byte(bufp, offsets, &fieldindex) << 8;
      mantissa_low  |= get_byte(bufp, offsets, &fieldindex);
      break;
      
    default:
      return IEEE_BADCALL;
    }
#ifdef DEBUG_PARSELIB
  if (debug > 4)
  {
    double d;
    float f;

    if (size == IEEE_SINGLE)
      {
	int i;

	for (i = 0; i < length; i++)
	  {
	    *((unsigned char *)(&f)+i) = *(*buffpp + offsets[i]);
	  }
	d = f;
      }
    else
      {
	int i;

	for (i = 0; i < length; i++)
	  {
	    *((unsigned char *)(&d)+i) = *(*buffpp + offsets[i]);
	  }
      }
    
    printf("fetchieee754: FP: %s -> %s -> %e(=%s)\n", fmt_hex(*buffpp, length),
	   fmt_flt(sign, mantissa_high, mantissa_low, characteristic),
	   d, fmt_hex((unsigned char *)&d, length));
  }
#endif

  *buffpp += fieldindex;
  
  /*
   * detect funny numbers
   */
  if (characteristic == maxexp)
    {
      /*
       * NaN or Infinity
       */
      if (mantissa_low || mantissa_high)
	{
	  /*
	   * NaN
	   */
	  return IEEE_NAN;
	}
      else
	{
	  /*
	   * +Inf or -Inf
	   */
	  return sign ? IEEE_NEGINFINITY : IEEE_POSINFINITY;
	}
    }
  else
    {
      /*
       * collect real numbers
       */

      /*
       * check for overflows
       */
      exponent = characteristic - bias;

      if (exponent > maxexp_lfp)	/* sorry an l_fp only so long */
	{
	  /*
	   * overflow only in respect to NTP-FP representation
	   */
	  return sign ? IEEE_NEGOVERFLOW : IEEE_POSOVERFLOW;
	}
      else
	{
	  int frac_offset;	/* where the fraction starts */
	  
	  frac_offset = mbits - exponent;

	  if (characteristic == 0) 
	    {
	      /*
	       * de-normalized or tiny number - fits only as 0
	       */
	      return IEEE_OK;
	    }
	  else
	    {
	      /*
	       * adjust for implied 1
	       */
	      if (mbits > 31)
		mantissa_high |= 1 << (mbits - 32);
	      else
		mantissa_low  |= 1 << mbits;

	      /*
	       * take mantissa apart - if only all machine would support
	       * 64 bit operations 8-(
	       */
	      if (frac_offset > mbits)
		{
		  setlfpuint(*lfpp, 0); /* only fractional number */
		  frac_offset -= mbits + 1; /* will now contain right shift count - 1*/
		  if (mbits > 31)
		    {
		      uint32_t frac;
		      frac   = mantissa_high << (63 - mbits);
		      frac  |= mantissa_low  >> (mbits - 33);
		      frac >>= frac_offset;
		      setlfpfrac(*lfpp, frac);
		    }
		  else
		    {
		      setlfpfrac(*lfpp, mantissa_low >> frac_offset);
		    }
		}
	      else
		{
		  if (frac_offset > 32)
		    {
		      /*
		       * must split in high word
		       */
		      setlfpuint(*lfpp, mantissa_high >> (frac_offset - 32));
		      setlfpfrac(*lfpp, ((mantissa_high & ((1 << (frac_offset - 32)) - 1)) << (64 - frac_offset)) | (mantissa_low  >> (frac_offset - 32)));
		    }
		  else
		    {
		      /*
		       * must split in low word
		       */
			setlfpuint(*lfpp, (mantissa_high << (32 - frac_offset)) | (((mantissa_low >> frac_offset) & ((1 << (32 - frac_offset)) - 1))));
		      /* coverity[large_shift] */
		      setlfpfrac(*lfpp, (mantissa_low & ((1 << frac_offset) - 1)) << (32 - frac_offset));
		    }
		}
	      
	      /*
	       * adjust for sign
	       */
	      if (sign)
		{
		  L_NEG(*lfpp);
		}
	      
	      return IEEE_OK;
	    }
	}
    }
}
  
#if defined(DEBUG) && defined(DEBUG_PARSELIB)
#include <stdlib.h>

static int
put_ieee754(
	    unsigned char **bufpp,
	    int size,
	    l_fp *lfpp,
	    offsets_t offsets
	    )
{
  l_fp outlfp;
#ifdef DEBUG_PARSELIB
  unsigned int sign;
  unsigned int bias;
#endif
/*unsigned int maxexp;*/
  int mbits;
  int msb;
  unsigned long mantissa_low = 0;
  unsigned long mantissa_high = 0;
#ifdef DEBUG_PARSELIB
  unsigned long characteristic = 0;
  long exponent;
#endif
/*int length;*/
  unsigned long mask;

  UNUSED_ARG(bufpp);
  UNUSED_ARG(offsets);
  
  outlfp = *lfpp;

  switch (size)
    {
    case IEEE_DOUBLE:
    /*length = 8;*/
      mbits  = 52;
#ifdef DEBUG_PARSELIB
      bias   = 1023;
#endif
    /*maxexp = 2047;*/
      break;

    case IEEE_SINGLE:
    /*length = 4;*/
      mbits  = 23;
#ifdef DEBUG_PARSELIB
      bias   = 127;
#endif
    /*maxexp = 255;*/
      break;

    default:
      return IEEE_BADCALL;
    }
  
  /*
   * find sign
   */
  if (L_ISNEG(outlfp))
    {
      L_NEG(outlfp);
#ifdef DEBUG_PARSELIB
      sign = 1;
#endif
    }
  else
    {
#ifdef DEBUG_PARSELIB
      sign = 0;
#endif
    }

  if (outlfp == 0)
    {
#ifdef DEBUG_PARSELIB
      exponent = mantissa_high = mantissa_low = 0; /* true zero */
#endif
    }
  else
    {
      /*
       * find number of significant integer bits
       */
      mask = 0x80000000;
      if (lfpuint(outlfp))
	{
	  msb = 63;
	  while (mask && ((lfpuint(outlfp) & mask) == 0))
	    {
	      mask >>= 1;
	      msb--;
	    }
	}
      else
	{
	  msb = 31;
	  while (mask && ((lfpfrac(outlfp) & mask) == 0))
	    {
	      mask >>= 1;
	      msb--;
	    }
	}
  
      switch (size)
	{
	case IEEE_SINGLE:
	  mantissa_high = 0;
	  if (msb >= 32)
	    {
	      mantissa_low  = (lfpuint(outlfp) & ((1 << (msb - 32)) - 1)) << (mbits - (msb - 32));
	      mantissa_low |=  lfpfrac(outlfp) >> (mbits - (msb - 32));
	    }
	  else
	    {
	      mantissa_low  = (lfpfrac(outlfp) << (mbits - msb)) & ((1 << mbits) - 1);
	    }
	  break;
	  
	case IEEE_DOUBLE:
	  if (msb >= 32)
	    {
	      mantissa_high  = (lfpuint(outlfp) << (mbits - msb)) & ((1 << (mbits - 32)) - 1);
	      mantissa_high |=  lfpfrac(outlfp) >> (32 - (mbits - msb));
	      mantissa_low   = (lfpuint(outlfp) & ((1 << (msb - mbits)) - 1)) << (32 - (msb - mbits));
	      /* coverity[negative_shift] */
	      mantissa_low  |=  lfpfrac(outlfp) >> (msb - mbits);
	    }
	  else
	    {
	      mantissa_high  = lfpfrac(outlfp) << (mbits - 32 - msb);
	      mantissa_low   = lfpfrac(outlfp) << (mbits - 32);
	    }
	}

#ifdef DEBUG_PARSELIB
      exponent = msb - 32;
      characteristic = exponent + bias;

      if (debug > 4)
	printf("FP: %s\n", fmt_flt(sign, mantissa_high, mantissa_low, characteristic));
#endif
    }
  return IEEE_OK;
}

int main(
	 int argc,
	 char **argv
	 )
{
  static offsets_t native_off = { 0, 1, 2, 3, 4, 5, 6, 7 };
  double f = 1.0;
  double *f_p = &f;
  l_fp fp;
  
  if (argc == 2)
    {
      if (sscanf(argv[1], "%lf", &f) != 1)
	{
	  printf("cannot convert %s to a float\n", argv[1]);
	  return EXIT_FAILURE;
	}
    }
  
  printf("double: %s %s\n", fmt_blong(*(unsigned long *)&f, 32), fmt_blong(*(unsigned long *)((char *)(&f)+4), 32));
  printf("fetch from %f = %d\n", f, fetch_ieee754((void *)&f_p, IEEE_DOUBLE, &fp, native_off));
  printf("fp [%s %s] = %s\n", fmt_blong(lfpuint(fp), 32), fmt_blong(lfpfrac(fp), 32), mfptoa(lfpuint(fp), lfpfrac(fp), 15));
  f_p = &f;
  put_ieee754((void *)&f_p, IEEE_DOUBLE, &fp, native_off);
  
  return EXIT_SUCCESS;
}

#endif
/*
 * History:
 *
 * ieee754io.c,v
 * Revision 4.12  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 4.11  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.8  1999/02/21 12:17:36  kardel
 * 4.91f reconciliation
 *
 * Revision 4.7  1999/02/21 11:26:03  kardel
 * renamed index to fieldindex to avoid index() name clash
 *
 * Revision 4.6  1998/11/15 20:27:52  kardel
 * Release 4.0.73e13 reconciliation
 *
 * Revision 4.5  1998/08/16 19:01:51  kardel
 * debug information only compile for LIBDEBUG case
 *
 * Revision 4.4  1998/08/09 09:39:28  kardel
 * Release 4.0.73e2 reconciliation
 *
 * Revision 4.3  1998/06/13 11:56:19  kardel
 * disabled putbute() for the time being
 *
 * Revision 4.2  1998/06/12 15:16:58  kardel
 * ansi2knr compatibility
 *
 * Revision 4.1  1998/05/24 07:59:56  kardel
 * conditional debug support
 *
 * Revision 4.0  1998/04/10 19:46:29  kardel
 * Start 4.0 release version numbering
 *
 * Revision 1.1  1998/04/10 19:27:46  kardel
 * initial NTP VERSION 4 integration of PARSE with GPS166 binary support
 *
 * Revision 1.1  1997/10/06 21:05:45  kardel
 * new parse structure
 *
 */
