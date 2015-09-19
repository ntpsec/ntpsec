/* ppsapi_timepps.h */

/*
 * This logic tries to get the timepps.h file from standard
 * location.
 */

#ifdef HAVE_TIMEPPS_H
# include <timepps.h>
#else
# ifdef HAVE_SYS_TIMEPPS_H
#  include <sys/timepps.h>
# endif
#endif
