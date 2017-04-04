/*
 * Created: Sun Jul 20 12:08:14 1997
 *
 * Copyright (c) 1997-2005 by Frank Kardel <kardel <AT> ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "config.h"
#include <stdio.h>
#include "ntp_types.h"
#include "ntp_stdlib.h"
#include "ntp_fp.h"
#include "ntp_calendar.h"
#include "mbg_gps166.h"
#include "binio.h"
#include "ieee754io.h"

static void get_mbg_tzname (unsigned char **, char *);
static void mbg_time_status_str (char **, unsigned int, size_t);

#if 0				/* no actual floats on Meinberg binary interface */
static offsets_t mbg_float  = { 1, 0, 3, 2, 0, 0, 0, 0 }; /* byte order for meinberg floats */
#endif
static offsets_t mbg_double = { 1, 0, 3, 2, 5, 4, 7, 6 }; /* byte order for meinberg doubles */

#define RAD2DEG	57.2957795131	/* 180/PI */

void
put_mbg_header(
	unsigned char **bufpp,
	GPS_MSG_HDR *headerp
	)
{
  put_lsb_short(bufpp, headerp->cmd);
  put_lsb_short(bufpp, headerp->len);
  put_lsb_short(bufpp, headerp->data_csum);
  put_lsb_short(bufpp, headerp->hdr_csum);
}

void
get_mbg_sw_rev(
	unsigned char **bufpp,
	SW_REV *sw_revp
	)
{
  sw_revp->code = get_lsb_uint16(bufpp);
  memcpy(sw_revp->name, *bufpp, sizeof(sw_revp->name));
  *bufpp += sizeof(sw_revp->name);
}

void
get_mbg_ascii_msg(
	unsigned char **bufpp,
	ASCII_MSG *ascii_msgp
	)
{
  ascii_msgp->csum  = (CSUM) get_lsb_short(bufpp);
  ascii_msgp->valid = get_lsb_int16(bufpp);
  memcpy(ascii_msgp->s, *bufpp, sizeof(ascii_msgp->s));
  *bufpp += sizeof(ascii_msgp->s);
}

void
get_mbg_svno(
	unsigned char **bufpp,
	SVNO *svnop
	)
{
  *svnop = (SVNO) get_lsb_short(bufpp);
}

void
get_mbg_health(
	unsigned char **bufpp,
	HEALTH *healthp
	)
{
  *healthp = (HEALTH) get_lsb_short(bufpp);
}

void
get_mbg_cfg(
	unsigned char **bufpp,
	CFG *cfgp
	)
{
  *cfgp = (CFG) get_lsb_short(bufpp);
}

void
get_mbg_tgps(
	unsigned char **bufpp,
	T_GPS *tgpsp
	)
{
  tgpsp->wn = get_lsb_uint16(bufpp);
  tgpsp->sec = get_lsb_ulong(bufpp);
  tgpsp->tick = get_lsb_ulong(bufpp);
}

void
get_mbg_tm(
	unsigned char **buffpp,
	TM_GPS *tmp
	)
{
  tmp->year = get_lsb_int16(buffpp);
  tmp->month = (int8_t)(*(*buffpp)++);
  tmp->mday = (int8_t)(*(*buffpp)++);
  tmp->yday = get_lsb_int16(buffpp);
  tmp->wday = (int8_t)(*(*buffpp)++);
  tmp->hour = (int8_t)(*(*buffpp)++);
  tmp->min = (int8_t)(*(*buffpp)++);
  tmp->sec = (int8_t)(*(*buffpp)++);
  tmp->frac = get_lsb_ulong(buffpp);
  tmp->offs_from_utc = get_lsb_ulong(buffpp);
  tmp->status = get_lsb_uint16(buffpp);
}

void
get_mbg_ttm(
	unsigned char **buffpp,
	TTM *ttmp
	)
{
  ttmp->channel = get_lsb_int16(buffpp);
  get_mbg_tgps(buffpp, &ttmp->t);
  get_mbg_tm(buffpp, &ttmp->tm);
}

