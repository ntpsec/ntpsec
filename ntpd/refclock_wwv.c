/*
 * refclock_wwv - audio WWV/H demodulator/decoder
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(REFCLOCK) && defined(CLOCK_WWV)

#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>
#include <math.h>
#ifdef HAVE_SYS_AUDIOIO_H
#include <sys/audioio.h>
#endif /* HAVE_SYS_AUDIOIO_H */
#ifdef HAVE_SUN_AUDIOIO_H
#include <sun/audioio.h>
#endif /* HAVE_SUN_AUDIOIO_H */
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif /* HAVE_SYS_IOCTL_H */

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_refclock.h"
#include "ntp_calendar.h"
#include "ntp_stdlib.h"

/*
 * Audio WWV/H demodulator/decoder
 *
 * This driver receives, demodulates and decodes WWV/H signals when
 * connected to the audio codec of a Sun workstation running SunOS or
 * Solaris, and with a little help, other workstations with similar
 * codecs or sound cards. The demodulation and decoding algorithms are
 * based on those developed for the TAPR DSP93 development board and the
 * TI 320C25 digital signal processor described in: Mills, D.L. A
 * precision radio clock for WWV transmissions. Electrical Engineering
 * Report 97-8-1, University of Delaware, August 1997, 25 pp. Available
 * from www.eecis.udel.edu/~mills/reports.htm.
 *
 * Since many variables are declared static in the various routines,
 * this driver is not reentrant and only one is supported on a system.
 */
/*
 * Interface definitions
 */
#define	PRECISION	(-10)	/* precision assumed (about 1 ms) */
#define	REFID		"WWV"	/* reference ID */
#define	DESCRIPTION	"WWV/H Audio Demodulator/Decoder" /* WRU */
#define AUDIO_BUFSIZ	160	/* codec buffer size (Solaris only) */
#define SAMPLES		8000	/* nominal sample rate (Hz) */
#define OFFSET		128	/* companded sample offset */
#define SIZE		256	/* decompanding table size */
#define	MAXSIG		6000.	/* maximum signal level */
#define MAXFREQ		(125e-6 * SAMPLES) /* freq tolerance (.025%) */
#define PI		3.1415926535 /* the real thing */
#define DATSIZ		(170 * MS) /* data matched filter size */
#define SYNSIZ		(800 * MS) /* second sync matched filter size */
#define MINUTE		(SAMPLES * 60) /* minute epoch */

/*
 * Macroni
 */
#define MOD(x, y)	((x) < 0 ? -(-(x) % (y)) : (x) % (y))

/*
 * General purpose status bits (status)
 *
 * Notes: SSYNC and MSYNC are set when the seconds epoch index and
 * minute epoch index are set, respectively. They are cleared and the
 * SYNERR alarm raised when either comb filter decays below STHR and
 * MTHR, respectively. DGATE is set if the data bit is invalid and BGATE
 * is set if any BCD digit bit is invalied. The data error counter is
 * incremented for each invalid bit. If too many errors are encountered
 * in one minute, the MODERR alarm is raised. If the probability of any
 * miscellaneous bit or any digit falls below ETHR, the SYMERR alarm is
 * raised. If the minutes unit digit is synchronized, DSYNC is lit;
 * otherwise, blow it out. INSYNC is set once the clock is completely
 * synchronized and never cleared.
 */
#define SSYNC		0x0001	/* second sync */
#define MSYNC		0x0002	/* minute sync */
#define DSYNC		0x0004	/* minute units synchronized */
#define INSYNC		0x0008	/* clock synchronized */
#define DGATE		0x0010	/* data bit error */
#define BGATE		0x0020	/* BCD data bit error */
#define WWV		0x0100	/* WWV station detected */
#define WWVH		0x0200	/* WWVH station detected */
#define ERRSTA		0x1000	/* error condition (buffer overrun) */
#define LEPSEC		0x2000	/* leap second in progress */
#define SLPSEC		0x4000	/* slip second in progress */

/*
 * Alarm status bits (alarm)
 *
 * These bits indicate various alarm conditions, which are decoded to
 * form the quality character included in the timecode. There are four
 * four-bit nibble fields in the word, each corresponding to a specific
 * alarm condition. At the end of each second, the word is shifted left
 * one position and the least significant bit of each nibble cleared.
 * This bit can be set during the next minute if the associated alarm
 * condition is raised. This provides a way to remember alarm conditions
 * up to four minutes.
 */
#define DECERR		0	/* BCD digit compare error */
#define SYMERR		4	/* low bit or digit probability */
#define MODERR		8	/* too many data bit errors */
#define SYNERR		12	/* low 1-s/1-m sync pulse amplitude */

/*
 * Tone frequency definitions.
 */
#define MS		8	/* samples per millisecond */
#define IN100		1	/* 100 Hz incr, 4.5-deg sin table */
#define IN1000		10	/* 1000 Hz incr, 4.5-deg sin table */
#define IN1200		12	/* 1200 Hz incr, 4.5-deg sin table */

/*
 * Acquisition and tracking time constants. Usually powers of 2.
 */
#define MINAVG		4	/* min time constant (s) */
#define MAXAVG		8	/* log2 max time constant (s) */
#define TCONST		16	/* minute time constant (s) */
#define SYNCTC		(1024 / (1 << MAXAVG)) /* FLL constant (s) */

/*
 * Thresholds. These establish the minimum signal level, minimum SNR and
 * maximum jitter thresholds which establish the error and false alarm
 * rates of the receiver. The values defined here may be on the
 * adventurous side in the interest of the highest sensitivity.
 */
#define ITHR		3000	/* station ident threshold */
#define CTHR		10 	/* epoch synch compare threshold */
#define STHR		1000	/* epoch sync threshold */
#define MTHR		300	/* seconds sync threshold */
#define DTHR		500	/* subcarrier amplitude threshold */
#define ETHR		500	/* data probability threshold */
#define BTHR		30	/* max bit error count in second */
#define NTHR		10	/* data SNR threshold (dB) */
#define TTHR		3	/* digit SNR threshold (dB) */

/*
 * Miscellaneous status bits (misc)
 *
 * These bits correspond to designated bits in the WWV/H timecode. The
 * bit probabilities are exponentially averaged over several minutes and
 * processed by a integrator and threshold.
 */
#define DUT1		0x01	/* 56 DUT .1 */
#define DUT2		0x02	/* 57 DUT .2 */
#define DUT4		0x04	/* 58 DUT .4 */
#define DUTS		0x08	/* 50 DUT sign */
#define DST1		0x10	/* 55 DST1 DST in progress */
#define DST2		0x20	/* 2 DST2 DST change warning */
#define SECWAR		0x40	/* 3 leap second warning */

/*
 * Experimentally determined fudge factors
 */
#define PDELAY	.0019		/* receiver phase delay (s) */

/*
 * Error flags (errflg)
 */
#define WWV_ERR_AMP	0x01	/* low carrier amplitude */
#define WWV_ERR_FREQ	0x02	/* frequency tolerance exceeded */
#define WWV_ERR_MOD	0x04	/* low modulation index */
#define WWV_ERR_SYNCH	0x08	/* frame synch error */
#define WWV_ERR_DECODE	0x10	/* frame decoding error */
#define WWV_ERR_CHECK	0x20	/* second numbering discrepancy */
#define WWV_ERR_ERROR	0x40	/* codec error (overrun) */

/*
 * Table of sin() values normalized by 1 / PI at 4.5-degree increments.
 * This is used for baseband conversions by the matched filters and
 * integrators.
 */
double sintab[] = {
 0.000000e+00,  2.497431e-02,  4.979464e-02,  7.430797e-02, /* 0-3 */
 9.836316e-02,  1.218119e-01,  1.445097e-01,  1.663165e-01, /* 4-7 */
 1.870979e-01,  2.067257e-01,  2.250791e-01,  2.420447e-01, /* 8-11 */
 2.575181e-01,  2.714038e-01,  2.836162e-01,  2.940800e-01, /* 12-15 */
 3.027307e-01,  3.095150e-01,  3.143910e-01,  3.173286e-01, /* 16-19 */
 3.183099e-01,  3.173286e-01,  3.143910e-01,  3.095150e-01, /* 20-23 */
 3.027307e-01,  2.940800e-01,  2.836162e-01,  2.714038e-01, /* 24-27 */
 2.575181e-01,  2.420447e-01,  2.250791e-01,  2.067257e-01, /* 28-31 */
 1.870979e-01,  1.663165e-01,  1.445097e-01,  1.218119e-01, /* 32-35 */
 9.836316e-02,  7.430797e-02,  4.979464e-02,  2.497431e-02, /* 36-39 */
-0.000000e+00, -2.497431e-02, -4.979464e-02, -7.430797e-02, /* 40-43 */
-9.836316e-02, -1.218119e-01, -1.445097e-01, -1.663165e-01, /* 44-47 */
-1.870979e-01, -2.067257e-01, -2.250791e-01, -2.420447e-01, /* 48-51 */
-2.575181e-01, -2.714038e-01, -2.836162e-01, -2.940800e-01, /* 52-55 */
-3.027307e-01, -3.095150e-01, -3.143910e-01, -3.173286e-01, /* 56-59 */
-3.183099e-01, -3.173286e-01, -3.143910e-01, -3.095150e-01, /* 60-63 */
-3.027307e-01, -2.940800e-01, -2.836162e-01, -2.714038e-01, /* 64-67 */
-2.575181e-01, -2.420447e-01, -2.250791e-01, -2.067257e-01, /* 68-71 */
-1.870979e-01, -1.663165e-01, -1.445097e-01, -1.218119e-01, /* 72-75 */
-9.836316e-02, -7.430797e-02, -4.979464e-02, -2.497431e-02, /* 76-79 */
 0.000000e+00};						    /* 80 */

/*
 * DST decode (DST2 DST1)
 */
char dstcod[] = {
	'S',			/* 00 standard time */
	'I',			/* 01 daylight warning */
	'O',			/* 10 standard warning */
	'D'			/* 11 daylight time */
};

/*
 * Command segment dispatch table indexed by second number. Each entry
 * contains case number and argument.
 */
struct progx {
	int sw;			/* case switch */
	int arg;		/* argument */
};

