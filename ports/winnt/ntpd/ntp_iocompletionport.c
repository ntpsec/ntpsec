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

static HANDLE WaitableIoEventHandle = NULL;

HANDLE
get_io_event()
{
	return( WaitableIoEventHandle );
}


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
	if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL)) {
		msyslog(LOG_ERR, "Can't set thread priority: %m");
	}

	for (;;) {
		while (GetQueuedCompletionStatus(hIoCompletionPort, &BytesTransferred, &Key, & (LPOVERLAPPED) Overlapped, INFINITE)) {
			if (Overlapped != NULL && 
				Overlapped->iofunction != NULL) {
				if (!Overlapped->iofunction(Key, Overlapped, BytesTransferred)) {
#ifdef DEBUG
			msyslog(LOG_INFO, "Overlapped IO Thread Exits: %m");	
#endif
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
	/* Create the event used to signal an IO event
	 */
	WaitableIoEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (WaitableIoEventHandle == NULL) {
		msyslog(LOG_ERR, "Can't create I/O event handle: %m");
		exit(1);
	}

	/* Create the IO completion port
	 */
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


static int QueueIORead( struct refclockio *rio ) {

	struct recvbuf *buff;

	buff = get_free_recv_buffer();
	
	if (buff == NULL)
		return 0;
	
	buff->fd = rio->fd;
	buff->iocompletioninfo.iofunction = OnIoReadComplete;
	if (!ReadFile((HANDLE) buff->fd, &buff->recv_buffer, sizeof(buff->recv_buffer), NULL, &buff->iocompletioninfo.overlapped)) {
			DWORD Result = GetLastError();
			switch (Result) {				
					case NO_ERROR :
					case ERROR_HANDLE_EOF :
					case ERROR_IO_PENDING :
				break ;

				default:
					msyslog(LOG_ERR, "Can't read from Refclock: %m");
					freerecvbuf(buff);
					return 0;
			}
	}
	return 1;
}



/* Return 1 on Successful Read */
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
		if( !SetEvent( WaitableIoEventHandle ) ) {
#ifdef DEBUG
			if (debug > 3) {
				printf( "Error %d setting IoEventHandle\n", GetLastError() );
			}
#endif
		}
		buff = NULL;
	}
	else 
 		freerecvbuf(buff);

	QueueIORead( rio );
	return 1;
}

/*  Add a reference clock data structures I/O handles to
 *  the I/O completion port. Return 1 if any error.
 */  
int
io_completion_port_add_clock_io(
	struct refclockio *rio
	)
{
	if (NULL == CreateIoCompletionPort((HANDLE) rio->fd, hIoCompletionPort, (DWORD) rio, 0)) {
		msyslog(LOG_ERR, "Can't add COM port to i/o completion port: %m");
		return 1;
	}
	QueueIORead( rio );
	return 0;
}

/* Queue a receiver on a socket. Returns 0 if no buffer can be queued */

static unsigned long QueueSocketRecv(SOCKET s) {
	
	struct recvbuf *buff;
	buff = get_free_recv_buffer();

	if (buff != NULL) {
		DWORD BytesReceived = 0;
		DWORD Flags = 0;
		buff->iocompletioninfo.iofunction = OnSocketRecv;
		buff->fd = s;
		buff->AddressLength = sizeof(struct sockaddr_in);

		if (SOCKET_ERROR == WSARecvFrom(buff->fd, &buff->wsabuff, 1, 
						&BytesReceived, &Flags, 
						(struct sockaddr *) &buff->recv_srcadr, (LPINT) &buff->AddressLength, 
						&buff->iocompletioninfo.overlapped, NULL)) {
			DWORD Result = WSAGetLastError();
			switch (Result) {
				case NO_ERROR :
				case WSA_IO_INCOMPLETE :
				case WSA_WAIT_IO_COMPLETION :
				case WSA_IO_PENDING :
				break ;

				case WSAENOTSOCK :
					netsyslog(LOG_ERR, "Can't read from socket, because it isn't a socket: %m");
					freerecvbuf(buff);
					return 0;
					break;

				case WSAEFAULT :
					netsyslog(LOG_ERR, "The buffers parameter is incorrect: %m");
					freerecvbuf(buff);
					return 0;
				break;

				default :
				  /* nop */ ;
			}
		}
	}
	else 
		return 0;
	return 1;
}


/* Returns 0 if any Error */
static int 
OnSocketRecv(DWORD i, struct IoCompletionInfo *Info, DWORD Bytes)
{
	struct recvbuf *buff = NULL;
	struct interface * inter = (struct interface *) i;
	
	/*  Convert the overlapped pointer back to a recvbuf pointer.
	*/
	
	buff = (struct recvbuf *) ( ((char *) Info) - offsetof(struct recvbuf, iocompletioninfo));
	get_systime(&buff->recv_time);	
	
	if (Bytes > 0) {	
		buff->recv_length = (int) Bytes;
		buff->receiver = receive; 
		buff->dstadr = inter;
#ifdef DEBUG
		if (debug > 3) {
		char  strbuffer[256];
		DWORD strlength = sizeof(strbuffer);
		if (0 == WSAAddressToString((struct sockaddr *) &buff->recv_srcadr, buff->AddressLength, NULL, strbuffer, &strlength)) 
  			printf("Received %d bytes from %s\n", Bytes, strbuffer);
	}
#endif
		add_full_recv_buffer(buff);
		if( !SetEvent( WaitableIoEventHandle ) ) {
#ifdef DEBUG
			if (debug > 3) {
				printf( "Error %d setting IoEventHandle\n", GetLastError() );
			}
#endif
		}
	}
	else {
		freerecvbuf(buff);
	}
	QueueSocketRecv(inter->fd);
	return 1;
}


/*  Add a socket handle to the I/O completion port, and send an I/O
 *  read request to the kernel.
 *
 *  Note: As per the winsock documentation, we use WSARecvFrom. Using
 *        ReadFile() is less efficient.
 */
extern void
io_completion_port_add_socket(SOCKET fd, struct interface *inter)
{
	if (fd != INVALID_SOCKET) {
		if (NULL == CreateIoCompletionPort((HANDLE) fd, hIoCompletionPort,
						   (DWORD) inter, 0)) {
			msyslog(LOG_ERR, "Can't add socket to i/o completion port: %m");
		}
		else QueueSocketRecv(fd);
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


static int 
OnWriteComplete(DWORD Key, struct IoCompletionInfo *Info, DWORD Bytes)
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
	struct sockaddr_storage* dest)
{
	transmitbuf *buff = NULL;
	DWORD Result = ERROR_SUCCESS;
	int errval;

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

			if(Result == SOCKET_ERROR)
			{
				errval = WSAGetLastError();
				switch (errval) {

				case NO_ERROR :
				case WSA_IO_INCOMPLETE :
				case WSA_WAIT_IO_COMPLETION :
				case WSA_IO_PENDING :
					Result = ERROR_SUCCESS;
					break ;

				default :
					netsyslog(LOG_ERR, "WSASendTo - error sending message: %m");
					break;
				}
			}
#ifdef DEBUG
			if (debug > 2) {
				char  strbuffer[256];
				DWORD strlength = sizeof(strbuffer);
				if (0 == WSAAddressToString((LPSOCKADDR) dest, sizeof(*dest), NULL, strbuffer, &strlength)) 
  					printf("WSASendTo - %d bytes to %s : %d\n", len, strbuffer, Result);
			}
#endif
			return (Result);
		}
		else {
#ifdef DEBUG
			if (debug)
			printf("No more transmit buffers left - data discarded\n");
#endif
			return ERROR_OUTOFMEMORY;
		}
	}
	else {
#ifdef DEBUG
		if (debug) printf("Packet too large\n");
#endif
		return ERROR_INSUFFICIENT_BUFFER;
	}
}


/*
 * Async IO Write
 */
DWORD	
io_completion_port_write(
	HANDLE fd,	
	char *pkt,	
	int len)
{
	transmitbuf *buff = NULL;
	DWORD lpNumberOfBytesWritten;
	DWORD Result = ERROR_INSUFFICIENT_BUFFER;

	if (len <= sizeof(buff->pkt)) {
		buff = get_free_transmit_buffer();
		if (buff != NULL) {

			memcpy(&buff->pkt, pkt, len);
			buff->iocompletioninfo.iofunction = OnWriteComplete;

			Result = WriteFile(fd, buff->pkt, len, &lpNumberOfBytesWritten, &buff->iocompletioninfo.overlapped);

#ifdef DEBUG
			if (debug > 2) {
				printf("SendTo - %d bytes %d\n", len, Result);
			}
#endif
			if (Result) return len;
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
	}
	return Result;
}

#else
  static int NonEmptyCompilationUnit;
#endif

