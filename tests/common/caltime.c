#include "config.h"
#include <time.h>

#include "caltime.h"
#include "ntp_stdlib.h"
#include "ntp_calendar.h"

time_t nowtime = 0;

time_t settime(int y, int m, int d, int H, int M, int S) {

	time_t days = ntpcal_edate_to_eradays(y-1, m-1, d-1) + (1 - DAY_UNIX_STARTS);
	time_t secs = ntpcal_etime_to_seconds(H, M, S);

	return days * SECSPERDAY + secs;
}


const char *CalendarToString(const struct calendar *cal) {
	char *str = malloc(255);
	snprintf(str, 255, "%hu-%u-%u (%u) %u:%u:%u\n", cal->year, (unsigned int)cal->month, (unsigned int)cal->monthday, cal->yearday, (unsigned int)cal->hour, (unsigned int)cal->minute, (unsigned int)cal->second);

	return str;
}