/*
 * Case switch values
 */
#define IDLE		0	/* no operation */
#define COEF		1	/* BCD bit conditioned on DSYNC */
#define COEF1		2	/* BCD bit */
#define COEF2		3	/* BCD bit ignored */
#define DECIM9		4	/* BCD digit 0-9 */
#define DECIM6		5	/* BCD digit 0-6 */
#define DECIM3		6	/* BCD digit 0-3 */
#define DECIM2		7	/* BCD digit 0-2 */
#define MSCBIT		8	/* miscellaneous bit */
#define MSC20		9	/* miscellaneous bit */		
#define MIN1		10	/* minute */		
#define MIN2		11	/* leap second */

/*
 * Offsets in decoding matrix
 */
#define MN		0	/* minute digits (2) */
#define HR		2	/* hour digits (2) */
#define DA		4	/* day digits (3) */
#define YR		7	/* year digits (2) */

struct progx progx[] = {
	{IDLE,	0},		/* 0 punched */
	{IDLE,	0},		/* 1 */
	{MSCBIT, DST2},		/* 2 dst2 */
	{MSCBIT, SECWAR},	/* 3 lw */
	{COEF,	0},		/* 4 1 year units */
	{COEF,	1},		/* 5 2 */
	{COEF,	2},		/* 6 4 */
	{COEF,	3},		/* 7 8 */
	{DECIM9, YR},		/* 8 */
	{IDLE,	0},		/* 9 p1 */
	{COEF1, 0},		/* 10 1 minute units */
	{COEF1,	1},		/* 11 2 */
	{COEF1,	2},		/* 12 4 */
	{COEF1,	3},		/* 13 8 */
	{DECIM9, MN},		/* 14 */
	{COEF, 0},		/* 15 10 minute tens */
	{COEF,	1},		/* 16 20 */
	{COEF,	2},		/* 17 40 */
	{COEF2,	3},		/* 18 80 (not used) */
	{DECIM6, MN + 1},	/* 19 p2 */
	{COEF,	0},		/* 20 1 hour units */
	{COEF,	1},		/* 21 2 */
	{COEF,	2},		/* 22 4 */
	{COEF,	3},		/* 23 8 */
	{DECIM9, HR},		/* 24 */
	{COEF,	0},		/* 25 10 hour tens */
	{COEF,	1},		/* 26 20 */
	{COEF2,	2},		/* 27 40 (not used) */
	{COEF2,	3},		/* 28 80 (not used) */
	{DECIM2, HR + 1},	/* 29 p3 */
	{COEF,	0},		/* 30 1 day units */
	{COEF,	1},		/* 31 2 */
	{COEF,	2},		/* 32 4 */
	{COEF,	3},		/* 33 8 */
	{DECIM9, DA},		/* 34 */
	{COEF,	0},		/* 35 10 day tens */
	{COEF,	1},		/* 36 20 */
	{COEF,	2},		/* 37 40 */
	{COEF,	3},		/* 38 80 */
	{DECIM9, DA + 1},	/* 39 p4 */
	{COEF,	0},		/* 40 100 day hundreds */
	{COEF,	1},		/* 41 200 */
	{COEF2,	2},		/* 42 400 (not used) */
	{COEF2,	3},		/* 43 800 (not used) */
	{DECIM3, DA + 2},	/* 44 */
	{IDLE,	0},		/* 45 */
	{IDLE,	0},		/* 46 */
	{IDLE,	0},		/* 47 */
	{IDLE,	0},		/* 48 */
	{IDLE,	0},		/* 49 p5 */
	{MSCBIT, DUTS},		/* 50 dut+- */
	{COEF,	0},		/* 51 10 year tens */
	{COEF,	1},		/* 52 20 */
	{COEF,	2},		/* 53 40 */
	{COEF,	3},		/* 54 80 */
	{MSC20, DST1},		/* 55 dst1 */
	{MSCBIT, DUT1},		/* 56 0.1 dut */
	{MSCBIT, DUT2},		/* 57 0.2 */
	{MSCBIT, DUT4},		/* 58 0.4 */
	{MIN1,	0},		/* 59 p6 */
	{MIN2,	0}		/* 60 leap second */
};

/*
 * BCD coefficients for maximum likelihood digit decode
 */
#define P15	1.		/* max positive number */
#define N15	-1.		/* max negative number */

/*
 * Digits 0-9
 */
#define P9	(P15 / 4)	/* mark (+1) */
#define N9	(N15 / 4)	/* space (-1) */

double bcd9[][4] = {
	{N9, N9, N9, N9}, 	/* 0 */
	{P9, N9, N9, N9}, 	/* 1 */
	{N9, P9, N9, N9}, 	/* 2 */
	{P9, P9, N9, N9}, 	/* 3 */
	{N9, N9, P9, N9}, 	/* 4 */
	{P9, N9, P9, N9}, 	/* 5 */
	{N9, P9, P9, N9}, 	/* 6 */
	{P9, P9, P9, N9}, 	/* 7 */
	{N9, N9, N9, P9}, 	/* 8 */
	{P9, N9, N9, P9}, 	/* 9 */
	{0, 0, 0, 0}		/* backstop */
};

/*
 * Digits 0-6 (minute tens)
 */
#define P6	(P15 / 3)	/* mark (+1) */
#define N6	(N15 / 3)	/* space (-1) */

double bcd6[][4] = {
	{N6, N6, N6, 0}, 	/* 0 */
	{P6, N6, N6, 0}, 	/* 1 */
	{N6, P6, N6, 0}, 	/* 2 */
	{P6, P6, N6, 0}, 	/* 3 */
	{N6, N6, P6, 0}, 	/* 4 */
	{P6, N6, P6, 0}, 	/* 5 */
	{N6, P6, P6, 0}, 	/* 6 */
	{0, 0, 0, 0}		/* backstop */
};

/*
 * Digits 0-3 (day hundreds)
 */
#define P3	(P15 / 2)	/* mark (+1) */
#define N3	(N15 / 2)	/* space (-1) */

double bcd3[][4] = {
	{N3, N3, 0, 0}, 	/* 0 */
	{P3, N3, 0, 0}, 	/* 1 */
	{N3, P3, 0, 0}, 	/* 2 */
	{P3, P3, 0, 0}, 	/* 3 */
	{0, 0, 0, 0}		/* backstop */
};

/*
 * Digits 0-2 (hour tens)
 */
#define P2	(P15 / 2)	/* mark (+1) */
#define N2	(N15 / 2)	/* space (-1) */

double bcd2[][4] = {
	{N2, N2, 0, 0}, 	/* 0 */
	{P2, N2, 0, 0}, 	/* 1 */
	{N2, P2, 0, 0}, 	/* 2 */
	{0, 0, 0, 0}		/* backstop */
};

/*
 * The decoding matrix consists of nine row vectors, one for each digit
 * of the timecode. The digits are stored from least to most significant
 * order. The maximum likelihood timecode is formed from the digits
 * corresponding to the maximum likelihood values reading in the
 * opposite order: yy ddd hh:mm.
 */
struct decvec {
	int radix;		/* radix (3, 4, 6, 10) */
	int digit;		/* current clock digit */
	int mldigit;		/* maximum likelihood digit */
	int phase;		/* maximum likelihood digit phase */
	int count;		/* match count */
	double digamp;		/* correlation amplitude */
	double digsnr;		/* correlation SNR */
	double like[10];	/* likelihood integrator 0-9 */
};

struct sync {
	double	ibuf[SYNSIZ];	/* I channel FIFO */
	double	qbuf[SYNSIZ];	/* Q channel FIFO */
	double	iamp;		/* I channel amplitude */
	double	qamp;		/* Q channel amplitude */
	double	max;		/* max amplitude on pulse */
	double	noise;		/* max amplitude off pulse */
	long	pos;		/* position at maximum amplitude */
	long	lastpos;	/* last position at maximum amplitude */
	int	state;		/* fsa state */
};

/*
 * WWV unit control structure
 */
struct wwvunit {
	l_fp	timestamp;	/* audio sample timestamp */
	l_fp	tick;		/* audio sample increment */
	double	comp[SIZE];	/* decompanding table */
	double	phase, freq;	/* logical clock phase and frequency */
	double	monitor;	/* audio monitor point */
	int	errflg;		/* error flags */
	int	bufcnt;		/* samples in buffer */
	int	bufptr;		/* buffer index pointer */
	int	port;		/* codec port */
	int	gain;		/* codec gain */
	int	clipcnt;	/* sample clipped count */
	int	seccnt;		/* second countdown */
	int	minset;		/* minutes since last clock set */

	/*
	 * Variables used to establish basic system timing
	 */
	int	avgint;		/* log2 master time constant (s) */
	int	epoch;		/* epoch ramp */
	int	yepoch;		/* second sync epoch */
	double	epomax;		/* second sync amplitude */
	double	secmax;		/* minute sync amplitude */
	double	irig;		/* data I channel amplitude */
	double	qrig;		/* data Q channel amplitude */
	int	datapt;		/* 10-ms ramp */
	double	datpha;		/* 10-ms ramp phase */
	
	int	rphase;		/* receiver sample counter */
	int	rsec;		/* receiver seconds counter */

	long	kptr;		/* minute sample counter */
	int	jptr;		/* minute sync pulse counter */

	struct sync wwv;	/* WWV sync channel */
	struct sync wwvh; 	/* WWVH sync channel */

	/*
	 * Variables used by the clock state machine
	 */
	struct decvec decvec[9]; /* decoding matrix */
	int	cdelay;		/* WWV propagation delay */
	int	hdelay;		/* WWVH propagation delay */
	int	tphase;		/* transmitter sample counter */
	int	tsec;		/* transmitter seconds counter */

	/*
	 * Variables used to estimate signal levels and bit/digit
	 * probabilities
	 */
	double	datamp;		/* max signal amplitude */
	double	noiamp;		/* average noise amplitude */
	double	datsnr;		/* data SNR (dB) */

	/*
	 * Variables used to establish status and alarm conditions
	 */
	int	status;		/* status bits */
	int	misc;		/* miscellaneous timecode bits */
	int	alarm;		/* alarm flashers */
	int	errcnt;		/* data bit error counter */
};

