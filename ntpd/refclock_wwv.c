/*
 * refclock_wwv - clock driver for NIST WWV/H time/frequency station
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
 * This driver synchronizes the computer time using data encoded in
 * radio transmissions from NIST time/frequency stations WWV in Boulder,
 * CO, and WWVH in Kauai, HI. Transmikssions are made continuously on
 * 2.5, 5, 10, 15 and 20 MHz in AM mode. An ordinary shortwave receiver
 * can be tuned manually to one of these frequencies or, in the case of
 * ICOM receivers, the receiver can be tuned automatically using the
 * minimuf and icom programs as propagation conditions change throughout
 * the day and night.
 *
 * The driver receives, demodulates and decodes the radio signals when
 * connected to the audio codec of a Sun workstation running SunOS or
 * Solaris, and with a little help, other workstations with similar
 * codecs or sound cards. In this implementation, only one audio driver
 * and codec can be supported on a single machine.
 *
 * The demodulation and decoding algorithms used in this driver are
 * based on those developed for the TAPR DSP93 development board and the
 * TI 320C25 digital signal processor described in: Mills, D.L. A
 * precision radio clock for WWV transmissions. Electrical Engineering
 * Report 97-8-1, University of Delaware, August 1997, 25 pp. Available
 * from www.eecis.udel.edu/~mills/reports.htm. The algorithms described
 * in this report have been modified somewhat to improve performance
 * under weak signal conditions and to provide an automatic station
 * identification feature.
 */
/*
 * Interface definitions
 */
#define	PRECISION	(-10)	/* precision assumed (about 1 ms) */
#define	REFID		"NONE"	/* reference ID */
#define	DESCRIPTION	"WWV/H Audio Demodulator/Decoder" /* WRU */
#define AUDIO_BUFSIZ	160	/* codec buffer size (Solaris only) */
#define SECOND		8000	/* second epoch (sample rate) (Hz) */
#define MINUTE		(SECOND * 60) /* minute epoch */
#define OFFSET		128	/* companded sample offset */
#define SIZE		256	/* decompanding table size */
#define	MAXSIG		6000.	/* maximum signal level reference */
#define DGAIN		20.	/* data channel gain reference */
#define SGAIN		10.	/* sync channel gain reference */
#define MAXFREQ		(125e-6 * SECOND) /* freq tolerance (.0125%) */
#define PI		3.1415926535 /* the real thing */
#define DATSIZ		(170 * MS) /* data matched filter size */
#define SYNSIZ		(800 * MS) /* minute sync matched filter size */
#define UTCYEAR		72	/* the first UTC year */
#define MAXERR		30	/* max data bit errors in minute */
#define PANIC		(4 * 1440) /* panic restart */

/*
 * Macroni
 */
#define MOD(x, y)	((x) < 0 ? -(-(x) % (y)) : (x) % (y))

/*
 * General purpose status bits (status)
 *
 * Notes: WWV and/or WWVH are set when the minute sync pulse from either
 * or both stations has been acquired. SSYNC or MSYNC is set when the
 * second or minute sync pulse has been acquired, respectively. These
 * bits are cleared and the SYNERR alarm raised when the associated
 * pulse amplitude decays below STHR and ITHR, respectively. DGATE is
 * set if a data bit is invalid, BGATE is set if a BCD digit bit is
 * invalid. DSYNC is set when the minutes unit digit has reached the
 * threshold and INSYNC is set when if all nine digits have reached the
 * threshold.
 *
 * The data error counter is incremented for each invalid data bit. If
 * too many data bit errors are encountered in one minute, the MODERR
 * alarm is raised. The DECERR alarm is raised if a maximum likelihood
 * digit fails to compare with the current clock digit. If the
 * probability of any miscellaneous bit or any digit falls below ETHR,
 * the SYMERR alarm is raised.
 *
 * LEPSEC is set when the SECWAR of the timecode is set on the last
 * second of 30 June or 31 December. At the end of this minute both the
 * receiver and transmitter insert second 60 in the minute and the
 * minute sync slips a second.
 */
#define MSYNC		0x0001	/* minute epoch sync */
#define SSYNC		0x0002	/* second epoch sync */
#define DSYNC		0x0004	/* minute units sync */
#define INSYNC		0x0008	/* clock synchronized */
#define DGATE		0x0010	/* data bit error */
#define BGATE		0x0020	/* BCD digit bit error */
#define WWV		0x0100	/* WWV station detected */
#define SELV		0x0200	/* WWV station selected */
#define WWVH		0x0400	/* WWVH station detected */
#define SELH		0x0800	/* WWVH station selected */
#define LEPSEC		0x1000	/* leap second in progress */

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
#define SYNERR		12	/* not synchronized to station */

/*
 * Tone frequency definitions.
 */
#define MS		8	/* samples per millisecond */
#define IN100		1	/* 100 Hz 4.5-deg sin table */
#define IN1000		10	/* 1000 Hz 4.5-deg sin table */
#define IN1200		12	/* 1200 Hz 4.5-deg sin table */

/*
 * Acquisition and tracking time constants. Usually powers of 2.
 */
#define MINAVG		8	/* min time constant (s) */
#define MAXAVG		7	/* max time constant (log2 s) */
#define TCONST		16	/* minute time constant (s) */
#define SYNCTC		(1024 / (1 << MAXAVG)) /* FLL constant (s) */

/*
 * Thresholds. These establish the minimum signal level, minimum SNR and
 * maximum jitter thresholds which establish the error and false alarm
 * rates of the receiver. The values defined here may be on the
 * adventurous side in the interest of the highest sensitivity.
 */
