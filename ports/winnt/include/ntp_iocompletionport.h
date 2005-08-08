#if !defined __ntp_iocompletionport_h
# define __ntp_iocompletionport_h

#include "ntp.h"

# if defined(HAVE_IO_COMPLETION_PORT)

struct IoCompletionInfo;
struct refclockio;

/*
 * Request types
 */
enum {
	SOCK_RECV,
	SOCK_SEND,
	CLOCK_READ,
	CLOCK_WRITE
};

typedef int IoCompletionInfoFunction(DWORD Key, struct IoCompletionInfo *, DWORD Bytes);

typedef struct IoCompletionInfo {
	OVERLAPPED			overlapped;
	int				request_type;
	LPVOID				buff;
	IoCompletionInfoFunction *	iofunction;
} IoCompletionInfo;

extern	void	init_io_completion_port (void);
extern	void	uninit_io_completion_port (void);

extern	int	io_completion_port_add_clock_io	(struct refclockio * /*rio */);

extern	void	io_completion_port_add_socket (SOCKET fd, struct interface *);

extern	DWORD	io_completion_port_sendto (struct interface *, struct pkt *, int, struct sockaddr_storage*);

extern	HANDLE get_io_event (void);

struct recvbuf *GetReceivedBuffers(void);

static int QueueIORead( struct refclockio * );

static int OnSocketRecv(DWORD, IoCompletionInfo *, DWORD);
static int OnIoReadComplete(DWORD, IoCompletionInfo *, DWORD);
static int OnWriteComplete(DWORD, IoCompletionInfo *, DWORD);
# endif

#endif
