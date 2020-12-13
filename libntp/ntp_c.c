/*
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Python binding for selected libntp library functions
 */

/* This include has to come early or we get warnings from redefining
 * _POSIX_C_SOURCE and _XOPEN_SOURCE on some systems.
 */
#include "config.h"

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_fp.h"
#include "ntp_stdlib.h"
#include "ntp_syslog.h"
#include "timespecops.h"

#include "ntp_config.h"
#include "ntp_assert.h"

#include "ntp_control.h"

#include "pymodule-mac.h"

void ntpc_setprogname(char*);
char *ntpc_prettydate(char*);
double ntpc_lfptofloat(char*);
int ntpc_set_tod(int, int);
bool ntpc_adj_systime(double);
bool ntpc_step_systime(double);

/* Don't include anything from OpenSSL */

const char *version = NTPSEC_VERSION_EXTENDED;
const char *progname = "libntpc";
int   SYS_TYPE = TYPE_SYS;
int  PEER_TYPE = TYPE_PEER;
int CLOCK_TYPE = TYPE_CLOCK;

/*
 * Client utility functions
 */

void
ntpc_setprogname(char *s)
{
	/*
	 * This function is only called from clients.  Therefore
	 * log to stderr rather than syslog, and suppress logfile
	 * impediments.  If we ever want finer-grained control, that
	 * will be easily implemented with additional arguments.
	 */
	syslogit = false;	/* don't log messages to syslog */
	termlogit = true;	/* duplicate to stdout/err */
	termlogit_pid = false;
	msyslog_include_timestamp = false;
	progname = strdup(s);
}

char *
ntpc_prettydate(char *s)
{
	l_fp ts;

	if (false == hextolfp(s+2, &ts)) {
		errno = EINVAL;
		return strdup("ERROR");
	}
	errno = 0;
	return prettydate(ts);
}

double
ntpc_lfptofloat(char *s)
{
	l_fp ts;
	struct timespec tt;

	if (false == hextolfp(s+2, &ts)) {
		errno = EINVAL;
		return -0;
	}
	errno = 0;
	tt = lfp_stamp_to_tspec(ts, time(NULL));
	return tt.tv_sec + (tt.tv_nsec * S_PER_NS);
}

int
ntpc_set_tod(int seconds, int fractional)
{
	struct timespec ts;
	ts.tv_sec = seconds;
	ts.tv_nsec = fractional;

	return ntp_set_tod(&ts);
}

bool
ntpc_adj_systime(double adjustment)
{
	return adj_systime(adjustment, adjtime) ? 1 : 0;
}

bool
ntpc_step_systime(double adjustment)
{
	doubletime_t full_adjustment;

	/*
	 * What we really want is for Python to parse a long double.
	 * As this is, it's a potential source of problems in the Python
	 * utilities if and when the time difference between the Unix epoch
	 * and now exceeds the range of a double.
	 */
	full_adjustment = adjustment;
	return step_systime(full_adjustment, ntp_set_tod);
}
