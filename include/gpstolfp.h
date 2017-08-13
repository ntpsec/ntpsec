#include "ntp_calendar.h"
#include "ntp_fp.h"
#include "parse.h"

#define MIN_BUILD_GPSWEEK  (1900) /* minimum GPS week number of compile*/
#define MAX_BUILD_GPSWEEK (6078) /* testcase limit */

extern void gpstolfp (int weeks, int days, unsigned long seconds, l_fp *);
extern void gpsweekadj (unsigned int * week, unsigned int build_week);
extern void gpstocal (unsigned int week, unsigned int TOW, int UTC_offset, struct calendar *);
extern void caltogps (const struct calendar *, int UTC_offset, unsigned int * week, unsigned int * TOW);
