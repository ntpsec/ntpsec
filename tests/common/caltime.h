#include <time.h>

#include "ntp_calendar.h"

time_t settime(int y, int m, int d, int H, int M, int S);
const char *CalendarToString(const struct calendar *cal);
