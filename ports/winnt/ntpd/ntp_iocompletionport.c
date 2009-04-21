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
#include "ntp_request.h"
#include "clockstuff.h"
#include "ntp_io.h"
#include "clockstuff.h"

/*
 * Request types
 */
enum {
	SOCK_RECV,
	SOCK_SEND,
	SERIAL_WAIT,
	SERIAL_READ,
	SERIAL_WRITE
};


typedef struct IoCompletionInfo {
	OVERLAPPED			overlapped;
	int				request_type;
	union {
		recvbuf_t		*rbuf;
		transmitbuf_t		*tbuf;
	} buff_space;
} IoCompletionInfo;

#define	recv_buf	buff_space.rbuf
#define	trans_buf	buff_space.tbuf


#if !defined( _W64 )
  /*
   * if ULONG_PTR needs to be defined then the build environment
   * is pure 32 bit Windows. Since ULONG_PTR and DWORD have 
   * the same size in 32 bit Windows we can safely define
   * a replacement.
   */
  typedef DWORD ULONG_PTR;
#endif

/*
 * local function definitions
 */
static int QueueSerialWait(struct refclockio *, recvbuf_t *buff, IoCompletionInfo *lpo, BOOL clear_timestamp);

static int OnSocketRecv(ULONG_PTR, IoCompletionInfo *, DWORD, int);
static int OnSerialWaitComplete(ULONG_PTR, IoCompletionInfo *, DWORD, int);
static int OnSerialReadComplete(ULONG_PTR, IoCompletionInfo *, DWORD, int);
static int OnWriteComplete(ULONG_PTR, IoCompletionInfo *, DWORD, int);

/* #define USE_HEAP */

#ifdef USE_HEAP
static HANDLE hHeapHandle = NULL;
#endif

static HANDLE hIoCompletionPort = NULL;

static HANDLE WaitableIoEventHandle = NULL;
static HANDLE WaitableExitEventHandle = NULL;

#ifdef NTPNEEDNAMEDHANDLE
#define WAITABLEIOEVENTHANDLE "WaitableIoEventHandle"
#else
#define WAITABLEIOEVENTHANDLE NULL
#endif

#define MAXHANDLES 3
HANDLE WaitHandles[MAXHANDLES] = { NULL, NULL, NULL };

IoCompletionInfo *
GetHeapAlloc(char *fromfunc)
{
	IoCompletionInfo *lpo;

#ifdef USE_HEAP
	lpo = (IoCompletionInfo *) HeapAlloc(hHeapHandle,
			     HEAP_ZERO_MEMORY,
			     sizeof(IoCompletionInfo));
#else
	lpo = (IoCompletionInfo *) calloc(1, sizeof(*lpo));
#endif
	DPRINTF(3, ("Allocation %d memory for %s, ptr %x\n", sizeof(IoCompletionInfo), fromfunc, lpo));

	return (lpo);
}

void
FreeHeap(IoCompletionInfo *lpo, char *fromfunc)
{
	DPRINTF(3, ("Freeing memory for %s, ptr %x\n", fromfunc, lpo));

#ifdef USE_HEAP
	HeapFree(hHeapHandle, 0, lpo);
#else
	free(lpo);
#endif
}

transmitbuf_t *
get_trans_buf()
{
	transmitbuf_t *tb  = (transmitbuf_t *) emalloc(sizeof(transmitbuf_t));
	tb->wsabuf.len = 0;
	tb->wsabuf.buf = (char *) &tb->pkt;
	return (tb);
}

void
free_trans_buf(transmitbuf_t *tb)
{
	free(tb);
}

