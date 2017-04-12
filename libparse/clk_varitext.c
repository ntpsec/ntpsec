/*
 * Varitext code variant by A.McConnell 1997/01/19
 *
 * Supports Varitext's Radio Clock
 *
 * Used the Meinberg/Computime clock as a template for Varitext Radio Clock
 *
 * Copyright (c) 1989-2005 by Frank Kardel <kardel@ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "config.h"
#include "ntp_fp.h"
#include "ntp_calendar.h"

#include "parse.h"

# include "ntp_stdlib.h"
# include <stdio.h>

/* Not used.  Comment out to avoid warnings
static const uint8_t VT_INITIALISED      = 0x01;
static const uint8_t VT_SYNCHRONISED     = 0x02;
static const uint8_t VT_ALARM_STATE      = 0x04;
static const uint8_t VT_SEASON_CHANGE    = 0x10;
static const uint8_t VT_LAST_TELEGRAM_OK = 0x20;
*/
static const uint8_t VT_BST              = 0x08;

/*
 * The Varitext receiver sends a datagram in the following format every minute
 *
 * Timestamp	T:YY:MM:MD:WD:HH:MM:SSCRLFSTXXX
 * Pos          0123456789012345678901 2 3 4567
 *              0000000000111111111122 2 2 2222
 * Parse        T:  :  :  :  :  :  :  \r\n
 *
 * T	Startcharacter "T" specifies start of the timestamp
 * YY	Year MM	Month 1-12
 * MD	Day of the month
 * WD	Day of week
 * HH	Hour
 * MM	Minute
 * SS	Second
 * CR	Carriage return
 * LF	Linefeed
 * ST	Status character
 *	Bit 0 - Set= Initialised; Reset=Time Invalid (DO NOT USE)
 *	Bit 1 - Set= Synchronised; Reset= Unsynchronised
 *	Bit 2 - Set= Alarm state; Reset= No alarm
 *	Bit 3 - Set= BST; Reset= GMT
 *	Bit 4 - Set= Seasonal change in approx hour; Reset= No seasonal change expected
 *	Bit 5 - Set= Last MSF telegram was OK; Reset= Last telegram was in error;
 *	Bit 6 - Always set
 *	Bit 7 - Unused
 * XXX	Checksum calculated using Fletcher's method (ignored for now).
 */

static struct format varitext_fmt =
{
  {
    {8, 2},  {5,  2}, {2,  2},	/* day, month, year */
    {14, 2}, {17, 2}, {20, 2},	/* hour, minute, second */
    {11, 2}, {24, 1}		/* dayofweek, status */
  },
  (const unsigned char*)"T:  :  :  :  :  :  :  \r\n    ",
  0
};

static parse_cvt_fnc_t cvt_varitext;
static parse_inp_fnc_t inp_varitext;

struct varitext {
  unsigned char start_found;
  unsigned char end_found;
  unsigned char end_count;
  unsigned char previous_ch;
  timestamp_t   tstamp;
};

clockformat_t   clock_varitext =
{
  inp_varitext,			/* Because of the strange format we need to parse it ourselves */
  cvt_varitext,			/* Varitext conversion */
  0,				/* no PPS monitoring */
  (void *)&varitext_fmt,	/* conversion configuration */
  "Varitext Radio Clock",	/* Varitext Radio Clock */
  30,				/* string buffer */
  sizeof(struct varitext),	/* Private data size required to hold current parse state */
};

/*
 * parse_cvt_fnc_t cvt_varitext
 *
 * convert simple type format
 */
