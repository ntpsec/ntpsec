/*
 * Created: Sun Jun 28 16:30:38 1998
 *
 * Copyright (c) 1998-2005 by Frank Kardel <kardel@ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "config.h"
#include "ntp_fp.h"
#include "ntp_calendar.h"
#include "parse.h"

#define GPSORIGIN       2524953600u  /* GPS origin - NTP origin in seconds */

void
gpstolfp(
	 int weeks,
	 int days,
	 unsigned long  seconds,
	 l_fp * lfp
	 )
{
  if (weeks < GPSWRAP)
    {
      weeks += GPSWEEKS;
    }

   /* convert to NTP time, note no fractional seconds */
  *lfp = lfptouint((uint64_t)weeks * SECSPERWEEK
                   + (uint64_t)days * SECSPERDAY
                   + (uint64_t)seconds
                   + GPSORIGIN);
  setlfpfrac(*lfp, 0);
}

/*
 * History:
 *
 * gpstolfp.c,v
 * Revision 4.8  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 4.7  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.3  1999/02/28 11:42:44  kardel
 * (GPSWRAP): update GPS rollover to 990 weeks
 *
 * Revision 4.2  1998/07/11 10:05:25  kardel
 * Release 4.0.73d reconciliation
 *
 * Revision 4.1  1998/06/28 16:47:15  kardel
 * added gpstolfp() function
 */
