#ifndef GUARD_WIN32_IO_H
#define GUARD_WIN32_IO_H

extern	void	InitSockets(void);
extern	void	connection_reset_fix(SOCKET fd, sockaddr_u *addr);

#endif /* GUARD_WIN32_IO_H */
