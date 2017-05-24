/*
 * Copyright (c) 1989-2005 by Frank Kardel <kardel@ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-clause
 */

#ifndef GUARD_PARSE_CONF_H
#define GUARD_PARSE_CONF_H


/*
 * field location structure
 */
#define O_DAY   0
#define O_MONTH 1
#define O_YEAR  2
#define O_HOUR  3
#define O_MIN   4
#define O_SEC   5
#define O_WDAY  6
#define O_FLAGS 7
#define O_ZONE  8
#define O_UTCHOFFSET 9
#define O_UTCMOFFSET 10
#define O_UTCSOFFSET 11
#define O_COUNT (O_UTCSOFFSET+1)


/*
 * see below for field offsets
 */

struct format
{
  struct foff
    {
      unsigned short offset;		/* offset into buffer */
      unsigned short length;		/* length of field */
    }         field_offsets[O_COUNT];
  const unsigned char *fixed_string;		/* string with must be chars (blanks = wildcards) */
  unsigned long      flags;
};
#endif

/*
 * History:
 *
 * parse_conf.h,v
 * Revision 4.7  2005/06/25 10:58:45  kardel
 * add missing log keywords
 *
 */
