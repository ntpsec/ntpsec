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
 * connected to the audio codec /dev/audio. The WWV/H signal format
 * consists of a 100-Hz subcarrier with pulse-width modulated data bits.
 * Seconds and minutes synchronization uses a 1000/1200-Hz subcarrier
 * with seconds pips and a 800-ms minutes pulse.
 *
 * The program processes 8000-Hz mu-law companded samples using a
 * lowpass filter to extract the 100-Hz subcarrier and a bandpass filter
 * to extract the seconds/minutes pulses. The seconds pulse is further
 * processed using a comb filter and the minutes pulse using a
 * noncoherent integrator.
 */
/*
 * Interface definitions
 */
#define	PRECISION	(-10)	/* precision assumed (about 1 ms) */
#define	REFID		"WWV"	/* reference ID */
#define	DESCRIPTION	"Generic WWVH Audio Driver" /* WRU */

#define AUDIO_BUFSIZ	160	/* codec buffer size (Solaris only) */
#define SAMPLES		8000	/* nominal sample rate (Hz) */
#define BAUD		80	/* samples per baud interval */
#define OFFSET		128	/* companded sample offset */
#define SIZE		256	/* decompanding table size */
#define SUBFLD		10	/* bits per subfield */
#define FIELD		10	/* subfields per field */
#define MINTC		2	/* min PLL time constant */
#define MAXTC		20	/* max PLL time constant max */
#define	MAXSIG		6000.	/* maximum signal level */
#define DRPOUT		100.	/* dropout signal level */
#define MODMIN		0.5	/* minimum modulation index */
#define MAXFREQ		(250e-6 * SAMPLES) /* freq tolerance (.025%) */
#define PI		3.1415926535 /* the real thing */
#define IGRSIZ		170

/*
 * General purpose status bits (status)
 */
#define SSYNC		0x0001	/* second sync */
#define MSYNC		0x0002	/* minute sync */
#define DGATE		0x0004	/* data gate */
#define BGATE		0x0008	/* data bit error */
#define INSYNC		0x0010	/* clock is synchronized */
#define LEPSEC		0x0020	/* leap second in progress */
#define WWVH		0x0040	/* WWV (0) WWVH (1) select */
#define ERRSTA		0x0080	/* error condition (buffer overrun) */
#define LEAPYR		0x0100	/* leap year flag */
#define DGSYNC		0x0200	/* digit compare error */
#define POPOFF		0x0400	/* frequency popoff */

/*
 * Decoder status flashers (flash) as implmented in the DSP93 program.
 */
#define LED1		0x01	/* signal indicator */
#define LED2		0x02	/* CPU idle */
#define LED3		0x04	/* data indicator */
#define LED4		0x08	/* sync indicator */
#define LED5		0x10	/* data decode */
#define LED6		0x20	/* decoding error */
#define LED7		0x40	/* decoding error */
#define LED8		0x80	/* power indicator (always on) */

/*
 * Alarm status bits (alarm)
 *
 * These bits indicate various alarm conditions, which are decoded to
 * form the quality character included in the timecode. There are four
 * four-bit nibble fields in the word, each corresponding to a specific
 * alarm condition. At the end of each minute, the word is shifted left
 * one position and the two least significant bits of each nibble
 * cleared. The least significant two bits are set if the associated
 * alarm condition is raised during the minute. The least significant
 * bit is cleared at the end of each second. The least significant bit
 * of some nibbles is indicated by a panel LED; the most significant
 * bits are decoded for the quality character.
 */
#define DECERR		0	/* BCD digit decoding error */
#define SYMERR		4	/* low symbol probability */
#define MODERR		8	/* too many data bit errors */
#define SYNERR		12	/* low 1-s/1-m sync pulse amplitude */

/*
 * Tone frequency definitions. Note the nominal sample frequency of 7949
 * Hz must be corrected to 8000 Hz by replicating samples at intervals
 * of about 157 samples, which is equivalent to adding about 420 to the
 * low order word of the epoch counter for each sample.
 */
#define SAMPLES		8000	/* sample clock frequency (Hz) */
#define MS		8	/* actual samples per millisecond */
#define IN100		1	/* 100 Hz incr, 4.5-deg sin table */
#define IN1000		10	/* 1000 Hz incr, 4.5-deg sin table */
#define IN1200		12	/* 1200 Hz incr, 4.5-deg sin table */

/*
 * Odds and ends
 */
#define LIMIT		10000	/* limiter threshold (LED 1) */
#define MAXAVG		256	/* max time constant (8 = 256 s) */
#define MINAVG		4	/* max time constant (8 = 256 s) */
#define PDELAY		188	/* default prop delay (23.5 ms) */
#define TCONST		4-1	/* log2 time constant (about 16 min) */

/*
 * Thresholds. These establish the minimum signal level, minimum SNR and
 * maximum jitter thresholds which establish the error and false alarm
 * rates of the receiver. The values defined here may be on the
 * adventurous side in the interest of the highest sensitivity.
 */
#define CTHR		10 	/* 1-s synch compare threshold */
#define STHR		1000	/* 1-s sync threshold */
#define MTHR		500	/* 1-m sync threshold */
#define DTHR		1000	/* data amplitude threshold */
#define NTHR		100	/* data SNR threshold (dB * 10) */
#define ETHR		1000	/* digit correlator threshold */
#define TTHR		30	/* digit SNR threshold (dB * 10) */
#define BTHR		30	/* max bit error count in second */

/*
 * Miscellaneous status bits (misc)
 *
 * These bits correspond to designated bits in the WWV timecode. The bit
 * probabilities are exponentially averaged over several minutes and
 * processed by a integrator and decision circuit.
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
#define WWV		.0019	/* WWV phase delay */

/*
 * Data bit definitions
 */
#define BIT0		0	/* zero */
#define BIT1		1	/* one */
#define BITP		2	/* position identifier */

/*
 * Debug control
 */
#define BUG1		0x01	/* DSP93 minute timecode */
#define BUG2		0x02	/* DSP93 digit scan */
#define BUG3		0x04	/* DSP93 minute sync */
#define BUG4		0x08	/* DSP93 seconds sync */
#define BUG5		0x10	/* DSP93 miscellaneous bits */
#define BUG6		0x20	/* receiver second */

