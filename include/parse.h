/*
 * Copyright (c) 1989-2015 by Frank Kardel <kardel@ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-clause
 */

#ifndef GUARD__PARSE_H
#define GUARD__PARSE_H

#include "ntp_types.h"

#include "parse_conf.h"

#include <stdio.h>
#include "ntp_syslog.h"
#ifdef	DEBUG
#define DD_PARSE 5
#define DD_RAWDCF 4
#define parseprintf(LEVEL, ARGS) if (debug > LEVEL) printf ARGS
#else	/* DEBUG */
#define parseprintf(LEVEL, ARGS)
#endif	/* DEBUG */

/*
 * some constants useful for GPS time conversion
 */
#define GPSWRAP         990                        /* assume week count less than this in the previous epoch */
#define GPSWEEKS        1024                       /* number of weeks until the GPS epch rolls over */

/*
 * state flags
 */
#define PARSEB_POWERUP            0x00000001U /* no synchronisation */
#define PARSEB_NOSYNC             0x00000002U /* timecode currently not confirmed */

/*
 * time zone information
 */
#define PARSEB_ANNOUNCE           0x00000010U /* switch time zone warning (DST switch) */
#define PARSEB_DST                0x00000020U /* DST in effect */
#define PARSEB_UTC		  0x00000040U /* UTC time */

/*
 * leap information
 */
#define PARSEB_LEAPDEL		  0x00000100U /* LEAP deletion warning */
#define PARSEB_LEAPADD		  0x00000200U /* LEAP addition warning */
#define PARSEB_LEAPS		  0x00000300U /* LEAP warnings */
#define PARSEB_LEAPSECOND	  0x00000400U /* actual leap second */
/*
 * optional status information
 */
#define PARSEB_CALLBIT		  0x00001000U /* "call bit" used to signalize irregularities in the control facilities */
#define PARSEB_POSITION		  0x00002000U /* position available */
#define PARSEB_MESSAGE            0x00004000U /* addtitional message data */
/*
 * feature information
 */
#define PARSEB_S_LEAP		  0x00010000U /* supports LEAP */
#define PARSEB_S_CALLBIT	  0x00020000U /* supports callbit information */
#define PARSEB_S_PPS     	  0x00040000U /* supports PPS time stamping */
#define PARSEB_S_POSITION	  0x00080000U /* supports position information (GPS) */

/*
 * time stamp availability
 */
#define PARSEB_TIMECODE		  0x10000000U /* valid time code sample */
#define PARSEB_PPS		  0x20000000U /* valid PPS sample */

#define PARSE_TCINFO		(PARSEB_ANNOUNCE|PARSEB_POWERUP|PARSEB_NOSYNC|PARSEB_DST|\
				 PARSEB_UTC|PARSEB_LEAPS|PARSEB_CALLBIT|PARSEB_S_LEAP|\
				 PARSEB_S_LOCATION|PARSEB_TIMECODE|PARSEB_MESSAGE)

#define PARSE_POWERUP(x)        ((x) & PARSEB_POWERUP)
#define PARSE_NOSYNC(x)         (((x) & (PARSEB_POWERUP|PARSEB_NOSYNC)) == PARSEB_NOSYNC)
#define PARSE_SYNC(x)           (((x) & (PARSEB_POWERUP|PARSEB_NOSYNC)) == 0)
#define PARSE_ANNOUNCE(x)       ((x) & PARSEB_ANNOUNCE)
#define PARSE_DST(x)            ((x) & PARSEB_DST)
#define PARSE_UTC(x)		((x) & PARSEB_UTC)
#define PARSE_LEAPADD(x)	(PARSE_SYNC(x) && (((x) & PARSEB_LEAPS) == PARSEB_LEAPADD))
#define PARSE_LEAPDEL(x)	(PARSE_SYNC(x) && (((x) & PARSEB_LEAPS) == PARSEB_LEAPDEL))
#define PARSE_CALLBIT(x)	((x) & PARSEB_CALLBIT)
#define PARSE_LEAPSECOND(x)	(PARSE_SYNC(x) && ((x) & PARSEB_LEAP_SECOND))

#define PARSE_S_LEAP(x)		((x) & PARSEB_S_LEAP)
#define PARSE_S_CALLBIT(x)	((x) & PARSEB_S_CALLBIT)
#define PARSE_S_PPS(x)		((x) & PARSEB_S_PPS)
#define PARSE_S_POSITION(x)	((x) & PARSEB_S_POSITION)

#define PARSE_TIMECODE(x)	((x) & PARSEB_TIMECODE)
#define PARSE_PPS(x)		((x) & PARSEB_PPS)
#define PARSE_POSITION(x)	((x) & PARSEB_POSITION)
#define PARSE_MESSAGE(x)	((x) & PARSEB_MESSAGE)

