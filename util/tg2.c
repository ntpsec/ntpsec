/*
 * tg.c generate IRIG signals for test
 */
/*
 * This program can generate audio signals that simulate yhe IRIG-B
 * timecode commonly used to synchronize laboratory equipment. It is
 * the IRIG driver (refclock_irig.c) in the NTP driver collection.
 *
 * Besides testing the driver itself, this program can be used to
 * synchronize remote machines over audio transmission lines or program
 * feeds. The program reads the time on the local machine and sets the
 * initial epoch of the signal generator within one millisecond.
 * Alernatively, the initial epoch can be set to an arbitrary time. This
 * is useful when searching for bugs and testing for correct response to
 * a leap second in UTC. Note however, the ultimate accuracy is limited
 * by the intrinsic frequency error of the codec sample clock, which can
 * reach well over 100 PPM.
 *
 * The default is to route generated signals to the line output
 * jack; the s option on the command line routes these signals to the
 * internal speaker as well. The v option controls the speaker volume
 * over the range 0-255.
 *
 * Once started the program runs continuously. The default initial epoch
 * for the signal generator is read from the computer system clock when
 * the program starts. The y option specifies an alternate epoch using a
 * string yydddhhmmss, where yy is the year of century, ddd the day of
 * year, hh the hour of day and mm the minute of hour. For instance,
 * 1946Z on 1 January 2006 is 060011946. The remaining options are
 * specified below under the Parse Options heading. Most of these are
 * for testing.
 *
 * During operation the program displays the IRIG timecode (20 digits)
 * as each new string is constructed. The display is followed by the
 * BCD binary bits as transmitted. Note that the transmissionorder is
 * low-order first as the frame is processed left to right.  The IRIG
 * ton-time marker M preceeds the first (units) bit, so its code is
 * delayed one bit and the next digit (tens) needs only three bits.
 *
 * The program has been tested with the Sun Blade 1500 running Solaris
 * 10, but not yet with other machines. It uses no special features and
 * should be readily portable to other hardware and operating systems.
 *
 * Assumes the Sun Audio Driver API (SADA) interface used by SunOS, Solaris,
 * and OpenSolaris, supported in the Linux OSS sound layer, 
 * and still used by the *BSD operating systems. In particular it should work
 * for FreeBSD from version 4.1 on with compatible sound card.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#undef VERSION		/* avoid conflict below */

#ifdef  HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#else
# ifdef HAVE_SYS_AUDIOIO_H
# include <sys/audioio.h>
# else
# include <sys/audio.h>
# endif
#endif

#include "ntp_stdlib.h"	/* for strlcat(), strlcpy() */

#include <math.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#define VERSION		(0)
#define	ISSUE		(23)
#define	ISSUE_DATE	"2007-02-12"

#define	SECOND	(8000)			/* one second of 125-us samples */
#define BUFLNG	(400)			/* buffer size */
#define	DEVICE	"/dev/audio"	/* default audio device */
#define	OFF		(0)				/* zero amplitude */
#define	LOW		(1)				/* low amplitude */
#define	HIGH	(2)				/* high amplitude */
#define	M2		(2)				/* IRIG 0 pulse */
#define	M5		(5)				/* IRIG 1 pulse */
#define	M8		(8)				/* IRIG PI pulse */

#define	NUL		(0)

#define	SECONDS_PER_MINUTE	(60)
#define SECONDS_PER_HOUR	(3600)

#define	OUTPUT_DATA_STRING_LENGTH	(200)

/* Attempt at unmodulated - "high" */
int u6000[] = {
	247, 247, 247, 247, 247, 247, 247, 247, 247, 247,	/*  0- 9 */
    247, 247, 247, 247, 247, 247, 247, 247, 247, 247,	/* 10-19 */
    247, 247, 247, 247, 247, 247, 247, 247, 247, 247,	/* 20-29 */
    247, 247, 247, 247, 247, 247, 247, 247, 247, 247,	/* 30-39 */
    247, 247, 247, 247, 247, 247, 247, 247, 247, 247,	/* 40-49 */
    247, 247, 247, 247, 247, 247, 247, 247, 247, 247, 	/* 50-59 */
    247, 247, 247, 247, 247, 247, 247, 247, 247, 247,	/* 60-69 */
    247, 247, 247, 247, 247, 247, 247, 247, 247, 247}; 	/* 70-79 */

/* Attempt at unmodulated - "low" */
int u3000[] = {
	119, 119, 119, 119, 119, 119, 119, 119, 119, 119,	/*  0- 9 */
    119, 119, 119, 119, 119, 119, 119, 119, 119, 119,	/* 10-19 */
    119, 119, 119, 119, 119, 119, 119, 119, 119, 119,	/* 20-29 */
    119, 119, 119, 119, 119, 119, 119, 119, 119, 119,	/* 30-39 */
    119, 119, 119, 119, 119, 119, 119, 119, 119, 119,	/* 40-49 */
    119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 	/* 50-59 */
    119, 119, 119, 119, 119, 119, 119, 119, 119, 119,	/* 60-69 */
    119, 119, 119, 119, 119, 119, 119, 119, 119, 119}; 	/* 70-79 */

/*
 * Companded sine table amplitude 3000 units
 */
int c3000[] = {1, 48, 63, 70, 78, 82, 85, 89, 92, 94,	/* 0-9 */
     96,  98,  99, 100, 101, 101, 102, 103, 103, 103,	/* 10-19 */
    103, 103, 103, 103, 102, 101, 101, 100,  99,  98,	/* 20-29 */
     96,  94,  92,  89,  85,  82,  78,  70,  63,  48,	/* 30-39 */
    129, 176, 191, 198, 206, 210, 213, 217, 220, 222,	/* 40-49 */
    224, 226, 227, 228, 229, 229, 230, 231, 231, 231, 	/* 50-59 */
    231, 231, 231, 231, 230, 229, 229, 228, 227, 226,	/* 60-69 */
    224, 222, 220, 217, 213, 210, 206, 198, 191, 176}; 	/* 70-79 */
/*
 * Companded sine table amplitude 6000 units
 */
int c6000[] = {1, 63, 78, 86, 93, 98, 101, 104, 107, 110, /* 0-9 */
    112, 113, 115, 116, 117, 117, 118, 118, 119, 119,	/* 10-19 */
    119, 119, 119, 118, 118, 117, 117, 116, 115, 113,	/* 20-29 */
    112, 110, 107, 104, 101,  98,  93,  86,  78,  63,	/* 30-39 */
    129, 191, 206, 214, 221, 226, 229, 232, 235, 238,	/* 40-49 */
    240, 241, 243, 244, 245, 245, 246, 246, 247, 247, 	/* 50-59 */
    247, 247, 247, 246, 246, 245, 245, 244, 243, 241,	/* 60-69 */
    240, 238, 235, 232, 229, 226, 221, 214, 206, 191}; 	/* 70-79 */

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
 * IRIG format frames (1000 Hz, 1 second for 10 frames of data)
 */

/*
 * IRIG format frame 10 - MS straight binary seconds
 */
struct progx progu[] = {
	{COEF,	2},		/* 0 0x0 0200 seconds */
	{COEF,	4},		/* 1 0x0 0400 */
	{COEF,	8},		/* 2 0x0 0800 */
	{DECC,	1},		/* 3 0x0 1000 */
	{COEF,	2},		/* 4 0x0 2000 */
	{COEF,	4},		/* 6 0x0 4000 */
	{COEF,	8},		/* 7 0x0 8000 */
	{DECC,	1},		/* 8 0x1 0000 */
	{COEF,  2},     /* 9 0x2 0000 - but only 86,401 / 0x1 5181 seconds in a day, so always zero */
	{NODEC,	M8},	/* 9 PI */
};

/*
 * IRIG format frame 8 - MS control functions
 */
struct progx progv[] = {
	{COEF,	2},		/*  0 CF # 19 */
	{COEF,	4},		/*  1 CF # 20 */
	{COEF,	8},		/*  2 CF # 21 */
	{DECC,	1},		/*  3 CF # 22 */
	{COEF,	2},		/*  4 CF # 23 */
	{COEF,	4},		/*  6 CF # 24 */
	{COEF,	8},		/*  7 CF # 25 */
	{DECC,	1},		/*  8 CF # 26 */
	{COEF,  2},		/*  9 CF # 27 */
	{DEC,	M8},	/* 10 PI */
};

/*
 * IRIG format frames 7 & 9 - LS control functions & LS straight binary seconds
 */
