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
#define CYCLE		8	/* samples per carrier cycle */
#define SUBFLD		10	/* bits per subfield */
#define FIELD		10	/* subfields per field */
#define MINTC		2	/* min PLL time constant */
#define MAXTC		20	/* max PLL time constant max */
#define	MAXSIG		6000.	/* maximum signal level */
#define DRPOUT		100.	/* dropout signal level */
#define MODMIN		0.5	/* minimum modulation index */
#define MAXFREQ		(250e-6 * SAMPLES) /* freq tolerance (.025%) */
#define PI		3.1415926535 /* the real thing */

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
#define POPOFF		0z0400	/* frequency popoff */

/*
 * Decoder status flashers (flash), as implmented in the DSP93 program.
 */
#define LED1		0x01	/* signal indicator */
#define LED2		0x02	/* CPU idle */
#define LED3		0x04	/* data indicator */
#define LED4		0x08	/* sync indicator */
#define LED5		0x10	/* data decode */
#define LED6		0x20	/* decoding error */
#define LED7		0x40	/* decoding error */
#define LED8		0x80	/* power indicator */

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
#define COMSIZ		8000	/* sample clock frequency (Hz) */
#define MS		8	/* actual samples per millisecond */
#define IN1000		10	/* 1000 Hz incr, 4.5-deg sin table */
#define IN1200		12	/* 1200 Hz incr, 4.5-deg sin table */

/*
 * Odds and ends
 */
#define P15		32767	/* max positive number */
#define M15		-P15	/* max negative number */
#define LIMIT		10000	/* limiter threshold (LED 1) */
#define MAXAVG		8-1	/* max time constant (8 = 256 s) */
#define AVGINC		0x4000	/* LSW averaging interval increment */
#define PDELAY		188	/* default prop delay (23.5 ms) */
#define TCONST		4-1	/* log2 time constant (about 16 min) */

/*
 * Thresholds. These establish the minimum signal level, minimum SNR and
 * maximum jitter thresholds which establish the error and false alarm
 * rates of the receiver. The values defined here may be on the
 * adventurous side in the interest of the highest sensitivity.
 */
#define CTHR		10 	/* 1-s synch compare threshold */
#define STHR		1500	/* 1-s sync threshold */
#define MTHR		500	/* 1-m sync threshold */
#define DTHR		1000	/* data amplitude threshold */
#define MNTHR		100	/* data SNR threshold (dB * 10) */
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
/*
double sintab = {
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
 * Seconds routine decode
 */
#define IDLE		0	/* do nothing */
#define COEF		1	/* accumulate BCD digit */
#define MSCBIT		2	/* miscellaneous bit 1 */
#define MSC20		3	/* miscellaneous bit 2 */
#define DECIM2		4	/* decimal update digits 0-2 */
#define DECIM3		5	/* decimal update digits 0-3 */
#define DECIM6		6	/* decimal update digits 0-6 */
#define DECIM9		7	/* decimal update digits 0-9 */
#define LEAP		8	/* leap second test */
#define MIN1		9	/* minute 1 */
#define MIN2		10	/* minute 2 */

/*
 * Command segment dispatch table indexed by second number. Each entry
 * contains routine address and argument.
 */
struct proc[] = {
	int case;
	int arg;
};

#define YRPROB		0	/* year digits (2) */
#define DAPROB		2	/* day digits (3) */
#define HRPROB		5	/* hour digits (2) */
#define MNPROB		7	/* minute digits (2) */
#define SSPROB		9	/* second digits (2) */

