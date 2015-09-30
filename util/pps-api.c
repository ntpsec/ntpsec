/*

Try to run this program to see what the PPS-API finds. You give it the
device as argument and you may have to modify the pp.mode = BLA assignment.

Poul-Henning

*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <sys/timepps.h>

#define timespecsub(vvp, uvp)                                           \
        do {                                                            \
                (vvp)->tv_sec -= (uvp)->tv_sec;                         \
                (vvp)->tv_nsec -= (uvp)->tv_nsec;                       \
                if ((vvp)->tv_nsec < 0) {                               \
                        (vvp)->tv_sec--;                                \
                        (vvp)->tv_nsec += 1000000000;                   \
                }                                                       \
        } while (0)


void
Chew(struct timespec *tsa, struct timespec *tsc, unsigned sa, unsigned sc)
{
	struct timespec ts;

	printf("%ld.%09ld ", tsa->tv_sec, tsa->tv_nsec);
	printf("%ld.%09ld ", tsc->tv_sec, tsc->tv_nsec);
	printf("%u %u ", sa, sc);

	ts = *tsc;
	timespecsub(&ts,tsa);
	printf("%.9f ", ts.tv_sec + ts.tv_nsec / 1e9);
	printf("\n");
	fflush(stdout);
}

static int err(int out, const char *legend)
{
    fprintf(stderr, "pps-api: %s\n", legend);
    exit(out);
}

int
main(int argc, char **argv)
{
	int fd;
	pps_info_t pi;
	pps_params_t pp;
	pps_handle_t ph;
	int i, mode;
	u_int olda, oldc;
	struct timespec to;

	if (argc < 2)
		argv[1] = "/dev/cuaa1";
	setbuf(stdout, 0);
	fd = open(argv[1], O_RDONLY);
	if (fd < 0) 
		err(1, argv[1]);
	i = time_pps_create(fd, &ph);
	if (i < 0)
		err(1, "time_pps_create");

	i = time_pps_getcap(ph, &mode);
	if (i < 0)
		err(1, "time_pps_getcap");

	pp.mode = PPS_CAPTUREASSERT | PPS_ECHOASSERT;
	pp.mode = PPS_CAPTUREBOTH;
	/* pp.mode = PPS_CAPTUREASSERT; */

	i = time_pps_setparams(ph, &pp);
	if (i < 0)
		err(1, "time_pps_setparams");

	while (1) {
		to.tv_nsec = 0;
		to.tv_sec = 0;
		i = time_pps_fetch(ph, PPS_TSFMT_TSPEC, &pi, &to);
		if (i < 0)
			err(1, "time_pps_fetch");
		if (olda == pi.assert_sequence &&
		    oldc == pi.clear_sequence) {
			usleep(10000);
			continue;
		}

		Chew(&pi.assert_timestamp, &pi.clear_timestamp,
			pi.assert_sequence, pi.clear_sequence);
		olda = pi.assert_sequence;
		oldc = pi.clear_sequence;
	}

	return(0);
}
