/* This file implementes system calls that are not compatible with UNIX */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#include <stdio.h>
#include "ntp_machine.h"
#include "ntp_stdlib.h"
#include "ntp_syslog.h"
#include "ntp_fp.h"
#include "ntp.h"
#include "ntp_refclock.h"
#include "win32_io.h"


int NT_set_process_priority(void)
{
	DWORD  SingleCPUMask = 0;
	DWORD ProcessAffinityMask, SystemAffinityMask;
	if (!GetProcessAffinityMask(GetCurrentProcess(), &ProcessAffinityMask, &
		 SystemAffinityMask))
		msyslog(LOG_ERR, "GetProcessAffinityMask: %m");
	else {
		SingleCPUMask = 1; 
# ifdef DEBUG 
	msyslog(LOG_INFO, "System AffinityMask = %x", SystemAffinityMask); 
# endif 
		}

	while (SingleCPUMask && !(SingleCPUMask & SystemAffinityMask)) 
		SingleCPUMask = SingleCPUMask << 1; 
		
	if (!SingleCPUMask) 
		msyslog(LOG_ERR, "Can't set Processor Affinity Mask"); 
	else if (!SetProcessAffinityMask(GetCurrentProcess(), SingleCPUMask)) 
		msyslog(LOG_ERR, "SetProcessAffinityMask: %m"); 
# ifdef DEBUG 
	else msyslog(LOG_INFO,"ProcessorAffinity Mask: %x", SingleCPUMask ); 
# endif 
	if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) 
		{
		msyslog(LOG_ERR, "SetPriorityClass: %m"); 
		return 0;
		}
	else 
		return 1;
}

/*
 * refclock_open - open serial port for reference clock
 *
 * This routine opens a serial port for I/O and sets default options. It
 * returns the file descriptor if success and zero if failure.
 */
int
refclock_open(
	char *dev,		/* device name pointer */
	u_int speed,		/* serial port speed (code) */
	u_int flags		/* line discipline flags */
	)
{
	HANDLE Handle = INVALID_HANDLE_VALUE;
	COMMTIMEOUTS timeouts;
	DCB dcb = {0};

	//
	// open communication port handle
	//
	Handle = CreateFile(dev,
		GENERIC_READ | GENERIC_WRITE,
		0, // no sharing
		NULL, // no security
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL); // not template
	if (Handle == INVALID_HANDLE_VALUE) {  
		 
		msyslog(LOG_ERR, "NT_COM: Device %s: CreateFile error: %m", dev);
		return -1;
	}

	/*  Change the input/output buffers to be large.
	*/
	if (!SetupComm( Handle, 1024, 1024)) {
		msyslog(LOG_ERR, "NT_COM: Device %s: SetupComm error: %m", dev);
		return -1;
	}

	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(Handle, &dcb)) {
		// Error getting current DCB settings
		msyslog(LOG_ERR, "NT_COM: Device %s: GetCommState error: %m", dev);
		return -1;
	}

	switch (speed) {
	  case B300 :   dcb.BaudRate = 300; break;
	  case B1200 :  dcb.BaudRate = 1200; break;
	  case B2400 :  dcb.BaudRate = 2400; break;
	  case B4800 :  dcb.BaudRate = 4800; break;
	  case B9600 :  dcb.BaudRate = 9600; break;
	  case B19200 : dcb.BaudRate = 19200; break;
	  case B38400 : dcb.BaudRate = 38400; break;
	  default :
		msyslog(LOG_ERR, "NT_COM: Device %s: unsupported baud rate", dev);
		return -1;
	}


	dcb.ByteSize = 8;
	dcb.fBinary = TRUE;
	dcb.fParity = TRUE;
	dcb.fOutxCtsFlow = 0;
	dcb.fOutxDsrFlow = 0;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = 0;
	dcb.fTXContinueOnXoff = FALSE;
	dcb.fOutX = 0; 
	dcb.fInX = 0;
	dcb.fErrorChar = 0;
	dcb.fNull = 0;
	dcb.fRtsControl = RTS_CONTROL_DISABLE; // RTS_CONTROL_DISABLE;
	dcb.fAbortOnError = 0;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = NOPARITY;
	dcb.ErrorChar = 0;
	dcb.EvtChar = 0;
	dcb.EofChar = 0;

	if (!SetCommState(Handle, &dcb)) {
		msyslog(LOG_ERR, "NT_COM: Device %s: SetCommState error: %m", dev);
		return -1;
	}

	timeouts.ReadIntervalTimeout = 20; 
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 5000;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 5000;

	   // Error setting time-outs.
	if (!SetCommTimeouts(Handle, &timeouts)) {
		msyslog(LOG_ERR, "NT_COM: Device %s: SetCommTimeouts error: %m", dev);
		return -1;
	}

	return (int) Handle;
}


