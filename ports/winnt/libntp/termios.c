/* This file implements system calls that are not compatible with UNIX */

#include <config.h>
#include <io.h>
#include <stdio.h>
#include "ntp_machine.h"
#include "ntp_stdlib.h"
#include "ntp_syslog.h"
#include "ntp_debug.h"
#include "ntp_fp.h"
#include "ntp.h"
#include "ntp_refclock.h"
#include "win32_io.h"

#define MAX_SERIAL 255	/* COM1: - COM255: */


/*
 * common_serial_open ensures duplicate opens of the same port
 * work by duplicating the handle for the 2nd open, allowing
 * refclock_atom to share a GPS refclock's comm port.
 */

HANDLE common_serial_open(
	char *	dev,
	char **	pwindev
	)
{
	static HANDLE *	hnds = NULL;	/* handle array */
	static int	c_hnd = 0;	/* current array size */
	static char	windev[32];	/* return pointer into this */
	HANDLE		handle;
	int		unit;
	int		prev_c_hnd;
	char *		pch;

	/*
	 * This is odd, but we'll take any unix device path
	 * by looking for the initial '/' and strip off everything
	 * before the final digits, then translate that to COM__:
	 * maintaining backward compatibility with NTP practice of
	 * mapping unit 0 to the nonfunctional COM0:
	 *
	 * To ease the job of taking the windows COMx: device names
	 * out of reference clocks, we'll also work with those
	 * equanimously.
	 */

	DPRINTF(1, ("common_serial_open given %s\n", dev));

	pch = NULL;
	if ('/' == dev[0]) {
		pch = dev + strlen(dev) - 1;

		//DPRINTF(1, ("common_serial_open initial %s\n", pch));
		if (isdigit(pch[0])) {
			while (isdigit(pch[0])) {
				pch--;
				//DPRINTF(1, ("common_serial_open backed up to %s\n", pch));
			}
			pch++;
		}
		DPRINTF(1, ("common_serial_open skipped to ending digits leaving %s\n", pch));
	} else if ('c' == tolower(dev[0])
		   && 'o' == tolower(dev[1])
		   && 'm' == tolower(dev[2])) {
		pch = dev + 3;
		DPRINTF(1, ("common_serial_open skipped COM leaving %s\n", pch));
	}

	if (!pch || !isdigit(pch[0])) {
		DPRINTF(1, ("not a digit: %s\n", pch ? pch : "[NULL]"));
		return INVALID_HANDLE_VALUE;
	}

	if (1 != sscanf(pch, "%d", &unit) 
	    || unit > MAX_SERIAL
	    || unit < 0) {
		DPRINTF(1, ("sscanf failure of %s\n", pch));
		return INVALID_HANDLE_VALUE;
	}


	if (c_hnd < unit + 1) {
		prev_c_hnd = c_hnd;
		c_hnd = unit + 1;
		/* round up to closest multiple of 4 to avoid churn */
		c_hnd = (c_hnd + 3) & ~3;
		hnds = erealloc(hnds, c_hnd * sizeof hnds[0]);
		memset(&hnds[prev_c_hnd], 0, 
		       (c_hnd - prev_c_hnd) * sizeof hnds[0]);
	}

	if (NULL == hnds[unit]) {
		snprintf(windev, sizeof(windev), "\\\\.\\COM%d", unit);
		DPRINTF(1, ("windows device %s\n", windev));
		*pwindev = windev;
		hnds[unit] = CreateFile(
				windev,
				GENERIC_READ | GENERIC_WRITE,
				0, /* sharing prohibited */
				NULL, /* default security */
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL
				    | FILE_FLAG_OVERLAPPED,
				NULL);
	}

	if (INVALID_HANDLE_VALUE == hnds[unit]) {
		hnds[unit] = NULL;
		handle = INVALID_HANDLE_VALUE;
	} else
		DuplicateHandle(
			GetCurrentProcess(),
			hnds[unit],
			GetCurrentProcess(),
			&handle,
			0,
			FALSE,
			DUPLICATE_SAME_ACCESS
			);

	return handle;
}

/*
 * tty_open - open serial port for refclock special uses
 *
 * This routine opens a serial port for and returns the 
 * file descriptor if success and -1 if failure.
 */
int tty_open(
	char *dev,		/* device name pointer */
	int access,		/* O_RDWR */
	int mode		/* unused */
	)
{
	HANDLE	Handle;
	char *	windev;

	/*
	 * open communication port handle
	 */
	windev = NULL;
	Handle = common_serial_open(dev, &windev);
	windev = (windev) ? windev : dev;

	if (Handle == INVALID_HANDLE_VALUE) {  
		msyslog(LOG_ERR, "tty_open: device %s CreateFile error: %m", windev);
		errno = EMFILE; /* lie, lacking conversion from GetLastError() */
		return -1;
	}

	return (int) _open_osfhandle((int)Handle, _O_TEXT);
}