struct progx progw[] = {
	{COEF,	1},		/*  0  CF # 10, 0x0 0001 seconds */
	{COEF,	2},		/*  1  CF # 11, 0x0 0002 */
	{COEF,	4},		/*  2  CF # 12, 0x0 0004 */
	{COEF,	8},		/*  3  CF # 13, 0x0 0008 */
	{DECC,	1},		/*  4  CF # 14, 0x0 0010 */
	{COEF,	2},		/*  6  CF # 15, 0x0 0020 */
	{COEF,	4},		/*  7  CF # 16, 0x0 0040 */
	{COEF,	8},		/*  8  CF # 17, 0x0 0080 */
	{DECC,  1},		/*  9  CF # 18, 0x0 0100 */
	{NODEC,	M8},	/* 10  PI */
};

/*
 * IRIG format frames 2 to 6 - minutes, hours, days, hundreds days, 2 digit years (also called control functions bits 1-9)
 */
struct progx progy[] = {
	{COEF,	1},		/* 0 1 units, CF # 1 */
	{COEF,	2},		/* 1 2 units, CF # 2 */
	{COEF,	4},		/* 2 4 units, CF # 3 */
	{COEF,	8},		/* 3 8 units, CF # 4 */
	{DECZ,	M2},	/* 4 zero bit, CF # 5 / unused, default zero in years */
	{COEF,	1},		/* 5 10 tens, CF # 6 */
	{COEF,	2},		/* 6 20 tens, CF # 7*/
	{COEF,	4},		/* 7 40 tens, CF # 8*/
	{COEF,	8},		/* 8 80 tens, CF # 9*/
	{DEC,	M8},	/* 9 PI */
};

/*
 * IRIG format first frame, frame 1 - seconds
 */
struct progx progz[] = {
	{MIN,	M8},	/* 0 PI (on-time marker for the second at zero cross of 1st cycle) */
	{COEF,	1},		/* 1 1 units */
	{COEF,	2},		/* 2 2 */
	{COEF,	4},		/* 3 4 */
	{COEF,	8},		/* 4 8 */
	{DECZ,	M2},	/* 5 zero bit */
	{COEF,	1},		/* 6 10 tens */
	{COEF,	2},		/* 7 20 */
	{COEF,	4},		/* 8 40 */
	{DEC,	M8},	/* 9 PI */
};

/* LeapState values. */
#define	LEAPSTATE_NORMAL			(0)
#define	LEAPSTATE_DELETING			(1)
#define	LEAPSTATE_INSERTING			(2)
#define	LEAPSTATE_ZERO_AFTER_INSERT	(3)


/*
 * Forward declarations
 */
void	digit(int);		/* encode digit */
void	peep(int, int, int);	/* send cycles */
void	poop(int, int, int, int); /* Generate unmodulated from similar tables */
void	delay(int);		/* delay samples */
int		ConvertMonthDayToDayOfYear (int, int, int);	/* Calc day of year from year month & day */
void	Help (void);	/* Usage message */
void	ReverseString(char *);

/*
 * Extern declarations, don't know why not in headers
 */
//float	round ( float );

/*
 * Global variables
 */
char	buffer[BUFLNG];		/* output buffer */
int	bufcnt = 0;		/* buffer counter */
int	fd;			/* audio codec file descriptor */
int	DstFlag = 0;		/* winter/summer time */
int	utc = 0;		/* option epoch */
bool IrigIncludeYear = false;	/* Whether to send year in first control functions area, between P5 and P6. */
bool IrigIncludeIeee = false;	/* Whether to send IEEE 1344 control functions extensions between P6 and P8. */
int	StraightBinarySeconds = 0;
int	ControlFunctions = 0;
bool	Debug = false;
bool	Verbose = true;
char	*CommandName;

#ifndef  HAVE_SYS_SOUNDCARD_H
int	level = AUDIO_MAX_GAIN / 8; /* output level */
int	port = AUDIO_LINE_OUT;	/* output port */
#endif

int		TotalSecondsCorrected = 0;
int		TotalCyclesAdded = 0;
int		TotalCyclesRemoved = 0;


/*
 * Main program
 */
