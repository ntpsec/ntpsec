/* Windows NT Clock Routines
 *
 * Created by Sven Dietrich  sven@inter-yacht.com
 *
 * New interpolation scheme by Dave Hart <davehart@davehart.com> February 2009
 * overcomes 500us-1ms inherent jitter with the older scheme, first identified
 * by Peter Rosin (nee Ekberg) <peda@lysator.liu.se> in 2003 [Bug 216].
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/resource.h>	/* our private version */

#if defined(_MSC_VER) && _MSC_VER >= 1400	/* VS 2005 */
#include <intrin.h>				/* for __rdtsc() */
#endif

#ifdef HAVE_PPSAPI
#include <timepps.h>
/*
 * ports/winnt/include/timepps.h defines EOPNOTSUPP for compatibility
 * with PPSAPI on other platforms.  ports/winnt/include/isc/net.h has
 * #define EOPNOTSUPP WSAEOPNOTSUPP, so to avoid a macro redefinition
 * warning undefine it.
 */
#undef EOPNOTSUPP
#endif	/* HAVE_PPSAPI */

#include "ntp_stdlib.h"
#include "ntp_unixtime.h"
#include "ntp_timer.h"
#include "clockstuff.h"
#include "ntservice.h"
#include "ntpd.h"
#include "../../../ntpd/ntpd-opts.h"

extern double sys_residual;	/* residual from previous adjustment */

/*
 * Include code to possibly modify the MM timer while the service is active. 
 */

/*
 * Whether or not MM timer modifications takes place is still controlled 
 * by the variable below which is initialized by a default value but 
 * might be changed depending on a command line switch.
 */
static int modify_mm_timer = MM_TIMER_LORES;

#define MM_TIMER_INTV   1  /* the interval we'd want to set the MM timer to [ms] */

static UINT wTimerRes;

BOOL init_randfile();

static long last_Adj = 0;

#define LS_CORR_INTV_SECS  2   /* seconds to apply leap second correction */
#define LS_CORR_INTV   ( (LONGLONG) HECTONANOSECONDS * LS_CORR_INTV_SECS )  
#define LS_CORR_LIMIT  ( (LONGLONG) HECTONANOSECONDS / 2 )  // half a second

typedef union ft_ull {
	FILETIME ft;
	ULONGLONG ull;
	LONGLONG ll;
	LARGE_INTEGER li;
} FT_ULL;

/* leap second stuff */
static FT_ULL ls_ft;
static DWORD ls_time_adjustment;
static ULONGLONG ls_ref_perf_cnt;
static LONGLONG ls_elapsed;

static BOOL winnt_time_initialized = FALSE;
static BOOL winnt_use_interpolation = FALSE;
static ULONGLONG last_interp_time;
static unsigned clock_thread_id;


void WINAPI GetInterpTimeAsFileTime(LPFILETIME pft);
static void StartClockThread(void);
static void tune_ctr_freq(LONGLONG, LONGLONG);
void StopClockThread(void);
void atexit_revert_mm_timer(void);
void time_stepped(void);

static HANDLE clock_thread = NULL;
static HANDLE TimerThreadExitRequest = NULL;

/*
 * interp_time estimates time in 100ns units
 * based on a performance counter value given.
 * The 2nd parameter indicates if this is
 * part of a current time-of-day calculation.
 */
ULONGLONG interp_time(ULONGLONG, BOOL);

/*
 * add_counter_time_pair is called by the
 * high priority clock thread with a new
 * sample.
 */
void add_counter_time_pair(ULONGLONG, LONGLONG);

/*
 * globals used by the above two functions to 
 * implement the counter/time history
 */
#define BASELINES_TOT	256
#define BASELINES_USED	64

static volatile int	newest_baseline = 0;
static volatile int	newest_baseline_gen = 0;
static ULONGLONG	baseline_counts[BASELINES_TOT] = {0};
static LONGLONG		baseline_times[BASELINES_TOT] = {0};

static int		clock_backward_count;
static ULONGLONG	clock_backward_max;


/*
 * clockperiod is the period used for SetSystemTimeAdjustment 
 * slewing calculations but does not necessarily correspond
 * to the precision of the OS clock.  Prior to Windows Vista
 * (6.0) the two were identical.  In 100ns units.
 */
static DWORD clockperiod;

/*
 * os_clock_precision is the observed precision of the OS
 * clock, meaning the increment between discrete values. This
 * is currently calculated once at startup.  100ns units.
 */
static ULONGLONG os_clock_precision;

/*
 * NomPerfCtrFreq is from QueryPerformanceFrequency and is the 
 * number of performance counter beats per second.  PerfCtrFreq
 * starts from NomPerfCtrFreq but is maintained using a sliding
 * window average based on actual performance counter behavior,
 * to allow us to better tolerate powersaving measures that
 * alter the effective frequency of the processor cycle counter
 * (TSC) which sometimes underlies QueryPerformanceCounter.
 *
 * Note that the OS is unlikely to be so subtle in its internal
 * scheduling of waitable timers, presumably done using the
 * performance counter.  Therefore our calculations for
 * interpolated time should be based on PerfCtrFreq but our
 * calculations for SetWaitableTimer should assume the OS will
 * convert from FILETIME 100ns units to performance counter
 * beats using the nominal frequency.
 */

volatile ULONGLONG PerfCtrFreq = 0;
	 ULONGLONG NomPerfCtrFreq = 0;

/* 
 * If we're using RDTSC beating at the same rate as
 * QueryPerformanceCounter, there is a systemic
 * offset we need to account for when using
 * counterstamps from serialpps.sys, which are
 * always from QPC (actually KeQueryPerformanceCounter).
 */
static LONGLONG QPC_offset = 0;

/*
 * use_pcc could be a local static in perf_ctr but
 * lock_thread_to_processor cares about it.
 */
static int use_pcc = -1;

/*
 * ppm_per_adjust_unit is parts per million effect on the OS
 * clock per slewing adjustment unit per second.  Per haps.
 */
static DOUBLE ppm_per_adjust_unit = 0.0;

/*
 * performance counter frequency observations
 */
#define TUNE_CTR_DEPTH		3	/* running avg depth */