struct prog prog = {
	{IDLE,	0},		/* 0 punched */
	{IDLE,	0},		/* 1 */
	{MSCBIT, DST2},		/* 2 dst2 */
	{MSCBIT, SECWAR},	/* 3 lw */
	{COEF,	0},		/* 4 1 year */
	{COEF,	1},		/* 5 2 */
	{COEF,	2},		/* 6 4 */
	{COEF,	3},		/* 7 8 */
	{DECIM9, YRPROG},	/* 8 *
	{IDLE,	0},		/* 9 p1 */
	{COEF1, 0},		/* 10 1 minutes */
	{COEF,	1},		/* 11 2 */
	{COEF,	2},		/* 12 4 */
	{COEF,	3},		/* 13 8 */
	{DECIM9, MNPROB},	/* 14 */
	{COEF2, 0},		/* 15 10 */
	{COEF,	1},		/* 16 20 */
	{COEF,	2},		/* 17 40 */
	{COEF,	3},		/* 18 */
	{DECIM6, MNPROB + 1},	/* 19 p2 */
	{COEF,	0},		/* 20 1 hours */
	{COEF,	1},		/* 21 2 */
	{COEF,	2},		/* 22 4 */
	{COEF,	3},		/* 23 8 */
	{DECIM9, HRPROB},	/* 24 */
	{COEF,	0},		/* 25 10 */
	{COEF,	1},		/* 26 20 */
	{COEF,	2},		/* 27 */
	{COEF,	3},		/* 28 */
	{DECIM2, HRPROB + 1},	/* 29 p3 */
	{COEF,	0},		/* 30 1 days */
	{COEF,	1},		/* 31 2 */
	{COEF,	2},		/* 32 4 */
	{COEF,	3},		/* 33 8 */
	{DECIM9, DAPROB},	/* 34 */
	{COEF,	0},		/* 35 10 */
	{COEF,	1},		/* 36 20 */
	{COEF,	2},		/* 37 40 */
	{COEF,	3},		/* 38 80 */
	{DECIM9, DAPROB + 1},	/* 39 p4 */
	{COEF,	0},		/* 40 100 */
	{COEF,	1},		/* 41 200 */
	{COEF,	2},		/* 42 */
	{COEF,	3},		/* 43 */
	{DECIM3, DAPROB + 2)},	/* 44 */
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
/*
 * Digits 0-9
 */
#define m9		p15 / 4	/* mark (+1) */
#define s9		m15 / 4	/* space (-1) */

struct bcd bcd9 = {
	{s9,s9,s9,s9},		/* 0 */
	{m9,s9,s9,s9},		/* 1 */
	{s9,m9,s9,s9},		/* 2 */
	{m9,m9,s9,s9},		/* 3 */
	{s9,s9,m9,s9},		/* 4 */
	{m9,s9,m9,s9},		/* 5 */
	{s9,m9,m9,s9},		/* 6 */
	{m9,m9,m9,s9},		/* 7 */
	{s9,s9,s9,m9},		/* 8 */
	{m9,s9,s9,m9},		/* 9 */
	{0,0,0,0}		/* backstop */
};

/*
 * Digits 0-6 (10 minutes)
 */
#define m6		p15 / 3	/* mark (+1) */
#define s6		m15 / 3	/* space (-1) */

struct bcd bcd6	= {
	{s6,s6,s6,0},		/* 0 */
	{m6,s6,s6,0},		/* 1 */
	{s6,m6,s6,0},		/* 2 */
	{m6,m6,s6,0},		/* 3 */
	{s6,s6,m6,0},		/* 4 */
	{m6,s6,m6,0},		/* 5 */
	{s6,m6,m6,0},		/* 6 */
	{0,0,0,0}		/* backstop */
};

/*
 * Digits 0-3 (100 days)
 */
#define m3		p15 / 2	/* mark (+1) */
#define s3		m15 / 2	/* space (-1) */

struct bcd bcd3	= {
	{s3,s3,0,0},		/* 0 */
	{m3,s3,0,0},		/* 1 */
	{s3,m3,0,0},		/* 2 */
	{m3,m3,0,0},		/* 3 */
	{0,0,0,0}		/* backstop */
};

/*
 * Digits 0-2 (10 hours)
 */
#define m2		p15 / 2	/* mark (+1) */
#define s2		m15 / 2	/* space (-1) */

struct bcd bcd2	= {
	{s2,s2,0,0},		/* 0 */
	{m2,s2,0,0},		/* 1 */
	{s2,m2,0,0},		/* 2 */
	{0,0,0,0}		/* backstop */
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
	 * Decoding table format
	 */
	struct decvec[9] = {
		char mlchar;	/* maximum likelihood character */
		double decint[10]; /* digit integrator 0-9 */
		char last;	/* last decoded character */
		int count;	/* character count */
	};

