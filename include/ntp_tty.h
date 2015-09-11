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

#ifdef TERMIOS_NEEDS__SVID3
# define _SVID3
#endif
#include <termios.h>
#ifdef TERMIOS_NEEDS__SVID3
# undef _SVID3
#endif
#define TTY	struct termios

#if defined(HAVE_SYS_MODEM_H)
#include <sys/modem.h>
#endif

/*
 * Line discipline flags.  The depredated ones required line discipline
 * or streams modules to be installed/loaded in the kernel and are now
 * ignored.  Leave the LDISC_CLK and other deprecated symbols defined
 * until 2013 or 2014 to avoid complicating the use of newer drivers on
 * older ntpd, which is often as easy as dropping in the refclock *.c.
 */
#define LDISC_STD	0x000	/* standard */
#define LDISC_CLK	0x001	/* depredated tty_clk \n */
#define LDISC_CLKPPS	0x002	/* depredated tty_clk \377 */
#define LDISC_ACTS	0x004	/* depredated tty_clk #* */
#define LDISC_CHU	0x008	/* depredated */
#define LDISC_PPS	0x010	/* depredated */
#define LDISC_RAW	0x020	/* raw binary */
#define LDISC_ECHO	0x040	/* enable echo */
#define	LDISC_REMOTE	0x080	/* remote mode */
#define	LDISC_7O1	0x100	/* 7-bit, odd parity for Z3801A */

/* function prototypes for ntp_tty.c */
#if !defined(SYS_VXWORKS) && !defined(SYS_WINNT)
extern	int	ntp_tty_setup(int, u_int, u_int);
extern	int	ntp_tty_ioctl(int, u_int);
#endif

#endif /* GUARD_NTP_TTY_H */