void
get_mbg_synth(
	unsigned char **buffpp,
	SYNTH *synthp
	)
{
  synthp->freq  = get_lsb_int16(buffpp);
  synthp->range = get_lsb_int16(buffpp);
  synthp->phase = get_lsb_int16(buffpp);
}

static void
get_mbg_tzname(
	unsigned char **buffpp,
	char *tznamep
	)
{
  strlcpy(tznamep, (char *)*buffpp, sizeof(TZ_NAME));
  *buffpp += sizeof(TZ_NAME);
}

void
get_mbg_tzdl(
	unsigned char **buffpp,
	TZDL *tzdlp
	)
{
  tzdlp->offs = get_lsb_ulong(buffpp);
  tzdlp->offs_dl = get_lsb_ulong(buffpp);
  get_mbg_tm(buffpp, &tzdlp->tm_on);
  get_mbg_tm(buffpp, &tzdlp->tm_off);
  get_mbg_tzname(buffpp, (char *)tzdlp->name[0]);
  get_mbg_tzname(buffpp, (char *)tzdlp->name[1]);
}

void
get_mbg_antinfo(
	unsigned char **buffpp,
	ANT_INFO *antinfop
	)
{
  antinfop->status = get_lsb_int16(buffpp);
  get_mbg_tm(buffpp, &antinfop->tm_disconn);
  get_mbg_tm(buffpp, &antinfop->tm_reconn);
  antinfop->delta_t = get_lsb_ulong(buffpp);
}

static void
mbg_time_status_str(
	char **buffpp,
	unsigned int status,
	size_t size
	)
{
	static struct state
	{
		int         flag;       /* bit flag */
		const char *string;     /* bit name */
	} states[] =
		  {
			  { TM_UTC,    "UTC CORR" },
			  { TM_LOCAL,  "LOCAL TIME" },
			  { TM_DL_ANN, "DST WARN" },
			  { TM_DL_ENB, "DST" },
			  { TM_LS_ANN, "LEAP WARN" },
			  { TM_LS_ENB, "LEAP SEC" },
			  { 0, "" }
		  };

	if (status)
	{
		char *start, *p;
		struct state *s;

		start = p = *buffpp;

		for (s = states; s->flag; s++)
		{
			if ( (unsigned int)s->flag & status)
			{
				if (p != *buffpp)
				{
					strlcpy(p, ", ",
                                                size - (size_t)(p - start));
					p += 2;
				}
				strlcpy(p, s->string,
                                        size - (size_t)(p - start));
				p += strlen(p);
			}
		}
		*buffpp = p;
	}
}

void
mbg_tm_str(
	char **buffpp,
	TM_GPS *tmp,
	size_t size,
	int print_status
	)
{
	char *s = *buffpp;

	snprintf(*buffpp, size, "%04d-%02d-%02d %02d:%02d:%02d.%07ld (%c%02d%02d) ",
		 tmp->year, tmp->month, tmp->mday,
		 tmp->hour, tmp->min, tmp->sec, (long) tmp->frac,
		 (tmp->offs_from_utc < 0) ? '-' : '+',
		 abs((int)tmp->offs_from_utc) / 3600,
		 (abs((int)tmp->offs_from_utc) / 60) % 60);
	*buffpp += strlen(*buffpp);

	if (print_status)
		mbg_time_status_str(buffpp, tmp->status,
                                    size - (size_t)(*buffpp - s));
}

void
mbg_tgps_str(
	char **buffpp,
	T_GPS *tgpsp,
	size_t size
	)
{
	snprintf(*buffpp, size, "week %d + %ld days + %ld.%07ld sec",
		 tgpsp->wn, (long) tgpsp->sec / SECSPERDAY,
		 (long) tgpsp->sec % SECSPERDAY, (long) tgpsp->tick);
	*buffpp += strlen(*buffpp);
}

