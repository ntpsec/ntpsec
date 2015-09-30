/* Checks for the RS/6000 AIX adjtime() bug, in which if a negative
 * offset is given, the system gets messed up and never completes the
 * adjustment.  If the problem is fixed, this program will print the
 * time, sit there for 10 seconds, and exit.  If the problem isn't fixed,
 * the program will print an occasional "result=nnnnnn" (the residual
 * slew from adjtime()).
 *
 * This version is in modern ANSI C rather than the archaic dialect of
 * the RS6000's day.
 *
 * Run this as root!
 */
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

struct timeval adjustment, result;

static void
timeout(int sig)
{
	if (adjtime(&adjustment, &result))
	    printf("adjtime call failed\n");
	if (result.tv_sec != 0 || result.tv_usec != 0) {
		printf("result = %d.%6d  ",
		       (int) result.tv_sec,
		       (int) result.tv_usec);
	}
}

int
main (
	int argc,
	char *argv[]
	)
{
	struct itimerval value, oldvalue;
	int i;
	time_t curtime;
	char ascbuf[BUFSIZ];
	struct sigaction sa;

	curtime = time(0);
	printf("Starting: %s", ctime_r(&curtime, ascbuf));
	value.it_interval.tv_sec = value.it_value.tv_sec = 1;
	value.it_interval.tv_usec = value.it_value.tv_usec = 0;
	adjustment.tv_sec = 0;
	adjustment.tv_usec = -2000;
	sa.sa_handler = timeout;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	signal(SIGALRM, timeout);
	setitimer(ITIMER_REAL, &value, &oldvalue);
	for (i=0; i<10; i++) {
		pause();
	}

	exit(0);
}

