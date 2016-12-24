/*
 * NTP test program
 *
 * This program tests to see if the NTP user interface routines
 * ntp_gettime() and ntp_adjtime() have been implemented in the kernel.
 * If so, each of these routines is called to display current timekeeping
 * data.
 */

#include <config.h>

#include "ntp_fp.h"
#include "timespecops.h"
#include "ntp_syscall.h"
#include "ntp_stdlib.h"

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>

#ifdef STRUCT_NTPTIMEVAL_HAS_TIME_TV_NSEC
#define tv_frac_sec tv_nsec
#else
#define tv_frac_sec tv_usec
#endif

/* microseconds per second */
#define MICROSECONDS 1000000

/*
 * Convert usec to a time stamp fraction.
 */
# define TVUTOTSF(tvu)	\
	(uint32_t)((((uint64_t)(tvu) << 32) + MICROSECONDS / 2) / MICROSECONDS)

/*
 * Convert a struct timeval to a time stamp.
 */
#define TVTOTS(tv, ts) \
	do { \
		setlfpuint(*ts, (u_long)(tv)->tv_sec);   \
		setlfpfrac(*ts, TVUTOTSF((tv)->tv_usec)); \
	} while (false)

#define NS_PER_MS_FLOAT	1000.0

/* MUSL port shim */
#ifndef HAVE_NTP_GETTIME
int ntp_gettime(struct ntptimeval *ntv)
{
	struct timex tntx;
	int result;

	ZERO(tntx);
	result = ntp_adjtime(&tntx);
	ntv->time = tntx.time;
	ntv->maxerror = tntx.maxerror;
	ntv->esterror = tntx.esterror;
#if defined(STRUCT_NTPTIMEVAL_HAS_TAI)
	ntv->tai = tntx.tai;
#endif
	return result;
}
#endif	/* !HAVE_NTP_GETTIME */


#define TIMEX_MOD_BITS \
"\20\1OFFSET\2FREQUENCY\3MAXERROR\4ESTERROR\5STATUS\6TIMECONST\
\13PLL\14FLL\15MICRO\16NANO\17CLKB\20CLKA"
 
#define TIMEX_STA_BITS \
"\20\1PLL\2PPSFREQ\3PPSTIME\4FLL\5INS\6DEL\7UNSYNC\10FREQHOLD\
\11PPSSIGNAL\12PPSJITTER\13PPSWANDER\14PPSERROR\15CLOCKERR\
\16NANO\17MODE\20CLK"

#define SCALE_FREQ 65536		/* frequency scale */

/*
 * These constants are used to round the time stamps computed from
 * a struct timeval to the microsecond (more or less).  This keeps
 * things neat.
 */
#define	TS_MASK		0xfffff000	/* mask to usec, for time stamps */
#define	TS_ROUNDBIT	0x00000800	/* round at this bit */

/*
 * Function prototypes
 */
const char *	snprintb	(size_t, char *, u_int, const char *);
const char *	timex_state	(int);

#ifdef SIGSYS
void pll_trap		(int);

static struct sigaction newsigsys;	/* new sigaction status */
static struct sigaction sigsys;		/* current sigaction status */
static sigjmp_buf env;		/* environment var. for pll_trap() */
#endif

static volatile int pll_control; /* (0) daemon, (1) kernel loop */
static volatile int status;	/* most recent status bits */
static volatile int flash;	/* most recent ntp_adjtime() bits */
char* progname;
static char optargs[] = "MNT:cde:f:hjm:o:rs:t:";

