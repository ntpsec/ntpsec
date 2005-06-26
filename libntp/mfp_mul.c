/*
 * /src/NTP/ntp4-dev/libntp/mfp_mul.c,v 4.7 2005/04/16 17:32:10 kardel RELEASE_20050508_A
 *
 * mfp_mul.c,v 4.7 2005/04/16 17:32:10 kardel RELEASE_20050508_A
 *
 * $Created: Sat Aug 16 20:35:08 1997 $
 *
 * Copyright (c) 1997-2005 by Frank Kardel <kardel <AT> ntp.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#include <stdio.h>
#include "ntp_stdlib.h"
#include "ntp_types.h"
#include "ntp_fp.h"

#define LOW_MASK  (u_int32)((1<<(FRACTION_PREC/2))-1)
#define HIGH_MASK (u_int32)(LOW_MASK << (FRACTION_PREC/2))

void
mfp_mul(
	int32   *o_i,
	u_int32 *o_f,
	int32    a_i,
	u_int32  a_f,
	int32    b_i,
	u_int32  b_f
	)
{
  int32 i, j;
  u_int32  f;
  u_long a[4];			/* operand a */
  u_long b[4];			/* operand b */
  u_long  c[4];			/* result c */
  
  int neg = 0;

  if (a_i < 0)			/* examine sign situation */
    {
      neg = 1;
      M_NEG(a_i, a_f);
    }

  if (b_i < 0)			/* examine sign situation */
    {
      neg = !neg;
      M_NEG(b_i, b_f);
    }
  
  a[0] = a_f & LOW_MASK;	/* prepare a operand */
  a[1] = (a_f & HIGH_MASK) >> (FRACTION_PREC/2);
  a[2] = a_i & LOW_MASK;
  a[3] = (a_i & HIGH_MASK) >> (FRACTION_PREC/2);
  
  b[0] = b_f & LOW_MASK;	/* prepare b operand */
  b[1] = (b_f & HIGH_MASK) >> (FRACTION_PREC/2);
  b[2] = b_i & LOW_MASK;
  b[3] = (b_i & HIGH_MASK) >> (FRACTION_PREC/2);

  c[0] = c[1] = c[2] = c[3] = 0;

  for (i = 0; i < 4; i++)	/* we do assume 32 * 32 = 64 bit multiplication */
    for (j = 0; j < 4; j++)
      {
	u_long result_low, result_high;
	
	result_low = (u_long)a[i] * (u_long)b[j];	/* partial product */

	if ((i+j) & 1)		/* splits across two result registers */
	  {
	    result_high   = result_low >> (FRACTION_PREC/2);
	    result_low  <<= FRACTION_PREC/2;
	  }
	else
	  {			/* stays in a result register - except for overflows */
	    result_high = 0;
	  }
	
	if (((c[(i+j)/2] >> 1) + (result_low >> 1)) & (u_int32)((unsigned)1<<(FRACTION_PREC - 1)))
	  result_high++;	/* propagate overflows */

	c[(i+j)/2]   += result_low; /* add up partial products */

	if (((c[(i+j+1)/2] >> 1) + (result_high >> 1)) & (u_int32)((unsigned)1<<(FRACTION_PREC - 1)))
	  c[1+(i+j)/2]++;	/* propagate overflows */

	c[(i+j+1)/2] += result_high;
      }

#ifdef DEBUG
  if (debug > 6)
    printf("mfp_mul: 0x%04lx%04lx%04lx%04lx * 0x%04lx%04lx%04lx%04lx = 0x%08lx%08lx%08lx%08lx\n",
	 a[3], a[2], a[1], a[0], b[3], b[2], b[1], b[0], c[3], c[2], c[1], c[0]);
#endif

  if (c[3])			/* overflow */
    {
      i = ((unsigned)1 << (FRACTION_PREC-1)) - 1;
      f = ~(unsigned)0;
    }
  else
    {				/* take produkt - discarding extra precision */
      i = c[2];
      f = c[1];
    }
  
  if (neg)			/* recover sign */
    {
      M_NEG(i, f);
    }

  *o_i = i;
  *o_f = f;

#ifdef DEBUG
  if (debug > 6)
    printf("mfp_mul: %s * %s => %s\n",
	   mfptoa((u_long)a_i, a_f, 6),
	   mfptoa((u_long)b_i, b_f, 6),
	   mfptoa((u_long)i, f, 6));
#endif
}

/*
 * History:
 *
 * mfp_mul.c,v
 * Revision 4.7  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 4.6  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.3  1999/02/21 12:17:37  kardel
 * 4.91f reconcilation
 *
 * Revision 4.2  1998/12/20 23:45:28  kardel
 * fix types and warnings
 *
 * Revision 4.1  1998/05/24 07:59:57  kardel
 * conditional debug support
 *
 * Revision 4.0  1998/04/10 19:46:38  kardel
 * Start 4.0 release version numbering
 *
 * Revision 1.1  1998/04/10 19:27:47  kardel
 * initial NTP VERSION 4 integration of PARSE with GPS166 binary support
 *
 * Revision 1.1  1997/10/06 21:05:46  kardel
 * new parse structure
 *
 */