/*
 * Error flags (up->errflg)
 */
#define WWV_ERR_AMP	0x01	/* low carrier amplitude */
#define WWV_ERR_FREQ	0x02	/* frequency tolerance exceeded */
#define WWV_ERR_MOD	0x04	/* low modulation index */
#define WWV_ERR_SYNCH	0x08	/* frame synch error */
#define WWV_ERR_DECODE	0x10	/* frame decoding error */
#define WWV_ERR_CHECK	0x20	/* second numbering discrepancy */
#define WWV_ERR_ERROR	0x40	/* codec error (overrun) */

/*
 * Table of sin() / PI at 4.5-degree increments. This is used by the
 * 100-Hz matched filter and signal generator.
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
 3.898172e-17, -2.497431e-02, -4.979464e-02, -7.430797e-02, /* 40-43 */
-9.836316e-02, -1.218119e-01, -1.445097e-01, -1.663165e-01, /* 44-47 */
-1.870979e-01, -2.067257e-01, -2.250791e-01, -2.420447e-01, /* 48-51 */
-2.575181e-01, -2.714038e-01, -2.836162e-01, -2.940800e-01, /* 52-55 */
-3.027307e-01, -3.095150e-01, -3.143910e-01, -3.173286e-01, /* 56-59 */
-3.183099e-01, -3.173286e-01, -3.143910e-01, -3.095150e-01, /* 60-63 */
-3.027307e-01, -2.940800e-01, -2.836162e-01, -2.714038e-01, /* 64-67 */
-2.575181e-01, -2.420447e-01, -2.250791e-01, -2.067257e-01, /* 68-71 */
-1.870979e-01, -1.663165e-01, -1.445097e-01, -1.218119e-01, /* 72-75 */
-9.836316e-02, -7.430797e-02, -4.979464e-02, -2.497431e-02, /* 76-79 */
-7.796344e-17};						    /* 80 */

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
 * contains routine address and argument.
 */
struct progx {
	int sw;
	int arg;
};

#define IDLE		0	/* no operation */
#define COEF		1	/* hex bit */
#define COEF1		2	/* special */
#define COEF2		3	/* special */
#define DECIM9		4	/* BCD digit 0-9 */
#define DECIM6		5	/* BCD digit 0-6 */
#define DECIM3		6	/* BCD digit 0-3 */
#define DECIM2		7	/* BCD digit 0-2 */
#define MSCBIT		8	/* miscellaneous bit */
#define MSC20		9	/* miscellaneous bit */		
#define LEAP		10	/* leap second */		
#define MIN1		11	/* minute 1 */		
#define MIN2		12	/* minute 2 */

#define MN		0	/* minute digits (2) */
#define HR		2	/* hour digits (2) */
#define DA		4	/* day digits (3) */
#define YR		7	/* year digits (2) */

struct progx progx[] = {
	{IDLE,	0},		/* 0 punched */
	{IDLE,	0},		/* 1 */
	{MSCBIT, DST2},		/* 2 dst2 */
	{MSCBIT, SECWAR},	/* 3 lw */
	{COEF,	0},		/* 4 1 year */
	{COEF,	1},		/* 5 2 */
	{COEF,	2},		/* 6 4 */
	{COEF,	3},		/* 7 8 */
	{DECIM9, YR},		/* 8 */
	{IDLE,	0},		/* 9 p1 */
	{COEF1, 0},		/* 10 1 minutes */
	{COEF,	1},		/* 11 2 */
	{COEF,	2},		/* 12 4 */
	{COEF,	3},		/* 13 8 */
	{DECIM9, MN},		/* 14 */
	{COEF2, 0},		/* 15 10 */
	{COEF,	1},		/* 16 20 */
	{COEF,	2},		/* 17 40 */
	{COEF,	3},		/* 18 */
	{DECIM6, MN + 1},	/* 19 p2 */
	{COEF,	0},		/* 20 1 hours */
	{COEF,	1},		/* 21 2 */
	{COEF,	2},		/* 22 4 */
	{COEF,	3},		/* 23 8 */
	{DECIM9, HR},		/* 24 */
	{COEF,	0},		/* 25 10 */
	{COEF,	1},		/* 26 20 */
	{COEF,	2},		/* 27 */
	{COEF,	3},		/* 28 */
	{DECIM2, HR + 1},	/* 29 p3 */
	{COEF,	0},		/* 30 1 days */
	{COEF,	1},		/* 31 2 */
	{COEF,	2},		/* 32 4 */
	{COEF,	3},		/* 33 8 */
	{DECIM9, DA},		/* 34 */
	{COEF,	0},		/* 35 10 */
	{COEF,	1},		/* 36 20 */
	{COEF,	2},		/* 37 40 */
	{COEF,	3},		/* 38 80 */
	{DECIM9, DA + 1},	/* 39 p4 */
	{COEF,	0},		/* 40 100 */
	{COEF,	1},		/* 41 200 */
	{COEF,	2},		/* 42 */
	{COEF,	3},		/* 43 */
	{DECIM3, DA + 2},	/* 44 */
	{IDLE,	0},		/* 45 */
	{IDLE,	0},		/* 46 */
	{IDLE,	0},		/* 47 */
	{IDLE,	0},		/* 48 */
	{IDLE,	0},		/* 49 p5 */
	{MSCBIT, DUTS},		/* 50 dut+- */
	{COEF,	0},		/* 51 10 year */
	{COEF,	1},		/* 52 20 */
	{COEF,	2},		/* 53 40 */
	{COEF,	3},		/* 54 80 */
	{MSC20, DST1},		/* 55 dst1 */
	{MSCBIT, DUT1},		/* 56 0.1 dut */
	{MSCBIT, DUT2},		/* 57 0.2 */
	{LEAP,	DUT4},		/* 58 0.4 */
	{MIN1,	0},		/* 59 p6 */
	{MIN2,	0}		/* 60 leap second */
};

/*
 * BCD coefficients for maximum likelihood decode
 */
#define P15	((double)1.)	/* max positive number */
#define N15	((double)-1.)	/* max negative number */

/*
 * Digits 0-9
 */
