#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clockstuff.h"

DWORD units_per_tick = 0;

int
gettimeofday(
	struct timeval *tv
	)
{
	/*  Use the system time (roughly synchronised to the tick, and
	 *  extrapolated using the system performance counter.
	 */

	FILETIME StartTime;
	ULONGLONG Time;

	GetSystemTimeAsFileTime(&StartTime);
	Time = (((ULONGLONG) StartTime.dwHighDateTime) << 32) + 
		(ULONGLONG) StartTime.dwLowDateTime;

	/* Convert the hecto-nano second time to tv format
	 */
	Time -= FILETIME_1970;
	tv->tv_sec = (LONG) ( Time / 10000000ui64);
	tv->tv_usec = (LONG) (( Time % 10000000ui64) / 10);

	return 0;
}
