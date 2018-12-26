/*
 * ntp_sandbox.c - privilege containment for the NTP daemon
 *
 * SPDX-License-Identifier: BSD-2-clause
 */

#include "config.h"

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include "ntp_assert.h"

#ifdef ENABLE_DROPROOT
# include <ctype.h>
# include <grp.h>
# include <pwd.h>
# ifdef HAVE_LINUX_CAPABILITY
#  include <sys/capability.h>
#  include <sys/prctl.h>
# endif /* HAVE_LINUX_CAPABILITY */
# if defined(HAVE_PRIV_H) && defined(HAVE_SOLARIS_PRIVS)
#  include <priv.h>
# endif /* HAVE_PRIV_H */
#endif /* ENABLE_DROPROOT */

#ifdef HAVE_SOLARIS_PRIVS
# define LOWPRIVS "basic,sys_time,net_privaddr,proc_setid,!proc_info,!proc_session,!proc_exec"
static priv_set_t *lowprivs = NULL;
static priv_set_t *highprivs = NULL;
#endif /* HAVE_SOLARIS_PRIVS */

#ifdef HAVE_SECCOMP_H
# include <seccomp.h>
static void catchTrap(int sig, siginfo_t *, void *);
#endif

#ifdef ENABLE_DROPROOT
static uid_t sw_uid;
static gid_t sw_gid;
static char *endp;
static struct group *gr;
static struct passwd *pw;
#endif /* ENABLE_DROPROOT */

#include "ntp_syslog.h"
#include "ntp_stdlib.h"


