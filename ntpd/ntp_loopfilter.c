/*
 * ntp_loopfilter.c - implements the NTP loop filter algorithm
 *
 */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>


#include <signal.h>
#include <setjmp.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_unixtime.h"
#include "ntp_stdlib.h"

#if defined(VMS) && defined(VMS_LOCALUNIT)	/*wjm*/
#include "ntp_refclock.h"
#endif /* VMS */

#ifdef KERNEL_PLL
#include "ntp_syscall.h"
#endif /* KERNEL_PLL */

/*
 * This is an implementation of the clock discipline algorithm described
 * in UDel TR 97-4-3, as amended. It operates as an adaptive parameter,
 * hybrid phase/frequency-lock loop. A number of sanity checks are
 * included to protect against timewarps, timespikes and general mayhem.
 * All units are in s and s/s, unless noted otherwise.
 */
#define CLOCK_MAX	.128	/* default max offset (s) */
#define CLOCK_PANIC	1000.	/* default panic offset (s) */
#define CLOCK_MAXSTAB	2e-6	/* max frequency stability */
#define CLOCK_MAXERR	1e-2	/* max phase jitter (s) */
#define SHIFT_PLL	4	/* PLL loop gain (shift) */
#define CLOCK_AVG	4.	/* FLL loop gain */
#define CLOCK_MINSEC	256.	/* min FLL update interval (s) */
#define CLOCK_MINSTEP	900.	/* step-change timeout (s) */
#define CLOCK_DAY	86400.	/* one day of seconds */
#define CLOCK_LIMIT	30	/* poll-adjust threshold */
#define CLOCK_PGATE	4.	/* poll-adjust gate */
#define CLOCK_ALLAN	1024.	/* min Allan intercept (s) */
#define CLOCK_ADF	1e11	/* Allan deviation factor */

/*
 * Clock discipline state machine. This is used to control the
 * synchronization behavior during initialization and following a
 * timewarp. 
 */
#define S_NSET	0		/* clock never set */
#define S_FSET	1		/* frequency set from the drift file */
#define S_TSET	2		/* time set */
#define S_FREQ	3		/* frequency mode */
#define S_SYNC	4		/* clock synchronized */
#define S_SPIK	5		/* spike detected */

/*
 * Kernel PLL/PPS state machine. This is used with the kernel PLL
 * modifications described in the README.kernel file.
 *
 * If kernel support for the ntp_adjtime() system call is available, the
 * ntp_control flag is set. The ntp_enable and kern_enable flags can be
 * set at configuration time or run time using ntpdc. If ntp_enable is
 * false, the discipline loop is unlocked and no correctios of any kind
 * are made. If both ntp_control and kern_enable are set, the kernel
 * support is used as described above; if false, the kernel is bypassed
 * entirely and the daemon PLL used instead.
 *
 * Each update to a prefer peer sets pps_stratum if it survives the
 * intersection algorithm and its time is within range. The PPS time
 * discipline is enabled (STA_PPSTIME bit set in the status word) when
 * pps_stratum is true and the PPS frequency discipline is enabled. If
 * the PPS time discipline is enabled and the kernel reports a PPS
 * signal is present, the pps_control variable is set to the current
 * time. If the current time is later than pps_control by PPS_MAXAGE
 * (120 s), this variable is set to zero.
 *
 * If an external clock is present, the clock driver sets STA_CLK in the
 * status word. When the local clock driver sees this bit, it updates
 * via this routine, which then calls ntp_adjtime() with the STA_PLL bit
 * set to zero, in which case the system clock is not adjusted. This is
 * also a signal for the external clock driver to discipline the system
 * clock.
 */
#define PPS_MAXAGE 120		/* kernel pps signal timeout (s) */

/*
 * Program variables
 */
static double clock_offset;	/* clock offset adjustment (s) */
double	drift_comp;		/* clock frequency (ppm) */
double	clock_stability;	/* clock stability (ppm) */
double	clock_max = CLOCK_MAX;	/* max offset allowed before step (s) */
static double clock_panic = CLOCK_PANIC; /* max offset before panic */
u_long	pps_control;		/* last pps sample time */
static void rstclock P((int));	/* state transition function */

