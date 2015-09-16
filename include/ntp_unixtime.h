/*
 * ntp_unixtime.h - much of what was here is now in timevalops.h
 */

#ifndef GUARD_NTP_UNIXTIME_H
#define GUARD_NTP_UNIXTIME_H

#include "ntp_types.h"	/* picks up time.h via ntp_machine.h */
#include "ntp_calendar.h"

/*
 * Time of day conversion constant.  Ntp's time scale starts in 1900,
 * Unix in 1970.  The value is 1970 - 1900 in seconds, 0x83aa7e80 or
 * 2208988800.  This is larger than 32-bit INT_MAX, so unsigned
 * type is forced.
 */
#define	JAN_1970 ((u_int)NTP_TO_UNIX_DAYS * (u_int)SECSPERDAY)

#endif /* GUARD_!defined(NTP_UNIXTIME_H) */
