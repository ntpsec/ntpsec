/* Windows NT Clock Routines
 *
 *
 * Revision History:
 * $Log: <Not implemented> $
 * Revision 1.9  2000/11/19 09:02:12  dietrich
 * From: Ron Thornton [rthornto@pictel.com]
 * Sent: Thu 11/16/00 8:51 AM
 * On Windows 2000 it requires a privilege on the current process token
 * that is disabled by default on Windows 2000.
 *
 * I set the token by adding the following code at the beginning of the
 * init_winnt_time() function in nt_clockstuff.c.
 *
 * Revision 1.8  2000/11/19 08:03:20  dietrich
 * From: "Colin Dancer" <colin.dancer@pyrochrome.net>
 * To: <bugs@ntp.org>
 * Sent: 10 November 2000 12:59
 * Subject: NT bug in NTP 4.0.99j
 *
 * I've found a bug in (and produced a fix for) the NT clock interpolation
 * code in NTP 4.0.99j.
 *
 * The symptoms of the problem are that gettimeofday() function on NT
 * can be wrong by hundreds of seconds if, while a gettimeofday() call
 * is being processed, an APC completes after the query of the performance
 * counter but before the lock is grabbed.  The most obvious fix is to move
 * the lock to include the querying of the performance counter, but this
 * could affect the predictability of the timestamp so I have instead
 * tweaked the code to detect and sidestep the duff calculation.
 *
 * I've also found that on a loaded system the execution of the APC can be
 * delayed, leading to errors of upto 10ms.  There is no easy fix to this,
 * but I do have code for an alternative interpolation scheme which avoids
 * the problem on single processor systems. I'm currently integrating this
 * along with code for deciding which algorithm to use based on whether
 * the system is SP or MP.
 *
 * Created by Sven Dietrich  sven@inter-yacht.com
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ntp_stdlib.h"
#include "clockstuff.h"
#include "ntp_timer.h"

extern double sys_residual;	/* residual from previous adjustment */

static long last_Adj = 0;

static CRITICAL_SECTION TimerCritialSection; /* lock for LastTimerCount & LastTimerTime */

static ULONGLONG RollOverCount = 0;
static ULONGLONG LastTimerCount = 0;
static ULONGLONG LastTimerTime = 0;

static HANDLE ClockThreadHandle = NULL;
static HANDLE TimerThreadExitRequest = NULL;

static DWORD every = 0;
static DWORD initial_units_per_tick = 0;
static DWORD lastLowTimer = 0;

ULONGLONG PerfFrequency = 0;
static DWORD units_per_tick = 0;
static DOUBLE ppm_per_adjust_unit = 0.0;

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
   long dwTimeAdjustment;

	/*
	 * Add the residual from the previous adjustment to the new
	 * adjustment, bound and round.
	 */
	dtemp = sys_residual + now;
	sys_residual = 0;
	if (dtemp < 0) {
		isneg = 1;
		dtemp = -dtemp;
	}

	if (dtemp > NTP_MAXFREQ)
		dtemp = NTP_MAXFREQ;

	dtemp = dtemp * 1e6;

	if (isneg)
		dtemp = -dtemp;

	/* dtemp is in micro seconds. NT uses 100 ns units,
	 * so a unit change in dwTimeAdjustment corresponds
	 * to slewing 10 ppm on a 100 Hz system.
	 * Calculate the number of 100ns units to add,
	 * using OS tick frequency as per suggestion from Harry Pyle,
	 * and leave the remainder in dtemp */
	dwTimeAdjustment = (DWORD)( dtemp / ppm_per_adjust_unit + (isneg ? -0.5 : 0.5)) ;
	dtemp += (double) -dwTimeAdjustment * ppm_per_adjust_unit;	

	/* only adjust the clock if adjustment changes */
	if (last_Adj != dwTimeAdjustment) { 	
			last_Adj = dwTimeAdjustment;
# ifdef DEBUG
		if (debug > 1)
			printf("SetSystemTimeAdjustment( %ld) + (%ld)\n", dwTimeAdjustment, units_per_tick);			
# endif
			dwTimeAdjustment += units_per_tick;
			rc = !SetSystemTimeAdjustment(dwTimeAdjustment, FALSE);
	}
	else rc = 0;
	if (rc)
	{
		msyslog(LOG_ERR, "Can't adjust time: %m");
		return 0;
	}
	else {
		sys_residual = dtemp / 1000000.0;
	}

#ifdef DEBUG
	if (debug > 6)
		printf("adj_systime: adj %.9f -> remaining residual %.9f\n", now, sys_residual);
#endif
	return 1;
}


