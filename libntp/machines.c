/* machines.c - provide special support for peculiar architectures
 *
 * Real bummers unite !
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ntp_machine.h"
#include "ntp_syslog.h"
#include "ntp_stdlib.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef SYS_WINNT

#ifdef SYS_VXWORKS
#include "taskLib.h"
#include "sysLib.h"
#include "time.h"
#include "ntp_syslog.h"

/*	some translations to the world of vxWorkings -casey */
/* first some netdb type things */
#include "ioLib.h"
#include <socket.h>
int h_errno;

struct hostent *gethostbyname(char *name)
	{
	struct hostent *host1;
	h_errno = 0;					/* we are always successful!!! */
	host1 = (struct hostent *) malloc (sizeof(struct hostent));
	host1->h_name = name;
	host1->h_addrtype = AF_INET;
	host1->h_aliases = name;
	host1->h_length = 4;
	host1->h_addr_list[0] = (char *)hostGetByName (name);
	host1->h_addr_list[1] = NULL;
	return host1;
	}

struct hostent *gethostbyaddr(char *name, int size, int addr_type)
	{
	struct hostent *host1;
	h_errno = 0;  /* we are always successful!!! */
	host1 = (struct hostent *) malloc (sizeof(struct hostent));
	host1->h_name = name;
	host1->h_addrtype = AF_INET;
	host1->h_aliases = name;
	host1->h_length = 4;
	host1->h_addr_list = NULL;
	return host1;
	}

struct servent *getservbyname (char *name, char *type)
	{
	struct servent *serv1;
	serv1 = (struct servent *) malloc (sizeof(struct servent));
	serv1->s_name = "ntp";      /* official service name */
	serv1->s_aliases = NULL;	/* alias list */
	serv1->s_port = 123;		/* port # */
	serv1->s_proto = "udp";     /* protocol to use */
	return serv1;
	}

/* second
 * vxworks thinks it has insomnia
 * we have to sleep for number of seconds
 */

#define CLKRATE 	sysClkRateGet()

/* I am not sure how valid the granularity is - it is from G. Eger's port */
#define CLK_GRANULARITY  1		/* Granularity of system clock in usec	*/
								/* Used to round down # usecs/tick		*/
								/* On a VCOM-100, PIT gets 8 MHz clk,	*/
								/*	& it prescales by 32, thus 4 usec	*/
								/* on mv167, granularity is 1usec anyway*/
								/* To defeat rounding, set to 1 		*/
#define USECS_PER_SEC		1000000L		/* Microseconds per second	*/
#define TICK (((USECS_PER_SEC / CLKRATE) / CLK_GRANULARITY) * CLK_GRANULARITY)

/* emulate unix sleep
 * casey
 */
void sleep(int seconds)
	{
	taskDelay(seconds*TICK);
	}
/* emulate unix alarm
 * that pauses and calls SIGALRM after the seconds are up...
 * so ... taskDelay() fudged for seconds should amount to the same thing.
 * casey
 */
void alarm (int seconds)
	{
	sleep(seconds);
	}

#endif /* SYS_VXWORKS */

#ifdef SYS_PTX			/* Does PTX still need this? */
/*#include <sys/types.h>	*/
#include <sys/procstats.h>

int
gettimeofday(
	struct timeval *tvp
	)
{
	/*
	 * hi, this is Sequents sneak path to get to a clock
	 * this is also the most logical syscall for such a function
	 */
	return (get_process_stats(tvp, PS_SELF, (struct procstats *) 0,
				  (struct procstats *) 0));
}
#endif /* SYS_PTX */

const char *set_tod_using = "UNKNOWN";

int
ntp_set_tod(
	struct timeval *tvp,
	void *tzp
	)
{
	int rc;

#ifdef HAVE_CLOCK_SETTIME
	{
		struct timespec ts;

		/* Convert timeval to timespec */
		ts.tv_sec = tvp->tv_sec;
		ts.tv_nsec = 1000 *  tvp->tv_usec;

		rc = clock_settime(CLOCK_REALTIME, &ts);
		if (!rc)
		{
			set_tod_using = "clock_settime";
			return rc;
		}
	}
#endif /* HAVE_CLOCK_SETTIME */
#ifdef HAVE_SETTIMEOFDAY
	{
		rc = settimeofday(tvp, tzp);
		if (!rc)
		{
			set_tod_using = "settimeofday";
			return rc;
		}
	}
#endif /* HAVE_SETTIMEOFDAY */
#ifdef HAVE_STIME
	{
		rc = stime(&tvp->tv_sec); /* lie as bad as SysVR4 */
		if (!rc)
		{
			set_tod_using = "stime";
			return rc;
		}
	}
#endif /* HAVE_STIME */
	set_tod_using = "Failed!";
	return -1;
}

