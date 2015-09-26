#include "config.h"

#include "ntp_syslog.h"
#include "ntp_stdlib.h"
#include "ntp_unixtime.h"
#include "clockstuff.h"

int
gettimeofday(
	struct timeval *	tv,
	void *			ignored
	)
{
	struct timespec ts;

	UNUSED_ARG(ignored);

	getclock(TIMEOFDAY, &ts);

	tv->tv_sec = ts.tv_sec;
	tv->tv_usec = ts.tv_nsec / 10;

	return 0;
}
