/*
 * ntp_loopfilter.c - implements the NTP loop filter algorithm
 *
 * ATTENTION: Get approval from Dave Mills on all changes to this file!
 *
 */
#include "config.h"

#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>

#include <signal.h>
#include <setjmp.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_calendar.h"
#include "ntp_stdlib.h"
#include "ntp_syscall.h"
#include "timespecops.h"

#define NTP_MAXFREQ	500e-6

# define FREQTOD(x)	((x) / 65536e6)            /* NTP to double */
# define DTOFREQ(x)	((int32_t)((x) * 65536e6)) /* double to NTP */

/*
 * This is an implementation of the clock discipline algorithm described
 * in UDel TR 97-4-3, as amended. It operates as an adaptive parameter,
 * hybrid phase/frequency-lock loop. A number of sanity checks are
 * included to protect against timewarps, timespikes and general mayhem.
 * All units are in s and s/s, unless noted otherwise.
 */
#define CLOCK_MAX	.128	/* default step threshold (s) */
#define CLOCK_MINSTEP	300.	/* default stepout threshold (s) */
#define CLOCK_PANIC	1000.	/* default panic threshold (s) */
#define	CLOCK_PHI	15e-6	/* max frequency error (s/s) */
#define CLOCK_PLL	16.	/* PLL loop gain (log2) */
#define CLOCK_AVG	8.	/* parameter averaging constant */
#define CLOCK_FLL	.25	/* FLL loop gain */
#define	CLOCK_FLOOR	.0005	/* startup offset floor (s) */
#define	CLOCK_ALLAN	11	/* Allan intercept (log2 s) */
#define CLOCK_LIMIT	30	/* poll-adjust threshold */
#define CLOCK_PGATE	4.	/* poll-adjust gate */
/* #define PPS_MAXAGE	120	* kernel pps signal timeout (s) UNUSED */

/*
 * Clock discipline state machine. This is used to control the
 * synchronization behavior during initialization and following a
 * timewarp.
 *
 *	State	< step		> step		Comments
 *	========================================================
 *	NSET	FREQ		step, FREQ	freq not set
 *
 *	FSET	SYNC		step, SYNC	freq set
 *
 *	FREQ	if (mu < 900)	if (mu < 900)	set freq direct
 *		    ignore	    ignore
 *		else		else
 *		    freq, SYNC	    freq, step, SYNC
 *
 *	SYNC	SYNC		SPIK, ignore	adjust phase/freq
 *
 *	SPIK	SYNC		if (mu < 900)	adjust phase/freq
 *				    ignore
 *				step, SYNC
 */
/*
 * Kernel PLL/PPS state machine. This is used with the kernel PLL
 * modifications described in the documentation.
 *
 * If kernel support for the ntp_adjtime() system call is available, the
 * ntp_control flag is set. The ntp_enable and kern_enable flags can be
 * set at configuration time or run time using ntpq. If ntp_enable is
 * false, the discipline loop is unlocked and no corrections of any kind
 * are made. If both ntp_control and kern_enable are set, the kernel
 * support is used as described above; if false, the kernel is bypassed
 * entirely and the daemon discipline used instead.
 *
 * There have been three versions of the kernel discipline code. The
 * first (microkernel) now in Solaris disciplines the microseconds. The
 * second and third (nanokernel) disciplines the clock in nanoseconds.
 * These versions are identified if the symbol STA_PLL is present in the
 * header file /usr/include/sys/timex.h. The third and current version
 * includes TAI offset and is identified by the symbol NTP_API with
 * value 4.
 *
 * Each PPS time/frequency discipline can be enabled by the atom driver
 * or another driver. If enabled, the STA_PPSTIME and STA_FREQ bits are
 * set in the kernel status word; otherwise, these bits are cleared.
 * These bits are also cleard if the kernel reports an error.
 *
 * If an external clock is present, the clock driver sets STA_CLK in the
 * status word. When the local clock driver sees this bit, it updates
 * via this routine, which then calls ntp_adjtime() with the STA_PLL bit
 * set to zero, in which case the system clock is not adjusted. This is
 * also a signal for the external clock driver to discipline the system
 * clock. Unless specified otherwise, all times are in seconds.
 */
/*
 * Program variables that can be tinkered.
 */
static double	clock_minstep = CLOCK_MINSTEP; /* stepout threshold */
static double	clock_panic = CLOCK_PANIC; /* panic threshold */
double	clock_phi = CLOCK_PHI;	/* dispersion rate (s/s) */
struct ntp_loop_data loop_data = {
	.clock_max_back = CLOCK_MAX, /* step threshold */
	.clock_max_fwd =  CLOCK_MAX, /* step threshold */
	.clock_phi = CLOCK_PHI       /* dispersion rate (s/s) */
};

/*
 * Program variables
 */
static double clock_offset;	/* offset (non-lockclock case only) */
static uptime_t clock_epoch;	/* last update (non-lockclock case only) */
static double init_drift_comp; /* initial frequency (PPM) */
unsigned int	sys_tai;		/* TAI offset from UTC */
/* following variables are non-lockclock case only */
static bool loop_started;	/* true after LOOP_DRIFTINIT */
static void rstclock (int, double); /* transition function */
static double direct_freq(double); /* direct set frequency */
static void set_freq(double);	/* set frequency */

#ifndef PATH_MAX
# define PATH_MAX MAX_PATH
#endif
static char relative_path[PATH_MAX + 1]; /* relative path per recursive make */
static char *this_file = NULL;

static struct timex ntv;	/* ntp_adjtime() parameters */
static int	pll_status;	/* last kernel status bits */
#if defined(STA_NANO) && defined(NTP_API) && NTP_API == 4
static unsigned int loop_tai;	/* last TAI offset (non-lockclock case only) */
#endif /* STA_NANO */
static	void	start_kern_loop(void);
static	void	stop_kern_loop(void);

