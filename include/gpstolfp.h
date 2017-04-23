#include "ntp_calendar.h"
#include "ntp_fp.h"
#include "parse.h"

#define MIN_BUILD_GPSWEEK  (1900) /* minimum GPS week number of compile*/
#define MAX_BUILD_GPSWEEK (6078) /* testcase limit */

extern void gpstolfp (int weeks, int days, unsigned long seconds, l_fp *);
extern void gpsweekadj (u_int * week, u_int build_week);
extern void gpstocal (u_int week, u_int TOW, int UTC_offset, struct calendar *);
extern void caltogps (const struct calendar *, int UTC_offset, u_int * week, u_int * TOW);