int
main(
	int		argc,		/* command line options */
	char	**argv		/* poiniter to list of tokens */
	)
{
#ifndef  HAVE_SYS_SOUNDCARD_H
	audio_info_t info;	/* Sun audio structure */
	int	rval;           /* For IOCTL calls */
#endif

	struct	timespec	 TimeValue;				/* System clock at startup */
	time_t			 SecondsPartOfTime;		/* Sent to gmtime_r() for calculation of TimeStructure (can apply offset). */
	time_t			 BaseRealTime;			/* Base realtime so can determine seconds since starting. */
	time_t			 NowRealTime;			/* New realtime to can determine seconds as of now. */
	unsigned		 SecondsRunningRealTime;	/* Difference between NowRealTime and BaseRealTime. */
	unsigned		 SecondsRunningSimulationTime;	/* Time that the simulator has been running. */
	int				 SecondsRunningDifference;	/* Difference between what real time says we have been running */
												/* and what simulator says we have been running - will slowly  */
												/* change because of clock drift. */
	int				 ExpectedRunningDifference = 0;	/* Stable value that we've obtained from check at initial start-up.	*/
	unsigned		 StabilityCount;		/* Used to check stability of difference while starting */
#define	RUN_BEFORE_STABILITY_CHECK	(30)	// Must run this many seconds before even checking stability.
#define	MINIMUM_STABILITY_COUNT		(10)	// Number of consecutive differences that need to be within initial stability band to say we are stable.
#define	INITIAL_STABILITY_BAND		( 2)	// Determining initial stability for consecutive differences within +/- this value.
#define	RUNNING_STABILITY_BAND		( 5)	// When running, stability is defined as difference within +/- this value.

	struct	tm		*TimeStructure = NULL;	/* Structure returned by gmtime */
	char	device[200];	/* audio device */
	char	code[200];	/* timecode */
	int	temp;
	int	arg = 0;
	int	sw = 0;
	int	ptr = 0;

	int	Year;
	int	Month;
	int	DayOfMonth;
	int	Hour;
	int	Minute;
	int	Second = 0;
	int	DayOfYear;

	int	BitNumber;
#ifdef HAVE_SYS_SOUNDCARD_H
	int	AudioFormat;
	int	MonoStereo;     /* 0=mono, 1=stereo */
#define	MONO	(0)
#define	STEREO	(1)
	int	SampleRate;
	int	SampleRateDifference;
#endif
	int	SetSampleRate;
	char FormatCharacter = '3';		/* Default is IRIG-B with IEEE 1344 extensions */
	char AsciiValue;
	int	HexValue;
	int FrameNumber = 0;

	/* Time offset for IEEE 1344 indication. */
	float TimeOffset = 0.0;
	int	OffsetSignBit = 0;
	int OffsetOnes = 0;
	int OffsetHalf = 0;

	int	TimeQuality = 0;	/* Time quality for IEEE 1344 indication. */
	char ParityString[200];	/* Partial output string, to calculate parity on. */
	int	ParitySum = 0;
	int	ParityValue;
	char *StringPointer;

	/* Flags to indicate requested leap second addition or deletion by command line option. */
	/* Should be mutually exclusive - generally ensured by code which interprets command line option. */
	bool	InsertLeapSecond = false;
	bool	DeleteLeapSecond = false;

	/* Date and time of requested leap second addition or deletion. */
	int	LeapYear					= 0;
	int LeapMonth					= 0;
	int	LeapDayOfMonth				= 0;
	int LeapHour					= 0;
	int	LeapMinute					= 0;
	int	LeapDayOfYear				= 0;

	/* State flag for the insertion and deletion of leap seconds, esp. deletion, */
	/* where the logic gets a bit tricky. */
	int	LeapState = LEAPSTATE_NORMAL;

	/* Flags for indication of leap second pending and leap secod polarity in IEEE 1344 */
	bool	LeapSecondPending = false;
	bool	LeapSecondPolarity = false;

	/* Date and time of requested switch into or out of DST by command line option. */
	int	DstSwitchYear				= 0;
	int DstSwitchMonth				= 0;
	int	DstSwitchDayOfMonth			= 0;
	int DstSwitchHour				= 0;
	int	DstSwitchMinute				= 0;
	int	DstSwitchDayOfYear			= 0;

	/* Indicate when we have been asked to switch into or out of DST by command line option. */
	bool	DstSwitchFlag = false;

	/* To allow predict for DstPendingFlag in IEEE 1344 */
	int	DstSwitchPendingYear		= 0;	/* Default value isn't valid, but I don't care. */
	int	DstSwitchPendingDayOfYear	= 0;
	int	DstSwitchPendingHour		= 0;
	int	DstSwitchPendingMinute		= 0;

	/* /Flag for indication of a DST switch pending in IEEE 1344 */
	bool	DstPendingFlag = false;

	/* Attempt at unmodulated */
	bool	Unmodulated = false;
	bool	UnmodulatedInverted = false;

	/* Offset to actual time value sent. */
	float	UseOffsetHoursFloat;
	int		UseOffsetSecondsInt = 0;
	float	UseOffsetSecondsFloat;

	/* String to allow us to put out reversed data - so can read the binary numbers. */
	char	OutputDataString[OUTPUT_DATA_STRING_LENGTH];
	
	/* Number of seconds to send before exiting.  Default = 0 = forever. */
	int		SecondsToSend = 0;
	int		CountOfSecondsSent = 0;	/* Counter of seconds */
	
	/* Flags to indicate whether to add or remove a cycle for time adjustment. */
	bool		AddCycle = false;	 	// We are ahead, add cycle to slow down and get back in sync.
	bool		RemoveCycle = false;	// We are behind, remove cycle to slow down and get back in sync.
	int		RateCorrection;			// Aggregate flag for passing to subroutines.
	bool		EnableRateCorrection = true;
	
	float	RatioError;


	CommandName = argv[0];

	if	(argc < 1)
		{
		Help ();
		exit (-1);
		}

	/*
	 * Parse options
	 */
	strlcpy(device, DEVICE, sizeof(device));
	Year = 0;
	SetSampleRate = SECOND;
	
#if	HAVE_SYS_SOUNDCARD_H
	while ((temp = getopt(argc, argv, "a:b:c:df:g:hHi:jk:l:o:q:r:stxy:z?")) != -1) {
#else
	while ((temp = getopt(argc, argv, "a:b:c:df:g:hHi:jk:l:o:q:r:u:v:xy:z?")) != -1) {
#endif
		switch (temp) {

		case 'a':	/* specify audio device (/dev/audio) */
			strlcpy(device, optarg, sizeof(device));
			break;

		case 'b':	/* Remove (delete) a leap second at the end of the specified minute. */
			sscanf(optarg, "%2d%2d%2d%2d%2d", &LeapYear, &LeapMonth, &LeapDayOfMonth,
			    &LeapHour, &LeapMinute);
			InsertLeapSecond = false;
			DeleteLeapSecond = true;
			break;
			
		case 'c':	/* specify number of seconds to send output for before exiting, 0 = forever */
			sscanf(optarg, "%d", &SecondsToSend);
			break;

		case 'd':	/* start with DST active (IRIG) */
			DstFlag++;
			break;

		case 'f':	/* select format: i=IRIG-98 (default) 2=IRIG-2004 3-IRIG+IEEE-1344 */
			sscanf(optarg, "%c", &FormatCharacter);
			break;

		case 'g':	/* Date and time to switch back into / out of DST active. */
			sscanf(optarg, "%2d%2d%2d%2d%2d", &DstSwitchYear, &DstSwitchMonth, &DstSwitchDayOfMonth,
			    &DstSwitchHour, &DstSwitchMinute);
			DstSwitchFlag = true;
			break;

		case 'h':
		case 'H':
		case '?':
			Help ();
			exit(-1);
			break;

		case 'i':	/* Insert (add) a leap second at the end of the specified minute. */
			sscanf(optarg, "%2d%2d%2d%2d%2d", &LeapYear, &LeapMonth, &LeapDayOfMonth,
			    &LeapHour, &LeapMinute);
			InsertLeapSecond = true;
			DeleteLeapSecond = false;
			break;
			
		case 'j':
			EnableRateCorrection = false;
			break;

		case 'k':
			sscanf (optarg, "%d", &RateCorrection);
			EnableRateCorrection = false;
			if  (RateCorrection < 0)
				{
				RemoveCycle = true;
				AddCycle = false;
				
				if  (Verbose)
					printf ("\n> Forcing rate correction removal of cycle...\n");
				}
			else
				{
				if  (RateCorrection > 0)
					{
					RemoveCycle = false;
					AddCycle = true;
				
					if  (Verbose)
						printf ("\n> Forcing rate correction addition of cycle...\n");
					}
				}
			break;

		case 'l':	/* use time offset from UTC */
			sscanf(optarg, "%f", &UseOffsetHoursFloat);
			UseOffsetSecondsFloat = UseOffsetHoursFloat * (float) SECONDS_PER_HOUR;
			UseOffsetSecondsInt = (int) (UseOffsetSecondsFloat + 0.5);
			break;

		case 'o':	/* Set IEEE 1344 time offset in hours - positive or negative, to the half hour */
			sscanf(optarg, "%f", &TimeOffset);
			if  (TimeOffset >= -0.2)
				{
				OffsetSignBit = 0;

				if  (TimeOffset > 0)
					{
					OffsetOnes    = TimeOffset;

					if  ( (TimeOffset - floor(TimeOffset)) >= 0.4)
						OffsetHalf = 1;
					else
						OffsetHalf = 0;
					}
				else
					{
					OffsetOnes    = 0;
					OffsetHalf    = 0;
					}
				}
			else
				{
				OffsetSignBit = 1;
				OffsetOnes    = -TimeOffset;

				if  ( (ceil(TimeOffset) - TimeOffset) >= 0.4)
					OffsetHalf = 1;
				else
					OffsetHalf = 0;
				}

			/*printf ("\nGot TimeOffset = %3.1f, OffsetSignBit = %d, OffsetOnes = %d, OffsetHalf = %d...\n",
					TimeOffset, OffsetSignBit, OffsetOnes, OffsetHalf);
			*/
			break;

		case 'q':	/* Hex quality code 0 to 0x0F - 0 = maximum, 0x0F = no lock */
			sscanf(optarg, "%x", &TimeQuality);
			TimeQuality &= 0x0F;
			/*printf ("\nGot TimeQuality = 0x%1X...\n", TimeQuality);
			*/
			break;

		case 'r':	/* sample rate (nominally 8000, integer close to 8000 I hope) */
			sscanf(optarg, "%d", &SetSampleRate);
			break;

#ifndef  HAVE_SYS_SOUNDCARD_H
		case 'v':	/* set output level (0-255) */
			sscanf(optarg, "%d", &level);
			break;
#endif

		case 'x':	/* Turn off verbose output. */
			Verbose = false;
			break;

		case 'y':	/* Set initial date and time */
			sscanf(optarg, "%2d%2d%2d%2d%2d%2d", &Year, &Month, &DayOfMonth,
			    &Hour, &Minute, &Second);
			utc++;
			break;

		case 'z':	/* Turn on Debug output (also turns on Verbose below) */
			Debug = true;
			break;

		default:
			printf("Invalid option \"%c\", aborting...\n", temp);
			exit (-1);
			break;
		}
	}

	if  (Debug)
	    Verbose = true;

	if  (InsertLeapSecond || DeleteLeapSecond)
		{
		LeapDayOfYear = ConvertMonthDayToDayOfYear (LeapYear, LeapMonth, LeapDayOfMonth);

		if	(Debug)
			{
			printf ("\nHave request for leap second %s at year %4d day %3d at %2.2dh%2.2d....\n",\
					DeleteLeapSecond ? "DELETION" : (InsertLeapSecond ? "ADDITION" : "( error ! )" ),
					LeapYear, LeapDayOfYear, LeapHour, LeapMinute);
			}
		}

	if	(DstSwitchFlag)
		{
		DstSwitchDayOfYear = ConvertMonthDayToDayOfYear (DstSwitchYear, DstSwitchMonth, DstSwitchDayOfMonth);

		/* Figure out time of minute previous to DST switch, so can put up warning flag in IEEE 1344 */
		DstSwitchPendingYear		= DstSwitchYear;
		DstSwitchPendingDayOfYear	= DstSwitchDayOfYear;
		DstSwitchPendingHour		= DstSwitchHour;
		DstSwitchPendingMinute		= DstSwitchMinute - 1;
		if 	(DstSwitchPendingMinute < 0)
			{
			DstSwitchPendingMinute = 59;
			DstSwitchPendingHour--;
			if	(DstSwitchPendingHour < 0)
				{
				DstSwitchPendingHour = 23;
				DstSwitchPendingDayOfYear--;
				if	(DstSwitchPendingDayOfYear < 1)
					{
					DstSwitchPendingYear--;
					}
				}
			}

		if	(Debug)
			{
			printf ("\nHave DST switch request for year %4d day %3d at %2.2dh%2.2d,",
					DstSwitchYear, DstSwitchDayOfYear, DstSwitchHour, DstSwitchMinute);
			printf ("\n    so will have warning at year %4d day %3d at %2.2dh%2.2d.\n",
					DstSwitchPendingYear, DstSwitchPendingDayOfYear, DstSwitchPendingHour, DstSwitchPendingMinute);
			}
		}

	switch (FormatCharacter) {
	case 'i': case 'I':
		printf ("\nFormat is IRIG-1998 (no year coded)...\n\n");
		IrigIncludeYear = false;
		IrigIncludeIeee = false;
		break;

	case '2':
		printf ("\nFormat is IRIG-2004 (BCD year coded)...\n\n");
		IrigIncludeYear = true;
		IrigIncludeIeee = false;
		break;

	case '3':
		printf ("\nFormat is IRIG with IEEE-1344 (BCD year coded, and more control functions)...\n\n");
		IrigIncludeYear = true;
		IrigIncludeIeee = true;
		break;

	case '4':
		printf ("\nFormat is unmodulated IRIG with IEEE-1344 (BCD year coded, and more control functions)...\n\n");
		IrigIncludeYear = true;
		IrigIncludeIeee = true;

		Unmodulated = true;
		UnmodulatedInverted = false;
		break;

	case '5':
		printf ("\nFormat is inverted unmodulated IRIG with IEEE-1344 (BCD year coded, and more control functions)...\n\n");
		IrigIncludeYear = true;
		IrigIncludeIeee = true;

		Unmodulated = true;
		UnmodulatedInverted = true;
		break;

	default:
		printf ("\n\nUnexpected format value of \'%c\', cannot parse, aborting...\n\n", FormatCharacter);
		exit (-1);
		break;
	}

	/*
	 * Open audio device and set options
	 */
	fd = open(device, O_WRONLY);
	if (fd <= 0) {
		printf("Unable to open audio device \"%s\", aborting: %s\n", device, strerror(errno));
		exit(1);
	}

#ifdef  HAVE_SYS_SOUNDCARD_H
	/* First set coding type */
	AudioFormat = AFMT_MU_LAW;
	if (ioctl(fd, SNDCTL_DSP_SETFMT, &AudioFormat)==-1)
	{ /* Fatal error */
	printf ("\nUnable to set output format, aborting...\n\n");
	exit(-1);
	}

	if  (AudioFormat != AFMT_MU_LAW)
	{
	printf ("\nUnable to set output format for mu law, aborting...\n\n");
	exit(-1);
	}

	/* Next set number of channels */
	MonoStereo = MONO;	/* Mono */
	if (ioctl(fd, SNDCTL_DSP_STEREO, &MonoStereo)==-1)
	{ /* Fatal error */
	printf ("\nUnable to set mono/stereo, aborting...\n\n");
	exit(-1);
	}

	if (MonoStereo != MONO)
	{
	printf ("\nUnable to set mono/stereo for mono, aborting...\n\n");
	exit(-1);
	}

	/* Now set sample rate */
	SampleRate = SetSampleRate;
	if (ioctl(fd, SNDCTL_DSP_SPEED, &SampleRate)==-1)
	{ /* Fatal error */
	printf ("\nUnable to set sample rate to %d, returned %d, aborting...\n\n", SetSampleRate, SampleRate);
	exit(-1);
	}

	SampleRateDifference = SampleRate - SetSampleRate;

	if  (SampleRateDifference < 0)
		SampleRateDifference = - SampleRateDifference;

	/* Fixed allowable sample rate error 0.1% */
	if (SampleRateDifference > (SetSampleRate/1000))
	{
	printf ("\nUnable to set sample rate to %d, result was %d, more than 0.1 percent, aborting...\n\n", SetSampleRate, SampleRate);
	exit(-1);
	}
	else
	{
	/* printf ("\nAttempt to set sample rate to %d, actual %d...\n\n", SetSampleRate, SampleRate); */
	}
#else
	rval = ioctl(fd, AUDIO_GETINFO, &info);
	if (rval < 0) {
		printf("\naudio control %s", strerror(errno));
		exit(0);
	}
	info.play.port = port;
	info.play.gain = level;
	info.play.sample_rate = SetSampleRate;
	info.play.channels = 1;
	info.play.precision = 8;
	info.play.encoding = AUDIO_ENCODING_ULAW;
	printf("\nport %d gain %d rate %d chan %d prec %d encode %d\n",
	    info.play.port, info.play.gain, info.play.sample_rate,
	    info.play.channels, info.play.precision,
	    info.play.encoding);
	ioctl(fd, AUDIO_SETINFO, &info);
#endif

 	/*
	 * Unless specified otherwise, read the system clock and
	 * initialize the time.
	 */
	clock_gettime(CLOCK_REALTIME, &TimeValue);		// Now always read the system time to keep "real time" of operation.
	NowRealTime = BaseRealTime = SecondsPartOfTime = TimeValue.tv_sec;
	SecondsRunningSimulationTime = 0;	// Just starting simulation, running zero seconds as of now.
	StabilityCount = 0;					// No stability yet.

	if	(utc)
		{
		DayOfYear = ConvertMonthDayToDayOfYear (Year, Month, DayOfMonth);
		}
	else
		{
		struct tm tmbuf;
		/* Apply offset to time. */
		if	(UseOffsetSecondsInt >= 0)
			SecondsPartOfTime += (time_t)   UseOffsetSecondsInt;
		else
			SecondsPartOfTime -= (time_t) (-UseOffsetSecondsInt);

		TimeStructure = gmtime_r(&SecondsPartOfTime, &tmbuf);
		Minute = TimeStructure->tm_min;
		Hour = TimeStructure->tm_hour;
		DayOfYear = TimeStructure->tm_yday + 1;
		Year = TimeStructure->tm_year % 100;
		Second = TimeStructure->tm_sec;

		/*
		 * Delay the first second so the generator is accurately
		 * aligned with the system clock within one sample (125
		 * microseconds ).
		 */
		delay(SECOND - TimeValue.tv_nsec * 8 / 1000000);
		}

	StraightBinarySeconds = Second + (Minute * SECONDS_PER_MINUTE) + (Hour * SECONDS_PER_HOUR);

	memset(code, 0, sizeof(code));
	printf ("IRIG-B time signal, starting point:\n");
	printf (" Year = %02d, Day of year = %03d, Time = %02d:%02d:%02d, Straight binary seconds (SBS) = %05d / 0x%04X.\n",
	    Year, DayOfYear, Hour, Minute, Second, StraightBinarySeconds, StraightBinarySeconds);
	printf ("\n");
	if  (Verbose)
	    {
	printf ("Codes: \".\" = marker/position indicator, \"-\" = zero dummy bit, \"0\" = zero bit, \"1\" = one bit.\n");
		if  ((EnableRateCorrection) || (AddCycle) || (RemoveCycle))
			{
			printf ("       \"o\" = short zero, \"*\" = long zero, \"x\" = short one, \"+\" = long one.\n");
			}
	printf ("Numerical values are time order reversed in output to make it easier to read.\n");
	/*                 111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999 */
	/*       0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789 */
	    printf ("\n");
	printf ("Legend of output codes:\n");
	//printf ("\n");
	    //printf ("|  StraightBinSecs  | IEEE_1344_Control |   Year  |    Day_of_Year    |  Hours  | Minutes |Seconds |\n");
	//printf ("|  ---------------  | ----------------- |   ----  |    -----------    |  -----  | ------- |------- |\n");
	//printf ("|                   |                   |         |                   |         |         |        |\n");
	}

	/*
	 * Run the signal generator to generate new timecode strings
	 * once per second for IRIG.
	 */
	for (CountOfSecondsSent=0; ((SecondsToSend==0) || (CountOfSecondsSent<SecondsToSend)); CountOfSecondsSent++)
		{
		if  ((((Second % 20) == 0) || (CountOfSecondsSent == 0)))
			{
	    	printf ("\n");

			printf (" Year = %02d, Day of year = %03d, Time = %02d:%02d:%02d, Straight binary seconds (SBS) = %05d / 0x%04X.\n",
			    Year, DayOfYear, Hour, Minute, Second, StraightBinarySeconds, StraightBinarySeconds);
			if  ((EnableRateCorrection) || (RemoveCycle) || (AddCycle))
				{
				printf (" CountOfSecondsSent = %d, TotalCyclesAdded = %d, TotalCyclesRemoved = %d\n", CountOfSecondsSent, TotalCyclesAdded, TotalCyclesRemoved);
				if  ((CountOfSecondsSent != 0) && ((TotalCyclesAdded != 0) || (TotalCyclesRemoved != 0)))
					{
					RatioError = ((float) (TotalCyclesAdded - TotalCyclesRemoved)) / (1000.0 * (float) CountOfSecondsSent);
					printf (" Adjusted by %2.1f%%, apparent send frequency is %4.2f Hz not %d Hz.\n\n", 
									RatioError*100.0, (1.0+RatioError)*((float) SetSampleRate), SetSampleRate);
					}
				}
			else
				printf ("\n");

		    /* printf ("|Seconds | Minutes |  Hours  |    Day_of_Year    |   Year  | IEEE_1344_Control |  StraightBinSecs  |\n");
    		printf ("|------- | ------- |  -----  |    -----------    |   ----  | ----------------- |-------------------|\n");
	    	printf ("|        |         |         |                   |         |                   |                   |\n");*/
		    printf ("|  StraightBinSecs  | IEEE_1344_Control |   Year  |    Day_of_Year    |  Hours  | Minutes |Seconds |\n");
    		printf ("|  ---------------  | ----------------- |   ----  |    -----------    |  -----  | ------- |------- |\n");
	    	printf ("|                   |                   |         |                   |         |         |        |\n");
			}

		if  (RemoveCycle)
			{
			RateCorrection = -1;
			TotalSecondsCorrected ++;
			}
		else
			{
			if  (AddCycle)
				{
				TotalSecondsCorrected ++;
				RateCorrection = +1;
				}
			else
				RateCorrection = 0;
			}

		/*
		 * Crank the state machine to propagate carries to the
		 * year of century. Note that we delayed up to one
		 * second for alignment after reading the time, so this
		 * is the next second.
		 */

		if  (LeapState == LEAPSTATE_NORMAL)
			{
			/* If on the second of a leap (second 59 in the specified minute), then add or delete a second */
			if  ((Year == LeapYear) && (DayOfYear == LeapDayOfYear) && (Hour == LeapHour) && (Minute == LeapMinute))
				{
				/* To delete a second, which means we go from 58->60 instead of 58->59->00. */
				if  ((DeleteLeapSecond) && (Second == 58))
					{
					LeapState = LEAPSTATE_DELETING;

					if	(Debug)
						printf ("\n<--- Ready to delete a leap second...\n");
					}
				else
					{	/* Delete takes precedence over insert. */
					/* To add a second, which means we go from 59->60->00 instead of 59->00. */
					if  ((InsertLeapSecond) && (Second == 59))
						{
						LeapState = LEAPSTATE_INSERTING;

						if	(Debug)
							printf ("\n<--- Ready to insert a leap second...\n");
						}
					}
				}
			}

		switch (LeapState)
			{
			case LEAPSTATE_NORMAL:
				Second = (Second + 1) % 60;
				break;

			case LEAPSTATE_DELETING:
				Second = 0;
				LeapState = LEAPSTATE_NORMAL;

				if	(Debug)
					printf ("\n<--- Deleting a leap second...\n");
				break;

			case LEAPSTATE_INSERTING:
				Second = 60;
				LeapState = LEAPSTATE_ZERO_AFTER_INSERT;

				if	(Debug)
					printf ("\n<--- Inserting a leap second...\n");
				break;

			case LEAPSTATE_ZERO_AFTER_INSERT:
				Second = 0;
				LeapState = LEAPSTATE_NORMAL;

				if	(Debug)
					printf ("\n<--- Inserted a leap second, now back to zero...\n");
				break;

			default:
				printf ("\n\nLeap second state invalid value of %d, aborting...", LeapState);
				exit (-1);
				break;
			}

		/* Check for second rollover, increment minutes and ripple upward if required. */
		if (Second == 0) {
			Minute++;
			if (Minute >= 60) {
				Minute = 0;
				Hour++;
			}

			/* Check for activation of DST switch. */
			/* If DST is active, this would mean that at the appointed time, we de-activate DST, */
			/* which translates to going backward an hour (repeating the last hour). */
			/* If DST is not active, this would mean that at the appointed time, we activate DST, */
			/* which translates to going forward an hour (skipping the next hour). */
			if	(DstSwitchFlag)
				{
				/* The actual switch happens on the zero'th second of the actual minute specified. */
				if	((Year == DstSwitchYear) && (DayOfYear == DstSwitchDayOfYear) && (Hour == DstSwitchHour) && (Minute == DstSwitchMinute))
					{
					if  (DstFlag == 0)
						{	/* DST flag is zero, not in DST, going to DST, "spring ahead", so increment hour by two instead of one. */
						Hour++;
						DstFlag = 1;

						/* Must adjust offset to keep consistent with UTC. */
						/* Here we have to increase offset by one hour.  If it goes from negative to positive, then we fix that. */
						if	(OffsetSignBit == 0)
							{	/* Offset is positive */
							if	(OffsetOnes == 0x0F)
								{
								OffsetSignBit = 1;
								OffsetOnes    = (OffsetHalf == 0) ? 8 : 7;
								}
							else
								OffsetOnes++;
							}
						else
							{	/* Offset is negative */
							if  (OffsetOnes == 0)
								{
								OffsetSignBit = 0;
								OffsetOnes    = (OffsetHalf == 0) ? 1 : 0;
								}
							else
								OffsetOnes--;
							}

						if	(Debug)
							printf ("\n<--- DST activated, spring ahead an hour, new offset !...\n");
						}
					else
						{	/* DST flag is non zero, in DST, going out of DST, "fall back", so no increment of hour. */
						Hour--;
						DstFlag = 0;

						/* Must adjust offset to keep consistent with UTC. */
						/* Here we have to reduce offset by one hour.  If it goes negative, then we fix that. */
						if	(OffsetSignBit == 0)
							{	/* Offset is positive */
							if  (OffsetOnes == 0)
								{
								OffsetSignBit = 1;
								OffsetOnes    = (OffsetHalf == 0) ? 1 : 0;
								}
							else
								OffsetOnes--;
							}
						else
							{	/* Offset is negative */
							if	(OffsetOnes == 0x0F)
								{
								OffsetSignBit = 0;
								OffsetOnes    = (OffsetHalf == 0) ? 8 : 7;
								}
							else
								OffsetOnes++;
							}

						if	(Debug)
							printf ("\n<--- DST de-activated, fall back an hour!...\n");
						}

					DstSwitchFlag = false;	/* One time deal, not intended to run this program past two switches... */
					}
				}

			if (Hour >= 24) {
				/* Modified, just in case dumb case where activating DST advances 23h59:59 -> 01h00:00 */
				Hour = Hour % 24;
				DayOfYear++;
			}

			/*
			 * At year rollover check for leap second.
			 */
			if (DayOfYear >= (Year & 0x3 ? 366 : 367)) {
				DayOfYear = 1;
				Year++;
			}
		}	/* End of "if  (Second == 0)" */

		/* After all that, if we are in the minute just prior to a leap second, warn of leap second pending */
		/* and of the polarity */
		if  ((Year == LeapYear) && (DayOfYear == LeapDayOfYear) && (Hour == LeapHour) && (Minute == LeapMinute))
			{
			LeapSecondPending = true;
			LeapSecondPolarity = DeleteLeapSecond;
			}
		else
			{
			LeapSecondPending = false;
			LeapSecondPolarity = false;
			}

		/* Notification through IEEE 1344 happens during the whole minute previous to the minute specified. */
		/* The time of that minute has been previously calculated. */
		if	((Year == DstSwitchPendingYear) && (DayOfYear == DstSwitchPendingDayOfYear) &&
					(Hour == DstSwitchPendingHour) && (Minute == DstSwitchPendingMinute))
			{
			DstPendingFlag = true;
			}
		else
			{
			DstPendingFlag = false;
			}


		StraightBinarySeconds = Second + (Minute * SECONDS_PER_MINUTE) + (Hour * SECONDS_PER_HOUR);

		if  (IrigIncludeIeee)
			{
			if  ((OffsetOnes == 0) && (OffsetHalf == 0))
				OffsetSignBit = 0;

			ControlFunctions = (LeapSecondPending == 0 ? 0x00000 : 0x00001) | (LeapSecondPolarity == 0 ? 0x00000 : 0x00002)
					| (DstPendingFlag == 0 ? 0x00000 : 0x00004) | (DstFlag == 0 ? 0x00000 : 0x00008)
					| (OffsetSignBit == 0 ? 0x00000 : 0x00010)  | ((OffsetOnes & 0x0F) << 5)           | (OffsetHalf == 0 ? 0x00000 : 0x00200)
					| ((TimeQuality & 0x0F) << 10);
			/* if  (Verbose)
				printf ("\nDstFlag = %d, OffsetSignBit = %d, OffsetOnes = %d, OffsetHalf = %d, TimeQuality = 0x%1.1X ==> ControlFunctions = 0x%5.5X...",
					    DstFlag, OffsetSignBit, OffsetOnes, OffsetHalf, TimeQuality, ControlFunctions);
			*/
			}
		else
			ControlFunctions = 0;

		/*
					      YearDay HourMin Sec
		snprintf(code, sizeof(code), "%04x%04d%06d%02d%02d%02d",
			0, Year, DayOfYear, Hour, Minute, Second);
		*/
		if  (IrigIncludeYear) {
			snprintf(ParityString, sizeof(ParityString),
			    "%04X%02d%04d%02d%02d%02d",
			    ControlFunctions & 0x7FFF, Year,
			    DayOfYear, Hour, Minute, Second);
		} else {
			snprintf(ParityString, sizeof(ParityString),
			    "%04X%02d%04d%02d%02d%02d",
			    ControlFunctions & 0x7FFF,
			    0, DayOfYear, Hour, Minute, Second);
		}

		if  (IrigIncludeIeee)
			{
			ParitySum = 0;
			for (StringPointer=ParityString; *StringPointer!=NUL; StringPointer++)
				{
				switch (*StringPointer)
					{
					case '1':
					case '2':
					case '4':
					case '8':
						ParitySum += 1;
						break;

					case '3':
					case '5':
					case '6':
					case '9':
					case 'A': case 'a':
					case 'C': case 'c':
						ParitySum += 2;
						break;

					case '7':
					case 'B': case 'b':
					case 'D': case 'd':
					case 'E': case 'e':
						ParitySum += 3;
						break;

					case 'F': case 'f':
						ParitySum += 4;
						break;
					}
				}

			if  ((ParitySum & 0x01) == 0x01)
				ParityValue = 0x01;
			else
				ParityValue = 0;
			}
		else
			ParityValue = 0;

		ControlFunctions |= ((ParityValue & 0x01) << 14);

		if  (IrigIncludeYear) {
			snprintf(code, sizeof(code),
			    /* YearDay HourMin Sec */
			    "%05X%05X%02d%04d%02d%02d%02d",
			    StraightBinarySeconds,
			    ControlFunctions, Year, DayOfYear,
			    Hour, Minute, Second);
		} else {
			snprintf(code, sizeof(code),
			    /* YearDay HourMin Sec */
			    "%05X%05X%02d%04d%02d%02d%02d",
			    StraightBinarySeconds,
			    ControlFunctions, 0, DayOfYear,
			    Hour, Minute, Second);
		}

		if  (Debug)
			printf("\nCode string: %s, ParityString = %s, ParitySum = 0x%2.2X, ParityValue = %d, DstFlag = %d...\n", code, ParityString, ParitySum, ParityValue, DstFlag);

		ptr = strlen(code)-1;

		/*
		 * The IRIG second consists of 20 BCD digits of width-
		 * modulateod pulses at 2, 5 and 8 ms and modulated 50
		 * percent on the 1000-Hz carrier.
		 */
		/* Initialize the output string */
		OutputDataString[0] = '\0';

		for (BitNumber = 0; BitNumber < 100; BitNumber++) {
			FrameNumber = (BitNumber/10) + 1;
			switch (FrameNumber)
				{
				case 1:
					/* bits 0 to 9, first frame */
					sw  = progz[BitNumber % 10].sw;
					arg = progz[BitNumber % 10].arg;
					break;

				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
					/* bits 10 to 59, second to sixth frame */
					sw  = progy[BitNumber % 10].sw;
					arg = progy[BitNumber % 10].arg;
					break;

				case 7:
					/* bits 60 to 69, seventh frame */
					sw  = progw[BitNumber % 10].sw;
					arg = progw[BitNumber % 10].arg;
					break;

				case 8:
					/* bits 70 to 79, eighth frame */
					sw  = progv[BitNumber % 10].sw;
					arg = progv[BitNumber % 10].arg;
					break;

				case 9:
					/* bits 80 to 89, ninth frame */
					sw  = progw[BitNumber % 10].sw;
					arg = progw[BitNumber % 10].arg;
					break;

				case 10:
					/* bits 90 to 99, tenth frame */
					sw  = progu[BitNumber % 10].sw;
					arg = progu[BitNumber % 10].arg;
					break;

				default:
					/* , Unexpected values of FrameNumber */
					printf ("\n\nUnexpected value of FrameNumber = %d, cannot parse, aborting...\n\n", FrameNumber);
					exit (-1);
					break;
				}

			switch(sw) {

			case DECC:	/* decrement pointer and send bit. */
				ptr--;
			case COEF:	/* send BCD bit */
				AsciiValue = toupper((int)code[ptr]);
				HexValue   = isdigit((int)AsciiValue) ? AsciiValue - '0' : (AsciiValue - 'A')+10;
				/* if  (Debug) {
					if  (ptr != OldPtr) {
					if  (Verbose)
					    printf("\n(%c->%X)", AsciiValue, HexValue);
					OldPtr = ptr;
					}
				}
				*/
				// OK, adjust all unused bits in hundreds of days.
				if  ((FrameNumber == 5) && ((BitNumber % 10) > 1))
					{
					if  (RateCorrection < 0)
						{	// Need to remove cycles to catch up.
						if  ((HexValue & arg) != 0) 
							{
							if  (Unmodulated)
								{
								poop(M5, 1000, HIGH, UnmodulatedInverted);
								poop(M5-1, 1000, LOW,  UnmodulatedInverted);

								TotalCyclesRemoved += 1;
								}
							else
								{
								peep(M5, 1000, HIGH);
								peep(M5-1, 1000, LOW);

								TotalCyclesRemoved += 1;
								}
							strlcat(OutputDataString, "x", OUTPUT_DATA_STRING_LENGTH);
							}
						else 
							{
							if	(Unmodulated)
								{
								poop(M2, 1000, HIGH, UnmodulatedInverted);
								poop(M8-1, 1000, LOW,  UnmodulatedInverted);

								TotalCyclesRemoved += 1;
								}
							else
								{
								peep(M2, 1000, HIGH);
								peep(M8-1, 1000, LOW);

								TotalCyclesRemoved += 1;
								}
							strlcat(OutputDataString, "o", OUTPUT_DATA_STRING_LENGTH);
							}
						}	// End of true clause for "if  (RateCorrection < 0)"
					else
						{	// Else clause for "if  (RateCorrection < 0)"
						if  (RateCorrection > 0)
							{	// Need to add cycles to slow back down.
							if  ((HexValue & arg) != 0) 
								{
								if  (Unmodulated)
									{
									poop(M5, 1000, HIGH, UnmodulatedInverted);
									poop(M5+1, 1000, LOW,  UnmodulatedInverted);

									TotalCyclesAdded += 1;
									}
								else
									{
									peep(M5, 1000, HIGH);
									peep(M5+1, 1000, LOW);

									TotalCyclesAdded += 1;
									}
								strlcat(OutputDataString, "+", OUTPUT_DATA_STRING_LENGTH);
								}
							else 
								{
								if	(Unmodulated)
									{
									poop(M2, 1000, HIGH, UnmodulatedInverted);
									poop(M8+1, 1000, LOW,  UnmodulatedInverted);

									TotalCyclesAdded += 1;
									}
								else
									{
									peep(M2, 1000, HIGH);
									peep(M8+1, 1000, LOW);

									TotalCyclesAdded += 1;
									}
								strlcat(OutputDataString, "*", OUTPUT_DATA_STRING_LENGTH);
								}
							}	// End of true clause for "if  (RateCorrection > 0)"
						else
							{	// Else clause for "if  (RateCorrection > 0)"
							// Rate is OK, just do what you feel!
							if  ((HexValue & arg) != 0) 
								{
								if  (Unmodulated)
									{
									poop(M5, 1000, HIGH, UnmodulatedInverted);
									poop(M5, 1000, LOW,  UnmodulatedInverted);
									}
								else
									{
									peep(M5, 1000, HIGH);
									peep(M5, 1000, LOW);
									}
								strlcat(OutputDataString, "1", OUTPUT_DATA_STRING_LENGTH);
								}
							else 
								{
								if	(Unmodulated)
									{
									poop(M2, 1000, HIGH, UnmodulatedInverted);
									poop(M8, 1000, LOW,  UnmodulatedInverted);
									}
								else
									{
									peep(M2, 1000, HIGH);
									peep(M8, 1000, LOW);
									}
								strlcat(OutputDataString, "0", OUTPUT_DATA_STRING_LENGTH);
								}
							}	// End of else clause for "if  (RateCorrection > 0)"
						}	// End of else claues for "if  (RateCorrection < 0)"
					}	// End of true clause for "if  ((FrameNumber == 5) && (BitNumber == 8))"
				else
					{	// Else clause for "if  ((FrameNumber == 5) && (BitNumber == 8))"
					if  ((HexValue & arg) != 0) 
						{
						if  (Unmodulated)
							{
							poop(M5, 1000, HIGH, UnmodulatedInverted);
							poop(M5, 1000, LOW,  UnmodulatedInverted);
							}
						else
							{
							peep(M5, 1000, HIGH);
							peep(M5, 1000, LOW);
							}
						strlcat(OutputDataString, "1", OUTPUT_DATA_STRING_LENGTH);
						}
					else 
						{
						if	(Unmodulated)
							{
							poop(M2, 1000, HIGH, UnmodulatedInverted);
							poop(M8, 1000, LOW,  UnmodulatedInverted);
							}
						else
							{
							peep(M2, 1000, HIGH);
							peep(M8, 1000, LOW);
							}
						strlcat(OutputDataString, "0", OUTPUT_DATA_STRING_LENGTH);
						}
					} // end of else clause for "if  ((FrameNumber == 5) && (BitNumber == 8))"
				break;

			case DECZ:	/* decrement pointer and send zero bit */
				ptr--;
				if	(Unmodulated)
					{
					poop(M2, 1000, HIGH, UnmodulatedInverted);
					poop(M8, 1000, LOW,  UnmodulatedInverted);
					}
				else
					{
					peep(M2, 1000, HIGH);
					peep(M8, 1000, LOW);
					}
				strlcat(OutputDataString, "-", OUTPUT_DATA_STRING_LENGTH);
				break;

			case DEC:	/* send marker/position indicator IM/PI bit */
				ptr--;
			case NODEC:	/* send marker/position indicator IM/PI bit but no decrement pointer */
			case MIN:	/* send "second start" marker/position indicator IM/PI bit */
				if  (Unmodulated)
					{
					poop(arg,      1000, HIGH, UnmodulatedInverted);
					poop(10 - arg, 1000, LOW,  UnmodulatedInverted);
					}
				else
					{
					peep(arg,      1000, HIGH);
					peep(10 - arg, 1000, LOW);
					}
				strlcat(OutputDataString, ".", OUTPUT_DATA_STRING_LENGTH);
				break;

			default:
				printf ("\n\nUnknown state machine value \"%d\", unable to continue, aborting...\n\n", sw);
				exit (-1);
				break;
			}
			if (ptr < 0)
				break;
		}
		ReverseString ( OutputDataString );
		if  (Verbose)
			{
		printf("%s", OutputDataString);
			if  (RateCorrection > 0)
				printf(" fast\n");
			else
				{
				if  (RateCorrection < 0)
					printf (" slow\n");
				else
					printf ("\n");
				}
			}
		}

	if  (EnableRateCorrection)
		{
		SecondsRunningSimulationTime++;

		clock_gettime(CLOCK_REALTIME, &TimeValue);
		NowRealTime = TimeValue.tv_sec;

		if  (NowRealTime >= BaseRealTime)		// Just in case system time corrects backwards, do not blow up.
			{
			SecondsRunningRealTime = (unsigned) (NowRealTime - BaseRealTime);
			SecondsRunningDifference = SecondsRunningSimulationTime - SecondsRunningRealTime;

			if  (Debug)
				{
				printf ("> NowRealTime = 0x%8.8X, BaseRealtime = 0x%8.8X, SecondsRunningRealTime = 0x%8.8X, SecondsRunningSimulationTime = 0x%8.8X.\n",
							(unsigned) NowRealTime, (unsigned) BaseRealTime, SecondsRunningRealTime, SecondsRunningSimulationTime);
				printf ("> SecondsRunningDifference = 0x%8.8X, ExpectedRunningDifference = 0x%8.8X.\n",
							SecondsRunningDifference, ExpectedRunningDifference);
				}

			if  (SecondsRunningSimulationTime > RUN_BEFORE_STABILITY_CHECK)
				{
				if  (StabilityCount < MINIMUM_STABILITY_COUNT)
					{
					if  (StabilityCount == 0)
						{
						ExpectedRunningDifference = SecondsRunningDifference;
						StabilityCount++;
						if  (Debug)
							printf ("> Starting stability check.\n");
						}
					else
						{	// Else for "if  (StabilityCount == 0)"
						if  ((ExpectedRunningDifference+INITIAL_STABILITY_BAND > SecondsRunningDifference)
								&& (ExpectedRunningDifference-INITIAL_STABILITY_BAND < SecondsRunningDifference))
							{	// So far, still within stability band, increment count.
							StabilityCount++;
							if  (Debug)
								printf ("> StabilityCount = %d.\n", StabilityCount);
							}
						else
							{	// Outside of stability band, start over.
							StabilityCount = 0;
							if  (Debug)
								printf ("> Out of stability band, start over.\n");
							}
						} // End of else for "if  (StabilityCount == 0)"
					}	// End of true clause for "if  (StabilityCount < MINIMUM_STABILITY_COUNT))"
				else
					{	// Else clause for "if  (StabilityCount < MINIMUM_STABILITY_COUNT))" - OK, so we are supposed to be stable.
					if  (AddCycle)
						{
						if  (ExpectedRunningDifference >= SecondsRunningDifference)
							{
							if  (Debug)
								printf ("> Was adding cycles, ExpectedRunningDifference >= SecondsRunningDifference, can stop it now.\n");

							AddCycle = false;
							RemoveCycle = false;
							}
						else
							{
							if  (Debug)
								printf ("> Was adding cycles, not done yet.\n");
							}
						}
					else
						{
						if  (RemoveCycle)
							{
							if  (ExpectedRunningDifference <= SecondsRunningDifference)
								{
								if  (Debug)
									printf ("> Was removing cycles, ExpectedRunningDifference <= SecondsRunningDifference, can stop it now.\n");

								AddCycle = false;
								RemoveCycle = false;
								}
							else
								{
								if  (Debug)
									printf ("> Was removing cycles, not done yet.\n");
								}
							}
						else
							{
							if  ((ExpectedRunningDifference+RUNNING_STABILITY_BAND > SecondsRunningDifference)
									&& (ExpectedRunningDifference-RUNNING_STABILITY_BAND < SecondsRunningDifference))
								{	// All is well, within tolerances.
								if  (Debug)
									printf ("> All is well, within tolerances.\n");
								}
							else
								{	// Oops, outside tolerances.  Else clause of "if  ((ExpectedRunningDifference...SecondsRunningDifference)"
								if  (ExpectedRunningDifference > SecondsRunningDifference)
									{
									if  (Debug)
										printf ("> ExpectedRunningDifference > SecondsRunningDifference, running behind real time.\n");

									// Behind real time, have to add a cycle to slow down and get back in sync.
									AddCycle = false;
									RemoveCycle = true;
									}
								else
									{	// Else clause of "if  (ExpectedRunningDifference < SecondsRunningDifference)"
									if  (ExpectedRunningDifference < SecondsRunningDifference)
										{
										if  (Debug)
											printf ("> ExpectedRunningDifference < SecondsRunningDifference, running ahead of real time.\n");

										// Ahead of real time, have to remove a cycle to speed up and get back in sync.
										AddCycle = true;
										RemoveCycle = false;
										}
									else
										{
										if  (Debug)
											printf ("> Oops, outside tolerances, but doesn't fit the profiles, how can this be?\n");
										}
									}	// End of else clause of "if  (ExpectedRunningDifference > SecondsRunningDifference)"
								}	// End of else clause of "if  ((ExpectedRunningDifference...SecondsRunningDifference)"
							}	// End of else clause of "if  (RemoveCycle)".
						}	// End of else clause of "if  (AddCycle)".
					}	// End of else clause for "if  (StabilityCount < MINIMUM_STABILITY_COUNT))"
				}	// End of true clause for "if  ((SecondsRunningSimulationTime > RUN_BEFORE_STABILITY_CHECK)"
			}	// End of true clause for "if  (NowRealTime >= BaseRealTime)"
		else
			{
			if  (Debug)
				printf ("> Hmm, time going backwards?\n");
			}
		}	// End of true clause for "if  (EnableRateCorrection)"
		
	fflush (stdout);
	}
	
	
printf ("\n\n>> Completed %d seconds, exiting...\n\n", SecondsToSend);
return (0);
}