#ifdef KERNEL_PLL
struct timex ntv;		/* kernel API parameters */
int	pll_status;		/* status bits for kernel pll */
int	pll_nano;		/* nanosecond kernel switch */
#endif /* KERNEL_PLL */

/*
 * Clock state machine control flags
 */
int	ntp_enable;		/* clock discipline enabled */
int	pll_control;		/* kernel support available */
int	kern_enable;		/* kernel support enabled */
int	pps_enable;		/* kernel PPS discipline enabled */
int	ext_enable;		/* external clock enabled */
int	pps_stratum;		/* pps stratum */
int	allow_set_backward = TRUE; /* step corrections allowed */
int	correct_any = FALSE;	/* corrections > 1000 s allowed */

/*
 * Clock state machine variables
 */
u_char	sys_poll;		/* log2 of system poll interval */
int	state;			/* clock discipline state */
int	tc_counter;		/* poll-adjust counter */
u_long	last_time;		/* time of last clock update (s) */
double	last_offset;		/* last clock offset (s) */
double	allan_xpt;		/* Allan intercept (s) */
double	sys_jitter;		/* system RMS jitter (s) */

#if defined(KERNEL_PLL)
/* Emacs cc-mode goes nuts if we split the next line... */
#define MOD_BITS (MOD_OFFSET | MOD_MAXERROR | MOD_ESTERROR | \
    MOD_STATUS | MOD_TIMECONST)
#ifdef SIGSYS
static void pll_trap P((int));	/* configuration trap */
static struct sigaction sigsys;	/* current sigaction status */
static struct sigaction newsigsys; /* new sigaction status */
static sigjmp_buf env;		/* environment var. for pll_trap() */
#endif /* SIGSYS */
#endif /* KERNEL_PLL */

/*
 * init_loopfilter - initialize loop filter data
 */
void
init_loopfilter(void)
{
	/*
	 * Initialize state variables. Initially, we expect no drift
	 * file, so set the state to S_NSET.
	 */
	rstclock(S_NSET);
}

/*
 * local_clock - the NTP logical clock loop filter. Returns 1 if the
 * clock was stepped, 0 if it was slewed and -1 if it is hopeless.
 */