void
get_mbg_cfgh(
	unsigned char **buffpp,
	CFGH *cfghp
	)
{
  int i;

  cfghp->csum = (CSUM) get_lsb_short(buffpp);
  cfghp->valid = get_lsb_int16(buffpp);
  get_mbg_tgps(buffpp, &cfghp->tot_51);
  get_mbg_tgps(buffpp, &cfghp->tot_63);
  get_mbg_tgps(buffpp, &cfghp->t0a);

  for (i = 0; i < N_SVNO_GPS; i++)
    {
      get_mbg_cfg(buffpp, &cfghp->cfg[i]);
    }

  for (i = 0; i < N_SVNO_GPS; i++)
    {
      get_mbg_health(buffpp, &cfghp->health[i]);
    }
}

void
get_mbg_utc(
	unsigned char **buffpp,
	UTC *utcp
	)
{
  utcp->csum  = (CSUM) get_lsb_short(buffpp);
  utcp->valid = get_lsb_int16(buffpp);

  get_mbg_tgps(buffpp, &utcp->t0t);

  if (fetch_ieee754(buffpp, IEEE_DOUBLE, &utcp->A0, mbg_double) != IEEE_OK)
    {
      utcp->A0 = 0;
    }

  if (fetch_ieee754(buffpp, IEEE_DOUBLE, &utcp->A1, mbg_double) != IEEE_OK)
    {
      utcp->A1 = 0;
    }

  utcp->WNlsf      = get_lsb_uint16(buffpp);
  utcp->DNt        = get_lsb_int16(buffpp);
  utcp->delta_tls  = (int8_t)(*(*buffpp)++);
  utcp->delta_tlsf = (int8_t)(*(*buffpp)++);
}

void
get_mbg_lla(
	unsigned char **buffpp,
	LLA lla
	)
{
  int i;

  for (i = LAT; i <= ALT; i++)
    {
      if  (fetch_ieee754(buffpp, IEEE_DOUBLE, &lla[i], mbg_double) != IEEE_OK)
      	lla[i] = 0;
      else
	if (i != ALT)
	      lla[i] *= RAD2DEG;	/* convert to degrees (* 180/PI) */
    }
}

void
get_mbg_xyz(
	unsigned char **buffpp,
	XYZ xyz
	)
{
  int i;

  for (i = XP; i <= ZP; i++)
    {
      if  (fetch_ieee754(buffpp, IEEE_DOUBLE, &xyz[i], mbg_double) != IEEE_OK)
	{
	  xyz[i] = 0;
	}
    }
}

static void
get_mbg_comparam(
	unsigned char **buffpp,
	COM_PARM *comparamp
	)
{
  size_t i;

  comparamp->baud_rate = get_lsb_ulong(buffpp);
  for (i = 0; i < sizeof(comparamp->framing); i++)
    {
      comparamp->framing[i] = (char)(*(*buffpp)++);
    }
  comparamp->handshake = get_lsb_int16(buffpp);
}

void
get_mbg_portparam(
	unsigned char **buffpp,
	PORT_PARM *portparamp
	)
{
  int i;

  for (i = 0; i < DEFAULT_N_COM; i++)
    {
      get_mbg_comparam(buffpp, &portparamp->com[i]);
    }
  for (i = 0; i < DEFAULT_N_COM; i++)
    {
      portparamp->mode[i] = *(*buffpp)++;
    }
}

#define FETCH_DOUBLE(src, addr)							\
	if  (fetch_ieee754(src, IEEE_DOUBLE, addr, mbg_double) != IEEE_OK)	\
	{									\
	  *addr = 0;								\
	}

