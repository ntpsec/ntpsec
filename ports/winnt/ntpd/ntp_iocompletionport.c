
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if defined (HAVE_IO_COMPLETION_PORT)

#include <stddef.h>
#include <stdio.h>
#include <process.h>
#include "ntp_stdlib.h"
#include "syslog.h"
#include "ntp_machine.h"
#include "ntp_fp.h"
#include "ntp.h"
#include "ntpd.h"
#include "ntp_refclock.h"
#include "ntp_iocompletionport.h"
#include "transmitbuff.h"


static HANDLE hIoCompletionPort = NULL;

static int 
OnExitRequest(DWORD Key, struct IoCompletionInfo *Info, DWORD Bytes)
{
  (void) Info;
  (void) Bytes;
  (void) Key;
  return 0; /* auto-fail to request an exit */
}

static IoCompletionInfo ExitRequest;
/*  This function will add an entry to the I/O completion port
 *  that will signal the I/O thread to exit (gracefully)
 */
static void
signal_io_completion_port_exit()
{
	ExitRequest.iofunction = OnExitRequest;
	if (!PostQueuedCompletionStatus(hIoCompletionPort, 0, 0, &ExitRequest.overlapped)) {
		msyslog(LOG_ERR, "Can't request service thread to exit: %m");
		exit(1);
	}
}




static void __cdecl 
iocompletionthread(void *NotUsed)
{
	DWORD BytesTransferred = 0;
	DWORD Key = 0;
	IoCompletionInfo * Overlapped = NULL;
	(void) NotUsed;

	/*	Set the thread priority high enough so I/O will
	 *	preempt normal recv packet processing, but not
	 * 	higher than the timer sync thread.
	 */
	if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL)) {
		msyslog(LOG_ERR, "Can't set thread priority: %m");
	}

	for (;;) {
		while (GetQueuedCompletionStatus(hIoCompletionPort, &BytesTransferred, &Key, & (LPOVERLAPPED) Overlapped, INFINITE)) {
			if (Overlapped != NULL && Overlapped->iofunction != NULL) {
				if (!Overlapped->iofunction(Key, Overlapped, BytesTransferred)) {
					return ; /* fail */
				}

			}
		}
	}
}

/*  Create/initialise the I/O creation port
 */
extern void
init_io_completion_port(
	void
	)
{
	hIoCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hIoCompletionPort == NULL) {
		msyslog(LOG_ERR, "Can't create I/O completion port: %m");
		exit(1);
	}
	
	/* Have one thread servicing I/O - there were 4, but this would 
	 * somehow cause NTP to stop replying to ntpq requests; TODO
 	 */
	_beginthread(iocompletionthread, 0, NULL);
}
	

extern void
uninit_io_completion_port(
	void
	)
{
	if (hIoCompletionPort != NULL) {
		/*  Get each of the service threads to exit
		*/
		signal_io_completion_port_exit();
	}
}




static int 
OnIoReadComplete(DWORD i, struct IoCompletionInfo *Info, DWORD Bytes)
{
	struct recvbuf *buff = NULL;
	struct refclockio * rio = (struct refclockio *) i;

	/*  Convert the overlapped pointer back to a recvbuf pointer.
	*/
	buff = (struct recvbuf *) ( ((char *) Info) - offsetof(struct recvbuf, iocompletioninfo));

	if (Bytes > 0) { /* ignore 0 bytes read due to timeout's */
		get_systime(&buff->recv_time);
		buff->recv_length = (int) Bytes;
		buff->receiver = rio->clock_recv;
		buff->dstadr = NULL;
		buff->recv_srcclock = rio->srcclock;
		add_full_recv_buffer(buff);
		buff = NULL;
	}

	/*  If we used the last buffer, then get another
	 */
	if (buff == NULL) {
	  buff = get_free_recv_buffer();
	}

	if (buff != NULL) {
		buff->fd = rio->fd;
		buff->iocompletioninfo.iofunction = OnIoReadComplete;
		if (!ReadFile((HANDLE) buff->fd, &buff->recv_buffer, sizeof(buff->recv_buffer), NULL, &buff->iocompletioninfo.overlapped)) {
			DWORD Result = GetLastError();
			switch (Result) {
			}
		}
	}
	return 1;
}




/*  Add a reference clock data structures I/O handles to
 *  the I/O completion port
 */  
int
io_completion_port_add_clock_io(
	struct refclockio *rio
	)
{
	struct recvbuf *buff = NULL;
	if (NULL == CreateIoCompletionPort((HANDLE) rio->fd, hIoCompletionPort, (DWORD) rio, 0)) {
		msyslog(LOG_ERR, "Can't add COM port to i/o completion port: %m");
		return 1;
	}

	if (NULL != (buff = get_free_recv_buffer())) {
		buff->fd = rio->fd;
		buff->iocompletioninfo.iofunction = OnIoReadComplete;
		if (!ReadFile((HANDLE) buff->fd, &buff->recv_buffer, sizeof(buff->recv_buffer), NULL, &buff->iocompletioninfo.overlapped)) {
			DWORD Result = GetLastError();
			switch (Result) {
			}
		}
	}
	else return 1;
	return 0;
}







