
#include "clockstuff.h"

char *	set_tod_using = "SetSystemTime";

int
ntp_set_tod(
	struct timeval *tv,
	void *tzp
	)
{
	SYSTEMTIME st;
	struct tm *gmtm;
	long x = tv->tv_sec;
	long y = tv->tv_usec;
	(void) tzp;

	gmtm = gmtime((const time_t *) &x);
	st.wSecond		= (WORD) gmtm->tm_sec;
	st.wMinute		= (WORD) gmtm->tm_min;
	st.wHour		= (WORD) gmtm->tm_hour;
	st.wDay 		= (WORD) gmtm->tm_mday;
	st.wMonth		= (WORD) (gmtm->tm_mon	+ 1);
	st.wYear		= (WORD) (gmtm->tm_year + 1900);
	st.wDayOfWeek		= (WORD) gmtm->tm_wday;
	st.wMilliseconds	= (WORD) (y / 1000);

	if (!SetSystemTime(&st)) {
		msyslog(LOG_ERR, "SetSystemTime failed: %m\n");
		return -1;
	}
	return 0;
}