/*
 * Generate cycles of 100 Hz or any multiple of 100 Hz.
 */
void peep(
	int	pulse,		/* pulse length (ms) */
	int	freq,		/* frequency (Hz) */
	int	amp		/* amplitude */
	)
{
	int	increm;		/* phase increment */
	int	i, j;

	if (amp == OFF || freq == 0)
		increm = 10;
	else
		increm = freq / 100;
	j = 0;
	for (i = 0 ; i < pulse * 8; i++) {
		switch (amp) {

		case HIGH:
			buffer[bufcnt++] = ~c6000[j];
			break;

		case LOW:
			buffer[bufcnt++] = ~c3000[j];
			break;

		default:
			buffer[bufcnt++] = ~0;
		}
		if (bufcnt >= BUFLNG) {
			write(fd, buffer, BUFLNG);
			bufcnt = 0;
		}
		j = (j + increm) % 80;
	}
}


/*
 * Generate unmodulated from similar tables.
 */
void poop(
	int	pulse,		/* pulse length (ms) */
	int	freq,		/* frequency (Hz) */
	int	amp,		/* amplitude */
	int inverted	/* is upside down */
	)
{
	int	increm;		/* phase increment */
	int	i, j;

	if (amp == OFF || freq == 0)
		increm = 10;
	else
		increm = freq / 100;
	j = 0;
	for (i = 0 ; i < pulse * 8; i++) {
		switch (amp) {

		case HIGH:
			if  (inverted)
				buffer[bufcnt++] = ~u3000[j];
			else
				buffer[bufcnt++] = ~u6000[j];
			break;

		case LOW:
			if  (inverted)
				buffer[bufcnt++] = ~u6000[j];
			else
				buffer[bufcnt++] = ~u3000[j];
			break;

		default:
			buffer[bufcnt++] = ~0;
		}
		if (bufcnt >= BUFLNG) {
			write(fd, buffer, BUFLNG);
			bufcnt = 0;
		}
		j = (j + increm) % 80;
	}
}

