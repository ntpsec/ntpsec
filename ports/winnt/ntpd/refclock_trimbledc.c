#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(REFCLOCK) && defined(CLOCK_TRIMBLEDC)

#include "refclock_trimbledc.h"
extern int debug;

static	int	trimbledc_start		(int, struct peer *);
static	void	trimbledc_shutdown	(int, struct peer *);
static	void	trimbledc_receive	(struct recvbuf *);
static	void	trimbledc_poll		(int, struct peer *);
static	void	trimbledc_io		(char, int, unsigned char *, l_fp*, struct trimbledc_unit *);


/*
 * Transfer vector
 */
struct refclock refclock_trimbledc = {
	trimbledc_start,	/* start up driver */
	trimbledc_shutdown,	/* shut down driver */
	trimbledc_poll,		/* transmit poll message */
	noentry,		/* not used  */
	noentry,		/* initialize driver (not used) */
	noentry,		/* not used */
	NOFLAGS			/* not used */
};


/*
 * trimbledc_start - open the devices and initialize data for processing
 */
static int
trimbledc_start (
	int unit,
	struct peer *peer
	)
{
	register struct trimbledc_unit *up;
	struct refclockproc *pp;
	int fd;
	char gpsdev[20];

	struct termios tio;
#ifdef SYS_WINNT
	(void) sprintf(gpsdev, DEVICE, unit);
#else
	(void) sprintf(gpsdev, DEVICE, unit + 1);
#endif
	/*
	 * Open serial port. 
	 */
	fd = refclock_open(gpsdev, SPEED232, LDISC_RAW);
	if (fd == -1) {
		msyslog(LOG_ERR,"Trimble (%d) start: open %s failed: %m",
			unit, gpsdev);
		return 0;
	}

	msyslog(LOG_NOTICE, "Trimble (%d) fd: %d dev: %s", unit, fd, gpsdev);

        if (tcgetattr(fd, &tio) < 0) {
                msyslog(LOG_ERR, 
			"Trimble (%d) tcgetattr(fd, &tio): %m",unit);
                return (0);
        }

        tio.c_cflag |= (PARENB|PARODD);
        tio.c_iflag &= ~ICRNL;

	if (tcsetattr(fd, TCSANOW, &tio) == -1) {
                msyslog(LOG_ERR, "Trimble (%d) tcsetattr(fd, &tio): %m",unit);
                return 0;
        }

	/*
	 * Allocate and initialize unit structure
	 */
	if (!(up = (struct trimbledc_unit *)
	      emalloc(sizeof(struct trimbledc_unit)))) {
		(void) close(fd);
		return (0);
	}
	memset((char *)up, 0, sizeof(struct trimbledc_unit));

	pp = peer->procptr;
	pp->io.clock_recv = trimbledc_receive;
	pp->io.srcclock = (caddr_t)peer;
	pp->io.datalen = 0;
	pp->io.fd = fd;

	if (!io_addclock(&pp->io)) {
		(void) close(fd);
		free(up);
		return (0);
	}

	/*
	 * Initialize miscellaneous variables
	 */
	pp->unitptr = (caddr_t)up;
	pp->clockdesc = DESCRIPTION;

	peer->precision = PRECISION;
	peer->sstclktype = CTL_SST_TS_UHF;
	peer->minpoll = TRIMBLEDC_MINPOLL;
	peer->maxpoll = TRIMBLEDC_MAXPOLL;
	memcpy((char *)&pp->refid, REFID, 4);

	up->leap_status = 0;
	up->unit = unit;
	up->io_ptr[0] = up->io_ptr[1] = 0;

	return 1;
}


/*
 * trimbledc_shutdown - shut down the clock
 */
static void
trimbledc_shutdown (
	int unit,
	struct peer *peer
	)
{
	register struct trimbledc_unit *up;
	struct refclockproc *pp;
	pp = peer->procptr;
	up = (struct trimbledc_unit *)pp->unitptr;
	io_closeclock(&pp->io);
	free(up);
}



