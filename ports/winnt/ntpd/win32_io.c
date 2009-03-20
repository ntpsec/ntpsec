/* This file implements system calls that are not compatible with UNIX */

#include <config.h>
#include <stdio.h>
#include "ntp_machine.h"
#include "ntp_stdlib.h"
#include "ntp_syslog.h"
#include "ntp_debug.h"
#include "ntp_fp.h"
#include "ntp.h"
#include "ntp_refclock.h"
#include "win32_io.h"

#define MAX_SERIAL 16	/* COM1-COM16 */


int NT_set_process_priority(void)
{
	if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) 
		{
		msyslog(LOG_ERR, "SetPriorityClass: %m"); 
		return 0;
		}
	else 
		return 1;
}


/*
 * common_serial_open ensures duplicate opens of the same port
 * work by duplicating the handle for the 2nd open, allowing
 * refclock_atom to share a GPS refclock's comm port.
 *
 */

HANDLE common_serial_open(
	char *dev
	)
{
	static HANDLE SerialHandles[MAX_SERIAL+1] = {0};
	HANDLE RetHandle;
	int unit;

	if (1 != sscanf(dev, "COM%d:", &unit) || unit > MAX_SERIAL)
		return INVALID_HANDLE_VALUE;

	if (!SerialHandles[unit])
		SerialHandles[unit] = CreateFile(
					dev,
					GENERIC_READ | GENERIC_WRITE,
					0, /* sharing prohibited */
					NULL, /* default security */
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
					NULL);

	if (INVALID_HANDLE_VALUE == SerialHandles[unit]) {
		SerialHandles[unit] = 0;
		return INVALID_HANDLE_VALUE;
	}

	DuplicateHandle(
		GetCurrentProcess(),
		SerialHandles[unit],
		GetCurrentProcess(),
		&RetHandle,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS
		);

	return RetHandle;
}

/*
 * pps_open - open serial port for PPS
 *
 * This routine opens a serial port for and returns the 
 * file descriptor if success and -1 if failure.
 */
int pps_open(
	char *dev,		/* device name pointer */
	int access,		/* O_RDWR */
	int mode		/* unused */
	)
{
	HANDLE Handle;
	char windev[3 + 20 + 1 + 1];
	int unit;

	if (1 != sscanf(dev, "/dev/pps%d", &unit)) {
		errno = ENOENT;
		return -1;
	}
	/*
	 * there never is a COM0: but this is the ntp convention
	 */
	_snprintf(windev, sizeof(windev)-1, "COM%d:", unit);
	windev[sizeof(windev)-1] = 0; 

	/*
	 * open communication port handle
	 */
	Handle = common_serial_open(windev);

	if (Handle == INVALID_HANDLE_VALUE) {  
		msyslog(LOG_ERR, "pps_open: Device %s CreateFile error: %m", windev);
		errno = EMFILE; /* lie, lacking conversion from GetLastError() */
		return -1;
	}

	return (int) _open_osfhandle((int)Handle, _O_TEXT);
}

/*
 * refclock_open - open serial port for reference clock
 *
 * This routine opens a serial port for I/O and sets default options. It
 * returns the file descriptor if success and zero if failure.
 */
