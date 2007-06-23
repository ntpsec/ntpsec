#if !defined __ntp_iocompletionport_h
# define __ntp_iocompletionport_h

#include "ntp_fp.h"
#include "ntp.h"

# if defined(HAVE_IO_COMPLETION_PORT)

struct refclockio;


extern	void	init_io_completion_port (void);
extern	void	uninit_io_completion_port (void);

extern	int	io_completion_port_add_clock_io	(struct refclockio * /*rio */);

extern	int	io_completion_port_add_socket (SOCKET fd, struct interface *);

extern	DWORD	io_completion_port_sendto (struct interface *, struct pkt *, int, struct sockaddr_storage*);

extern	HANDLE get_io_event (void);

extern HANDLE get_exit_event(void);		/* Handle of the exit event */

int GetReceivedBuffers(void);

# endif

#endif
