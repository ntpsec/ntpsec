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
  unsigned long characteristic;    /* biased exponent */
  long exponent;                   /* unbiased exponent */
  unsigned int maxexp_lfp;         /* maximum exponent that fits in an l_fp */
#ifdef DEBUG_PARSELIB
  int length;
#endif
  unsigned char val;
  int fieldindex = 0;              /* index into bufp */
  

  *lfpp = 0;           /* return zero for all errors: NAN, +INF, -INF, etc. */

  /* fetch sign byte & first part of characteristic */
  val = get_byte(bufp, offsets, &fieldindex);

  sign     = (val & 0x80) != 0;
  characteristic = (val & 0x7F);

  /* fetch rest of characteristic and start of mantissa */
  val = get_byte(bufp, offsets, &fieldindex);

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

    case IEEE_SINGLE:
#ifdef DEBUG_PARSELIB
      length = 4;
#endif
      maxexp_lfp = 127;
      mbits  = 23;
      bias   = 127;
      maxexp = 255;
      characteristic <<= 1;
      characteristic  |= (val & 0x80) != 0; /* grab last characteristic bit */

      mantissa_high  = 0;

      mantissa_low   = (val &0x7F) << 16;
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
