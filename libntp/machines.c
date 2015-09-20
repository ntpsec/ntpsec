/* machines.c - provide special support for peculiar architectures
 *
 * Real bummers unite !
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ntp.h"
#include "ntp_machine.h"
#include "ntp_syslog.h"
#include "ntp_stdlib.h"
#include "ntp_unixtime.h"
#include "lib_strbuf.h"
#include "ntp_debug.h"
#include <unistd.h>

#ifdef SYS_WINNT
int _getch(void);	/* Declare the one function rather than include conio.h */
#else

#define SET_TOD_UNDETERMINED	0
#define SET_TOD_CLOCK_SETTIME	1
#define SET_TOD_SETTIMEOFDAY	2
#define SET_TOD_STIME		3

const char * const set_tod_used[] = {
	"undetermined",
	"clock_settime",
	"settimeofday",
	"stime"
};

pset_tod_using	set_tod_using = NULL;


int
ntp_set_tod(
	struct timeval *tvp,
	void *tzp
	)
{
	static int	tod;
	int		rc;
	int		saved_errno;

	TRACE(1, ("In ntp_set_tod\n"));
	rc = -1;
	saved_errno = 0;

	if (rc && (SET_TOD_CLOCK_SETTIME == tod || !tod)) {
		struct timespec ts;

		/* Convert timeval to timespec */
		ts.tv_sec = tvp->tv_sec;
		ts.tv_nsec = 1000 *  tvp->tv_usec;

		errno = 0;
		rc = clock_settime(CLOCK_REALTIME, &ts);
		saved_errno = errno;
		TRACE(1, ("ntp_set_tod: clock_settime: %d %m\n", rc));
		if (!tod && !rc)
			tod = SET_TOD_CLOCK_SETTIME;

	}

	errno = saved_errno;	/* for %m below */
	TRACE(1, ("ntp_set_tod: Final result: %s: %d %m\n",
		  set_tod_used[tod], rc));
	/*
	 * Say how we're setting the time of day
	 */
	if (!rc && NULL != set_tod_using) {
		(*set_tod_using)(set_tod_used[tod]);
		set_tod_using = NULL;
	}

	if (rc)
		errno = saved_errno;

	return rc;
}

#endif /* not SYS_WINNT */

#if defined (SYS_WINNT)
/* getpass is used in ntpq.c */

char *
getpass(const char * prompt)
{
	int c, i;
	static char password[32];

	fprintf(stderr, "%s", prompt);
	fflush(stderr);

	for (i=0; i<sizeof(password)-1 && ((c=_getch())!='\n' && c!='\r'); i++) {
		password[i] = (char) c;
	}
	password[i] = '\0';

	fputc('\n', stderr);
	fflush(stderr);

	return password;
}
#endif /* SYS_WINNT */
