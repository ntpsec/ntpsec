/*
 * Copyright (C) 2004  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2002  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* $Id: ntservice.c,v 1.3.2.1.10.3 2004/03/08 04:04:22 marka Exp $ */

#include <config.h>
#include <stdio.h>

#include <ntp_cmdargs.h>
#include <ntp_stdlib.h>
#include "syslog.h"
#include "ntservice.h"
#include "clockstuff.h"
#include "ntp_iocompletionport.h"
#include "isc/win32os.h"


/* Handle to SCM for updating service status */
static SERVICE_STATUS_HANDLE hServiceStatus = 0;
static BOOL foreground = FALSE;
static BOOL computer_shutting_down = FALSE;
static int glb_argc;
static char **glb_argv;
HANDLE hServDoneEvent = NULL;
int accept_wildcard_if_for_winnt;
extern volatile int debug;

void uninit_io_completion_port();
int ntpdmain(int argc, char *argv[]);
/*
 * libisc\nt_strerror.c
 */
#ifdef DEBUG
void FormatErrorFreeMem(void);
#endif
/*
 * Forward declarations
 */
void WINAPI ServiceControl(DWORD dwCtrlCode);
void ntservice_exit(void);

void WINAPI service_main( DWORD argc, LPTSTR *argv )
{
	if ( argc > 1 )
	{
		/*
		 * Let command line parameters from the Windows SCM GUI
		 * override the standard parameters from the ImagePath registry key.
		 */
		glb_argc = argc;
		glb_argv = argv;
	}

	ntpdmain( glb_argc, glb_argv );
}


/*
 * This is the entry point for the executable 
 * We can call ntpdmain() explicitly or via StartServiceCtrlDispatcher()
 * as we need to.
 */
int main( int argc, char *argv[] )
{
	int rc;
	int i = 1;

	/* Save the command line parameters */
	glb_argc = argc;
	glb_argv = argv;

	/* Under original Windows NT we must not discard the wildcard */
	/* socket to workaround a bug in NT's getsockname(). */
	if ( isc_win32os_majorversion() <= 4 )
		accept_wildcard_if_for_winnt = 1;

	/* Command line users should put -n in the options */
	while (argv[i]) {
		if (!_strnicmp(argv[i], "-d", 2) ||
			!strcmp(argv[i], "-q") ||
			!strcmp(argv[i], "--help") ||
			!strcmp(argv[i], "-n")) {
			foreground = TRUE;
			break;
		}
		i++;
	}

	if (foreground) {
		/* run in console window */
		rc = ntpdmain(argc, argv);
	} else {
		/* Start up as service */

		SERVICE_TABLE_ENTRY dispatchTable[] = {
			{ TEXT(NTP_DISPLAY_NAME), service_main },
			{ NULL, NULL }
		};

		rc = StartServiceCtrlDispatcher(dispatchTable);
		if (rc)
			rc = 0; 
		else {
			rc = GetLastError();
#ifdef DEBUG
			fprintf(stderr, "%s: unable to start as service, rc: %i\n\n", argv[0], rc);
#endif
			fprintf(stderr, "\nUse -d, -q, --help or -n to run from the command line.\n");
		}
	}
	return rc;
}

/*
 * Initialize the Service by registering it.
 */
void
ntservice_init() {
	char ConsoleTitle[256];

	if (!foreground) {
		/* Register handler with the SCM */
		hServiceStatus = RegisterServiceCtrlHandler(NTP_DISPLAY_NAME,
					ServiceControl);
		if (!hServiceStatus) {
			NTReportError(NTP_SERVICE_NAME,
				"could not register service control handler");
			exit(1);
		}
		UpdateSCM(SERVICE_RUNNING);
	} else {
		strcpy(ConsoleTitle, "NTP Version ");
		strcat(ConsoleTitle, Version);
		SetConsoleTitle(ConsoleTitle);
	}

	#ifdef _DEBUG
		/* ask the runtime to dump memory leaks at exit */
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		#ifdef WANT_LEAK_CHECK_ON_STDERR_TOO
			/* hart: I haven't seen this work, running ntpd.exe -n from a shell */
			/* to both a file and the debugger output window */
			_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
			/* the file being stderr */
			_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
		#endif
	#endif /* _DEBUG */

	atexit( ntservice_exit );
}