int
local_clock(
	struct peer *peer,	/* synch source peer structure */
	double fp_offset,	/* clock offset (s) */
	double epsil		/* jittter (square s*s) */
	)
{
	double mu;		/* interval since last update (s) */
	double oerror;		/* previous error estimate */
	double flladj;		/* FLL frequency adjustment (ppm) */
	double plladj;		/* PLL frequency adjustment (ppm) */
	double clock_frequency;	/* clock frequency adjustment (ppm) */
	double dtemp, etemp;	/* double temps */
	int retval;		/* return value */

	/*
	 * If the loop is opened, monitor and record the offsets
	 * anyway in order to determine the open-loop response.
	 */
#ifdef DEBUG
	if (debug)
		printf(
		    "local_clock: assid %d offset %.6f jitter %.6f state %d\n",
		    peer->associd, fp_offset, SQRT(epsil), state);
#endif
	if (!ntp_enable) {
		record_loop_stats();
		return(0);
	}

	/*
	 * If the clock is way off, don't tempt fate by correcting it.
	 */
#ifndef SYS_WINNT
	if (fabs(fp_offset) >= clock_panic && !correct_any) {
		msyslog(LOG_ERR,
		    "time error %.0f over %d seconds; set clock manually",
		    fp_offset, (int)clock_panic);
		return (-1);
	}
#endif
	/*
	 * If the clock has never been set, set it and initialize the
	 * discipline parameters. We then switch to frequency mode to
	 * speed the inital convergence process. If lucky, after an hour
	 * the ntp.drift file is created and initialized and we don't
	 * get here again.
	 */
	if (state == S_NSET) {
		step_systime(fp_offset);
		NLOG(NLOG_SYNCEVENT|NLOG_SYSEVENT)
		    msyslog(LOG_NOTICE, "time set %.6f s", fp_offset);
		rstclock(S_TSET);
		rstclock(S_FREQ);
		return (1);
	}

	/*
	 * Update the jitter estimate.
	 */
	oerror = sys_jitter;
	dtemp = SQUARE(sys_jitter);
	sys_jitter = SQRT(dtemp + (epsil - dtemp) / CLOCK_AVG);

	/*
	 * Clock state machine transition function. This is where the
	 * action is and defines how the system reacts to large phase
	 * and frequency errors. There are two main regimes: when the
	 * phase error exceeds the maximum allowed for ordinary tracking
	 * and otherwise when it does not.
	 */
	retval = 0;
	clock_frequency = flladj = plladj = 0;
	mu = current_time - last_time;
	if (fabs(fp_offset) > clock_max) {
		switch (state) {

		/*
		 * In S_TSET state the time has been set at the last
		 * valid update and the offset at that time set to zero.
		 * If following that we cruise outside the capture
		 * range, assume a really bad frequency error and switch
		 * to S_FREQ state.
		 */
		case S_TSET:
			rstclock(S_FREQ);
			last_offset = clock_offset = fp_offset;
			return (0);

		/*
		 * In S_SYNC state we ignore outlyers. At the first
		 * outlyer after CLOCK_MINSTEP (900 s), switch to S_SPIK
		 * state.
		 */
		case S_SYNC:
			if (mu < CLOCK_MINSTEP)
				return (0);
			rstclock(S_SPIK);
			return (0);

		/*
		 * In S_FREQ state we ignore outlyers. At the first
		 * outlyer after CLOCK_MINSTEP (900 s), compute the
		 * apparent phase and frequency correction.
		 */
		case S_FREQ:
			if (mu < CLOCK_MINSTEP)
				return (0);
			clock_frequency = (fp_offset - clock_offset) /
			    mu;
			/* fall through to default */

		/*
		 * In S_SPIK state a large correction is necessary.
		 * Since the outlyer may be due to a large frequency
		 * error, compute the apparent frequency correction.
		 */
		case S_SPIK:
			clock_frequency = (fp_offset - clock_offset) /
			    mu;
			/* fall through to default */

		/*
		 * We get here directly in S_FSET state and indirectly
		 * from S_FREQ and S_SPIK states. The clock is either
		 * reset or shaken, but never stirred.
		 */
		default:
			if (allow_set_backward || correct_any) {
				step_systime(fp_offset);
				NLOG(NLOG_SYNCEVENT|NLOG_SYSEVENT)
				    msyslog(LOG_NOTICE, "time reset %.6f s",
		   		    fp_offset);
				rstclock(S_TSET);
				retval = 1;
			} else {
				NLOG(NLOG_SYNCEVENT|NLOG_SYSEVENT)
				    msyslog(LOG_NOTICE, "time slew %.6f s",
				    fp_offset);
				rstclock(S_FREQ);
				last_offset = clock_offset = fp_offset;
			}
			break;
		}
	} else {
		switch (state) {

		/*
		 * If this is the first update, initialize the
		 * discipline parameters and pretend we had just set the
		 * clock. We don't want to step the clock unless we have
		 * to.
		 */
		case S_FSET:
			rstclock(S_TSET);
			last_offset = clock_offset = fp_offset;
			return (0);

		/*
		 * In S_FREQ state we ignore updates until CLOCK_MINSTEP
		 * (900 s). After that, correct the phase and frequency
		 * and switch to S_SYNC state.
		 */
		case S_FREQ:
			if (mu < CLOCK_MINSTEP)
				return (0);
			clock_frequency = (fp_offset - clock_offset) /
			    mu;
			clock_offset = fp_offset;
			rstclock(S_SYNC);
			break;

		/*
		 * Either the clock has just been set or the previous
		 * update was a spike and ignored. Since this update is
		 * not an outlyer, fold the tent and resume life.
		 */
		case S_TSET:
		case S_SPIK:
			rstclock(S_SYNC);
			/* fall through to default */

		/*
		 * We come here in the normal case for linear phase and
		 * frequency adjustments. If the offset exceeds the
		 * previous time error estimate by CLOCK_SGATE and the
		 * interval since the last update is less than twice the
		 * poll interval, consider the update a popcorn spike
		 * and ignore it.
		 */
		default:
			if (fabs(fp_offset - last_offset) >
			    CLOCK_SGATE * oerror && mu <
			    ULOGTOD(sys_poll + 1)) {
#ifdef DEBUG
				if (debug)
					printf(
					    "local_clock: popcorn %.6f %.6f\n",
					    fabs(fp_offset - last_offset),
					    CLOCK_SGATE * oerror);
#endif
				last_offset = fp_offset;
				return (0);
			}

			/*
			 * Compute the FLL and PLL frequency adjustments
			 * conditioned on intricate weighting factors.
			 * For the FLL, the averaging interval is
			 * clamped not to decrease below the Allan
			 * intercept and the gain is decreased from
			 * unity for mu above CLOCK_MINSEC (1024 s) to
			 * zero below CLOCK_MINSEC (256 s). For the PLL,
			 * the averaging interval is clamped not to
			 * exceed the sustem poll interval. These
			 * measures insure stability of the clock
			 * discipline even when the rules of fair
			 * engagement are broken.
			 */
			dtemp = max(mu, allan_xpt);
			etemp = min(max(0, mu - CLOCK_MINSEC) /
			    CLOCK_ALLAN, 1.);
			flladj = fp_offset * etemp / (dtemp *
			    CLOCK_AVG);
			dtemp = ULOGTOD(SHIFT_PLL + 2 + sys_poll);
			etemp = min(mu, ULOGTOD(sys_poll));
			plladj = fp_offset * etemp / (dtemp * dtemp);
			clock_offset = fp_offset;
			break;
		}
	}

	/*
	 * Clamp the poll interval if the system peer has changed.
	 */
	if (sys_poll > peer->maxpoll)
		sys_poll = peer->maxpoll;
	else if (sys_poll < peer->minpoll)
		sys_poll = peer->minpoll;

#if defined(KERNEL_PLL)
	/*
	 * This code segment works when clock adjustments are made using
	 * precision time kernel support and the ntp_adjtime() system
	 * call. This support is available in Solaris 2.6 and later,
	 * Digital Unix 4.0 and later, FreeBSD, Linux and specially
	 * modified kernels for HP-UX 9 and Ultrix 4. In the case of the
	 * DECstation 5000/240 and Alpha AXP, additional kernel
	 * modifications provide a true microsecond clock and nanosecond
	 * clock, respectively.
	 */
	if (pll_control && kern_enable) {

		/*
		 * We initialize the structure for the ntp_adjtime()
		 * system call. We have to convert everything to
		 * microseconds or nanoseconds first. Do not update the
		 * system variables if the ext_enable flag is set. In
		 * this case, the external clock driver will update the
		 * variables, which will be read later by the local
		 * clock driver. Afterwards, remember the time and
		 * frequency offsets for jitter and stability values and
		 * to update the drift file.
		 */
		memset(&ntv,  0, sizeof ntv);
		if (ext_enable) {
			ntv.modes = MOD_STATUS;
		} else {
			ntv.modes = MOD_BITS;
			if (clock_offset < 0)
				dtemp = -.5;
			else
				dtemp = .5;
			if (pll_nano) {
				ntv.offset = (int32)(clock_offset *
				    1e9 + dtemp);
				ntv.constant = sys_poll;
			} else {
				ntv.offset = (int32)(clock_offset *
				    1e6 + dtemp);
				ntv.constant = sys_poll - 4;
			}
			if (clock_frequency != 0) {
				ntv.modes |= MOD_FREQUENCY;
				ntv.freq = (int32)((clock_frequency +
				    drift_comp) * 65536e6);
			}
			ntv.esterror = (u_int32)(sys_jitter * 1e6);
			ntv.maxerror = (u_int32)((sys_rootdelay / 2 +
			    sys_rootdispersion) * 1e6);
			ntv.status = STA_PLL;

			/*
			 * Set the leap bits in the status word.
			 */
			if (sys_leap == LEAP_NOTINSYNC) {
				ntv.status |= STA_UNSYNC;
			} else if (calleapwhen(sys_reftime.l_ui) <
				    CLOCK_DAY) {
				if (sys_leap & LEAP_ADDSECOND)
					ntv.status |= STA_INS;
				else if (sys_leap & LEAP_DELSECOND)
					ntv.status |= STA_DEL;
			}

			/*
			 * Switch to FLL mode if the poll interval is
			 * greater than MAXDPOLL, so that the kernel
			 * loop behaves as the daemon loop; viz.,
			 * selects the FLL when necessary, etc. For
			 * legacy only.
			 */
			if (sys_poll > NTP_MAXDPOLL)
				ntv.status |= STA_FLL;

			/*
			 * If the PPS signal is up and enabled, light
			 * the frequency bit. If the PPS driver is
			 * working, light the phase bit as well. If not,
			 * douse the lights, since somebody else may
			 * have left the switch on.
			 */
			if (pps_enable && pll_status & STA_PPSSIGNAL) {
				ntv.status |= STA_PPSFREQ;
				if (pps_stratum < STRATUM_UNSPEC)
					ntv.status |= STA_PPSTIME;
			} else {
				ntv.status &= ~(STA_PPSFREQ |
				    STA_PPSTIME);
			}
		}

		/*
		 * Pass the stuff to the kernel. If it squeals, turn off
		 * the pigs. In any case, fetch the kernel offset and
		 * frequency and pretend we did it here.
		 */
		if (ntp_adjtime(&ntv) == TIME_ERROR) {
			if (ntv.status != pll_status)
				msyslog(LOG_ERR,
				    "kernel time discipline status change %x",
				    ntv.status);
			ntv.status &= ~(STA_PPSFREQ | STA_PPSTIME);
		}
		pll_status = ntv.status;
		if (pll_nano)
			clock_offset = ntv.offset / 1e9;
		else
			clock_offset = ntv.offset / 1e6;
		clock_frequency = ntv.freq / 65536e6 - drift_comp;
		flladj = plladj = 0;

		/*
		 * If the kernel PPS is lit, monitor its performance.
		 */
		if (ntv.status & STA_PPSTIME) {
			if (!pps_control)
				NLOG(NLOG_SYSEVENT)msyslog(LOG_INFO,
				    "pps sync enabled");
			pps_control = current_time;
			if (pll_nano)
				sys_jitter = ntv.jitter / 1e9;
			else
				sys_jitter = ntv.jitter / 1e6;
		}
	}
#endif /* KERNEL_PLL */
 
	/*
	 * Adjust the clock frequency and calculate the stability. If
	 * kernel support is available, we use the results of the kernel
	 * discipline instead of the PLL/FLL discipline. In this case,
	 * drift_comp is a sham and used only for updating the drift
	 * file and for billboard eye candy.
	 */
	etemp = clock_frequency + flladj + plladj;
	drift_comp += etemp;
	if (drift_comp > NTP_MAXFREQ)
		drift_comp = NTP_MAXFREQ;
	else if (drift_comp <= -NTP_MAXFREQ)
		drift_comp = -NTP_MAXFREQ;
	dtemp = SQUARE(clock_stability);
	etemp = SQUARE(etemp) - dtemp;
	clock_stability = SQRT(dtemp + etemp / CLOCK_AVG);
	allan_xpt = max(CLOCK_ALLAN, clock_stability * CLOCK_ADF);

	/*
	 * In SYNC state, adjust the poll interval.
	 */
	if (state == S_SYNC) {
		if (clock_stability < CLOCK_MAXSTAB &&
		    fabs(clock_offset) < CLOCK_PGATE * sys_jitter) {
			tc_counter += sys_poll;
			if (tc_counter > CLOCK_LIMIT) {
				tc_counter = CLOCK_LIMIT;
				if (sys_poll < peer->maxpoll) {
					tc_counter = 0;
					sys_poll++;
				}
			}
		} else {
			tc_counter -= sys_poll << 1;
			if (tc_counter < -CLOCK_LIMIT) {
				tc_counter = -CLOCK_LIMIT;
				if (sys_poll > peer->minpoll) {
					tc_counter = 0;
					sys_poll--;
				}
			}
		}
	}

	/*
	 * Update the system time variables.
	 */
	last_time = current_time;
	last_offset = clock_offset;
	dtemp = peer->disp + sys_jitter;
	if ((peer->flags & FLAG_REFCLOCK) == 0 && dtemp < MINDISPERSE)
		dtemp = MINDISPERSE;
	sys_rootdispersion = peer->rootdispersion + dtemp;
	record_loop_stats();
#ifdef DEBUG
	if (debug > 1)
		printf(
	"local_clock: mu %.0f allan %.0f fadj %.3f fll %.3f pll %.3f\n",
		    mu, allan_xpt, clock_frequency * 1e6, flladj * 1e6,
		    plladj * 1e6);
#endif /* DEBUG */
#ifdef DEBUG
	if (debug > 1)
		printf(
		    "local_clock: jit %.6f freq %.3f stab %.3f poll %d cnt %d\n",
		    sys_jitter, drift_comp * 1e6, clock_stability * 1e6,
		    sys_poll, tc_counter);
#endif /* DEBUG */
	return (retval);
}


