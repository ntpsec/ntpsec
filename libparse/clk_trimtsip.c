/*
 * Trimble TSIP support
 * Thanks to Sven Dietrich for providing test hardware
 *
 * Copyright (c) 1995-2009 by Frank Kardel <kardel <AT> ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "config.h"
#include "ntp_syslog.h"
#include "ntp_types.h"
#include "ntp_fp.h"
#include "timespecops.h"
#include "ntp_calendar.h"
#include "ntp_machine.h"
#include "ntp_stdlib.h"

#include "parse.h"

#include <stdio.h>

#include "ascii.h"
#include "binio.h"
#include "ieee754io.h"
#include "trimble.h"
#include "gpstolfp.h"

/*
 * Trimble low level TSIP parser / time converter
 *
 * The receiver uses a serial message protocol called Trimble Standard
 * Interface Protocol (it can support others but this driver only supports
 * TSIP). Messages in this protocol have the following form:
 *
 * <DLE><id> ... <data> ... <DLE><ETX>
 *
 * Any bytes within the <data> portion of value 10 hex (<DLE>) are doubled
 * on transmission and compressed back to one on reception. Otherwise
 * the values of data bytes can be anything. The serial interface is RS-422
 * asynchronous using 9600 baud, 8 data bits with odd party (**note** 9 bits
 * in total!), and 1 stop bit. The protocol supports byte, integer, single,
 * and double datatypes. Integers are two bytes, sent most significant first.
 * Singles are IEEE754 single precision floating point numbers (4 byte) sent
 * sign & exponent first. Doubles are IEEE754 double precision floating point
 * numbers (8 byte) sent sign & exponent first.
 * The receiver supports a large set of messages, only a very small subset of
 * which is used here.
 *
 * From this module the following are recognised:
 *
 *  ID    Description
 *
 *  41    GPS Time
 *  46    Receiver health
 *  4F    UTC correction data (used to get leap second warnings)
 *
 * All others are accepted but ignored for time conversion - they are passed up to higher layers.
 *
 */

static offsets_t trim_offsets = { 0, 1, 2, 3, 4, 5, 6, 7 };

struct trimble
{
	uint8_t  t_in_pkt;	/* first DLE received */
	uint8_t  t_dle;		/* subsequent DLE received */
	unsigned short t_week;		/* GPS week */
	unsigned short t_weekleap;	/* GPS week of next/last week */
	unsigned short t_dayleap;	/* day in week */
	unsigned short t_gpsutc;	/* GPS - UTC offset */
	unsigned short t_gpsutcleap;	/* offset at next/last leap */
	uint8_t  t_operable;	/* receiver feels OK */
	uint8_t  t_mode;		/* actual operating mode */
	uint8_t  t_leap;		/* possible leap warning */
        uint8_t  t_utcknown;	/* utc offset known */
};

#define STATUS_BAD    0		/* BAD or UNINITIALIZED receiver status */
#define STATUS_UNSAFE 1		/* not enough receivers for full precision */
#define STATUS_SYNC   2		/* enough information for good operation */

static unsigned long inp_tsip (parse_t *, char, timestamp_t *);
static unsigned long cvt_trimtsip (unsigned char *, int, struct format *, clocktime_t *, void *);

struct clockformat clock_trimtsip =
{
	inp_tsip,		/* Trimble TSIP input handler */
	cvt_trimtsip,		/* Trimble TSIP conversion */
	pps_one,		/* easy PPS monitoring */
	0,			/* no configuration data */
	"Trimble TSIP",
	400,			/* input buffer */
	sizeof(struct trimble)	/* private data */
};

#define ADDSECOND	0x01
#define DELSECOND	0x02

