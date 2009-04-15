/*
 * getclock.c - Emulate Unix getclock(3) nanosecond interface for libntp/ntpd
 */
#include "config.h"
#include "clockstuff.h"
#include "ntp_stdlib.h"

/*
 * getclock() is in libntp.  To use interpolation, 
 * ports/winnt/ntpd/nt_clockstuff.c overrides GetSystemTimeAsFileTime 
 * via the pointer get_sys_time_as_filetime.
 */
PGSTAFT get_sys_time_as_filetime;

int
getclock(
	int		clktyp,
	struct timespec *ts
	)
{
	union {
		FILETIME ft;
		ULONGLONG ull;
	} uNow;

	if (clktyp != TIMEOFDAY) {
#ifdef DEBUG
		if (debug) {
			printf("getclock() supports only TIMEOFDAY clktyp\n");
		}
#endif
		errno = EINVAL;
		return -1;
	}

	if (! get_sys_time_as_filetime)
		get_sys_time_as_filetime = GetSystemTimeAsFileTime;

	(*get_sys_time_as_filetime)(&uNow.ft);

	/* 
	 * Convert the hecto-nano second time to timespec format
	 */
	uNow.ull -= FILETIME_1970;
	ts->tv_sec = (time_t)( uNow.ull / HECTONANOSECONDS);
	ts->tv_nsec = (long)(( uNow.ull % HECTONANOSECONDS) * 100);

	return 0;
}