#define P9	(P15 / 4.)	/* mark (+1) */
#define N9	(N15 / 4.)	/* space (-1) */

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
#define P6	(P15 / 3.)	/* mark (+1) */
#define N6	(N15 / 3.)	/* space (-1) */

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
 * Digits 0-3 (days hundreds)
 */
#define P3	(P15 / 2.)	/* mark (+1) */
#define N3	(N15 / 2.)	/* space (-1) */

double bcd3[][4] = {
	{N3, N3, 0, 0}, 	/* 0 */
	{P3, N3, 0, 0}, 	/* 1 */
	{N3, P3, 0, 0}, 	/* 2 */
	{P3, P3, 0, 0}, 	/* 3 */
	{0, 0, 0, 0}		/* backstop */
};

/*
 * Digits 0-2 (hours tens)
 */
#define P2	(P15 / 2.)	/* mark (+1) */
#define N2	(N15 / 2.)	/* space (-1) */

double bcd2[][4] = {
	{N2, N2, 0, 0}, 	/* 0 */
	{P2, N2, 0, 0}, 	/* 1 */
	{N2, P2, 0, 0}, 	/* 2 */
	{0, 0, 0, 0}		/* backstop */
};
/*
 * Decoding table format
 */
struct decvec {
	int radix;		/* radix (10, 6, 4, 3) */
	int mldigit;		/* maximum likelihood digit */
	int ckdigit;		/* current clock digit */
	int count;		/* match count */
	double like[10];	/* likelihood integrator 0-9 */
};

/*
 * WWV unit control structure
 */
struct wwvunit {
	u_char	timecode[21];	/* timecode string */
	l_fp	timestamp;	/* audio sample timestamp */
	l_fp	tick;		/* audio sample increment */
	double	comp[SIZE];	/* decompanding table */
	double	integ[BAUD];	/* baud integrator */
	double	phase, freq;	/* logical clock phase and frequency */
	double	zxing;		/* phase detector integrator */
	double	yxing;		/* phase detector display */
	double	modndx;		/* modulation index */
	int	errflg;		/* error flags */
	int	bufcnt;		/* samples in buffer */
	int	bufptr;		/* buffer index pointer */
	int	pollcnt;	/* poll counter */
	int	port;		/* codec port */
	int	gain;		/* codec gain */
	int	clipcnt;	/* sample clipped count */
	int	seccnt;		/* second countdown */

	/*
	 * The decoding matrix consists of nine row vectors, one for
	 * each digit of the timecode. Each vector includes (1) the
	 * current clock digit, (2) an 11-stage shift register used to
	 * save the current probabilities for each of the ten possible
	 * digit values plus a carry value, (3) the most recent
	 * maximum-likelihood digit decoded and (4) a compare counter.
	 * The digits are stored from least to most significant order.
	 * The timecode is formed from the digits of maximum value
	 * reading in the opposite order: yy ddd hh:mm.
	 *
	 * Decoding table
	 */
	struct decvec decvec[9];

	/*
	 * Variables used to establish 1-s sync epoch within the second
	 * and discipline master oscillator frequency
	 */
	int synptr;		/* 1-s sync integrator pointer */
	double max;		/* 1-s sync max amplitude */
	int treg[3];		/* 1-s sync epoch median filter */
	int yepoch;		/* 1-s sync epoch */
	int ppimax;		/* 1-m sync max amplitude */
	int ppipos;		/* 1-m sync epoch at max amplitude */

	int minset;

	/*
	 * Variables used to establish basic system timing
	 */
	int epoch;		/* epoch ramp */
	int rphase;		/* 1-s ramp at receiver */
	int rsec;		/* receiver seconds counter */
	double tconst;
	int avgint;
	double dpulse;

	double irig;
	double qrig;
	double secmax;
	int secpos;
	double epomax;
	int epopos;

	/*
	 * Variables used to establish the second, minute and day
	 */
	int cdelay;		/* WWV propagation delay */
	int hdelay;		/* WWVH propagation delay */
	int tphase;		/* 1-s ramp at transmitter */
	int tsec;		/* second of minute (0-59/60) */
	int minute;		/* minute of day (0-1439) */
	int day;		/* day of year (1-365/366) */
	int minsec;		/* minutes since last set */
	int one;		/* the one and only one */

double monitor;

	/*
	 * Variables used to estimate signal levels and bit/digit
	 * probabilities
	 */
	double secamp;		/* seconds sync amplitude (for debug) */
	double minamp;		/* minutes sync amplitude */
	double datamp;		/* max signal amplitude */
	double noiamp;		/* average noise amplitude */
	double datsnr;		/* data SNR (dB) */
	double digamp;		/* max digit amplitude */
	double digsnr;		/* digit SNR (dB) */
	double subphase;	/* 100 Hz phase adj */

	/*
	 * Variables used to establish status and alarm conditions
	 */
	int status;		/* status bits */
	int misc;		/* miscellaneous timecode bits */
	int alarm;		/* alarm flashers */
	int errcnt;		/* data bit error counter */
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
static	void	wwv_epoch	P((struct wwvunit *, double));
static	void	wwv_rf		P((struct wwvunit *, double));
static	void	wwv_endpoc	P((struct wwvunit *, double, int));
static	void	wwv_vsec	P((struct wwvunit *, double));
static	double	wwv_data	P((struct wwvunit *, double));
static	void	corr4		P((struct wwvunit *, struct decvec *,
				    double [], double [][]));
static	void	wwv_gain	P((struct peer *));
static	int	wwv_audio	P((void));
static	void	wwv_show	P((void));
static	double	wwv_snr		P((double, double));
static	void	tock		P((struct wwvunit *));
static	int	carry		P((struct decvec *));
static	int	mod8k		P((int));
static	void	timecode	P((struct wwvunit *));

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

static int	wwv_debug = 0xff; /* debug control */

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
	up->pollcnt = 2;

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

