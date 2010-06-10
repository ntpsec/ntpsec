#include "libntptest.h"

/* This file contains various constants that libntp needs to be set
 *  and that is normally defined in ntpd/ntpq/...
 */

u_long current_time = 4; // needed by authkeys. Used only in to calculate lifetime.
volatile int debug = 0;
const char *progname = "libntptest";