bool sandbox(const bool droproot,
	     char *user, const char *group,
	     const char *chrootdir,
	     bool want_dynamic_interface_tracking)
{
	bool nonroot = false;
#ifndef ENABLE_DROPROOT
	UNUSED_ARG(droproot);
	UNUSED_ARG(user);
	UNUSED_ARG(group);
	UNUSED_ARG(chrootdir);
	UNUSED_ARG(want_dynamic_interface_tracking);
#else
#ifndef HAVE_LINUX_CAPABILITY
	UNUSED_ARG(want_dynamic_interface_tracking);
#endif /* HAVE_LINUX_CAPABILITY */
#if !defined(HAVE_LINUX_CAPABILITY) && !defined(HAVE_SOLARIS_PRIVS) && !defined(HAVE_SYS_CLOCKCTL_H)
	if (droproot) {
		msyslog(LOG_ERR,
			"INIT: root can't be dropped due to missing capabilities.");
		exit(-1);
	}
#endif /* !defined(HAVE_LINUX_CAPABILITY) && !defined(HAVE_SOLARIS_PRIVS)  && !defined(HAVE_SYS_CLOCKCTL_H) */
	if (droproot) {
		/* Drop super-user privileges and chroot now if the OS supports this */
#  ifdef HAVE_LINUX_CAPABILITY
		/* set flag: keep privileges across setuid() call. */
		if (prctl( PR_SET_KEEPCAPS, 1L, 0L, 0L, 0L ) == -1) {
			msyslog( LOG_ERR, "INIT: prctl( PR_SET_KEEPCAPS, 1L ) failed: %m" );
			exit(-1);
		}
#  elif defined(HAVE_SOLARIS_PRIVS)
		/* Nothing to do here */
#  else
		/* we need a user to switch to */
		if (user == NULL) {
			msyslog(LOG_ERR, "INIT: Need user name to drop root privileges (see -u flag!)" );
			exit(-1);
		}
#  endif	/* HAVE_LINUX_CAPABILITY || HAVE_SOLARIS_PRIVS */

		if (user != NULL) {
			if (isdigit((unsigned char)*user)) {
				sw_uid = (uid_t)strtoul(user, &endp, 0);
				if (*endp != '\0')
					goto getuser;

				if ((pw = getpwuid(sw_uid)) != NULL) {
					free((void *)user);
					user = estrdup(pw->pw_name);
					sw_gid = pw->pw_gid;
				} else {
					errno = 0;
					msyslog(LOG_ERR, "INIT: Cannot find user ID %s", user);
					exit (-1);
				}

			} else {
getuser:
				errno = 0;
				if ((pw = getpwnam(user)) != NULL) {
					sw_uid = pw->pw_uid;
					sw_gid = pw->pw_gid;
				} else {
					if (errno)
						msyslog(LOG_ERR, "INIT: getpwnam(%s) failed: %m", user);
					else
						msyslog(LOG_ERR, "INIT: Cannot find user `%s'", user);
					exit (-1);
				}
			}
		}
		if (group != NULL) {
			if (isdigit((unsigned char)*group)) {
				sw_gid = (gid_t)strtoul(group, &endp, 0);
				if (*endp != '\0')
					goto getgroup;
			} else {
getgroup:
				if ((gr = getgrnam(group)) != NULL) {
					sw_gid = gr->gr_gid;
				} else {
					errno = 0;
					msyslog(LOG_ERR, "INIT: Cannot find group `%s'", group);
					exit (-1);
				}
			}
		}

		if (chrootdir ) {
			/* make sure cwd is inside the jail: */
			if (chdir(chrootdir)) {
				msyslog(LOG_ERR, "INIT: Cannot chdir() to `%s': %m", chrootdir);
				exit (-1);
			}
			if (chroot(chrootdir)) {
				msyslog(LOG_ERR, "INIT: Cannot chroot() to `%s': %m", chrootdir);
				exit (-1);
			}
			if (chdir("/")) {
				msyslog(LOG_ERR, "INIT: Cannot chdir() to root after chroot(): %m");
				exit (-1);
			}
		}
#  ifdef HAVE_SOLARIS_PRIVS
		if ((lowprivs = priv_str_to_set(LOWPRIVS, ",", NULL)) == NULL) {
			msyslog(LOG_ERR, "INIT: priv_str_to_set() failed:%m");
			exit(-1);
		}
		if ((highprivs = priv_allocset()) == NULL) {
			msyslog(LOG_ERR, "INIT: priv_allocset() failed:%m");
			exit(-1);
		}
		(void) getppriv(PRIV_PERMITTED, highprivs);
		(void) priv_intersect(highprivs, lowprivs);
		if (setppriv(PRIV_SET, PRIV_PERMITTED, lowprivs) == -1) {
			msyslog(LOG_ERR, "INIT: setppriv() failed:%m");
			exit(-1);
		}
#  endif /* HAVE_SOLARIS_PRIVS */
                /* FIXME? Apple takes an int as 2nd argument */
		if (user && initgroups(user, (gid_t)sw_gid)) {
			msyslog(LOG_ERR, "INIT: Cannot initgroups() to user `%s': %m", user);
			exit (-1);
		}
		if (group && setgid(sw_gid)) {
			msyslog(LOG_ERR, "INIT: Cannot setgid() to group `%s': %m", group);
			exit (-1);
		}
		if (group && setegid(sw_gid)) {
			msyslog(LOG_ERR, "INIT: Cannot setegid() to group `%s': %m", group);
			exit (-1);
		}
		if (group) {
			if (0 != setgroups(1, &sw_gid)) {
				msyslog(LOG_ERR, "INIT: setgroups(1, %u) failed: %m",
                                        sw_gid);
				exit (-1);
			}
		}
		else if (pw)
			if (0 != initgroups(pw->pw_name, (gid_t)pw->pw_gid)) {
				msyslog(LOG_ERR,
                                        "INIT: initgroups(<%s>, %u) filed: %m",
                                        pw->pw_name, pw->pw_gid);
				exit (-1);
			}
		if (user && setuid(sw_uid)) {
			msyslog(LOG_ERR, "INIT: Cannot setuid() to user `%s': %m", user);
			exit (-1);
		}
		if (user && seteuid(sw_uid)) {
			msyslog(LOG_ERR, "INIT: Cannot seteuid() to user `%s': %m", user);
			exit (-1);
		}

#  if !defined(HAVE_LINUX_CAPABILITY) && !defined(HAVE_SOLARIS_PRIVS)
		/*
		 * for now assume that the privilege to bind to privileged ports
		 * is associated with running with uid 0 - should be refined on
		 * ports that allow binding to NTP_PORT with uid != 0
		 */
		nonroot = nonroot || (sw_uid != 0);  /* also notifies routing message listener */
#  endif /* !HAVE_LINUX_CAPABILITY && !HAVE_SOLARIS_PRIVS */

#  ifdef HAVE_LINUX_CAPABILITY
		{
			/*
			 *  We may be running under non-root uid now,
			 *  but we still hold full root privileges!
			 *  We drop all of them, except for the
			 *  crucial few: cap_sys_nice, cap_sys_time and
			 *  cap_net_bind_service for doing dynamic
			 *  interface tracking.
			 */
			cap_t caps;
			const char *captext;

			captext = want_dynamic_interface_tracking
				      ? "cap_sys_nice,cap_sys_time,cap_net_bind_service=pe"
				      : "cap_sys_nice,cap_sys_time=pe";
			caps = cap_from_text(captext);
			if (!caps) {
				msyslog(LOG_ERR,
					"INIT: cap_from_text(%s) failed: %m",
					captext);
				exit(-1);
			}
			if (-1 == cap_set_proc(caps)) {
				msyslog(LOG_ERR,
					"INIT: cap_set_proc() failed to drop root privs: %m");
				exit(-1);
			}
			cap_free(caps);
		}
#  endif	/* HAVE_LINUX_CAPABILITY */
#  ifdef HAVE_SOLARIS_PRIVS
		if (priv_delset(lowprivs, "proc_setid") == -1) {
			msyslog(LOG_ERR, "INIT: priv_delset() failed:%m");
			exit(-1);
		}
		if (setppriv(PRIV_SET, PRIV_PERMITTED, lowprivs) == -1) {
			msyslog(LOG_ERR, "INIT: setppriv() failed:%m");
			exit(-1);
		}
		priv_freeset(lowprivs);
		priv_freeset(highprivs);
#  endif /* HAVE_SOLARIS_PRIVS */
	}	/* if (droproot) */
# endif	/* ENABLE_DROPROOT */

/* libseccomp sandboxing */
#if defined(HAVE_SECCOMP_H)

#ifdef ENABLE_KILL_ON_TRAP
  #define MY_SCMP_ACT SCMP_ACT_KILL
#else
  #define MY_SCMP_ACT SCMP_ACT_TRAP
#endif
	scmp_filter_ctx ctx = seccomp_init(MY_SCMP_ACT);

        signal_no_reset1(SIGSYS, catchTrap);

	if (NULL == ctx) {
		msyslog(LOG_ERR, "INIT: sandbox: seccomp_init() failed: %m");
		exit (1);
		}

int scmp_sc[] = {

#ifdef ENABLE_EARLY_DROPROOT
/* Initialization uses a few syscalls that are not otherwise used.
 * Collect them here.
 * There are probably a few below that were added before we
 * understood the need for this section.
 * We could make a second pass after initialization to remove
 * these from the list.
 */

#ifndef ENABLE_DNS_LOOKUP
	/* libcrypto uses pthread_once() */
	/* We could avoid this by calling ssl_init() first. */
	SCMP_SYS(futex),	/* sem_xxx, used by threads */
#endif

	SCMP_SYS(getdents),	/* Scanning /etc/ntp.d/ */
	SCMP_SYS(getdents64),
#ifdef __NR_prlimit64
	SCMP_SYS(prlimit64),	/* 64 bit Fedora 26 with early_droproot*/
#endif
#endif  /* ENABLE_EARLY_DROPROOT */

        SCMP_SYS(access),
	SCMP_SYS(adjtimex),
	SCMP_SYS(bind),
	SCMP_SYS(brk),
	SCMP_SYS(chdir),
	SCMP_SYS(clock_gettime),
	SCMP_SYS(clock_settime),
	SCMP_SYS(close),
	SCMP_SYS(connect),
	SCMP_SYS(exit),
	SCMP_SYS(exit_group),
	SCMP_SYS(fcntl),
	SCMP_SYS(fstat),
	SCMP_SYS(fsync),


#ifdef __NR_getrandom
	SCMP_SYS(getrandom),	/* Added in 3.17 kernel */
#endif
#ifdef __NR_ugetrlimit
	SCMP_SYS(ugetrlimit),	/* sysconf */
#endif
#ifdef __NR_getrlimit
	SCMP_SYS(getrlimit),	/* sysconf */
	SCMP_SYS(setrlimit),
#endif
	SCMP_SYS(getrusage),
	SCMP_SYS(getsockname),
	SCMP_SYS(getsockopt),
	SCMP_SYS(gettimeofday),	/* mkstemp */
	SCMP_SYS(ioctl),
	SCMP_SYS(link),
	SCMP_SYS(lseek),
	SCMP_SYS(munmap),
	SCMP_SYS(open),
#ifdef __NR_openat
	SCMP_SYS(openat),	/* SUSE */
#endif
	SCMP_SYS(poll),
	SCMP_SYS(pselect6),
	SCMP_SYS(read),
	SCMP_SYS(recvfrom),    /* Comment this out for testing.
				* It will die on the first reply.
				* (Or maybe sooner if a request arrives.)
				*/
	SCMP_SYS(recvmsg),
	SCMP_SYS(rename),
	SCMP_SYS(rt_sigaction),
	SCMP_SYS(rt_sigprocmask),
	SCMP_SYS(rt_sigreturn),
	SCMP_SYS(sigaction),
	SCMP_SYS(sigprocmask),
	SCMP_SYS(sigreturn),
#ifdef __NR_select
	SCMP_SYS(select),	/* not in ARM */
#endif
	SCMP_SYS(sendto),
	SCMP_SYS(setsid),
#ifdef __NR_setsockopt
	SCMP_SYS(setsockopt),	/* not in old kernels */
#endif
	SCMP_SYS(socket),
	SCMP_SYS(socketcall),	/* old kernels */
	SCMP_SYS(stat),
	SCMP_SYS(statfs64),	/* from getaddrinfo after lid open */
#ifdef __NR_time
	SCMP_SYS(time),		/* not in ARM */
#endif
#ifdef HAVE_TIMER_CREATE
	SCMP_SYS(timer_create),
	SCMP_SYS(timer_gettime),
	SCMP_SYS(timer_settime),
#else
	SCMP_SYS(getitimer),
	SCMP_SYS(setitimer),
#endif
	SCMP_SYS(write),
        SCMP_SYS(unlink),

#ifdef ENABLE_DNS_LOOKUP
/* Don't comment out this block for testing.
 * pthread_create blocks signals so it will crash
 * rather than generate a trap.
 */
	SCMP_SYS(clone),	/* threads */
	SCMP_SYS(kill),		/* generate signal */
	SCMP_SYS(madvise),
	SCMP_SYS(mprotect),
	SCMP_SYS(set_robust_list),
	SCMP_SYS(sendmmsg),	/* DNS lookup */
	SCMP_SYS(socketpair),
	SCMP_SYS(statfs),
	SCMP_SYS(uname),
#endif
/* This shouldn't be needed if we don't use DNS, but
 * several libraries call pthread_once, just in case.
 */
	SCMP_SYS(futex),	/* sem_xxx, used by threads */


#ifdef REFCLOCK
	SCMP_SYS(nanosleep),
#endif
#ifdef CLOCK_SHM
        SCMP_SYS(shmget),
        SCMP_SYS(shmat),
        SCMP_SYS(shmdt),
#endif

	SCMP_SYS(fcntl64),
	SCMP_SYS(fstat64),

/* Arch Linux */
	SCMP_SYS(getpid),
	SCMP_SYS(gettid),
	SCMP_SYS(geteuid),
	SCMP_SYS(ppoll),
	SCMP_SYS(sendmsg),
#ifdef __NR_geteuid32
	SCMP_SYS(geteuid32),
#endif

#ifdef __NR_mmap
	/* gentoo 64-bit and 32-bit, Intel and Arm use mmap */
	SCMP_SYS(mmap),
#endif
#if defined(__i386__) || defined(__arm__)
	SCMP_SYS(_newselect),
	SCMP_SYS(_llseek),
	SCMP_SYS(mmap2),
	SCMP_SYS(send),
	SCMP_SYS(stat64),
#endif
};
	{
		unsigned int i;

		for (i = 0; i < COUNTOF(scmp_sc); i++) {
			if (seccomp_rule_add(ctx,
			    SCMP_ACT_ALLOW, scmp_sc[i], 0) < 0) {
				msyslog(LOG_ERR,
				    "INIT: sandbox: seccomp_rule_add() failed: %m");
			    exit(1);
			}
		}
	}

	if (seccomp_load(ctx) < 0) {
		msyslog(LOG_ERR, "INIT: sandbox: seccomp_load() failed: %m");
		exit(1);
	}
	else {
		msyslog(LOG_NOTICE, "INIT: sandbox: seccomp enabled.");
	}
        seccomp_release(ctx);

#endif /* HAVE_SECCOMP_H */

	return nonroot;
}

