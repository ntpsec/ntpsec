#if !defined __ntp_iocompletionport_h
# define __ntp_iocompletionport_h

#include "ntp.h"

# if defined(HAVE_IO_COMPLETION_PORT)

struct IoCompletionInfo;
struct refclockio;

typedef int IoCompletionInfoFunction(DWORD Key, struct IoCompletionInfo *, DWORD Bytes);

typedef struct IoCompletionInfo {
	OVERLAPPED			overlapped;
	IoCompletionInfoFunction *	iofunction;
} IoCompletionInfo;

extern	void	init_io_completion_port P((void));
extern	void	uninit_io_completion_port P((void));

extern	int	io_completion_port_add_clock_io	P((struct refclockio * /*rio */));

extern	void	io_completion_port_add_socket P((struct interface *));

extern	DWORD	io_completion_port_sendto P((struct interface *, struct pkt *, int, struct sockaddr_in*));

# endif

#endif
