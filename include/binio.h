/*
 * Created: Sun Jul 20 13:03:05 1997
 *
 * Copyright (c) 1997-2005 by Frank Kardel <kardel <AT> ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-clause
 */
#ifndef GUARD_BINIO_H
#define GUARD_BINIO_H

#include "ntp_stdlib.h"

int16_t get_lsb_int16 (unsigned char **);
void put_lsb_uint16 (unsigned char **, uint16_t);
int32_t get_lsb_int32 (unsigned char **);

#define get_lsb_uint16( _x_ )  ((uint16_t) get_lsb_int16( _x_ ))
#define get_lsb_uint32( _x_ )  ((uint32_t) get_lsb_int32( _x_ ))

unsigned short get_msb_ushort(unsigned char *);
short getmsb_short(unsigned char *);
long get_msb_short(unsigned char **);

#endif
/*
 * History:
 *
 * binio.h,v
 * Revision 4.5  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 4.4  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.2  1998/06/28 16:52:15  kardel
 * added binio MSB prototypes for {get,put}_msb_{short,long}
 *
 * Revision 4.1  1998/06/12 15:07:40  kardel
 * fixed prototyping
 *
 * Revision 4.0  1998/04/10 19:50:38  kardel
 * Start 4.0 release version numbering
 *
 * Revision 1.1  1998/04/10 19:27:32  kardel
 * initial NTP VERSION 4 integration of PARSE with GPS166 binary support
 *
 * Revision 1.1  1997/10/06 20:55:37  kardel
 * new parse structure
 *
 */
