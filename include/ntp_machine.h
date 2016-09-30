/*
 * ntp_machine.h
 *
 * Collect all machine dependent idiosyncrasies in one place.
 *
 */

#ifndef GUARD_NTP_MACHINE_H
#define GUARD_NTP_MACHINE_H

#include <sys/time.h>
#include <time.h>

#ifndef CLOCK_REALTIME
/*
 * Pacify platforms that don't have a real clock_gettime(2),
 * notably Mac OS X.
 */
#define CLOCK_REALTIME	0
#define CLOCK_MONOTONIC	1
typedef int clockid_t;
int clock_gettime(clockid_t clock_id, struct timespec *tp);
#endif

int ntp_set_tod (struct timespec *tvs);

#ifdef NO_MAIN_ALLOWED
/* we have no main routines so lets make a plan */
#define CALL(callname, progname, callmain) \
	extern int callmain (int,char**); \
	void callname (a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10) \
		char *a0;  \
		char *a1;  \
		char *a2;  \
		char *a3;  \
		char *a4;  \
		char *a5;  \
		char *a6;  \
		char *a7;  \
		char *a8;  \
		char *a9;  \
		char *a10; \
	{ \
	  char *x[11]; \
	  int argc; \
	  char *argv[] = {progname,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}; \
	  int i; \
	  for (i=0;i<11;i++) \
	   x[i] = NULL; \
	  x[0] = a0; \
	  x[1] = a1; \
	  x[2] = a2; \
	  x[3] = a3; \
	  x[4] = a4; \
	  x[5] = a5; \
	  x[6] = a6; \
	  x[7] = a7; \
	  x[8] = a8; \
	  x[9] = a9; \
	  x[10] = a10; \
	  argc=1; \
	  for (i=0; i<11;i++) \
		if (x[i]) \
		{ \
		  argv[argc++] = x[i];	\
		} \
	 callmain(argc,argv);  \
	}
#endif /* NO_MAIN_ALLOWED */

#endif	/* GUARD_NTP_MACHINE_H */
