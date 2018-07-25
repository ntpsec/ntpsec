/*
 * Parser module for reference clock
 *
 * Copyright (c) 1989-2015 by Frank Kardel <kardel@ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "config.h"
#include "ntp_fp.h"
#include "timespecops.h"
#include "ntp_calendar.h"
#include "ntp_stdlib.h"
#include "ntp_machine.h"
#include "ntp.h"		/* (get Y2KFixes definitions) 	Y2KFixes */

#include "parse.h"

# include <stdio.h>

/*
 * PPS edge info
 */
#define SYNC_ONE	0x01

static unsigned long timepacket (parse_t *);
static unsigned int parse_restart (parse_t *, char);
static parse_pps_fnc_t pps_simple;

bool
parse_timedout(
	       parse_t *parseio,
	       timestamp_t *tstamp,
	       struct timespec *del
	       )
{
	struct timespec delta;

	l_fp delt;

	delt = *tstamp;
	delt -= parseio->parse_lastchar;
	delta = lfp_uintv_to_tspec(delt);
       if (cmp_tspec(delta, *del) > 0)
	{
		parseprintf(DD_PARSE, ("parse: timedout: TRUE\n"));
		return true;
	}
	else
	{
		parseprintf(DD_PARSE, ("parse: timedout: FALSE\n"));
		return false;
	}
}

/*ARGSUSED*/
bool
parse_ioinit(
	parse_t *parseio
	)
{
	parseprintf(DD_PARSE, ("parse_iostart\n"));

	parseio->parse_plen = 0;
	parseio->parse_pdata = (void *)0;

	parseio->parse_data = 0;
	parseio->parse_ldata = 0;
	parseio->parse_dsize = 0;

	parseio->parse_badformat = 0;
	parseio->parse_ioflags   = PARSE_IO_CS7;	/* usual unix default */
	parseio->parse_index     = 0;
	parseio->parse_ldsize    = 0;

	return true;
}

/*ARGSUSED*/
void
parse_ioend(
	parse_t *parseio
	)
{
	parseprintf(DD_PARSE, ("parse_ioend\n"));

	if (parseio->parse_pdata)
	    free(parseio->parse_pdata);

	if (parseio->parse_data)
	    free(parseio->parse_data);
}

static unsigned int
parse_restart(
	      parse_t *parseio,
	      char ch
	      )
{
	unsigned int updated = PARSE_INP_SKIP;

	/*
	 * re-start packet - timeout - overflow - start symbol
	 */

	if (parseio->parse_index)
	{
		/*
		 * filled buffer - thus not end character found
		 * do processing now
		 */
		parseio->parse_data[parseio->parse_index] = '\0';
		memcpy(parseio->parse_ldata, parseio->parse_data, (unsigned)(parseio->parse_index+1));
		parseio->parse_ldsize = parseio->parse_index;
		updated = PARSE_INP_TIME;
	}

	parseio->parse_index = 1;
	parseio->parse_data[0] = ch;
	parseprintf(DD_PARSE, ("parse: parse_restart: buffer start (updated = %x)\n", updated));
	return updated;
}

unsigned int
parse_addchar(
	      parse_t *parseio,
	      char ch
	      )
{
	/*
	 * add to buffer
	 */
	if (parseio->parse_index < parseio->parse_dsize)
	{
		/*
		 * collect into buffer
		 */
		parseprintf(DD_PARSE,
                            ("parse: parse_addchar: buffer[%d] = 0x%x\n",
                             parseio->parse_index, (unsigned)ch));
		parseio->parse_data[parseio->parse_index++] = (char)ch;
		return PARSE_INP_SKIP;
	}
	else
		/*
		 * buffer overflow - attempt to make the best of it
		 */
		return parse_restart(parseio, ch);
}