/*
 * Function prototypes
 */
static	int	wwv_start	P((int, struct peer *));
static	void	wwv_shutdown	P((int, struct peer *));
static	void	wwv_receive	P((struct recvbuf *));
static	void	wwv_poll	P((int, struct peer *));

/*
 * More function prototypes
 */
static	void	wwv_epoch	P((struct peer *, double));
static	void	wwv_rf		P((struct wwvunit *, double));
static	void	wwv_endpoc	P((struct wwvunit *, double, int));
static	double	wwv_rsec	P((struct wwvunit *, double));
static	void	wwv_tsec	P((struct wwvunit *));
static	double	wwv_data	P((struct wwvunit *, double));
static	void	wwv_corr4	P((struct wwvunit *, struct decvec *,
				    double [], double [][]));
static	void	wwv_gain	P((struct peer *));
static	int	wwv_audio	P((void));
static	void	wwv_show	P((void));
static	double	wwv_snr		P((double, double));
static	void	wwv_qrz		P((struct wwvunit *, struct sync *,
				    double, int));
static	int	carry		P((struct decvec *));
static	int	timecode	P((struct wwvunit *, char *));

/*
 * Transfer vector
 */
struct	refclock refclock_wwv = {
	wwv_start,		/* start up driver */
	wwv_shutdown,		/* shut down driver */
	wwv_poll,		/* transmit poll message */
	noentry,		/* not used (old wwv_control) */
	noentry,		/* initialize driver (not used) */
	noentry,		/* not used (old wwv_buginfo) */
	NOFLAGS			/* not used */
};

/*
 * Global variables
 */
#ifdef HAVE_SYS_AUDIOIO_H
struct	audio_device device;	/* audio device ident */
#endif /* HAVE_SYS_AUDIOIO_H */
static struct	audio_info info; /* audio device info */
static int	wwv_ctl_fd;	/* audio control file descriptor */


/*
 * wwv_start - open the devices and initialize data for processing
 */
static int
wwv_start(
	int	unit,		/* instance number (not used) */
	struct peer *peer	/* peer structure pointer */
	)
{
	struct refclockproc *pp;
	struct wwvunit *up;

	/*
	 * Local variables
	 */
	int	fd;		/* file descriptor */
	int	i;		/* index */
	double	step;		/* codec adjustment */

	/*
	 * Open audio device
	 */
	fd = open("/dev/audio", O_RDWR | O_NONBLOCK, 0777);
	if (fd == -1) {
		perror("audio");
		return (0);
	}

	/*
	 * Allocate and initialize unit structure
	 */
	if (!(up = (struct wwvunit *)
	      emalloc(sizeof(struct wwvunit)))) {
		(void) close(fd);
		return (0);
	}
	memset((char *)up, 0, sizeof(struct wwvunit));
	pp = peer->procptr;
	pp->unitptr = (caddr_t)up;
	pp->io.clock_recv = wwv_receive;
	pp->io.srcclock = (caddr_t)peer;
	pp->io.datalen = 0;
	pp->io.fd = fd;
	if (!io_addclock(&pp->io)) {
		(void)close(fd);
		free(up);
		return (0);
	}

	/*
	 * Initialize miscellaneous variables
	 */
	peer->precision = PRECISION;
	pp->clockdesc = DESCRIPTION;
	memcpy((char *)&pp->refid, REFID, 4);
	up->gain = (AUDIO_MAX_GAIN - AUDIO_MIN_GAIN) / 2;
	if (wwv_audio() < 0) {
		io_closeclock(&pp->io);
		return(0);
	}

	/*
	 * The companded samples are encoded sign-magnitude. The table
	 * contains all the 256 values in the interest of speed.
	 */
	up->comp[0] = up->comp[OFFSET] = 0.;
	up->comp[1] = 1; up->comp[OFFSET + 1] = -1.;
	up->comp[2] = 3; up->comp[OFFSET + 2] = -3.;
	step = 2.;
	for (i = 3; i < OFFSET; i++) {
		up->comp[i] = up->comp[i - 1] + step;
		up->comp[OFFSET + i] = -up->comp[i];
                if (i % 16 == 0)
		    step *= 2.;
	}
	DTOLFP(1. / SAMPLES, &up->tick);
	up->decvec[MN].radix = 10;	/* minutes */
	up->decvec[MN + 1].radix = 6;
	up->decvec[HR].radix = 10;	/* hours */
	up->decvec[HR + 1].radix = 3;
	up->decvec[DA].radix = 10;	/* days */
	up->decvec[DA + 1].radix = 10;
	up->decvec[DA + 2].radix = 4;
	up->decvec[YR].radix = 10;	/* years */
	up->decvec[YR + 1].radix = 10;
	return (1);
}


/*
 * wwv_shutdown - shut down the clock
 */
static void
wwv_shutdown(
	int	unit,		/* instance number (not used) */
	struct peer *peer	/* peer structure pointer */
	)
{
	struct refclockproc *pp;
	struct wwvunit *up;

	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;
	io_closeclock(&pp->io);
	free(up);
}


/*
 * wwv_receive - receive data from the audio device
 *
 * This routine reads input samples and adjusts the logical clock to
 * track the A/D sample clock by dropping or duplicating codec samples.
 * It also controls the A/D signal level with an AGC loop to mimimize
 * quantization noise and avoid overload.
 */
static void
wwv_receive(
	struct recvbuf *rbufp	/* receive buffer structure pointer */
	)
{
	struct peer *peer;
	struct refclockproc *pp;
	struct wwvunit *up;

	/*
	 * Local variables
	 */
	double	sample;		/* codec sample */
	u_char	*dpt;		/* buffer pointer */
	l_fp	ltemp;
	int	isneg;
	double	dtemp;
	int	i, j;

	peer = (struct peer *)rbufp->recv_srcclock;
	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;

	/*
	 * Main loop - read until there ain't no more. Note codec
	 * samples are bit-inverted.
	 */
	up->timestamp = rbufp->recv_time;
	up->bufcnt = rbufp->recv_length;
	DTOLFP((double)up->bufcnt / SAMPLES, &ltemp);
	L_SUB(&up->timestamp, &ltemp);
	dpt = rbufp->recv_buffer;
	for (up->bufptr = 0; up->bufptr < up->bufcnt; up->bufptr++) {
		sample = up->comp[~*dpt & 0xff];

		/*
		 * Clip noise spikes greater than MAXSIG. If no clips,
		 * increase the gain a tad; if the clips are too high, 
		 * decrease a tad.
		 */
		if (sample > MAXSIG) {
			sample = MAXSIG;
			up->clipcnt++;
		} else if (sample < -MAXSIG) {
			sample = -MAXSIG;
			up->clipcnt++;
		}

		/*
		 * Variable frequency oscillator. A phase change of one
		 * unit produces a change of 360 degrees; a frequency
		 * change of one unit produces a change of 1 Hz.
		 */
		up->phase += up->freq / SAMPLES;
		if (up->phase >= .5) {
			up->phase -= 1.;
		} else if (up->phase < - .5) {
			up->phase += 1.;
			wwv_epoch(peer, sample);
			wwv_epoch(peer, sample);
		} else {
			wwv_epoch(peer, sample);
		}
		L_ADD(&up->timestamp, &up->tick);

		/*
		 * Once each second adjust the codec port and gain;
		 * while at it, initialize the propagation delay for
		 * both WWV and WWVH. Don't forget to correct for the
		 * receiver phase delay, mostly due to the 600-Hz
		 * IIR bandpass filter used for the sync signals.
		 */
		up->cdelay = (int)(SAMPLES * (pp->fudgetime1 + PDELAY));
		up->hdelay = (int)(SAMPLES * (pp->fudgetime2 + PDELAY));
		up->seccnt = (up->seccnt + 1) % SAMPLES;
		if (up->seccnt == 0) {
			if (pp->sloppyclockflag & CLK_FLAG2)
			    up->port = AUDIO_LINE_IN;
			else
			    up->port = AUDIO_MICROPHONE;
			wwv_gain(peer);
			up->clipcnt = 0;
		}

		/*
		 * During development, it is handy to have an audio
		 * monitor that can be switched to various signals. This
		 * code converts the linear signal left in up->monitor
		 * to codec format. If we can get the grass out of this
		 * thing and improve modem performance, this expensive
		 * code will be permanently nixed.
		 */
		isneg = 0;
		dtemp = up->monitor;
		if (sample < 0) {
			isneg = 1;
			dtemp -= dtemp;
		}
		i = 0;
		j = OFFSET >> 1;
		while (j != 0) {
			if (dtemp > up->comp[i])
				i += j;
			else if (dtemp < up->comp[i])
				i -= j;
			else
				break;
			j >>= 1;
		}
		if (isneg)
			*dpt = ~(i + OFFSET);
		else
			*dpt = ~i;
		dpt++;
	}

	/*
	 * Squawk to the monitor speaker if enabled.
	 */
	if (pp->sloppyclockflag & CLK_FLAG3)
	    if (write(pp->io.fd, (u_char *)&rbufp->recv_space,
		      (u_int)up->bufcnt) < 0)
		perror("wwv:");
}


/*
 * wwv_poll - called by the transmit procedure
 *
 * This routine keeps track of status. If nothing is heard for two
 * successive poll intervals, a timeout event is declared and any
 * orphaned timecode updates are sent to foster care. 
 */
static void
wwv_poll(
	int	unit,		/* instance number (not used) */
	struct peer *peer	/* peer structure pointer */
	)
{
	struct refclockproc *pp;
	struct wwvunit *up;
	char synchar, qualchar, leapchar;

	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;
	/*
	 * Keep book for tattletales
	 */
	if (pp->coderecv == pp->codeproc)
		refclock_report(peer, CEVNT_TIMEOUT);
	else
		pp->polls++;

	/*
	 * Decode the timecode string previously encoded with care and
	 * set the dingbat bits.
	 */
	if (sscanf(pp->a_lastcode, "%c%c %2d %3d %2d:%2d:%2d.%3d %c",
	    &synchar, &qualchar, &pp->year, &pp->day, &pp->hour,
	    &pp->minute, &pp->second, &pp->msec, &leapchar) != 9)
		refclock_report(peer, CEVNT_BADREPLY);
	if (synchar != ' ')
		pp->leap = LEAP_NOTINSYNC;
	else if (leapchar == 'L')
		pp->leap = LEAP_ADDSECOND;
	else
		pp->leap = LEAP_NOWARNING;
	if (!refclock_process(pp))
		refclock_report(peer, CEVNT_BADTIME);
  	record_clock_stats(&peer->srcadr, pp->a_lastcode);
	refclock_receive(peer);
}