/*
 * Routine to check if this is a service or a foreground program
 */
BOOL
ntservice_isservice() {
	return(!foreground);
}

/*
 * Routine to check if the service is stopping
 * because the computer is shutting down
 */
BOOL
ntservice_systemisshuttingdown() {
	return computer_shutting_down;
}

void
ntservice_exit( void )
{
	uninit_io_completion_port();
	Sleep( 200 );  	//##++ 

	reset_winnt_time();

	msyslog(LOG_INFO, "ntservice: The Network Time Protocol Service is stopping.");

#ifdef _DEBUG
	FormatErrorFreeMem();
#endif 

	if (!foreground) {
		/* service mode, need to have the service_main routine
		 * register with the service control manager that the 
		 * service has stopped running, before exiting
		 */
		UpdateSCM(SERVICE_STOPPED);
	}
}

/* 
 * ServiceControl(): Handles requests from the SCM and passes them on
 * to the service.
 */
void WINAPI
ServiceControl(DWORD dwCtrlCode) {
	/* Handle the requested control code */
	HANDLE exitEvent = get_exit_event();

	switch(dwCtrlCode) {

	case SERVICE_CONTROL_SHUTDOWN:
		computer_shutting_down = TRUE;
		/* fall through to stop case */

	case SERVICE_CONTROL_STOP:
		if (exitEvent != NULL) {
			SetEvent(exitEvent);
			UpdateSCM(SERVICE_STOP_PENDING);
			Sleep( 100 );  //##++
		}
		return;

	case SERVICE_CONTROL_PAUSE:
	case SERVICE_CONTROL_CONTINUE:
	case SERVICE_CONTROL_INTERROGATE:
        default:
		break;
	}
	UpdateSCM(SERVICE_RUNNING);
}

/*
 * Tell the Service Control Manager the state of the service.
 */
void UpdateSCM(DWORD state) {
	SERVICE_STATUS ss;
	static DWORD dwState = SERVICE_STOPPED;

	if (hServiceStatus) {
		if (state)
			dwState = state;

		memset(&ss, 0, sizeof(SERVICE_STATUS));
		ss.dwServiceType |= SERVICE_WIN32_OWN_PROCESS;
		ss.dwCurrentState = dwState;
		ss.dwControlsAccepted = SERVICE_ACCEPT_STOP |
					SERVICE_ACCEPT_SHUTDOWN;
		ss.dwCheckPoint = 0;
		ss.dwServiceSpecificExitCode = 0;
		ss.dwWin32ExitCode = NO_ERROR;
		ss.dwWaitHint = dwState == SERVICE_STOP_PENDING ? 5000 : 1000;

		SetServiceStatus(hServiceStatus, &ss);
	}
}

BOOL WINAPI 
OnConsoleEvent(  
	DWORD dwCtrlType
	)
{
	HANDLE exitEvent = get_exit_event();

	switch (dwCtrlType) {
#ifdef DEBUG
		case CTRL_BREAK_EVENT:
			if (debug > 0) {
				debug <<= 1;
			}
			else {
				debug = 1;
			}
			if (debug > 8) {
				debug = 0;
			}
			msyslog(LOG_DEBUG, "debug level %d", debug);
			break;
#else
		case CTRL_BREAK_EVENT:
			break;
#endif

		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			if (exitEvent != NULL) {
				SetEvent(exitEvent);
				Sleep( 100 );  //##++
			}
			break;

		default :
			/* pass to next handler */
			return FALSE; 
	}

	/* we've handled it, no more handlers should be called */
	return TRUE;;
}