static HANDLE		ctr_freq_timer = INVALID_HANDLE_VALUE;
static ULONGLONG	tune_ctr_freq_max_interval;
static unsigned		tune_ctr_period;
void start_ctr_freq_timer(ULONGLONG now_time);
void reset_ctr_freq_timer(ULONGLONG when, ULONGLONG now);
void reset_ctr_freq_timer_abs(ULONGLONG when);

/* round a Windows time to the next bottom of the second */

#define ROUND_TO_NEXT_SEC_BOTTOM(t)	\
do {	\
	(t) += 3 * HECTONANOSECONDS / 2 - 1;	\
	(t) /= HECTONANOSECONDS;	\
	(t) *= HECTONANOSECONDS;	\
	(t) -= HECTONANOSECONDS / 2;	\
} while (0)

/*
 * workaround for VC6 inability to convert unsigned __int64 to double
 */
#define	LL_HNS	((LONGLONG)HECTONANOSECONDS)

/*
 * NT native time format is 100's of nanoseconds since 1601-01-01.
 * Helpers for converting between "hectonanoseconds" and the 
 * performance counter scale from which interpolated time is
 * derived.
 */
#define HNS2PERF(hns)	((hns) * PerfCtrFreq / LL_HNS)
#define PERF2HNS(ctr)	((ctr) * LL_HNS / PerfCtrFreq)


#if defined(_MSC_VER) && _MSC_VER >= 1400	/* VS 2005 */
#define	get_pcc()	__rdtsc()
#else
/*
 * something like this can be used for a compiler without __rdtsc()
 */
ULONGLONG __forceinline 
get_pcc(void)
{
	/* RDTSC returns in EDX:EAX, same as C compiler */
	__asm {
		RDTSC
	}
}
#endif

/*
 * perf_ctr() returns the current performance counter value, 
 * from QueryPerformanceCounter or RDTSC.
 */

ULONGLONG WINAPI 
perf_ctr(void)
{
	FT_ULL ft1;

	if (1 == use_pcc) {
		return get_pcc();
	} else if (0 == use_pcc) {
		QueryPerformanceCounter(&ft1.li);
		return ft1.ull;
	} else {
		FT_ULL ft2;
		FT_ULL ft3;
		FT_ULL ft4;
		FT_ULL ft5;
		LONGLONG offset;
		const char *ntpd_pcc_freq_text;

		/* one-time initialization */

		/*
		 * It's time to make some more permanent knobs,
		 * but for right now the RDTSC aka PCC dance on x86 is:
		 *
		 * 1.  With none of these variables defined, only QPC
		 *     is used because there is no reliable way to
		 *     detect counter frequency variation after ntpd
		 *     startup implemented.
		 * 2.  We need a better knob, but for now if you know
		 *     your RDTSC / CPU frequency is invariant, set
		 *     NTPD_PCC and assuming your QPC is based on the
		 *     PCC as well, RDTSC will be substituted.
		 * 3.  More forcefully, you can jam in a desired exact
		 *     processor frequency, expressed in cycles per
		 *     second by setting NTPD_PCC_FREQ=398125000, for
		 *     example, if yor actual known CPU frequency is
		 *     398.125 MHz, and NTPD_PCC doesn't work because
		 *     QueryPerformanceCounter is implemented using
		 *     another counter.  It is very easy to make ntpd
		 *     fall down if the NTPD_PCC_FREQ value isn't very
		 *     close to the observed RDTSC units per second.
		 *
		 * Items 2 and 3 could probably best be combined into one
		 * new windows-specific command line switch such as
		 *   ntpd --pcc
		 * or
		 *   ntpd --pcc=398125000
		 *
		 * They are currently tied to Windows because that is
		 * the only ntpd port with its own interpolation, and
		 * to x86/x64 because no one has ported the Windows
		 * ntpd port to the sole remaining alternative, Intel
		 * Itanium.
		 */
		if (HAVE_OPT(PCCFREQ))
			ntpd_pcc_freq_text = OPT_ARG(PCCFREQ);
		else
			ntpd_pcc_freq_text = getenv("NTPD_PCC_FREQ");

		if (!HAVE_OPT(USEPCC)
		    && NULL == ntpd_pcc_freq_text
		    && NULL == getenv("NTPD_PCC")) {
			use_pcc = 0;
			QueryPerformanceCounter(&ft1.li);
			return ft1.ull;
		}

#ifdef DEBUG
		if (!NomPerfCtrFreq) {
			msyslog(LOG_ERR, "NomPerfCtrFreq not initialized before first perf_ctr() call");
			exit(-1);
		}
#endif

		QueryPerformanceCounter(&ft1.li);
		ft2.ull = get_pcc();
		Sleep(1);
		QueryPerformanceCounter(&ft3.li);
		Sleep(1);
		ft4.ull = get_pcc();
		Sleep(1);
		QueryPerformanceCounter(&ft5.li);

		offset = ft2.ull - ft1.ull;
		ft3.ull += offset;
		ft5.ull += offset;

		if (ntpd_pcc_freq_text  ||
			(ft2.ull <= ft3.ull &&
			 ft3.ull <= ft4.ull &&
			 ft4.ull <= ft5.ull)) {

			use_pcc = 1;
			ft1.ull = ft2.ull;
			QPC_offset = offset;

			if (ntpd_pcc_freq_text)
				sscanf(ntpd_pcc_freq_text, 
				       "%I64u", 
				       &NomPerfCtrFreq);

			NLOG(NLOG_CLOCKINFO)
				msyslog(LOG_INFO, 
					"using processor cycle counter "
					"%.3f MHz", 
					(LONGLONG)NomPerfCtrFreq / 1e6);
		} else {
			use_pcc = 0;
		}

		return ft1.ull;
	}
}

/*
 * Request Multimedia Timer
 */
void
set_mm_timer(
	int timerres
	)
{
	modify_mm_timer = timerres;
}

/*
 * adj_systime - called once every second to make system time adjustments.
 * Returns 1 if okay, 0 if trouble.
 */