/*
 * wwv_epoch() main loop
 *
 * This routine establishes receiver and transmitter epoch
 * synchronization and determines the data subcarrier pulse length.
 * Receiver synchronization is determined by the epoch sync pulse
 * detected in the RF input routine. This establishes when to sample the
 * data subcarrier in-phase signal for the maximum level and noise level
 * and when to determine the pulse length. The transmitter epoch leads
 * the receiver epoch by the propagation delay. It establishes the clock
 * time and implements the sometimes idiosyncratic conventional clock
 * time and civil calendar. 
 *
 * Most communications radios use a lowpass filter in the audio stages,
 * which can do nasty things to the subcarrier phase relative to the
 * epoch sync pulse. Therefore, the data subcarrier reference phase is
 * disciplined using the quadrature-phase signal sampled at the same
 * time as the in-phase signal.
 */
void
wwv_epoch(
	struct peer *peer,	/* peer structure pointer */
	double sig		/* decompanded rf signal sample */
	)
{
	static double dpulse;	/* data pulse length */
	struct refclockproc *pp;
	struct wwvunit *up;
	double dtemp, etemp;
	int temp;

	/*
	 * Extract the data and sync signals.
	 */
	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;
	wwv_rf(up, sig);

	/*
	 * Estimate the noise level by integrating the I-channel energy
	 * before epoch 30 ms.
	 */
	if (up->rphase < 30 * MS) {
		up->noiamp += (up->irig - up->noiamp) / (MINAVG <<
		    up->avgint);

	/*
	 * Strobe the maximum I-channel data signal at epoch 200 ms.
	 * Compute the SNR and adjust the 100-Hz reference oscillator
	 * phase using the Q-channel data signal at that epoch.
	 */
	} else if (up->rphase == 200 * MS) {
		up->datamp = up->irig;
		if (up->datamp < 0)
			up->datamp = 0;
		up->datsnr = wwv_snr(up->datamp, up->noiamp);
		up->datpha = up->qrig / (MINAVG << up->avgint);
		if (up->datpha > MS)
			up->datapt++;
		else if (up->datpha < -MS)
			up->datapt--;
		if (up->datapt >= 80)
			up->datapt -= 80;
		if (up->datapt < 0)
			up->datapt += 80;

	/*
	 * The slice level is set half way between the noise level and
	 * the maximum signal. Strobe the negative zero crossing after
	 * epoch 200 ms and record the epoch at that time. This defines
	 * the size of the data pulse, which will later be converted
	 * into scaled bit probabilities. However, the data
	 */
	} else if (up->rphase > 200 * MS) {
		dtemp = (up->datamp + up->noiamp) / 2;
		if (up->irig < dtemp && dpulse == 0)
			dpulse = up->rphase;
	}

	/*
	 * At the end of the transmitter second, crank the clock state
	 * machine.
	 */
	up->tphase++;
	if (up->status & WWVH)
		temp = up->hdelay;	/* Kaui */
	else
		temp = up->cdelay;	/* Ft. Collins */
	if ((up->epoch + temp) % SAMPLES == up->yepoch) {
		wwv_tsec(up);
		up->tphase = 0;
		pp->lastrec = up->timestamp;
		pp->lencode = timecode(up, pp->a_lastcode);
	}

	/*
	 * At the end of the receiver second, process the data bit and
	 * update the decoding matrix probabilities.
	 */
	up->rphase++;
	if (up->epoch == up->yepoch) {
		temp = up->rsec;
		dtemp = wwv_data(up, dpulse);
		etemp = wwv_rsec(up, dtemp);
		if (up->status & SLPSEC)
			up->status &= ~SLPSEC;
#ifdef DEBUG
		if (debug && up->status & MSYNC && !(up->status &
		    DSYNC)) {
			printf("wwv3 %2d %04x %3d %5.0f", up->rsec,
			    up->status, up->gain, up->epomax);
			printf(
		" %04x %5.0f %5.0f %5.0f %5.1f %5.0f %5.0f\n",
			    up->alarm, up->secmax, up->datamp,
			    up->datpha, up->datsnr, dtemp, etemp);
		}
#endif /* DEBUG */
		up->rphase = dpulse = 0;
	}
}


/*
 * wwv_rf() - process signals and demodulate to baseband
 *
 * This routine grooms and filters decompanded raw audio samples. The
 * output signals include the 100-Hz baseband data signal in quadrature
 * form, plus the epoch index of the epoch sync signal and the second
 * index of the minutes sync signal.
 *
 * There are three 1-s ramps used by this program, all spanning the
 * range 0-7999 logical samples for exactly one second, as determined by
 * the logical clock. The first drives the epoch comb filter and runs
 * continuously. The second determines the receiver epoch and the third
 * the transmitter epoch. The receiver epoch begins upon arrival of the
 * 5-ms epoch sync pulse which begins the second; while the transmitter
 * epoch begins before it by the specified propagation delay.
 *
 * There are three 1-m ramps spanning the range 0-59 seconds. The first
 * drives the minute comb filter and runs continuously. The second
 * determines the receiver second and the third the transmitter second.
 * The receiver second begins upon arrival of the 800-ms sync pulse sent
 * during the first second of the minute. 
 *
 * The output signals include the epoch index and phase and second
 * maximum and index. The epoch index provides the master reference for
 * all signal and timing functions, while the second index identifies
 * the first second of the minute. The epoch and second maxima are used
 * to calculate SNR for gating functions.
 *
 * Demodulation operations are based on three synthesized quadrature
 * sinusoids: 100 Hz for the data subcarrier, 1000 Hz for the WWV sync
 * signals and 1200 Hz for the WWVH sync signal. These drive synchronous
 * matched filters for the data subcarrier (170 ms at 100 Hz), WWV
 * minute sync signal (800 ms at 1000 Hz) and WWVH minute sync signal
 * (800 ms at 1200 Hz). Two additional matched filters are switched in
 * as required for the WWV seconds sync signal (5 ms at 1000 Hz) and
 * WWVH seconds sync signal (5 ms at 1200 Hz).
 */