	/*
	 * Variables used to establish 1-s sync epoch within the second
	 * and discipline master oscillator frequency
	 */
	int synptr;		/* 1-s sync integrator pointer */
	double max;		/* 1-s sync max amplitude */
	int tepoch;		/* 1-s sync epoch at max amplitude */
	int treg[3];		/* 1-s sync epoch median filter */
	int xepoch;		/* 1-s sync epoch (for compare) */
	int yepoch;		/* 1-s sync epoch */
	int zepoch;		/* 1-s sync saved last interval */
	int syncnt;		/* 1-s sync run-length counter */
	int jitcnt;		/* 1-s sync run-length counter */
	int avgcnt;		/* averaging interval counter */
	int avgint;		/* log2 averaging time */
	int ppimax;		/* 1-m sync max amplitude */
	int ppipos;		/* 1-m sync epoch at max amplitude */

	/*
	 * Variables used to establish basic system timing
	 */
	int epoch;		/* epoch ramp */
	int fudge;		/* epoch phase */
	int incrx;		/* epoch frequency */
	int milli;		/* 1000/1200-Hz ramp */
	int cycle;		/* (2) 100-Hz ramp */
	int rphase;		/* 1-s ramp at receiver */
	int rsec;		/* receiver seconds counter */

	/*
	 * Variables used to establish the second, minute and day
	 */
	int cdelay;		/* WWV propagation delay */
	int hdelay;		/* WWVH propagation delay */
	int tphase;		/* 1-s ramp at transmitter */
	int tsec;		/* second of minute (0-59/60) */
	int minute;		/* minute of day (0-1439) */
	int day;		/* day of year (1-365/366) */
	int minset;		/* minutes since last set */
	int one;		/* the one and only one */

	/*
	 * Variables used to estimate signal levels and bit/digit
	 * probabilities
	 */
	int secamp;		/* seconds sync amplitude (for debug) */
	int minamp;		/* minutes sync amplitude */
	int datamp;		/* max signal amplitude */
	int noiamp;		/* average noise amplitude */
	int datsnr;		/* data SNR (dB) */
	int digamp;		/* max digit amplitude */
	int digsnr;		/* digit SNR (dB) */

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
static	void	wwv_endpoc	P((struct wwvunit *, int));
static	void	wwv_vsec	P((struct wwvunit *, int));
static	void	wwv_data	P((struct wwvunit *, int));
static	void	wwv_corr4	P((struct wwvunit *, struct decode *,
				    struct bcd *, struct bcd *));
static	void	wwv_gain	P((struct peer *));
static	int	wwv_audio	P((void));
static	void	wwv_debug	P((void));

/*
 * Transfer vector
 */
struct	refclock refclock_irig = {
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
static char	hexchar[] = {	/* really quick decoding table */
	'0', '8', '4', 'c',	/* 0000 0001 0010 0011 */
	'2', 'a', '6', 'e',	/* 0100 0101 0110 0111 */
	'1', '9', '5', 'd',	/* 1000 1001 1010 1011 */
	'3', 'b', '7', 'f'	/* 1100 1101 1110 1111 */
};


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
	up->tc = MINTC;
	up->decim = 1;
	up->fdelay = WWV_B;
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
		perror("irig:");
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
		up->timecnt = 0;
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
	rfinp(sig);