/*
 * Clock state machine control flags
 */
struct clock_control_flags clock_ctl = {
	.ntp_enable = true,	/* clock discipline enabled */
	.pll_control = false,	/* kernel support available */
	.kern_enable = true,	/* kernel support enabled */
	.hardpps_enable = false,		/* kernel PPS discipline enabled */
	.allow_panic = false,	/* allow panic correction (-g) */
	.force_step_once = false, /* always step time once at startup (-G) */
	.mode_ntpdate = false	/* exit on first clock set (-q) */
};
int	freq_cnt;		/* initial frequency clamp */

static int freq_set;		/* initial set frequency switch */
static bool	ext_enable;	/* external clock enabled */

/*
 * Clock state machine variables
 */
static int	state = 0;	/* clock discipline state (non-lockclock only) */
struct clock_state_machine clkstate = {
  .allan_xpt = CLOCK_ALLAN,
};

/*
 * Huff-n'-puff filter variables
 */
static double *sys_huffpuff;	/* huff-n'-puff filter */
static int sys_hufflen;		/* huff-n'-puff filter stages */
static int sys_huffptr;		/* huff-n'-puff filter pointer */
static double sys_mindly;	/* huff-n'-puff filter min delay */

/* Emacs cc-mode goes nuts if we split the next line... */
#define MOD_BITS (MOD_OFFSET | MOD_MAXERROR | MOD_ESTERROR | \
    MOD_STATUS | MOD_TIMECONST)

static void
sync_status(const char *what, int ostatus, int nstatus) {
	/* only used in non-lockclock case */
	char obuf[256], nbuf[256], tbuf[1024];
	snprintf(obuf, sizeof(obuf), "%04x", (unsigned)ostatus);
	snprintf(nbuf, sizeof(nbuf), "%04x", (unsigned)nstatus);
	snprintf(tbuf, sizeof(tbuf), "%s status: %s -> %s", what, obuf, nbuf);
	report_event(EVNT_KERN, NULL, tbuf);
}

/*
 * file_name - return pointer to non-relative portion of this C file pathname
 */
static char *file_name(void) {
	if (this_file == NULL) {
	    (void)strlcpy(relative_path, __FILE__, PATH_MAX);
	    for (this_file=relative_path;
		*this_file && ! isalnum((unsigned char)*this_file);
		this_file++) { ;
	    }
	}
	return this_file;
}

/*
 * init_loopfilter - initialize loop filter data
 */
void
init_loopfilter(void) {
	/*
	 * Initialize state variables.
	 */
	clkstate.sys_poll = rstrct.ntp_minpoll;
	clkstate.clock_jitter = LOGTOD(sys_vars.sys_precision);
	freq_cnt = (int)clock_minstep;
}

/*
 * ntp_adjtime_error_handler - process errors from ntp_adjtime
 */