#define ATHR		2000	/* acquisition amplitude threshold */
#define ASNR		6.0	/* acquisition SNR threshold (dB) */
#define AWND		20	/* acquisition window threshold (ms) */
#define ACMP		5	/* acquisition compare threshold */
#define STHR		500	/* second sync amplitude threshold */
#define SCMP		10 	/* second sync compare threshold */
#define DTHR		1000	/* bit amplitude threshold */
#define DSNR		10.0	/* bit SNR threshold (dB) */
#define BTHR		1000	/* digit probability threshold */
#define BSNR		3.0	/* digit likelihood threshold (dB) */
#define BCMP		5	/* digit compare threshold (dB) */

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
 * The total system delay with the DSP93 program was is at 22.5 ms,
 * including the propagation delay from Ft. Collins, CO, to Newark, DE
 * (8.9 ms), the communications receiver delay and the delay of the
 * DSP93 program itself. The DSP93 program delay is due mainly to the
 * 400-Hz FIR bandpass filter (5 ms) and second sync matched filter (5
 * ms), leaving about 3.6 ms for the receiver delay and strays.
 *
 * The total system delay with this program is estimated at 27.1 ms by
 * comparison with another PPS-synchronized NTP server over a 10-Mb/s
 * Ethernet. The propagation and receiver delays are the same as with
 * the DSP93 program. The program delay is due only to the 600-Hz
 * IIR bandpass filter (1.1 ms), since other delays have been removed.
 * Assuming 4.7 ms for the receiver, program and strays, this leaves
 * 13.5 ms for the audio codec and operating system latencies for a
 * total of 18.2 ms. as the systematic delay. The additional propagation
 * delay specific to each receiver location can be programmed in the
 * fudge time1 and time2 values for WWV and WWVH, respectively.
 */
#define PDELAY	(.0036 + .0011 + .0135)	/* net system delay (s) */

/*
 * Table of sine values at 4.5-degree increments. This is used by the
 * synchronous matched filter demodulators. The integral of sine-squared
 * over one complete cycle is PI, so the table is normallized by 1 / PI.
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
 * Decoder operations at the end of each second are driven by a state
 * machine. The transition matrix consists of a dispatch table indexed
 * by second number. Each entry in the table contains a case switch
 * number and argument.
 */
struct progx {
	int sw;			/* case switch number */
	int arg;		/* argument */
};

/*
 * Case switch numbers
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
	{COEF,	0},		/* 15 10 minute tens */
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
 * DST decode (DST2 DST1) for prettyprint
 */
char dstcod[] = {
	'S',			/* 00 standard time */
	'I',			/* 01 daylight warning */
	'O',			/* 10 standard warning */
	'D'			/* 11 daylight time */
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
	double digprb;		/* max digit probability */
	double digsnr;		/* likelihood function (dB) */
	double like[10];	/* likelihood integrator 0-9 */
};

/*
 * The station structure is used to acquire the minute pulse from WWV
 * and/or WWVH. These stations are distinguished by the frequency used
 * for the second and minute sync pulses, 1000 Hz for WWV and 1200 Hz
 * for WWVH. Other than frequency, the format is the same.
 */
struct sync {
	double	ibuf[SYNSIZ];	/* I channel delay line */
	double	qbuf[SYNSIZ];	/* Q channel delay line */
	double	iamp;		/* I channel amplitude */
	double	qamp;		/* Q channel amplitude */
	double	noise;		/* max amplitude off pulse */
	double	max;		/* max amplitude on pulse */
	double	lastmax;	/* last max amplitude on pulse */
	long	pos;		/* position at maximum amplitude */
	long	lastpos;	/* last position at maximum amplitude */
	long	mepoch;		/* minute synch epoch */
	int	count;		/* compare counter */
	int	sinptr;		/* sine table pointer */
	int	incr;		/* sine table increment */
	char	call[5];	/* station callsign */
	int	ident;		/* station ident bit */
	int	pdelay;		/* propagation delay (samples) */
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
	char	call[5];	/* station callsign */

	/*
	 * Variables used to establish basic system timing
	 */
	int	avgint;		/* log2 master time constant (s) */
	int	epoch;		/* second epoch ramp */
	int	repoch;		/* receiver sync epoch */
	int	yepoch;		/* transmitter sync epoch */
	double	epomax;		/* second sync amplitude */
	double	irig;		/* data I channel amplitude */
	double	qrig;		/* data Q channel amplitude */
	int	datapt;		/* 100 Hz ramp */
	double	datpha;		/* 100 Hz VFO control */
	int	rphase;		/* receiver sample counter */
	int	rsec;		/* receiver seconds counter */
	long	mphase;		/* minute sample counter */
	long	nepoch;		/* minute epoch index */
	int	count;		/* minute sync compare counter */
	int	jptr;		/* minute sync pulse counter */
	struct sync wwv;	/* WWV sync channel */
	struct sync wwvh; 	/* WWVH sync channel */

	/*
	 * Variables used by the clock state machine
	 */
	struct decvec decvec[9]; /* decoding matrix */
	int	pdelay;		/* propagation delay */
	int	tphase;		/* transmitter sample counter */
	int	tsec;		/* transmitter seconds counter */
	int	digcnt;		/* count of digits synchronized */

