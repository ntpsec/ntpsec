/*
 * iosignal.c - input/output routines for ntpd.	The socket-opening code
 *		   was shamelessly stolen from ntpd.
 */

/*
 * [Bug 158]
 * Do the #includes differently, as under some versions of Linux
 * sys/param.h has a #undef CONFIG_PHONE line in it.
 *
 * As we have ~40 CONFIG_ variables, I don't feel like renaming them
 * every time somebody adds a new macro to some system header.
 */

#include <config.h>

#include <stdio.h>
#include <signal.h>
#include <fcntl.h>

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_stdlib.h"
#include "iosignal.h"

#if defined(ENABLE_SIGNALED_IO)
static void sigio_handler	(int);

/* consistency safegurad to catch BLOCK/UNBLOCK oversights */
static volatile int sigio_block_count = 0;

/* main inputhandler to be called on SIGIO */
static input_handler_t *input_handler_callback = NULL;

/*
 * If sigaction() is used for signal handling and a signal is
 * pending then the kernel blocks the signal before it calls
 * the signal handler.
 *
 * The variable below is used to take care that the SIGIO signal
 * is not unintentionally unblocked inside the sigio_handler()
 * if the handler executes a piece of code that is normally
 * bracketed by BLOCKIO()/UNBLOCKIO() calls.
 */
static volatile int sigio_handler_active = 0;	/* semaphore, not bool */

/*
 * SIGIO ROUTINES.
 */



/*
 * TTY initialization routines.
 */
bool
init_clock_sig(
	struct refclockio *rio
	)
{
	/*
	 * SuSv2 way to set up asynchronous tty  I/O - process is sent SIGIO when
	 * I/O is possible on the fd.  Confusingly, according to the srabdard,
	 * it would be SIGURG on a socket. But real implementations seem to
	 * use the same signal number for these.
	 */
	if (fcntl(rio->fd, F_SETOWN, getpid()) == -1)
	{
		msyslog(LOG_ERR, "fcntl(F_SETOWN) fails for clock I/O: %m");
		return true;
	}

	if (fcntl(rio->fd, F_SETFL, O_NONBLOCK|O_ASYNC) < 0)
	{
		msyslog(LOG_ERR,
			"fcntl(O_NONBLOCK|O_ASYNC) fails for clock I/O: %m");
		return true;
	}
	return false;
}



void
init_socket_sig(
	int fd
	)
{
	int flags;

	/*
	 * SuSv2 way to set up asynchronous socket I/O - process is
	 * sent SIGURG when I/O is possible on the fd.  Confusingly,
	 * it would be SIGIO on a tty.  But in the real world it
	 * appears that SIGIO == SIGURG.  Code later in this fimction
	 * assumes this.
	 */
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
	{
		msyslog(LOG_ERR, "fcntl(F_GETFL) fails: %m - EXITING");
		exit(1);
		/*NOTREACHED*/
	}
	if (fcntl(fd, F_SETFL, flags|O_ASYNC) < 0)
	{
		msyslog(LOG_ERR, "fcntl(...|O_ASYNC) fails: %m - EXITING");
		exit(1);
		/*NOTREACHED*/
	}


	if (fcntl(fd, F_SETOWN, getpid()) == -1)
	{
		msyslog(LOG_ERR, "fcntl(F_SETOWN) fails: %m - EXITING");
		exit(1);
		/*NOTREACHED*/
	}
}

static void
sigio_handler(
	int sig
	)
{
	int saved_errno = errno;
	l_fp ts;

	get_systime(&ts);

	sigio_handler_active++;
	if (sigio_handler_active != 1)  /* This should never happen! */
	    msyslog(LOG_ERR, "sigio_handler: sigio_handler_active != 1");

	INSIST(input_handler_callback != NULL);
	(*input_handler_callback)(&ts);

	sigio_handler_active--;
	if (sigio_handler_active != 0)  /* This should never happen! */
	    msyslog(LOG_ERR, "sigio_handler: sigio_handler_active != 0");

	errno = saved_errno;
}

/*
 * Signal support routines.
 */
