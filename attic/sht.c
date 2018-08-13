/*
 * sht.c - Testprogram for shared memory refclock
 * read/write shared memory segment; see usage
 */
#include "config.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "ntp.h"
#include "ntp_stdlib.h"

char	*progname;

struct shmTime {
	int    mode; /* 0 - if valid set
		      *       use values,
		      *       clear valid
		      * 1 - if valid set
		      *       if count before and after read of values is equal,
		      *         use values
		      *       clear valid
		      */
	volatile int	count;
	time_t		clockTimeStampSec;
	int		clockTimeStampUSec;
	time_t		receiveTimeStampSec;
	int		receiveTimeStampUSec;
	int		leap;
	int		precision;
	int		nsamples;
	volatile int	valid;
	unsigned	clockTimeStampNSec;	/* Unsigned ns timestamps */
	unsigned	receiveTimeStampNSec;	/* Unsigned ns timestamps */
};

static struct shmTime *
getShmTime (
	int unit
	)
{
	int shmid=shmget (0x4e545030+unit, sizeof (struct shmTime), IPC_CREAT|0777);
	if (shmid==-1) {
		perror ("shmget");
		exit (1);
	}
	else {
		struct shmTime *p=(struct shmTime *)shmat (shmid, 0, 0);
		if ((int)(long)p==-1) {
			perror ("shmat");
			p=0;
		}
		assert (p!=0);
		return p;
	}
}


int
main (
	int argc,
	char *argv[]
	)
{
	volatile struct shmTime *p;
	int unit;
	char *argp;

	progname = argv[0];

	if (argc<=1) {
	  usage:
		printf ("usage: %s [uu:]{r[c][l]|w|snnn}\n",argv[0]);
		printf ("       uu use clock unit uu (default: 2)\n");
		printf ("       r read shared memory\n");
		printf ("       c clear valid-flag\n");
		printf ("       l loop (so, rcl will read and clear in a loop\n");
		printf ("       w write shared memory with current time\n");
		printf ("       snnnn set nsamples to nnn\n");
		printf ("       lnnnn set leap to nnn\n");
		printf ("       pnnnn set precision to -nnn\n");
		exit (0);
	}

	unit = (int)strtoul(argv[1], &argp, 10);
	if (argp == argv[1])
		unit = 2;
	else if (*argp == ':')
		argp++;
	else
		goto usage;

	p=getShmTime(unit);
	switch (*argp) {
	case 's':
		p->nsamples=atoi(argp+1);
		break;

	case 'l':
		p->leap=atoi(argp+1);
		break;

	case 'p':
		p->precision=-atoi(argp+1);
		break;

	case 'r': {
		int clear=0;
		int loop=0;
		printf ("reader\n");
		while (*++argp) {
			switch (*argp) {
			case 'l': loop=1; break;
			case 'c': clear=1; break;
			default : goto usage;
			}
		}
again:
		printf ("mode=%d, count=%d, clock=%ld.%09u, rec=%ld.%09u,\n",
			p->mode,p->count,
			(long)p->clockTimeStampSec,p->clockTimeStampNSec,
			(long)p->receiveTimeStampSec,p->receiveTimeStampNSec);
		printf ("  leap=%d, precision=%d, nsamples=%d, valid=%d\n",
			p->leap, p->precision, p->nsamples, p->valid);
		if (!p->valid)
			printf ("***\n");
		if (clear) {
			p->valid=0;
			printf ("cleared\n");
		}
		if (loop) {
			sleep (1);
			goto again;
		}
		break;
	}

	case 'w': {
		/* To show some life action, we read the system
		 * clock and use a bit of fuzz from 'ntp_random()' to get a
		 * bit of wobbling into the values (so we can observe a
		 * certain jitter!)
		 */
		time_t clk_sec, rcv_sec;
		unsigned int clk_frc, rcv_frc;

		/* Here we have a high-resolution system clock, and
		 * we're not afraid to use it!
		 */
		struct timespec tmptime;
		if (0 == clock_gettime(CLOCK_REALTIME, &tmptime)) {
			rcv_sec = tmptime.tv_sec;
			rcv_frc = (unsigned int)tmptime.tv_nsec;
		}
		else
		{
			time(&rcv_sec);
			rcv_frc = (unsigned int)ntp_random() % 1000000000u;
		}
		/* add a wobble of ~3.5msec to the clock time */
		clk_sec = rcv_sec;
		clk_frc = rcv_frc + (unsigned int)(ntp_random()%7094713 - 3547356);
		/* normalise result -- the SHM driver is picky! */
		while ((int)clk_frc < 0) {
			clk_frc += 1000000000;
			clk_sec -= 1;
		}
		while ((int)clk_frc >= 1000000000) {
			clk_frc -= 1000000000;
			clk_sec += 1;
		}

		/* Most 'real' time sources would create a clock
		 * (reference) time stamp where the fraction is zero,
		 * but that's not an actual requirement. So we show how
		 * to deal with the time stamps in general; changing the
		 * behaviour for cases where the fraction of the
		 * clock time is zero should be trivial.
		 */
		printf ("writer\n");
		p->mode=0;
		if (!p->valid) {
			p->clockTimeStampSec    = clk_sec;
			p->clockTimeStampUSec   = (int)(clk_frc / 1000); /* truncate! */
			p->clockTimeStampNSec   = clk_frc;
			p->receiveTimeStampSec  = rcv_sec;
			p->receiveTimeStampUSec = (int)(rcv_frc / 1000); /* truncate! */
			p->receiveTimeStampNSec = rcv_frc;
			printf ("%ld.%09u %ld.%09u\n",
				(long)p->clockTimeStampSec  , p->clockTimeStampNSec  ,
				(long)p->receiveTimeStampSec, p->receiveTimeStampNSec);
			p->valid=1;
		}
		else {
			printf ("p->valid still set\n"); /* not an error! */
		}
		break;
	}
	default:
		break;
	}
	return 0;
}