	/*
	 * Variables used to estimate signal levels and bit/digit
	 * probabilities
	 */
	double	datamp;		/* max data bit amplitude */
	double	noiamp;		/* average noise amplitude */
	double	datsnr;		/* data bit SNR (dB) */

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
static	void	wwv_rf		P((struct peer *, double));
static	void	wwv_endpoc	P((struct peer *, double, int));
static	double	wwv_rsec	P((struct peer *, double));
static	void	wwv_qrz		P((struct peer *, struct sync *,
				    double));
static	void	wwv_corr4	P((struct peer *, struct decvec *,
				    double [], double [][]));
static	void	wwv_gain	P((struct peer *));
static	void	wwv_tsec	P((struct wwvunit *));
static	double	wwv_data	P((struct wwvunit *, double));
static	int	timecode	P((struct wwvunit *, char *));
static	int	wwv_audio	P((void));
static	void	wwv_show	P((void));
static	double	wwv_snr		P((double, double));
static	int	carry		P((struct decvec *));

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
	DTOLFP(1. / SECOND, &up->tick);
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
	up->decvec[MN].radix = 10;	/* minutes */
	up->decvec[MN + 1].radix = 6;
	up->decvec[HR].radix = 10;	/* hours */
	up->decvec[HR + 1].radix = 3;
	up->decvec[DA].radix = 10;	/* days */
	up->decvec[DA + 1].radix = 10;
	up->decvec[DA + 2].radix = 4;
	up->decvec[YR].radix = 10;	/* years */
	up->decvec[YR + 1].radix = 10;
	up->wwv.incr = IN1000;
	up->wwv.ident = WWV | SELV;
	strcpy(up->wwv.call, "WWV ");
	up->wwvh.incr = IN1200;
	up->wwvh.ident = WWVH | SELH;
	strcpy(up->wwvh.call, "WWVH");
	strcpy(up->call, "NONE");
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
	DTOLFP((double)up->bufcnt / SECOND, &ltemp);
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
		up->phase += up->freq / SECOND;
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
		 * Once each second adjust the codec port and gain.
		 * While at it, initialize the propagation delay for
		 * both WWV and WWVH. Don't forget to correct for the
		 * receiver phase delay, mostly due to the 600-Hz
		 * IIR bandpass filter used for the sync signals.
		 */
		up->wwv.pdelay = (int)(SECOND * (pp->fudgetime1 +
		    PDELAY));
		up->wwvh.pdelay = (int)(SECOND * (pp->fudgetime2 +
		    PDELAY));
		up->seccnt = (up->seccnt + 1) % SECOND;
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
		 * to codec format.
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

	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;
	if (pp->coderecv == pp->codeproc)
		up->errflg = CEVNT_TIMEOUT;
	else
		pp->polls++;
	if (up->status & INSYNC) {
		if (up->minset > PANIC)
			up->status = up->alarm = 0;
		else
			peer->reach |= 1;
	}
	if (up->errflg)
		refclock_report(peer, up->errflg);
	up->errflg = 0;
}


/*
 * wwv_epoch() main loop
 *
 * This routine establishes receiver and transmitter epoch
 * synchronization and determines the data subcarrier pulse length.
 * Receiver synchronization is determined by the minute sync pulse
 * detected in the wwv_rf() routine and the second sync pulse detected
 * in the wwv_epoch() routine. This establishes when to sample the data
 * subcarrier in-phase signal for the maximum level and noise level and
 * when to determine the pulse length. The transmitter second leads the
 * receiver second by the propagation delay, receiver delay and filter
 * delay of this program. It establishes the clock time and implements
 * the sometimes idiosyncratic conventional clock time and civil
 * calendar. 
 *
 * Most communications radios use a highpass filter in the audio stages,
 * which can do nasty things to the subcarrier phase relative to the
 * sync pulses. Therefore, the data subcarrier reference phase is
 * disciplined using the hardlimited quadrature-phase signal sampled at
 * the same time as the in-phase signal. The phase tracking loop uses
 * phase adjustments of plus-minus one sample (125 us).
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
	l_fp offset;		/* NTP format offset */
	char tbuf[80];		/* monitor buffer */
	double dtemp, etemp;
	int temp;

	/*
	 * Extract the data and sync signals.
	 */
	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;
	wwv_rf(peer, sig);

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
		if (up->datpha >= 0) {
			up->datapt++;
			if (up->datapt >= 80)
				up->datapt -= 80;
		} else {
			up->datapt--;
			if (up->datapt < 0)
				up->datapt += 80;
		}

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
	 * machine. Note we have to be careful to set the transmitter
	 * epoch at the same time as the receiver epoch to be sure the
	 * right propagation delay is used. We don't bother the heavy
	 * machinery unless the clock is set.
	 */
	up->tphase++;
	if (up->epoch == up->yepoch) {
		wwv_tsec(up);
		up->tphase = 0;

		/*
		 * Determine the current offset from the time of century
		 * and the sample timestamp, but only if the clock has
		 * been set and the second sync is tracking.
		 */
		if (up->status & INSYNC && up->status & SSYNC) {
			pp->second = up->tsec;
			pp->minute = up->decvec[MN].digit +
			    up->decvec[MN + 1].digit * 10;
			pp->hour = up->decvec[HR].digit +
			    up->decvec[HR + 1].digit * 10;
			pp->day = up->decvec[DA].digit + up->decvec[DA +
			    1].digit * 10 + up->decvec[DA + 2].digit *
			    100;
			pp->year = up->decvec[YR].digit +
			    up->decvec[YR + 1].digit * 10;
			if (pp->year < UTCYEAR)
				pp->year += 2000;
			else
				pp->year += 1900;
			pp->lastrec = up->timestamp;
			L_CLR(&offset);
			if (!clocktime(pp->day, pp->hour, pp->minute,
			    pp->second, GMT, pp->lastrec.l_ui,
			    &pp->yearstart, &offset.l_ui))
				up->errflg = CEVNT_BADTIME;
			else
				refclock_process_offset(pp, offset,
				    pp->lastrec, 0.);
			if (up->misc & SECWAR)
				pp->leap = LEAP_ADDSECOND;
			else
				pp->leap = LEAP_NOWARNING;
			if (!refclock_process(pp))
				up->errflg = CEVNT_BADTIME;
		} else {
			pp->leap = LEAP_NOTINSYNC;
		}
	}

	/*
	 * At the end of the receiver second, process the data bit and
	 * update the decoding matrix probabilities.
	 */
	up->rphase++;
	if (up->epoch == up->repoch) {
		temp = up->rsec;
		dtemp = wwv_data(up, dpulse);
		etemp = wwv_rsec(peer, dtemp);
		if (up->status & MSYNC && !(up->status & DSYNC)) {
			sprintf(tbuf,
			    "wwv3 %2d %04x %5.0f %04x %5.0f %5.0f %5.1f %5.0f %5.0f",
			    up->rsec, up->status, up->epomax, up->alarm,
			    up->datamp, up->datpha, up->datsnr, dtemp,
			    etemp);
			if (pp->sloppyclockflag & CLK_FLAG4)
				record_clock_stats(&peer->srcadr, tbuf);
#ifdef DEBUG
			if (debug)
				printf("%s\n", tbuf);
#endif /* DEBUG */
		}
		up->rphase = dpulse = 0;
	}
}