int 
ioctl(int fd,
	  int cmd,
	  int *x) {

	if ((cmd == TIOCMSET) && (*x & TIOCM_RTS)) {
		if (!EscapeCommFunction((HANDLE) fd, SETRTS)) 
			return -1;
	}
	else if ((cmd == TIOCMSET) && !(*x & TIOCM_RTS)){
		if (!EscapeCommFunction((HANDLE) fd, CLRRTS)) 
			return -1;
	}

	return 0;
}


int	
tcsetattr(
	int fd, 
	int optional_actions, 
	const struct termios * s)
{
	DCB dcb = { 0 };
	HANDLE Handle = (HANDLE) fd;
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(Handle, &dcb)) {
		// Error getting current DCB settings
		msyslog(LOG_ERR, "NT_COM: GetCommState error: %m");
		return FALSE;
	}

	switch (max(s->c_ospeed, s->c_ispeed)) {
		case B300 :   dcb.BaudRate = 300; break;
		case B1200 :  dcb.BaudRate = 1200; break;
		case B2400 :  dcb.BaudRate = 2400; break;
		case B4800 :  dcb.BaudRate = 4800; break;
		case B9600 :  dcb.BaudRate = 9600; break;
		case B19200 : dcb.BaudRate = 19200; break;
		case B38400 : dcb.BaudRate = 38400; break;
		default :
			msyslog(LOG_ERR, "NT_COM: unsupported baud rate");
			return FALSE;
	}

	switch (s->c_cflag & CSIZE) {
		case CS5 : dcb.ByteSize = 5; break;
		case CS6 : dcb.ByteSize = 6; break;
		case CS7 : dcb.ByteSize = 7; break;
		case CS8 : dcb.ByteSize = 8; break;
		default :
			msyslog(LOG_ERR, "NT_COM: unsupported word size");
			return FALSE;
	}

	if (s->c_cflag & PARENB) {
		dcb.fParity = TRUE;
		if (s->c_cflag & PARODD) {
			dcb.Parity = ODDPARITY;
		}
		else {
			dcb.Parity = EVENPARITY;
		}
	}
	else {
		dcb.fParity = FALSE;
		dcb.Parity = NOPARITY;
	}


	dcb.fOutxCtsFlow = 0;
	dcb.fOutxDsrFlow = 0;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = 0;
	dcb.fOutX = 0; 
	dcb.fInX = 0;
	dcb.fErrorChar = 0;
	dcb.fNull = 0;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fAbortOnError = 0;
	dcb.ErrorChar = 0;
	dcb.EvtChar = 0;
	dcb.EofChar = 0;

	if (!SetCommState(Handle, &dcb)) {
		msyslog(LOG_ERR, "NT_COM: SetCommState error: %m");
		return FALSE;
	}
	return TRUE;
}

extern	int	
tcgetattr(
	int fd, struct termios *s)
{
	DCB dcb = { 0 };
	HANDLE Handle = (HANDLE) fd;
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(Handle, &dcb)) {
		// Error getting current DCB settings
		msyslog(LOG_ERR, "NT_COM: GetCommState error: %m");
		return FALSE;
	}

	/*  Set c_ispeed & c_ospeed
 	*/
	switch (dcb.BaudRate) {
		case 300 : s->c_ispeed = s->c_ospeed = B300; break;
		case 1200 : s->c_ispeed = s->c_ospeed = B1200; break;
		case 2400 : s->c_ispeed = s->c_ospeed = B2400; break;
		case 4800 : s->c_ispeed = s->c_ospeed = B4800; break;
		case 9600 : s->c_ispeed = s->c_ospeed = B9600; break;
		case 19200 : s->c_ispeed = s->c_ospeed = B19200; break;
		case 38400 : s->c_ispeed = s->c_ospeed = B38400; break;
		default : s->c_ispeed = s->c_ospeed = B9600;
	}
	

	s->c_cflag = 0;
	switch (dcb.ByteSize) {
		case 5 : s->c_cflag |= CS5; break;
		case 6 : s->c_cflag |= CS6; break;
		case 7 : s->c_cflag |= CS7; break;
		case 8 : s->c_cflag |= CS8; break;
	}
	if (dcb.fParity) {
		  s->c_cflag |= PARENB;
	}
	switch (dcb.Parity) {
		case EVENPARITY : break;
		case MARKPARITY : break;
		case NOPARITY : break;
		case ODDPARITY : s->c_cflag |= PARODD; break;
		case SPACEPARITY : break;
	}

	s->c_iflag = 0;
	s->c_lflag = 0;
	s->c_line = 0;
	s->c_oflag = 0;

	return TRUE; /* ok */
}


extern int tcflush(int fd, int mode) {




return 0;

}

extern int cfsetispeed(struct termios *tio, int speed) {
		
return 0;		
};	


extern int cfsetospeed(struct termios *tio, int speed) {
		
return 0;		
};	