	up->avgint = 4;
	up->decvec[MN].radix = 10;	/* minutes */
	up->decvec[MN + 1].radix = 6;
	up->decvec[HR].radix = 10;	/* hours */
	up->decvec[HR + 1].radix = 2;
	up->decvec[DA].radix = 10;	/* days */
	up->decvec[DA + 1].radix = 10;
	up->decvec[DA + 2].radix = 3;
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
 * track the irig clock by dropping or duplicating codec samples.
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
	l_fp	ltemp;		/* l_fp temp */

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
		sample = up->comp[~*dpt++ & 0xff];

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
			wwv_epoch(up, sample);
			wwv_epoch(up, sample);
		} else {
			wwv_epoch(up, sample);
		}
		L_ADD(&up->timestamp, &up->tick);

		/*
		 * Once each second adjust the codec port and gain.
		 */
		up->seccnt = (up->seccnt + 1) % SAMPLES;
		if (up->seccnt == 0) {
			if (pp->sloppyclockflag & CLK_FLAG2)
			    up->port = AUDIO_LINE_IN;
			else
			    up->port = AUDIO_MICROPHONE;
			wwv_gain(peer);
			up->clipcnt = 0;
		}
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

	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;
	/*
	 * Keep book for tattletales
	 */
	if (up->pollcnt == 0) {
		refclock_report(peer, CEVNT_TIMEOUT);
		return;
	}
	up->pollcnt--;
	pp->polls++;
	
}


/*
 * Main loop
 *
 * There are three 1-s epoches used by this program, all spanning the
 * range 0-7999 sample ticks and all repeating at intervals of exactly
 * one second. The receiver epoch begins upon arrival of the 1-s sync
 * pulse; while the transmitter epoch begins before it by the specified
 * propagation delay. The third epoch, called simply that in the
 * comments, begins at an arbitrary time, depending on when this program
 * actually starts up.
 *
 * This section scans the epoch looking for seconds sync and establishes
 * the transmitter and receiver epoch. The seconds sync relative to the
 * epoch is determined by the 1-s sync pulse detected in the RF input
 * routine. The actual nominal A/D sample clock is determined by the
 * hardware at 8000 samples/s. The phase and frequency of the sample
 * clock are adjusted to match the 1-s sync pulse as broadcast. The
 * receiver epoch establishes the modulation maxima, minima and data
 * sample time. The transmitter epoch leads the receiver epoch by the
 * propagation delay. It establishes the clock time and implements the
 * sometimes idiosyncratic conventional clock time and civil calendar. 
 *
 * The receiver epoch contains one of three data signals at 100 Hz, a
 * 200-ms pulse (P0), 500-ms pulse (P1) or 800-ms pulse (P2), but the
 * first 30 ms of every pusle is punched out for the 1-s sync pulse. The
 * data are filtered by a 170-ms synchronous matched filter in the RF
 * input routine. The noise reference level is the averaged in-phase
 * filter output at the end of the first 30-ms segment, while the signal
 * reference level is at the end of the first 200-ms segment. The slice
 * level is midway between the signal and noise reference levels. The
 * length of the data bit pulse is determined from the reciever epoch at
 * the first negative-going crossing of the slice level following the
 * first 200-ms segment. The probabilities of P0, P1 and P2 are
 * determined from the signal crossing time, delayed by one-half the
 * matched filter delay.
 *
 * Most communications radios use a lowpass filter in the audio stages,
 * which can do nasty things to the 100-Hz subcarrier phase relative to
 * the 1-s sync pulse. Therefore, the 100-Hz signal is disciplined to
 * the averaged quadrature-phase filter output at the end of the first
 * 200-ms segment.
 */
void
wwv_epoch(
	struct wwvunit *up,	/* driver unit pointer */
	double sig		/* decompanded sample */
)
{
	double dtemp;

	/*
	 * Extract the data and sync signals
	 */
	wwv_rf(up, sig);
	if (up->rphase < 30 * MS) {

		/*
		 * Estimate the noise level by integrating the energy
		 * before epoch 30 ms.
		 */
		up->noiamp += (up->irig - up->noiamp) / up->avgint;
	} else if (up->rphase == 170 * MS) {

		/*
		 * Strobe the maximum I-channel data signal at epoch 170
		 * ms. Compute the SNR and adjust the 100-Hz reference
		 * oscillator phase using the Q-channel amplitude at
		 * that epoch.
		 */
		up->datamp = up->irig;
		if (up->datamp < 0)
			up->datamp = 0;
		up->datsnr = wwv_snr(up->datamp, up->noiamp);
		up->subphase = up->qrig;
	} else if (up->rphase > 170 * MS) {

		/*
		 * Strobe the negative zero crossing after epoch 170 ms
		 * and record the epoch at that time. This defines the
		 * size of the data pulse, which will later be converted
		 * into bit probabilities.
		 */
		dtemp = (up->datamp + up->noiamp) / 2;
		if (up->irig < dtemp && up->dpulse == 0)
			up->dpulse = up->rphase;
	}

	/*
	 * At the end of the transmitter second, crank the clock state
	 * machine.
	 */
	if (up->status & WWVH)
		dtemp = up->hdelay;	/* Kaui */
	else
		dtemp = up->cdelay;	/* Ft. Collins */
	if (dtemp + up->rphase < SAMPLES - 1) {
		up->tphase++;
	} else {
		tock(up);
		up->tphase = 0;
	}

	/*
	 * At the end of the receiver second, process the data bit and
	 * update the decoding matrix probabilities.
	 */
	up->rphase++;
	if (up->epoch == up->yepoch) {
		up->rphase = up->dpulse = 0;
		wwv_vsec(up, wwv_data(up, up->dpulse));
		if (wwv_debug & BUG6)
			printf(
	"wwv6 %2d %04x %4d %6.0f %6.0f %6.0f %5d %6.0f %5d %4d %7.2f\n",
 			    up->rsec, up->status, up->gain, up->datamp,
			    up->noiamp, up->secmax, up->secpos,
			    up->epomax, up->yepoch, up->avgint,
			    up->freq / SAMPLES * 1e6);
	}
}

/*
 * wwv_rf() - process rf signals
 *
 * This routine grooms and filters decompanded raw audio samples. The
 * output signals include the 100-Hz baseband data signal in quadrature
 * form, plus the epoch number of the seconds sync signal and the
 * second number of the minutes sync signal. Note in passing that the
 * data subcarrier phase must be disciplined  due to the very different
 * audio passband characteristics of typical shortwave radios.
 *
 * The output signals include the epoch maximum and position and second
 * maximum and position. The epoch position provides the master
 * reference phase for all signal and timing functions, while the second
 * position identifies the first second of the minute. The epoch and
 * second maximum are used to calculate SNR for gating function.
 *
 * Demodulation operations are based on three internally generated
 * ramps, one at 1000 Hz (WWV) or 1200 Hz (WWVH) for the epoch and
 * second sync signals, another at 100 Hz for the data subcarrier and a
 8 third at 1 Hz for the epoch sync.
 */