	if (rphase < 30 * MS) {

		/*
		 * Estimate the noise level by integrating the energy
		 * before epoch 30 ms.
		 */
		noiamp += (irig - noiamp) / tconst;
	} else if (rphase == 170 * MS) {

		/*
		 * Strobe the maximum I-channel data signal at epoch 170
		 * ms. Compute the SNR and adjust the 100-Hz reference
		 * oscillator phase using the Q-channel amplitude at
		 * that epoch.
		 */
		datamp = irig;
		if (datamp < 0)
			datamp = 0;
		datsnr = wwv_snr(datamp, noiamp) 
		if (up->status & SSYNC) {
			cycle += qrig * avgint
		cycle %= 80;
	} else if (rphase > 170 * MS) {

		/*
		 * Strobe the negative zero crossing after epoch 170 ms
		 * and record the epoch at that time. This defines the
		 * size of the data pulse, which will later be converted
		 * into bit probabilities.
		 */
		dtemp = (datamp + noiamp) / 2;
		if (irig < dtemp && dpulse == 0)
			dpulse = rphase;
	}

	/*
	 * At the end of the transmitter second, crank the clock state
	 * machine.
	 */
	tphase++;
	if (up->status & WWVH)
		dtemp = hdelay	/* Kaui */
	else
		dtemp = cdelay;	/* Ft. Collins */
	if (dtemp + rphase == COMSIZ)
		blah
	tock();
	tphase = 0;

	/*
	 * At the end of the receiver second, process the data bit and
	 * update the decoding matrix probabilities.
	 */
	rphase++;
	if (epoch == yepoch) {
		data(up, dpulse);
		vsec(up);
		rphase = dpulse = 0;
	}

	/*
	 * Advance 1-ms and 10-ms ramps, and determine if a stuffing
	 * cycle is needed. We don't care about the 1000/1200-Hz phase,
	 * but we do care about the 100-Hz phase, since that has been
	 * fiddled above.
	 */
	if (up->status & WWVH)
		milli = ((milli + IN1200) % 80);
	else
		milli = ((milli + IN1000) % 80);
	cycle = (cycle++ % 80);
	fudge
}

/*
 * wwv_rf() - process rf signals
 *
 * This routine grooms and filters decompanded raw audio samples. The
 * output signals include the 100-Hz baseband data signal in quadrature
 * form, plus the sample number of the seconds sync signal and the
 * second number of the minutes sync signal. A gated AGC signal is
 * derived from the seconds sync signal. Note in passing that the more
 * natural choice of subcarrier amplitude can't be used due to the very
 * different audio passband characteristics of typical shortwave radios.
 *
 * The demodulation operations are based on a number of ramps generated
 * by the timing module.
 *	name	freq		used by
 *	epoch	epoch counter	sec sync
 *	cycle	100 Hz		ramp for syntab matched filter
 *	milli	1000/1200 Hz	ramp for syntab
 *	rsec	seconds counter
 *	rphase	epoch counter
 */
double
wwv_rf(
	struct wwvunit *up;	/* driver structure pointer */
	double isig		/* input signal */
	)
{
	static double bpf[9];	/* bpf delay line */
	static double lpf[5];	/* lpf delay line */
	static double mf[41];	/* mf delay line */
	double sync;		/* sync signal */
	double data;		/* data signal */
	static double mintab[60]; /* 1-m sync matched filter */
	static double sectab[COMSIZ]; /* 1-s sync matched filter */
	static double irig;	/* data I channel */
	static double qrig;	/* data Q channel */
	static double ibuf[170 * MS]; /* data I channel filter */
	static double qbuf[170 * MS]; /* data Q channel filter */
	static double secmax;	/* 1-s max sync amplitude */
	statuc int secpos;	/* 1-s sync position */
	static double minmax;	/* 1-m max sync amplitude */
	statuc int minpos;	/* 1-m max position */
	double inpsig;		/* groomed input signal */
	double dtemp;		/* double temp */
	int i, j;		/* int temps */

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
	i = up->cycle;			/* I channel matched filter */
	dtemp = sintab[i] * data;
	up->irig -= ibuf[iptr];
	igrbuf[iptr] = dtemp;
	up->irig += dtemp;
	i -= 20;			/* Q channel matched filter */
	if (i < 0)
		i += 80;
	dtemp = sintab[i] * data;
	up->qrig -= qbuf[iptr];
	igrbuf[iptr] = dtemp;
	up->qrig += dtemp;
	iptr = (iptr++ % (170 * MS));

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
	sync = (bpf[8] = bpf[7]) * 4.897278e-01;
	sync += (bpf[7] = bpf[6]) * -2.765914e+00;
	sync += (bpf[6] = bpf[5]) * 8.110921e+00;
	sync += (bpf[5] = bpf[4]) * -1.517732e+01;
	sync += (bpf[4] = bpf[3]) * 1.975197e+01;
	sync += (bpf[3] = bpf[2]) * -1.814365e+01;
	sync += (bpf[2] = bpf[1]) * 1.159783e+01;
	sync += (bpf[1] = bpf[0]) * -4.735040e+00;
	bpf[0] = isig - sync;
	sync = bpf[0] * 8.203628e-03
	    + bpf[1] * -2.375732e-02
	    + bpf[2] * 3.353214e-02
	    + bpf[3] * -4.080258e-02
	    + bpf[4] * 4.605479e-02;
	    + bpf[5] * -4.080258e-02
	    + bpf[6] * 3.353214e-02
	    + bpf[7] * -2.375732e-02
	    + bpf[8] * 8.203628e-03;

	/*
	 * Extract the minutes sync pulse using a 800-ms comb filter at
	 * 1000/1200 Hz.
	 */
	i = milli;
	if (up->up->status & SSYNC)
		dtemp = sync;
	else
		dtemp = 0;
	dtemp = sintab(i) * sync;
	i -= 20;
	if (i < 0)
		i += 80;
	etemp = (sintab(i) * sync;
	dtemp = dtemp * dtemp + etemp * etemp;
	if (!(up->up->status & SSYNC))
		dtemp = 0;
	if (up->tepoch < 800 * MS) {
		i = up->rsec;
		dtemp = secbuf[i] += (dtemp - secbuf[i]) / 8;
		if (dtemp > secmax) {
			secmax = dtemp;
			secpos = i;
		}
	}
	if (rsec == 60 - 1) {
		secmax = 0;
		up->secmax = secmax;
	}

	/*
	 * Enhance the seconds sync pulse using 5-ms matched filters at
	 * 1000/1200 Hz.
	 */
	if (up->status & WWVH) {

		/*
		 * WWVH Matched filter, six cycles at 1200-Hz sinewave.
		 */
		mfsync = (mf[40] = mf[39]) * 4.833363e-02;
		mfsync += (mf[39] = mf[38]) * 5.681959e-02;
		mfsync += (mf[38] = mf[37]) * 1.846180e-02;
		mfsync += (mf[37] = mf[36]) * -3.511644e-02;
		mfsync += (mf[36] = mf[35]) * -5.974365e-02;
		mfsync += (mf[35] = mf[34]) * -3.511644e-02;
		mfsync += (mf[34] = mf[33]) * 1.846180e-02;
		mfsync += (mf[33] = mf[32]) * 5.681959e-02;
		mfsync += (mf[32] = mf[31]) * 4.833363e-02;
		mf[31] = mf[30];
		mfsync += (mf[30] = mf[29]) * -4.833363e-02;
		mfsync += (mf[29] = mf[28]) * -5.681959e-02;
		mfsync += (mf[28] = mf[27]) * -1.846180e-02;
		mfsync += (mf[27] = mf[26]) * 3.511644e-02;
		mfsync += (mf[26] = mf[25]) * 5.974365e-02;
		mfsync += (mf[25] = mf[24]) * 3.511644e-02;
		mfsync += (mf[24] = mf[23]) * -1.846180e-02;
		mfsync += (mf[23] = mf[22]) * -5.681959e-02;
		mfsync += (mf[22] = mf[21]) * -4.833363e-02;
		mf[21] = mf[20];
		mfsync += (mf[20] = mf[19]) * 4.833363e-02;
		mfsync += (mf[19] = mf[18]) * 5.681959e-02;
		mfsync += (mf[18] = mf[17]) * 1.846180e-02;
		mfsync += (mf[17] = mf[16]) * -3.511644e-02;
		mfsync += (mf[16] = mf[15]) * -5.974365e-02;
		mfsync += (mf[15] = mf[14]) * -3.511644e-02;
		mfsync += (mf[14] = mf[13]) * 1.846180e-02;
		mfsync += (mf[13] = mf[12]) * 5.681959e-02;
		mfsync += (mf[12] = mf[11]) * 4.833363e-02;
		mf[11] = mf[10];
		mfsync += (mf[10] = mf[9]) * -4.833363e-02;
		mfsync += (mf[9] = mf[8]) * -5.681959e-02;
		mfsync += (mf[8] = mf[7]) * -1.846180e-02;
		mfsync += (mf[7] = mf[6]) * 3.511644e-02;
		mfsync += (mf[6] = mf[5]) * 5.974365e-02;
		mfsync += (mf[5] = mf[4]) * 3.511644e-02;
		mfsync += (mf[4] = mf[3]) * -1.846180e-02;
		mfsync += (mf[3] = mf[2]) * -5.681959e-02;
		mfsync += (mf[2] = mf[1]) * -4.833363e-02;
		mf[1] = mf[0];
		mfsync += mf[0] = sync;
	} else {

		/*
		 * WWV Matched filter, five cycles at 1000-Hz sinewave.
		 */
		mfsync = (mf[40] = mf[39]) * 4.224514e-02;
		mfsync += (mf[39] = mf[38]) * 5.974365e-02;
		mfsync += (mf[38] = mf[37]) * 4.224514e-02;
		mf[37] = mf[36];
		mfsync += (mf[36] = mf[35]) * -4.224514e-02;
		mfsync += (mf[35] = mf[34]) * -5.974365e-02;
		mfsync += (mf[34] = mf[33]) * -4.224514e-02;
		mf[33] = mf[32];
		mfsync += (mf[32] = mf[31]) * 4.224514e-02;
		mfsync += (mf[31] = mf[30]) * 5.974365e-02;
		mfsync += (mf[30] = mf[29]) * 4.224514e-02;
		mf[29] = mf[28];
		mfsync += (mf[28] = mf[27]) * -4.224514e-02;
		mfsync += (mf[27] = mf[26]) * -5.974365e-02;
		mfsync += (mf[26] = mf[25]) * -4.224514e-02;
		mf[25] = mf[24];
		mfsync += (mf[24] = mf[23]) * 4.224514e-02;
		mfsync += (mf[23] = mf[22]) * 5.974365e-02;
		mfsync += (mf[22] = mf[21]) * 4.224514e-02;
		mf[21] = mf[20];
		mfsync += (mf[20] = mf[19]) * -4.224514e-02;
		mfsync += (mf[19] = mf[18]) * -5.974365e-02;
		mfsync += (mf[18] = mf[17]) * -4.224514e-02;
		mf[17] = mf[16];
		mfsync += (mf[16] = mf[15]) * 4.224514e-02;
		mfsync += (mf[15] = mf[14]) * 5.974365e-02;
		mfsync += (mf[14] = mf[13]) * 4.224514e-02;
		mf[13] = mf[12];
		mfsync += (mf[12] = mf[11]) * -4.224514e-02;
		mfsync += (mf[11] = mf[10]) * -5.974365e-02;
		mfsync += (mf[10] = mf[9]) * -4.224514e-02;
		mf[9] = mf[8];
		mfsync += (mf[8] = mf[7]) * 4.224514e-02;
		mfsync += (mf[7] = mf[6]) * 5.974365e-02;
		mfsync += (mf[6] = mf[5]) * 4.224514e-02;
		mf[5] = mf[4];
		mfsync += (mf[4] = mf[3]) * -4.224514e-02;
		mfsync += (mf[3] = mf[2]) * -5.974365e-02;
		mfsync += (mf[2] = mf[1]) * -4.224514e-02;
		mf[1] = mf[0];
		mfsync += mf[0] = sync;
	}

	/*
	 * Extract the seconds sync pulse using a 1-s comb filter at
	 * baseband.
	 */
	i = up->epoch;
	dtemp = secbuf[i] += (dtemp - secbuf[i]) / tconst;
	if (dtemp > secmax) {
		secmax = dtemp;
		secpos = i;
	}
	if (i == COMSIZ - 1) {
		secmax = 0;
		up->secpos = secpos;
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
void
wwv_endpoc(
	struct wwvunit,		/* driver structure pointer */
	int blip		/* seconds sync epoch */
	)
{
	static epoch_mf[3];	/* epoch median filter */
 	double tepoch;		/* filtered epoch */
	double xepoch;		/* filtered epoch */
 	double yepoch;		/* filtered epoch */
	double tspan;		/* filtered epoch span */

	/*
	 * A three-stage median filter is used to help denoise the
	 * seconds sync epoch. The median sample becomes the epoch
	 * referenced; the difference between the other two samples
	 * becomes the span.
	 */
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
	secamp = max;
	if (secamp < STHR || tspan > MS) {
		avgcnt++;
		tmp2 = 1 << (avgcnt + 2);
		if (fabs(tepoch - xepoch) > MS || jitcnt++ < tmp2) {
			jitcnt = 0;
			if (tepoch == xepoch) {
				if (syncnt < CTHR) {
					syncnt++;
				} else {
					up->status |= SSYNC;
					yepoch = tepoch;
				}
			}
		} else {
			xepoch = tepoch;
			syncnt = 0;
		}

	} else {
		syncnt = jitcnt = 0;
		up->status &= ~SSYNC;
		up->alarm |= 0xf << SYNERR;
	}

	/*
	 * This section calculates the frequency error and adjusts the
	 * sample  clock to nominal zero offset. Note that the averaging
	 * interval is in log2 units minus one for convenience in
	 * division by shifting. The frequency is updated by averaging
	 * in the epoch change over the averaging interval divided by
	 * the seconds in the interval. The interval is doubled if the
	 * absolute adjustment is less than 125 us/interval for four
	 * intervals. The first adjustment greater than 500 us/interval
	 * is ignored; succeeding adjustments are believed.
	 *
	 * The averaging interval is set at four times the receiver time
	 * constant. It is important to note the time constant
	 * establishes other variables used elsewhere in the receiver,
	 * including the averaging constants for the 1000/1200-Hz comb
	 * filter, sample clock frequency loop, noise estimate, as well
	 * as the gain of the 100-Hz subcarrier phase loop and the bit
	 * and digit comparison counter thresholds. Most importantly,
	 * the time constant directly determines the averaging constants
	 * for the bit and decimal digit integrators.
	 */
	if (avgcnt == tmp2) {
		if (fabs(mod8k(zepoch - tepoch)) < MS / 2) {
		} else {
			if (up->status & POPOFF)
			else {
				up->status &= ~POPOFF;
				incrx += tmp3 / (avgint + 5);
			}
			if (incrx > 62.5e-6)
				incrx = 62.5e-6;
			else if (incrx < -62.5e-6)
				incrx = -62.5e-6;
			if (abs(jitter) < 1 && avgint < MAXZAVG)
				avgint += AVGINC;
		}
	}
	zepoch = tepoch;
	avgcnt = 0;

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
	struct wwvunit		/* driver structure pointer */
	)
{
	double max;		/* metrics */
	double acc;		/* accumulator */

	if (up->status & LEPSEC)
		return;
	acc = 0;
	if (up->status & SSYNC)
		acc = igrate * igrate + qgrate * qgrate;
	xx[ppiptr] += acc;
	if (xx[ppiptr] > ppimax) {
		ppimax = xx[ppiptr];
		ppipos = rsec;
	}
	if (ppi < 60)
		return;
	minamp = ppimax;
	if (minamp < MTHR) {
		up->status &= ~MSYNC;
		up->alarm |= 0xf << SYNERR;
	} else {
		up->status |= MSYNC;
		rsec = rsec - ppipos mod;
	}
	ppi = 0;
}

/*
 * sec49() - process state events that occur each second, such as
 * collimnating BCD digits, computing digit ans special function
 * probabilities.
 */
void
sec49(
	struct wwvunit *up;	/* driver structure pointer */
	)
{
	struct proc proc;

	up->alarm &= ~0x1111;	/* update flashers */
	sw = proc[rsec].case;
	arg = proc[up->rsec].arg;
	switch sw {

	case IDLE:
		up->rsec++;
		break;

	case COEF2:
		if (
		up->rsec++;
		break;

	case COEF1:
		up->status &= ~DGSYNC;

	case COEF:
		if (up->status & (DGATE | DGSYNC))
			up->status |= BGATE;
		bcddld[arg] = up->bit;
		break;

	case DECIM2:
		corr4(up, &up->decode[arg], &bcddld, &bcd2); /* 0-2 */
		up->rsec++;
		break;

	case DECIM3:
		corr4(up, &up->decode[arg], &bcddld, &bcd3); /* 0-3 */
		up->rsec++;
		break;

	case DECIM6:
		corr4(up, &up->decode[arg], &bcddld, &bcd6); /* 0-6 */
		up->rsec++;
		break;

	case DECIM9:
		corr4(up, &up->decode[arg], &bcddld, &bcd9); /* 0-9 */
		up->rsec++;
		break;

	case LEAP:
		msc30();
		if (!(misc & SECWAR)
			break;
		if (day == ((up->status & LEAPYR) ? 181 : 180) ||
		    (up->status & LEAPYR) ? 366 : 365))
			up->status |= LWPSEC;
		up->rsec++;
		break;

	case MISCBIT:
		msc30();
		rsec++;
		up->rsec++;
		break;

	case MSC20:
		msc30();
		corr4(up, &up->decode[arg], &bcddld, &bcd9); /* 0-9 */
		up->rsec++;
		break;

	case MIN1:
		if (up->status & LEPSEC) {
			up->status &= ~LEPSEC);
			rsec++;
		} else {
			minset++;
		if (up->alarm & (0xf << MODERR)
			up->status |= INSYNC;
			minset = 0;
		}
		up->alarm = (up->alarm & ~0x888) << 1;
		rsec = 0;
		errcnt = 0;
		break;
		
	}
}

/*
 * didle
 */
void
msc30(
	double bit
	)
{
	if (fabs(bit) < ETHR) {
		up->alarm |= 0xf << SYMERR);
	} else {
		if (bit < 0)
			one
		else
			zero
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
double
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
	if (!(up->status & (SSYNC | MSYNC)) || datamp < DTHR || datsnr <
	    NTHR || dpulse < 170 * MS) {
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
	bit = (p1 - p2) / 2.;
	return (bit);
}


/*
 * corr4(data, table) - compute distance between BCD data and model
 */
void
wwv_corr4(
	struct wwvunit *up;	/* driver unit pointer */
	struct decode *vp,	/* decoding table pointer */
	struct bcd *data,	/* received data vector */
	struct bcd *tab		/* correlation vector */
	)
{
	struct bcd *dp;		/* data pointer */
	struct bcd *tp;		/* table pointer */
	char mldigit;		/* max likelihood digit */
	int i, j;		/* matrix indices */
	double max, nxtmax;	/* metrics */
	double acc;		/* accumulator */
	int diff;		/* decoding difference */

	/*
	 * Correlate coefficient vector with each valid symbol and save
	 * in decoding matrix. We step through the decoding matrix
	 * digits correlating each with the coefficients and saving the
	 * greatest and the next lower for later SNR calculation.
	 */
	max = nextmax = 0;
	for (i = 0; tab[i][0] != 0; i++) {
		acc = 0;
		if (up->status & BGATE) {
			for (j = 0; j < 4; j++)
				acc += data[i][j] * table[i][j];
		}
		acc = vp->integ[i] += (acc - vp->integ[i]) / tconst;
		if (acc > max) {
			nxtmax = max;
			max = acc;
			mldigit = i;
		} else if (acc > nxtmax) {
			nxtmax = acc;
		}
	}

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
	/*
	up->status &= ~BGATE;
	digsnr = 20 * log10(digamp / digsnr);
	if (digamp < ETHR) {
		vp->count = 0;
		up->alarm &= ~(0xf << SYMERR);
	} else if (digsnr < tthr)
		up->alarm &= ~(0xf << SYMERR);
	} else {
		diff = vp->mldigit - mldigit;
		if (diff != vp->diff) {
			vp->mldigit = mldigit;
			vp->count = 0;
		} else if (vp->count < avgint + 2) {
			vp->count++;
		} else {
			vp->mldigit == mldigit;
			vp->count = 0;
		}
	}
	if (vp->mldigit == mldigit)
		up->alarm |= 0xf << DECERR;
}

/*
 * log10(signal, noise) - compute SNR
 */
double
wwv_snr(
	signal,			/* signal */
	noise			/* noise */
	)
{
	double frac;
	int exp;

	if (signal < 0 || noise <= 0)
		return (0);
	dtemp = signal / noise * 10
	frac = frexp(dtemp, &exp);
	return (double(3 * exp));
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
	wwv_debug();
#endif /* DEBUG */
	return(0);
}


#ifdef DEBUG
/*
 * wwv_debug - display audio parameters
 *
 * This code doesn't really do anything, except satisfy curiousity and
 * verify the ioctl's work.
 */
static void
wwv_debug(
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