static void
ntp_adjtime_error_handler(
	const char *caller,	/* name of calling function */
	struct timex *ptimex,	/* pointer to struct timex */
	int ret,		/* return value from ntp_adjtime */
	int saved_errno,	/* value of errno when ntp_adjtime returned */
	bool pps_call,		/* ntp_adjtime call was PPS-related */
	bool tai_call,		/* ntp_adjtime call was TAI-related */
	int line		/* line number of ntp_adjtime call */
	)
{
	char des[1024] = "";	/* Decoded Error Status */

	switch (ret) {
	    case -1:
		switch (saved_errno) {
		    case EFAULT:
			msyslog(LOG_ERR, "CLOCK: %s: %s line %d: invalid "
                                         "struct timex pointer: 0x%lx",
			    caller, file_name(), line,
			    (long unsigned)((void *)ptimex)
			);
		    break;
		    case EINVAL:
			msyslog(LOG_ERR, "CLOCK: %s: %s line %d: invalid struct timex \"constant\" element value: %ld",
			    caller, file_name(), line,
			    (long)(ptimex->constant)
			);
		    break;
		    case EPERM:
			if (tai_call) {
			    errno = saved_errno;
			    msyslog(LOG_ERR,
				"CLOCK: %s: ntp_adjtime(TAI) failed: %s",
				caller, strerror(errno));
			}
			errno = saved_errno;
			msyslog(LOG_ERR,
				"CLOCK: %s: %s line %d: ntp_adjtime: %s",
				caller, file_name(), line, strerror(errno)
			);
		    break;
		    default:
			msyslog(LOG_NOTICE,
				"CLOCK: %s: %s line %d: unhandled errno value %d after failed ntp_adjtime call",
				caller, file_name(), line,
				saved_errno
			);
		    break;
		}
	    break;
#ifdef TIME_OK
	    case TIME_OK: /* 0: synchronized, no leap second warning */
		/* msyslog(LOG_INFO, "CLOCK: kernel reports time is synchronized normally"); */
	    break;
#else
# warning TIME_OK is not defined
#endif
#ifdef TIME_INS
	    case TIME_INS: /* 1: positive leap second warning */
		msyslog(LOG_INFO, "CLOCK: kernel reports leap second insertion scheduled");
	    break;
#else
# warning TIME_INS is not defined
#endif
#ifdef TIME_DEL
	    case TIME_DEL: /* 2: negative leap second warning */
		msyslog(LOG_INFO, "CLOCK: kernel reports leap second deletion scheduled");
	    break;
#else
# warning TIME_DEL is not defined
#endif
#ifdef TIME_OOP
	    case TIME_OOP: /* 3: leap second in progress */
		msyslog(LOG_INFO, "CLOCK: kernel reports leap second in progress");
	    break;
#else
# warning TIME_OOP is not defined
#endif
#ifdef TIME_WAIT
	    case TIME_WAIT: /* 4: leap second has occurred */
		msyslog(LOG_INFO, "CLOCK: kernel reports leap second has occurred");
	    break;
#else
# warning TIME_WAIT is not defined
#endif
#ifdef TIME_ERROR
#if 0

from the reference implementation of ntp_gettime():

		// Hardware or software error
        if ((time_status & (STA_UNSYNC | STA_CLOCKERR))

	/*
         * PPS signal lost when either time or frequency synchronization
         * requested
         */
	|| (time_status & (STA_PPSFREQ | STA_PPSTIME)
	    && !(time_status & STA_PPSSIGNAL))

        /*
         * PPS jitter exceeded when time synchronization requested
         */
	|| (time_status & STA_PPSTIME &&
            time_status & STA_PPSJITTER)

        /*
         * PPS wander exceeded or calibration error when frequency
         * synchronization requested
         */
	|| (time_status & STA_PPSFREQ &&
            time_status & (STA_PPSWANDER | STA_PPSERROR)))
                return (TIME_ERROR);

or, from ntp_adjtime():

	if (  (time_status & (STA_UNSYNC | STA_CLOCKERR))
	    || (time_status & (STA_PPSFREQ | STA_PPSTIME)
		&& !(time_status & STA_PPSSIGNAL))
	    || (time_status & STA_PPSTIME
		&& time_status & STA_PPSJITTER)
	    || (time_status & STA_PPSFREQ
		&& time_status & (STA_PPSWANDER | STA_PPSERROR))
	   )
		return (TIME_ERROR);
#endif

	    case TIME_ERROR: /* 5: unsynchronized, or loss of synchronization */
				/* error (see status word) */

		des[0] = 0;
		des[1] = 0;  /* we skip first ":".  */

		if (ptimex->status & STA_UNSYNC)
			strlcat(des, ":Clock Unsynchronized", sizeof(des));

		if (ptimex->status & STA_CLOCKERR)
			strlcat(des, ":Clock Error", sizeof(des));

		if (!(ptimex->status & STA_PPSSIGNAL)
		    && ptimex->status & STA_PPSFREQ)
			strlcat(des, ":PPS Frequency Sync wanted but no PPS", sizeof(des));

		if (!(ptimex->status & STA_PPSSIGNAL)
		    && ptimex->status & STA_PPSTIME)
			strlcat(des, ":PPS Time Sync wanted but no PPS signal", sizeof(des));

		if (   ptimex->status & STA_PPSTIME
		    && ptimex->status & STA_PPSJITTER)
			strlcat(des, ":PPS Time Sync wanted but PPS Jitter exceeded", sizeof(des));

		if (   ptimex->status & STA_PPSFREQ
		    && ptimex->status & STA_PPSWANDER)
			strlcat(des, ":PPS Frequency Sync wanted but PPS Wander exceeded", sizeof(des));

		if (   ptimex->status & STA_PPSFREQ
		    && ptimex->status & STA_PPSERROR)
			strlcat(des, ":PPS Frequency Sync wanted but Calibration error detected", sizeof(des));

		if (pps_call && !(ptimex->status & STA_PPSSIGNAL))
			report_event(EVNT_KERN, NULL,
			    "no PPS signal");
		DPRINT(1, ("kernel loop status %#x (%s)\n",
			   (unsigned)ptimex->status, des+1));
		/*
		 * This code may be returned when ntp_adjtime() has just
		 * been called for the first time, quite a while after
		 * startup, when ntpd just starts to discipline the kernel
		 * time. In this case the occurrence of this message
		 * can be pretty confusing.
		 *
		 * HMS: How about a message when we begin kernel processing:
		 *    Determining kernel clock state...
		 * so an initial TIME_ERROR message is less confising,
		 * or skipping the first message (ugh),
		 * or ???
		 * msyslog(LOG_INFO, "CLOCK: kernel reports time synchronization lost");
		 */
		msyslog(LOG_INFO, "CLOCK: kernel reports TIME_ERROR: %#x: %s",
			(unsigned)ptimex->status, des+1);
	    break;
#else
# warning TIME_ERROR is not defined
#endif
	    default:
		msyslog(LOG_NOTICE, "CLOCK: %s: %s line %d: unhandled return value %d from ntp_adjtime() in %s at line %d",
		    caller, file_name(), line,
		    ret,
		    __func__, __LINE__
		);
	    break;
	}
	return;
}

/*
 * local_clock - the NTP logical clock loop filter.
 *
 * Return codes:
 * -1	update ignored: exceeds panic threshold
 * 0	update ignored: popcorn or exceeds step threshold
 * 1	clock was slewed
 * 2	clock was stepped
 *
 * If lockclock is on, the only thing this routine does is set the
 * sys_rootdisp variable equal to the peer dispersion.
 */
