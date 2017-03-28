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

void
signal_no_reset(
	int sig,
	void (*func)(int)
	)
{
	int n;
	struct sigaction vec;
	struct sigaction ovec;

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
		n = sigaction(sig, &vec, &ovec);
	while (-1 == n && EINTR == errno);
	if (-1 == n) {
		perror("sigaction");
		exit(1);
	}
}
