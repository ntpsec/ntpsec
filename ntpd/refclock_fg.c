/*
 * refclock_fg - clock driver for the Forum Graphic GPS datating station
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if defined(REFCLOCK) && defined(CLOCK_FG)

#include <time.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_refclock.h"
#include "ntp_calendar.h"
#include "ntp_stdlib.h"

/*
 * This driver supports the Forum Graphic GPS dating station.
 * More information about FG GPS is available on http://www.forumgraphic.com
 * Contact das@amt.ru for any question about this driver.
 */

/*
 * Interface definitions
 */
#define	DEVICE		"/dev/fgclock%d"
#define	PRECISION	(-10)	/* precision assumed (about 1 ms) */
#define REFID		"GPS"
#define DESCRIPTION	"Forum Graphic GPS dating station"
#define LENFG		26	/* timecode length */
#define SPEED232        B9600   /* uart speed (9600 baud) */

/*
 * Function prototypes
 */
static	int 	fg_start 		P((int, struct peer *));
static	void	fg_shutdown		P((int, struct peer *));
static	void	fg_poll		P((int, struct peer *));
static  void    fg_receive     P((struct recvbuf *));


/*
 * Transfer vector
 */
struct  refclock refclock_fg = {
	fg_start,              /* start up driver */
	fg_shutdown,           /* shut down driver */
	fg_poll,               /* transmit poll message */
	noentry,                /* not used */
	noentry,                /* initialize driver (not used) */
	noentry,                /* not used */
	NOFLAGS                 /* not used */
};


/*
 * fg_start - open the device and initialize data for processing
 */
static int
fg_start(
     	int unit,
	struct peer *peer
	)
{
	struct refclockproc *pp;
	int fd;
	char device[20], buf[LENFG];


	/*
	 * Open device file for reading.
	 */
	(void)sprintf(device, DEVICE, unit);

#ifdef DEBUG
	if (debug)
		printf ("starting FG with device %s\n",device);
#endif
	 if (!(fd = refclock_open(device, SPEED232, LDISC_CLK)))
                return (0);
	
        /*
         * Allocate and initialize unit structure
         */
	pp = peer->procptr;
	pp->io.clock_recv = fg_receive;
	pp->io.srcclock = (caddr_t)peer;
	pp->io.datalen = 0;
	pp->io.fd = fd;
 	if (!io_addclock(&pp->io)) {
                (void) close(fd);
                return (0);
        }

	
	/*
	 * Initialize miscellaneous variables
	 */
	peer->precision = PRECISION;
	pp->clockdesc = DESCRIPTION;
	memcpy((char *)&pp->refid, REFID, 3);
	
	/* 
	 * Setup dating station to use GPS receiver.
	 * GPS receiver should work before this operation.
         */
	memset(buf,0,26);	
	buf[0] = '';
	buf[1] = 'H';
	buf[2] = '';
	buf[3] = '\r';

	if (write(pp->io.fd, buf, LENFG) != LENFG)
                refclock_report(peer, CEVNT_FAULT);

	return (1);
}


/*
 * fg_shutdown - shut down the clock
 */
static void
fg_shutdown(
	int unit,
	struct peer *peer
	)
{
	struct refclockproc *pp;
	
	pp = peer->procptr;
        io_closeclock(&pp->io);
}


/*
 * fg_poll - called by the transmit procedure
 */
static void
fg_poll(
	int unit,
	struct peer *peer
	)
{
	struct refclockproc *pp;
	char buf[LENFG];

	
	pp = peer->procptr;

	 /*
         * Time to poll the clock. The FG clock responds to a
         * "<DLE>D<DLE><CR>" by returning a timecode in the format specified
         * above. If nothing is heard from the clock for two polls,
         * declare a timeout and keep going.
         */

	memset(buf,0,26);	
	buf[0] = '';
	buf[1] = 'D';
	buf[2] = '';
	buf[3] = '\r';

	if (write(pp->io.fd, buf, LENFG) != LENFG)
                refclock_report(peer, CEVNT_FAULT);
        else
                pp->polls++;
        if (peer->burst > 0)
                return;

        if (pp->coderecv == pp->codeproc) {
                refclock_report(peer, CEVNT_TIMEOUT);
                return;
        }
        peer->burst = NSTAGE;

        record_clock_stats(&peer->srcadr, pp->a_lastcode);
        refclock_receive(peer);

}

/*
 * fg_receive - receive data from the serial interface
 */
static void
fg_receive(
        struct recvbuf *rbufp
        )
{
        struct refclockproc *pp;
        struct peer *peer;
	char buf[256],*bpt;

        /*
         * Initialize pointers and read the timecode and timestamp
	 * We can't use gtlin function because we need bynary data in buf */

        peer = (struct peer *)rbufp->recv_srcclock;
        pp = peer->procptr;
	
	if (rbufp->recv_length < (LENFG-2))
	{
		refclock_report(peer, CEVNT_BADREPLY);
            	return; /* The reply is invalid discard it. */
	}

	/* Below I trying to find a correct reply in buffer.
	 * Sometime GPS reply located in the beginnig of buffer,
	 * sometime you can find it with some offset.
	 */

	bpt = (char *)rbufp->recv_space.X_recv_buffer;
	while(*bpt != '')
		bpt++;

      	memcpy(buf, bpt, LENFG);

#define BP2(x) ( buf[x] & 15 )
#define BP1(x) (( buf[x] & 240 ) >> 4)
	
        pp->year = BP1(2)*10 + BP2(2);
	
	if(pp->year == 94)
	{
		refclock_report(peer, CEVNT_BADREPLY);
            	return; /* GPS is just powered up. The date is invalid
                       discard it. */
		/* Sorry - this driver will broken in 2094 ;) */
	        /* You should rerun ntpd one more time to initilize
	           GPS device. */
	}	
	
	if (pp->year < 99)
                pp->year += 100;

        pp->year +=  1900;
        pp->day = 100 * BP2(3) + 10 * BP1(4) + BP2(4);
        pp->hour = BP1(5)*10 + BP2(5);
        pp->minute = BP1(6)*10 + BP2(6);
        pp->second = BP1(7)*10 + BP2(7);
        pp->usec = BP1(8)*100 + BP2(8)*10 + BP1(9);
        pp->lencode = sprintf(pp->a_lastcode, "%d %d %d %d %d", pp->year, pp->day, pp->hour, pp->minute, pp->second);
        get_systime(&pp->lastrec);

#ifdef DEBUG
        if (debug)
                printf ("fg: time is %04d/%03d %02d:%02d:%02d UTC\n",
                         pp->year, pp->day, pp->hour, pp->minute, pp->second);
#endif

        if (peer->stratum <= 1)
                peer->refid = pp->refid;
        pp->disp =  (10e-6);

        /*
         * Process the new sample in the median filter and determine the
         * timecode timestamp.
         */

        if (!refclock_process(pp))
                refclock_report(peer, CEVNT_BADTIME);
        
	return;
}


#else
int refclock_fg_bs;
#endif /* REFCLOCK */
