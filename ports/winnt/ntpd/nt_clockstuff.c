#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clockstuff.h"

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

DWORD units_per_tick = 0;

void init_winnt_time(void) {
	BOOL noslew;

		/* Reset the Clock to a reasonable increment */
	if (!GetSystemTimeAdjustment(&initial_units_per_tick, &every, &noslew))	{
		msyslog(LOG_ERR, "GetSystemTimeAdjustment failed: %m\n");
		exit (-1);
	}

	units_per_tick = initial_units_per_tick;

#ifdef DEBUG
	msyslog(LOG_INFO, "Initial Clock increment %7.1f us\n",
			(float) (units_per_tick / 10));
#endif

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
	LONGLONG TicksElapsed;
	LONG time_adjustment;

	/*  Mark a mark ASAP. The latency to here should 
	 *  be reasonably deterministic
	 */
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
	  TicksElapsed = NowCount - Count; /* linear progression of ticks */
	else 
		TicksElapsed = NowCount + (RollOverCount - Count);

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
	lastLowTimer = dwTimerLowValue;
	
	/* Grab the counter first of all */
	QueryPerformanceCounter(&LargeIntNowCount);

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