/*
 * Delay for initial phasing
 */
void delay (
	int	Delay		/* delay in samples */
	)
{
	int	samples;	/* samples remaining */

	samples = Delay;
	memset(buffer, 0, BUFLNG);
	while (samples >= BUFLNG) {
		write(fd, buffer, BUFLNG);
		samples -= BUFLNG;
	}
		write(fd, buffer, samples);
}


/* Calc day of year from year month & day */
/* Year - 0 means 2000, 100 means 2100. */
/* Month - 1 means January, 12 means December. */
/* DayOfMonth - 1 is first day of month */
int
ConvertMonthDayToDayOfYear (int YearValue, int MonthValue, int DayOfMonthValue)
	{
	int	ReturnValue;
	int	LeapYear;
	int	MonthCounter;

	/* Array of days in a month.  Note that here January is zero. */
	/* NB: have to add 1 to days in February in a leap year! */
	int DaysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


	LeapYear = false;
	if  ((YearValue % 4) == 0)
		{
		if  ((YearValue % 100) == 0)
			{
			if  ((YearValue % 400) == 0)
				{
				LeapYear = true;
				}
			}
		else
			{
			LeapYear = true;
			}
		}

	if  (Debug)
		printf ("\nConvertMonthDayToDayOfYear(): Year %d %s a leap year.\n", YearValue+2000, LeapYear ? "is" : "is not");

	/* Day of month given us starts in this algorithm. */
	ReturnValue = DayOfMonthValue;

	/* Add in days in month for each month past January. */
	for (MonthCounter=1; MonthCounter<MonthValue; MonthCounter++)
		{
		ReturnValue += DaysInMonth [ MonthCounter - 1 ];
		}

	/* Add a day for leap years where we are past February. */
	if  ((LeapYear) && (MonthValue > 2))
		{
		ReturnValue++;
		}

	if  (Debug)
		printf ("\nConvertMonthDayToDayOfYear(): %4.4d-%2.2d-%2.2d represents day %3d of year.\n",
				YearValue+2000, MonthValue, DayOfMonthValue, ReturnValue);

	return (ReturnValue);
	}


