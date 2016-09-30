/*
 * Trimble SV6 clock support - several collected codepieces
 *
 * Copyright (c) 1989-2005 by Frank Kardel <kardel@ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <config.h>
#include "ntp_fp.h"
#include "ntp_calendar.h"

#include "parse.h"

#include "ntp_stdlib.h"

/*	0000000000111111111122222222223333333	/ char
 *	0123456789012345678901234567890123456	\ posn
 *	>RTMhhmmssdddDDMMYYYYoodnnvrrrrr;*xx<	Actual
 *	----33445566600112222BB7__-_____--99-	Parse
 *	>RTM                      1     ;*  <",	Check
 */

#define	hexval(x) (('0' <= (x) && (x) <= '9') ? (x) - '0' : \
		   ('a' <= (x) && (x) <= 'f') ? (x) - 'a' + 10 : \
		   ('A' <= (x) && (x) <= 'F') ? (x) - 'A' + 10 : \
		   -1)
#define	O_USEC		O_WDAY
#define	O_GPSFIX	O_FLAGS
#define	O_CHKSUM	O_UTCHOFFSET
     static struct format trimsv6_fmt =
{ { { 13, 2 }, {15, 2}, { 17, 4}, /* Day, Month, Year */
    {  4, 2 }, { 6, 2}, {  8, 2}, /* Hour, Minute, Second */
    { 10, 3 }, {23, 1}, {  0, 0}, /* uSec, FIXes (WeekDAY, FLAGS, ZONE) */
    { 34, 2 }, { 0, 0}, { 21, 2}, /* cksum, -, utcS (UTC[HMS]OFFSET) */
},
  (const unsigned char *)">RTM                      1     ;*  <",
  0
};

static parse_cvt_fnc_t cvt_trimtaip;
static parse_inp_fnc_t inp_trimtaip;

clockformat_t clock_trimtaip =
{
  inp_trimtaip,			/* no input handling */
  cvt_trimtaip,			/* Trimble conversion */
  pps_one,			/* easy PPS monitoring */
  (void *)&trimsv6_fmt,		/* conversion configuration */
  "Trimble TAIP",
  37,				/* string buffer */
  0				/* no private data */
};

/* parse_cvt_fnc_t cvt_trimtaip */
static unsigned long
cvt_trimtaip(
	     unsigned char *buffer,
	     int            size,
	     struct format *format,
	     clocktime_t   *clock_time,
	     void          *local
	     )
{
	long gpsfix;
	uint8_t calc_csum = 0;
	long   recv_csum;
	int	 i;

	UNUSED_ARG(size);
	UNUSED_ARG(local);

	if (!Strok(buffer, format->fixed_string)) return CVT_NONE;
#define	OFFS(x) format->field_offsets[(x)].offset
#define	STOI(x, y) \
	Stoi(&buffer[OFFS(x)], y, \
	     format->field_offsets[(x)].length)
		if (	STOI(O_DAY,	&clock_time->day)	||
			STOI(O_MONTH,	&clock_time->month)	||
			STOI(O_YEAR,	&clock_time->year)	||
			STOI(O_HOUR,	&clock_time->hour)	||
			STOI(O_MIN,	&clock_time->minute)	||
			STOI(O_SEC,	&clock_time->second)	||
			STOI(O_USEC,	&clock_time->usecond)||
			STOI(O_GPSFIX,	&gpsfix)
			) return CVT_FAIL|CVT_BADFMT;

	clock_time->usecond *= 1000;
	/* Check that the checksum is right */
	for (i=OFFS(O_CHKSUM)-1; i >= 0; i--) calc_csum ^= buffer[i];
	recv_csum =	(hexval(buffer[OFFS(O_CHKSUM)]) << 4) |
		hexval(buffer[OFFS(O_CHKSUM)+1]);
	if (recv_csum < 0) return CVT_FAIL|CVT_BADTIME;
	if (((uint8_t) recv_csum) != calc_csum) return CVT_FAIL|CVT_BADTIME;

	clock_time->utcoffset = 0;

	/* What should flags be set to ? */
	clock_time->flags = PARSEB_UTC;

	/* if the current GPS fix is 9 (unknown), reject */
	if (0 > gpsfix || gpsfix > 9) clock_time->flags |= PARSEB_POWERUP;

	return CVT_OK;
}

/*
 * parse_inp_fnc_t inp_trimtaip
 *
 * grab data from input stream
 */
static unsigned long
inp_trimtaip(
	     parse_t      *parseio,
	     char         ch,
	     timestamp_t  *tstamp
	  )
{
	unsigned int rtc;

	parseprintf(DD_PARSE, ("inp_trimtaip(0x%lx, 0x%x, ...)\n", (long)parseio, ch));

	switch (ch)
	{
	case '>':
		parseprintf(DD_PARSE, ("inp_trimptaip: START seen\n"));

		parseio->parse_index = 1;
		parseio->parse_data[0] = ch;
		parseio->parse_dtime.parse_stime = *tstamp; /* collect timestamp */
		return PARSE_INP_SKIP;

	case '<':
		parseprintf(DD_PARSE, ("inp_trimtaip: END seen\n"));
		if ((rtc = parse_addchar(parseio, ch)) == PARSE_INP_SKIP)
			return parse_end(parseio);
		else
			return rtc;


	default:
		return parse_addchar(parseio, ch);
	}
}

/*
 * History:
 *
 * clk_trimtaip.c,v
 * Revision 4.11  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 4.10  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.7  1999/11/28 09:13:51  kardel
 * RECON_4_0_98F
 *
 * Revision 4.6  1998/08/16 18:46:27  kardel
 * (clock_trimtaip =): changed format name
 *
 * Revision 4.5  1998/06/14 21:09:38  kardel
 * Sun acc cleanup
 *
 * Revision 4.4  1998/06/13 12:06:57  kardel
 * fix SYSV clock name clash
 *
 * Revision 4.3  1998/06/12 15:22:29  kardel
 * fix prototypes
 *
 * Revision 4.2  1998/06/12 09:13:26  kardel
 * conditional compile macros fixed
 * printf prototype
 *
 * Revision 4.1  1998/05/24 09:39:54  kardel
 * implementation of the new IO handling model
 *
 * Revision 4.0  1998/04/10 19:45:31  kardel
 * Start 4.0 release version numbering
 *
 * from V3 1.4 log info deleted 1998/04/11 kardel
 */