static void
wwv_rf(
	struct wwvunit *up,	/* driver structure pointer */
	double isig		/* input signal */
	)
{
	static double lpf[5];	/* 150-Hz lpf buffer */
	double data;		/* lpf output */
	static double bpf[9];	/* 1000/1200-Hz bpf buffer */
	double syncx;		/* bpf output */
	static double mf[41];	/* 1000/1200-Hz mf buffer */
	double mfsync;		/* mf output */

	static double ibuf[170 * MS]; /* I channel data buffer */
	static double qbuf[170 * MS]; /* Q channel data buffer */
	static int datapt;	/* 10-ms ramp */
	static int iptr;	/* data buffer pointer */

	static double isec, qsec; /* second integrators */
	static double secbuf[60]; /* second comb filter */
	static int syncpt;	/* 1-ms ramp */
	static double secmax;	/* second max */
	static int secpos;	/* second max position */

	static double epobuf[SAMPLES]; /* epoch comb filter */
	static double epomax;	/* epoch max */
	static int epopos;	/* epoch max position */

	static int iniflg;	/* initialization flag */
	double dtemp;		/* double temp */
	int i, j;		/* int temps */

	if (!iniflg) {
		iniflg = 1;
		memset((char *)lpf, 0, sizeof(lpf));
		memset((char *)bpf, 0, sizeof(bpf));
		memset((char *)mf, 0, sizeof(mf));
		memset((char *)ibuf, 0, sizeof(ibuf));
		memset((char *)qbuf, 0, sizeof(qbuf));
		memset((char *)secbuf, 0, sizeof(secbuf));
		memset((char *)epobuf, 0, sizeof(epobuf));
	}

	/*
	 * Baseband data demodulation. The 100-Hz subcarrier is
	 * extracted using a 150-Hz lowpass filter. This attenuates the
	 * 1000/1200-Hz sync signals, as well as the 440-Hz and 600-Hz
	 * tones and most of the noise and voice modulation components.
	 * The I and Q baseband signals are produced by multiplying by
	 * sin and cosine signals, respectively. The baseband signals
	 * are processed by 170-ms matched filters, which  produce the I
	 * and Q signals used by the decoder.
	 */
	/*
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
	j = iptr;
	iptr = (iptr + 1) % (170 * MS);
	i = datapt;			/* I channel matched filter */
	datapt = (i + IN100) % 80;
	dtemp = sintab[i] * data / (17 * PI);
	up->irig -= ibuf[j];
	ibuf[j] = dtemp;
	up->irig += dtemp;
	i -= 20;			/* Q channel matched filter */
	if (i < 0)
		i += 80;
	dtemp = sintab[i] * data / (17 * PI);
	up->qrig -= qbuf[j];
	qbuf[j] = dtemp;
	up->qrig += dtemp;

	/*
	 * Baseband sync demodulation. The 1000/1200 sync signals are
	 * extracted using a 600-Hz bandpass filter. This removes the
	 * 100-Hz data subcarrier, as well as the 440-Hz and 600-Hz
	 * tones and most of the noise and voice modulation components.
	 */
	/*
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
	 * Extract the minutes sync pulse using a 800-ms comb filter at
	 * 1000/1200 Hz.
	 */
	if (up->rphase == 0) {
		i = up->rsec;
		secbuf[i] = (sqrt(isec * isec + qsec * qsec) -
		    secbuf[i]) / 8;
		if (secbuf[i] > secmax) {
			secmax = secbuf[i];
			secpos = i;
		}
		isec = qsec = 0;
		if (up->subphase > 0) {
			iptr++;
			if (iptr >= 80)
				iptr -= 80;
		} else if (up->subphase < 0) {
			iptr--;
			if (iptr < 0)
				iptr += 80;
		}
		up->subphase = 0;
		if (up->rsec == 0) {
			up->secmax = secmax;
			up->secpos = secpos;
			if (secmax < MTHR || !(up->status & SSYNC)) {
				up->status &= ~MSYNC;
				up->alarm |= 0xf << SYNERR;
			} else {
				up->status |= MSYNC;
				up->rsec -= secpos;
				if (up->rsec < 0)
					up->rsec += 60;
			}
			secmax = 0;
		}
	}
	i = syncpt;
	if (up->status & WWVH)
		syncpt = (syncpt + IN1200) % 80;
	else
		syncpt = (syncpt + IN1000) % 80;
	if (up->rphase < 800 * MS) {
		isec += sintab[i] * syncx / 800;
		i -= 20;
		if (i < 0)
			i += 80;
		qsec += sintab[i] * syncx / 800;
	}

	/*
	 * Enhance the seconds sync pulse using 5-ms matched filters at
	 * 1000/1200 Hz.
	 */
	if (up->status & WWVH) {

		/*
		 * WWVH Matched filter, six cycles at 1200-Hz sinewave.
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
	} else {

		/*
		 * WWV Matched filter, five cycles at 1000-Hz sinewave.
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
	}

	/*
	 * Extract the seconds sync pulse using a 1-s comb filter at
	 * baseband.
	 */
	i = up->epoch;
	up->epoch = (i + 1) % SAMPLES;
	if (i == 0) {
		wwv_endpoc(up, epomax, epopos);
		up->epomax = epomax;
		up->epopos = epopos;
		epomax = 0;
	}
	dtemp = (epobuf[i] += (mfsync - epobuf[i]) / up->avgint);
	if (dtemp > epomax) {
		epomax = dtemp;
		epopos = i;
	}
}


/*
 * This routine is called at the end of the epoch. It determines scan
 * phase and adjusts frequency using a frequency-lock loop.
 *
 * Seconds sync is determined in the RF input routine as the maximum
 * over all 8000 samples in the seconds comb filter. To assure accurate
 * and reliable time and frequency discipline, this routine performs a
 * great deal of heavy-handed data filtering and conditioning.
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

	static int iniflg;	/* initialization flag */
	double dtemp;
	int tmp2, tmp3;

	if (!iniflg) {
		iniflg = 1;
		memset((char *)epoch_mf, 0, sizeof(epoch_mf));
	}

	/*
	 * A three-stage median filter is used to help denoise the
	 * seconds sync epoch. The median sample becomes the epoch
	 * referenced; the difference between the other two samples
	 * becomes the span.
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
	 * The sample is discarded if the median filter span is greater
	 * than 1 ms or if the 1-s sync pulse amplitude is less than the
	 * decision threshold. If the difference between the current and
	 * the last sample is less than 1 ms, the sample is considered
	 * valid and the jitter counter is reset to zero; otherwise, the
	 * sample is ignored and the jitter counter is incremented. If
	 * the jitter counter exceeds the frequency averaging interval,
	 * the new sample is considered valid anyway and replaces the
	 * old one. The compare counter is incremented if the current
	 * sample is identical to the last one; otherwise, it is forced
	 * to zero. If the compare counter increments to 10, the
	 * receiver epoch is reset to the 1-s pulse epoch as broadcast.
	 */
	tmp2 = mod8k(tepoch - xepoch);
	if (epomax < STHR || tspan > MS) {
		syncnt = jitcnt = 0;
		up->status &= ~SSYNC;
		up->alarm |= 0xf << SYNERR;
		xepoch = zepoch = tepoch;
		avgcnt = 0;
	} else {
		if (abs(tmp2) <= MS || jitcnt >= up->avgint) {
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
		} else {
			jitcnt++;
		}
		if (wwv_debug & BUG3) 
			printf("wwv3 %5d %6.0f %5d %5d %5d %5d %5d\n",
			    avgcnt, epomax, tepoch, tspan, tmp2, syncnt,
			    jitcnt);
		avgcnt++;
	}

	/*
	 * This section calculates the frequency error and adjusts the
	 * sample clock to nominal zero frequency offset. The frequency
	 * is updated each averaging interval by a constant times epoch
	 * change divided by interval. The interval is doubled if the
	 * absolute adjustment is equal to or less than 125 us/interval.
	 *
	 * The averaging interval affects other receiver functions,
	 * including the the 1000/1200-Hz comb filter and sample clock
	 * loop. It also affects the 100-Hz subcarrier loop and the bit
	 * and digit comparison counter thresholds.
	 */
	tmp3 = mod8k(tepoch - zepoch);
	if (avgcnt >= up->avgint) {
		if (abs(tmp3) > MS / 2 && !(up->status & POPOFF)) {
			up->status |= POPOFF;
		} else {
			up->status &= ~POPOFF;
			dtemp = (double)tmp3 / avgcnt;
			up->freq += dtemp / 4;
			if (up->freq > MAXFREQ)
				up->freq = MAXFREQ;
			else if (up->freq < -MAXFREQ)
				up->freq = -MAXFREQ;
			if (abs(tmp3) <= 1 && up->avgint < MAXAVG)
				up->avgint *= 2;
			if (wwv_debug & BUG4) 
				printf("wwv4 %5d %5d %5d %7.2f %7.2f\n",
				    avgcnt, up->avgint, tmp3, dtemp /
				    SAMPLES * 1e6, up->freq / SAMPLES *
				    1e6);
		}
		zepoch = tepoch;
		avgcnt = 0;
	}
}