/*
 * operation flags - lower nibble contains driver option flags
 */
#define PARSE_TRUSTTIME     CLK_FLAG1  /* use flag1 to indicate the time2 references mean the trust time */
#define PARSE_CLEAR         CLK_FLAG2  /* use flag2 to control pps on assert */
#define PARSE_PPSKERNEL     CLK_FLAG3  /* use flag3 to bind PPS to kernel */
#define PARSE_LEAP_DELETE   CLK_FLAG4  /* use flag4 to force leap deletion - only necessary when earth slows down */

#define PARSE_FIXED_FMT     0x10  /* fixed format */
#define PARSE_PPSAPI        0x20  /* try to get PPS time stamp via API */

/*
 * size of buffers
 */
#define PARSE_TCMAX	    400	  /* maximum addition data size */

typedef l_fp timestamp_t;

/*
 * standard time stamp structure
 */
struct parsetime
{
  unsigned long  parse_status;	/* data status - CVT_OK, CVT_NONE, CVT_FAIL ... */
  timestamp_t	 parse_time;	/* PARSE timestamp */
  timestamp_t	 parse_stime;	/* telegram sample timestamp */
  timestamp_t	 parse_ptime;	/* PPS time stamp */
  long           parse_usecerror;	/* sampled usec error */
  unsigned long	 parse_state;	/* current receiver state */
  unsigned short parse_format;	/* format code */
  unsigned short parse_msglen;	/* length of message */
  unsigned char  parse_msg[PARSE_TCMAX]; /* original messages */
};

typedef struct parsetime parsetime_t;

/*------ IO handling flags (sorry) ------*/

#define PARSE_IO_CSIZE	0x00000003
#define PARSE_IO_CS5	0x00000000
#define PARSE_IO_CS6	0x00000001
#define PARSE_IO_CS7	0x00000002
#define PARSE_IO_CS8	0x00000003

/*
 * ioctl structure
 */
union parsectl
{
  struct parsegettc
    {
      unsigned long         parse_state;	/* last state */
      unsigned long         parse_badformat; /* number of bad packets since last query */
      unsigned short parse_format;/* last decoded format */
      unsigned short parse_count;	/* count of valid time code bytes */
      char           parse_buffer[PARSE_TCMAX+1]; /* timecode buffer */
    } parsegettc;

  struct parseformat
    {
      unsigned short parse_format;/* number of examined format */
      unsigned short parse_count;	/* count of valid string bytes */
      char           parse_buffer[PARSE_TCMAX+1]; /* format code string */
    } parseformat;

  struct parsesetcs
    {
      unsigned long         parse_cs;	/* character size (needed for stripping) */
    } parsesetcs;
};

typedef union parsectl parsectl_t;

/*------ for conversion routines --------*/

struct parse			/* parse module local data */
{
  int            parse_flags;	/* operation and current status flags */

  int		 parse_ioflags;	   /* io handling flags (5-8 Bit control currently) */

  /*
   * private data - fixed format only
   */
  unsigned short parse_plen;	/* length of private data */
  void          *parse_pdata;	/* private data pointer */

  /*
   * time code input buffer (from RS232 or PPS)
   */
  unsigned short parse_index;	/* current buffer index */
  char          *parse_data;    /* data buffer */
  unsigned short parse_dsize;	/* size of data buffer */
  unsigned short parse_lformat;	/* last format used */
  unsigned long  parse_lstate;	/* last state code */
  char          *parse_ldata;	/* last data buffer */
  unsigned short parse_ldsize;	/* last data buffer length */
  unsigned long  parse_badformat;	/* number of unparsable pakets */

  timestamp_t    parse_lastchar; /* last time a character was received */
  parsetime_t    parse_dtime;	/* external data prototype */
};

typedef struct parse parse_t;

struct clocktime		/* clock time broken up from time code */
{
  long day;
  long month;
  long year;
  long hour;
  long minute;
  long second;
  long usecond;
  long utcoffset;	/* in seconds */
  time_t utctime;	/* the actual time - alternative to date/time */
  unsigned long flags;	/* current clock status */
};

typedef struct clocktime clocktime_t;

/*
 * parser related return/error codes
 */