int
adj_systime(
	double now
	)
{
	double dtemp;
	u_char isneg = 0;
	int rc;
	long TimeAdjustment;

	/*
	 * Add the residual from the previous adjustment to the new
	 * adjustment, bound and round.
	 */
	dtemp = sys_residual + now;
	sys_residual = 0;
	if (dtemp < 0)
	{
		isneg = 1;
		dtemp = -dtemp;
	}

	if (dtemp > NTP_MAXFREQ)
		dtemp = NTP_MAXFREQ;

	dtemp = dtemp * 1e6;

	if (isneg)
		dtemp = -dtemp;

	/* 
	 * dtemp is in micro seconds. NT uses 100 ns units,
	 * so a unit change in TimeAdjustment corresponds
	 * to slewing 10 ppm on a 100 Hz system. Calculate
	 * the number of 100ns units to add, using OS tick
	 * frequency as per suggestion from Harry Pyle,
	 * and leave the remainder in dtemp
	 */
	TimeAdjustment = (long) (dtemp / ppm_per_adjust_unit + (isneg ? -0.5 : 0.5));
	dtemp -= (double) TimeAdjustment * ppm_per_adjust_unit;	


	/*
	 * If a leap second is pending then determine the UTC time stamp 
	 * of when the insertion must take place 
	 */
	if (leap_sec > 0)
	{
		if ( ls_ft.ull == 0 )  /* time stamp has not yet been computed */
		{
			SYSTEMTIME st;

			GetSystemTime(&st);

 			/*
			 * Accept leap announcement only 1 month in advance,
			 * for end of March, June, September, or December.
			 */
			if ( ( st.wMonth % 3 ) == 0 )
			{
				/*
				 * The comparison time stamp is computed according 
				 * to 0:00h UTC of the following day 
				 */
				if ( ++st.wMonth > 12 )
				{
					st.wMonth -= 12;
					st.wYear++;
				}
				
				st.wDay = 1;
				st.wHour = 0;
				st.wMinute = 0;
				st.wSecond = 0;
				st.wMilliseconds = 0;

				SystemTimeToFileTime(&st, &ls_ft.ft);
				msyslog(LOG_NOTICE,
					"Detected positive leap second announcement "
					"for %04d-%02d-%02d %02d:%02d:%02d UTC",
					st.wYear, st.wMonth, st.wDay,
					st.wHour, st.wMinute, st.wSecond);
			}
		}
	}

 
	/*
	 * If the time stamp for the next leap second has been set
	 * then check if the leap second must be handled
	 */
	if ( ls_ft.ull )
	{
		ULONGLONG this_perf_count;

		this_perf_count = perf_ctr();

		if ( ls_time_adjustment == 0 ) /* has not yet been scheduled */
		{
			FT_ULL curr_ft;

	 		GetSystemTimeAsFileTime(&curr_ft.ft);   
			if ( curr_ft.ull >= ls_ft.ull )
			{
				ls_time_adjustment = clockperiod / LS_CORR_INTV_SECS;
				ls_ref_perf_cnt = this_perf_count;
				ls_elapsed = 0;
				msyslog(LOG_NOTICE, "Inserting positive leap second.");
			}
		}
		else  /* leap sec adjustment has been scheduled previously */
		{
			ls_elapsed = ( this_perf_count - ls_ref_perf_cnt ) 
				       * HECTONANOSECONDS / PerfCtrFreq;
		}

		if ( ls_time_adjustment )  /* leap second adjustment is currently active */
		{
			if ( ls_elapsed > ( LS_CORR_INTV - LS_CORR_LIMIT ) )
			{
				ls_time_adjustment = 0;  /* leap second adjustment done */
				ls_ft.ull = 0;
			}

			/* 
			 * NOTE: While the system time is slewed during the leap second 
			 * the interpolation function which is based on the performance 
			 * counter does not account for the slew.
			*/
			TimeAdjustment -= ls_time_adjustment;
		}
	}


	/* only adjust the clock if adjustment changes */
	if (last_Adj != TimeAdjustment) {
		last_Adj = TimeAdjustment;
		DPRINTF(1, ("SetSystemTimeAdjustment(%+ld)\n", TimeAdjustment));
		rc = !SetSystemTimeAdjustment(clockperiod + TimeAdjustment, FALSE);
	}
	else rc = 0;
	if (rc)
	{
		msyslog(LOG_ERR, "Can't adjust time: %m");
		return 0;
	}
	else {
		sys_residual = dtemp / 1e6;
	}

	DPRINTF(6, ("adj_systime: adj %.9f -> remaining residual %.9f\n", 
		    now, sys_residual));

	return 1;
}


