# include "config.h"

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

#include "ntp_syslog.h"
#include "ntp_stdlib.h"

# ifdef SA_RESTART
#  define Z_SA_RESTART		SA_RESTART
# else
#  define Z_SA_RESTART		0
# endif

/* set an sa_handler */
void
signal_no_reset(
	int sig,
	void (*func)(int)
	)
{
	int n;
	struct sigaction vec;

	ZERO(vec);
	sigemptyset(&vec.sa_mask);
	vec.sa_handler = func;

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
		n = sigaction(sig, &vec, NULL);
	while (-1 == n && EINTR == errno);
	if (-1 == n) {
		perror("ERROR: signal_no_reset() sigaction");
		exit(1);
	}
}

/* set an sa_sigaction */
void
signal_no_reset1(
	int sig,
	void (*func)(int, siginfo_t *, void *)
	)
{
	int n;
	struct sigaction vec;

	ZERO(vec);
	sigemptyset(&vec.sa_mask);
	vec.sa_sigaction = func;
	vec.sa_flags = SA_SIGINFO;

	n = sigaction(sig, &vec, NULL);
	if (-1 == n) {
		perror("ERROR: signal_no_reset1() sigaction");
		exit(1);
	}
}
