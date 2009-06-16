#include <config.h>
#include <windows.h>
#include <stdio.h>
#include <sys/resource.h>	/* our private version */
#include "ntp_machine.h"
#include "ntp_stdlib.h"
#include "ntp_syslog.h"
#include "ntp_debug.h"
#include "ntp_fp.h"
#include "ntp.h"
#include "clockstuff.h"


/*
 * setpriority
 *
 * to reduce the #ifdef forest in the portable code,
 * we emulate the BSD setpriority interface:
 *
 * 		if (-1 == setpriority(PRIO_PROCESS, 0, NTP_PRIO))
 * 			msyslog(LOG_ERR, "setpriority() error: %m");
 *
 * However, since the Windows port of ntpd has always raised its
 * priority (to realtime if allowed, or silently downgraded to 
 * high by the system if not) with or without -N.  Changing that
 * now would endanger users who upgrade the binary without adding
 * -N to its invocation.  Instsrv assumes ntpd.exe is installed
 * with no command-line arguments.
 *
 * This routine is used by utilities as well as ntpd itself, so
 * it checks if the priority is already high or realtime and 
 * logs no complaints in that case, to avoid duplicating.  ntpd
 * will have raised the priority to one of those in
 * init_winnt_time, while the utilities will rely on this
 * code.
 *
 */

int setpriority(
		int which,
		int who,
		int prio
		)
{
	BOOL success;
	DWORD prio_class;

	if (PRIO_PROCESS != which || who || NTP_PRIO != prio) {
		DPRINTF(1,("windows setpriority() clone needs work.\n"));
	}

	prio_class = GetPriorityClass(GetCurrentProcess());
	
	if (HIGH_PRIORITY_CLASS == prio_class ||
	    REALTIME_PRIORITY_CLASS == prio_class)
		return 0;

	success = SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	if (!success) {
		msyslog(LOG_ERR, "Unable to raise priority: %m"); 
		errno = EPERM;
		return -1;
	}

	prio_class = GetPriorityClass(GetCurrentProcess());

	if (REALTIME_PRIORITY_CLASS == prio_class)
		msyslog(LOG_INFO, "Raised to realtime priority class");
	else if (HIGH_PRIORITY_CLASS == prio_class)
		msyslog(LOG_ERR,  "Raised to high priority class, realtime "
				  "requires Increase Scheduling Priority "
				  "privilege (enabled with secpol.msc).");
	else
		msyslog(LOG_ERR,  "Unexpected process priority class %d",
				 prio_class);

	return 0; 
}

/*
 * InitSockets -- once known as Win32InitSockets()
 *
 * This doesn't have much to do with setpriority but we
 * want the routine in libntp and this is a convenient
 * existing Windows-only libntp source file.
 */
void
InitSockets(
	void
	)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Need Winsock 2.0 or better */
	wVersionRequested = MAKEWORD(2, 0);
 
	err = WSAStartup(wVersionRequested, &wsaData);
	if ( err != 0 ) {
		fprintf(stderr, "No useable winsock.dll: %s\n", strerror(err));
		SetLastError(err);
		msyslog(LOG_ERR, "No usable winsock.dll: %m");
		exit(1);
	}
}
