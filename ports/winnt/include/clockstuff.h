#ifndef _CLOCKSTUFF_H
#define _CLOCKSTUFF_H

#include "ntp_machine.h"
#include <time.h>
#include <sys\timeb.h>

#include "ntp_syslog.h"


void init_winnt_time(void);
void reset_winnt_time(void);
void lock_thread_to_processor(HANDLE);

/* 100ns intervals between 1/1/1601 and 1/1/1970 as reported by
 * SystemTimeToFileTime()
 */

#define FILETIME_1970     0x019db1ded53e8000
#define HECTONANOSECONDS  10000000

/*
 * Multimedia Timer
 */

void set_mm_timer(int);

enum {
	MM_TIMER_LORES,
	MM_TIMER_HIRES
};

/*
 * Optional callback from libntp ntp_set_tod() to Windows ntpd code
 * in nt_clockstuff that needs to know.  Optional because other
 * libntp clients like ntpdate don't use it.
 */

typedef void (*time_stepped_callback)(void);
extern time_stepped_callback	step_callback;

/*
 * get_sys_time_as_filetime is a function pointer to
 * either GetSystemTimeAsFileTime provided by Windows
 * or ntpd's interpolating replacement.
 */

typedef void (WINAPI *PGSTAFT)(LPFILETIME pft);
extern PGSTAFT get_sys_time_as_filetime;

void lock_thread_to_processor(HANDLE);

#ifdef HAVE_PPSAPI
/*
 * ntp_timestamp_from_counter provides an interface for 
 * serialpps.sys counterstamps to be converted to 
 * interpolated timestamps.
 */

extern void ntp_timestamp_from_counter(l_fp *, ULONGLONG, ULONGLONG);
#endif

#endif