void
get_mbg_eph(
	unsigned char ** buffpp,
	EPH *ephp
	)
{
  ephp->csum   = (CSUM) get_lsb_short(buffpp);
  ephp->valid  = get_lsb_int16(buffpp);

  ephp->health = (HEALTH) get_lsb_short(buffpp);
  ephp->IODC   = (IOD) get_lsb_short(buffpp);
  ephp->IODE2  = (IOD) get_lsb_short(buffpp);
  ephp->IODE3  = (IOD) get_lsb_short(buffpp);

  get_mbg_tgps(buffpp, &ephp->tt);
  get_mbg_tgps(buffpp, &ephp->t0c);
  get_mbg_tgps(buffpp, &ephp->t0e);

  FETCH_DOUBLE(buffpp, &ephp->sqrt_A);
  FETCH_DOUBLE(buffpp, &ephp->e);
  FETCH_DOUBLE(buffpp, &ephp->M0);
  FETCH_DOUBLE(buffpp, &ephp->omega);
  FETCH_DOUBLE(buffpp, &ephp->OMEGA0);
  FETCH_DOUBLE(buffpp, &ephp->OMEGADOT);
  FETCH_DOUBLE(buffpp, &ephp->deltan);
  FETCH_DOUBLE(buffpp, &ephp->i0);
  FETCH_DOUBLE(buffpp, &ephp->idot);
  FETCH_DOUBLE(buffpp, &ephp->crc);
  FETCH_DOUBLE(buffpp, &ephp->crs);
  FETCH_DOUBLE(buffpp, &ephp->cuc);
  FETCH_DOUBLE(buffpp, &ephp->cus);
  FETCH_DOUBLE(buffpp, &ephp->cic);
  FETCH_DOUBLE(buffpp, &ephp->cis);

  FETCH_DOUBLE(buffpp, &ephp->af0);
  FETCH_DOUBLE(buffpp, &ephp->af1);
  FETCH_DOUBLE(buffpp, &ephp->af2);
  FETCH_DOUBLE(buffpp, &ephp->tgd);

  ephp->URA = get_lsb_uint16(buffpp);

  ephp->L2code = *(*buffpp)++;
  ephp->L2flag = *(*buffpp)++;
}

void
get_mbg_alm(
	unsigned char **buffpp,
	ALM *almp
	)
{
  almp->csum   = (CSUM) get_lsb_short(buffpp);
  almp->valid  = get_lsb_int16(buffpp);

  almp->health = (HEALTH) get_lsb_short(buffpp);
  get_mbg_tgps(buffpp, &almp->t0a);


  FETCH_DOUBLE(buffpp, &almp->sqrt_A);
  FETCH_DOUBLE(buffpp, &almp->e);

  FETCH_DOUBLE(buffpp, &almp->M0);
  FETCH_DOUBLE(buffpp, &almp->omega);
  FETCH_DOUBLE(buffpp, &almp->OMEGA0);
  FETCH_DOUBLE(buffpp, &almp->OMEGADOT);
  FETCH_DOUBLE(buffpp, &almp->deltai);
  FETCH_DOUBLE(buffpp, &almp->af0);
  FETCH_DOUBLE(buffpp, &almp->af1);
}

void
get_mbg_iono(
	unsigned char **buffpp,
	IONO *ionop
	)
{
  ionop->csum   = (CSUM) get_lsb_short(buffpp);
  ionop->valid  = get_lsb_int16(buffpp);

  FETCH_DOUBLE(buffpp, &ionop->alpha_0);
  FETCH_DOUBLE(buffpp, &ionop->alpha_1);
  FETCH_DOUBLE(buffpp, &ionop->alpha_2);
  FETCH_DOUBLE(buffpp, &ionop->alpha_3);

  FETCH_DOUBLE(buffpp, &ionop->beta_0);
  FETCH_DOUBLE(buffpp, &ionop->beta_1);
  FETCH_DOUBLE(buffpp, &ionop->beta_2);
  FETCH_DOUBLE(buffpp, &ionop->beta_3);
}

/*
 * data_mbg.c,v
 * Revision 4.8  2006/06/22 18:40:01  kardel
 * clean up signedness (gcc 4)
 *
 * Revision 4.7  2005/10/07 22:11:10  kardel
 * bounded buffer implementation
 *
 * Revision 4.6.2.1  2005/09/25 10:23:06  kardel
 * support bounded buffers
 *
 * Revision 4.6  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 4.5  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.3  1999/02/21 12:17:42  kardel
 * 4.91f reconciliation
 *
 * Revision 4.2  1998/06/14 21:09:39  kardel
 * Sun acc cleanup
 *
 * Revision 4.1  1998/05/24 08:02:06  kardel
 * trimmed version log
 *
 * Revision 4.0  1998/04/10 19:45:33  kardel
 * Start 4.0 release version numbering
 */