/*
 * This routine is called at the end of the receiver second to determine
 * minute synchronization. A 1000/1200-Hz tone burst of 800-ms duration
 * is sent during second zero of the minute. This is detected using a
 * pair of 800-ms synchronous integrators, one for the I phase and the
 * other for the Q phase. The sum of the I and Q squares is processed by
 * a 60-stage comb filter to determine the second containing the tone
 * burst.
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
void
wwv_vsec(
	struct wwvunit	*up,	/* driver structure pointer */
	double bit		/* bit probability */
	)
{
	int sw, arg;
	double bcddld[4];
	static double bitvec[60];
	double fill = 1.;

	up->alarm &= ~0x1111;	/* update flashers */
	bitvec[up->rsec] += (bit - bitvec[up->rsec]) / up->avgint;
	sw = progx[up->rsec].sw;
	arg = progx[up->rsec].arg;
	switch (sw) {

	case IDLE:
		up->rsec++;
		break;
	/*
	 * Save bit value in coefficient vector at arg. Note that all
	 * bits of the BCD character have to be above the data gate
	 * threshold to be valid. Note that the bits in all digits are
	 * assumed bad until the minutes unit digit is synchronized.
	 */
	case COEF2:
		up->rsec++;
		break;

	case COEF1:
		up->status &= ~DGSYNC;
		/* fall through */

	case COEF:
		if (up->status & (DGATE | DGSYNC))
			up->status |= BGATE;
		bcddld[arg] = bit;
		up->rsec++;
		break;

	/*
	 * Correlate coefficient vector with each valid digit vector and
	 * save in decoding matrix. We step through the decoding matrix
	 * digits correlating each with the coefficients and saving the
	 * greatest and the next lower for later SNR calculation.
	 */
	case DECIM2:
		corr4(up, &up->decvec[arg], bcddld, bcd2); /* 0-2 */
		up->rsec++;
		break;

	case DECIM3:
		corr4(up, &up->decvec[arg], bcddld, bcd3); /* 0-3 */
		up->rsec++;
		break;

	case DECIM6:
		corr4(up, &up->decvec[arg], bcddld, bcd6); /* 0-6 */
		up->rsec++;
		break;

	case DECIM9:
		corr4(up, &up->decvec[arg], bcddld, bcd9); /* 0-9 */
		up->rsec++;
		break;

	/*
	 * Miscellaneous bits. If above the positive threshold, declare
	 * 1; if below the negative threshold, declare 0; otherwise
	 * error.
	 */
	case MSC20:
		corr4(up, &up->decvec[arg], bcddld, bcd9); /* 0-9 */
		/* fall through */

	case MSCBIT:
		if (bitvec[up->rsec] > ETHR)
			up->status |= arg;
		else if (bitvec[up->rsec] < ETHR)
			up->status &= ~arg;
		else
			up->alarm |= (0xf << SYMERR);
		up->rsec++;
		break;

	/*
	 * The endgame
	 */
	case LEAP:
		if (up->misc & SECWAR)
			up->status |= LEPSEC;
		up->rsec++;
		break;

	case MIN1:
		if (up->status & LEPSEC) {
			up->rsec++;
			break;
		}
		/* fall through */

	case MIN2:
		up->status &= ~LEPSEC;
		up->minsec++;
		if (!(up->alarm & (0xf << MODERR))) {
			up->status &= ~INSYNC;
			up->minset = 0;
		}
		timecode(up);
		up->alarm = (up->alarm & ~0x8888) << 1;
		up->rsec = 0;
		up->errcnt = 0;
		break;
	}
}


