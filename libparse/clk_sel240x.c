//////////////////////////////////////////////////////////////////////////////
// Copyright Schweitzer Engineering Laboratories, Inc. <opensource@selinc.com>
//////////////////////////////////////////////////////////////////////////////

#include "config.h"

// _XOPEN_SOURCE needed for strptime()
// For OpenBSD, the value must be at least 500
// For FreeBSD, the value must be at least 700 for ldexpl() in ntp_fp.h
#define _XOPEN_SOURCE 700

#include "ntp.h"	// only for the u_* typedefs from GCC; remove someday
#include "ntp_syslog.h"
#include "ntp_types.h"
#include "ntp_fp.h"
#include "ntp_calendar.h"
#include "ntp_machine.h"
#include "ntp_stdlib.h"

#include "parse.h"

#include <stdio.h>

#include <time.h>          // for strptime()

//////////////////////////////////////////////////////////////////////////////
// The B8 output has the following format B8 = '\x01YYYY:ddd:hh:mm:ssq\r\n'
//    where q = ' ' locked
//              '.' <1 us
//              '*' <10 us
//              '#' <100 us
//              '?' >100 us
//
// Based on this we need to record the stime when we receive the <SOH>
// character and end it when we see the \n.
//
// The q or quality character indicates satellite lock and sync.   For the
// purposes of NTP, we are going to call it valid when we receive anything but
// a '?'.  But we are only going to call it synced when we receive a ' '
//////////////////////////////////////////////////////////////////////////////

static parse_inp_fnc_t inp_sel240x;
static parse_cvt_fnc_t cvt_sel240x;

// Parse clock format structure describing the message above
static struct format sel240x_fmt =
{ { {  6, 3 },
    {  0, 0 },
    {  1, 4 },
    { 10, 2 },
    { 13, 2 },
    { 16, 2 },
    {  0, 0 },
    {  0, 0 },
    {  0, 0 },
    {  0, 0 },
    {  0, 0 },
    {  0, 0 }
  },
  (const unsigned char *)"\x01    :   :  :  :   \x0d\x0a",
  0
};

// Structure desctibing the parser
clockformat_t clock_sel240x =
{
	inp_sel240x,
	cvt_sel240x,
	pps_one,
	(void*)&sel240x_fmt,
	"SEL B8",
	25,
	0
};

//////////////////////////////////////////////////////////////////////////////
static unsigned long
inp_sel240x( parse_t      *parseio,
	     char         ch,
	     timestamp_t  *tstamp
	   )
{
	unsigned long rc;

	parseprintf(DD_PARSE, ("inp_sel240x(0x%lx, 0x%x, ...)\n",
                    (unsigned long)parseio, (unsigned)ch));

	switch( ch ) {
	case '\x01':
		parseio->parse_index = 1;
		parseio->parse_data[0] = ch;
		parseio->parse_dtime.parse_stime = *tstamp;
		rc = PARSE_INP_SKIP;
		break;
	case '\n':
		if( (rc = parse_addchar(parseio, ch)) == PARSE_INP_SKIP ) {
			rc = parse_end( parseio );
		}
		break;
	default:
		rc = parse_addchar( parseio, ch );
	}

	return rc;
}

//////////////////////////////////////////////////////////////////////////////
static unsigned long
cvt_sel240x( unsigned char *buffer,
	     int            size,
	     struct format *format,
	     clocktime_t   *clock_time,
	     void          *local
	   )
{
	unsigned long rc = CVT_NONE;

	UNUSED_ARG(size);
	UNUSED_ARG(local);

	if( Strok(buffer, format->fixed_string) ) {
		struct tm ptime;
		buffer++;
		buffer = (unsigned char *)strptime(
			(const char *)buffer, "%Y:%j:%H:%M:%S", &ptime );
		if( *(buffer+1) != '\x0d' ) {
			rc = CVT_FAIL | CVT_BADFMT;
		} else {
			clock_time->day = ptime.tm_mday;
			clock_time->month = ptime.tm_mon + 1;
			clock_time->year = ptime.tm_year + 1900;
			clock_time->hour = ptime.tm_hour;
			clock_time->minute = ptime.tm_min;
			clock_time->second = ptime.tm_sec;
			clock_time->usecond = 0;
			clock_time->utcoffset = 0;
			clock_time->flags = PARSEB_UTC;

			if( *buffer == '?' ) {
				clock_time->flags |= PARSEB_POWERUP;
			} else if( *buffer != ' ' ) {
				clock_time->flags |= PARSEB_NOSYNC;
			}

			rc = CVT_OK;
		}
	}

	return rc;
}