/* 
 * TSIP_decode - decode the TSIP data packets 
 */
static int
trimbledc_decode (
	struct peer *peer
	)
{
#ifdef DEBUG
	double lat, lon, alt;
#endif
	int st, ts;
	long   secint;
	double secs;
	double secfrac;
	unsigned short event = 0;

	register struct trimbledc_unit *up;
	struct refclockproc *pp;

	pp = peer->procptr;
	up = (struct trimbledc_unit *)pp->unitptr;

	/*
	 * Check the time packet, decode its contents. 
	 * If the timecode has invalid length or is not in
	 * proper format, declare bad format and exit.
	 */

	if (up->rpt_buf[0] ==0x41) 
	/* standard time packet - GPS time and GPS week number */
		return 0;	


	refclock_report(peer, CEVNT_BADREPLY);
	up->polled = -1;
#ifdef DEBUG
	printf("TRIMBLEDC_decode: unit %d: bad packet %02x-%02x event %d len %d\n", 
		   up->unit, up->rpt_buf[0] & 0xff, mb(0) & 0xff, 
			event, up->rpt_cnt);
#endif
	return 0;
}

/*
 * trimbledc__receive - receive data from the serial interface
 */

static void
trimbledc_receive (
	struct recvbuf *rbufp
	)
{
	register struct trimbledc_unit *up;
	struct refclockproc *pp;
	struct peer *peer;

	/*
	 * Initialize pointers and read the timecode and timestamp.
	 */
	peer = (struct peer *)rbufp->recv_srcclock;
	pp = peer->procptr;
	up = (struct trimbledc_unit *)pp->unitptr;

	
	for (;FALSE;) {
		trimbledc_io(pp->sloppyclockflag & CLK_FLAG2, rbufp->recv_length,
			    &rbufp->recv_buffer, &pp->lastrec, up);       




		(void) sprintf(pp->a_lastcode,"%4d %03d %02d:%02d:%02d.%06ld",
				pp->year,pp->day,pp->hour,pp->minute, pp->second,pp->usec); 
				pp->lencode = 24;

		if (!refclock_process(pp)) {
			refclock_report(peer, CEVNT_BADTIME);

#ifdef DEBUG
			printf("trimbledc_receive: unit %d: refclock_process failed!\n",
				up->unit);
#endif
			continue;
		}

		record_clock_stats(&peer->srcadr, pp->a_lastcode); 

#ifdef DEBUG
		if (debug)
		    printf("trimbledc_receive: unit %d: %s\n",
			   up->unit, prettydate(&pp->lastrec));
#endif

		refclock_receive(peer);
	}
}


/*
 * trimbledc_poll - called by the transmit procedure
 *
 */
static void
trimbledc_poll (
	int unit,
	struct peer *peer
	)
{
	struct trimbledc_unit *up;
	struct refclockproc *pp;
	
	pp = peer->procptr;
	up = (struct trimbledc_unit *)pp->unitptr;

	pp->polls++;
	if (up->polled > 0) /* last reply never arrived or error */ 
	    refclock_report(peer, CEVNT_TIMEOUT);

	up->polled = 2; /* synchronous packet + 1 event */
	
#ifdef DEBUG
	if (debug)
	    printf("trimbledc_poll: unit %d: polling %s\n", unit,
		   (pp->sloppyclockflag & CLK_FLAG2) ? 
			"synchronous packet" : "event");
#endif 

	if (pp->sloppyclockflag & CLK_FLAG2) 
	    return;  /* using synchronous packet input */

//	if (HW_poll(pp) < 0) 
	    refclock_report(peer, CEVNT_FAULT); 
}


static void
trimbledc_io (
	char noevents,
	int buflen,			/* bytes in buffer to process */
	unsigned char *bufp,		/* receive buffer */
	l_fp* t_in,			/* receive time stamp */
	struct trimbledc_unit *up	/* pointer to unit data structure   */
	)
{
	
}




#endif /* REFCLOCK */