/*
 * adj_host_clock - Called once every second to update the local clock.
 */
void
adj_host_clock(
	void
	)
{
	double adjustment;

	/*
	 * Update the dispersion since the last update. In contrast to
	 * NTPv3, NTPv4 does not declare unsynchronized after one day,
	 * since the dispersion check serves this function. Also,
	 * since the poll interval can exceed one day, the old test
	 * would be counterproductive. Note we do this even with
	 * external clocks, since the clock driver will recompute the
	 * maximum error and the local clock driver will pick it up and
	 * pass to the common refclock routines. Very elegant.
	 */
	sys_rootdispersion += CLOCK_PHI;

	/*
	 * Declare PPS kernel unsync if the pps signal has not been
	 * heard for a few minutes.
	 */
	if (pps_control && current_time - pps_control > PPS_MAXAGE) {
		if (pps_control)
			NLOG(NLOG_SYSEVENT) /* conditional if clause */
			    msyslog(LOG_INFO, "pps sync disabled");
		pps_control = 0;
	}
	if (!ntp_enable)
		return;

	/*
	 * If the phase-lock loop is implemented in the kernel, we
	 * have no business going further.
	 */
	if (pll_control && kern_enable)
		return;

	/*
	 * Intricate wrinkle for legacy only. If the local clock driver
	 * is in use and selected for synchronization, somebody else may
	 * tinker the adjtime() syscall. If this is the case, the driver
	 * is marked prefer and we have to avoid calling adjtime(),
	 * since that may truncate the other guy's requests.
	 */
	if (sys_peer != 0) {
		if (sys_peer->refclktype == REFCLK_LOCALCLOCK &&
		    sys_peer->flags & FLAG_PREFER)
			return;
	}
	adjustment = clock_offset / ULOGTOD(SHIFT_PLL + sys_poll);
	clock_offset -= adjustment;
	adj_systime(adjustment + drift_comp);
}


