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
#ifdef DEBUG
#include <crtdbg.h>
#endif

/* Handle to SCM for updating service status */
static SERVICE_STATUS_HANDLE hServiceStatus = 0;
static BOOL foreground = FALSE;
static char ConsoleTitle[128];
static int glb_argc;
static char **glb_argv;
HANDLE hServDoneEvent = NULL;
extern volatile int debug;
extern char *progname;

void uninit_io_completion_port();
int ntpdmain(int argc, char *argv[]);
/*
 * Forward declarations
 */
void ServiceControl(DWORD dwCtrlCode);
void ntservice_exit(void);

void WINAPI service_main( DWORD argc, LPTSTR *argv )
{
  /* pass the global command line options on to the service */
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

	/* Command line users should put -f in the options */
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
		exit(ntpdmain(argc, argv));
	} else {
		/* Start up as service */

		SERVICE_TABLE_ENTRY dispatchTable[] = {
			{ TEXT(NTP_DISPLAY_NAME), (LPSERVICE_MAIN_FUNCTION) service_main },
			{ NULL, NULL }
		};

		rc = StartServiceCtrlDispatcher(dispatchTable);
		if (!rc) {
			progname = argv[0];
			rc = GetLastError();
#ifdef DEBUG
			fprintf(stderr, "%s: unable to start as service, rc: %i\n\n", progname, rc);
#endif
			fprintf(stderr, "\nUse -d, -q, --help or -n to run from the command line.\n");
			exit(rc);
		}
	}
	exit(0);
}

/*
 * Initialize the Service by registering it.
 */
void
ntservice_init() {
	if (!foreground) {
		/* Register handler with the SCM */
		hServiceStatus = RegisterServiceCtrlHandler(NTP_DISPLAY_NAME,
					(LPHANDLER_FUNCTION)ServiceControl);
		if (!hServiceStatus) {
			NTReportError(NTP_SERVICE_NAME,
				"could not register service control handler");
			UpdateSCM(SERVICE_STOPPED);
			exit(1);
		}
		UpdateSCM(SERVICE_RUNNING);
	} else {
		strcpy(ConsoleTitle, "NTP Version ");
		strcat(ConsoleTitle, Version);
		SetConsoleTitle(ConsoleTitle);
	}

	atexit( ntservice_exit );
}
/*
 * Routine to check if this is a service or a foreground program
 */
BOOL
ntservice_isservice() {
	return(!foreground);
}
/* service_ctrl - control handler for NTP service
 * signals the service_main routine of start/stop requests
 * from the control panel or other applications making
 * win32API calls
 */
void
ntservice_exit( void )
{

	if (!foreground) { /* did not become a service, simply exit */
		/* service mode, need to have the service_main routine
		 * register with the service control manager that the 
		 * service has stopped running, before exiting
		 */
		UpdateSCM(SERVICE_STOPPED);
	}
	uninit_io_completion_port();
	Sleep( 200 );  	//##++ 

	reset_winnt_time();

	msyslog(LOG_INFO, "ntservice: The Network Time Protocol Service has stopped.");

# ifdef DEBUG
	_CrtDumpMemoryLeaks();
# endif 
}

/* 
 * ServiceControl(): Handles requests from the SCM and passes them on
 * to the service.
 */
void
ServiceControl(DWORD dwCtrlCode) {
	/* Handle the requested control code */
	HANDLE exitEvent = get_exit_event();

	switch(dwCtrlCode) {

	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP:
		UpdateSCM(SERVICE_STOP_PENDING);
		if (exitEvent != NULL) {
			SetEvent(exitEvent);
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

		if (!SetServiceStatus(hServiceStatus, &ss)) {
			ss.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(hServiceStatus, &ss);
		}
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
		case CTRL_BREAK_EVENT :
			if (debug > 0) {
				debug <<= 1;
			}
			else {
				debug = 1;
			}
			if (debug > 8) {
				debug = 0;
			}
			printf("debug level %d\n", debug);
		break ;
#endif

		case CTRL_C_EVENT  :
		case CTRL_CLOSE_EVENT :
		case CTRL_SHUTDOWN_EVENT :
			if (exitEvent != NULL) {
				SetEvent(exitEvent);
				Sleep( 100 );  //##++
			}
		break;

		default :
			return FALSE;


	}
	return TRUE;;
}

