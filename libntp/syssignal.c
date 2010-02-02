#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

#include "ntp_syslog.h"
#include "ntp_stdlib.h"


#ifdef HAVE_SIGACTION

#ifdef SA_RESTART
# define Z_SA_RESTART	SA_RESTART
#else
# define Z_SA_RESTART	0
#endif

# ifdef SA_SIGINFO
#  define Z_SA_SIGINFO	SA_SIGINFO
# else
#  define Z_SA_SIGINFO	0
# endif

# define IGNORED_SA_FLAGS	(Z_SA_RESTART | Z_SA_SIGINFO)


void
signal_no_reset(
	int sig,
	void (*func)(int)
	)
{
	int n;
	struct sigaction vec;
	struct sigaction ovec;

	vec.sa_handler = func;
	sigemptyset(&vec.sa_mask);

	vec.sa_flags = 0;
	/* Added for PPS clocks on Solaris 7 which get EINTR errors */
# ifdef SIGPOLL
	if (SIGPOLL == sig)
		vec.sa_flags = Z_SA_RESTART;
# endif
# ifdef SIGIO
	if (SIGIO == sig)
		vec.sa_flags = Z_SA_RESTART;
# endif

	do
		n = sigaction(sig, &vec, &ovec);
	while (-1 == n && EINTR == errno);
	if (-1 == n) {
		perror("sigaction");
		exit(1);
	}
	if (ovec.sa_flags & ~IGNORED_SA_FLAGS)
		msyslog(LOG_DEBUG,
			"signal_no_reset: signal %d had flags %x",
			sig, ovec.sa_flags);
}

#elif  HAVE_SIGVEC

void
signal_no_reset(
	int sig,
	RETSIGTYPE (*func)(int)
	)
{
	struct sigvec sv;
	int n;

	memset(&sv, 0, sizeof(sv));
	sv.sv_handler = func;
	n = sigvec(sig, &sv, (struct sigvec *)NULL);
	if (-1 == n) {
		perror("sigvec");
		exit(1);
	}
}

#elif  HAVE_SIGSET

void
signal_no_reset(
	int sig,
	RETSIGTYPE (*func)(int)
	)
{
	int n;

	n = sigset(sig, func);
	if (-1 == n) {
		perror("sigset");
		exit(1);
	}
}

#else

/* Beware!	This implementation resets the signal to SIG_DFL */
void
signal_no_reset(
	int sig,
	RETSIGTYPE (*func)(int)
	)
{
#ifndef SIG_ERR
# define SIG_ERR	(-1)
#endif
	if (SIG_ERR == signal(sig, func)) {
		perror("signal");
		exit(1);
	}
}

#endif