void 
init_winnt_time(void)
{
	char szMsgPath[MAX_PATH+1];
	HANDLE hToken = INVALID_HANDLE_VALUE;
	TOKEN_PRIVILEGES tkp;
	TIMECAPS tc;
	BOOL noslew;
	DWORD adjclockperiod;
	LARGE_INTEGER Freq;
	FT_ULL initial_hectonanosecs;
	FT_ULL next_hectonanosecs;

	if (winnt_time_initialized)
		return;

	/*
	 * Make sure the service is initialized
	 * before we do anything else
	 */
	ntservice_init();

	/* Set up the Console Handler */
	if (!SetConsoleCtrlHandler(OnConsoleEvent, TRUE))
	{
		msyslog(LOG_ERR, "Can't set console control handler: %m");
	}

	/* Set the Event-ID message-file name. */
	if (!GetModuleFileName(NULL, szMsgPath, sizeof(szMsgPath))) 
	{
		msyslog(LOG_ERR, "GetModuleFileName(PGM_EXE_FILE) failed: %m\n");
		exit(1);
	}

	/* Initialize random file before OpenSSL checks */
	if (!init_randfile())
		msyslog(LOG_ERR, "Unable to initialize .rnd file\n");

#pragma warning(push)
#pragma warning(disable: 4127) /* conditional expression is constant */

#ifdef DEBUG
	if (SIZEOF_TIME_T != sizeof(time_t)
	    || SIZEOF_INT != sizeof(int)
	    || SIZEOF_SIGNED_CHAR != sizeof(char)) {
	
		msyslog(LOG_ERR, "config.h SIZEOF_* macros wrong, fatal");
		exit(1);
	}
#endif

#pragma warning(pop)

	/*
	 * Get privileges needed for fiddling with the clock
	 */

	/* get the current process token handle */
	if (!OpenProcessToken(GetCurrentProcess(),
	     TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		msyslog(LOG_ERR, "OpenProcessToken failed: %m");
		exit(-1);
	}
	/* get the LUID for system-time privilege. */
	LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;  /* one privilege to set */
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	/* get set-time privilege for this process. */
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
	 	(PTOKEN_PRIVILEGES) NULL, 0);

	/* cannot use return value of AdjustTokenPrivileges. */
	/* (success does not indicate all privileges were set) */
	if (GetLastError() != ERROR_SUCCESS) 
	{
		msyslog(LOG_ERR, "AdjustTokenPrivileges failed: %m");
	 	/* later set time call will probably fail */
	}

	CloseHandle(hToken);
	hToken = INVALID_HANDLE_VALUE;

	/*
	 * ntpd on Windows has always raised its priority, without
	 * requiring -N as on Unix.  Since Windows ntpd doesn't share
	 * the history of unix ntpd of once having no -N and therefore
	 * needing to be invoked under nice, there is no reason to
	 * bring it in line with the Unix version in this regard.
	 * Instsrv assumes ntpd is invoked with no arguments, and
	 * upgrading users would be negatively surprised by the 
	 * poor timekeeping if they failed to add -N as part of 
	 * upgrading were we to correct this platform difference.
	 */
	if (-1 == setpriority(PRIO_PROCESS, 0, NTP_PRIO))
		exit(-1);

	/*
	 * register with libntp ntp_set_tod() to call us back
	 * when time is stepped.
	 */
	step_callback = time_stepped;

	/* 
	 * before we start looking at clock period, do any multimedia
	 * timer manipulation requested via -M option.
	 */
	if (modify_mm_timer) {

		if (timeGetDevCaps(&tc, sizeof(tc)) == TIMERR_NOERROR) {

			wTimerRes = min(max(tc.wPeriodMin, MM_TIMER_INTV), tc.wPeriodMax);
			timeBeginPeriod(wTimerRes);
			atexit(atexit_revert_mm_timer);
			
			msyslog(LOG_INFO, "MM timer resolution: %u..%u msec, set to %u msec",
				tc.wPeriodMin, tc.wPeriodMax, wTimerRes );
		} else
			msyslog(LOG_ERR, "Multimedia timer unavailable");
	}
	
	/* get the performance counter ticks per second */
	if (!QueryPerformanceFrequency(&Freq) || !Freq.QuadPart)
	{
		msyslog(LOG_ERR, "QueryPerformanceFrequency failed: %m\n");
		exit(-1);
	}

	NomPerfCtrFreq = PerfCtrFreq = Freq.QuadPart;
	/*
	 * the cast to LONGLONG is for VC6 compatibility:
	 * nt_clockstuff.c(586) : error C2520: conversion from
	 * unsigned __int64 to double not implemented, use signed 
	 * __int64
	 */
	msyslog(LOG_INFO, 
		"Performance counter frequency %.3f MHz", 
		(LONGLONG)PerfCtrFreq / 1e6);

	/* Determine the existing system time slewing */
	if (!GetSystemTimeAdjustment(&adjclockperiod, &clockperiod, &noslew))
	{
		msyslog(LOG_ERR, "GetSystemTimeAdjustment failed: %m\n");
		exit(-1);
	}

	/*
	 * If there is no slewing before ntpd, adjclockperiod and clockperiod
	 * will be equal.  Any difference is carried into adj_systime's first
	 * pass as the previous adjustment.
	 */
	last_Adj = adjclockperiod - clockperiod;
	
	if (last_Adj)
		msyslog(LOG_INFO, 
			"Clock interrupt period %.3f msec "
			"(startup slew %.1f usec/period)",
			clockperiod / 1e4,
			last_Adj / 10.);
	else
		msyslog(LOG_INFO, 
			"Clock interrupt period %.3f msec", 
			clockperiod / 1e4);

	/*
	 * Calculate the time adjustment resulting from incrementing
	 * units per tick by 1 unit for 1 second 
	 */
	ppm_per_adjust_unit = 1e6 / clockperiod;

	/*
	 * Spin on GetSystemTimeAsFileTime to determine its
	 * granularity.  Prior to Windows Vista this is 
	 * typically the same as the clock period.
	 */
	GetSystemTimeAsFileTime(&initial_hectonanosecs.ft);
	do {
		GetSystemTimeAsFileTime(&next_hectonanosecs.ft);
	} while (initial_hectonanosecs.ull == next_hectonanosecs.ull);

	os_clock_precision = next_hectonanosecs.ull -
		initial_hectonanosecs.ull;

	msyslog(LOG_INFO,
		"Windows clock precision %.3f msec, min. slew %.3f ppm/s",
		(LONGLONG)os_clock_precision / 1e4, 
		ppm_per_adjust_unit);

	winnt_time_initialized = TRUE;

	if (os_clock_precision < 4 * 10000 && !getenv("NTPD_USE_INTERP_DANGEROUS")) {
		msyslog(LOG_INFO, "using Windows clock directly");
	} else {
		winnt_use_interpolation = TRUE;
		get_sys_time_as_filetime = GetInterpTimeAsFileTime;
		StartClockThread();
	}
}


void
atexit_revert_mm_timer(void)
{
	timeEndPeriod(wTimerRes); 
	DPRINTF(1, ("MM timer resolution reset\n"));
}


void 
reset_winnt_time(void)
{
	SYSTEMTIME st;

	/*
	 * If we're in the 2-second slew right after a leap second, 
	 * we don't want to continue that extreme slew, in that case
	 * disable our slewing and return clock discipline to the 
	 * kernel.  Similarly if we are not yet synchronized, 
	 * our current slew may not be a good ongoing trim.
	 * Otherwise, our leave in place the last SetSystemTimeAdjustment
	 * as an ongoing frequency correction, better than nothing.
	 * TODO:
	 * Verify this will not call SetSystemTimeAdjustment if
	 * ntpd is running in ntpdate mode.
	 */
	if (sys_leap == LEAP_NOTINSYNC || ls_time_adjustment)
		SetSystemTimeAdjustment(0, TRUE);	 

	/*
	 * Read the current system time, and write it back to
	 * force CMOS update, only if we are exiting because
	 * the computer is shutting down and we are already
	 * synchronized.
	 */
	 if (ntservice_systemisshuttingdown() && sys_leap != LEAP_NOTINSYNC) {
		GetSystemTime(&st);
		SetSystemTime(&st);
		NLOG(NLOG_SYSEVENT | NLOG_CLOCKINFO)
			msyslog(LOG_NOTICE, "system is shutting down, CMOS time reset.");
	}
}


/*
 * GetSystemTimeAsFileTime() interface clone is used by getclock() in ntpd.
 */

