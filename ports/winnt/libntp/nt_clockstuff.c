

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ntp_machine.h"
#include "ntp_syslog.h"

#include <time.h>
#include <sys\timeb.h>
#include "ntp_syslog.h"

char *	set_tod_using = "SetSystemTime";
extern double drift_comp;

/* Windows NT versions of gettimeofday and settimeofday
 *
 * ftime() has internal DayLightSavings related BUGS
 * therefore switched to GetSystemTimeAsFileTime()
 */

/* 100ns intervals between 1/1/1601 and 1/1/1970 as reported by
 * SystemTimeToFileTime()
 */

#define FILETIME_1970     0x019db1ded53e8000
#define HECTONANOSECONDS  10000000ui64

LONGLONG PerfFrequency = 0;
static LONGLONG LastTimerCount = 0;
static ULONGLONG LastTimerTime = 0;
static CRITICAL_SECTION TimerCritialSection; /* lock for LastTimerCount & LastTimerTime */
static HANDLE ClockThreadHandle = NULL;
static HANDLE TimerThreadExitRequest = NULL;
LONGLONG sys_ticks = 0; /* Diagnostics */

extern int debug;
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
	if (NowCount >= Count) {
	  TicksElapsed = NowCount - Count; /* linear progression of ticks */
	}
	else {
	  TicksElapsed = NowCount + 1 + ~Count; /* tick counter has wrapped around - I don't think this will ever happen*/
		msyslog(LOG_INFO, "Wraparound %m");

	}

	if (TicksElapsed < 0) {
		TicksElapsed = 5000; // go for the middle for now
		msyslog(LOG_INFO, "Negtix %m");

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
	(void) lpArgToCompletionRoutine; /* not used */
	sys_ticks++;
	if (TryEnterCriticalSection(&TimerCritialSection)) {
		  QueryPerformanceCounter((LARGE_INTEGER *) &LastTimerCount);
		  LastTimerTime = ((ULONGLONG) dwTimerHighValue << 32) +
			  (ULONGLONG) dwTimerLowValue;
  		  LeaveCriticalSection(&TimerCritialSection);
	}
}


DWORD WINAPI ClockThread(void *arg)
{
	LARGE_INTEGER DueTime;
	HANDLE WaitableTimerHandle = CreateWaitableTimer(NULL, FALSE, NULL);

	(void) arg; /* not used */
	/* Reset the Clock to a reasonable increment */
	SetSystemTimeAdjustment(PRESET_TICK, FALSE);

	if (WaitableTimerHandle != NULL) {
		DueTime.QuadPart = 0i64;
		if (SetWaitableTimer(WaitableTimerHandle, &DueTime, 5L /* ms */, TimerApcFunction, &WaitableTimerHandle, FALSE) != NO_ERROR) {
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
    if (QueryPerformanceFrequency(&Freq)) { 
		PerfFrequency = Freq.QuadPart;
	}

	/* init variables with the time now */
	GetSystemTimeAsFileTime(&StartTime);
	LastTimerTime = (((ULONGLONG) StartTime.dwHighDateTime) << 32) + (ULONGLONG) StartTime.dwLowDateTime;

	/* init sync objects */
	InitializeCriticalSection(&TimerCritialSection);
	TimerThreadExitRequest = CreateEvent(NULL, FALSE, FALSE, NULL);
	ClockThreadHandle = CreateThread(NULL, 0, ClockThread, NULL, 0, &tid);
	if (ClockThreadHandle != NULL) {
		/* remober the thread priority is only within the process class */
		  if (!SetThreadPriority(ClockThreadHandle, THREAD_PRIORITY_TIME_CRITICAL)) {
			printf("Error setting thread priority\n");
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
	} 
}

typedef void (__cdecl *CRuntimeFunction)(void);

#pragma data_seg(".CRT$XIY")
	CRuntimeFunction _StartClockThread = StartClockThread;
#pragma data_seg(".CRT$XTY")
	CRuntimeFunction _StopClockThread = StopClockThread;
#pragma data_seg()  /* reset */


int
ntp_set_tod(
	struct timeval *tv,
	void *tzp
	)
{
	SYSTEMTIME st;
	struct tm *gmtm;
	long x = tv->tv_sec;
	long y = tv->tv_usec;
	(void) tzp;

	gmtm = gmtime((const time_t *) &x);
	st.wSecond		= (WORD) gmtm->tm_sec;
	st.wMinute		= (WORD) gmtm->tm_min;
	st.wHour		= (WORD) gmtm->tm_hour;
	st.wDay 		= (WORD) gmtm->tm_mday;
	st.wMonth		= (WORD) (gmtm->tm_mon	+ 1);
	st.wYear		= (WORD) (gmtm->tm_year + 1900);
	st.wDayOfWeek		= (WORD) gmtm->tm_wday;
	st.wMilliseconds	= (WORD) (y / 1000);

	if (!SetSystemTime(&st)) {
		msyslog(LOG_ERR, "SetSystemTime failed: %m\n");
		return -1;
	}
	return 0;
}