static void
wwv_rf(
	struct wwvunit *up,	/* driver structure pointer */
	double isig		/* input signal */
	)
{
	static double lpf[5];	/* 150-Hz lpf delay line */
	double data;		/* lpf output */
	static double bpf[9];	/* 1000/1200-Hz bpf delay line */
	double syncx;		/* bpf output */
	static double mf[41];	/* 1000/1200-Hz mf delay line */
	double mfsync = 0;	/* mf output */

	static int csyncpt;	/* 1000-Hz ramp */
	static int hsyncpt;	/* 1200-Hz ramp */

	static double ibuf[DATSIZ]; /* data I channel FIFO */
	static double qbuf[DATSIZ]; /* data Q channel FIFO */
	static int iptr;	/* data channels pointer */

	static double epobuf[SAMPLES]; /* epoch sync comb filter */
	static double epomax;	/* epoch sync amplitude buffer */
	static int epopos;	/* epoch sync position buffer */

	static int iniflg;	/* initialization flag */
	double dtemp;
	long i, temp = 0;

	if (!iniflg) {
		iniflg = 1;
		memset((char *)lpf, 0, sizeof(lpf));
		memset((char *)bpf, 0, sizeof(bpf));
		memset((char *)mf, 0, sizeof(mf));
		memset((char *)ibuf, 0, sizeof(ibuf));
		memset((char *)qbuf, 0, sizeof(qbuf));
		memset((char *)epobuf, 0, sizeof(epobuf));
	}
	up->monitor = isig;		/* change for debug */

	/*
	 * Baseband data demodulation. The 100-Hz subcarrier is
	 * extracted using a 150-Hz IIR lowpass filter. This attenuates
	 * the 1000/1200-Hz sync signals, as well as the 440-Hz and
	 * 600-Hz tones and most of the noise and voice modulation
	 * components.
	 *
	 * Matlab IIR 4th-order IIR elliptic, 150 Hz lowpass, 0.2 dB
	 * passband ripple, -50 dB stopband ripple.
	 */
	data = (lpf[4] = lpf[3]) * 8.360961e-01;
	data += (lpf[3] = lpf[2]) * -3.481740e+00;
	data += (lpf[2] = lpf[1]) * 5.452988e+00;
	data += (lpf[1] = lpf[0]) * -3.807229e+00;
	lpf[0] = isig - data;
	data = lpf[0] * 3.281435e-03
	    + lpf[1] * -1.149947e-02
	    + lpf[2] * 1.654858e-02
	    + lpf[3] * -1.149947e-02
	    + lpf[4] * 3.281435e-03;

	/*
	 * The I and Q data signals are produced by multiplying the
	 * filtered signal by 100-Hz sine and cosine signals,
	 * respectively. The data signals are demodulated by 170-ms
	 * synchronous matched filters to produce the amplitude and
	 * phase signals used by the decoder.
	 */
	i = up->datapt;			/* data I channel */
	up->datapt = (up->datapt + IN100) % 80;
	dtemp = sintab[i] * data / (DATSIZ / (MS * 10));
	up->irig -= ibuf[iptr];
	ibuf[iptr] = dtemp;
	up->irig += dtemp;
	i = (i + 20) % 80;		/* data Q channel */
	dtemp = sintab[i] * data / (DATSIZ / (MS * 10));
	up->qrig -= qbuf[iptr];
	qbuf[iptr] = dtemp;
	up->qrig += dtemp;
	iptr = (iptr + 1) % DATSIZ;

	/*
	 * Baseband sync demodulation. The 1000/1200 sync signals are
	 * extracted using a 600-Hz IIR bandpass filter. This removes
	 * the 100-Hz data subcarrier, as well as the 440-Hz and 600-Hz
	 * tones and most of the noise and voice modulation components.
	 *
	 * Matlab 4th-order IIR elliptic, 800-1400 Hz bandpass, 0.2 dB
	 * passband ripple, -50 dB stopband ripple.
	 */
	syncx = (bpf[8] = bpf[7]) * 4.897278e-01;
	syncx += (bpf[7] = bpf[6]) * -2.765914e+00;
	syncx += (bpf[6] = bpf[5]) * 8.110921e+00;
	syncx += (bpf[5] = bpf[4]) * -1.517732e+01;
	syncx += (bpf[4] = bpf[3]) * 1.975197e+01;
	syncx += (bpf[3] = bpf[2]) * -1.814365e+01;
	syncx += (bpf[2] = bpf[1]) * 1.159783e+01;
	syncx += (bpf[1] = bpf[0]) * -4.735040e+00;
	bpf[0] = isig - syncx;
	syncx = bpf[0] * 8.203628e-03
	    + bpf[1] * -2.375732e-02
	    + bpf[2] * 3.353214e-02
	    + bpf[3] * -4.080258e-02
	    + bpf[4] * 4.605479e-02
	    + bpf[5] * -4.080258e-02
	    + bpf[6] * 3.353214e-02
	    + bpf[7] * -2.375732e-02
	    + bpf[8] * 8.203628e-03;

	/*
	 * The I and Q minute sync signals are produced by multiplying
	 * the filtered signal by 1000-Hz (WWV) and 1200-Hz (WWVH) sine
	 * and cosine signals, respectively. The resulting signals are
	 * demodulated by 800-ms synchronous matched filters to
	 * synchronize the second and minute and to detect which one (or
	 * both) the WWV or WWVH signal is present.
	 */
	wwv_qrz(up, &up->wwv, syncx, csyncpt);
	csyncpt = (csyncpt + IN1000) % 80;
	wwv_qrz(up, &up->wwvh, syncx, hsyncpt);
	hsyncpt = (hsyncpt + IN1200) % 80;
	up->jptr = (up->jptr + 1) % SYNSIZ;

if (up->kptr == 0) {
	up->status &= ~(WWV | WWVH | MSYNC);
	if (up->wwv.state == 2 && up->wwvh.state != 2) {
		up->status |= MSYNC | WWV;
		i = up->wwv.pos - SYNSIZ;
		if (i < 0)
			i += MINUTE;
		up->rsec = (MINUTE - i) / SAMPLES;
		temp = (up->epoch + i) % SAMPLES;
		if (!(up->status & SSYNC))
			up->yepoch = temp;
	} else if (up->wwv.state != 2 && up->wwvh.state == 2) {
		up->status |= MSYNC | WWVH;
		i = up->wwvh.pos - SYNSIZ;
		if (i < 0)
			i += MINUTE;
		up->rsec = (MINUTE - i) / SAMPLES;
		temp = (up->epoch + i) % SAMPLES;
		if (!(up->status & SSYNC))
			up->yepoch = temp;
	}

printf("wwv0 %2d %6ld %4d\n", up->rsec, temp, up->yepoch);

}
	up->kptr = (up->kptr + 1) % MINUTE;

	/*
	 * The second sync pulse is extracted using 5-ms FIR matched
	 * filters at 1000 Hz for WWV or 1200 Hz for WWVH. This pulse is
	 * used for the most precise synchronization, since if provides
	 * a resolution of one sample (125 us).
	 */
	if (up->status & WWV) {

		/*
		 * WWV FIR matched filter, five cycles of 1000-Hz
		 * sinewave.
		 */
		mf[40] = mf[39];
		mfsync = (mf[39] = mf[38]) * 4.224514e-02;
		mfsync += (mf[38] = mf[37]) * 5.974365e-02;
		mfsync += (mf[37] = mf[36]) * 4.224514e-02;
		mf[36] = mf[35];
		mfsync += (mf[35] = mf[34]) * -4.224514e-02;
		mfsync += (mf[34] = mf[33]) * -5.974365e-02;
		mfsync += (mf[33] = mf[32]) * -4.224514e-02;
		mf[32] = mf[31];
		mfsync += (mf[31] = mf[30]) * 4.224514e-02;
		mfsync += (mf[30] = mf[29]) * 5.974365e-02;
		mfsync += (mf[29] = mf[28]) * 4.224514e-02;
		mf[28] = mf[27];
		mfsync += (mf[27] = mf[26]) * -4.224514e-02;
		mfsync += (mf[26] = mf[25]) * -5.974365e-02;
		mfsync += (mf[25] = mf[24]) * -4.224514e-02;
		mf[24] = mf[23];
		mfsync += (mf[23] = mf[22]) * 4.224514e-02;
		mfsync += (mf[22] = mf[21]) * 5.974365e-02;
		mfsync += (mf[21] = mf[20]) * 4.224514e-02;
		mf[20] = mf[19];
		mfsync += (mf[19] = mf[18]) * -4.224514e-02;
		mfsync += (mf[18] = mf[17]) * -5.974365e-02;
		mfsync += (mf[17] = mf[16]) * -4.224514e-02;
		mf[16] = mf[15];
		mfsync += (mf[15] = mf[14]) * 4.224514e-02;
		mfsync += (mf[14] = mf[13]) * 5.974365e-02;
		mfsync += (mf[13] = mf[12]) * 4.224514e-02;
		mf[12] = mf[11];
		mfsync += (mf[11] = mf[10]) * -4.224514e-02;
		mfsync += (mf[10] = mf[9]) * -5.974365e-02;
		mfsync += (mf[9] = mf[8]) * -4.224514e-02;
		mf[8] = mf[7];
		mfsync += (mf[7] = mf[6]) * 4.224514e-02;
		mfsync += (mf[6] = mf[5]) * 5.974365e-02;
		mfsync += (mf[5] = mf[4]) * 4.224514e-02;
		mf[4] = mf[3];
		mfsync += (mf[3] = mf[2]) * -4.224514e-02;
		mfsync += (mf[2] = mf[1]) * -5.974365e-02;
		mfsync += (mf[1] = mf[0]) * -4.224514e-02;
		mf[0] = syncx;
	} else if (up->status & WWVH) {

		/*
		 * WWVH FIR matched filter, six cycles of 1200-Hz
		 * sinewave.
		 */
		mf[40] = mf[39];
		mfsync = (mf[39] = mf[38]) * 4.833363e-02;
		mfsync += (mf[38] = mf[37]) * 5.681959e-02;
		mfsync += (mf[37] = mf[36]) * 1.846180e-02;
		mfsync += (mf[36] = mf[35]) * -3.511644e-02;
		mfsync += (mf[35] = mf[34]) * -5.974365e-02;
		mfsync += (mf[34] = mf[33]) * -3.511644e-02;
		mfsync += (mf[33] = mf[32]) * 1.846180e-02;
		mfsync += (mf[32] = mf[31]) * 5.681959e-02;
		mfsync += (mf[31] = mf[30]) * 4.833363e-02;
		mf[30] = mf[29];
		mfsync += (mf[29] = mf[28]) * -4.833363e-02;
		mfsync += (mf[28] = mf[27]) * -5.681959e-02;
		mfsync += (mf[27] = mf[26]) * -1.846180e-02;
		mfsync += (mf[26] = mf[25]) * 3.511644e-02;
		mfsync += (mf[25] = mf[24]) * 5.974365e-02;
		mfsync += (mf[24] = mf[23]) * 3.511644e-02;
		mfsync += (mf[23] = mf[22]) * -1.846180e-02;
		mfsync += (mf[22] = mf[21]) * -5.681959e-02;
		mfsync += (mf[21] = mf[20]) * -4.833363e-02;
		mf[20] = mf[19];
		mfsync += (mf[19] = mf[18]) * 4.833363e-02;
		mfsync += (mf[18] = mf[17]) * 5.681959e-02;
		mfsync += (mf[17] = mf[16]) * 1.846180e-02;
		mfsync += (mf[16] = mf[15]) * -3.511644e-02;
		mfsync += (mf[15] = mf[14]) * -5.974365e-02;
		mfsync += (mf[14] = mf[13]) * -3.511644e-02;
		mfsync += (mf[13] = mf[12]) * 1.846180e-02;
		mfsync += (mf[12] = mf[11]) * 5.681959e-02;
		mfsync += (mf[11] = mf[10]) * 4.833363e-02;
		mf[10] = mf[9];
		mfsync += (mf[9] = mf[8]) * -4.833363e-02;
		mfsync += (mf[8] = mf[7]) * -5.681959e-02;
		mfsync += (mf[7] = mf[6]) * -1.846180e-02;
		mfsync += (mf[6] = mf[5]) * 3.511644e-02;
		mfsync += (mf[5] = mf[4]) * 5.974365e-02;
		mfsync += (mf[4] = mf[3]) * 3.511644e-02;
		mfsync += (mf[3] = mf[2]) * -1.846180e-02;
		mfsync += (mf[2] = mf[1]) * -5.681959e-02;
		mfsync += (mf[1] = mf[0]) * -4.833363e-02;
		mf[0] = syncx;
	}

	/*
	 * Extract the seconds sync pulse using a 1-s comb filter at
	 * baseband. Correct for the FIR matched filter delay, which is
	 * 5 ms for both the WWV and WWVH filters.
	 */
	i = up->epoch;
	up->epoch = (i + 1) % SAMPLES;
	if (i == 0) {
		wwv_endpoc(up, epomax, epopos);
		up->epomax = epomax;
		epomax = 0;
	}
	dtemp = (epobuf[i] += (mfsync - epobuf[i]) / (MINAVG <<
	    up->avgint));
	if (dtemp > epomax) {
		epomax = dtemp;
		epopos = i - 5 * MS;
		if (epopos < 0)
			epopos += SAMPLES;
	}
}


/*
 * wwv_qrz() - dig in the muck for a minute sync pulse
 */
