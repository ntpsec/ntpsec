# include "config.h"

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

#include "ntp_syslog.h"
#include "ntp_stdlib.h"

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

	n = sigaction(sig, &vec, NULL);
	if (-1 == n) {
		perror("ERROR: signal_no_reset() sigaction");
		exit(1);
	}
}

#ifndef __NetBSD
/* NetBSD 10.0, May 2024, barfs when it can't find siginfo_t ???
 * There is a similar test in include/ntp_stdlib.h
 * This is only used by ntp_sandbox inside an ifdef for HAVE_SECCOMP_H */

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
#endif