void WINAPI 
GetInterpTimeAsFileTime(
	LPFILETIME pft
	)
{
	FT_ULL now_time;
	FT_ULL now_count;

	/*  Mark a mark ASAP. The latency to here should
	 *  be reasonably deterministic
	*/

	now_count.ull = perf_ctr();
	now_time.ull = interp_time(now_count.ull, TRUE);

	if (last_interp_time > now_time.ull) {

		clock_backward_count++;
		if (last_interp_time - now_time.ull > clock_backward_max)
			clock_backward_max = last_interp_time - now_time.ull;
		now_time.ull = last_interp_time;
	} else
		last_interp_time = now_time.ull;

	*pft = now_time.ft;
	return;
}


/*
 * TimerApcFunction is invoked on the high-priority clock
 * thread to capture a new  baseline system time and
 * performance counter correlation every 43 msec (64Hz 
 * OS clock precision).
 */
static void CALLBACK
TimerApcFunction(
	LPVOID lpArgToCompletionRoutine,
	DWORD dwTimerLowValue,
	DWORD dwTimerHighValue
	)
{
	static BOOL		ctr_freq_timer_started = FALSE;
	static ULONGLONG	prev_count;
	ULONGLONG		now_time;
	FT_ULL			now_count;

	/* Grab the counter first of all */
	now_count.ull = perf_ctr();

	now_time = (((ULONGLONG)dwTimerHighValue << 32) |
				dwTimerLowValue);

	/*
	 * Save this correlation in the history.
	 */
	add_counter_time_pair(now_count.ull, now_time);

	/*
	 * Once we're synchronized start the counter frequency
	 * tuning timer.
	 */
	if (INVALID_HANDLE_VALUE == ctr_freq_timer &&
	    LEAP_NOTINSYNC != sys_leap)

		start_ctr_freq_timer(now_time);
}


unsigned WINAPI 
ClockThread(
	void *arg
	)
{
	LARGE_INTEGER	DueTime;
	HANDLE		timer;
	double		HZ;
	double		TimerHz;
	DWORD		timer_period_msec;
	DWORD		res;
	char		*ntpd_int_int_text;

	UNUSED_ARG(arg);

	timer = CreateWaitableTimer(NULL, FALSE, NULL);

	ntpd_int_int_text = getenv("NTPD_INT_INT");

	HZ = (double)HECTONANOSECONDS / clockperiod;

	if (HZ > 63 && HZ < 65) {
		timer_period_msec = 43;
	} else if (HZ > 98 && HZ < 102) {
		timer_period_msec = 27;
		if (!ntpd_int_int_text)
			msyslog(LOG_WARNING, 
				"%.3f Hz system clock may benefit from "
				"custom NTPD_INT_INT env var timer interval "
				"override between approx. 20 and 50 msecs.",
				HZ);
	} else {
		timer_period_msec = (DWORD)(0.5 + (2.752 * clockperiod / 10000));
		if (!ntpd_int_int_text)
			msyslog(LOG_WARNING, 
				"unfamiliar %.3f Hz system clock may benefit "
				"from custom NTPD_INT_INT env var timer "
				"interval override between approx. 20 and 50 "
				"msecs.",
				HZ);
	}

	if (ntpd_int_int_text) {
		timer_period_msec = atoi(ntpd_int_int_text);
		timer_period_msec = max(9, timer_period_msec);
		msyslog(LOG_NOTICE, 
			"using NTPD_INT_INT env var override %u", 
			timer_period_msec);
	}

	TimerHz = 1e3 / timer_period_msec;
	msyslog(LOG_NOTICE, "HZ %.3f using %u msec timer %.3f Hz %d deep", 
		HZ,
		timer_period_msec,
		TimerHz,
		BASELINES_USED);

	/* negative DueTime means relative to now */
	DueTime.QuadPart = -(int)timer_period_msec;

	SetWaitableTimer(
		timer, 
		&DueTime,		/* first fire */
		timer_period_msec,	/* period thereafter */
		TimerApcFunction,	/* callback routine */
		&timer,			/* context for callback */
		FALSE);			/* do not interfere with power saving */

	/*
	 * The clock thread spends the rest of its life in the TimerApcFunction
	 * and ctr_freq_timer_fired timer APC callbacks, which can only occur 
	 * while this thread is in an alertable wait.  Note the Ex on 
	 * WaitForSingleObjectEx and TRUE for fAlertable.  The wait will return 
	 * after each APC callback in which case we simply wait again.  We will
	 * break out of the loop when StopClockThread signals our exit event.
	 */
	do res = WaitForSingleObjectEx(
			TimerThreadExitRequest, 
			INFINITE, 
			TRUE);
	while (WAIT_OBJECT_0 != res);

	CloseHandle(timer);

	if (ctr_freq_timer != INVALID_HANDLE_VALUE) {
		CloseHandle(ctr_freq_timer);
		ctr_freq_timer = INVALID_HANDLE_VALUE;
	}

	return 0;
}


static void 
StartClockThread(void)
{
	static BOOL done_once = FALSE;
	FT_ULL StartTime;

	/* init variables with the time now */
	GetSystemTimeAsFileTime(&StartTime.ft);
	baseline_times[0] = StartTime.ull;
	baseline_counts[0] = perf_ctr();

	/* init sync objects */
	TimerThreadExitRequest = CreateEvent(NULL, FALSE, FALSE, NULL);

	clock_thread = 
		(HANDLE)_beginthreadex(
			NULL, 
			0, 
			ClockThread, 
			NULL, 
			CREATE_SUSPENDED, 
			&clock_thread_id);

	if (clock_thread != NULL) {
		/* remember the thread priority is only within the process class */
		if (!SetThreadPriority(clock_thread, THREAD_PRIORITY_TIME_CRITICAL)) {
			DPRINTF(1, ("Error setting thread priority\n"));
		}

		lock_thread_to_processor(clock_thread);
		ResumeThread(clock_thread);

		if (FALSE == done_once) {
			done_once = TRUE;
			lock_thread_to_processor(GetCurrentThread());
			atexit( StopClockThread );
		}

		/*
		 * Give the clock thread time to fill its counter/time
		 * sample buffer.  This will underfill the buffer a
		 * bit for sample periods over 43 msec.
		 */
		Sleep(BASELINES_USED * 43);
	}
}