int
local_clock(
	struct	peer *peer,	/* synch source peer structure */
	double	fp_offset	/* clock offset (s) */
	)
{
	/*
	 * If the loop is opened or the NIST lockclock scheme is in use,
	 * monitor and record the offsets anyway in order to determine
	 * the open-loop response and then go home.
	 */
	if (loop_data.lockclock || !clock_ctl.ntp_enable) {
		record_loop_stats(fp_offset, loop_data.drift_comp,
            clkstate.clock_jitter, loop_data.clock_stability,
            clkstate.sys_poll);
		return (1);     /* very tiny slew */
	}

	int	rval;		/* return code */
	int	osys_poll;	/* old system poll */
	int	ntp_adj_ret;	/* returned by ntp_adjtime */
	double	mu;		/* interval since last update */
	double	clock_frequency; /* clock frequency */
	double	dtemp, etemp;	/* double temps */
	char	tbuf[80];	/* report buffer */

	/*
	 * If the clock is way off, panic is declared. The clock_panic
	 * defaults to 1000 s; if set to zero, the panic will never
	 * occur. The allow_panic defaults to false, so the first panic
	 * will exit. It can be set true by a command line option, in
	 * which case the clock will be set anyway and time marches on.
	 * But, allow_panic will be set false when the update is less
	 * than the step threshold; so, subsequent panics will exit.
	 */
	if (fabs(fp_offset) > clock_panic && clock_panic > 0 &&
	    !clock_ctl.allow_panic) {
		snprintf(tbuf, sizeof(tbuf),
		    "%+.0f s; set clock manually within %.0f s.",
		    fp_offset, clock_panic);
		report_event(EVNT_SYSFAULT, NULL, tbuf);
		return (-1);
	}

	/*
	 * This section simulates ntpdate. If the offset exceeds the
	 * step threshold (128 ms), step the clock to that time and
	 * exit. Otherwise, slew the clock to that time and exit. Note
	 * that the slew will persist and eventually complete beyond the
	 * life of this program. Note that while ntpdate is active, the
	 * terminal does not detach, so the termination message prints
	 * directly to the terminal.
	 */
	if (clock_ctl.mode_ntpdate) {
		if (  ( fp_offset > loop_data.clock_max_fwd  && loop_data.clock_max_fwd  > 0)
		   || (-fp_offset > loop_data.clock_max_back && loop_data.clock_max_back > 0)) {
			step_systime(fp_offset);
			msyslog(LOG_NOTICE, "CLOCK: time set %+.6f s",
			    fp_offset);
			printf("ntpd: time set %+.6fs\n", fp_offset);
		} else {
			adj_systime(fp_offset, adjtime);
			msyslog(LOG_NOTICE, "CLOCK: time slew %+.6f s",
			    fp_offset);
			printf("ntpd: time slew %+.6fs\n", fp_offset);
		}
		record_loop_stats(fp_offset, loop_data.drift_comp,
            clkstate.clock_jitter, loop_data.clock_stability,
            clkstate.sys_poll);
		exit(0);
	}

	/*
	 * The huff-n'-puff filter finds the lowest delay in the recent
	 * interval. This is used to correct the offset by one-half the
	 * difference between the sample delay and minimum delay. This
	 * is most effective if the delays are highly asymmetric and
	 * clockhopping is avoided and the clock frequency wander is
	 * relatively small.
	 */
	if (sys_huffpuff != NULL) {
		if (peer->delay < sys_huffpuff[sys_huffptr])
			sys_huffpuff[sys_huffptr] = peer->delay;
		if (peer->delay < sys_mindly)
			sys_mindly = peer->delay;
		if (fp_offset > 0)
			dtemp = -(peer->delay - sys_mindly) / 2;
		else
			dtemp = (peer->delay - sys_mindly) / 2;
		fp_offset += dtemp;
		DPRINT(1, ("local_clock: size %d mindly %.6f huffpuff %.6f\n",
			   sys_hufflen, sys_mindly, dtemp));
	}

	/*
	 * Clock state machine transition function which defines how the
	 * system reacts to large phase and frequency excursion. There
	 * are two main regimes: when the offset exceeds the step
	 * threshold (128 ms) and when it does not. Under certain
	 * conditions updates are suspended until the stepout threshold
	 * (900 s) is exceeded. See the documentation on how these
	 * thresholds interact with commands and command line options.
	 *
	 * Note the kernel is disabled if step is disabled or greater
	 * than 0.5 s or in ntpdate mode.
	 */
	osys_poll = clkstate.sys_poll;
	if (clkstate.sys_poll < peer->cfg.minpoll)
		clkstate.sys_poll = peer->cfg.minpoll;
	if (clkstate.sys_poll > peer->cfg.maxpoll)
		clkstate.sys_poll = peer->cfg.maxpoll;
	mu = current_time - clock_epoch;
	clock_frequency = loop_data.drift_comp;
	rval = 1;
	if (  ( fp_offset > loop_data.clock_max_fwd  && loop_data.clock_max_fwd  > 0)
	   || (-fp_offset > loop_data.clock_max_back && loop_data.clock_max_back > 0)
	   || clock_ctl.force_step_once ) {
		if (clock_ctl.force_step_once) {
			clock_ctl.force_step_once = false;  /* we want this only once after startup */
			msyslog(LOG_NOTICE, "CLOCK: Doing initial time step" );
		}

		switch (state) {

		/*
		 * In SYNC state we ignore the first outlier and switch
		 * to SPIK state.
		 */
		case EVNT_SYNC:
			snprintf(tbuf, sizeof(tbuf), "%+.6f s",
			    fp_offset);
			report_event(EVNT_SPIK, NULL, tbuf);
			state = EVNT_SPIK;
			return (0);

		/*
		 * In FREQ state we ignore outliers and inliers. At the
		 * first outlier after the stepout threshold, compute
		 * the apparent frequency correction and step the phase.
		 */
		case EVNT_FREQ:
			if (mu < clock_minstep) {
				return (0);
			}

			clock_frequency = direct_freq(fp_offset);

		/*
		 * In SPIK state we ignore succeeding outliers until
		 * either an inlier is found or the stepout threshold is
		 * exceeded.
		 */
		/* FALLTHRU to EVNT_SPIK */
                        FALLTHRU
		case EVNT_SPIK:
			if (mu < clock_minstep)
				return (0);

		/*
		 * We get here by default in NSET and FSET states and
		 * from above in FREQ or SPIK states.
		 *
		 * In NSET state an initial frequency correction is not
		 * available, usually because the frequency file has not
		 * yet been written. Since the time is outside the step
		 * threshold, the clock is stepped. The frequency will
		 * be set directly following the stepout interval.
		 *
		 * In FSET state the initial frequency has been set from
		 * the frequency file. Since the time is outside the
		 * step threshold, the clock is stepped immediately,
		 * rather than after the stepout interval. Guys get
		 * nervous if it takes 15 minutes to set the clock for
		 * the first time.
		 *
		 * In FREQ and SPIK states the stepout threshold has
		 * expired and the phase is still above the step
		 * threshold. Note that a single spike greater than the
		 * step threshold is always suppressed, even with a
		 * long time constant.
		 */
			/* FALLTHRU to default */
                        FALLTHRU
		default:
			snprintf(tbuf, sizeof(tbuf), "%+.6f s",
			    fp_offset);
			report_event(EVNT_CLOCKRESET, NULL, tbuf);
			step_systime(fp_offset);
			reinit_timer();
			clkstate.tc_counter = 0;
			clkstate.clock_jitter = LOGTOD(sys_vars.sys_precision);
			rval = 2;
			if (state == EVNT_NSET) {
				rstclock(EVNT_FREQ, 0);
				return (rval);
			}
			break;
		}
		rstclock(EVNT_SYNC, 0);
	} else {
		/*
		 * The offset is less than the step threshold. Calculate
		 * the jitter as the exponentially weighted offset
		 * differences.
		 */
		etemp = SQUARE(clkstate.clock_jitter);
		dtemp = SQUARE(fp_offset - clkstate.last_offset);
		clkstate.clock_jitter = SQRT(etemp + (dtemp - etemp) / CLOCK_AVG);
		switch (state) {

		/*
		 * In NSET state this is the first update received and
		 * the frequency has not been initialized. Adjust the
		 * phase, but do not adjust the frequency until after
		 * the stepout threshold.
		 */
		case EVNT_NSET:
			adj_systime(fp_offset, adjtime);
			rstclock(EVNT_FREQ, fp_offset);
			break;

		/*
		 * In FREQ state ignore updates until the stepout
		 * threshold. After that, compute the new frequency, but
		 * do not adjust the frequency until the holdoff counter
		 * decrements to zero.
		 */
		case EVNT_FREQ:
			if (mu < clock_minstep) {
				return (0);
			}

			clock_frequency = direct_freq(fp_offset);
			/* fall through */

		/*
		 * We get here by default in FSET, SPIK and SYNC states.
		 * Here compute the frequency update due to PLL and FLL
		 * contributions. Note, we avoid frequency discipline at
		 * startup until the initial transient has subsided.
		 */
		default:
			clock_ctl.allow_panic = false;
			if (freq_cnt == 0) {

				/*
				 * The FLL and PLL frequency gain constants
				 * depend on the time constant and Allan
				 * intercept. The PLL is always used, but
				 * becomes ineffective above the Allan intercept
				 * where the FLL becomes effective.
				 */
				if (clkstate.sys_poll >= clkstate.allan_xpt) {
					clock_frequency += (fp_offset -
					    clock_offset) / (max(ULOGTOD(clkstate.sys_poll),
					    mu) * CLOCK_FLL);
				}

				/*
				 * The PLL frequency gain (numerator) depends on
				 * the minimum of the update interval and Allan
				 * intercept. This reduces the PLL gain when the
				 * FLL becomes effective.
				 */
				etemp = min(ULOGTOD(clkstate.allan_xpt), mu);
				dtemp = 4 * CLOCK_PLL * ULOGTOD(clkstate.sys_poll);
				clock_frequency += fp_offset * etemp / (dtemp *
				    dtemp);
			}
			rstclock(EVNT_SYNC, fp_offset);
			if (fabs(fp_offset) < CLOCK_FLOOR) {
				freq_cnt = 0;
			}
			break;
		}
	}

	/*
	 * This code segment works when clock adjustments are made using
	 * precision time kernel support and the ntp_adjtime() system
	 * call. This support is available in Solaris 2.6 and later,
	 * Digital Unix 4.0 and later, FreeBSD, Linux and specially
	 * modified kernels for HP-UX 9 and Ultrix 4. In the case of the
	 * DECstation 5000/240 and Alpha AXP, additional kernel
	 * modifications provide a true microsecond clock and nanosecond
	 * clock, respectively.
	 *
	 * Important note: The kernel discipline is used only if the
	 * step threshold is less than 0.5 s, as anything higher can
	 * lead to overflow problems. This might occur if some misguided
	 * lad set the step threshold to something ridiculous.
	 */
	if (clock_ctl.pll_control && clock_ctl.kern_enable && freq_cnt == 0) {
		static int kernel_status;	/* from ntp_adjtime */

		/*
		 * We initialize the structure for the ntp_adjtime()
		 * system call. We have to convert everything to
		 * microseconds or nanoseconds first. Do not update the
		 * system variables if the ext_enable flag is set. In
		 * this case, the external clock driver will update the
		 * variables, which will be read later by the local
		 * clock driver. Afterwards, remember the time and
		 * frequency offsets for jitter and stability values and
		 * to update the frequency file.
		 */
		ZERO(ntv);
		if (ext_enable) {
			ntv.modes = MOD_STATUS;
		} else {
#ifdef STA_NANO
			ntv.modes = MOD_BITS | MOD_NANO;
#else /* STA_NANO */
			ntv.modes = MOD_BITS;
#endif /* STA_NANO */
			if (clock_offset < 0) {
				dtemp = -.5;
			} else {
				dtemp = .5;
			}
			ntv.offset = (long)(clock_offset * NS_PER_S + dtemp);
			ntv.constant = clkstate.sys_poll;
			ntv.esterror = (long)(clkstate.clock_jitter * US_PER_S);
			ntv.maxerror = (long)((sys_vars.sys_rootdelay / 2 +
			    sys_vars.sys_rootdisp) * US_PER_S);
			ntv.status = STA_PLL;

			/*
			 * Enable/disable the PPS if requested.
			 */
			if (clock_ctl.hardpps_enable) {
				ntv.status |= (STA_PPSTIME | STA_PPSFREQ);
				if (!(pll_status & STA_PPSTIME))
					sync_status("PPS enabled",
						pll_status,
						ntv.status);
			} else {
				ntv.status &= ~(STA_PPSTIME | STA_PPSFREQ);
				if (pll_status & STA_PPSTIME)
					sync_status("PPS disabled",
						pll_status,
						ntv.status);
			}
			if (sys_vars.sys_leap == LEAP_ADDSECOND)
				ntv.status |= STA_INS;
			else if (sys_vars.sys_leap == LEAP_DELSECOND) ntv.status |= STA_DEL;
		}

		/*
		 * Pass the stuff to the kernel. If it squeals, turn off
		 * the pps. In any case, fetch the kernel offset,
		 * frequency and jitter.
		 */
		ntp_adj_ret = ntp_adjtime_ns(&ntv);
		/*
		 * A squeal is a return status < 0, or a state change.
		 */
		if ((0 > ntp_adj_ret) || (ntp_adj_ret != kernel_status)) {
			kernel_status = ntp_adj_ret;
			ntp_adjtime_error_handler(__func__, &ntv, ntp_adj_ret, errno, clock_ctl.hardpps_enable, false, __LINE__ - 1);
		}
		pll_status = ntv.status;
		clock_offset = ntv.offset * S_PER_NS;
		clock_frequency = FREQTOD(ntv.freq);

		/*
		 * If the kernel PPS is lit, monitor its performance.
		 */
		if (ntv.status & STA_PPSTIME) {
			clkstate.clock_jitter = ntv.jitter * S_PER_NS;
		}

#if defined(STA_NANO) && defined(NTP_API) && NTP_API == 4
		/*
		 * If the TAI changes, update the kernel TAI.
		 */
		if (loop_tai != sys_tai) {
			loop_tai = sys_tai;
			ntv.modes = MOD_TAI;
			ntv.constant = (long)sys_tai;
			if ((ntp_adj_ret = ntp_adjtime_ns(&ntv)) != 0) {
			    ntp_adjtime_error_handler(__func__, &ntv, ntp_adj_ret, errno, false, true, __LINE__ - 1);
			}
		}
#endif /* STA_NANO */
	}

	/*
	 * Clamp the frequency within the tolerance range and calculate
	 * the frequency difference since the last update.
	 */
	if (fabs(clock_frequency) > NTP_MAXFREQ)
		msyslog(LOG_NOTICE,
		    "CLOCK: frequency error %.0f PPM exceeds tolerance %.0f PPM",
		    clock_frequency * US_PER_S, NTP_MAXFREQ * US_PER_S);
	dtemp = SQUARE(clock_frequency - loop_data.drift_comp);
	if (clock_frequency > NTP_MAXFREQ)
		loop_data.drift_comp = NTP_MAXFREQ;
	else if (clock_frequency < -NTP_MAXFREQ)
		loop_data.drift_comp = -NTP_MAXFREQ;
	else
		loop_data.drift_comp = clock_frequency;

	/*
	 * Calculate the wander as the exponentially weighted RMS
	 * frequency differences. Record the change for the frequency
	 * file update.
	 */
	etemp = SQUARE(loop_data.clock_stability);
	loop_data.clock_stability = SQRT(etemp + (dtemp - etemp) / CLOCK_AVG);

	/*
	 * Here we adjust the time constant by comparing the current
	 * offset with the clock jitter. If the offset is less than the
	 * clock jitter times a constant, then the averaging interval is
	 * increased, otherwise it is decreased. A bit of hysteresis
	 * helps calm the dance. Works best using burst mode. Don't
	 * fiddle with the poll during the startup clamp period.
	 */
	if (freq_cnt > 0) {
		clkstate.tc_counter = 0;
	} else if (fabs(clock_offset) < CLOCK_PGATE * clkstate.clock_jitter) {
		clkstate.tc_counter += clkstate.sys_poll;
		if (clkstate.tc_counter > CLOCK_LIMIT) {
			clkstate.tc_counter = CLOCK_LIMIT;
			if (clkstate.sys_poll < peer->cfg.maxpoll) {
				clkstate.tc_counter = 0;
				clkstate.sys_poll++;
			}
		}
	} else {
		clkstate.tc_counter -= clkstate.sys_poll << 1;
		if (clkstate.tc_counter < -CLOCK_LIMIT) {
			clkstate.tc_counter = -CLOCK_LIMIT;
			if (clkstate.sys_poll > peer->cfg.minpoll) {
				clkstate.tc_counter = 0;
				clkstate.sys_poll--;
			}
		}
	}

	/*
	 * If the time constant has changed, update the poll variables.
	 */
	if (osys_poll != clkstate.sys_poll) {
		poll_update(peer, clkstate.sys_poll);
	}

	/*
	 * Yibbidy, yibbbidy, yibbidy; that'h all folks.
	 */
	record_loop_stats(clock_offset, loop_data.drift_comp,
        clkstate.clock_jitter, loop_data.clock_stability, clkstate.sys_poll);
	DPRINT(1, ("local_clock: offset %.9f jit %.9f freq %.6f stab %.3f poll %d\n",
		   clock_offset, clkstate.clock_jitter,
           loop_data.drift_comp * US_PER_S,
           loop_data.clock_stability * US_PER_S, clkstate.sys_poll));
	return (rval);
}