static int 
OnSocketRecv(DWORD i, struct IoCompletionInfo *Info, DWORD Bytes)
{
	struct recvbuf *buff = NULL;
	struct interface * inter = (struct interface *) i;

	/*  Convert the overlapped pointer back to a recvbuf pointer.
	*/
	buff = (struct recvbuf *) ( ((char *) Info) - offsetof(struct recvbuf, iocompletioninfo));

	buff->recv_length = (int) Bytes;
	get_systime(&buff->recv_time);
	buff->receiver = receive; 
	buff->dstadr = inter;
	if (debug > 3) {
		char  strbuffer[256];
		DWORD strlength = sizeof(strbuffer);
		if (0 == WSAAddressToString((struct sockaddr *) &buff->recv_srcadr, buff->AddressLength, NULL, strbuffer, &strlength)) {
#ifdef DEBUG
			if (debug)
  			printf("Received %d bytes from %s\n", Bytes, strbuffer);
#endif
		}

	}

	add_full_recv_buffer(buff);
	buff = NULL;

	if (NULL != (buff = get_free_recv_buffer())) {
		DWORD BytesReceived = 0;
		DWORD Flags = 0;
		buff->iocompletioninfo.iofunction = OnSocketRecv;
		buff->fd = inter->fd;
		buff->AddressLength = sizeof(struct sockaddr_in);
		if (SOCKET_ERROR == WSARecvFrom(buff->fd, &buff->wsabuff, 1, 
						&BytesReceived, &Flags, 
						(struct sockaddr *) &buff->recv_srcadr, &buff->AddressLength, 
						&buff->iocompletioninfo.overlapped, NULL)) {
			DWORD Result = WSAGetLastError();
			switch (Result) {
				case NO_ERROR :
				case WSA_IO_INCOMPLETE :
				case WSA_WAIT_IO_COMPLETION :
				case WSA_IO_PENDING :

				break ;

				case WSAENOTSOCK :
					msyslog(LOG_ERR, "Can't read from socket, because it isn't a socket: %m");
				break;

				case WSAEFAULT :
					msyslog(LOG_ERR, "The buffers parameter is incorrect: %m");
				break;

				default :
				  /* nop */ ;

			}
		}
	}
	else return 0;
	return 1;
}





/*  Add a socket handle to the I/O completion port, and send an I/O
 *  read request to the kernel.
 *
 *  Note: As per the winsock documentation, we use WSARecvFrom. Using
 *        ReadFile() is less efficient.
 */
extern void
io_completion_port_add_socket(struct interface *inter)
{
	struct recvbuf *buff = NULL;

	if (NULL == CreateIoCompletionPort((HANDLE) inter->fd, hIoCompletionPort, (DWORD) inter, 0)) {
		msyslog(LOG_ERR, "Can't add socket to i/o completion port: %m");
		exit(1);
	}

	if (NULL != (buff = get_free_recv_buffer())) {
		DWORD BytesReceived = 0;
		DWORD Flags = 0;
		buff->iocompletioninfo.iofunction = OnSocketRecv;
		buff->fd = inter->fd;
		buff->AddressLength = sizeof(struct sockaddr_in);
		buff->receiver = receive; 

		if (SOCKET_ERROR == WSARecvFrom(buff->fd, &buff->wsabuff, 1, 
						&BytesReceived, &Flags, 
						(struct sockaddr *) &buff->recv_srcadr, &buff->AddressLength, 
						&buff->iocompletioninfo.overlapped, NULL)) {
			DWORD Result = WSAGetLastError();
			switch (Result) {
				case NO_ERROR :
				case WSA_IO_INCOMPLETE :
				case WSA_WAIT_IO_COMPLETION :
				case WSA_IO_PENDING :

				break ;

				case WSAENOTSOCK :
					msyslog(LOG_ERR, "Can't read from socket, because it isn't a socket: %m");
				break;

				case WSAEFAULT :
					msyslog(LOG_ERR, "The buffers parameter is incorrect: %m");
				break;

				default :
				  /* nop */ ;

			}
		}
	}

}


static int 
OnSendToComplete(DWORD Key, struct IoCompletionInfo *Info, DWORD Bytes)
{
	transmitbuf *buff = NULL;
	(void) Bytes;
	(void) Key;

	buff = (struct transmitbuf *) ( ((char *) Info) - offsetof(struct transmitbuf, iocompletioninfo));

	free_transmit_buffer(buff);
	return 1;
}

DWORD	
io_completion_port_sendto(
	struct interface *inter,	
	struct pkt *pkt,	
	int len, 
	struct sockaddr_in* dest)
{
	transmitbuf *buff = NULL;
	DWORD Result = ERROR_SUCCESS;

	if (len <= sizeof(buff->pkt)) {
		buff = get_free_transmit_buffer();
		if (buff != NULL) {
			DWORD BytesSent = 0;
			DWORD Flags = 0;

			memcpy(&buff->pkt, pkt, len);
			buff->wsabuf.buf = buff->pkt;
			buff->wsabuf.len = len;


			buff->iocompletioninfo.iofunction = OnSendToComplete;
			Result = WSASendTo(inter->fd, &buff->wsabuf, 1, &BytesSent, Flags, (struct sockaddr *) dest, sizeof(struct sockaddr_in), &buff->iocompletioninfo.overlapped, NULL);
			if (debug > 2) {
				char  strbuffer[256];
				DWORD strlength = sizeof(strbuffer);
				if (0 == WSAAddressToString((LPSOCKADDR) dest, sizeof(*dest), NULL, strbuffer, &strlength)) {
#ifdef DEBUG
					if (debug)
  					printf("SendTo - %d bytes to %s : %d\n", len, strbuffer, Result);
#endif
				}
			}

		}
		else {
#ifdef DEBUG
			if (debug)
			printf("No more transmit buffers left - data discarded\n");
#endif
		}
	}
	else {
#ifdef DEBUG
		if (debug) printf("Packet too large\n");
#endif
		exit(1);
	}
	return Result;
}



#else
  static int NonEmptyCompilationUnit;
#endif