static unsigned long
cvt_varitext(
	     unsigned char	*buffer,
	     int    		size,
	     struct format	*format,
	     clocktime_t	*clock_time,
	     void		*local
	     )
{
  UNUSED_ARG(size);
  UNUSED_ARG(local);

  if (!Strok(buffer, format->fixed_string)) {
    return CVT_NONE;
  } else {
    if (Stoi(&buffer[format->field_offsets[O_DAY].offset], &clock_time->day,
	     format->field_offsets[O_DAY].length) ||
	Stoi(&buffer[format->field_offsets[O_MONTH].offset], &clock_time->month,
	     format->field_offsets[O_MONTH].length) ||
	Stoi(&buffer[format->field_offsets[O_YEAR].offset], &clock_time->year,
	     format->field_offsets[O_YEAR].length) ||
	Stoi(&buffer[format->field_offsets[O_HOUR].offset], &clock_time->hour,
	     format->field_offsets[O_HOUR].length) ||
	Stoi(&buffer[format->field_offsets[O_MIN].offset], &clock_time->minute,
	     format->field_offsets[O_MIN].length) ||
	Stoi(&buffer[format->field_offsets[O_SEC].offset], &clock_time->second,
	     format->field_offsets[O_SEC].length)) {
      return CVT_FAIL | CVT_BADFMT;
    } else {
      uint8_t *f = (uint8_t*) &buffer[format->field_offsets[O_FLAGS].offset];

      clock_time->flags = 0;
      clock_time->utcoffset = 0;

      if (((*f) & VT_BST))	/* BST flag is set so set to indicate daylight saving time is active and utc offset */
	{
	  clock_time->utcoffset = -1*60*60;
	  clock_time->flags |= PARSEB_DST;
	}
      /*
	 if (!((*f) & VT_INITIALISED))  Clock not initialised
	 clock_time->flags |= PARSEB_POWERUP;

	 if (!((*f) & VT_SYNCHRONISED))   Clock not synchronised
	 clock_time->flags |= PARSEB_NOSYNC;

	 if (((*f) & VT_SEASON_CHANGE))  Seasonal change expected in the next hour
	 clock_time->flags |= PARSEB_ANNOUNCE;
	 */
      return CVT_OK;
    }
  }
}

/* parse_inp_fnc_t inp_varitext */
static unsigned long
inp_varitext(
	     parse_t	 *parseio,
	     char ch,
	     timestamp_t *tstamp
	     )
{
  struct varitext *t = (struct varitext *)parseio->parse_pdata;
  int    rtc;

  parseprintf(DD_PARSE, ("inp_varitext(0x%lx, 0x%x, ...)\n",
              (unsigned long)parseio, (unsigned)ch));

  if (!t)
    return PARSE_INP_SKIP;	/* local data not allocated - sigh! */

  if (ch == 'T')
    t->tstamp = *tstamp;

  if ((t->previous_ch == 'T') && (ch == ':'))
    {
      parseprintf(DD_PARSE, ("inp_varitext: START seen\n"));

      parseio->parse_data[0] = 'T';
      parseio->parse_index=1;
      parseio->parse_dtime.parse_stime = t->tstamp; /* Time stamp at packet start */
      t->start_found = 1;
      t->end_found = 0;
      t->end_count = 0;
    }

  if (t->start_found)
    {
      if ((rtc = (int)parse_addchar(parseio, ch)) != PARSE_INP_SKIP)
	{
	  parseprintf(DD_PARSE, ("inp_varitext: ABORTED due to too many characters\n"));

	  memset(t, 0, sizeof(struct varitext));
	  return (unsigned long)rtc;
	}

      if (t->end_found)
	{
	  if (++(t->end_count) == 4) /* Finally found the end of the message */
	    {
	      parseprintf(DD_PARSE, ("inp_varitext: END seen\n"));

	      memset(t, 0, sizeof(struct varitext));
	      if ((rtc = (int)parse_addchar(parseio, 0)) == PARSE_INP_SKIP)
		return parse_end(parseio);
	      else
		return (unsigned long)rtc;
	    }
	}

      if ((t->previous_ch == '\r') && (ch == '\n'))
	{
	  t->end_found = 1;
	}

    }

  t->previous_ch = (unsigned char)ch;

  return PARSE_INP_SKIP;
}

/*
 * History:
 *
 * clk_varitext.c,v
 * Revision 1.5  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 1.4  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 *
 * Revision 1.0  1997/06/02 13:16:30  McConnell
 * File created
 *
 */