unsigned int
parse_end(
	  parse_t *parseio
	  )
{
	/*
	 * message complete processing
	 */
	parseio->parse_data[parseio->parse_index] = '\0';
	memcpy(parseio->parse_ldata, parseio->parse_data, (unsigned)(parseio->parse_index+1));
	parseio->parse_ldsize = parseio->parse_index;
	parseio->parse_index = 0;
	parseprintf(DD_PARSE, ("parse: parse_end: buffer end\n"));
	return PARSE_INP_TIME;
}

/*ARGSUSED*/
int
parse_ioread(
	parse_t *parseio,
	char ch,
	timestamp_t *tstamp
	)
{
	unsigned int updated = CVT_NONE;
	/*
	 * within STREAMS CSx (x < 8) chars still have the upper bits set
	 * so we normalize the characters by masking unnecessary bits off.
	 *
	 * (ESR, 2015: Probably not necessary since STREAMS support has
	 * been removed, but harmless.)
	 */
	switch (parseio->parse_ioflags & PARSE_IO_CSIZE)
	{
	    case PARSE_IO_CS5:
		ch &= 0x1F;
		break;

	    case PARSE_IO_CS6:
		ch &= 0x3F;
		break;

	    case PARSE_IO_CS7:
		ch &= 0x7F;
		break;

	    case PARSE_IO_CS8:
		ch &= (char) 0xFFU;
		break;

            default:
                /* huh? */
                break;
	}

	parseprintf(DD_PARSE, ("parse_ioread(0x%lx, char=0x%x, ..., ...)\n",
                    (unsigned long)parseio, (unsigned)(ch & 0xFF)));

	if (!clockformats[parseio->parse_lformat]->convert)
	{
		parseprintf(DD_PARSE, ("parse_ioread: input dropped.\n"));
		return CVT_NONE;
	}

	if (clockformats[parseio->parse_lformat]->input)
	{
		unsigned long input_status;

		input_status = clockformats[parseio->parse_lformat]->input(parseio, ch, tstamp);

		if (input_status & PARSE_INP_SYNTH)
		{
			updated = CVT_OK;
		}

		if (input_status & PARSE_INP_TIME)	/* time sample is available */
		{
			updated = (unsigned int) timepacket(parseio);
		}

		if (input_status & PARSE_INP_DATA) /* got additional data */
		{
			updated |= CVT_ADDITIONAL;
		}
	}


	/*
	 * remember last character time
	 */
	parseio->parse_lastchar = *tstamp;

#ifdef DEBUG
	if ((updated & CVT_MASK) != CVT_NONE)
	{
		parseprintf(DD_PARSE, ("parse_ioread: time sample accumulated (status=0x%x)\n", updated));
	}
#endif

	parseio->parse_dtime.parse_status = updated;

	return (((updated & CVT_MASK) != CVT_NONE) ||
		((updated & CVT_ADDITIONAL) != 0));
}

/*
 * parse_iodone
 *
 * clean up internal status for new round
 */
/*ARGSUSED*/
void
parse_iodone(
	parse_t *parseio
	)
{
	/*
	 * we need to clean up certain flags for the next round
	 */
	parseprintf(DD_PARSE, ("parse_iodone: DONE\n"));
	parseio->parse_dtime.parse_state = 0; /* no problems with ISRs */
}

/*---------- conversion implementation --------------------*/

/*
 * convert a struct clock to UTC since Jan, 1st 1970 0:00 (the UNIX EPOCH)
 */