/*
 * adj_host_clock - Called once every second to update the local clock.
 *
 * If lockclock is on the only thing this routine does is increment the
 * sys_rootdisp variable.
 */
void
adj_host_clock(
	void
	)
{
	double	offset_adj;
	double	freq_adj;

	/*
	 * Update the dispersion since the last update. In contrast to
	 * NTPv3, NTPv4 does not declare unsynchronized after one day,
	 * since the dispersion check serves this function. Also,
	 * since the poll interval can exceed one day, the old test
	 * would be counterproductive. During the startup clamp period, the
	 * time constant is clamped at 2.
	 */
	sys_vars.sys_rootdisp += loop_data.clock_phi;
	if (loop_data.lockclock || !clock_ctl.ntp_enable || clock_ctl.mode_ntpdate)
		return;
	/*
	 * Determine the phase adjustment. The gain factor (denominator)
	 * increases with poll interval, so is dominated by the FLL
	 * above the Allan intercept. Note the reduced time constant at
	 * startup.
	 */
	if (state != EVNT_SYNC) {
		offset_adj = 0.;
	} else if (freq_cnt > 0) {
		offset_adj = clock_offset / (CLOCK_PLL * ULOGTOD(1));
		freq_cnt--;
	} else if (clock_ctl.pll_control && clock_ctl.kern_enable) {
		offset_adj = 0.;
	} else {
		offset_adj = clock_offset / (CLOCK_PLL * ULOGTOD(clkstate.sys_poll));
	}

	/*
	 * If the kernel discipline is enabled the frequency correction
	 * drift_comp has already been engaged via ntp_adjtime() in
	 * set_freq().  Otherwise it is a component of the adj_systime()
	 * offset.
	 */
	if (clock_ctl.pll_control && clock_ctl.kern_enable)
		freq_adj = 0.;
	else
		freq_adj = loop_data.drift_comp;

	/* Bound absolute value of total adjustment to NTP_MAXFREQ. */
	if (offset_adj + freq_adj > NTP_MAXFREQ) {
		offset_adj = NTP_MAXFREQ - freq_adj;
	} else if (offset_adj + freq_adj < -NTP_MAXFREQ) {
		offset_adj = -NTP_MAXFREQ - freq_adj;
	}

	clock_offset -= offset_adj;
	/*
	 * FIXME: With the Windows legacy port officially gone, this
	 * could be called much less often.  However, leave some
	 * version of the comment below in place in case of a re-port.
	 * See also the related FIXME comment in libntp/systime.c
	 *
	 * Windows port adj_systime() must be called each second,
	 * even if the argument is zero, to ease emulation of
	 * adjtime() using Windows' slew API which controls the rate
	 * but does not automatically stop slewing when an offset
	 * has decayed to zero.
	 */
	adj_systime(offset_adj + freq_adj, adjtime);
}