void 
StopClockThread(void)
{
	/*
	 * if the clock thread exit()s this routine
	 * will be called on the clock thread and
	 * we need not (and can't) use the normal
	 * TimerThreadExitRequest event.
	 */
	if (GetCurrentThreadId() != clock_thread_id) {

		if (!SetEvent(TimerThreadExitRequest) ||
		    WaitForSingleObject(clock_thread, 2 * 1000) != 
		    WAIT_OBJECT_0) {
			msyslog(LOG_ERR, "Failed to stop clock thread.");
		}
	}
	CloseHandle(TimerThreadExitRequest);
	TimerThreadExitRequest = NULL;
	CloseHandle(clock_thread);
	clock_thread = NULL;
}


void
lock_thread_to_processor(HANDLE thread)
{
	static	DWORD_PTR	ProcessAffinityMask;
	static	DWORD_PTR	ThreadAffinityMask;
	DWORD_PTR		SystemAffinityMask;
	char			*cputext;
	unsigned int		cpu;

	if ( ! winnt_time_initialized) {
		DPRINTF(1, ("init_winnt_time() must be called before "
				"lock_thread_to_processor(), exiting\n"));
		exit(-1);
	}

	if ( ! winnt_use_interpolation)
		return;
	
	if (-1 == use_pcc) {
		DPRINTF(1, ("perf_ctr is not called before affinity, "
			    "change lock_thread_to_processor to call it\n"));
		exit(-1);
	} else if (0 == use_pcc)
		return;

	/*
	 * Calculate the ThreadAffinityMask we'll use once on the
	 * first invocation.
	 */
	if ( ! ProcessAffinityMask) {

		/*
		 * Choose which processor to nail the main and clock threads to.
		 * If we have more than one, we simply choose the 2nd.
		 * Randomly choosing from 2 to n would be better, but in
		 * either case with clock and network interrupts more likely
		 * to be serviced by the first procecssor, let's stay away 
		 * from it.  QueryPerformanceCounter is not necessarily
		 * consistent across CPUs, hence the need to nail the two
		 * threads involved in QPC-based interpolation to the same
		 * CPU.
		 */

		GetProcessAffinityMask(
			GetCurrentProcess(), 
			&ProcessAffinityMask,
			&SystemAffinityMask);

		/*
		 * respect NTPD_CPU environment variable if present
		 * for testing.  NTPD_CPU=0 means use all CPUs, 1-64
		 * means lock threads involved in interpolation to
		 * that CPU.  Default to 2nd if more than 1.
		 */

		cpu = 2;
		cputext = getenv("NTPD_CPU");
		if (cputext) {
			cpu = (unsigned int) atoi(cputext);
			cpu = min((8 * sizeof(DWORD_PTR)), cpu);
		}

		/* 
		 * Clear all bits except the 2nd.  If we have only one proc
		 * that leaves ThreadAffinityMask zeroed and we won't bother
		 * with SetThreadAffinityMask.
		 */

		ThreadAffinityMask = (0 == cpu) ? 0 : (1 << (cpu - 1));

		if (ThreadAffinityMask && 
			!(ThreadAffinityMask & ProcessAffinityMask)) 

			DPRINTF(1, ("Selected CPU %u (mask %x) is outside "
					"process mask %x, using all CPUs.\n",
					cpu, ThreadAffinityMask, 
					ProcessAffinityMask));
		else
			DPRINTF(1, ("Wiring to processor %u (0 means all) "
					"affinity mask %x\n",	
					cpu, ThreadAffinityMask));

		ThreadAffinityMask &= ProcessAffinityMask;
	}

	if (ThreadAffinityMask && 
	    !SetThreadAffinityMask(thread, ThreadAffinityMask))

		msyslog(LOG_ERR, 
			"Unable to wire thread to mask %x: %m\n", 
			ThreadAffinityMask);
}


#ifdef HAVE_PPSAPI
/*
 * helper routine for serial PPS which returns QueryPerformanceCounter
 * timestamp and needs to interpolate it to an NTP timestamp.
 */
void 
pps_ntp_timestamp_from_counter(
	ntp_fp_t	*result, 
	ULONGLONG	Timestamp, 
	ULONGLONG	Counterstamp
	)
{
	/*
	 * convert between equivalent l_fp and PPSAPI ntp_fp_t
	 */
	ntp_timestamp_from_counter(
		(l_fp *)result,
		Timestamp,
		Counterstamp);
}


void 
ntp_timestamp_from_counter(
	l_fp *result, 
	ULONGLONG Timestamp, 
	ULONGLONG Counterstamp
	)
{
#ifdef DEBUG
	FT_ULL		Now;
#endif
	ULONGLONG	InterpTimestamp;

	if (winnt_use_interpolation) {
		InterpTimestamp = interp_time(Counterstamp + QPC_offset, FALSE);

#ifdef DEBUG
		/* sanity check timestamp is within 1 minute of now */
		GetSystemTimeAsFileTime(&Now.ft);
		Now.ll -= InterpTimestamp;
		if (Now.ll > 60 * HECTONANOSECONDS || 
		    Now.ll < -60 * (LONGLONG) HECTONANOSECONDS) {
			DPRINTF(1, ("ntp_timestamp_from_counter interpolated "
				    "time %.6fs from current\n",
					Now.ll / (double) HECTONANOSECONDS));
			DPRINTF(1, ("interpol time %llx from  %llx\n",
					InterpTimestamp,
					Counterstamp));
			exit(-1);
		}
#endif
	} else {  /* ! winnt_use_interpolation */
		/* have to simply use the driver's system time timestamp */
		InterpTimestamp = Timestamp;
#ifdef DEBUG
		/* sanity check timestamp is within 1 minute of now */
		GetSystemTimeAsFileTime(&Now.ft);
		Now.ll -= InterpTimestamp;
		if (Now.ll > 60 * HECTONANOSECONDS || 
		    Now.ll < -60 * (LONGLONG) HECTONANOSECONDS) {
			DPRINTF(1, ("ntp_timestamp_from_counter serial driver system "
				    "time %.6fs from current\n",
				    Now.ll / (double) HECTONANOSECONDS));
			exit(-1);
		}
#endif
	}

	/* convert from 100ns units to NTP fixed point format */

	InterpTimestamp -= FILETIME_1970;
	result->l_ui = JAN_1970 + (u_int32)(InterpTimestamp / HECTONANOSECONDS);
	result->l_uf = (u_int32)((InterpTimestamp % HECTONANOSECONDS) *
				 (FRAC / HECTONANOSECONDS));
}
#endif  /* HAVE_PPSAPI */