/*
 * data() - phase discriminator.
 *
 * This routine is called at the end of the receiver second to calculate
 * the probabilities that the previous second contained a zero (P0), one
 * (P1) or position indicator (P2) pulse. If not in sync or if the data
 * bit is bad, a bit error is declared and the probabilities are forced
 * to zero. Otherwise, the data gate is enabled and the probabilities
 * are calculated. Note that the data matched filter contributes half
 * the pulse width, or IGRSIZ / 2 = 85 ms..
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
	dpulse = (pulse - IGRSIZ / 2) / 2;
	up->status |= DGATE;

	/*
	 * If the data amplitude or SNR are below threshold or if the
	 * pulse length is less than 170 ms, declare an erasure.
	 */
	if (!(up->status & (SSYNC | MSYNC)) || up->datamp < DTHR ||
	    up->datsnr < NTHR || dpulse < 170 * MS) {
		up->errcnt++;
		if (up->errcnt > BTHR)
			up->alarm |= 0xf << MODERR;
		up->status &= ~DGATE;
		return (0); 
	}

	/*
	 * The probability of P0 is one below 200 ms falling to zero at
	 * 500 ms. The probability of P1 is zero below 200 ms rising to
	 * one at 500 ms and falling to zero at 800 ms. The probability
	 * of P2 is zero below 500 ms, rising to one above 800 ms.
	 */
	if (dpulse < 200 * MS) {
		p0 = 1.;
	} else if (dpulse < 500 * MS) {
		dpulse -= 200 * MS;
		p1 = dpulse / 300 * MS;
		p0 = 1. - p1;
	} else if (dpulse < 800 * MS) {
		dpulse -= 500 * MS;
		p2 = dpulse / 300 * MS;
		p1 = 1. - p2;
	} else {
		p2 = 1.;
	}

	/*
	 * The ouput is a metric that ranges from -1 (P0), to +1 (P1).
	 * An output of zero represents an erasure, either because of a
	 * data error or pulse length greater than 800 ms.
	 */
	return ((p1 - p2) / 2.);

}


/*
 * corr4(data, table) - compute distance between BCD data and model
 */
static void
corr4(
	struct wwvunit *up,	/* driver unit pointer */
	struct decvec *vp,	/* decoding table pointer */
	double data[],		/* received data vector */
	double tab[][4]		/* correlation vector array */
	)
{
	char mldigit;		/* max likelihood digit */
	int i, j;		/* matrix indices */
	double max, nxtmax;	/* metrics */
	double acc;		/* accumulator */
	int diff = 0;		/* decoding difference */
	int tthr;

	/*
	 * Correlate coefficient vector with each valid symbol and save
	 * in decoding matrix. We step through the decoding matrix
	 * digits correlating each with the coefficients and saving the
	 * greatest and the next lower for later SNR calculation.
	 */
	max = nxtmax = 0;
	for (i = 0; tab[i][0] != 0; i++) {
		acc = 0;
		if (up->status & BGATE) {
			for (j = 0; j < 4; j++)
				acc += data[j] * tab[i][j];
		}
		acc = vp->like[i] += (acc - vp->like[i]) / up->tconst;
		if (acc > max) {
			nxtmax = max;
			max = acc;
			mldigit = i;
		} else if (acc > nxtmax) {
			nxtmax = acc;
		}
	}

return;

	/*
	 * At this point we have identified the maximum likelihood
	 * digit. The next section determines if it passes sanity checks
	 * and then compares with the current clock digit. The
	 * difference between the maximum likelihood digit and current
	 * clock digit represents the phase error, which should remain
	 * constant (most of the time) as succeeding digits in the same
	 * position are decoded. When a sufficient number of digits have
	 * been accumulated with the same such difference, the decoded
	 * data can be considered authoritative and used to correct the
	 * current clock. Great care is taken here to avoid noise from
	 * changing a presumed correct clock.
	 */
	up->status &= ~BGATE;
	up->digsnr = 20 * log10(up->digamp / up->digsnr);
	if (up->digamp < ETHR) {
		vp->count = 0;
		up->alarm &= ~(0xf << SYMERR);
	} else if (up->digsnr < tthr) {
		up->alarm &= ~(0xf << SYMERR);
	} else {
		diff = vp->mldigit - mldigit;
		if (diff != 0) {
			vp->mldigit = mldigit;
			vp->count = 0;
		} else if (vp->count < up->avgint + 2) {
			vp->count++;
		} else {
			vp->mldigit = mldigit;
			vp->count = 0;
		}
	}
	if (vp->mldigit == mldigit)
		up->alarm |= 0xf << DECERR;
	if (wwv_debug & BUG2)
		printf("wwv2 %2d %2d %d %d %d %7.2f\n", 
		    up->rsec, vp->radix, vp->mldigit, vp->ckdigit,
		    vp->count, vp->like[vp->mldigit]);
}

/*
 * log10(signal, noise) - compute SNR
 */
static double
wwv_snr(
	double signal,		/* signal */
	double noise		/* noise */
	)
{
	double frac;
	int fexp;
	double dtemp;

	if (signal < 0 || noise <= 0)
		return (0);
	dtemp = signal / noise * 10;
	frac = frexp(dtemp, &exp);
	return (3 * fexp);
}


/*
 * This routine is called at the end of the transmitter second. It
 * implements a state machine that advances the logical clock subject to
 * the funny rules that govern the conventional clock and calendar. Note
 * that carries from the least significant (minutes) digit are inhibited
 * until that digit is synchronized.
 */