static void
wwv_qrz(
	struct wwvunit *up,	/* driver structure pointer */
	struct sync *sp,	/* sync channel structure */
	double syncx,		/* bandpass filtered sync signal */
	int index		/* sintab index */
)
{
	double dtemp;
	long i, temp = 0;

	i = index;
	dtemp = sintab[i] * syncx / (SYNSIZ / MS);
	sp->iamp = sp->iamp - sp->ibuf[up->jptr] + dtemp;
	sp->ibuf[up->jptr] = dtemp;
	i = (i + 20) % 80;
	dtemp = sintab[i] * syncx / (SYNSIZ / MS);
	sp->qamp = sp->qamp - sp->qbuf[up->jptr] + dtemp;
	sp->qbuf[up->jptr] = dtemp;
	dtemp = sp->iamp * sp->iamp + sp->qamp * sp->qamp;
	if (dtemp > sp->max) {
		sp->max = dtemp;
		sp->pos = up->kptr;
	}
	if (abs(MOD(up->kptr - sp->lastpos, MINUTE)) > SAMPLES &&
	    dtemp > sp->noise)
		sp->noise = ITHR / 4;
	if (up->kptr == 0) {
		sp->max = sqrt(sp->max);
		temp = sp->max > ITHR && sp->noise < ITHR / 2 &&
		    abs(MOD(sp->pos - sp->lastpos, MINUTE)) < 10 * MS;
		switch (sp->state) {

		/*
		 * In state 0 the station has not been heard for some
		 * time. Look for the biggest blip greater than the
		 * amplitude threshold in the minute and assume that the
		 * minute sync pulse. If found, bump to state 1.
		 */
		case 0:
			if (sp->max >= ITHR)
				sp->state = 1;
			break;

		/*
		 * In state 1 a candidate blip has been found and the
		 * minute is being searched for other trash less than
		 * the threshold. If trash is found less than 6 dB below
		 * the threshold or if the next blip is less than the
		 * threshold, drop back to state 0 and hunt some more.
		 * Otherwise, a legitimate minute pulse has been found,
		 * so bump to state 2.
		 */
		case 1:
			if (!temp) {
				sp->state = 0;
				break;
			}
			sp->state = 2;
			/* fall through */

		/*
		 * In state 2 the blip is confirmed legitimate. Continue
		 * to monitor for trash or signal runt. If so, drop back
		 * to state 1 but do not declare the station has sunk
		 * beneath the waves.
		 */
		case 2:
			if (!temp)
				sp->state = 1;
			break;
		}
printf("WWV %d %5.0f %5.0f %6ld %6ld\n",
    sp->state, sp->max, sp->noise, sp->lastpos, sp->pos);
		sp->max = sp->noise = 0;
		sp->lastpos = sp->pos;
	}
}


/*
 * wwv_endpoc() - process receiver epoch
 *
 * This routine is called at the end of the receiver epoch. It
 * determines the epoch position within the second and disciplines the
 * sample clock using a frequency-lock loop (FLL).
 *
 * Seconds sync is determined in the RF input routine as the maximum
 * over all 8000 samples in the seconds comb filter. To assure accurate
 * and reliable time and frequency discipline, this routine performs a
 * great deal of heavy-handed data filtering and grooming.
 */
static void
wwv_endpoc(
	struct wwvunit *up,	/* driver structure pointer */
	double epomax,		/* epoch max */
	int epopos		/* epoch max position */
	)
{
	static int epoch_mf[3]; /* epoch median filter */
 	static int tepoch;	/* median filter epoch */
	static int tspan;	/* median filter span */
 	static int xepoch;	/* last second epoch */
 	static int zepoch;	/* last averaging interval epoch */
	static int syncnt;	/* second epoch run length counter */
	static int jitcnt;	/* jitter holdoff counter */
	static int avgcnt;	/* averaging interval counter */
	static int avginc;	/* averaging ratchet */

	static int iniflg;	/* initialization flag */
	double dtemp;
	int tmp2, tmp3;

	if (!iniflg) {
		iniflg = 1;
		memset((char *)epoch_mf, 0, sizeof(epoch_mf));
	}

	/*
	 * A three-stage median filter is used to help denoise the
	 * seconds sync pulse. The median sample becomes the candidate
	 * epoch; the difference between the other two samples becomes
	 * the span, which is used currently only for debugging.
	 */
	epoch_mf[2] = epoch_mf[1];
	epoch_mf[1] = epoch_mf[0];
	epoch_mf[0] = epopos;
	if (epoch_mf[0] > epoch_mf[1]) {
		if (epoch_mf[1] > epoch_mf[2]) {
			tepoch = epoch_mf[1];	/* 0 1 2 */
			tspan = epoch_mf[0] - epoch_mf[2];
		} else if (epoch_mf[2] > epoch_mf[0]) {
			tepoch = epoch_mf[0];	/* 2 0 1 */
			tspan = epoch_mf[2] - epoch_mf[1];
		} else {
			tepoch = epoch_mf[2];	/* 0 2 1 */
			tspan = epoch_mf[0] - epoch_mf[1];
		}
	} else {
		if (epoch_mf[1] < epoch_mf[2]) {
			tepoch = epoch_mf[1];	/* 2 1 0 */
			tspan = epoch_mf[2] - epoch_mf[0];
		} else if (epoch_mf[2] < epoch_mf[0]) {
			tepoch = epoch_mf[0];	/* 1 0 2 */
			tspan = epoch_mf[1] - epoch_mf[2];
		} else {
			tepoch = epoch_mf[2];	/* 1 2 0 */
			tspan = epoch_mf[1] - epoch_mf[0];
		}
	}

	/*
	 * The candidate epoch is discarded and the receiver set
	 * unsynchronized if the pulse amplitude is less than the
	 * decision threshold. If the new candidate is within 1 ms of
	 * the last one, the new candidate replaces the last one and the
	 * jitter counter is reset; otherwise, the candidate is ignored
	 * and the jitter counter is incremented. If the jitter counter
	 * exceeds the frequency averaging interval, the new candidate
	 * replaces the old one anyway. The compare counter is
	 * incremented if the new candidate is identical to the last
	 * one; otherwise, it is forced to zero. If the compare counter
	 * increments to 10, the epoch is reset and the receiver is set
	 * synchronized.
	 */
	tmp2 = MOD(tepoch - xepoch, SAMPLES);
	if (epomax < STHR) {
		up->status &= ~SSYNC;
		jitcnt = syncnt = avgcnt = 0;
	} else if (abs(tmp2) <= MS || jitcnt >= (MINAVG << up->avgint))
	    {
		jitcnt = 0;
		if (tmp2 != 0) {
			xepoch = tepoch;
			syncnt = 0;
		} else {
			if (syncnt < CTHR) {
				syncnt++;
			} else {
				up->status |= SSYNC;
				up->yepoch = tepoch;
			}
		}
		avgcnt++;
	} else {
		jitcnt++;
		syncnt = avgcnt = 0;
	}
#ifdef DEBUG
	if (debug && !(up->status & SSYNC) && 0) {
		printf("wwv1 %2d %04x %3d %5.0f", up->rsec, up->status,
		    up->gain, up->epomax);
		printf(" %2d %5.0f %5d %5d %5d %2d %4d\n", avgcnt,
		    epomax, tepoch, tspan, tmp2, syncnt, jitcnt);
	}
#endif /* DEBUG */

	/*
	 * The sample clock frequency is disciplined using a first-order
	 * feedback loop with time constant consistent with the Allan
	 * intercept of typical computer clocks. The loop update is
	 * calculated each averaging interval from the epoch change in
	 * 125-us units and interval length in seconds. The interval is
	 * increased by 25% if the epoch change is equal to or less than
	 * one unit.
	 *
	 * The averaging interval affects other receiver functions,
	 * including the the 1000/1200-Hz comb filter and sample clock
	 * loop. It also affects the 100-Hz subcarrier loop and the bit
	 * and digit comparison counter thresholds.
	 */
	tmp3 = MOD(tepoch - zepoch, SAMPLES);
	if (avgcnt >= (MINAVG << up->avgint)) {
		if (abs(tmp3) < MS / 2) {
			dtemp = (double)tmp3 / avgcnt;
			up->freq += dtemp / SYNCTC;
			if (up->freq > MAXFREQ)
				up->freq = MAXFREQ;
			else if (up->freq < -MAXFREQ)
				up->freq = -MAXFREQ;
			if (abs(tmp3) <= 1 && up->avgint < MAXAVG) {
				if (avginc < 4) {
					avginc++;
				} else {
					avginc = 0;
					up->avgint++;
				}
			}
#ifdef DEBUG
			if (debug && up->avgint < MAXAVG) {
				printf("wwv2 %2d %04x %3d %5.0f",
				    up->rsec, up->status, up->gain,
				    up->epomax);
				printf(
				   " %5d %5d %2d %2d %7.2f %7.2f\n",
				    MINAVG << up->avgint, avgcnt,
				    avginc, tmp3, dtemp / SAMPLES * 1e6,
				    up->freq / SAMPLES * 1e6);
			}
#endif /* DEBUG */
		}
		zepoch = tepoch;
		avgcnt = 0;
	}
}


/*
 * wwv_rsec - process receiver second
 *
 * This routine is called at the end of each receiver second to
 * implement the per-second state machine. The machine assembles BCD
 * digit bits, decodes miscellaneous bits and dances the leap seconds.
 *
 * Normally, the minute has 60 seconds numbered 0-59. If the leap
 * warning bit is set, the last minute (1439) of 30 June (day 181 or 182
 * for leap years) or 31 December (day 365 or 366 for leap years) is
 * augmented by one second numbered 60. This is accomplished by
 * extending the minute interval by one second and teaching the state
 * machine to ignore it. BTW, stations WWV/WWVH cowardly kill the
 * transmitter carrier for a few seconds around the leap to avoid icky
 * details of transmission format during the leap.
 */