void 
time_stepped(void)
{
	/*
	 * called back by ntp_set_tod after the system
	 * time has been stepped (set).
	 *
	 * We normally prevent the reported time from going backwards
	 * but need to allow it in this case.
	 */
	if (FALSE == winnt_use_interpolation)
		return;


	/*
	 * Restart the clock thread to get a new baseline
	 * time/counter correlation.
	 */
	StopClockThread();

	/*
	 * newest_baseline_gen is a generation counter
	 * incremented once each time newest_baseline
	 * is reset.
	 */
	newest_baseline_gen++;

	last_interp_time = 
		clock_backward_max = 
		clock_backward_count = 
		newest_baseline = 0;

	memset(baseline_counts, 0, sizeof(baseline_counts));
	memset(baseline_times, 0, sizeof(baseline_times));

	StartClockThread();
}


/*
 * log2ull - log base 2 of a unsigned 64-bit number
 */
int 
log2ull(
	ULONGLONG n
	)
{
	const ULONGLONG one = 1;
	int log = 0;

	if (n >= one<<32) { n >>= 32; log += 32; }
	if (n >= one<<16) { n >>= 16; log += 16; }
	if (n >= one<< 8) { n >>=  8; log +=  8; }
	if (n >= one<< 4) { n >>=  4; log +=  4; }
	if (n >= one<< 2) { n >>=  2; log +=  2; }
	if (n >= one<< 1) {	      log +=  1; }

	return (n) ? log : (-1);
}


/*
 * ctr_freq_timer_fired is called once a few seconds before
 * tune_ctr_period seconds have elapsed, to reset the timer
 * and hopefully minimize error due to the system using the
 * nominal performance counter frequency to set the timer
 * internally, which is typically dozens of PPM from the
 * actual performance counter rate.  A few seconds later
 * it is called again to observe the counter and estimate the
 * counter frequency.
 */
static void CALLBACK
ctr_freq_timer_fired(
	LPVOID arg,
	DWORD dwTimeLow,
	DWORD dwTimeHigh
	)
{
	static	FT_ULL		begin_time = {0};
	static	FT_ULL		begin_count = {0};
	static	ULONGLONG	next_period_time = 0;
	static	ULONGLONG	report_systemtime = 0;
	const	ULONGLONG	five_minutes = 5ui64 * 60 * HECTONANOSECONDS;
	FT_ULL			now_time;
	FT_ULL			now_count;

	if (!begin_time.ull) {
		begin_count.ull = perf_ctr();
		begin_time.ft.dwLowDateTime = dwTimeLow;
		begin_time.ft.dwHighDateTime = dwTimeHigh;

		/*
		 * adapt perf ctr observation interval to the
		 * counter frequency
		 */
		tune_ctr_period = 22680 / log2ull(NomPerfCtrFreq);

		/*
		 * reset timer 2s before period ends to minimize
		 * error from OS timer routines using nominal 
		 * performance frequency internally.
		 */
		tune_ctr_freq_max_interval = tune_ctr_period - 2;

		next_period_time = begin_time.ull + 
			(ULONGLONG)tune_ctr_period * HECTONANOSECONDS;

		ROUND_TO_NEXT_SEC_BOTTOM(next_period_time);

		reset_ctr_freq_timer(next_period_time, begin_time.ull);

		return;
	}

	now_time.ft.dwLowDateTime = dwTimeLow;
	now_time.ft.dwHighDateTime = dwTimeHigh;

	if (now_time.ull >= next_period_time) {

		now_count.ull = perf_ctr();
		tune_ctr_freq(
			now_count.ull - begin_count.ull,
			now_time.ull - begin_time.ull);

		next_period_time += (ULONGLONG)tune_ctr_period * HECTONANOSECONDS;
		begin_count.ull = now_count.ull;
		begin_time.ull = now_time.ull;
	}

	/* 
	 * Log clock backward events no more often than 5 minutes.
	 */
	if (!report_systemtime) 

		report_systemtime = now_time.ull + five_minutes;

	else if (report_systemtime <= now_time.ull) {

		report_systemtime +=  five_minutes;

		if (clock_backward_count) {
			msyslog(LOG_WARNING, 
				"clock would have gone backward %d times, "
				"max %.1f usec",
				clock_backward_count, 
				(LONGLONG)clock_backward_max / 10.);

			clock_backward_max = clock_backward_count = 0;
		}
	}

	reset_ctr_freq_timer(next_period_time, now_time.ull);
}


void
reset_ctr_freq_timer_abs(
	ULONGLONG when
	)
{
	FT_ULL	fire_time;

	fire_time.ull = when; 

	SetWaitableTimer(
		ctr_freq_timer,
		&fire_time.li,		/* first fire */
		0,			/* not periodic */
		ctr_freq_timer_fired,	/* callback routine */
		NULL,			/* context for callback */
		FALSE);			/* do not interfere with power saving */
}


void
reset_ctr_freq_timer(
	ULONGLONG when,
	ULONGLONG now
	)
{
	if (when - now > 
	    (tune_ctr_freq_max_interval * HECTONANOSECONDS + HECTONANOSECONDS))

		when = now + tune_ctr_freq_max_interval * HECTONANOSECONDS;

	reset_ctr_freq_timer_abs(when);
}


void
start_ctr_freq_timer(
	ULONGLONG now_time
	)
{
	ULONGLONG when;

	ctr_freq_timer = CreateWaitableTimer(NULL, FALSE, NULL);

	when = now_time;
	ROUND_TO_NEXT_SEC_BOTTOM(when);

	reset_ctr_freq_timer_abs(when);
}


/*
 * tune_ctr_freq is called once per tune_ctr_period seconds
 * with a counter difference and time difference.
 */