#ifdef SYS_CYGWIN32
#include <stdio.h>
#include <time.h>
int
settimeofday_NT(
	struct timeval *tv
	)
{
	SYSTEMTIME st;
	struct tm *gmtm;
	long x = tv->tv_sec;
	long y = tv->tv_usec;
	printf("NT Set: %d %d\n",x,y);
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
#endif

#else /* SYS_WINNT */


#include <time.h>
#include <sys\timeb.h>
#include "ntp_syslog.h"

char *	set_tod_using = "SetSystemTime";

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

static LONGLONG PerfFrequency = 0;
static LONGLONG LastTimerCount = 0;
static ULONGLONG LastTimerTime = 0;
static CRITICAL_SECTION TimerCritialSection; /* lock for LastTimerCount & LastTimerTime */


int
gettimeofday(
	struct timeval *tv
	)
{
	/*  Use the system time (roughly synchronised to the tick, and
	 *  extrapolated using the system performance counter.
	 */

	ULONGLONG Count;
	ULONGLONG Time;
	LARGE_INTEGER LargeIntNowCount;
	ULONGLONG NowCount;
	ULONGLONG TicksElapsed;

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
	{
		Count = LastTimerCount;
		Time = LastTimerTime;
	}
	LeaveCriticalSection(&TimerCritialSection);

	printf ("Count  %I64d\n", Count);

	/*  Caclulate when now is.
	 *  
	 *  Result = LastTimerTime +  (NowCount - LastTimerCount) / PerfFrequency
	 */
	if (NowCount >= Count) {
	  TicksElapsed = NowCount - Count; /* linear progression of ticks */
	}
	else {
	  TicksElapsed = NowCount + 1 + ~Count; /* tick counter has wrapped around - I don't think this will ever happen*/
	}

	/*  Calculate the new time (in 100's of nano-seconds)
	 */
        Time += ((TicksElapsed * HECTONANOSECONDS) / PerfFrequency);


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
	ULARGE_INTEGER Time;
	(void) lpArgToCompletionRoutine; /* not used */

	if (QueryPerformanceCounter(&LargeIntNowCount)) {

		/*  Fill in the data
		 */
		Time.u.LowPart = dwTimerLowValue;
		Time.u.HighPart = dwTimerHighValue;

		EnterCriticalSection(&TimerCritialSection); 
		{
			LastTimerCount = LargeIntNowCount.QuadPart;
			LastTimerTime = Time.QuadPart;
		}
		LeaveCriticalSection(&TimerCritialSection);
	}
}




static HANDLE ClockThreadHandle = NULL;
static HANDLE TimerThreadExitRequest = NULL;

DWORD WINAPI ClockThread(void *arg)
{
	LARGE_INTEGER DueTime;
	HANDLE WaitableTimerHandle = CreateWaitableTimer(NULL, FALSE, NULL);

	(void) arg; /* not used */

	if (WaitableTimerHandle != NULL) {
		DueTime.QuadPart = 0i64;
		if (SetWaitableTimer(WaitableTimerHandle, &DueTime, 10L /* ms */, TimerApcFunction, &WaitableTimerHandle, FALSE) != NO_ERROR) {
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



#endif /* SYS_WINNT */

#if defined (SYS_WINNT) || defined (SYS_VXWORKS)
/* getpass is used in ntpq.c and ntpdc.c */

char *
getpass(const char * prompt)
{
	int c, i;
	static char password[32];

	fprintf(stderr, "%s", prompt);
	fflush(stderr);
	for (i=0; i<sizeof(password)-1 && ((c=_getch())!='\n'); i++) {
		password[i] = c;
	}
	password[i] = '\0';

	return password;
}

#endif /* SYS_WINNT */

#if !defined(HAVE_MEMSET)
void
ntp_memset(
	char *a,
	int x,
	int c
	)
{
	while (c-- > 0)
		*a++ = x;
}
#endif /*POSIX*/