void
set_signal(input_handler_t *input)
{
	INSIST(input != NULL);
	
	input_handler_callback = input;

	using_sigio = true;
	(void) signal_no_reset(SIGIO, sigio_handler);
}

void
block_io_and_alarm(void)
{
	sigset_t set;

	if (sigemptyset(&set))
	    msyslog(LOG_ERR, "block_io_and_alarm: sigemptyset() failed: %m");
	if (sigaddset(&set, SIGIO))
	    msyslog(LOG_ERR, "block_io_and_alarm: sigaddset(SIGIO) failed: %m");
	if (sigaddset(&set, SIGALRM))
	    msyslog(LOG_ERR, "block_io_and_alarm: sigaddset(SIGALRM) failed: %m");

	if (sigprocmask(SIG_BLOCK, &set, NULL))
	    msyslog(LOG_ERR, "block_io_and_alarm: sigprocmask() failed: %m");
}

void
block_sigio(void)
{
	/* not called from within signal handler */
	if ( sigio_handler_active == 0 )
	{
		sigset_t set;

		++sigio_block_count;
		if (sigio_block_count > 1)
		    msyslog(LOG_INFO, "block_sigio: sigio_block_count > 1");
		if (sigio_block_count < 1)
		    msyslog(LOG_INFO, "block_sigio: sigio_block_count < 1");

		if (sigemptyset(&set))
		    msyslog(LOG_ERR, "block_sigio: sigemptyset() failed: %m");
		if (sigaddset(&set, SIGIO))
		    msyslog(LOG_ERR, "block_sigio: sigaddset(SIGIO) failed: %m");
		if (sigprocmask(SIG_BLOCK, &set, NULL))
		    msyslog(LOG_ERR, "block_sigio: sigprocmask() failed: %m");
	}
}

void
unblock_io_and_alarm(void)
{
	sigset_t unset;

	if (sigemptyset(&unset))
	    msyslog(LOG_ERR, "unblock_io_and_alarm: sigemptyset() failed: %m");

	if (sigaddset(&unset, SIGIO))
	    msyslog(LOG_ERR, "unblock_io_and_alarm: sigaddset(SIGIO) failed: %m");
	if (sigaddset(&unset, SIGALRM))
	    msyslog(LOG_ERR, "unblock_io_and_alarm: sigaddset(SIGALRM) failed: %m");

	if (sigprocmask(SIG_UNBLOCK, &unset, NULL))
	    msyslog(LOG_ERR, "unblock_io_and_alarm: sigprocmask() failed: %m");
}

void
unblock_sigio(void)
{
	if ( sigio_handler_active == 0 )  /* not called from within signal handler */
	{
		sigset_t unset;

		--sigio_block_count;
		if (sigio_block_count > 0)
		    msyslog(LOG_INFO, "unblock_sigio: sigio_block_count > 0");
		if (sigio_block_count < 0)
		    msyslog(LOG_INFO, "unblock_sigio: sigio_block_count < 0");

		if (sigemptyset(&unset))
		    msyslog(LOG_ERR, "unblock_sigio: sigemptyset() failed: %m");

		if (sigaddset(&unset, SIGIO))
		    msyslog(LOG_ERR, "unblock_sigio: sigaddset(SIGIO) failed: %m");
		if (sigprocmask(SIG_UNBLOCK, &unset, NULL))
		    msyslog(LOG_ERR, "unblock_sigio: sigprocmask() failed: %m");
	}
}

void
wait_for_signal(void)
{
	sigset_t old;

	if (sigprocmask(SIG_UNBLOCK, NULL, &old))
	    msyslog(LOG_ERR, "wait_for_signal: sigprocmask() failed: %m");

	if (sigdelset(&old, SIGIO))
	    msyslog(LOG_ERR, "wait_for_signal: sigdelset(SIGIO) failed: %m");

	if (sigdelset(&old, SIGALRM))
	    msyslog(LOG_ERR, "wait_for_signal: sigdelset(SIGALRM) failed: %m");

	if (sigsuspend(&old) && (errno != EINTR))
	    msyslog(LOG_ERR, "wait_for_signal: sigsuspend() failed: %m");
}

#else
int  NotAnEmptyCompilationUnit;
#endif 