#ifdef HAVE_SECCOMP_H
/*
 * catchTrap - get here if something missing from list above
 * (or a bad guy finds a way in)
 *
 * The list above is a moving target.  Most syscalls will be
 * obvious but libc (and friends) can remap things and
 * getaddrinfo does all sorts of syscalls.
 *
 * To track down a missing call:
 *
 * Option one:
 *  The code below should print out the syscall number.
 *  grep _NR_ /usr/include/ -r | grep <number here> -w
 *  You will get several hits for various architures/modes.
 *  You can probably guess the right one.
 *
 * Option two:
 *  use strace
 *  sudo strace -t -f -o<filename> <path-to-ntpd> <args>
 *  When it crashes, the last syscall will be near the end of the log file
 *  just before the line with "--- SIGSYS", a page or two from the end
 *  depending on the stack trace.
 *
 * Option three:
 *  use gdb, break on catchTrap, get a trace.
 *  If you have the symbols, you can probably guess the syscall.
 *  You may have to get the sources for libc.
 *
 */
static void catchTrap(int sig, siginfo_t *si, void *u)
{
	UNUSED_ARG(sig);	/* signal number */
	UNUSED_ARG(u);	        /* unused ucontext_t */
	msyslog(LOG_ERR, "ERR: SIGSYS: got a trap.\n");
	if ( si->si_syscall ) {
	    msyslog(LOG_ERR, "ERR: SIGSYS/seccomp bad syscall %d/%#x\n",
		    si->si_syscall, si->si_arch);
        }
#ifndef BACKTRACE_DISABLED
        backtrace_log();
#endif

	exit(1);
}
#endif /* HAVE_SECCOMP_H */


/* end */