void init_winnt_time(void) {
	BOOL noslew;
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	/*
	 * Get privileges needed for fiddling with the clock
	  */

	  /* get the current process token handle */
	if (!OpenProcessToken(GetCurrentProcess(),
	  TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
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

	/* Reset the Clock to a reasonable increment */
	if (!GetSystemTimeAdjustment(&initial_units_per_tick, &every,&noslew))
		{
		msyslog(LOG_ERR, "GetSystemTimeAdjustment failed: %m\n");
		exit (-1);
		}

	units_per_tick = initial_units_per_tick;

	/* Calculate the time adjustment resulting from incrementing
	 * units per tick by 1 unit for 1 second */
	ppm_per_adjust_unit = 1000000.0 / (double) every;

#ifdef DEBUG
	msyslog(LOG_INFO, "Initial Clock increment %7.1f us",
			(float) (units_per_tick / 10));
	msyslog(LOG_INFO, "Adjustment rate %5.3f ppm/s", ppm_per_adjust_unit);
#endif

    /*++++ Gerhard Junker
     * see Platform SDK for QueryPerformanceCounter
     * On a multiprocessor machine, it should not matter which processor is called. 
     * However, you can get different results on different processors due to bugs in the BIOS or the HAL. 
     * To specify processor affinity for a thread, use the SetThreadAffinityMask function. 
     * ... we will hope, the apc routine will run on the same processor
     */

    SetThreadAffinityMask(GetCurrentThread(), 1L);

    /*---- Gerhard Junker */

}


void reset_winnt_time(void) {

	/* restore the clock frequency back to its original value */
	if (!SetSystemTimeAdjustment(initial_units_per_tick, FALSE)) {
		msyslog(LOG_ERR, "Failed to reset clock frequency, SetSystemTimeAdjustment(): %m");
	}
	if (!SetSystemTimeAdjustment(0, TRUE)) {
		msyslog(LOG_ERR, "Failed to reset clock state, SetSystemTimeAdjustment(): %m");
	}
}



int
gettimeofday(
	struct timeval *tv
	)
{
	/*  Use the system time (roughly synchronised to the tick, and
	 *  extrapolated using the system performance counter.
	 */

	ULONGLONG Count;
	LARGE_INTEGER LargeIntNowCount;
	ULONGLONG Time;
	ULONGLONG NowCount;
   ULONGLONG PreCount;                                                  /*FIX*/
	LONGLONG TicksElapsed;
	LONG time_adjustment;

	/*  Mark a mark ASAP. The latency to here should
	 *  be reasonably deterministic
	 */

   PreCount = LastTimerCount;                                           /*FIX*/

	if (!QueryPerformanceCounter(&LargeIntNowCount)) {
		msyslog(LOG_ERR, "QueryPeformanceCounter failed: %m");
		exit(1);
	}

	NowCount = LargeIntNowCount.QuadPart;

	/*  Get base time we are going to extrapolate from
	 */	
	EnterCriticalSection(&TimerCritialSection);
		Count = LastTimerCount;
		Time = LastTimerTime;
	LeaveCriticalSection(&TimerCritialSection);

	/*  Calculate when now is.
	 *
	 *  Result = LastTimerTime +  (NowCount - LastTimerCount) / PerfFrequency
	 */

	if (NowCount >= Count)
   {
	    TicksElapsed = NowCount - Count; /* linear progression of ticks */
   }
	else
   {
     /************************************************************************/
     /* Differentiate between real rollover and the case of taking a         */
     /* perfcount then the APC coming in.                                    */
     /************************************************************************/
     if (Count > PreCount)                                              /*FIX*/
     {                                                                  /*FIX*/
       TicksElapsed = 0;                                                /*FIX*/
     }                                                                  /*FIX*/
     else                                                               /*FIX*/
     {                                                                  /*FIX*/
       TicksElapsed = NowCount + (RollOverCount - Count);               /*FIX*/
     }                                                                  /*FIX*/
   }

	/*  Calculate the new time (in 100's of nano-seconds)
	 */
    time_adjustment = (long) ((TicksElapsed * HECTONANOSECONDS) / PerfFrequency);
	Time += time_adjustment;

	/* Convert the hecto-nano second time to tv format
	 */
	Time -= FILETIME_1970;
	tv->tv_sec = (LONG) ( Time / 10000000ui64);
	tv->tv_usec = (LONG) (( Time % 10000000ui64) / 10);

	return 0;
}

static void CALLBACK
TimerApcFunction(
	LPVOID lpArgToCompletionRoutine,
	DWORD dwTimerLowValue,
	DWORD dwTimerHighValue
	)
{
	LARGE_INTEGER LargeIntNowCount;
	(void) lpArgToCompletionRoutine; /* not used */

	if (dwTimerLowValue == lastLowTimer) return;
	
	/* Grab the counter first of all */
	QueryPerformanceCounter(&LargeIntNowCount);

	/* Save this for next time */
	lastLowTimer = dwTimerLowValue;

	/* Check to see if the counter has rolled. This happens
	   more often on Multi-CPU systems */

	if ((ULONGLONG) LargeIntNowCount.QuadPart < LastTimerCount) {
		/* Counter Rolled - try and estimate the rollover point using
		  the nominal counter frequency divided by an estimate of the
		  OS frequency */
		RollOverCount = LastTimerCount + PerfFrequency * every /  HECTONANOSECONDS -
			(ULONGLONG) LargeIntNowCount.QuadPart;
#ifdef DEBUG
		msyslog(LOG_INFO,
			"Performance Counter Rollover %I64u:\rLast Timer Count %I64u\rCurrent Count %I64u",
				RollOverCount, LastTimerCount, LargeIntNowCount.QuadPart);
#endif
	}

	/* Now we can hang out and wait for the critical section to free up;
	   we will get the CPU this timeslice. Meanwhile other tasks can use
	   the last value of LastTimerCount */
		
	EnterCriticalSection(&TimerCritialSection);
	LastTimerCount = (ULONGLONG) LargeIntNowCount.QuadPart;
	LastTimerTime = ((ULONGLONG) dwTimerHighValue << 32) +
				(ULONGLONG) dwTimerLowValue;
	LeaveCriticalSection(&TimerCritialSection);
}



DWORD WINAPI ClockThread(void *arg)
{

	LARGE_INTEGER DueTime;
	HANDLE WaitableTimerHandle = CreateWaitableTimer(NULL, FALSE, NULL);

	(void) arg; /* not used */

	if (WaitableTimerHandle != NULL) {
		DueTime.QuadPart = 0i64;
		if (SetWaitableTimer(WaitableTimerHandle, &DueTime, 1L /* ms */, TimerApcFunction, &WaitableTimerHandle, FALSE) != NO_ERROR) {
			for(;;) {
				if (WaitForSingleObjectEx(TimerThreadExitRequest, INFINITE, TRUE) == WAIT_OBJECT_0) {
					break; /* we've been asked to exit */
				}
			}
		}
		CloseHandle(WaitableTimerHandle);
		WaitableTimerHandle = NULL;
	}
	return 0;
}


static void StartClockThread(void)
{
	DWORD tid;
	FILETIME StartTime;
	LARGE_INTEGER Freq = { 0, 0 };
	
	/* get the performance counter freq*/
    if (!QueryPerformanceFrequency(&Freq)) {
		msyslog(LOG_ERR, "QueryPerformanceFrequency failed: %m\n");
		exit (-1);
	}

	PerfFrequency = Freq.QuadPart;

	/* init variables with the time now */
	GetSystemTimeAsFileTime(&StartTime);
	LastTimerTime = (((ULONGLONG) StartTime.dwHighDateTime) << 32) +
		(ULONGLONG) StartTime.dwLowDateTime;

	/* init sync objects */
	InitializeCriticalSection(&TimerCritialSection);
	TimerThreadExitRequest = CreateEvent(NULL, FALSE, FALSE, NULL);

	ClockThreadHandle = CreateThread(NULL, 0, ClockThread, NULL, 0, &tid);
	if (ClockThreadHandle != NULL) {
		/* remember the thread priority is only within the process class */
		  if (!SetThreadPriority(ClockThreadHandle, THREAD_PRIORITY_TIME_CRITICAL)) {
#ifdef DEBUG
			printf("Error setting thread priority\n");
#endif
		  }
	}
}

static void StopClockThread(void)
{	
	if (SetEvent(TimerThreadExitRequest) &&
	    WaitForSingleObject(ClockThreadHandle, 10000L) == 0) {
		CloseHandle(TimerThreadExitRequest);
		TimerThreadExitRequest = NULL;

		CloseHandle(ClockThreadHandle);
		ClockThreadHandle = NULL;

		DeleteCriticalSection(&TimerCritialSection);
		msyslog(LOG_INFO, "The Network Time Protocol Service has stopped.");
	}
	else
		msyslog(LOG_ERR, "Network Time Protocol Service Failed to Stop");
}

typedef void (__cdecl *CRuntimeFunction)(void);

#pragma data_seg(".CRT$XIY")
	CRuntimeFunction _StartClockThread = StartClockThread;
#pragma data_seg(".CRT$XTY")
	CRuntimeFunction _StopClockThread = StopClockThread;
#pragma data_seg()  /* reset */
