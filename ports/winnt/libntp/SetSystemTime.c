
#include "clockstuff.h"
#include "ntp_stdlib.h"

const char *set_tod_using = "SetSystemTime";

time_stepped_callback	step_callback = NULL;

int
ntp_set_tod(
	struct timeval *tv,
	void *tzp
	)
{
	SYSTEMTIME st;
	union {
		FILETIME ft;
		ULONGLONG ull;
	} t;

	UNUSED_ARG(tzp);

	t.ull = FILETIME_1970 +
		(ULONGLONG)tv->tv_sec * 10 * 1000 * 1000 +
		(ULONGLONG)tv->tv_usec * 10;

	if (!FileTimeToSystemTime(&t.ft, &st) || !SetSystemTime(&st)) {
		msyslog(LOG_ERR, "SetSystemTime failed: %m\n");
		return -1;
	}

	if (step_callback)
		(*step_callback)();

	return 0;
}
