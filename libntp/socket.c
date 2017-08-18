/*
 * socket.c - low-level socket operations
 */

#include "config.h"

#include <stdio.h>

#include "ntp.h"
#include "ntp_io.h"
#include "ntp_net.h"
#include "ntp_debug.h"

/*
 * on Unix systems the stdio library typically
 * makes use of file descriptors in the lower
 * integer range.  stdio usually will make use
 * of the file descriptors in the range of
 * [0..FOPEN_MAX]
 * in order to keep this range clean, for socket
 * file descriptors we attempt to move them above
 * FOPEN_MAX. This is not as easy as it sounds as
 * FOPEN_MAX changes from implementation to implementation
 * and may exceed the current file descriptor limits.
 * We are using following strategy:
 * - keep a current socket fd boundary initialized with
 *   max(0, min(sysconf(_SC_OPEN_MAX) - FD_CHUNK, FOPEN_MAX))
 * - attempt to move the descriptor to the boundary or
 *   above.
 *   - if that fails and boundary > 0 set boundary
 *     to min(0, socket_fd_boundary - FD_CHUNK)
 *     -> retry
 *     if failure and boundary == 0 return old fd
 *   - on success close old fd return new fd
 *
 * effects:
 *   - fds will be moved above the socket fd boundary
 *     if at all possible.
 *   - the socket boundary will be reduced until
 *     allocation is possible or 0 is reached - at this
 *     point the algrithm will be disabled
 */
SOCKET
move_fd(
	SOCKET fd
	)
{
#if defined(F_DUPFD)
#ifdef OVERRIDE_FD_CHUNK
#define FD_CHUNK	OVERRIDE_FD_CHUNK
#else
#define FD_CHUNK	10
#endif

/*
 * number of fds we would like to have for
 * stdio FILE* available.
 * we can pick a "low" number as our use of
 * FILE* is limited to log files and temporarily
 * to data and config files. Except for log files
 * we don't keep the other FILE* open beyond the
 * scope of the function that opened it.
 */
#ifdef OVERRIDE_FD_PREFERRED_SOCKBOUNDARY
#define FD_PREFERRED_SOCKBOUNDARY OVERRIDE_FD_PREFERRED_SOCKBOUNDARY
#else
#define FD_PREFERRED_SOCKBOUNDARY 48
#endif

	static SOCKET socket_boundary = -1;
	SOCKET newfd;

	REQUIRE((int)fd >= 0);

	/*
	 * check whether boundary has be set up
	 * already
	 */
	if (socket_boundary == -1) {
		socket_boundary = max(0, min(sysconf(_SC_OPEN_MAX) - FD_CHUNK,
					     min(FOPEN_MAX, FD_PREFERRED_SOCKBOUNDARY)));
		TPRINT(1, ("move_fd: estimated max descriptors: %d, "
			   "initial socket boundary: %d\n",
			   (int)sysconf(_SC_OPEN_MAX), socket_boundary));
	}

	/*
	 * Leave a space for stdio to work in. potentially moving the
	 * socket_boundary lower until allocation succeeds.
	 */
	do {
		if (fd >= 0 && fd < socket_boundary) {
			/* inside reserved range: attempt to move fd */
			newfd = fcntl(fd, F_DUPFD, socket_boundary);

			if (newfd != -1) {
				/* success: drop the old one - return the new one */
				close(fd);
				return newfd;
			}
		} else {
			/* outside reserved range: no work - return the original one */
			return fd;
		}
		socket_boundary = max(0, socket_boundary - FD_CHUNK);
		TPRINT(1, ("move_fd: selecting new socket boundary: %d\n",
			   socket_boundary));
	} while (socket_boundary > 0);
#else
	REQUIRE((int)fd >= 0);
#endif /* defined(F_DUPFD) */
	return fd;
}


/*
 * make_socket_nonblocking() - set up descriptor to be non blocking
 */
void
make_socket_nonblocking(
	SOCKET fd
	)
{
	/*
	 * set non-blocking,
	 */

	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		msyslog(LOG_ERR,
			"ERR: fcntl(O_NONBLOCK) fails on fd #%d: %m", fd);
		exit(1);
	}
}

