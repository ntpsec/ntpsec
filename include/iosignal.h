#ifndef GUARD_IOSIGNAL_H
#define GUARD_IOSIGNAL_H

#include "ntp_refclock.h"

/* 
 * type of input handler function
 * only shared between iosignal.c and ntp_io.c
 */
typedef void (input_handler_t)(l_fp *);

#if defined(ENABLE_SIGNALED_IO)
#define	USING_SIGIO()	using_sigio

extern bool		using_sigio;

extern void		block_sigio	(void);
extern void		unblock_sigio	(void);
extern bool		init_clock_sig	(struct refclockio *);
extern void		init_socket_sig	(int);
extern void		set_signal	(input_handler_t *);
extern void		wait_for_signal (void);

# define BLOCKIO()	block_sigio()
# define UNBLOCKIO()	unblock_sigio()

#else	/* !ENABLE_SIGNALED_IO follows */
# define BLOCKIO()	do {} while (0)
# define UNBLOCKIO()	do {} while (0)
# define USING_SIGIO()	false
#endif

#endif	/* GUARD_IOSIGNAL_H */