/*
 * refclock_open - open serial port for reference clock
 *
 * This routine opens a serial port for I/O and sets default options. It
 * returns the file descriptor or -1 indicating failure.
 */
int refclock_open(
	char *	dev,		/* device name pointer */
	u_int	speed,		/* serial port speed (code) */
	u_int	flags		/* line discipline flags */
	)
{
	char *		windev;
	HANDLE		h;
	COMMTIMEOUTS	timeouts;
	DCB		dcb;

	/*
	 * open communication port handle
	 */
	windev = NULL;
	h = common_serial_open(dev, &windev);
	windev = (windev) ? windev : dev;

	if (INVALID_HANDLE_VALUE == h) {  
		msyslog(LOG_ERR, "Device %s CreateFile error: %m", windev);
		return -1;
	}

	/* Change the input/output buffers to be large. */
	if (!SetupComm(h, 1024, 1024)) {
		msyslog(LOG_ERR, "Device %s SetupComm error: %m", windev);
		return -1;
	}

	dcb.DCBlength = sizeof(dcb);

	if (!GetCommState(h, &dcb)) {
		msyslog(LOG_ERR, "Device %s GetCommState error: %m",
				 windev);
		return -1;
	}

	switch (speed) {

	case B300:
		dcb.BaudRate = 300;
		break;

	case B1200:  
		dcb.BaudRate = 1200;
		break;

	case B2400:
		dcb.BaudRate = 2400;
		break;

	case B4800:
		dcb.BaudRate = 4800;
		break;

	case B9600:
		dcb.BaudRate = 9600;
		break;

	case B19200:
		dcb.BaudRate = 19200;
		break;

	case B38400:
		dcb.BaudRate = 38400;
		break;

	case B57600:
		dcb.BaudRate = 57600;
		break;

	case B115200:
		dcb.BaudRate = 115200;
		break;

	default:
		msyslog(LOG_ERR, "Device %s unsupported baud rate "
				 "code %u", windev, speed);
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
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fAbortOnError = 0;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = NOPARITY;
	dcb.ErrorChar = 0;
	dcb.EvtChar = 13; /* CR */
	dcb.EofChar = 0;

	if (!SetCommState(h, &dcb)) {
		msyslog(LOG_ERR, "Device %s SetCommState error: %m", windev);
		return -1;
	}

	/* watch out for CR (dcb.EvtChar) as well as the CD line */
	if (!SetCommMask(h, EV_RXFLAG | EV_RLSD)) {
		msyslog(LOG_ERR, "Device %s SetCommMask error: %m", windev);
		return -1;
	}

	/* configure the handle to never block */
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;

	if (!SetCommTimeouts(h, &timeouts)) {
		msyslog(LOG_ERR, "Device %s SetCommTimeouts error: %m", windev);
		return -1;
	}

	return (int)_open_osfhandle((int)h, _O_TEXT);
}


int
ioctl_tiocmget(
	HANDLE h,
	int *pi
	)
{
	DWORD	dw;

	if (!GetCommModemStatus(h, &dw)) {
		errno = ENOTTY;
		return -1;
	}

	*pi = ((dw & MS_CTS_ON)  ? TIOCM_CTS : 0)
	    | ((dw & MS_DSR_ON)  ? TIOCM_DSR : 0)
	    | ((dw & MS_RLSD_ON) ? TIOCM_CAR : 0)
	    | ((dw & MS_RING_ON) ? TIOCM_RI  : 0);

	return 0;
}


int
ioctl_tiocmset(
	HANDLE h,
	int *pi
	)
{
	BOOL	failed;
	int	result;
	
	failed = EscapeCommFunction(
			h, 
			(*pi & TIOCM_RTS) 
			    ? SETRTS
			    : CLRRTS
			);

	if (!failed)
		failed = EscapeCommFunction(
				h, 
				(*pi & TIOCM_DTR) 
				    ? SETDTR
				    : CLRDTR
				);

	if (failed) {
		errno = ENOTTY;
		result = -1;
	} else
		result = 0;

	return result;
}


int 
ioctl(
	int fd,
	int op,
	int *pi
	)
{
	HANDLE	h;
	int	result;
	
	h = (HANDLE)_get_osfhandle(fd);

	if (INVALID_HANDLE_VALUE == h) {
		/* errno already set */
		return -1;
	}

	switch (op) {

	case TIOCMGET:
		result = ioctl_tiocmget(h, pi);
		break;

	case TIOCMSET:
		result = ioctl_tiocmset(h, pi);
		break;

	default:
		errno = EINVAL;
		result = -1;
	}

	return result;
}


int	
tcsetattr(
	int			fd, 
	int			optional_actions, 
	const struct termios *	tios
	)
{
	DCB dcb;
	HANDLE h;

	UNUSED_ARG(optional_actions);

	h = (HANDLE)_get_osfhandle(fd);

	if (INVALID_HANDLE_VALUE == h) {
		/* errno already set */
		return -1;
	}

	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(h, &dcb)) {
		errno = ENOTTY;
		return -1;
	}

	switch (max(tios->c_ospeed, tios->c_ispeed)) {

	case B300:
		dcb.BaudRate = 300;
		break;

	case B1200:
		dcb.BaudRate = 1200;
		break;

	case B2400:
		dcb.BaudRate = 2400;
		break;

	case B4800:
		dcb.BaudRate = 4800;
		break;

	case B9600:
		dcb.BaudRate = 9600;
		break;

	case B19200:
		dcb.BaudRate = 19200;
		break;

	case B38400:
		dcb.BaudRate = 38400;
		break;

	case B57600:
		dcb.BaudRate = 57600;
		break;

	case B115200:
		dcb.BaudRate = 115200;
		break;

	default:
		msyslog(LOG_ERR, "unsupported serial baud rate");
		errno = EINVAL;
		return -1;
	}

	switch (tios->c_cflag & CSIZE) {

	case CS5:
		dcb.ByteSize = 5;
		break;

	case CS6:
		dcb.ByteSize = 6;
		break;

	case CS7:
		dcb.ByteSize = 7;
		break;

	case CS8:
		dcb.ByteSize = 8;
		break;

	default:
		msyslog(LOG_ERR, "unsupported serial word size");
		errno = EINVAL;
		return FALSE;
	}

	if (PARENB & tios->c_cflag) {
		dcb.fParity = TRUE;
		dcb.Parity = (tios->c_cflag & PARODD)
				? ODDPARITY
				: EVENPARITY;
	} else {
		dcb.fParity = FALSE;
		dcb.Parity = NOPARITY;
	}

	dcb.StopBits = (CSTOPB & tios->c_cflag)
			? TWOSTOPBITS
			: ONESTOPBIT;

	if (!SetCommState(h, &dcb)) {
		errno = ENOTTY;
		return -1;
	}

	return 0;
}