int refclock_open(
	char *dev,		/* device name pointer */
	u_int speed,		/* serial port speed (code) */
	u_int flags		/* line discipline flags */
	)
{
	HANDLE Handle = INVALID_HANDLE_VALUE;
	COMMTIMEOUTS timeouts;
	DCB dcb;

	/*
	 * open communication port handle
	 */
	Handle = common_serial_open(dev);

	if (Handle == INVALID_HANDLE_VALUE) {  
		 
		msyslog(LOG_ERR, "NT_COM: Device %s CreateFile error: %m", dev);
		return -1;
	}

	/*  Change the input/output buffers to be large.
	*/
	if (!SetupComm( Handle, 1024, 1024)) {
		msyslog(LOG_ERR, "NT_COM: Device %s SetupComm error: %m", dev);
		return -1;
	}

	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(Handle, &dcb)) {
		// Error getting current DCB settings
		msyslog(LOG_ERR, "NT_COM: Device %s GetCommState error: %m", dev);
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
	  case B57600 : dcb.BaudRate = 57600; break;
	  case B115200 : dcb.BaudRate = 115200; break;
	  default :
		msyslog(LOG_ERR, "NT_COM: Device %s unsupported baud rate", dev);
		return -1;
	}


	dcb.fBinary = TRUE;
	dcb.fParity = TRUE;
	dcb.fOutxCtsFlow = 0;
	dcb.fOutxDsrFlow = 0;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = 0;
	dcb.fTXContinueOnXoff = TRUE;
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
	dcb.EvtChar = 13; /* CR */
	dcb.EofChar = 0;

	if (!SetCommState(Handle, &dcb)) {
		msyslog(LOG_ERR, "NT_COM: Device %s SetCommState error: %m", dev);
		return -1;
	}

	/* watch out for CR (dcb.EvtChar) as well as the CD line */
	if (!SetCommMask(Handle, EV_RXFLAG | EV_RLSD)) {
		msyslog(LOG_ERR, "NT_COM: Device %s SetCommMask error: %m", dev);
		return -1;
	}

	/* configure the handle to never block */
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;

	if (!SetCommTimeouts(Handle, &timeouts)) {
		msyslog(LOG_ERR, "NT_COM: Device %s SetCommTimeouts error: %m", dev);
		return -1;
	}

	return (int) _open_osfhandle((int)Handle, _O_TEXT);
}

int 
ioctl(int fd,
	int cmd,
	int *x)
{
	HANDLE h = (HANDLE) _get_osfhandle(fd);

	if ((cmd == TIOCMSET) && (*x & TIOCM_RTS)) {
		if (!EscapeCommFunction(h, SETRTS)) 
			return -1;
	}
	else if ((cmd == TIOCMSET) && !(*x & TIOCM_RTS)){
		if (!EscapeCommFunction(h, CLRRTS)) 
			return -1;
	}
	if ((cmd == TIOCMSET) && (*x & TIOCM_DTR)) {
		if (!EscapeCommFunction(h, SETDTR)) 
			return -1;
	}
	else if ((cmd == TIOCMSET) && !(*x & TIOCM_DTR)){
		if (!EscapeCommFunction(h, CLRDTR)) 
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
	HANDLE Handle = (HANDLE) _get_osfhandle(fd);
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
		case B57600 : dcb.BaudRate = 57600; break;
		case B115200 : dcb.BaudRate = 115200; break;
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
	if (s->c_cflag & CSTOPB ) {
		dcb.StopBits = TWOSTOPBITS;
	}
	else {
		dcb.StopBits = ONESTOPBIT;
	}

	if (!SetCommState(Handle, &dcb)) {
		msyslog(LOG_ERR, "NT_COM: SetCommState error 2: %m");
		return FALSE;
	}
	return TRUE;
}

extern	int	
tcgetattr(
	int fd, struct termios *s)
{
	DCB dcb;
	HANDLE Handle = (HANDLE) _get_osfhandle(fd);

	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(Handle, &dcb)) {
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
		case 57600 : s->c_ispeed = s->c_ospeed = B57600; break;
		case 115200 : s->c_ispeed = s->c_ospeed = B115200; break;
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
	switch (dcb.StopBits) {
		case ONESTOPBIT : break;
		case ONE5STOPBITS : break;
		case TWOSTOPBITS : s->c_cflag |= CSTOPB; break;
	}

	s->c_iflag = 0;
	s->c_lflag = 0;
	s->c_line = 0;
	s->c_oflag = 0;

	return TRUE; /* ok */
}


extern int tcflush(int fd, int mode) {
	int Result = 0;
	HANDLE h = (HANDLE) _get_osfhandle(fd);
	switch ( mode ) {
	case TCIFLUSH:
		Result = PurgeComm(h, PURGE_RXCLEAR);
		break;
	case TCOFLUSH:
		Result = PurgeComm(h, PURGE_TXABORT);
		break;
	case TCIOFLUSH:
		Result = PurgeComm(h, PURGE_RXCLEAR | PURGE_TXABORT);
		break;
	}
	if ( Result == 0 ) return -1;	/* failed */
	else return 0;					/* successful */
}


extern int cfsetispeed(struct termios *tio, int speed) {

return 0;		
};	


extern int cfsetospeed(struct termios *tio, int speed) {

return 0;		
};	