static void
tock(
	struct wwvunit *up	/* driver structure pointer */
	)
{
	struct decvec *dp;
	int temp;
	int minute, day;

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
/*
	if (!(up->status & INSYNC && up->status & DGSYNC))
		return;
*/

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
	 * Decode the current minute and day. Set the leap year bit is
	 * the year is divisible by 4. Set the leap second enable bit on
	 * the last minute of 30 June and 31 December.
	 */
	minute = up->decvec[MN].ckdigit + up->decvec[MN + 1].ckdigit *
	    10 + up->decvec[HR].ckdigit * 60 + up->decvec[HR +
	    1].ckdigit * 600;
	day = up->decvec[DA].ckdigit + up->decvec[DA + 1].ckdigit * 10 +
	    up->decvec[DA + 2].ckdigit * 100;
	up->status &= ~LEAPYR;
	if ((up->decvec[YR].ckdigit & 0x3) == 0)
		up->status |= LEAPYR;
	up->status &= ~SECWAR;
	if (minute == 1439 && (day == ((up->status & LEAPYR) ? 183 :
	    182) || day == ((up->status & LEAPYR) ? 366 : 365)))
		up->status |= SECWAR;

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
	if (day != (up->status & LEAPYR) ? 366 : 365)
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
 * This routine rotates a digit vector one position and increments the
 * assigned digit modulo the radix. It returns the carry indicator. Once
 * synchornized, the value of each digit position will match the maximum
 * likelihood digit corresponding to that position.
 */
static int
carry(
	struct decvec *dp	/* decoding table pointer */
	)
{
	int j;
	int temp;

	dp->ckdigit++;			/* advance clock digit */
	if (dp->ckdigit == dp->radix) {	/* modulo radix */
		dp->ckdigit = 0;
	}
	temp = dp->like[dp->radix - 1];
	for (j = dp->radix - 1; j > 0; j--) /* rotate digit vector */
		dp->like[j] = dp->like[j - 1];
	dp->like[0] = temp;
	return (dp->ckdigit);
}

/*
 * Compute mod 8k
 */
static int
mod8k(
	int arg
	)
{
	if (arg < 0)
		return (-(-arg % SAMPLES));
	else
		return (arg % SAMPLES);
}

/*
 * timecode() - desplay timecode for watchers
 *
 * Prettytime format
 *
 * sq a yy ddd hh:mm:ss.fff ld dut ttttt fffff ggggg uuuuuCR/LF
 *
 * s	sync indicator ('?' or ' ')
 * q	quality character (hex 0-F)
 * a	transmitter identifier ('C' or 'H')
 * yy	year of century
 * ddd	day of year
 * hh	hour of day
 * mm	minute of hour
 * fff	millisecond of second
 * l	leap second warning ' ' or 'L'
 * d	DST state 'S', 'D', 'I', or 'O'
 * dut	DUT sign and magnitude in deciseconds
 * bbbbb good data bit counter * fffff frequency offset in units of about 1.9 ns/s * ggggg frequency averaging time in seconds * uuuuu interval since last new data update in minutes
 */
void
timecode(
	struct wwvunit *up	/* driver structure pointer */
	)
{
	int year, day, hour, minute, second, frac, dut;
	char synchar, station, qual, leap, dst, duts;

	synchar = (up->status & INSYNC) ? ' ' : '?';
	qual = 0;
	if (up->alarm & (1 << (SYNERR + 2)))
		qual |= 0x8;
	if (up->alarm & (1 << (MODERR + 2)))
		qual |= 0x4;
	if (up->alarm & (1 << (MODERR + 2)))
		qual |= 0x2;
	if (up->alarm & (1 << (MODERR + 2)))
		qual |= 0x1;
	station = (up->status & WWVH) ? 'H' : 'C';
	year = up->decvec[7].ckdigit + up->decvec[7].ckdigit * 10;
	day = up->decvec[4].ckdigit + up->decvec[5].ckdigit * 10 +
	    up->decvec[6].ckdigit * 100;
	hour = up->decvec[2].ckdigit + up->decvec[3].ckdigit * 10;
	minute = up->decvec[0].ckdigit + up->decvec[1].ckdigit * 10;
	second = up->tsec;
	frac = (up->tphase * 1000) / SAMPLES;
	leap = (up->status & SECWAR) ? 'L' : ' ';
	dst = dstcod[(up->misc >> 4) & 0x3];
	duts = (up->misc & DUTS) ? '-' : '+';
	dut = up->misc & 0x7;
	
	printf("%c %1x %c ", synchar, qual, station);
	printf("%02d %03d %02d:%02d:%02d.%.03d", year, day, hour,
	    minute, second, frac);
	printf(" %c%c%c.%d %d %.3f %d %d\n", leap, dst, duts, dut,
	    up->errcnt, up->freq / SAMPLES * 1e6, up->avgint,
	    up->minset);
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
 * This code works with SunOS 4.1.3 and Solaris 2.6; however, it is
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
	printf("irig: name %s, version %s, config %s\n",
	       device.name, device.version, device.config);
#endif /* HAVE_SYS_AUDIOIO_H */
	ioctl(wwv_ctl_fd, (int)AUDIO_GETINFO, &info);
	printf(
		"irig: samples %d, channels %d, precision %d, encoding %d\n",
		info.record.sample_rate, info.record.channels,
		info.record.precision, info.record.encoding);
#ifdef HAVE_SYS_AUDIOIO_H
	printf("irig: gain %d, port %d, buffer %d\n",
	       info.record.gain, info.record.port,
	       info.record.buffer_size);
#else /* HAVE_SYS_AUDIOIO_H */
	printf("irig: gain %d, port %d\n",
	       info.record.gain, info.record.port);
#endif /* HAVE_SYS_AUDIOIO_H */
	printf(
		"irig: samples %d, eof %d, pause %d, error %d, waiting %d, balance %d\n",
		info.record.samples, info.record.eof,
		info.record.pause, info.record.error,
		info.record.waiting, info.record.balance);
#ifdef __NetBSD__
	printf("irig: monitor %d, blocksize %d, hiwat %d, lowat %d, mode %d\n",
	       info.monitor_gain, info.blocksize, info.hiwat, info.lowat, info.mode);
#else /* __NetBSD__ */
	printf("irig: monitor %d, muted %d\n",
	       info.monitor_gain, info.output_muted);
#endif /* __NetBSD__ */
}
#endif /* DEBUG */

#else
int refclock_wwv_bs;
#endif /* REFCLOCK */