time_t
parse_to_unixtime(
	clocktime_t   *clock_time,
	unsigned long *cvtrtc
	)
{
#define SETRTC(_X_)	{ if (cvtrtc) *cvtrtc = (_X_); }
	static int days_of_month[] =
	{
		0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	int i;
	time_t t;

	if (clock_time->utctime)
	    return clock_time->utctime;	/* if the conversion routine gets it right away - why not */

	if ( clock_time->year < YEAR_PIVOT )			/* Y2KFixes [ */
	    clock_time->year += 100;	/* convert 20xx%100 to 20xx-1900 */
	if ( clock_time->year < YEAR_BREAK )	/* expand to full four-digits */
	    clock_time->year += 1900;

	if (clock_time->year < 1970 )				/* Y2KFixes ] */
	{
		SETRTC(CVT_FAIL|CVT_BADDATE);
		return -1;
	}

	/*
	 * sorry, slow section here - but it's not time critical anyway
	 */
	t = julian0(clock_time->year) - julian0(1970);		/* Y2kFixes */
				/* month */
	if (clock_time->month <= 0 || clock_time->month > 12)
	{
		SETRTC(CVT_FAIL|CVT_BADDATE);
		return -1;		/* bad month */
	}

	if ( clock_time->month >= 3  &&  is_leapyear(clock_time->year) )
	    t++;		/* add one more if within leap year */

	for (i = 1; i < clock_time->month; i++)
	{
		t += days_of_month[i];
	}
				/* day */
	if (clock_time->day < 1 || ((clock_time->month == 2 && days_per_year(clock_time->year) == 366) ?
			       clock_time->day > 29 : clock_time->day > days_of_month[clock_time->month]))
	{
		SETRTC(CVT_FAIL|CVT_BADDATE);
		return -1;		/* bad day */
	}

	t += clock_time->day - 1;
				/* hour */
	if (clock_time->hour < 0 || clock_time->hour >= 24)
	{
		SETRTC(CVT_FAIL|CVT_BADTIME);
		return -1;		/* bad hour */
	}

	t = t*24 + clock_time->hour;

				/* min */
	if (clock_time->minute < 0 || clock_time->minute > 59)
	{
		SETRTC(CVT_FAIL|CVT_BADTIME);
		return -1;		/* bad min */
	}

	t = t*60 + clock_time->minute;
				/* sec */

	if (clock_time->second < 0 || clock_time->second > 60)	/* allow for LEAPs */
	{
		SETRTC(CVT_FAIL|CVT_BADTIME);
		return -1;		/* bad sec */
	}

	t  = t*60 + clock_time->second;

	t += clock_time->utcoffset;	/* warp to UTC */

				/* done */

	clock_time->utctime = t;		/* documentray only */

	return t;
}

/*--------------- format conversion -----------------------------------*/

int
Stoi(
	const unsigned char *s,
	long *zp,
	int cnt
	)
{
	char unsigned const *b = s;
	int f,z,v;
	char unsigned c;

	f=z=v=0;

	while(*s == ' ')
	    s++;

	if (*s == '-')
	{
		s++;
		v = 1;
	}
	else
	    if (*s == '+')
		s++;

	for(;;)
	{
		c = *s++;
		if (c == '\0' || c < '0' || c > '9' || (cnt && ((s-b) > cnt)))
		{
			if (f == 0)
			{
				return(-1);
			}
			if (v)
			    z = -z;
			*zp = z;
			return(0);
		}
		z = (z << 3) + (z << 1) + ( c - '0' );
		f=1;
	}
}

int
Strok(
	const unsigned char *s,
	const unsigned char *m
	)
{
	if (!s || !m)
	    return 0;

	while(*s && *m)
	{
		if ((*m == ' ') ? 1 : (*s == *m))
		{
			s++;
			m++;
		}
		else
		{
			return 0;
		}
	}
	return !*m;
}

unsigned long
updatetimeinfo(
	       parse_t *parseio,
	       unsigned long   flags
	       )
{
		parseio->parse_lstate          = parseio->parse_dtime.parse_state | flags | PARSEB_TIMECODE;

		parseio->parse_dtime.parse_state = parseio->parse_lstate;

	parseprintf(DD_PARSE, ("updatetimeinfo status=0x%lx, time=%x\n",
			       (unsigned long)parseio->parse_dtime.parse_state,
	                       lfpuint(parseio->parse_dtime.parse_time)));

	return CVT_OK;		/* everything fine and dandy... */
}

/*
 * parse_pps_fnc_t pps_simple
 *
 * handle a pps time stamp
 */
/*ARGSUSED*/
static unsigned long
pps_simple(
	parse_t *parseio,
	int status,
	timestamp_t *ptime
	)
{
	UNUSED_ARG(status);
	parseio->parse_dtime.parse_ptime  = *ptime;
	parseio->parse_dtime.parse_state |= PARSEB_PPS|PARSEB_S_PPS;

	return CVT_NONE;
}

/*
 * parse_pps_fnc_t pps_one
 *
 * handle a pps time stamp in ONE edge
 */
/*ARGSUSED*/
unsigned long
pps_one(
	parse_t *parseio,
	int status,
	timestamp_t *ptime
	)
{
	if (status)
		return pps_simple(parseio, status, ptime);

	return CVT_NONE;
}

/*
 * timepacket
 *
 * process a data packet
 */
static unsigned long
timepacket(
	parse_t *parseio
	)
{
	unsigned short format;
	time_t t;
	unsigned long cvtrtc;		/* current conversion result */
	clocktime_t clock_time;

	memset((char *)&clock_time, 0, sizeof clock_time);
	format = parseio->parse_lformat;

	if (format == (unsigned short)~0)
		return CVT_NONE;

	switch ((cvtrtc = clockformats[format]->convert ?
		 clockformats[format]->convert((unsigned char *)parseio->parse_ldata, parseio->parse_ldsize, (struct format *)(clockformats[format]->data), &clock_time, parseio->parse_pdata) :
		 CVT_NONE) & CVT_MASK)
	{
	case CVT_FAIL:
		parseio->parse_badformat++;
		return cvtrtc;

	case CVT_NONE:
		/*
		 * too bad - pretend bad format
		 */
		parseio->parse_badformat++;
		return CVT_NONE;

	case CVT_OK:
		break;

	case CVT_SKIP:
		return CVT_NONE;

	default:
		/* shouldn't happen */
		msyslog(LOG_WARNING, "ERR: parse: INTERNAL error: bad return code of convert routine \"%s\"", clockformats[format]->name);
		return CVT_FAIL|cvtrtc;
	}

	if ((t = parse_to_unixtime(&clock_time, &cvtrtc)) == -1)
	{
		return CVT_FAIL|cvtrtc;
	}

	/*
	 * time stamp
	 */
	struct timespec ts = {t, clock_time.usecond * 1000};
	parseio->parse_dtime.parse_time = tspec_stamp_to_lfp(ts);

	parseio->parse_dtime.parse_format       = format;

	return updatetimeinfo(parseio, clock_time.flags);
}

/*ARGSUSED*/
bool
parse_timecode(
	parsectl_t *dct,
	parse_t    *parse
	)
{
	dct->parsegettc.parse_state  = parse->parse_lstate;
	dct->parsegettc.parse_format = parse->parse_lformat;
	/*
	 * move out current bad packet count
	 * user program is expected to sum these up
	 * this is not a problem, as "parse" module are
	 * exclusive open only
	 */
	dct->parsegettc.parse_badformat = parse->parse_badformat;
	parse->parse_badformat = 0;

	if (parse->parse_ldsize <= PARSE_TCMAX)
	{
		dct->parsegettc.parse_count = parse->parse_ldsize;
		memcpy(dct->parsegettc.parse_buffer, parse->parse_ldata, dct->parsegettc.parse_count);
		return true;
	}
	else
	{
		return false;
	}
}


/*ARGSUSED*/
bool
parse_setfmt(
	parsectl_t *dct,
	parse_t    *parse
	)
{
	if (dct->parseformat.parse_count <= PARSE_TCMAX)
	{
		if (dct->parseformat.parse_count)
		{
			unsigned short i;

			for (i = 0; i < nformats; i++)
			{
				if (!strcmp(dct->parseformat.parse_buffer, clockformats[i]->name))
				{
					if (parse->parse_pdata)
						free(parse->parse_pdata);
					parse->parse_pdata = 0;

					parse->parse_plen = clockformats[i]->plen;

					if (parse->parse_plen)
					{
						parse->parse_pdata = malloc(parse->parse_plen);
						if (!parse->parse_pdata)
						{
							parseprintf(DD_PARSE, ("set format failed: malloc for private data area failed\n"));
							return false;
						}
						memset((char *)parse->parse_pdata, 0, parse->parse_plen);
					}

					if (parse->parse_data)
						free(parse->parse_data);
					parse->parse_ldata = parse->parse_data = 0;

					parse->parse_dsize = clockformats[i]->length;

					if (parse->parse_dsize)
					{
						parse->parse_data = (char*)malloc((unsigned)(parse->parse_dsize * 2 + 2));
						if (!parse->parse_data)
						{
							if (parse->parse_pdata)
								free(parse->parse_pdata);
							parse->parse_pdata = 0;

							parseprintf(DD_PARSE, ("init failed: malloc for data area failed\n"));
							return false;
						}
					}


					/*
					 * leave room for '\0'
					 */
					parse->parse_ldata     = parse->parse_data + parse->parse_dsize + 1;

					parse->parse_lformat  = i;

					return true;
				}
			}
		}
	}
	return false;
}

/*ARGSUSED*/
int
parse_getfmt(
	parsectl_t *dct,
	parse_t    *parse
	)
{
	UNUSED_ARG(dct);
	UNUSED_ARG(parse);
	if (dct->parseformat.parse_format < nformats &&
	    strlen(clockformats[dct->parseformat.parse_format]->name) <= PARSE_TCMAX)
	{
		dct->parseformat.parse_count = (unsigned short) (strlen(clockformats[dct->parseformat.parse_format]->name) + 1);
		memcpy(dct->parseformat.parse_buffer, clockformats[dct->parseformat.parse_format]->name, dct->parseformat.parse_count);
		return 1;
	}
	else
	{
		return 0;
	}
}

/*ARGSUSED*/
bool
parse_setcs(
	parsectl_t *dct,
	parse_t    *parse
	)
{
	parse->parse_ioflags &= ~PARSE_IO_CSIZE;
	parse->parse_ioflags |= (int) (dct->parsesetcs.parse_cs & PARSE_IO_CSIZE);
	return true;
}

/*
 * History:
 *
 * parse.c,v
 * Revision 4.20  2005/08/06 17:39:40  kardel
 * cleanup size handling wrt/ to buffer boundaries
 *
 * Revision 4.19  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 4.18  2004/11/14 16:11:05  kardel
 * update Id tags
 *
 * Revision 4.17  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.14  1999/11/28 09:13:52  kardel
 * RECON_4_0_98F
 *
 * Revision 4.13  1999/02/28 11:50:20  kardel
 * (timepacket): removed unnecessary code
 *
 * Revision 4.12  1999/02/21 12:17:44  kardel
 * 4.91f reconciliation
 *
 * Revision 4.11  1999/02/21 11:09:47  kardel
 * unified debug output
 *
 * Revision 4.10  1998/12/20 23:45:30  kardel
 * fix types and warnings
 *
 * Revision 4.9  1998/08/09 22:26:06  kardel
 * Trimble TSIP support
 *
 * Revision 4.8  1998/06/14 21:09:39  kardel
 * Sun acc cleanup
 *
 * Revision 4.7  1998/06/13 15:19:13  kardel
 * fix mem*() to b*() function macro emulation
 *
 * Revision 4.6  1998/06/13 13:24:13  kardel
 * printf fmt
 *
 * Revision 4.5  1998/06/13 13:01:10  kardel
 * printf fmt
 *
 * Revision 4.4  1998/06/13 12:12:10  kardel
 * bcopy/memcpy cleanup
 * fix SVSV name clash
 *
 * Revision 4.3  1998/06/12 15:22:30  kardel
 * fix prototypes
 *
 * Revision 4.2  1998/06/12 09:13:27  kardel
 * conditional compile macros fixed
 * printf prototype
 *
 * Revision 4.1  1998/05/24 09:39:55  kardel
 * implementation of the new IO handling model
 *
 * Revision 4.0  1998/04/10 19:45:36  kardel
 * Start 4.0 release version numbering
 *
 * from V3 3.46 log info deleted 1998/04/11 kardel
 */