static unsigned long
inp_tsip(
	 parse_t      *parseio,
	 char         ch,
	 timestamp_t  *tstamp
	)
{
	struct trimble *t = (struct trimble *)parseio->parse_pdata;

	if (!t)
	    return PARSE_INP_SKIP;		/* local data not allocated - sigh! */

	if (!t->t_in_pkt && ch != DLE) {
		/* wait for start of packet */
		return PARSE_INP_SKIP;
	}

	if ((parseio->parse_index >= (parseio->parse_dsize - 2)) ||
	    (parseio->parse_dtime.parse_msglen >= (sizeof(parseio->parse_dtime.parse_msg) - 2)))
		{		/* OVERFLOW - DROP! */
			t->t_in_pkt = t->t_dle = 0;
			parseio->parse_index = 0;
			parseio->parse_dtime.parse_msglen = 0;
		return PARSE_INP_SKIP;
	}

	switch (ch) {
	    case DLE:
		if (!t->t_in_pkt) {
			t->t_dle = 0;
			t->t_in_pkt = 1;
			parseio->parse_index = 0;
			parseio->parse_data[parseio->parse_index++] = ch;
			parseio->parse_dtime.parse_msglen = 0;
			parseio->parse_dtime.parse_msg[parseio->parse_dtime.parse_msglen++] = (unsigned char)ch;
			parseio->parse_dtime.parse_stime = *tstamp; /* pick up time stamp at packet start */
		} else if (t->t_dle) {
			/* Double DLE -> insert a DLE */
			t->t_dle = 0;
			parseio->parse_data[parseio->parse_index++] = DLE;
			parseio->parse_dtime.parse_msg[parseio->parse_dtime.parse_msglen++] = DLE;
		} else
		    t->t_dle = 1;
		break;

	    case ETX:
		if (t->t_dle) {
			/* DLE,ETX -> end of packet */
			parseio->parse_data[parseio->parse_index++] = DLE;
			parseio->parse_data[parseio->parse_index] = ch;
			parseio->parse_ldsize = (unsigned short) (parseio->parse_index + 1);
			memcpy(parseio->parse_ldata, parseio->parse_data, parseio->parse_ldsize);
			parseio->parse_dtime.parse_msg[parseio->parse_dtime.parse_msglen++] = DLE;
			parseio->parse_dtime.parse_msg[parseio->parse_dtime.parse_msglen++] = (unsigned char)ch;
			t->t_in_pkt = t->t_dle = 0;
			return PARSE_INP_TIME|PARSE_INP_DATA;
		}
		/*FALLTHROUGH*/

	    default:		/* collect data */
		t->t_dle = 0;
		parseio->parse_data[parseio->parse_index++] = ch;
		parseio->parse_dtime.parse_msg[parseio->parse_dtime.parse_msglen++] = (unsigned char)ch;
	}

  return PARSE_INP_SKIP;
}

/*
 * cvt_trimtsip
 *
 * convert TSIP type format
 */
static unsigned long
cvt_trimtsip(
	     unsigned char *buffer,
	     int            size,
	     struct format *format,
	     clocktime_t   *clock_time,
	     void          *local
	     )
{
        struct trimble *t = (struct trimble *)local; /* get local data space */
#define mb(_X_) (buffer[2+(_X_)]) /* shortcut for buffer access */
	uint8_t cmd;

	UNUSED_ARG(format);

	clock_time->flags = 0;

	if (!t) {
		return CVT_NONE;		/* local data not allocated - sigh! */
	}

	if ((size < 4) ||
	    (buffer[0]      != DLE) ||
	    (buffer[size-1] != ETX) ||
	    (buffer[size-2] != DLE))
	{
		printf("TRIMBLE BAD packet, size %d:\n", size);
		return CVT_NONE;
	}
	else
	{
		unsigned char *bp;
		cmd = buffer[1];

		    switch(cmd)
		    {
		    case CMD_RCURTIME:
			    {			/* GPS time */
				    l_fp secs;
				    int week = getmsb_short(&mb(4));
				    l_fp utcoffset;
				    l_fp gpstime;

				    bp = &mb(0);
				    if (fetch_ieee754(&bp, IEEE_SINGLE, &secs, trim_offsets) != IEEE_OK)
					    return CVT_FAIL|CVT_BADFMT;

				    if ((lfpsint(secs) <= 0) ||
					(t->t_utcknown == 0))
				    {
					    clock_time->flags = PARSEB_POWERUP;
					    return CVT_OK;
				    }
				    if (week < GPSWRAP) {
					    week += GPSWEEKS;
				    }

				    /* time OK */

				    /* fetch UTC offset */
				    bp = &mb(6);
				    if (fetch_ieee754(&bp, IEEE_SINGLE, &utcoffset, trim_offsets) != IEEE_OK)
					    return CVT_FAIL|CVT_BADFMT;

				    secs -= utcoffset; /* adjust GPS time to UTC time */

				    gpstolfp((unsigned short)week, (unsigned short)0,
					     lfpuint(secs), &gpstime);

				    setlfpfrac(gpstime, lfpfrac(secs));

				    clock_time->utctime = (time_t)(lfpuint(gpstime) - JAN_1970);

				    clock_time->usecond = lfp_intv_to_tspec(gpstime).tv_nsec / 1000;

				    if (t->t_leap == ADDSECOND)
					clock_time->flags |= PARSEB_LEAPADD;

				    if (t->t_leap == DELSECOND)
					clock_time->flags |= PARSEB_LEAPDEL;

				    switch (t->t_operable)
				      {
				      case STATUS_SYNC:
					clock_time->flags &= ~(PARSEB_POWERUP|PARSEB_NOSYNC);
					break;

				      case STATUS_UNSAFE:
					clock_time->flags |= PARSEB_NOSYNC;
					break;

				      case STATUS_BAD:
					clock_time->flags |= PARSEB_NOSYNC|PARSEB_POWERUP;
					break;
                                      default:
                                        /* huh? */
                                        break;
				      }

				    if (t->t_mode == 0)
					    clock_time->flags |= PARSEB_POSITION;

				    clock_time->flags |= PARSEB_S_LEAP|PARSEB_S_POSITION;

				    return CVT_OK;

			    } /* case 0x41 */

		    case CMD_RRECVHEALTH:
			    {
				    /* TRIMBLE health */
				    uint8_t status = mb(0);

				    switch (status)
				    {
				      case 0x00: /* position fixes */
					t->t_operable = STATUS_SYNC;
					break;

				      case 0x09: /* 1 satellite */
				      case 0x0A: /* 2 satellites */
				      case 0x0B: /* 3 satellites */
					t->t_operable = STATUS_UNSAFE;
					break;

				      default:
					t->t_operable = STATUS_BAD;
					break;
				    }
				    t->t_mode = status;
			    }
			    break;

		    case CMD_RUTCPARAM:
			    {
			            l_fp t0t;
				    unsigned char *lbp;

				    /* UTC correction data - derive a leap warning */
				    /* current leap correction (GPS-UTC) */
				    int tls = t->t_gpsutc = get_msb_ushort(&mb(12));

                                    /* new leap correction */
				    int tlsf = t->t_gpsutcleap = get_msb_ushort(&mb(24));

                                    /* week no of leap correction */
				    t->t_weekleap = get_msb_ushort(&mb(20));
				    if (t->t_weekleap < GPSWRAP)
				      t->t_weekleap = (unsigned short)(t->t_weekleap + GPSWEEKS);

                                    /* day in week of leap correction */
				    t->t_dayleap = get_msb_ushort(&mb(22));
                                    /* current week no */
				    t->t_week = get_msb_ushort(&mb(18));
				    if (t->t_week < GPSWRAP)
				      /* coverity[copy_paste_error] */
				      t->t_week = (unsigned short)(t->t_weekleap + GPSWEEKS);

				    lbp = (unsigned char *)&mb(14); /* last update time */
				    if (fetch_ieee754(&lbp, IEEE_SINGLE, &t0t, trim_offsets) != IEEE_OK)
					    return CVT_FAIL|CVT_BADFMT;

				    t->t_utcknown = lfpuint(t0t) != 0;

				    if ((t->t_utcknown) && /* got UTC information */
					(tlsf != tls)   && /* something will change */
					((t->t_weekleap - t->t_week) < 5)) /* and close in the future */
				    {
					    /* generate a leap warning */
					    if (tlsf > tls)
						t->t_leap = ADDSECOND;
					    else
						t->t_leap = DELSECOND;
				    }
				    else
				    {
					    t->t_leap = 0;
				    }
			    }
			    break;

		    default:
			    /* it's validly formed, but we don't care about it! */
			    break;
		}
	}
	return CVT_SKIP;
}

