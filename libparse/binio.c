/*
 * Created: Sun Jul 20 12:55:33 1997
 *
 * Copyright (c) 1997-2005 by Frank Kardel <kardel <AT> ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "config.h"
#include "binio.h"

int16_t
get_lsb_int16(
	unsigned char **bufpp
	)
{
  int16_t retval;

  retval  = *((*bufpp)++);
  retval |= *((*bufpp)++) << 8;

  return retval;
}

void
put_lsb_uint16(
	unsigned char **bufpp,
	uint16_t val
	)
{
  *((*bufpp)++) = (unsigned char) (val        & 0xFF);
  *((*bufpp)++) = (unsigned char) ((val >> 8) & 0xFF);
}

int32_t
get_lsb_int32(
	unsigned char **bufpp
	)
{
  int32_t retval;

  retval  = *((*bufpp)++);
  retval |= *((*bufpp)++) << 8;
  retval |= *((*bufpp)++) << 16;
  retval |= *((*bufpp)++) << 24;

  return retval;
}

unsigned short
get_msb_ushort(
	 unsigned char *p
	 )
{
	return (unsigned short) get_msb_short(&p);
}

short
getmsb_short(
	 unsigned char *p
	 )
{
	return (short) get_msb_short(&p);
}

long
get_msb_short(
	unsigned char **bufpp
	)
{
  long retval;

  retval  = *((*bufpp)++) << 8;
  retval |= *((*bufpp)++);

  return (retval & 0x8000) ? (~0xFFFF | retval) : retval;
}

/*
 * binio.c,v
 * Revision 4.2  1999/02/21 12:17:34  kardel
 * 4.91f reconciliation
 *
 * Revision 4.1  1998/06/28 16:47:50  kardel
 * added {get,put}_msb_{short,long} functions
 *
 * Revision 4.0  1998/04/10 19:46:16  kardel
 * Start 4.0 release version numbering
 *
 * Revision 1.1  1998/04/10 19:27:46  kardel
 * initial NTP VERSION 4 integration of PARSE with GPS166 binary support
 *
 * Revision 1.1  1997/10/06 21:05:46  kardel
 * new parse structure
 *
 */
