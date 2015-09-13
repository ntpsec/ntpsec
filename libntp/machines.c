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

#ifdef SYS_VXWORKS
#include "taskLib.h"
#include "sysLib.h"
#include "time.h"
#include "ntp_syslog.h"

/*	some translations to the world of vxWorkings -casey */
/* first some netdb type things */
#include "ioLib.h"
#include <socket.h>
int h_errno;

struct hostent *gethostbyname(char *name)
	{
	struct hostent *host1;
	h_errno = 0;					/* we are always successful!!! */
	host1 = (struct hostent *) malloc (sizeof(struct hostent));
	host1->h_name = name;
	host1->h_addrtype = AF_INET;
	host1->h_aliases = name;
	host1->h_length = 4;
	host1->h_addr_list[0] = (char *)hostGetByName (name);
	host1->h_addr_list[1] = NULL;
	return host1;
	}

struct hostent *gethostbyaddr(char *name, int size, int addr_type)
	{
	struct hostent *host1;
	h_errno = 0;  /* we are always successful!!! */
	host1 = (struct hostent *) malloc (sizeof(struct hostent));
	host1->h_name = name;
	host1->h_addrtype = AF_INET;
	host1->h_aliases = name;
	host1->h_length = 4;
	host1->h_addr_list = NULL;
	return host1;
	}

struct servent *getservbyname (char *name, char *type)
	{
	struct servent *serv1;
	serv1 = (struct servent *) malloc (sizeof(struct servent));
	serv1->s_name = "ntp";      /* official service name */
	serv1->s_aliases = NULL;	/* alias list */
	serv1->s_port = 123;		/* port # */
	serv1->s_proto = "udp";     /* protocol to use */
	return serv1;
	}

/* second
 * vxworks thinks it has insomnia
 * we have to sleep for number of seconds
 */

#define CLKRATE 	sysClkRateGet()

/* I am not sure how valid the granularity is - it is from G. Eger's port */
#define CLK_GRANULARITY  1		/* Granularity of system clock in usec	*/
								/* Used to round down # usecs/tick		*/
								/* On a VCOM-100, PIT gets 8 MHz clk,	*/
								/*	& it prescales by 32, thus 4 usec	*/
								/* on mv167, granularity is 1usec anyway*/
								/* To defeat rounding, set to 1 		*/
#define USECS_PER_SEC		MILLION		/* Microseconds per second	*/
#define TICK (((USECS_PER_SEC / CLKRATE) / CLK_GRANULARITY) * CLK_GRANULARITY)

/* emulate unix sleep
 * casey
 */
void sleep(int seconds)
	{
	taskDelay(seconds*TICK);
	}
/* emulate unix alarm
 * that pauses and calls SIGALRM after the seconds are up...
 * so ... taskDelay() fudged for seconds should amount to the same thing.
 * casey
 */
void alarm (int seconds)
	{
	sleep(seconds);
	}

#endif /* SYS_VXWORKS */


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

#if defined (SYS_WINNT) || defined (SYS_VXWORKS)
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
