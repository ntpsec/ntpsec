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

#include "syslog.h"
#include "ntservice.h"

/* Handle to SCM for updating service status */
static SERVICE_STATUS_HANDLE hServiceStatus = 0;
static BOOL foreground = FALSE;
static char ConsoleTitle[128];
extern char *Version;
HANDLE hServDoneEvent = NULL;

/*
 * Forward declarations
 */
void ServiceControl(DWORD dwCtrlCode);
void GetArgs(int *, char ***, char ***);
int ntpdmain(int, char *[]);
//int main(int, char *[], char *[]); /* From ns_main.c */

/*
 * Here we change the entry point for the executable to NTmain() from main()
 * This allows us to invoke as a service or from the command line easily.
 */
#pragma comment(linker, "/entry:NTmain")

/*
 * This is the entry point for the executable 
 * We can now call main() explicitly or via StartServiceCtrlDispatcher()
 * as we need to.
 */
int NTmain()
{
	int rc,
	i = 1;

	int argc;
	char **envp, **argv;

	/*
	 * We changed the entry point function, so we must initialize argv,
	 * etc. ourselves.  Ick.
	 */
	GetArgs(&argc, &argv, &envp);

	/* Command line users should put -f in the options */
	while (argv[i]) {
		if (!_strnicmp(argv[i], "-d", 2) ||
			!strcmp(argv[i], "-q") ||
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
			{ TEXT(NTP_DISPLAY_NAME), (LPSERVICE_MAIN_FUNCTION)ntpdmain },
			{ NULL, NULL }
		};

		rc = StartServiceCtrlDispatcher(dispatchTable);
		if (!rc) {
			fprintf(stderr, "Use -f to run from the command line.\n");
			exit(GetLastError());
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
}

void
ntservice_shutdown() {
	UpdateSCM(SERVICE_STOPPED);
}
/*
 * Routine to check if this is a service or a foreground program
 */
BOOL
ntservice_isservice() {
	return(!foreground);
}
void
ntservice_exit(int status)
{
	extern int debug;

	if (debug) /* did not become a service, simply exit */
	    ExitThread((DWORD)status);
	else {
		/* service mode, need to have the service_main routine
		 * register with the service control manager that the 
		 * service has stopped running, before exiting
		 */
		if ((status > 0) && (hServDoneEvent != NULL))
		    SetEvent(hServDoneEvent);
		ExitThread((DWORD)status);
	}
}

/* 
 * ServiceControl(): Handles requests from the SCM and passes them on
 * to named.
 */
void
ServiceControl(DWORD dwCtrlCode) {
	/* Handle the requested control code */
	switch(dwCtrlCode) {
        case SERVICE_CONTROL_INTERROGATE:
		UpdateSCM(0);
		break;

        case SERVICE_CONTROL_SHUTDOWN:
        case SERVICE_CONTROL_STOP:
		ntservice_exit(0);
		UpdateSCM(SERVICE_STOPPED);
		break;
        default:
		break;
	}
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
		ss.dwWaitHint = dwState == SERVICE_STOP_PENDING ? 10000 : 1000;

		if (!SetServiceStatus(hServiceStatus, &ss)) {
			ss.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(hServiceStatus, &ss);
		}
	}
}

/*
 * C-runtime stuff used to initialize the app and
 * get argv, argc, envp.
 */

typedef struct 
{
	int newmode;
} _startupinfo;

_CRTIMP void __cdecl __set_app_type(int);
 void __getmainargs(int *, char ***, char ***, int,
				   _startupinfo *);

extern int _newmode;		/* malloc new() handler mode */

typedef void (__cdecl *_PVFV)(void);
extern _PVFV *__onexitbegin;
extern _PVFV *__onexitend;
extern _CRTIMP char **__initenv;

/*
 * Do the work that mainCRTStartup() would normally do
 */
void GetArgs(int *argc, char ***argv, char ***envp)
{
	int dowildcard = 0;		/* passed to __getmainargs() */
	_startupinfo startinfo;
    
	/*
	 * Set the app type to Console (check CRT/SRC/INTERNAL.H:
	 * #define _CONSOLE_APP 1)
	 */
	__set_app_type(1);
	
	/* Mark this module as an EXE file */
	__onexitbegin = __onexitend = (_PVFV *)(-1);

	startinfo.newmode = _newmode;
	__getmainargs(argc, argv, envp, dowildcard, &startinfo);
	__initenv = *envp;

}