void
Help ( void )
	{
	printf ("\n\nTime Code Generation - IRIG-B v%d.%d, %s dmw", VERSION, ISSUE, ISSUE_DATE);
	printf ("\n\nRCS Info:");
	printf (  "\n  $Header: /home/dmw/src/IRIG_generation/ntp-4.2.2p3/util/RCS/tg.c,v 1.28 2007/02/12 23:57:45 dmw Exp $");
	printf ("\n\nUsage: %s [option]*", CommandName);
	printf ("\n\nOptions: -a device_name                 Output audio device name (default /dev/audio)");
	printf (  "\n         -b yymmddhhmm                  Remove leap second at end of minute specified");
	printf (  "\n         -c seconds_to_send             Number of seconds to send (default 0 = forever)");
	printf (  "\n         -d                             Start with IEEE 1344 DST active");
	printf (  "\n         -f format_type                 i = Modulated IRIG-B 1998 (no year coded)");
	printf (  "\n                                        2 = Modulated IRIG-B 2002 (year coded)");
	printf (  "\n                                        3 = Modulated IRIG-B w/IEEE 1344 (year & control funcs) (default)");
	printf (  "\n                                        4 = Unmodulated IRIG-B w/IEEE 1344 (year & control funcs)");
	printf (  "\n                                        5 = Inverted unmodulated IRIG-B w/IEEE 1344 (year & control funcs)");
	printf (  "\n         -g yymmddhhmm                  Switch into/out of DST at beginning of minute specified");
	printf (  "\n         -i yymmddhhmm                  Insert leap second at end of minute specified");
	printf (  "\n         -j                             Disable time rate correction against system clock (default enabled)");
	printf (  "\n         -k nn                          Force rate correction for testing (+1 = add cycle, -1 = remove cycle)");
	printf (  "\n         -l time_offset                 Set offset of time sent to UTC as per computer, +/- float hours");
	printf (  "\n         -o time_offset                 Set IEEE 1344 time offset, +/-, to 0.5 hour (default 0)");
	printf (  "\n         -q quality_code_hex            Set IEEE 1344 quality code (default 0)");
	printf (  "\n         -r sample_rate                 Audio sample rate (default 8000)");
#ifndef  HAVE_SYS_SOUNDCARD_H
	printf (  "\n         -v initial_output_level        Set initial output level (default %d, must be 0 to 255)", AUDIO_MAX_GAIN/8);
#endif
	printf (  "\n         -x                             Turn off verbose output (default on)");
	printf (  "\n         -y yymmddhhmmss                Set initial date and time as specified (default system time)");
	printf ("\n\nThis software licenced under the GPL, modifications performed 2006 & 2007 by Dean Weiten");
	printf (  "\nContact: Dean Weiten, Norscan Instruments Ltd., Winnipeg, MB, Canada, ph (204)-233-9138, E-mail dmw@norscan.com");
	printf ("\n\n");
	}

/* Reverse string order for nicer print. */
void
ReverseString(char *str)
	{
	int		StringLength;
	int		IndexCounter;
	int		CentreOfString;
	char	TemporaryCharacter;


	StringLength	= strlen(str);
	CentreOfString	= (StringLength/2)+1;
	for (IndexCounter = StringLength; IndexCounter >= CentreOfString; IndexCounter--)
		{
		TemporaryCharacter				= str[IndexCounter-1];
		str[IndexCounter-1]				= str[StringLength-IndexCounter];
		str[StringLength-IndexCounter]	= TemporaryCharacter;
		}
	}