#define CVT_MASK	 (unsigned)0x0000000F /* conversion exit code */
#define   CVT_NONE	 (unsigned)0x00000001 /* format not applicable */
#define   CVT_FAIL	 (unsigned)0x00000002 /* conversion failed - error code returned */
#define   CVT_OK	 (unsigned)0x00000004 /* conversion succeeded */
#define   CVT_SKIP	 (unsigned)0x00000008 /* conversion succeeded */
#define CVT_ADDITIONAL   (unsigned)0x00000010 /* additional data is available */
#define CVT_BADFMT	 (unsigned)0x00000100 /* general format error - (unparsable) */
#define CVT_BADDATE      (unsigned)0x00000200 /* date field incorrect */
#define CVT_BADTIME	 (unsigned)0x00000400 /* time field incorrect */

/*
 * return codes used by special input parsers
 */
#define PARSE_INP_SKIP  0x00	/* discard data - may have been consumed */
#define PARSE_INP_TIME  0x01	/* time code assembled */
#define PARSE_INP_PARSE 0x02	/* parse data using normal algorithm */
#define PARSE_INP_DATA  0x04	/* additional data to pass up */
#define PARSE_INP_SYNTH 0x08	/* just pass up synthesized time */

typedef unsigned long parse_inp_fnc_t(parse_t *, char, timestamp_t *);
typedef unsigned long parse_cvt_fnc_t(unsigned char *, int, struct format *, clocktime_t *, void *);
typedef unsigned long parse_pps_fnc_t(parse_t *, int, timestamp_t *);

struct clockformat
{
  /* special input protocol - implies fixed format */
  parse_inp_fnc_t *input;
  /* conversion routine */
  parse_cvt_fnc_t *convert;
  /* routine for handling RS232 sync events (time stamps) */
  /* PPS input routine */
  parse_pps_fnc_t *syncpps;
  /* time code synthesizer */

  void           *data;		/* local parameters */
  const char     *name;		/* clock format name */
  unsigned short  length;	/* maximum length of data packet */
  unsigned short  plen;		/* length of private data - implies fixed format */
};

typedef struct clockformat clockformat_t;

extern clockformat_t *clockformats[];
extern clockformat_t clock_computime;
extern clockformat_t clock_dcf7000;
extern clockformat_t clock_hopf6021;
extern clockformat_t clock_meinberg[];
extern clockformat_t clock_rawdcf;
extern clockformat_t clock_rcc8000;
extern clockformat_t clock_schmid;
extern clockformat_t clock_sel240x;
extern clockformat_t clock_trimtaip;
extern clockformat_t clock_trimtsip;
extern clockformat_t clock_varitext;
extern clockformat_t clock_wharton_400a;
extern unsigned short nformats;



/*
 * parse interface
 */
extern bool  parse_ioinit (parse_t *);
extern void parse_ioend (parse_t *);
extern int  parse_ioread (parse_t *, char, timestamp_t *);
extern void parse_iodone (parse_t *);
extern bool  parse_timecode (parsectl_t *, parse_t *);
extern int  parse_getfmt (parsectl_t *, parse_t *);
extern bool  parse_setfmt (parsectl_t *, parse_t *);
extern bool  parse_setcs (parsectl_t *, parse_t *);

extern unsigned int parse_addchar (parse_t *, char);
extern unsigned int parse_end (parse_t *);

extern int Strok (const unsigned char *, const unsigned char *)
		__attribute__((pure));
extern int Stoi (const unsigned char *, long *, int);

extern time_t parse_to_unixtime (clocktime_t *, unsigned long *);
extern unsigned long updatetimeinfo (parse_t *, unsigned long);
extern parse_pps_fnc_t pps_one;
extern bool parse_timedout (parse_t *, timestamp_t *, struct timespec *);

#endif

/*
 * History:
 *
 * parse.h,v
 * Revision 4.12  2007/01/14 08:36:03  kardel
 * make timestamp union anonymous to avoid conflicts with
 * some OSes that choose to create a nameing conflic here.
 *
 * Revision 4.11  2005/06/25 10:58:45  kardel
 * add missing log keywords
 *
 * Revision 4.5  1998/08/09 22:23:32  kardel
 * 4.0.73e2 adjustments
 *
 * Revision 4.4  1998/06/14 21:09:27  kardel
 * Sun acc cleanup
 *
 * Revision 4.3  1998/06/13 11:49:25  kardel
 * STREAM macro gone in favor of HAVE_SYS_STREAM_H
 *
 * Revision 4.2  1998/06/12 15:14:25  kardel
 * fixed prototypes
 *
 * Revision 4.1  1998/05/24 10:07:59  kardel
 * removed old data structure cruft (new input model)
 * new PARSE_INP* macros for input handling
 * removed old SYNC_* macros from old input model
 * (struct clockformat): removed old parse functions in favor of the
 * new input model
 * updated prototypes
 *
 * form V3 3.31 - log info deleted 1998/04/11 kardel
 */