HANDLE
get_io_event()
{
	return( WaitableIoEventHandle );
}
HANDLE
get_exit_event()
{
	return( WaitableExitEventHandle );
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

static unsigned WINAPI
iocompletionthread(void *NotUsed)
{
	BOOL bSuccess = FALSE;
	int errstatus = 0;
	DWORD BytesTransferred = 0;
	ULONG_PTR Key = 0;
	IoCompletionInfo * lpo = NULL;
	u_long time_next_ifscan_after_error = 0;

	UNUSED_ARG(NotUsed);

	/*
	 *	socket and refclock receive call gettimeofday()
	 *	so the I/O thread needs to be on the same 
	 *	processor as the main and timing threads
	 *	to ensure consistent QueryPerformanceCounter()
	 *	results.
	 */
	lock_thread_to_processor(GetCurrentThread());

	/*	Set the thread priority high enough so I/O will
	 *	preempt normal recv packet processing, but not
	 * 	higher than the timer sync thread.
	 */
	if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL)) {
		msyslog(LOG_ERR, "Can't set thread priority: %m");
	}

	while (TRUE) {
		bSuccess = GetQueuedCompletionStatus(
					hIoCompletionPort, 
					&BytesTransferred, 
					&Key, 
					(LPOVERLAPPED *) &lpo, 
					INFINITE);
		if (lpo == NULL)
		{
			DPRINTF(2, ("Overlapped IO Thread Exiting\n"));
			break; /* fail */
		}
		
		/*
		 * Deal with errors
		 */
		if (bSuccess)
			errstatus = 0;
		else
		{
			errstatus = GetLastError();
			if (BytesTransferred == 0)
			{
				if (WSA_OPERATION_ABORTED == errstatus) {
					DPRINTF(4, ("Transfer Operation aborted\n"));
				} else if (ERROR_UNEXP_NET_ERR == errstatus) {
					/*
					 * We get this error when trying to send an the network
					 * interface is gone or has lost link.  Rescan interfaces
					 * to catch on sooner, but no more than once per minute.
					 * Once ntp is able to detect changes without polling
					 * this should be unneccessary
					 */
					if (time_next_ifscan_after_error < current_time) {
						time_next_ifscan_after_error = current_time + 60;
						timer_interfacetimeout(current_time);
					}
					DPRINTF(4, ("sendto unexpected network error, interface may be down\n"));
				}
			}
			else
			{
				msyslog(LOG_ERR, "sendto error after %d bytes: %m", BytesTransferred);
			}
		}

		/*
		 * Invoke the appropriate function based on
		 * the value of the request_type
		 */
		switch(lpo->request_type)
		{
		case SERIAL_WAIT:
			OnSerialWaitComplete(Key, lpo, BytesTransferred, errstatus);
			break;
		case SERIAL_READ:
			OnSerialReadComplete(Key, lpo, BytesTransferred, errstatus);
			break;
		case SOCK_RECV:
			OnSocketRecv(Key, lpo, BytesTransferred, errstatus);
			break;
		case SOCK_SEND:
		case SERIAL_WRITE:
			OnWriteComplete(Key, lpo, BytesTransferred, errstatus);
			break;
		default:
			DPRINTF(1, ("Unknown request type %d found in completion port\n",
				    lpo->request_type));
			break;
		}
	}

	return 0;
}

/*  Create/initialise the I/O creation port
 */