int
tcgetattr(
	int		fd,
	struct termios *tios
	)
{
	DCB	dcb;
	HANDLE	h;

	h = (HANDLE)_get_osfhandle(fd);

	if (INVALID_HANDLE_VALUE == h) {
		/* errno already set */
		return -1;
	}

	dcb.DCBlength = sizeof(dcb);

	if (!GetCommState(h, &dcb)) {
		errno = ENOTTY;
		return -1;
	}

	/*  Set c_ispeed & c_ospeed */

	switch (dcb.BaudRate) {

	case 300:
		tios->c_ispeed = tios->c_ospeed = B300;
		break;

	case 1200: 
		tios->c_ispeed = tios->c_ospeed = B1200;
		break;

	case 2400:
		tios->c_ispeed = tios->c_ospeed = B2400;
		break;

	case 4800: 
		tios->c_ispeed = tios->c_ospeed = B4800;
		break;

	case 9600:
		tios->c_ispeed = tios->c_ospeed = B9600;
		break;

	case 19200:
		tios->c_ispeed = tios->c_ospeed = B19200;
		break;

	case 38400:
		tios->c_ispeed = tios->c_ospeed = B38400;
		break;

	case 57600:
		tios->c_ispeed = tios->c_ospeed = B57600;
		break;

	case 115200:
		tios->c_ispeed = tios->c_ospeed = B115200;
		break;

	default:
		tios->c_ispeed = tios->c_ospeed = B9600;
	}
	

	switch (dcb.ByteSize) {
		case 5:
			tios->c_cflag = CS5;
			break;

		case 6:
			tios->c_cflag = CS6;
			break;

		case 7: 
			tios->c_cflag = CS7; 
			break;

		case 8:
		default:
			tios->c_cflag = CS8;
	}

	if (dcb.fParity) {
		tios->c_cflag |= PARENB;

		if (ODDPARITY == dcb.Parity)
			tios->c_cflag |= PARODD;
	}

	if (TWOSTOPBITS == dcb.StopBits)
		tios->c_cflag |= CSTOPB;

	tios->c_iflag = 0;
	tios->c_lflag = 0;
	tios->c_line = 0;
	tios->c_oflag = 0;

	return 0;
}


int
tcflush(
	int fd,
	int mode
	)
{
	HANDLE	h;
	BOOL	success;
	DWORD	flags;
	int	result;

	h = (HANDLE)_get_osfhandle(fd);

	if (INVALID_HANDLE_VALUE == h) {
		/* errno already set */
		return -1;
	}

	switch (mode) {

	case TCIFLUSH:
		flags = PURGE_RXCLEAR;
		break;

	case TCOFLUSH:
		flags = PURGE_TXABORT;
		break;

	case TCIOFLUSH:
		flags = PURGE_RXCLEAR | PURGE_TXABORT;
		break;

	default:
		errno = EINVAL;
		return -1;
	}

	success = PurgeComm(h, flags);

	if (success)
		result = 0;
	else {
		errno = ENOTTY;
		result = -1;
	}

	return result;
}