int
main(
	int argc,
	char *argv[]
	)
{
	extern int ntp_optind;
	extern char *ntp_optarg;
	struct ntptimeval ntv;
	struct timeval tv;
	struct timex ntx, _ntx;
	int	times[20];
	double ftemp, gtemp, htemp;
	long time_frac;				/* ntv.time.tv_frac_sec (us/ns) */
	l_fp ts;
	volatile unsigned ts_mask = TS_MASK;		/* defaults to 20 bits (us) */
	volatile unsigned ts_roundbit = TS_ROUNDBIT;	/* defaults to 20 bits (us) */
	volatile int fdigits = 6;			/* fractional digits for us */
	size_t c;
	int ch;
	int errflg	= 0;
	int cost	= 0;
	volatile bool json      = false;
	volatile int rawtime	= 0;
	char ascbuf[BUFSIZ];

	ZERO(ntx);
	progname = argv[0];
	while ((ch = ntp_getopt(argc, argv, optargs)) != EOF) {
		switch (ch) {
#ifdef MOD_MICRO
		case 'M':
			ntx.modes |= MOD_MICRO;
			break;
#endif
#ifdef MOD_NANO
		case 'N':
			ntx.modes |= MOD_NANO;
			break;
#endif
#ifdef NTP_API
# if NTP_API > 3
		case 'T':
			ntx.modes = MOD_TAI;
			ntx.constant = atoi(ntp_optarg);
			break;
# endif
#endif
		case 'c':
			cost++;
			break;

		case 'e':
			ntx.modes |= MOD_ESTERROR;
			ntx.esterror = atoi(ntp_optarg);
			break;

		case 'f':
			ntx.modes |= MOD_FREQUENCY;
			ntx.freq = (long)(atof(ntp_optarg) * SCALE_FREQ);
			break;

		case 'j':
			json = true;
			break;

		case 'm':
			ntx.modes |= MOD_MAXERROR;
			ntx.maxerror = atoi(ntp_optarg);
			break;

		case 'o':
			ntx.modes |= MOD_OFFSET;
			ntx.offset = atoi(ntp_optarg);
			break;

		case 'r':
			rawtime++;
			break;

		case 's':
			ntx.modes |= MOD_STATUS;
			ntx.status = atoi(ntp_optarg);
			if (ntx.status < 0 || ntx.status >= 0x100)
				errflg++;
			break;

		case 't':
			ntx.modes |= MOD_TIMECONST;
			ntx.constant = atoi(ntp_optarg);
			break;

		default:
			errflg++;
		}
	}
	if (errflg || (ntp_optind != argc)) {
		fprintf(stderr,
			"usage: %s [-%s]\n\n\
%s%s%s\
-c		display the time taken to call ntp_gettime (us)\n\
-e esterror	estimate of the error (us)\n\
-f frequency	Frequency error (-500 .. 500) (ppm)\n\
-h		display this help info\n\
-j              report in JSON\n\
-m maxerror	max possible error (us)\n\
-o offset	current offset (ms)\n\
-r		print the unix and NTP time raw\n\
-s status	Set the status bits\n\
-t timeconstant	log2 of PLL time constant (0 .. %d)\n",
			progname, optargs,
#ifdef MOD_MICRO
"-M		switch to microsecond mode\n",
#else
"",
#endif
#ifdef MOD_NANO
"-N		switch to nanosecond mode\n",
#else
"",
#endif
#ifdef NTP_API
# if NTP_API > 3
"-T tai_offset	set TAI offset\n",
# else
"",
# endif
#else
"",
#endif
			MAXTC);
		exit(2);
	}

#ifdef SIGSYS
	/*
	 * Test to make sure the sigaction() works in case of invalid
	 * syscall codes.
	 */
	newsigsys.sa_handler = pll_trap;
	newsigsys.sa_flags = 0;
	if (sigaction(SIGSYS, &newsigsys, &sigsys)) {
		perror("sigaction() fails to save SIGSYS trap");
		exit(1);
	}
#endif /* SIGSYS */

	if (cost) {
#ifdef SIGSYS
		if (sigsetjmp(env, 1) == 0) {
#endif
			for (c = 0; c < COUNTOF(times); c++) {
				status = ntp_gettime(&ntv);
				if ((status < 0) && (errno == ENOSYS))
					--pll_control;
				if (pll_control < 0)
					break;
				times[c] = ntv.time.tv_frac_sec;
			}
#ifdef SIGSYS
		}
#endif
		if (pll_control >= 0) {
			printf("[ us %06d:", times[0]);
			for (c = 1; c < COUNTOF(times); c++)
			    printf(" %d", times[c] - times[c - 1]);
			printf(" ]\n");
		}
	}
#ifdef SIGSYS
	if (sigsetjmp(env, 1) == 0) {
#endif
		status = ntp_gettime(&ntv);
		if ((status < 0) && (errno == ENOSYS))
			--pll_control;
#ifdef SIGSYS
	}
#endif
	_ntx.modes = 0;				/* Ensure nothing is set */
#ifdef SIGSYS
	if (sigsetjmp(env, 1) == 0) {
#endif
		status = ntp_adjtime_ns(&_ntx);
		if ((status < 0) && (errno == ENOSYS))
			--pll_control;
		flash = _ntx.status;
#ifdef SIGSYS
	}
#endif
	if (pll_control < 0) {
		printf("NTP user interface routines are not configured in this kernel.\n");
		goto lexit;
	}

	/*
	 * Fetch timekeeping data and display.
	 */
	status = ntp_gettime(&ntv);
	if (status < 0) {
		perror("ntp_gettime() call fails");
	} else {
		/* oldstyle formats */
		const char *ofmt1 = "ntp_gettime() returns code %d (%s)\n";
		const char *ofmt2 = "  time %s, (.%0*d),\n";
		const char *ofmt3 = "  maximum error %lu us, estimated error %lu us";
		const char *ofmt4 = "  ntptime=%x.%x unixtime=%x.%0*d %s";
#if NTP_API > 3
		const char *ofmt5 = ", TAI offset %ld\n";
#else
		const char *ofmt6 = "\n";
#endif /* NTP_API */
		/* JSON formats */
		const char *jfmt1 = "{\"gettime-code\":%d,\"gettime-status\":\"%s\",";
		const char *jfmt2 = "\"time\":\"%s\",\"fractional-time\":\".%0*d\",";
		const char *jfmt3 = "\"maximum-error\":%lu,\"estimated-error\":%lu,";
		const char *jfmt4 = "\"raw-ntp-time\":\"%x.%x\",\"raw-unix-time\":\"%x.%0*d %s\",";
#if NTP_API > 3
		const char *jfmt5 = "\"TAI-offset\":%d,";
#else
		const char *jfmt6 = "";
#endif /* NTP_API */
		printf(json ? jfmt1 : ofmt1, status, timex_state(status));
		time_frac = ntv.time.tv_frac_sec;
#ifdef STA_NANO
		if (flash & STA_NANO) {
			ntv.time.tv_frac_sec /= 1000;
			ts_mask = 0xfffffffc;	/* 1/2^30 */
			ts_roundbit = 0x00000002;
			fdigits = 9;
		}
#endif
		tv.tv_sec = ntv.time.tv_sec;
		tv.tv_usec = ntv.time.tv_frac_sec;
		TVTOTS(&tv, &ts);
		setlfpuint(ts, lfpuint(ts) + JAN_1970);
		setlfpfrac(ts, lfpfrac(ts) + ts_roundbit);
		setlfpfrac(ts, lfpfrac(ts) & ts_mask);
		printf(json ? jfmt2 : ofmt2,  json ? rfc3339date(&ts) : prettydate(&ts), fdigits, (int)time_frac);
		printf(json ? jfmt3 : ofmt3,  (u_long)ntv.maxerror, (u_long)ntv.esterror);
		if (rawtime)
			printf(json ? jfmt4 : ofmt4,
			       (u_int)lfpuint(ts), (u_int)lfpfrac(ts),
			       (int)ntv.time.tv_sec, fdigits,
			       (int)time_frac,

			       ctime_r((time_t *)&ntv.time.tv_sec, ascbuf));
#if defined(STRUCT_NTPTIMEVAL_HAS_TAI)
		printf(json ? jfmt5 : ofmt5, (long)ntv.tai);
#else
		fputs(json ? jfmt6 : ofmt6, stdout);
#endif /* STRUCT_NTPTIMEVAL_HAS_TAI */
	}
	status = ntp_adjtime_ns(&ntx);
	if (status < 0) {
		perror((errno == EPERM) ? 
		   "Must be root to set kernel values\nntp_adjtime() call fails" :
		   "ntp_adjtime() call fails");
	} else {
		char binbuf[132];
		/* oldstyle formats */
		char *ofmt7 = "ntp_adjtime() returns code %d (%s)\n";
		char *ofmt8 = "  modes %s,\n";
		char *ofmt9 = "  offset %.3f";
		char *ofmt10 = " us, frequency %.3f ppm, interval %d s,\n";
		char *ofmt11 = "  maximum error %lu us, estimated error %lu us,\n";
		char *ofmt12 = "  status %s,\n";
		char *ofmt13 = "  time constant %lu, precision %.3f us, tolerance %.0f ppm,\n";
		char *ofmt14 = "  pps frequency %.3f ppm, stability %.3f ppm, jitter %.3f us,\n";
		char *ofmt15 = "  intervals %lu, jitter exceeded %lu, stability exceeded %lu, errors %lu.\n";
		/* JSON formats */
		char *jfmt7 = "\"adjtime-code\":%d,\"adjtime-status\":\"%s\",";
		char *jfmt8 = "\"modes\":\"%s\",";
		char *jfmt9 = "\"offset\":%.3f,";
		char *jfmt10 = "\"frequency\":%.3f,\"interval\":%d,";
		char *jfmt11 = "\"maximum-error\":%lu,\"estimated-error\":%lu,";
		char *jfmt12 = "\"status\":\"%s\",";
		char *jfmt13 = "\"time-constant\":%lu,\"precision\":%.3f,\"tolerance\":%.0f,";
		char *jfmt14 = "\"pps-frequency\":%.3f,\"stability\":%.3f,\"jitter\":%.3f,";
		char *jfmt15 = "\"intervals\":%lu,\"jitter-exceeded\":%lu,\"stability-exceeded\":%lu,\"errors:%lu\n";

		flash = ntx.status;
		printf(json ? jfmt7 : ofmt7, status, timex_state(status));
		printf(json ? jfmt8 : ofmt8,
		       snprintb(sizeof(binbuf), binbuf, ntx.modes, TIMEX_MOD_BITS));
		ftemp = (double)ntx.offset/NS_PER_MS_FLOAT;
		printf(json ? jfmt9 : ofmt9, ftemp);
		ftemp = (double)ntx.freq / SCALE_FREQ;
		printf(json ? jfmt10 : ofmt10, ftemp, 1 << ntx.shift);
		printf(json ? jfmt11 : ofmt11,
		     (u_long)ntx.maxerror, (u_long)ntx.esterror);
		printf(json ? jfmt12 : ofmt12,
		       snprintb(sizeof(binbuf), binbuf,
			       (u_int)ntx.status, TIMEX_STA_BITS));
		ftemp = (double)ntx.tolerance / SCALE_FREQ;
		/*
		 * Before the introduction of ntp_adjtime_ns() the
		 * ntptime code divided this by 1000 when the STA_NANO
		 * flash bit was on.  This doesn't match the Linux
		 * documentation; might have been an error, or
		 * possibly some other systems behave differently.
		 */
		gtemp = (double)ntx.precision;
		printf(json ? jfmt13 : ofmt13,
			(u_long)ntx.constant, gtemp, ftemp);
		if (ntx.shift != 0) {
			ftemp = (double)ntx.ppsfreq / SCALE_FREQ;
			gtemp = (double)ntx.stabil / SCALE_FREQ;
			htemp = (double)ntx.jitter/NS_PER_MS_FLOAT;
			printf(json ? jfmt14 : ofmt14,
			    ftemp, gtemp, htemp);
			printf(json ? jfmt15 : ofmt15,
			    (u_long)ntx.calcnt, (u_long)ntx.jitcnt,
			    (u_long)ntx.stbcnt, (u_long)ntx.errcnt);
		}
		if (json)
		    /* hack to avoid trailing comma - not semabtically needed */ 
		    fputs("\"version\":\""  NTPSEC_VERSION_STRING "\"}\n", stdout);
		exit(EXIT_SUCCESS);
	}

	/*
	 * Put things back together the way we found them.
	 */
    lexit:
#ifdef SIGSYS
	if (sigaction(SIGSYS, &sigsys, (struct sigaction *)NULL)) {
		perror("sigaction() fails to restore SIGSYS trap");
		exit(1);
	}
#endif
	if (json)
	    fputs("}\n", stdout);
	exit(status < 0 ? EXIT_FAILURE : EXIT_SUCCESS);
}