/*
 * Clock state machine. Enter new state and set state variables.
 */
static void
rstclock(
	int	trans,		/* new state */
	double	offset		/* new offset */
	)
{
        DPRINT(2, ("local_clock: mu %" PRIu32 " state %d poll %d count %d\n",
                  current_time - clock_epoch, trans, clkstate.sys_poll,
                  clkstate.tc_counter));
	if (trans != state && trans != EVNT_FSET)
		report_event(trans, NULL, NULL);
	state = trans;
	clkstate.last_offset = clock_offset = offset;
	clock_epoch = current_time;
}


/*
 * calc_freq - calculate frequency directly
 *
 * This is very carefully done. When the offset is first computed at the
 * first update, a residual frequency component results. Subsequently,
 * updates are suppressed until the end of the measurement interval while
 * the offset is amortized. At the end of the interval the frequency is
 * calculated from the current offset, residual offset, length of the
 * interval and residual frequency component. At the same time the
 * frequenchy file is armed for update at the next hourly stats.
 */
static double
direct_freq(
	double	fp_offset
	)
{
	set_freq(fp_offset / (current_time - clock_epoch));

	return loop_data.drift_comp;
}

/*
 * set_freq - set clock frequency correction
 *
 * Used to step the frequency correction at startup, possibly again once
 * the frequency is measured (that is, transitioning from EVNT_NSET to
 * EVNT_FSET), and finally to switch between daemon and kernel loop
 * discipline at runtime.
 *
 * When the kernel loop discipline is available but the daemon loop is
 * in use, the kernel frequency correction is disabled (set to 0) to
 * ensure drift_comp is applied by only one of the loops.
 */
