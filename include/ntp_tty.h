/*
 * ntp_tty.h - header file for serial lines handling
 */
#ifndef GUARD_NTP_TTY_H
#define GUARD_NTP_TTY_H

/*
 * use only one tty model - no use in initialising
 * a tty in three ways
 * only use HAVE_TERMIOS as it is POSIX-1:2001
 */

#include <termios.h>

#if defined(HAVE_SYS_MODEM_H)
#include <sys/modem.h>
#endif

/*
 * Line discipline flags.
 */
#define LDISC_STD	0x000	/* standard */
#define LDISC_RAW	0x020	/* raw binary */
#define	LDISC_7O1	0x100	/* 7-bit odd parity for Z3801A */
#define	LDISC_REMOTE	0x080	/* remote mode */

#endif /* GUARD_NTP_TTY_H */