#ifdef SIGSYS
/*
 * pll_trap - trap processor for undefined syscalls
 */
void
pll_trap(
	int arg
	)
{
    UNUSED_ARG(arg);
	pll_control--;
	siglongjmp(env, 1);
}
#endif

/*
 * Print a value a la the %b format of the kernel's printf
 */
const char *
snprintb(
	size_t		buflen,
	char *		buf,
	u_int		v,
	const char *	bits
	)
{
	char *cp;
	char *cplim;
	int i;
	bool any;
	char c;

	if (bits != NULL && *bits == 8)
		snprintf(buf, buflen, "0%o", v);
	else
		snprintf(buf, buflen, "0x%x", v);
	cp = buf + strlen(buf);
	cplim = buf + buflen;
	if (bits != NULL) {
		bits++;
		*cp++ = ' ';
		*cp++ = '(';
		any = false;
		while ((i = *bits++) != 0) {
			if (v & (1 << (i - 1))) {
				if (any) {
					*cp++ = ',';
					if (cp >= cplim)
						goto overrun;
				}
				any = true;
				for (; (c = *bits) > 32; bits++) {
					*cp++ = c;
					if (cp >= cplim)
						goto overrun;
				}
			} else {
				for (; *bits > 32; bits++)
					continue;
			}
		}
		*cp++ = ')';
		if (cp >= cplim)
			goto overrun;
	}
	*cp = '\0';
	return buf;

    overrun:
	return "snprintb buffer too small";
}

const char * const timex_states[] = {
	"OK", "INS", "DEL", "OOP", "WAIT", "ERROR"
};

const char *
timex_state(
	int s
	)
{
	static char buf[32];

	if ((size_t)s < COUNTOF(timex_states))
		return timex_states[s];
	snprintf(buf, sizeof(buf), "TIME-#%d", s);
	return buf;
}