static void
set_freq(
	double	freq		/* frequency update */
	)
{
	const char *	loop_desc;

	loop_data.drift_comp = freq;
	loop_desc = "ntpd";
	if (clock_ctl.pll_control) {
		int ntp_adj_ret;
		ZERO(ntv);
		ntv.modes = MOD_FREQUENCY;
		if (clock_ctl.kern_enable) {
			loop_desc = "kernel";
			ntv.freq = DTOFREQ(loop_data.drift_comp);
		}
		if ((ntp_adj_ret = ntp_adjtime_ns(&ntv)) != 0) {
		    ntp_adjtime_error_handler(__func__, &ntv, ntp_adj_ret, errno, false, false, __LINE__ - 1);
		}
	}
	mprintf_event(EVNT_FSET, NULL, "%s %.6f PPM", loop_desc,
	    loop_data.drift_comp * US_PER_S);
}

static void
start_kern_loop(void)
{
	static bool atexit_done;
	int ntp_adj_ret;

	clock_ctl.pll_control = true;
	ZERO(ntv);
	ntv.modes = MOD_BITS;
	ntv.status = STA_PLL | STA_UNSYNC;
	ntv.maxerror = sys_maxdisp * 1e6;
	ntv.esterror = sys_maxdisp * 1e6;
	ntv.constant = clkstate.sys_poll;
	if ((ntp_adj_ret = ntp_adjtime_ns(&ntv)) != 0) {
	    ntp_adjtime_error_handler(__func__, &ntv, ntp_adj_ret, errno, false, false, __LINE__ - 1);
	}

	/*
	 * Save the result status and light up an external clock
	 * if available.
	 */
	pll_status = ntv.status;
	if (clock_ctl.pll_control) {
		if (!atexit_done) {
			atexit_done = true;
			atexit(&stop_kern_loop);
		}
#ifdef STA_NANO
		if (pll_status & STA_CLK)
			ext_enable = true;
#endif /* STA_NANO */
		report_event(EVNT_KERN, NULL,
	  	    "kernel time sync enabled");
	}
}