/*
 * wwv_rf() - process signals and demodulate to baseband
 *
 * This routine grooms and filters decompanded raw audio samples. The
 * output signals include the 100-Hz baseband data signal in quadrature
 * form, plus the epoch index of the second sync signal and the second
 * index of the minute sync signal.
 *
 * There are three 1-s ramps used by this program, all spanning the
 * range 0-7999 logical samples for exactly one second, as determined by
 * the logical clock. The first drives the second epoch and runs
 * continuously. The second determines the receiver phase and the third
 * the transmitter phase within the second. The receiver second begins
 * upon arrival of the 5-ms second sync pulse which begins the second;
 * while the transmitter second begins before it by the specified
 * propagation delay.
 *
 * There are three 1-m ramps spanning the range 0-59 seconds. The first
 * drives the minute epoch in samples and runs continuously. The second
 * determines the receiver second and the third the transmitter second.
 * The receiver second begins upon arrival of the 800-ms sync pulse sent
 * during the first second of the minute; while the transmitter second
 * begins before it by the specified propagation delay.
 *
 * The output signals include the epoch maximum and phase and second
 * maximum and index. The epoch phase provides the master reference for
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
	struct peer *peer,	/* peerstructure pointer */
	double isig		/* input signal */
	)
{
	struct refclockproc *pp;
	struct wwvunit *up;

	static double lpf[5];	/* 150-Hz lpf delay line */
	double data;		/* lpf output */
	static double bpf[9];	/* 1000/1200-Hz bpf delay line */
	double syncx;		/* bpf output */
	static double mf[41];	/* 1000/1200-Hz mf delay line */
	double mfsync = 0;	/* mf output */

	static double ibuf[DATSIZ]; /* data I channel delay line */
	static double qbuf[DATSIZ]; /* data Q channel delay line */
	static int iptr;	/* data channels pointer */

	static double epobuf[SECOND]; /* epoch sync comb filter */
	static double epomax;	/* epoch sync amplitude buffer */
	static int epopos;	/* epoch sync position buffer */

	static int iniflg;	/* initialization flag */
	struct sync *sp;
	double dtemp;
	int i;

	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;
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
	 * The I and Q quadrature data signals are produced by
	 * multiplying the filtered signal by 100-Hz sine and cosine
	 * signals, respectively. The data signals are demodulated by
	 * 170-ms synchronous matched filters to produce the amplitude
	 * and phase signals used by the decoder.
	 */
	i = up->datapt;			/* data I channel */
	up->datapt = (up->datapt + IN100) % 80;
	dtemp = sintab[i] * data / DATSIZ * DGAIN;
	up->irig -= ibuf[iptr];
	ibuf[iptr] = dtemp;
	up->irig += dtemp;
	i = (i + 20) % 80;		/* data Q channel */
	dtemp = sintab[i] * data / DATSIZ * DGAIN;
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
	 * The I and Q quadrature minute sync signals are produced by
	 * multiplying the filtered signal by 1000-Hz (WWV) and 1200-Hz
	 * (WWVH) sine and cosine signals, respectively. The resulting
	 * signals are demodulated by 800-ms synchronous matched filters
	 * to synchronize the second and minute and to detect which one
	 * (or both) the WWV or WWVH signal is present.
	 */
	wwv_qrz(peer, &up->wwv, syncx);
	wwv_qrz(peer, &up->wwvh, syncx);
	up->jptr = (up->jptr + 1) % SYNSIZ;
	if (up->mphase == 0) {

		/*
		 * The program listens for minute sync pulses from both
		 * WWV and WWVH. The station with the greater compare
		 * count is selected, with ties broken by WWV, but only
		 * if the count is at least two. Once a station has been
		 * selected, second sync pulses only from that station
		 * will be used.
		 *
		 * The leap second is ugly. Just skip it and set the
		 * minute epoch back one second.
		 */
		if (up->rsec == 60) {
			up->mphase -= SECOND;
			if (up->mphase < 0)
				up->mphase += MINUTE;
		} else {
			if (up->wwv.count >= up->wwvh.count)
				sp = &up->wwv;
			else
				sp = &up->wwvh;
			up->count = sp->count;
			if (up->count >= 2) {
				up->pdelay = sp->pdelay;
				strcpy(up->call, sp->call);
				memcpy((char *)&pp->refid, up->call, 4);
				memcpy((char *)&peer->refid, up->call,
				    4);
				up->status |= MSYNC | (sp->ident &
				    (SELV | SELH));
				if (!(up->status & INSYNC)) {
					up->rsec = (MINUTE -
					    sp->mepoch) / SECOND;
					if (!(up->status & SSYNC)) {
						up->repoch =
						    sp->mepoch % SECOND;
						up->yepoch =
						    up->repoch -
						    up->pdelay;
						if (up->yepoch < 0)
							up->yepoch +=
							    SECOND;
					}
				}
			}
			if (up->count == 0) {
				up->status &= ~MSYNC;
				up->alarm |= 1 << SYNERR;
			}
		}
	}
	up->mphase = (up->mphase + 1) % MINUTE;

	/*
	 * The second sync pulse is extracted using 5-ms FIR matched
	 * filters at 1000 Hz for WWV or 1200 Hz for WWVH. This pulse is
	 * used for the most precise synchronization, since if provides
	 * a resolution of one sample (125 us).
	 */
	if (up->status & SELV) {

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
	} else if (up->status & SELH) {

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
	if (i == 0) {
		wwv_endpoc(peer, epomax, epopos);
		up->epomax = epomax;
		epomax = 0;
	}
	dtemp = (epobuf[i] += (mfsync - epobuf[i]) / (MINAVG <<
	    up->avgint));
	if (dtemp > epomax) {
		epomax = dtemp;
		epopos = i - 5 * MS;
		if (epopos < 0)
			epopos += SECOND;
	}
	up->epoch = (i + 1) % SECOND;
}


/*
 * wwv_qrz() - identify and acquire WWV/WWVH minute sync pulse
 *
 * This routine searches through the entire epoch minute in samples
 * using a filter matched to the 800-ms pulse sent at the beginning of
 * the minute. It then saves the maximum and epoch index of the pulse
 * and compares with subsequent samples. A pulse discriminator is used
 * to groom the samples. For acquisition, it requires that (a) the peak
 * on-pulse sample amplitude must be above a threshold, (b) the SNR
 * relative to the peak off-pulse sample amplitude must be 6 dB or more
 * below the peak and (c) the maximum difference between the current and
 * previous epoch indices must be less than 20 ms.
 *
 * Students of radar receiver technology will discover this algorithm
 * amounts to a range gate discriminator. In practice, the performance
 * of this gadget is amazing. Once setting teeth in a station, it hangs
 * on until the minute beep can barely be heard and long after the
 * second tick and comb filter have given up. 
 */
static void
wwv_qrz(
	struct peer *peer,	/* peerstructure pointer */
	struct sync *sp,	/* sync channel structure */
	double syncx		/* bandpass filtered sync signal */
)
{
	struct refclockproc *pp;
	struct wwvunit *up;
	char tbuf[80];		/* monitor buffer */
	double snr;		/* on-pulse/off-pulse ratio (dB) */
	double dtemp;
	long jitter, epoch;
	int i;

	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;

	/*
	 * Grind the sync pulses through a 800-ms synchronous matched
	 * filter tuned to the sync frequency, 1000 Hz for WWV or 1200
	 * Hz for WWVH. As the local oscillator is not good enough to
	 * synchronously correlate from minute to minute, use only the
	 * phasor amplitude and ignore the phase.
	 */
	i = sp->sinptr;
	sp->sinptr = (sp->sinptr + sp->incr) % 80;
	dtemp = sintab[i] * syncx / SYNSIZ * SGAIN;
	sp->iamp = sp->iamp - sp->ibuf[up->jptr] + dtemp;
	sp->ibuf[up->jptr] = dtemp;
	i = (i + 20) % 80;
	dtemp = sintab[i] * syncx / SYNSIZ * SGAIN;
	sp->qamp = sp->qamp - sp->qbuf[up->jptr] + dtemp;
	sp->qbuf[up->jptr] = dtemp;
	dtemp = sp->iamp * sp->iamp + sp->qamp * sp->qamp;

	/*
	 * Find the sample with peak energy within the window, which
	 * defines the minute epoch. If the clock has been set, center
	 * the window on the current minute epoch and search only the
	 * window for the peak. Otherwise, center the window on the last
	 * minute epoch found. In the latter case, if the station has
	 * been found, search only the window for the peak; otherwise,
	 * search the entire minute. In all cases find the peak energy
	 * outside the window for use as a noise metric.
	 */
	if (up->status & INSYNC)
		epoch = (up->nepoch + SYNSIZ) % MINUTE;
	else
		epoch = sp->lastpos;
	if (abs(MOD(up->mphase - epoch, MINUTE)) < SYNSIZ) {
		if (dtemp > sp->max) {
			sp->max = dtemp;
			sp->pos = up->mphase;
		}
	} else {
		if (!(up->status & INSYNC) && !(sp->ident &
		    up->status) && dtemp > sp->max) {
			sp->max = dtemp;
			sp->pos = up->mphase;
		}
		if (dtemp > sp->noise)
			sp->noise = dtemp;
	}
	if (up->mphase == 0) {

		/*
		 * At the end of the minute epoch, determine the epoch
		 * at the beginning of the sync pulse, as well as the
		 * difference between the current and previous epoch
		 * (jitter).
		 */
		jitter = MOD(sp->pos - sp->lastpos, MINUTE);
		sp->max = sqrt(sp->max);
		sp->noise = sqrt(sp->noise);
		snr = wwv_snr(sp->max, sp->noise);
		switch (sp->count) {

		/*
		 * In state 0 the station has not been heard for some
		 * time and the station identifier bit is cleared. Look
		 * for the biggest blip greater than the amplitude
		 * threshold in the minute and assume that the minute
		 * sync pulse. If found, bump to state 1.
		 */
		case 0:
			if (sp->max >= ATHR)
				sp->count++;
			break;

		/*
		 * In state 1 a candidate blip has been found and the
		 * next minute is being searched for other blips. If
		 * none is found greater than the threshold, or if the
		 * biggest blip outside the candidate pulse is less than
		 * 6 dB below the biggest blip, drop back to state 0 and
		 * hunt some more. Otherwise, a legitimate minute pulse
		 * has been found, so bump to state 2.
		 */
		case 1:
			if (sp->max < ATHR) {
				sp->count--;
				up->status &= ~sp->ident;
				break;
			} else if (snr < ASNR || abs(jitter) > AWND *
			    MS) {
				break;
			}
			up->status |= sp->ident & (WWV | WWVH);
			/* fall through */

		/*
		 * In states 2 and above the blip is confirmed
		 * legitimate and the station identifier bit is set.
		 * Continue to groom samples as before and drop back to
		 * the previous state if the groom fails. If it
		 * succeeds, bump to the next state. If the bump reaches
		 * the threshold, the minute epoch has been reliably
		 * determined.
		 */
		default:
			if (sp->max < ATHR || snr < ASNR || abs(jitter)
			    > AWND * MS) {
				sp->count--;
				break;
			}
			sp->mepoch = sp->pos - SYNSIZ;
			if (sp->mepoch < 0)
				sp->mepoch += MINUTE;
			if (sp->count < ACMP)
				sp->count++;
			break;
		}
		sprintf(tbuf,
		    "wwv8 %2d %04x %5.0f %s %d %5.0f %5.1f %7ld %7ld %7ld",
		    up->rsec, up->status, up->epomax, sp->call,
		    sp->count, sp->max, snr, sp->pos, jitter,
		    MOD(sp->pos - up->nepoch - SYNSIZ, MINUTE));
		if (pp->sloppyclockflag & CLK_FLAG4)
			record_clock_stats(&peer->srcadr, tbuf);
#ifdef DEBUG
		if (debug)
			printf("%s\n", tbuf);
#endif
		sp->lastmax = sp->max;
		sp->lastpos = sp->pos;
		sp->max = sp->noise = 0;
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
 * over all 8000 samples in the second comb filter. To assure accurate
 * and reliable time and frequency discipline, this routine performs a
 * great deal of heavy-handed data filtering and grooming.
 */
static void
wwv_endpoc(
	struct peer *peer,	/* peer structure pointer */
	double epomax,		/* epoch max */
	int epopos		/* epoch max position */
	)
{
	struct refclockproc *pp;
	struct wwvunit *up;

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
	char tbuf[80];		/* monitor buffer */
	double dtemp;
	int tmp2, tmp3;

	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;
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
	 * The candidate epoch is discarded and the SYNERR alarm raised 	 * if the pulse amplitude is less than the decision threshold.
	 * If the epoch candidate is within 1 ms of the last one, the
	 * new candidate replaces the last one and the jitter counter is
	 * reset; otherwise, the candidate is ignored and the jitter
	 * counter is incremented. If the jitter counter exceeds the
	 * frequency averaging interval, the new candidate replaces the
	 * old one anyway. The compare counter is incremented if the new
	 * candidate is identical to the last one; otherwise, it is
	 * forced to zero. If the compare counter increments to 10, the
	 * epoch is reset and the receiver second epoch is set.
	 */
	tmp2 = MOD(tepoch - xepoch, SECOND);
	if (epomax < STHR) {
		up->status &= ~SSYNC;
		up->alarm |= 1 << SYNERR;
		jitcnt = syncnt = avgcnt = 0;
	} else if (abs(tmp2) <= MS || jitcnt >= (MINAVG << up->avgint))
	    {
		jitcnt = 0;
		if (tmp2 != 0) {
			xepoch = tepoch;
			syncnt = 0;
		} else {
			if (syncnt < SCMP) {
				syncnt++;
			} else {
				up->status |= SSYNC;
				up->repoch = tepoch;
				up->yepoch = up->repoch - up->pdelay;
				if (up->yepoch < 0)
					up->yepoch += SECOND;
			}
		}
		avgcnt++;
	} else {
		jitcnt++;
		syncnt = avgcnt = 0;
	}
	if (!(up->status & SSYNC) && 0) {
		sprintf(tbuf,
		    "wwv1 %2d %04x %5.0f %2d %5.0f %5d %5d %5d %2d %4d",
		    up->rsec, up->status, up->epomax, avgcnt, epomax,
		    tepoch, tspan, tmp2, syncnt, jitcnt);
		if (pp->sloppyclockflag & CLK_FLAG4)
			record_clock_stats(&peer->srcadr, tbuf);
#ifdef DEBUG
		if (debug)
			printf("%s\n", tbuf);
#endif /* DEBUG */
	}

	/*
	 * The sample clock frequency is disciplined using a first-order
	 * feedback loop with time constant consistent with the Allan
	 * intercept of typical computer clocks. The loop update is
	 * calculated each averaging interval from the epoch change in
	 * 125-us units and interval length in seconds. The interval is
	 * doubled after four intervals where epoch change is not more
	 * than one sample.
	 *
	 * The averaging interval affects other receiver functions,
	 * including the the 1000/1200-Hz comb filter and sample clock
	 * loop. It also affects the 100-Hz subcarrier loop and the bit
	 * and digit comparison counter thresholds.
	 */
	tmp3 = MOD(tepoch - zepoch, SECOND);
	if (avgcnt >= (MINAVG << up->avgint)) {
		if (abs(tmp3) < MS) {
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
			if (up->avgint < MAXAVG) {
				sprintf(tbuf,
				    "wwv2 %2d %04x %5.0f %5d %5d %2d %2d %6.1f %6.1f",
				    up->rsec, up->status, up->epomax,
				    MINAVG << up->avgint, avgcnt,
				    avginc, tmp3, dtemp / SECOND * 1e6,
				    up->freq / SECOND * 1e6);
				if (pp->sloppyclockflag & CLK_FLAG4)
					record_clock_stats(
					    &peer->srcadr, tbuf);
#ifdef DEBUG
				if (debug)
					printf("%s\n", tbuf);
#endif /* DEBUG */
			}
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
	struct peer *peer,	/* peer structure pointer */
	double bit		/* bit probability */
	)
{
	static int iniflg;	/* initialization flag */
	static double bcddld[4]; /* BCD data bits */
	static double bitvec[61]; /* bit integrator for misc bits */
	struct refclockproc *pp;
	struct wwvunit *up;
	double rval;		/* likelihood value */
	int sw, arg, nsec;

	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;
	if (!iniflg) {
		iniflg = 1;
		memset((char *)bitvec, 0, sizeof(bitvec));
	}

	/*
	 * The bit represents the probability of a hit on zero (negative
	 * values), a hit on one (positive values) or a miss (zero
	 * value). The likelihood vector is the exponential average of
	 * these probabilities. Only the bits of this vector
	 * corresponding to the miscellaneous bits of the timecode are
	 * used, but it's easier to do them all. After that, crank the
	 * seconds state machine.
	 */
	nsec = up->rsec + 1;
	bitvec[up->rsec] += (bit - bitvec[up->rsec]) / TCONST;
	rval = bitvec[up->rsec];
	sw = progx[up->rsec].sw;
	arg = progx[up->rsec].arg;
	switch (sw) {

	/*
	 * Ignore this second.
	 */
	case IDLE:
		break;

	/*
	 * Save the bit probability in the BCD data vector at the index
	 * given by the argument. Note that all bits of the vector have
	 * to be above the data gate threshold for the digit to be
	 * considered valid. Bits not used in the digit are forced to
	 * zero and not checked for errors.
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
		wwv_corr4(peer, &up->decvec[arg], bcddld, bcd2);
		break;

	case DECIM3:
		wwv_corr4(peer, &up->decvec[arg], bcddld, bcd3);
		break;

	case DECIM6:
		wwv_corr4(peer, &up->decvec[arg], bcddld, bcd6);
		break;

	case DECIM9:
		wwv_corr4(peer, &up->decvec[arg], bcddld, bcd9);
		break;

	/*
	 * Miscellaneous bits. If above the positive threshold, declare
	 * 1; if below the negative threshold, declare 0; otherwise
	 * raise the SYMERR alarm.
	 */
	case MSC20:
		wwv_corr4(peer, &up->decvec[YR + 1], bcddld, bcd9);
		/* fall through */

	case MSCBIT:
		if (bitvec[up->rsec] > BTHR)
			up->misc |= arg;
		else if (bitvec[up->rsec] < -BTHR)
			up->misc &= ~arg;
		else
			up->alarm |= 1 << SYMERR;
		break;

	/*
	 * The endgame. If all nine clock digits are valid and
	 * synchronized to the second, the clock is set set and forever
	 * keeps the tick. If at least one digit is set, which by design
	 * must be the minute units digit, the clock state machine
	 * begins to count the minutes. If the SECWARN bit is set on the
	 * last minute of 30 June or 31 December, the LEPSEC bit is set.
	 * At the end of the minute in which LEPSEC is set the
	 * transmitter and receiver insert an extra second (60) in the
	 * timescale and the minute sync skips a second. We only get to
	 * test this wrinkle at intervals of about 18 months, so your
	 * mileage may vary.
	 */
	case MIN1:
		if (up->tsec == 60) {
			up->status &= ~LEPSEC;
			break;
		}
		/* fall through */

	case MIN2:
		up->minset = ((current_time - peer->update) + 30) / 60;
		if (up->digcnt > 0)
			up->status |= DSYNC;
		if (up->digcnt >= 9 && up->status & SSYNC) {
			up->status |= INSYNC;
			pp->lencode = timecode(up, pp->a_lastcode);
			record_clock_stats(&peer->srcadr,
			    pp->a_lastcode);
			refclock_receive(peer);
		} else {
			pp->lencode = timecode(up, pp->a_lastcode);
			if (pp->sloppyclockflag & CLK_FLAG4)
				record_clock_stats(&peer->srcadr,
				    pp->a_lastcode);
		}
#ifdef DEBUG
		if (debug)
			printf("wwv: timecode %d %s\n", pp->lencode,
			    pp->a_lastcode);
#endif /* DEBUG */
		up->alarm = (up->alarm & ~0x8888) << 1;
		up->errcnt = up->digcnt = nsec = 0;
		up->nepoch = up->mphase;
		break;
	}
	up->rsec = up->tsec = nsec;
	return (rval);
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
	if (!(up->status & MSYNC) || up->datamp < DTHR ||
	    up->datsnr < DSNR || dpulse < DATSIZ) {
		up->status |= DGATE;
		up->errcnt++;
		if (up->errcnt > MAXERR)
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
 * lower value determines the likelihood function. Note that, if the
 * digit is invalid, the likelihood vector is averaged toward a miss.
 */
static void
wwv_corr4(
	struct peer *peer,	/* peer unit pointer */
	struct decvec *vp,	/* decoding table pointer */
	double data[],		/* received data vector */
	double tab[][4]		/* correlation vector array */
	)
{
	struct refclockproc *pp;
	struct wwvunit *up;

	double max, nxtmax;	/* metrics */
	double acc;		/* accumulator */
	char tbuf[80];		/* monitor buffer */
	int mldigit;		/* max likelihood digit */
	int diff;		/* decoding difference */
	int i, j;

	pp = peer->procptr;
	up = (struct wwvunit *)pp->unitptr;

	/*
	 * Correlate digit vector with each BCD coefficient vector. If
	 * any BCD digit bit is bad, consider all bits a miss.
	 */
	mldigit = 0;
	max = nxtmax = -MAXSIG;
	for (i = 0; tab[i][0] != 0; i++) {
		acc = 0;
		for (j = 0; j < 4; j++) {
			if (!(up->status & BGATE))
				acc += data[j] * tab[i][j];
		}
		acc = (vp->like[i] += (acc - vp->like[i]) / TCONST);
		if (acc > max) {
			nxtmax = max;
			max = acc;
			mldigit = i;
		} else if (acc > nxtmax) {
			nxtmax = acc;
		}
	}
	vp->mldigit = mldigit;
	vp->digprb = max;
	vp->digsnr = wwv_snr(max, nxtmax);

	/*
	 * The maximum likelihood digit is compared with the current
	 * clock digit. The difference represents the decoding phase
	 * error. If the digit probability and likelihood are good and
	 * the difference stays the same for a number of comparisons,
	 * the clock digit is reset to the maximum likelihood digit.
	 */
	diff = mldigit - vp->digit;
	if (diff < 0)
		diff += vp->radix;
	if (diff != vp->phase) {
		vp->phase = diff;
		vp->count = 0;
	}
	if (vp->digprb < BTHR || vp->digsnr < BSNR || !(up->status &
	    MSYNC)) {
		vp->count = 0;
		up->alarm |= 1 << SYMERR;
	} else if (vp->count < BCMP) {
		if (!(up->status & INSYNC)) {
			vp->phase = 0;
			vp->digit = mldigit;
		}
		vp->count++;
	} else {
		vp->phase = 0;
		vp->digit = mldigit;
		up->digcnt++;
	}
	if (vp->digit != mldigit)
		up->alarm |= 1 << DECERR;
	if (up->status & MSYNC) {
		sprintf(tbuf,
		    "wwv4 %2d %04x %5.0f %2d %d %d %d %d %5.0f %5.1f",
		    up->rsec, up->status, up->epomax,  vp->radix,
		    vp->digit, vp->mldigit, vp->phase, vp->count,
		    vp->digprb, vp->digsnr);
		if (pp->sloppyclockflag & CLK_FLAG4)
			record_clock_stats(&peer->srcadr, tbuf);
#ifdef DEBUG
	if (debug)
		printf("%s\n", tbuf);
#endif /* DEBUG */
	}
	up->status &= ~BGATE;
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
	int minute, day, isleap;
	int temp;

	up->tsec++;
	if (up->tsec < 60 || up->status & LEPSEC)
		return;
	up->tsec = 0;

	/*
	 * Advance minute unit of the day. If the minute unit is not
	 * synchronized, go no further.
	 */
	temp = carry(&up->decvec[MN]);	/* minute units */
	if (!(up->status & DSYNC))
		return;

	/*
	 * Propagate carries through the day.
	 */ 
	if (temp == 0)			/* carry minutes */
		temp = carry(&up->decvec[MN + 1]);
	if (temp == 0)			/* carry hours */
		temp = carry(&up->decvec[HR]);
	if (temp == 0)
		temp = carry(&up->decvec[HR + 1]);

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
	temp = carry(&up->decvec[DA]);	/* carry days */
	if (temp == 0)
		temp = carry(&up->decvec[DA + 1]);
	if (temp == 0)
		temp = carry(&up->decvec[DA + 2]);

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
	temp = carry(&up->decvec[YR]);	/* carry years */
	if (temp)
		carry(&up->decvec[YR + 1]);
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
 * wwv_snr() - compute SNR or likelihood function
 */
static double
wwv_snr(
	double signal,		/* signal */
	double noise		/* noise */
	)
{
	double rval;

	if (signal <= 0 && noise <= 0)
		return (0);
	if (noise <= 0)
		return (30);
	rval = 20 * log10(signal / noise);
	if (rval > 30)
		return (30);
	return (rval);
}


/*
 * timecode() - assemble timecode string and length
 *
 * Prettytime format - similar to Spectracom
 *
 * sq yy ddd hh:mm:ss.fff ld dut lset agc stn comp errs freq avgt
 *
 * s	sync indicator ('?' or ' ')
 * q	quality character (hex 0-F)
 * yyyy	year of century
 * ddd	day of year
 * hh	hour of day
 * mm	minute of hour
 * ss	minute of hour
 * fff	millisecond of second
 * l	leap second warning ' ' or 'L'
 * d	DST state 'S', 'D', 'I', or 'O'
 * dut	DUT sign and magnitude in deciseconds
 * lset	minutes since last clock update
 * agc	audio gain (0-255)
 * stn	station identifier ('X', 'C', 'H', 'M')
 * comp	minute sync compare counter
 * errs	bit errors in last minute * freq	frequency offset (PPM) * avgt	averaging time (s) */
int
timecode(
	struct wwvunit *up,	/* driver structure pointer */
	char *ptr		/* target string */
	)
{
	int year, day, hour, minute, second, frac, dut;
	char synchar, qual, leapchar, dst, duts, ident;
	char *cptr;

	/*
	 * Common fixed-format fields
	 */
	synchar = (up->status & INSYNC) ? ' ' : '?';
	qual = 0;
	if (up->alarm & (0x3 << (DECERR)))
		qual |= 0x1;
	if (up->alarm & (0x3 << (SYMERR)))
		qual |= 0x2;
	if (up->alarm & (0x3 << (MODERR)))
		qual |= 0x4;
	if (up->alarm & (0x3 << (SYNERR)))
		qual |= 0x8;
	year = up->decvec[7].digit + up->decvec[7].digit * 10;
	if (year < UTCYEAR)
		year += 2000;
	else
		year += 1900;
	day = up->decvec[4].digit + up->decvec[5].digit * 10 +
	    up->decvec[6].digit * 100;
	hour = up->decvec[2].digit + up->decvec[3].digit * 10;
	minute = up->decvec[0].digit + up->decvec[1].digit * 10;
	second = up->tsec;
	frac = (up->tphase * 1000) / SECOND;
	leapchar = (up->misc & SECWAR) ? 'L' : ' ';
	dst = dstcod[(up->misc >> 4) & 0x3];
	duts = (up->misc & DUTS) ? '+' : '-';
	dut = up->misc & 0x7;

	/*
	 * Specific variable-format fields
	 */
	if (up->status & SELV && up->status & SELH)
		ident = 'M';
	else if (up->status & SELV)
		ident = 'C';
	else if (up->status & SELH)
		ident = 'H';
	else
		ident = 'X';
	cptr = ptr;
	sprintf(cptr, "%c%1X", synchar, qual);
	cptr = ptr + strlen(ptr);
	sprintf(cptr, " %4d %03d %02d:%02d:%02d.%.03d", year, day,
	    hour, minute, second, frac);
	cptr = ptr + strlen(ptr);
	sprintf(cptr, " %c%c %c%d %d %d %c %d %d %.1f %d", leapchar,
	    dst, duts, dut, up->minset, up->gain, ident, up->count,
	    up->errcnt, up->freq / SECOND * 1e6, MINAVG << up->avgint);
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
	} else if (up->clipcnt > SECOND / 100) {
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
		up->errflg = CEVNT_FAULT;
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
