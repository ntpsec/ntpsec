/**************************************************************
 * Dummy Header for Unix to Windows NT portability
 * Created for NTP package
 **************************************************************/

#ifndef SYS_TIME_H
#define SYS_TIME_H

#include "ntp_types.h"
#include <time.h>
#include <sys/timeb.h>

extern int gettimeofday (struct timeval *);
extern int settimeofday (struct timeval *);

#endif /* SYS_TIME_H */