static void
stop_kern_loop(void)
{
	if (clock_ctl.pll_control && clock_ctl.kern_enable)
		report_event(EVNT_KERN, NULL,
		    "kernel time sync disabled");
}


/*
 * select_loop() - choose kernel or daemon loop discipline.
 */
void
select_loop(
	int	use_kern_loop
	)
{
	if (clock_ctl.kern_enable == use_kern_loop)
		return;
	if (clock_ctl.pll_control && !use_kern_loop)
		stop_kern_loop();
	clock_ctl.kern_enable = use_kern_loop;
	if (clock_ctl.pll_control && use_kern_loop)
		start_kern_loop();
	/*
	 * If this loop selection change occurs after initial startup,
	 * call set_freq() to switch the frequency compensation to or
	 * from the kernel loop.
	 */
	if (!loop_data.lockclock && clock_ctl.pll_control && loop_started)
		set_freq(loop_data.drift_comp);
}


/*
 * huff-n'-puff filter
 */
void
huffpuff(void)
{
	int i;

	if (sys_huffpuff == NULL) {
		return;
	}

	sys_huffptr = (sys_huffptr + 1) % sys_hufflen;
	sys_huffpuff[sys_huffptr] = 1e9;
	sys_mindly = 1e9;
	for (i = 0; i < sys_hufflen; i++) {
		if (sys_huffpuff[i] < sys_mindly) {
			sys_mindly = sys_huffpuff[i];
		}
	}
}


/*
 * loop_config - configure the loop filter
 *
 * If lockclock is on, the LOOP_DRIFTINIT case is a no-op.
 */
void
loop_config(
	int	item,
	double	freq
	)
{
	int	i;

	DPRINT(2, ("loop_config: item %d freq %f\n", item, freq));
	switch (item) {

	/*
	 * We first assume the kernel supports the ntp_adjtime()
	 * syscall. If that syscall works, initialize the kernel time
	 * variables. Otherwise, continue leaving no harm behind.
	 */
	case LOOP_DRIFTINIT:
		if (loop_data.lockclock || clock_ctl.mode_ntpdate)
			break;

		start_kern_loop();

		/*
		 * Initialize frequency if given; otherwise, begin frequency
		 * calibration phase.
		 */
		{
			double ftemp = init_drift_comp / US_PER_S;
			if (ftemp > NTP_MAXFREQ) {
				ftemp = NTP_MAXFREQ;
			} else if (ftemp < -NTP_MAXFREQ) {
				ftemp = -NTP_MAXFREQ;
			}
			set_freq(ftemp);
		}
		if (freq_set)
			rstclock(EVNT_FSET, 0);
		else
			rstclock(EVNT_NSET, 0);
		loop_started = true;
		break;

	/*
	 * Tinker command variables for Ulrich Windl. Very dangerous.
	 */
	case LOOP_ALLAN:	/* Allan intercept (log2) (allan) */
		clkstate.allan_xpt = (uint8_t)freq;
		break;

	case LOOP_PHI:		/* dispersion threshold (dispersion) */
		loop_data.clock_phi = freq / US_PER_S;
		break;

	case LOOP_FREQ:		/* initial frequency (freq) */
		init_drift_comp = freq;
		freq_set++;
		break;

	case LOOP_HUFFPUFF:	/* huff-n'-puff length (huffpuff) */
		if (freq < HUFFPUFF)
			freq = HUFFPUFF;
		sys_hufflen = (int)(freq / HUFFPUFF);
		sys_huffpuff = emalloc(sizeof(sys_huffpuff[0]) *
		    (unsigned long)sys_hufflen);
		for (i = 0; i < sys_hufflen; i++) {
			sys_huffpuff[i] = 1e9;
		}
		sys_mindly = 1e9;
		break;

	case LOOP_PANIC:	/* panic threshold (panic) */
		clock_panic = freq;
		break;

	case LOOP_MAX:		/* step threshold (step) */
		loop_data.clock_max_fwd = loop_data.clock_max_back = freq;
		if ( D_ISZERO_NS(freq) || freq > 0.5)
			select_loop(false);
		break;

	case LOOP_MAX_BACK:	/* step threshold (step) */
		loop_data.clock_max_back = freq;
		/*
		 * Leave using the kernel discipline code unless both
		 * limits are massive.  This assumes the reason to stop
		 * using it is that it's pointless, not that it goes wrong.
		 */
		if ( D_ISZERO_NS(loop_data.clock_max_back) || (loop_data.clock_max_back > 0.5)
		   || D_ISZERO_NS(loop_data.clock_max_fwd) || (loop_data.clock_max_fwd  > 0.5))
			select_loop(false);
		break;

	case LOOP_MAX_FWD:	/* step threshold (step) */
		loop_data.clock_max_fwd = freq;
		if ( D_ISZERO_NS(loop_data.clock_max_back) || (loop_data.clock_max_back > 0.5)
		   || D_ISZERO_NS(loop_data.clock_max_fwd) || (loop_data.clock_max_fwd  > 0.5))
			select_loop(false);
		break;

	case LOOP_MINSTEP:	/* stepout threshold (stepout) */
		if (freq < CLOCK_MINSTEP) {
			clock_minstep = CLOCK_MINSTEP;
		} else {
			clock_minstep = freq;
		}
		break;

	case LOOP_LEAP:		/* not used, fall through */
	default:
		msyslog(LOG_NOTICE,
		    "CONFIG: loop_config: unsupported option %d", item);
	}
}