/*
 * Clock state machine. Enter new state and set state variables.
 */
static void
rstclock(
	int trans		/* new state */
	)
{
	correct_any = FALSE;
	state = trans;
	switch (state) {

	/*
	 * Frequency mode. The clock has ben set, but the frequency has
	 * not yet been determined. Note that the Allan intercept is set
	 * insure the clock filter considers only the most recent
	 * measurements.
	 */ 
	case S_FREQ:
		sys_poll = NTP_MINDPOLL;
		allan_xpt = CLOCK_ALLAN;
		last_time = current_time;
		break;

	/*
	 * Synchronized mode. Discipline the poll interval.
	 */
	case S_SYNC:
		sys_poll = NTP_MINDPOLL;
		allan_xpt = CLOCK_ALLAN;
		tc_counter = 0;
		break;

	/*
	 * Don't do anything in S_SPIK state; just continue from S_SYNC
	 * state.
	 */
	case S_SPIK:
		break;

	/*
	 * S_NSET, S_FSET and S_TSET states. These transient states set
	 * the time reference for future frequency updates.
	 */
	default:
		sys_poll = NTP_MINDPOLL;
		allan_xpt = CLOCK_ALLAN;
		last_time = current_time;
		last_offset = clock_offset = 0;
		break;
	}
}


