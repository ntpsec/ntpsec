/*
 * Parser configuration module for reference clocks
 *
 * Copyright (c) 1989-2005 by Frank Kardel <kardel@ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "config.h"
#include "ntp_fp.h"
#include "ntp_calendar.h"

#include "parse.h"

/*
 * format definitions
 */
clockformat_t *clockformats[] =
{
#ifdef CLOCK_MEINBERG
	&clock_meinberg[0],
	&clock_meinberg[1],
	&clock_meinberg[2],
#endif
#ifdef CLOCK_DCF7000
	&clock_dcf7000,
#endif
#ifdef CLOCK_SCHMID
	&clock_schmid,
#endif
#ifdef CLOCK_RAWDCF
	&clock_rawdcf,
#endif
#ifdef CLOCK_TRIMTAIP
	&clock_trimtaip,
#endif
#ifdef CLOCK_TRIMTSIP
	&clock_trimtsip,
#endif
#ifdef CLOCK_RCC8000
	&clock_rcc8000,
#endif
#ifdef CLOCK_HOPF6021
	&clock_hopf6021,
#endif
#ifdef CLOCK_COMPUTIME
	&clock_computime,
#endif
#ifdef CLOCK_WHARTON_400A
	&clock_wharton_400a,
#endif
#ifdef CLOCK_VARITEXT
        &clock_varitext,
#endif
#ifdef CLOCK_SEL240X
        &clock_sel240x,
#endif
	0};

unsigned short nformats = sizeof(clockformats) / sizeof(clockformats[0]) - 1;


/*
 * History:
 *
 * parse_conf.c,v
 * Revision 4.9  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 4.8  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.5  1999/11/28 09:13:53  kardel
 * RECON_4_0_98F
 *
 * Revision 4.4  1999/02/28 15:27:25  kardel
 * wharton clock integration
 *
 * Revision 4.3  1998/08/16 18:52:15  kardel
 * (clockformats): Trimble TSIP driver now also
 * available for kernel operation
 *
 * Revision 4.2  1998/06/12 09:13:48  kardel
 * conditional compile macros fixed
 *
 * Revision 4.1  1998/05/24 09:40:49  kardel
 * adjustments of log messages
 *
 *
 * from V3 3.24 log info deleted 1998/04/11 kardel
 */