extern void
init_io_completion_port(
	void
	)
{
	unsigned tid;
	HANDLE thread;

#ifdef USE_HEAP
	/*
	 * Create a handle to the Heap
	 */
	hHeapHandle = HeapCreate(0, 20*sizeof(IoCompletionInfo), 0);
	if (hHeapHandle == NULL)
	{
		msyslog(LOG_ERR, "Can't initialize Heap: %m");
		exit(1);
	}
#endif

	/* Create the event used to signal an IO event
	 */
	WaitableIoEventHandle = CreateEvent(NULL, FALSE, FALSE, WAITABLEIOEVENTHANDLE);
	if (WaitableIoEventHandle == NULL) {
		msyslog(LOG_ERR,
		"Can't create I/O event handle: %m - another process may be running - EXITING");
		exit(1);
	}
	/* Create the event used to signal an exit event
	 */
	WaitableExitEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (WaitableExitEventHandle == NULL) {
		msyslog(LOG_ERR,
		"Can't create exit event handle: %m - EXITING");
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
	WaitHandles[0] = get_io_event();
	WaitHandles[1] = get_exit_event(); /* exit request */
	WaitHandles[2] = get_timer_handle();

	/* Have one thread servicing I/O - there were 4, but this would 
	 * somehow cause NTP to stop replying to ntpq requests; TODO
 	 */
	thread = (HANDLE)_beginthreadex(
		NULL, 
		0, 
		iocompletionthread, 
		NULL, 
		CREATE_SUSPENDED, 
		&tid);
	ResumeThread(thread);
	CloseHandle(thread);
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


static int QueueSerialWait(struct refclockio *rio, recvbuf_t *buff, IoCompletionInfo *lpo, BOOL clear_timestamp)
{
	lpo->request_type = SERIAL_WAIT;
	lpo->recv_buf = buff;

	if (clear_timestamp)
		memset(&buff->recv_time, 0, sizeof(buff->recv_time));

	buff->fd = _get_osfhandle(rio->fd);
	if (!WaitCommEvent((HANDLE) buff->fd, (DWORD *)&buff->recv_buffer, (LPOVERLAPPED) lpo)) {
		if (ERROR_IO_PENDING != GetLastError()) {
			msyslog(LOG_ERR, "Can't wait on Refclock: %m");
			freerecvbuf(buff);
			return 0;
		}
	}
	return 1;
}


static int 
OnSerialWaitComplete(ULONG_PTR i, IoCompletionInfo *lpo, DWORD Bytes, int errstatus)
{
	recvbuf_t *buff;
	struct refclockio * rio = (struct refclockio *) i;
	struct peer *pp;
	l_fp arrival_time;
	DWORD comm_mask;
	DWORD modem_status;
	static const l_fp zero_time = { 0 };
	DWORD dwBytesReturned;
	BOOL rc;

	get_systime(&arrival_time);

	/*
	 * Get the recvbuf pointer from the overlapped buffer.
	 */
	buff = lpo->recv_buf;
	comm_mask = (*(DWORD *)&buff->recv_buffer);
#ifdef DEBUG
		if (errstatus || comm_mask & ~(EV_RXFLAG | EV_RLSD)) {
			msyslog(LOG_ERR, "WaitCommEvent returned unexpected mask %x errstatus %d",
				comm_mask, errstatus);
			exit(-1);
		}
#endif
		if (comm_mask & EV_RLSD) { 
			modem_status = 0;
			GetCommModemStatus((HANDLE)buff->fd, &modem_status);
			if (modem_status & MS_RLSD_ON) {
				/*
				 * Use the timestamp from this PPS CD not
				 * the later end of line.
				 */
				buff->recv_time = arrival_time;
			}

			if (!(comm_mask & EV_RXFLAG)) {
				/*
				 * if we didn't see an end of line yet
				 * issue another wait for it.
				 */
				QueueSerialWait(rio, buff, lpo, FALSE);
				return 1;
			}
		}

		/*
		 * We've detected the end of line of serial input.
		 * Use this timestamp unless we already have a CD PPS
		 * timestamp in buff->recv_time.
		 */
		if (memcmp(&buff->recv_time, &zero_time, sizeof buff->recv_time)) {
			/*
			 * We will first see a user PPS timestamp here on either
			 * the first or second line of text.  Log a one-time
			 * message while processing the second line.
			 */
			if (1 == rio->recvcount) {
				pp = (struct peer *)rio->srcclock;
				msyslog(LOG_NOTICE, "Using user-mode PPS timestamp for %s",
					refnumtoa(&pp->srcadr));
			}
		} else {
			buff->recv_time = arrival_time;
		}

		/*
		 * Now that we have a complete line waiting, read it.
		 * There is still a race here, but we're likely to win.
		 */

		lpo->request_type = SERIAL_READ;

		rc = ReadFile(
			(HANDLE)buff->fd,
			buff->wsabuff.buf,
			buff->wsabuff.len,
			&buff->wsabuff.len,
			(LPOVERLAPPED) lpo);

		if (!rc && ERROR_IO_PENDING != GetLastError()) {
			msyslog(LOG_ERR, "Can't read from Refclock: %m");
			freerecvbuf(buff);
			return 0;
		}

	return 1;
}

/* Return 1 on Successful Read */
static int 
OnSerialReadComplete(ULONG_PTR i, IoCompletionInfo *lpo, DWORD Bytes, int errstatus)
{
	recvbuf_t *buff;
	struct refclockio * rio = (struct refclockio *) i;

	/*
	 * Get the recvbuf pointer from the overlapped buffer.
	 */
	buff = lpo->recv_buf;

	/*
	 * ignore 0 bytes read due to timeout's and closure on fd
	 */
	if (!errstatus && Bytes) {
		buff->recv_length = (int) Bytes;
		buff->receiver = rio->clock_recv;
		buff->dstadr = NULL;
		buff->recv_srcclock = rio->srcclock;
		packets_received++;
		/*
		 * Eat the first line of input as it's possibly
		 * partial and if a PPS is present, it may not 
		 * have fired since the port was opened.
		 */
		if (rio->recvcount++) {
			add_full_recv_buffer(buff);
			/*
			 * Now signal we have something to process
			 */
			SetEvent(WaitableIoEventHandle);
			buff = get_free_recv_buffer_alloc();
		}
	}

	QueueSerialWait(rio, buff, lpo, TRUE);

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
	IoCompletionInfo *lpo;
	recvbuf_t *buff;

	if (NULL == CreateIoCompletionPort((HANDLE)_get_osfhandle(rio->fd), hIoCompletionPort, (ULONG_PTR) rio, 0)) {
		msyslog(LOG_ERR, "Can't add COM port to i/o completion port: %m");
		return 1;
	}

	lpo = (IoCompletionInfo *) GetHeapAlloc("io_completion_port_add_clock_io");
	if (lpo == NULL)
	{
		msyslog(LOG_ERR, "Can't allocate heap for completion port: %m");
		return 1;
	}

	buff = get_free_recv_buffer_alloc();
	QueueSerialWait(rio, buff, lpo, TRUE);
	return 0;
}

/*
 * Queue a receiver on a socket. Returns 0 if no buffer can be queued 
 *
 *  Note: As per the winsock documentation, we use WSARecvFrom. Using
 *        ReadFile() is less efficient.
 */
static unsigned long QueueSocketRecv(SOCKET s, recvbuf_t *buff, IoCompletionInfo *lpo) {
	
	int AddrLen;

	lpo->request_type = SOCK_RECV;
	lpo->recv_buf = buff;

	if (buff != NULL) {
		DWORD BytesReceived = 0;
		DWORD Flags = 0;
		buff->fd = s;
		AddrLen = sizeof(struct sockaddr_in);
		buff->src_addr_len = sizeof(struct sockaddr);

		if (SOCKET_ERROR == WSARecvFrom(buff->fd, &buff->wsabuff, 1, 
						&BytesReceived, &Flags, 
						(struct sockaddr *) &buff->recv_srcadr, (LPINT) &buff->src_addr_len, 
						(LPOVERLAPPED) lpo, NULL)) {
			DWORD Result = WSAGetLastError();
			switch (Result) {
				case NO_ERROR :
				case WSA_IO_PENDING :
					break ;

				case WSAENOTSOCK :
					netsyslog(LOG_ERR, "Can't read from socket, because it isn't a socket: %m");
					/* return the buffer */
					freerecvbuf(buff);
					return 0;
					break;

				case WSAEFAULT :
					netsyslog(LOG_ERR, "The buffers parameter is incorrect: %m");
					/* return the buffer */
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
OnSocketRecv(ULONG_PTR i, IoCompletionInfo *lpo, DWORD Bytes, int errstatus)
{
	struct recvbuf *buff = NULL;
	recvbuf_t *newbuff;
	isc_boolean_t ignore_this;
	l_fp arrival_time;
	struct interface * inter = (struct interface *) i;
	
	get_systime(&arrival_time);

	/*  Convert the overlapped pointer back to a recvbuf pointer.
	*/
	
	/*
	 * Check returned structures
	 */
	if (lpo == NULL)
		return (1); /* Nothing to do */

	buff = lpo->recv_buf;
	/*
	 * Make sure we have a buffer
	 */
	if (buff == NULL) {
		return (1);
	}

	/*
	 * If the socket is closed we get an Operation Aborted error
	 * Just clean up
	 */
	if (errstatus == WSA_OPERATION_ABORTED)
	{
		freerecvbuf(buff);
		FreeHeap(lpo, "OnSocketRecv: Socket Closed");
		return (1);
	}


	/*
	 * Get a new recv buffer for the replacement socket receive
	 */
	newbuff = get_free_recv_buffer_alloc();
	QueueSocketRecv(inter->fd, newbuff, lpo);

#ifdef DEBUG
	if(debug > 3 && get_packet_mode(buff) == MODE_BROADCAST)
		printf("****Accepting Broadcast packet on fd %d from %s\n", buff->fd, stoa(&buff->recv_srcadr));
#endif
	ignore_this = inter->ignore_packets;
#ifdef DEBUG
	if (debug > 3)
		printf(" Packet mode is %d\n", get_packet_mode(buff));
#endif
	/*
	 * If we keep it add some info to the structure
	 */
	if (Bytes > 0 && ignore_this == ISC_FALSE) {
		memcpy(&buff->recv_time, &arrival_time, sizeof buff->recv_time);	
		buff->recv_length = (int) Bytes;
		buff->receiver = receive; 
		buff->dstadr = inter;
#ifdef DEBUG
		if (debug > 1)
			printf("Received %d bytes of fd %d in buffer %x from %s\n", Bytes, buff->fd, buff, stoa(&buff->recv_srcadr));
#endif
		packets_received++;
		inter->received++;
		add_full_recv_buffer(buff);
		/*
		 * Now signal we have something to process
		 */
		SetEvent(WaitableIoEventHandle);
	} else
		freerecvbuf(buff);

	return 1;
}


/*  Add a socket handle to the I/O completion port, and send 
 *  NTP_RECVS_PER_SOCKET recv requests to the kernel.
 */
extern int
io_completion_port_add_socket(SOCKET fd, struct interface *inter)
{
	IoCompletionInfo *lpo;
	recvbuf_t *buff;
	int n;

	if (fd != INVALID_SOCKET) {
		if (NULL == CreateIoCompletionPort((HANDLE) fd, hIoCompletionPort,
						   (DWORD) inter, 0)) {
			msyslog(LOG_ERR, "Can't add socket to i/o completion port: %m");
			return 1;
		}
	}

	/*
	 * Windows 2000 bluescreens with bugcheck 0x76
	 * PROCESS_HAS_LOCKED_PAGES at ntpd process
	 * termination when using more than one pending
	 * receive per socket.  A runtime version test
	 * would allow using more on newer versions
	 * of Windows.
	 */

#define WINDOWS_RECVS_PER_SOCKET 1

	for (n = 0; n < WINDOWS_RECVS_PER_SOCKET; n++) {

		buff = get_free_recv_buffer_alloc();
		lpo = (IoCompletionInfo *) GetHeapAlloc("io_completion_port_add_socket");
		if (lpo == NULL)
		{
			msyslog(LOG_ERR, "Can't allocate heap for completion port: %m");
			return 1;
		}

		QueueSocketRecv(fd, buff, lpo);

	}
	return 0;
}

static int 
OnWriteComplete(ULONG_PTR i, IoCompletionInfo *lpo, DWORD Bytes, int errstatus)
{
	transmitbuf_t *buff;
	struct interface *inter;

	UNUSED_ARG(Bytes);

	buff = lpo->trans_buf;

	free_trans_buf(buff);

	if (SOCK_SEND == lpo->request_type) {
		switch (errstatus) {
		case WSA_OPERATION_ABORTED:
		case NO_ERROR:
			break;

		default:
			inter = (struct interface *)i;
			packets_notsent++;
			inter->notsent++;
			break;
		}
	}

	if (errstatus == WSA_OPERATION_ABORTED)
		FreeHeap(lpo, "OnWriteComplete: Socket Closed");
	else
		FreeHeap(lpo, "OnWriteComplete");
	return 1;
}


/*
 * Return value is really GetLastError-style error code
 * which is a DWORD but using int, which is large enough,
 * decreases #ifdef forest in ntp_io.c harmlessly.
 */
int	
io_completion_port_sendto(
	struct interface *inter,	
	struct pkt *pkt,	
	int len, 
	struct sockaddr_storage* dest)
{
	transmitbuf_t *buff = NULL;
	DWORD Result = ERROR_SUCCESS;
	int errval;
	int AddrLen;
	IoCompletionInfo *lpo;
	DWORD BytesSent = 0;
	DWORD Flags = 0;

	lpo = (IoCompletionInfo *) GetHeapAlloc("io_completion_port_sendto");

	if (lpo == NULL)
		return ERROR_OUTOFMEMORY;

	if (len <= sizeof(buff->pkt)) {
		buff = get_trans_buf();

		if (buff == NULL) {
			msyslog(LOG_ERR, "No more transmit buffers left - data discarded");
			FreeHeap(lpo, "io_completion_port_sendto");
			return ERROR_OUTOFMEMORY;
		}



		memcpy(&buff->pkt, pkt, len);
		buff->wsabuf.buf = buff->pkt;
		buff->wsabuf.len = len;

		AddrLen = sizeof(struct sockaddr_in);
		lpo->request_type = SOCK_SEND;
		lpo->trans_buf = buff;

		Result = WSASendTo(inter->fd, &buff->wsabuf, 1, &BytesSent, Flags, (struct sockaddr *) dest, AddrLen, (LPOVERLAPPED) lpo, NULL);

		if(Result == SOCKET_ERROR)
		{
			errval = WSAGetLastError();
			switch (errval) {

			case NO_ERROR :
			case WSA_IO_PENDING :
				Result = ERROR_SUCCESS;
				break ;

			/*
			 * Something bad happened
			 */
			default :
				netsyslog(LOG_ERR, "WSASendTo - error sending message: %m");
				free_trans_buf(buff);
				FreeHeap(lpo, "io_completion_port_sendto");
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
		if (debug) printf("Packet too large: %d Bytes\n", len);
#endif
		return ERROR_INSUFFICIENT_BUFFER;
	}
}


/*
 * async_write, clone of write(), used by some reflock drivers
 */
int	
async_write(
	int fd,
	const void *data,
	unsigned int count)
{
	transmitbuf_t *buff;
	IoCompletionInfo *lpo;
	DWORD BytesWritten;

	if (count > sizeof buff->pkt) {
#ifdef DEBUG
		if (debug) {
			printf("async_write: %d bytes too large, limit is %d\n",
				count, sizeof buff->pkt);
			exit(-1);
		}
#endif
		errno = ENOMEM;
		return -1;
	}

	buff = get_trans_buf();
	lpo = (IoCompletionInfo *) GetHeapAlloc("async_write");

	if (! buff || ! lpo) {
		if (buff) {
			free_trans_buf(buff);
#ifdef DEBUG
			if (debug)
				printf("async_write: out of memory, \n");
#endif
		} else {
			msyslog(LOG_ERR, "No more transmit buffers left - data discarded");
		}

		errno = ENOMEM;
		return -1;
	}

	lpo->request_type = SERIAL_WRITE;
	lpo->trans_buf = buff;
	memcpy(&buff->pkt, data, count);

	if (! WriteFile((HANDLE)_get_osfhandle(fd), buff->pkt, count, &BytesWritten, (LPOVERLAPPED) lpo) &&
		ERROR_IO_PENDING != GetLastError()) {

		msyslog(LOG_ERR, "async_write - error %m");
		free_trans_buf(buff);
		FreeHeap(lpo, "async_write");
		errno = EBADF;
		return -1;
	}

	return count;
}


/*
 * GetReceivedBuffers
 * Note that this is in effect the main loop for processing requests
 * both send and receive. This should be reimplemented
 */
int GetReceivedBuffers()
{
	isc_boolean_t have_packet = ISC_FALSE;
	while (!have_packet) {
		DWORD Index = WaitForMultipleObjects(MAXHANDLES, WaitHandles, FALSE, INFINITE);
		switch (Index) {
		case WAIT_OBJECT_0 + 0 : /* Io event */
# ifdef DEBUG
			if ( debug > 3 )
			{
				printf( "IoEvent occurred\n" );
			}
# endif
			have_packet = ISC_TRUE;
			break;
		case WAIT_OBJECT_0 + 1 : /* exit request */
			exit(0);
			break;
		case WAIT_OBJECT_0 + 2 : /* timer */
			timer();
			break;
		case WAIT_IO_COMPLETION : /* loop */
		case WAIT_TIMEOUT :
			break;
		case WAIT_FAILED:
			msyslog(LOG_ERR, "ntpd: WaitForMultipleObjects Failed: Error: %m");
			break;

			/* For now do nothing if not expected */
		default:
			break;		
				
		} /* switch */
	}

	return (full_recvbuffs());	/* get received buffers */
}

#else
  static int NonEmptyCompilationUnit;
#endif

