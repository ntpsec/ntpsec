/*
 * Hack to bump the system time for testing ntpd response.
 * Must be run as root.
 * Arg is microseconds, default is 100000.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
    struct timeval was, set, now;
    int rc1, rc2, rc3;
    int er1, er2, er3;
    int bump = 100000;

    if (argc > 1) {
	bump = atoi(argv[1]);
    }
    printf("Bumping clock by %d microseconds.\n", bump);

    rc1 = gettimeofday(&was, NULL);
    er1 = errno;

    set = was;
    set.tv_usec += bump;
    while (set.tv_usec >= 1000000 ) {
	set.tv_usec -= 1000000;
	set.tv_sec +=1;
    }
    while (set.tv_usec <= 0 ) {
	set.tv_usec += 1000000;
	set.tv_sec -=1;
    }
    rc2 = settimeofday(&set, NULL);
    er2 = errno;

    rc3 = gettimeofday(&now, NULL);
    er3 = errno;

    /* Defer printing so it doesn't distort timing. */
    if (rc1)
	printf("Couldn't get time: %s\n", strerror(er1));
    else
	printf("Was: %ld.%06ld\n", (long)was.tv_sec, (long)was.tv_usec);

    if (rc2) {
	printf("Couldn't set time: %s\n", strerror(er2));
	printf("Try: %ld.%06ld\n", (long)set.tv_sec, (long)set.tv_usec);
    } else
	printf("Set: %ld.%06ld\n", (long)set.tv_sec, (long)set.tv_usec);
 
   if (rc3)
	printf("Couldn't set time: %s\n", strerror(er3));
    else
	printf("Now: %ld.%06ld\n", (long)now.tv_sec, (long)now.tv_usec);

    return (rc1!=0 || rc2!=0 || rc3!=0);

}