/*
 * History:
 *
 * clk_trimtsip.c,v
 * Revision 4.19  2009/11/01 10:47:49  kardel
 * de-P()
 *
 * Revision 4.18  2009/11/01 08:46:46  kardel
 * clarify case FALLTHROUGH
 *
 * Revision 4.17  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 4.16  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.13  1999/11/28 09:13:51  kardel
 * RECON_4_0_98F
 *
 * Revision 4.12  1999/02/28 13:00:08  kardel
 * *** empty log message ***
 *
 * Revision 4.11  1999/02/28 11:47:54  kardel
 * (struct trimble): new member t_utcknown
 * (cvt_trimtsip): fixed status monitoring, bad receiver states are
 * now recognized
 *
 * Revision 4.10  1999/02/27 15:57:15  kardel
 * use mmemcpy instead of bcopy
 *
 * Revision 4.9  1999/02/21 12:17:42  kardel
 * 4.91f reconciliation
 *
 * Revision 4.8  1998/11/15 20:27:58  kardel
 * Release 4.0.73e13 reconciliation
 *
 * Revision 4.7  1998/08/16 18:49:20  kardel
 * (cvt_trimtsip): initial kernel capable version (no more floats)
 * (clock_trimtsip =): new format name
 *
 * Revision 4.6  1998/08/09 22:26:05  kardel
 * Trimble TSIP support
 *
 * Revision 4.5  1998/08/02 10:37:05  kardel
 * working TSIP parser
 *
 * Revision 4.4  1998/06/28 16:50:40  kardel
 * (getflt): fixed ENDIAN issue
 * (getdbl): fixed ENDIAN issue
 * (getint): use get_msb_short()
 * (cvt_trimtsip): use gpstolfp() for conversion
 *
 * Revision 4.3  1998/06/13 12:07:31  kardel
 * fix SYSV clock name clash
 *
 * Revision 4.2  1998/06/12 15:22:30  kardel
 * fix prototypes
 *
 * Revision 4.1  1998/05/24 09:39:54  kardel
 * implementation of the new IO handling model
 *
 * Revision 4.0  1998/04/10 19:45:32  kardel
 * Start 4.0 release version numbering
 *
 * from V3 1.8 loginfo deleted 1998/04/11 kardel
 */
