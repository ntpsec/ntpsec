/*
 * definitions for NTP file generations support
 *
 * Copyright (C) 1992, 1996 by Rainer Pruy
 * Friedrich-Alexander Universitaet Erlangen-Nuernberg, Germany
 *
 * SPDX-License-Identifier: BSD-2-clause
 */
#ifndef GUARD_NTP_FILEGEN_H
#define GUARD_NTP_FILEGEN_H

#include "ntp_types.h"

/*
 * supported file generation types
 */

#define FILEGEN_NONE	255	/* no generations - use plain file name */
#define FILEGEN_PID	1	/* one filegen per process incarnation */
#define FILEGEN_DAY	2	/* one filegen per day */
#define FILEGEN_WEEK	3	/* one filegen per week */
#define FILEGEN_MONTH	4	/* one filegen per month */
#define FILEGEN_YEAR	5	/* one filegen per year */
#define FILEGEN_AGE	6	/* change filegen each FG_AGE_SECS */

/*
 * supported file generation flags
 */

#define FGEN_FLAG_LINK		0x01 /* make a link to base name */

#define FGEN_FLAG_ENABLED	0x80 /* set this to really create files	  */
				     /* without this, open is suppressed */

typedef struct filegen_tag {
	FILE *	fp;	/* file referring to current generation */
	char *	dir;	/* currently always statsdir */
	char *	fname;	/* filename prefix of generation file */
			/* must be malloced, will be fed to free() */
	time_t	id_lo;	/* lower bound of ident value */
	time_t	id_hi;	/* upper bound of ident value */
	uint8_t	type;	/* type of file generation */
	uint8_t	flag;	/* flags modifying processing of file generation */
} FILEGEN;

extern	void	filegen_setup	(FILEGEN *, time_t);
extern	void	filegen_config	(FILEGEN *, const char *, const char *,
				 unsigned int, unsigned int);
extern	void	filegen_statsdir(void);
extern	FILEGEN *filegen_get	(const char *);
extern	void	filegen_register (const char *, const char *, FILEGEN *);
#ifdef DEBUG
extern	void	filegen_unregister(const char *);
#endif

#endif	/* GUARD_NTP_FILEGEN_H */
