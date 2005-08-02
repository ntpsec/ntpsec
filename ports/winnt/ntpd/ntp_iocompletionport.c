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


#define BUFCHECK_SECS	10
static void	TransmitCheckThread(void *NotUsed);
static BOOL	bExit;
static HANDLE	TimerHandle;
static HANDLE hHeapHandle = NULL;

static HANDLE hIoCompletionPort = NULL;

static HANDLE WaitableIoEventHandle = NULL;

#define MAXHANDLES 3
HANDLE WaitHandles[MAXHANDLES] = { NULL, NULL, NULL };

HANDLE
get_io_event()
{
	return( WaitableIoEventHandle );
}

/*  This function will add an entry to the I/O completion port
 *  that will signal the I/O thread to exit (gracefully)
 */
static void
signal_io_completion_port_exit()
{
	if (!PostQueuedCompletionStatus(hIoCompletionPort, 0, 0, 0)) {
		msyslog(LOG_ERR, "Can't request service thread to exit: %m");
		exit(1);
	}
}

static void
iocompletionthread(void *NotUsed)
{
	DWORD BytesTransferred = 0;
	DWORD Key = 0;
	IoCompletionInfo * lpo = NULL;

	/*	Set the thread priority high enough so I/O will
	 *	preempt normal recv packet processing, but not
	 * 	higher than the timer sync thread.
	 */
	if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL)) {
		msyslog(LOG_ERR, "Can't set thread priority: %m");
	}

	while (TRUE) {
		GetQueuedCompletionStatus(hIoCompletionPort, 
					  &BytesTransferred, 
					  &Key, 
					  & (LPOVERLAPPED) lpo, 
					  INFINITE);
		if (lpo == NULL)
		{
#ifdef DEBUG
			msyslog(LOG_INFO, "Overlapped IO Thread Exits: %m");	
#endif
			break; /* fail */
		}

		/*
		 * Invoke the appropriate function based on
		 * the value of the request_type
		 */
		switch(lpo->request_type)
		{
		case CLOCK_READ:
			OnIoReadComplete(Key, lpo, BytesTransferred);
			break;
		case SOCK_RECV:
			OnSocketRecv(Key, lpo, BytesTransferred);
			break;
		case SOCK_SEND:
		case CLOCK_WRITE:
			OnWriteComplete(Key, lpo, BytesTransferred);
			break;
		default:
#if DEBUG
			if (debug > 3) {
				printf("Unknown request type %d found in completion port\n",
					lpo->request_type);
			}
#endif
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

	/*
	 * Create a handle to the Heap
	 */
	hHeapHandle = HeapCreate(0, 20*sizeof(IoCompletionInfo), 0);

	/* Set the exit flag */
	bExit = FALSE;
	/*
	 * Initialize the timer watch section
	 */
//	_beginthread(TransmitCheckThread, 0, NULL);

	/* Create the event used to signal an IO event
	 */
	WaitableIoEventHandle = CreateEvent(NULL, FALSE, FALSE, "WaitableIoEventHandle");
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
	
	/*
	 * Initialize the Wait Handles
	 */
	WaitHandles[0] = CreateEvent(NULL, FALSE, FALSE, "WaitHandles0"); /* exit request */
	WaitHandles[1] = get_timer_handle();
	WaitHandles[2] = get_io_event();

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
	/*
	 * Tell the timer handle to exit
	 */
	bExit = TRUE;
	SetEvent(TimerHandle);

	if (hIoCompletionPort != NULL) {
		/*  Get each of the service threads to exit
		*/
		signal_io_completion_port_exit();
	}
}


static int QueueIORead( struct refclockio *rio ) {

	struct recvbuf *buff;
	IoCompletionInfo *lpo;

	lpo = (IoCompletionInfo *) HeapAlloc(hHeapHandle,
					     HEAP_ZERO_MEMORY,
					     sizeof(IoCompletionInfo));
	buff = get_free_recv_buffer();
	
	if (buff == NULL)
		return 0;
	
	lpo->request_type = CLOCK_READ;
	lpo->buff = buff;

	buff->fd = rio->fd;
	if (!ReadFile((HANDLE) buff->fd, &buff->recv_buffer, sizeof(buff->recv_buffer), NULL, (LPOVERLAPPED) lpo)) {
			DWORD Result = GetLastError();
			switch (Result) {				
					case NO_ERROR :
					case ERROR_HANDLE_EOF :
					case ERROR_IO_PENDING :
				break ;

				default:
					msyslog(LOG_ERR, "Can't read from Refclock: %m");
					freerecvbuf(buff);
					/* Clear the heap */
					if (lpo != NULL)
						HeapFree(hHeapHandle, 0, lpo);
					return 0;
			}
	}
	return 1;
}



/* Return 1 on Successful Read */
static int 
OnIoReadComplete(DWORD i, IoCompletionInfo *lpo, DWORD Bytes)
{
	struct recvbuf *buff = NULL;
	struct refclockio * rio = (struct refclockio *) i;

	/*  Convert the overlapped pointer back to a recvbuf pointer.
	*/
	buff = (struct recvbuf *) lpo->buff;
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

	/* Clear the heap */
	if (lpo != NULL)
		HeapFree(hHeapHandle, 0, lpo);

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
	IoCompletionInfo *lpo;
	int AddrLen;

	lpo = (IoCompletionInfo *) HeapAlloc(hHeapHandle,
					     HEAP_ZERO_MEMORY,
					     sizeof(IoCompletionInfo));
	buff = get_free_recv_buffer();

	lpo->request_type = SOCK_RECV;
	lpo->buff = buff;

	if (buff != NULL) {
		DWORD BytesReceived = 0;
		DWORD Flags = 0;
		buff->fd = s;
		AddrLen = sizeof(struct sockaddr_in);

		if (SOCKET_ERROR == WSARecvFrom(buff->fd, &buff->wsabuff, 1, 
						&BytesReceived, &Flags, 
						(struct sockaddr *) &buff->recv_srcadr, (LPINT) &AddrLen, 
						(LPOVERLAPPED) lpo, NULL)) {
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
					/* Clear the heap */
					if (lpo != NULL)
						HeapFree(hHeapHandle, 0, lpo);
					return 0;
					break;

				case WSAEFAULT :
					netsyslog(LOG_ERR, "The buffers parameter is incorrect: %m");
					/* Clear the heap */
					if (lpo != NULL)
						HeapFree(hHeapHandle, 0, lpo);
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
OnSocketRecv(DWORD i, IoCompletionInfo *lpo, DWORD Bytes)
{
	struct recvbuf *buff = NULL;
	struct interface * inter = (struct interface *) i;
	
	/*  Convert the overlapped pointer back to a recvbuf pointer.
	*/
	
	buff = (struct recvbuf *) lpo->buff;
	get_systime(&buff->recv_time);	
	
	if (Bytes > 0 && inter->ignore_packets == ISC_FALSE) {	
		buff->recv_length = (int) Bytes;
		buff->receiver = receive; 
		buff->dstadr = inter;
#ifdef DEBUG
		if (debug > 3)
  			printf("Received %d bytes from %s\n", stoa(&buff->recv_srcadr));
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
	/* Clear the heap */
	if (lpo != NULL)
		HeapFree(hHeapHandle, 0, lpo);

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
OnSendToComplete(DWORD Key, IoCompletionInfo *lpo, DWORD Bytes)
{
	transmitbuf *buff = NULL;
	(void) Bytes;
	(void) Key;

	buff = (struct transmitbuf *) lpo->buff;

	free_transmit_buffer(buff);
	/* Clear the heap */
	if (lpo != NULL)
		HeapFree(hHeapHandle, 0, lpo);
	return 1;
}


static int 
OnWriteComplete(DWORD Key, IoCompletionInfo *lpo, DWORD Bytes)
{
	transmitbuf *buff = NULL;
	(void) Bytes;
	(void) Key;

	buff = (struct transmitbuf *) lpo->buff;

	free_transmit_buffer(buff);
	/* Clear the heap */
	if (lpo != NULL)
		HeapFree(hHeapHandle, 0, lpo);
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
	int AddrLen;
	IoCompletionInfo *lpo;

	lpo = (IoCompletionInfo *) HeapAlloc(hHeapHandle,
					     HEAP_ZERO_MEMORY,
					     sizeof(IoCompletionInfo));

	if (len <= sizeof(buff->pkt)) {
		buff = get_free_transmit_buffer();
		if (buff != NULL) {
			DWORD BytesSent = 0;
			DWORD Flags = 0;

			memcpy(&buff->pkt, pkt, len);
			buff->wsabuf.buf = buff->pkt;
			buff->wsabuf.len = len;

			AddrLen = sizeof(struct sockaddr_in);
			lpo->request_type = SOCK_SEND;
			lpo->buff = buff;

			Result = WSASendTo(inter->fd, &buff->wsabuf, 1, &BytesSent, Flags, (struct sockaddr *) dest, AddrLen, (LPOVERLAPPED) lpo, NULL);

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
					free_transmit_buffer(buff);
					break;
				}
			}
#ifdef DEBUG
			if (debug > 3)
				printf("WSASendTo - %d bytes to %s : %d\n", len, stoa(dest), Result);
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
	DWORD errval;
	transmitbuf *buff = NULL;
	DWORD lpNumberOfBytesWritten;
	DWORD Result = ERROR_INSUFFICIENT_BUFFER;
	IoCompletionInfo *lpo;

	lpo = (IoCompletionInfo *) HeapAlloc(hHeapHandle,
					     HEAP_ZERO_MEMORY,
					     sizeof(IoCompletionInfo));

	if (len <= sizeof(buff->pkt)) {
		buff = get_free_transmit_buffer();
		if (buff != NULL) {

			lpo->request_type = CLOCK_WRITE;
			lpo->buff = buff;
			memcpy(&buff->pkt, pkt, len);

			Result = WriteFile(fd, buff->pkt, len, &lpNumberOfBytesWritten, (LPOVERLAPPED) lpo);

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
					netsyslog(LOG_ERR, "WriteFile - error sending message: %m");
					free_transmit_buffer(buff);
					break;
				}
			}
#ifdef DEBUG
			if (debug > 2) {
				printf("WriteFile - %d bytes %d\n", len, Result);
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

/*
 * GetReceivedBuffers
 * Note that this is in effect the main loop for processing requests
 * both send and receive. This should be reimplemented
 */
struct recvbuf *GetReceivedBuffers()
{
	DWORD Index = WaitForMultipleObjectsEx(MAXHANDLES, WaitHandles, FALSE, INFINITE, TRUE);
	switch (Index) {
	case WAIT_OBJECT_0 + 0 : /* exit request */
		exit(0);
		break;

	case WAIT_OBJECT_0 + 1 : /* timer */
		timer();
		break;

	case WAIT_OBJECT_0 + 2 : /* Io event */
# ifdef DEBUG
		if ( debug > 3 )
		{
			printf( "IoEvent occurred\n" );
		}
# endif
		break;

	case WAIT_IO_COMPLETION : /* loop */
	case WAIT_TIMEOUT :
		break;
	case WAIT_FAILED:
		msyslog(LOG_ERR, "ntpd: WaitForMultipleObjectsEx Failed: Error: %m");
		break;

		/* For now do nothing if not expected */
	default:
		break;		
				
	} /* switch */

	return (getrecvbufs());	/* get received buffers */
}
static void
TransmitCheckThread(void *NotUsed)
{

	int SleepSecs = BUFCHECK_SECS;
	while (TRUE) 
	{
		Sleep(SleepSecs*1000);

		/* If we are done we exit */
		if (bExit)
			return;
				
	} /* while */
}

#else
  static int NonEmptyCompilationUnit;
#endif

