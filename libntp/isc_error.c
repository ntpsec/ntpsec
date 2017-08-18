/*
 * Copyright (C) 2004, 2005, 2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1998-2001  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

/*! \file */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "ntp_config.h"
#include "ntp_syslog.h"

#include "isc_error.h"


#define MAX_UNEXPECTED_ERRORS 100
void
isc_error_unexpected(const char *file, int line, const char *format, ...) {
        va_list args;
        char errbuf[256];
        static int unexpected_error_cnt = 0;

        if (unexpected_error_cnt >= MAX_UNEXPECTED_ERRORS)
                return; /* avoid clutter in log */

        va_start(args, format);

        msyslog(LOG_ERR, "ERR: %s:%d: unexpected error:", file, line);
        vsnprintf(errbuf, sizeof(errbuf), format, args);
        msyslog(LOG_ERR, "%s", errbuf);

        if (++unexpected_error_cnt == MAX_UNEXPECTED_ERRORS)
                msyslog(LOG_ERR, "ERR: Too many errors.  Shutting up.");
        va_end(args);
}