void 
tune_ctr_freq(
	LONGLONG ctr_delta,
	LONGLONG time_delta
	)
{
	static unsigned count = 0;
	static unsigned dispcount = 0;
	static unsigned report_at_count = 0;
	static int disbelieved = 0;
	static int i = 0;
	static double nom_freq = 0;
	static LONGLONG diffs[TUNE_CTR_DEPTH] = {0};
	static LONGLONG sum = 0;

	LONGLONG delta;
	LONGLONG deltadiff;
	ULONGLONG ObsPerfCtrFreq;
	double obs_freq;
	double this_freq;
	int isneg;

	/* one-time initialization */
	if (!report_at_count) {
		report_at_count = 24 * 60 * 60 / tune_ctr_period;
		nom_freq = (LONGLONG)NomPerfCtrFreq / 1e6;
	}

	/* delta is the per-second observed frequency this time */
	delta = (LONGLONG)((double)ctr_delta * HECTONANOSECONDS /
			   time_delta);

	/* disbelieve any delta more than +/- 976 PPM from nominal */
	deltadiff = delta - NomPerfCtrFreq;
	if (0 > deltadiff) {
		isneg = 1;
		deltadiff = -deltadiff;
	} else
		isneg = 0;

	if ((ULONGLONG)deltadiff > (NomPerfCtrFreq / 1024)) {
		disbelieved++;
		dispcount++;
#ifdef DEBUG
		msyslog(LOG_DEBUG, "ctr delta %s%lld exceeds limit %llu",
				   (isneg) ? "-" : "",
				   deltadiff,
				   NomPerfCtrFreq / 1024);
#endif
	} else {

		/*
		 * collect average over TUNE_CTR_DEPTH samples
		 * for our PerfCtrFreq trimming.
		 */

		if (isneg)
			deltadiff = -deltadiff;
		sum -= diffs[i];
		diffs[i] = deltadiff;
		sum += deltadiff;
		i = (i + 1) % COUNTOF(diffs);
		count++;
		dispcount++;
	}

	this_freq = delta / 1e6;

	ObsPerfCtrFreq = NomPerfCtrFreq + (sum / COUNTOF(diffs));

#if 1	/* #if 0 to disable changing freq used */
	/* get rid of ObsPerfCtrFreq when removing the #ifdef */
	PerfCtrFreq = ObsPerfCtrFreq;
#endif
	obs_freq = (LONGLONG)ObsPerfCtrFreq / 1e6;

	/* 
	 * report observed ctr freq each time the estimate used during
	 * startup moves toward the observed freq from the nominal, 
	 * and once a day afterward.
	 */

	if (count > COUNTOF(diffs) &&
	    /* (count % COUNTOF(diffs)) && */	/* enables reporting each */
	    dispcount < report_at_count)	/* TUNE_CTR_DEPTH samples */
		return;

	NLOG(NLOG_CLOCKINFO)
		if (count <= COUNTOF(diffs))
			/* moving to observed freq. from nominal (startup) */
			msyslog(LOG_INFO,
				(obs_freq > 100)
				 ? "ctr %.3f MHz %+6.2f PPM using "
				       "%.3f MHz %+6.2f PPM"
				 : "ctr %.6f MHz %+6.2f PPM using "
				       "%.6f MHz %+6.2f PPM",
				this_freq,
				1e6 * (this_freq - nom_freq) / nom_freq,
				obs_freq, 
				1e6 * (obs_freq - nom_freq) / nom_freq);
		else
			/* steady state */
			msyslog(LOG_INFO,
				(obs_freq > 100)
				 ? "ctr %.3f MHz %+.2f PPM"
				 : "ctr %.6f MHz %+.2f PPM",
				obs_freq, 
				1e6 * (obs_freq - nom_freq) / nom_freq);

	if (disbelieved) {
		msyslog(LOG_ERR, 
			"%d ctr samples exceed +/- 976 PPM range gate",
			disbelieved);
		disbelieved = 0;
	}

	dispcount = 0;
}


/*
 * add_counter_time_pair is called by the
 * high priority clock thread with each new
 * baseline counter/time correlation.
 */
void
add_counter_time_pair(
	ULONGLONG ctr,
	LONGLONG time
	)
{
	int i;

	i = (newest_baseline + 1) % BASELINES_TOT;

	baseline_counts[i] = ctr;
	baseline_times[i] = time;

	newest_baseline = i;
}


/*
 * interp_time estimates NT time in 100ns units
 * based on a performance counter value given.
 * This must tolerate recent historical counters
 * as well as current.  When current is FALSE
 * we can't assume ctr is the latest/highest
 * seen.
 */
ULONGLONG
interp_time(
	ULONGLONG ctr,
	BOOL current
	)
{
	static __declspec(thread) int		last_newest = -1;
	static __declspec(thread) int		last_newest_gen;
	static __declspec(thread) int		best_index;
	ULONGLONG	this_ctr;
	LONGLONG	this_time;
	LONGLONG	latest_time;
	LONGLONG	ctr_diff;
	int		i;
	int		i_gen;
	int		c;

	/*
	 * Use the system time (roughly synchronised to the tick, and
	 * extrapolated using the system performance counter.
	 *
	 * Cache the results per thread and only repeat the
	 * calculation when new data has arrived.
	 */
	i = newest_baseline;
	i_gen = newest_baseline_gen;

	if (last_newest == i && last_newest_gen == i_gen) {
		this_time = baseline_times[best_index];
		ctr_diff = ctr - baseline_counts[best_index];
		this_time += (LONGLONG)PERF2HNS((double)ctr_diff);

		return this_time;
	}

	last_newest = i;
	last_newest_gen = i_gen;

	latest_time = 0;

	/*
	 * Run through the history calculating the interpolated 
	 * time based on each counter/time correlation in turn,
	 * and believe the latest one.  This is akin to the NTP
	 * protocol minimum delay clock filter.  Errors due to 
	 * counter/time correlations with stale time are all 
	 * negative.
	 */
	for (c = 0; c < BASELINES_USED; c++) {
		 if (baseline_times[i]) {
			this_time = baseline_times[i];
			this_ctr = baseline_counts[i];

			ctr_diff = ctr - this_ctr;

			if (current && ctr_diff < 0) {
				/* 
				 * The performance counter apparently went 
				 * backwards without rolling over.  It might 
				 * be nice to complain but we don't want 
				 * to do it repeatedly.
				 */
				ctr_diff = 0;
			}

			this_time += (LONGLONG)PERF2HNS((double)ctr_diff);

			if (this_time > latest_time) {
				latest_time = this_time;
				best_index = i;
			}
		}
		i = i ? (i - 1) : (BASELINES_TOT - 1);
	}

	return latest_time;
}