/*
 * loop_config - configure the loop filter
 */
void
loop_config(
	int item,
	double freq
	)
{

	switch (item) {
	    case LOOP_DRIFTINIT:

#ifdef KERNEL_PLL
		/*
		 * Assume the kernel supports the ntp_adjtime() syscall.
		 * If that syscall works, initialize the kernel
		 * variables. Otherwise, continue leaving no harm
		 * behind. While at it, ask to set nanosecond mode. If
		 * the kernel agrees, rejoice; othewise, it does only
		 * microseconds.
		 */
		pll_control = 1;
		memset(&ntv, 0, sizeof ntv);
#if NTP_API > 3 
		ntv.modes = MOD_BITS | MOD_NANO;
#else
		ntv.modes = MOD_BITS;
#endif /* NTP_API */
		ntv.maxerror = MAXDISPERSE;
		ntv.esterror = MAXDISPERSE;
		ntv.status = STA_UNSYNC;
#ifdef SIGSYS
		/*
		 * Use sigsetjmp() to save state and then call
		 * ntp_adjtime(); if it fails, then siglongjmp() is used
		 * to return control
		 */
		newsigsys.sa_handler = pll_trap;
		newsigsys.sa_flags = 0;
		if (sigaction(SIGSYS, &newsigsys, &sigsys)) {
			msyslog(LOG_ERR,
			    "sigaction() fails to save SIGSYS trap: %m");
			pll_control = 0;
		}
		if (sigsetjmp(env, 1) == 0)
			ntp_adjtime(&ntv);
		if ((sigaction(SIGSYS, &sigsys,
		    (struct sigaction *)NULL))) {
			msyslog(LOG_ERR,
			    "sigaction() fails to restore SIGSYS trap: %m");
			pll_control = 0;
		}
#else /* SIGSYS */
		ntp_adjtime(&ntv);
#endif /* SIGSYS */
		pll_status = ntv.status;
		if (pll_control) {
#if NTP_API > 3
			if (pll_status & STA_NANO)
				pll_nano = 1;
			if (pll_status & STA_CLK)
				ext_enable = 1;
#endif /* NTP_API */
			msyslog(LOG_NOTICE,
		  	   "kernel time discipline status %04x",
			    pll_status);
		}
#endif /* KERNEL_PLL */
		break;

	    case LOOP_DRIFTCOMP:

		/*
		 * Initialize the kernel frequency and clamp to
		 * reasonable value. Also set the initial state to
		 * S_FSET to indicated the frequency has been
		 * initialized from the previously saved drift file.
		 */
		rstclock(S_FSET);
		drift_comp = freq;
		if (drift_comp > NTP_MAXFREQ)
			drift_comp = NTP_MAXFREQ;
		if (drift_comp < -NTP_MAXFREQ)
			drift_comp = -NTP_MAXFREQ;

#ifdef KERNEL_PLL
		if (pll_control) {
			memset((char *)&ntv, 0, sizeof ntv);
			ntv.modes = MOD_FREQUENCY;
			if (kern_enable)
				ntv.freq = (int32)(drift_comp *
				    65536e6);
			(void)ntp_adjtime(&ntv);
		}
		break;
#endif /* KERNEL_PLL */
	}
}


#if defined(KERNEL_PLL) && defined(SIGSYS)
/*
 * _trap - trap processor for undefined syscalls
 *
 * This nugget is called by the kernel when the SYS_ntp_adjtime()
 * syscall bombs because the silly thing has not been implemented in
 * the kernel. In this case the phase-lock loop is emulated by
 * the stock adjtime() syscall and a lot of indelicate abuse.
 */
static RETSIGTYPE
pll_trap(
	int arg
	)
{
	pll_control = 0;
	siglongjmp(env, 1);
}
#endif /* KERNEL_PLL && SIGSYS */