double
wwv_rsec(
	struct wwvunit *up,	/* driver structure pointer */
	double bit		/* bit probability */
	)
{
	static int iniflg;	/* initialization flag */
	static double bcddld[4]; /* BCD data bits */
	static double bitvec[61]; /* bit integrator for misc bits */
	char debugcode[80];	/* debug timecode */
	int sw, arg, nsec;
	int temp;
	double dtemp;

	if (!iniflg) {
		iniflg = 1;
		memset((char *)bitvec, 0, sizeof(bitvec));
	}

	/*
	 * Update the bit vector with the latest probabilities. Only a
	 * few of these bits are used, but it's easier to do them all.
	 * After that, crank the seconds state machine.
	 */
	nsec = up->rsec + 1;
	bitvec[up->rsec] += (bit - bitvec[up->rsec]) / TCONST;
	dtemp = bitvec[up->rsec];
	if (!(up->status & MSYNC))
		up->alarm |= 1 << SYNERR;
	sw = progx[up->rsec].sw;
	arg = progx[up->rsec].arg;
	switch (sw) {

	/*
	 * Ignore this second.
	 */
	case IDLE:
		break;
	/*
	 * Save the bit probability in the BCD data vector at the
	 * displacement given by the argument. Note that all bits of the
	 * BCD vector have to be above the data gate threshold to be
	 * valid. Note that the bits in all digits are assumed bad until
	 * the minutes unit digit is synchronized.
	 */
	case COEF1:
		if (up->status & DGATE)
			up->status |= BGATE;
			bcddld[arg] = bit;
		break;

	case COEF2:
		bcddld[arg] = 0;
		break;

	case COEF:
		if (up->status & DGATE || !(up->status & DSYNC))
			up->status |= BGATE;
		bcddld[arg] = bit;
		break;

	/*
	 * Correlate coefficient vector with each valid digit vector and
	 * save in decoding matrix. We step through the decoding matrix
	 * digits correlating each with the coefficients and saving the
	 * greatest and the next lower for later SNR calculation.
	 */
	case DECIM2:
		wwv_corr4(up, &up->decvec[arg], bcddld, bcd2); /* 0-2 */
		break;

	case DECIM3:
		wwv_corr4(up, &up->decvec[arg], bcddld, bcd3); /* 0-3 */
		break;

	case DECIM6:
		wwv_corr4(up, &up->decvec[arg], bcddld, bcd6); /* 0-6 */
		break;

	case DECIM9:
		wwv_corr4(up, &up->decvec[arg], bcddld, bcd9); /* 0-9 */
		break;

	/*
	 * Miscellaneous bits. If above the positive threshold, declare
	 * 1; if below the negative threshold, declare 0; otherwise
	 * error.
	 */
	case MSC20:
		wwv_corr4(up, &up->decvec[YR + 1], bcddld, bcd9); /* 0-9 */
		/* fall through */

	case MSCBIT:
		if (bitvec[up->rsec] > ETHR)
			up->misc |= arg;
		else if (bitvec[up->rsec] < -ETHR)
			up->misc &= ~arg;
		else
			up->alarm |= 1 << SYMERR;
		break;

	/*
	 * The endgame. If the SECWARN bit is set on the last minute of
	 * 30 June or 31 December, the LEPSEC bit is set. At the end of
	 * the minute in which LEPSEC is set the transmitter and
	 * receiver insert an extra second (60) in the timescale and the
	 * minute sync integrator skips a step.
	 */
	case MIN1:
		if (up->decvec[MN].digamp < ETHR)
			up->status &= ~DSYNC;
		else if (up->decvec[MN].phase == 0 &&
		    up->decvec[MN].count >= up->avgint)
			up->status |= DSYNC;
		if (up->tsec == 60) {
			up->status &= ~LEPSEC;
			up->status |= SLPSEC;
			break;
		}
		/* fall through */

	case MIN2:
		up->minset++;
		if (!(up->alarm)) {
			up->status |= INSYNC;
			up->minset = 0;
		}
#ifdef DEBUG
		if (debug) {
			temp = timecode(up, debugcode);
			printf("wwv: timecode %d %s\n", temp,
			    debugcode);
		}
#endif /* DEBUG */
		up->alarm = (up->alarm & ~0x8888) << 1;
		nsec = 0;
		up->errcnt = 0;
		break;
	}
	up->rsec = up->tsec = nsec;
	return (dtemp);
}


/*
 * wwv_data() - calculate bit probability
 *
 * This routine is called at the end of the receiver second to calculate
 * the probabilities that the previous second contained a zero (P0), one
 * (P1) or position indicator (P2) pulse. If not in sync or if the data
 * bit is bad, a bit error is declared and the probabilities are forced
 * to zero. Otherwise, the data gate is enabled and the probabilities
 * are calculated. Note that the data matched filter contributes half
 * the pulse width, or 85 ms..
 */
static double
wwv_data(
	struct wwvunit *up,	/* driver unit pointer */
	double pulse		/* pulse length (sample units) */
	)
{
	double p0, p1, p2;	/* probabilities */
	double dpulse;		/* pulse length in ms */

	p0 = p1 = p2 = 0;
	dpulse = pulse - DATSIZ / 2;

	/*
	 * If the data amplitude or SNR are below threshold or if the
	 * pulse length is less than 170 ms, declare an erasure.
	 */
	if (!(up->status & MSYNC) || up->datamp < DTHR || up->datsnr
	    < NTHR || dpulse < DATSIZ) {
		up->status |= DGATE;
		up->errcnt++;
		if (up->errcnt > BTHR)
			up->alarm |= 1 << MODERR;
		return (0); 
	}

	/*
	 * The probability of P0 is one below 200 ms falling to zero at
	 * 500 ms. The probability of P1 is zero below 200 ms rising to
	 * one at 500 ms and falling to zero at 800 ms. The probability
	 * of P2 is zero below 500 ms, rising to one above 800 ms.
	 */
	up->status &= ~DGATE;
	if (dpulse < (200 * MS)) {
		p0 = 1;
	} else if (dpulse < 500 * MS) {
		dpulse -= 200 * MS;
		p1 = dpulse / (300 * MS);
		p0 = 1 - p1;
	} else if (dpulse < 800 * MS) {
		dpulse -= 500 * MS;
		p2 = dpulse / (300 * MS);
		p1 = 1 - p2;
	} else {
		p2 = 1;
	}

	/*
	 * The ouput is a metric that ranges from -1 (P0), to +1 (P1)
	 * scaled for convenience. An output of zero represents an
	 * erasure, either because of a data error or pulse length
	 * greater than 500 ms. At the moment, we don't use P2.
	 */
	return ((p1 - p0) * MAXSIG);
}


/*
 * wwv_corr4() - determine maximum likelihood digit
 *
 * This routine correlates the received digit vector with the BCD
 * coefficient vectors corresponding to all valid digits at the given
 * position in the decoding matrix. The maximum value corresponds to the
 * maximum likelihood digit, while the ratio of this value to the next
 * lower value determines the digit SNR. Note that, if the digit is
 * invalid, the likelihood vector is averaged toward zero.
 */
static void
wwv_corr4(
	struct wwvunit *up,	/* driver unit pointer */
	struct decvec *vp,	/* decoding table pointer */
	double data[],		/* received data vector */
	double tab[][4]		/* correlation vector array */
	)
{
	double max, nxtmax;	/* metrics */
	double acc;		/* accumulator */
	int mldigit;		/* max likelihood digit */
	int diff;		/* decoding difference */
	int i, j;		/* matrix indices */

	/*
	 * Correlate digit vector with each BCD coefficient vector. If
	 * any BCD digit bit is bad, consider them all bad.
	 */
	max = nxtmax = mldigit = 0;
	for (i = 0; tab[i][0] != 0; i++) {
		acc = 0;
		for (j = 0; j < 4; j++)
			if (!(up->status & BGATE))
				acc += data[j] * tab[i][j];
		acc = vp->like[i] += (acc - vp->like[i]) / TCONST;
		if (acc > max) {
			nxtmax = max;
			max = acc;
			mldigit = i;
		} else if (acc > nxtmax) {
			nxtmax = acc;
		}
	}
	vp->mldigit = mldigit;
	vp->digamp = max;
	vp->digsnr = wwv_snr(max, nxtmax);

	/*
	 * The maximum likelihood digit is compared with the current
	 * clock digit. The difference represents the decoding phase
	 * error. If this difference stays the same for a number of
	 * comparisons, the clock digit is reset to the maximum
	 * likelihood digit.
	 */
	up->status &= ~BGATE;
	if (vp->digamp < ETHR) {
		vp->count = 0;
		up->alarm |= 1 << SYMERR;
	} else if (vp->digsnr < TTHR) {
		up->alarm |= 1 << SYMERR;
	} else {
		diff = mldigit - vp->digit;
		if (diff < 0)
			diff += vp->radix;
		if (diff != vp->phase) {
			vp->count = 0;
			vp->phase = diff;
		} else if (vp->count < up->avgint) {
			vp->count++;
		} else {
			vp->phase = 0;
			vp->digit = mldigit;
		}
	}
	if (vp->digit != mldigit)
		up->alarm |= 1 << DECERR;
#ifdef DEBUG
	if (debug && up->status & MSYNC) {
		printf("wwv4 %2d %04x %3d %5.0f", up->rsec, up->status,
		    up->gain, up->epomax);
		printf(" %2d %d %d %d %d %5.0f %5.1f\n", 
		    vp->radix, vp->digit, vp->mldigit, vp->phase,
		    vp->count, vp->digamp, vp->digsnr);
		}
#endif /* DEBUG */
}


/*
 * wwv_tsec() - transmitter second processing
 *
 * This routine is called at the end of the transmitter second. It
 * implements a state machine that advances the logical clock subject to
 * the funny rules that govern the conventional clock and calendar. Note
 * that carries from the least significant (minutes) digit are inhibited
 * until that digit is synchronized.
 */
