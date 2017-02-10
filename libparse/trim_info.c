/*
 * Created: Sun Aug  2 20:20:34 1998
 *
 * Copyright (c) 1989-2005 by Frank Kardel <kardel@ntp.org>
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "config.h"
#include "ntp_types.h"
#include "trimble.h"

cmd_info_t *
trimble_convert(
		unsigned int cmd,
		cmd_info_t   *tbl
		)
{
  int i;

  for (i = 0; tbl[i].cmd != 0xFF; i++)
    {
      if (tbl[i].cmd == cmd)
	return &tbl[i];
    }
  return 0;
}

/*
 * trim_info.c,v
 * Revision 4.5  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 4.4  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.2  1998/12/20 23:45:31  kardel
 * fix types and warnings
 *
 * Revision 4.1  1998/08/09 22:27:48  kardel
 * Trimble TSIP support
 *
 */