static void
wwv_tsec(
	struct wwvunit *up	/* driver structure pointer */
	)
{
	struct decvec *dp;	/* decoding matrix pointer */
	int minute, day, isleap;
	int temp;

	up->tsec++;
	if (up->tsec < 60 || (up->tsec == 60 && up->status & LEPSEC))
		return;
	up->tsec = 0;

	/*
	 * Advance minute unit of the day. If the minute unit is not
	 * synchronized, go no further.
	 */
	dp = &up->decvec[MN];
	temp = carry(dp++);		/* minute units */
	if (!(up->status & DSYNC))
		return;

	/*
	 * Propagate carries through the day.
	 */ 
	if (temp == 0)
		temp = carry(dp++);	/* carry minutes */
	if (temp == 0)
		temp = carry(dp++);	/* carry hours */
	if (temp == 0)
		temp = carry(dp++);

	/*
	 * Decode the current minute and day. Set the leap second enable
	 * bit on the last minute of 30 June and 31 December.
	 */
	minute = up->decvec[MN].digit + up->decvec[MN + 1].digit *
	    10 + up->decvec[HR].digit * 60 + up->decvec[HR +
	    1].digit * 600;
	day = up->decvec[DA].digit + up->decvec[DA + 1].digit * 10 +
	    up->decvec[DA + 2].digit * 100;
	isleap = (up->decvec[YR].digit & 0x3) == 0;
	if (minute == 1439 && (day == (isleap ? 182 : 183) || day ==
	     (isleap ? 365 : 366)) && up->misc & SECWAR)
		up->status |= LEPSEC;

	/*
	 * Roll the day if this the first minute and propagate carries
	 * through the year.
	 */
	if (minute != 1440)
		return;
	minute = 0;
	while (carry(&up->decvec[HR]) != 0); /* advance to minute 0 */
	while (carry(&up->decvec[HR + 1]) != 0);
	day++;
	if (temp == 0)
		temp = carry(dp++);	/* carry days */
	if (temp == 0)
		temp = carry(dp++);
	if (temp == 0)
		temp = carry(dp++);

	/*
	 * Roll the year if this the first day and propagate carries
	 * through the century.
	 */
	if (day != (isleap ? 365 : 366))
		return;
	day = 1;
	while (carry(&up->decvec[DA]) != 1); /* advance to day 1 */
	while (carry(&up->decvec[DA + 1]) != 0);
	while (carry(&up->decvec[DA + 2]) != 0);
	dp = &up->decvec[YR];		/* carry years */
	temp = carry(dp++);
	if (temp)
		carry(dp++);
}


/*
 * carry() - process digit
 *
 * This routine rotates a likelihood vector one position and increments
 * the clock digit modulo the radix. It returns the new clock digit -
 * zero if a carry occured. Once synchronized, the clock digit will
 * match the maximum likelihood digit corresponding to that position.
 */
static int
carry(
	struct decvec *dp	/* decoding table pointer */
	)
{
	int temp;
	int j;

	dp->digit++;			/* advance clock digit */
	if (dp->digit == dp->radix) {	/* modulo radix */
		dp->digit = 0;
	}
	temp = dp->like[dp->radix - 1];	/* rotate likelihood vector */
	for (j = dp->radix - 1; j > 0; j--)
		dp->like[j] = dp->like[j - 1];
	dp->like[0] = temp;
	return (dp->digit);
}


/*
 * wwv_snr() - compute SNR
 */
static double
wwv_snr(
	double signal,		/* signal */
	double noise		/* noise */
	)
{
	if (signal <= 0 || noise <= 0)
		return (0);
	return (20 * log10(signal / noise));
}


/*
 * timecode() - assemble timecode string and length
 *
 * Prettytime format - similar to Spectracom
 *
 * sq yy ddd hh:mm:ss.fff ld du a errs freq cons lset
 *
 * s	sync indicator ('?' or ' ')
 * q	quality character (hex 0-F)
 * yy	year of century
 * ddd	day of year
 * hh	hour of day
 * mm	minute of hour
 * fff	millisecond of second
 * l	leap second warning ' ' or 'L'
 * d	DST state 'S', 'D', 'I', or 'O'
 * du	DUT sign and magnitude in deciseconds
 * a	transmitter identifier ('X', 'C', 'H', 'M')
 * errs	bit errors in last minute * freq	frequency offset (PPM) * cons	averaging time (s) * lset	interval since last clock update (m)
 */
int
timecode(
	struct wwvunit *up,	/* driver structure pointer */
	char *ptr		/* target string */
	)
{
	int year, day, hour, minute, second, frac, dut;
	char synchar, qual, leapchar, dst, duts, ident;
	char *cptr;

	synchar = (up->status & INSYNC) ? ' ' : '?';
	qual = 0;
	if (up->alarm & (1 << (DECERR + 2)))
		qual |= 0x1;
	if (up->alarm & (1 << (SYMERR + 2)))
		qual |= 0x2;
	if (up->alarm & (1 << (MODERR + 2)))
		qual |= 0x4;
	if (up->alarm & (1 << (SYNERR + 2)))
		qual |= 0x8;
	year = up->decvec[7].digit + up->decvec[7].digit * 10;
	day = up->decvec[4].digit + up->decvec[5].digit * 10 +
	    up->decvec[6].digit * 100;
	hour = up->decvec[2].digit + up->decvec[3].digit * 10;
	minute = up->decvec[0].digit + up->decvec[1].digit * 10;
	second = up->tsec;
	frac = (up->tphase * 1000) / SAMPLES;
	leapchar = (up->misc & SECWAR) ? 'L' : ' ';
	dst = dstcod[(up->misc >> 4) & 0x3];
	duts = (up->misc & DUTS) ? '+' : '-';
	dut = up->misc & 0x7;
	if (up->status & WWV && up->status & WWVH)
		ident = 'M';
	else if (up->status & WWV)
		ident = 'C';
	else if (up->status & WWV)
		ident = 'H';
	else
		ident = 'X';
	cptr = ptr;
	sprintf(cptr, "%c%1X", synchar, qual);
	cptr = ptr + strlen(ptr);
	sprintf(cptr, " %02d %03d %02d:%02d:%02d.%.03d", year, day,
	    hour, minute, second, frac);
	cptr = ptr + strlen(ptr);
	sprintf(cptr, " %c%c %c%d %c %d %.1f %d %d", leapchar, dst,
	    duts, dut, ident, up->errcnt, up->freq / SAMPLES * 1e6,
	    (MINAVG << up->avgint), up->minset);
	return (strlen(ptr));
}


/*
 * wwv_gain - adjust codec gain
 *
 * This routine is called once each second. If the signal envelope
 * amplitude is too low, the codec gain is bumped up by four units; if
 * too high, it is bumped down. The decoder is relatively insensitive to
 * amplitude, so this crudity works just fine. The input port is set and
 * the error flag is cleared, mostly to be ornery.
 */
static void
wwv_gain(
	struct peer *peer	/* peer structure pointer */
	)
{
	struct refclockproc *pp;
	struct wwvunit *up;

	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;

	/*
	 * Apparently, the codec uses only the high order bits of the
	 * gain control field. Thus, it may take awhile for changes to
	 * wiggle the hardware bits. Set the new bits in the structure
	 * and call AUDIO_SETINFO. Upon return, the old bits are in the
	 * structure.
	 */
	if (up->clipcnt == 0) {
		up->gain += 4;
		if (up->gain > AUDIO_MAX_GAIN)
		    up->gain = AUDIO_MAX_GAIN;
	} else if (up->clipcnt > SAMPLES / 100) {
		up->gain -= 4;
		if (up->gain < AUDIO_MIN_GAIN)
		    up->gain = AUDIO_MIN_GAIN;
	}
	AUDIO_INITINFO(&info);
	info.record.port = up->port;
	info.record.gain = up->gain;
	info.record.error = 0;
	ioctl(wwv_ctl_fd, (int)AUDIO_SETINFO, &info);
	if (info.record.error)
	    up->errflg |= WWV_ERR_ERROR;
}


/*
 * wwv_audio - initialize audio device
 *
 * This code works with SunOS 4.x and Solaris 2.x; however, it is
 * believed generic and applicable to other systems with a minor twid
 * or two. All it does is open the device, set the buffer size (Solaris
 * only), preset the gain and set the input port. It assumes that the
 * codec sample rate (8000 Hz), precision (8 bits), number of channels
 * (1) and encoding (ITU-T G.711 mu-law companded) have been set by
 * default.
 */
static int
wwv_audio(
	)
{
	/*
	 * Open audio control device
	 */
	if ((wwv_ctl_fd = open("/dev/audioctl", O_RDWR)) < 0) {
		perror("audioctl");
		return(-1);
	}
#ifdef HAVE_SYS_AUDIOIO_H
	/*
	 * Set audio device parameters.
	 */
	AUDIO_INITINFO(&info);
	info.record.buffer_size = AUDIO_BUFSIZ;
	if (ioctl(wwv_ctl_fd, (int)AUDIO_SETINFO, &info) < 0) {
		perror("AUDIO_SETINFO");
		close(wwv_ctl_fd);
		return(-1);
	}
#endif /* HAVE_SYS_AUDIOIO_H */
#ifdef DEBUG
	wwv_show();
#endif /* DEBUG */
	return(0);
}


#ifdef DEBUG
/*
 * wwv_show - display audio parameters
 *
 * This code doesn't really do anything, except satisfy curiousity and
 * verify the ioctl's work.
 */
static void
wwv_show(
	)
{
	if (debug == 0)
	    return;
#ifdef HAVE_SYS_AUDIOIO_H
	ioctl(wwv_ctl_fd, (int)AUDIO_GETDEV, &device);
	printf("wwv: name %s, version %s, config %s\n",
	       device.name, device.version, device.config);
#endif /* HAVE_SYS_AUDIOIO_H */
	ioctl(wwv_ctl_fd, (int)AUDIO_GETINFO, &info);
	printf(
		"wwv: samples %d, channels %d, precision %d, encoding %d\n",
		info.record.sample_rate, info.record.channels,
		info.record.precision, info.record.encoding);
#ifdef HAVE_SYS_AUDIOIO_H
	printf("wwv: gain %d, port %d, buffer %d\n",
	       info.record.gain, info.record.port,
	       info.record.buffer_size);
#else /* HAVE_SYS_AUDIOIO_H */
	printf("wwv: gain %d, port %d\n",
	       info.record.gain, info.record.port);
#endif /* HAVE_SYS_AUDIOIO_H */
	printf(
		"wwv: samples %d, eof %d, pause %d, error %d, waiting %d, balance %d\n",
		info.record.samples, info.record.eof,
		info.record.pause, info.record.error,
		info.record.waiting, info.record.balance);
#ifdef __NetBSD__
	printf("wwv: monitor %d, blocksize %d, hiwat %d, lowat %d, mode %d\n",
	       info.monitor_gain, info.blocksize, info.hiwat, info.lowat, info.mode);
#else /* __NetBSD__ */
	printf("wwv: monitor %d, muted %d\n",
	       info.monitor_gain, info.output_muted);
#endif /* __NetBSD__ */
}
#endif /* DEBUG */

#else
int refclock_wwv_bs;
#endif /* REFCLOCK */
