/*
 * ntp_sandbox.c - privilege containment for the NTP daemon
 *
 * SPDX-License-Identifier: BSD-2-clause
 */

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include "config.h"

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
#endif /* HAVE_SECCOMP_H */

#ifdef ENABLE_DROPROOT
static bool root_dropped;
static uid_t sw_uid;
static gid_t sw_gid;
static char *endp;
static struct group *gr;
static struct passwd *pw;
#endif /* ENABLE_DROPROOT */

#include "ntp_syslog.h"
#include "ntp_stdlib.h"

#ifdef ENABLE_SECCOMP
#ifdef HAVE_SECCOMP
static void catchTrap(int sig);
#endif
#endif

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
			"root can't be dropped due to missing capabilities.");
		exit(-1);
	}
#endif /* !defined(HAVE_LINUX_CAPABILITY) && !defined(HAVE_SOLARIS_PRIVS)  && !defined(HAVE_SYS_CLOCKCTL_H) */
	if (droproot) {
		/* Drop super-user privileges and chroot now if the OS supports this */
#  ifdef HAVE_LINUX_CAPABILITY
		/* set flag: keep privileges across setuid() call. */
		if (prctl( PR_SET_KEEPCAPS, 1L, 0L, 0L, 0L ) == -1) {
			msyslog( LOG_ERR, "prctl( PR_SET_KEEPCAPS, 1L ) failed: %m" );
			exit(-1);
		}
#  elif defined(HAVE_SOLARIS_PRIVS)
		/* Nothing to do here */
#  else
		/* we need a user to switch to */
		if (user == NULL) {
			msyslog(LOG_ERR, "Need user name to drop root privileges (see -u flag!)" );
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
					msyslog(LOG_ERR, "Cannot find user ID %s", user);
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
						msyslog(LOG_ERR, "getpwnam(%s) failed: %m", user);
					else
						msyslog(LOG_ERR, "Cannot find user `%s'", user);
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
					msyslog(LOG_ERR, "Cannot find group `%s'", group);
					exit (-1);
				}
			}
		}

		if (chrootdir ) {
			/* make sure cwd is inside the jail: */
			if (chdir(chrootdir)) {
				msyslog(LOG_ERR, "Cannot chdir() to `%s': %m", chrootdir);
				exit (-1);
			}
			if (chroot(chrootdir)) {
				msyslog(LOG_ERR, "Cannot chroot() to `%s': %m", chrootdir);
				exit (-1);
			}
			if (chdir("/")) {
				msyslog(LOG_ERR, "Cannot chdir() to root after chroot(): %m");
				exit (-1);
			}
		}
#  ifdef HAVE_SOLARIS_PRIVS
		if ((lowprivs = priv_str_to_set(LOWPRIVS, ",", NULL)) == NULL) {
			msyslog(LOG_ERR, "priv_str_to_set() failed:%m");
			exit(-1);
		}
		if ((highprivs = priv_allocset()) == NULL) {
			msyslog(LOG_ERR, "priv_allocset() failed:%m");
			exit(-1);
		}
		(void) getppriv(PRIV_PERMITTED, highprivs);
		(void) priv_intersect(highprivs, lowprivs);
		if (setppriv(PRIV_SET, PRIV_PERMITTED, lowprivs) == -1) {
			msyslog(LOG_ERR, "setppriv() failed:%m");
			exit(-1);
		}
#  endif /* HAVE_SOLARIS_PRIVS */
		if (user && initgroups(user, (gid_t)sw_gid)) {
			msyslog(LOG_ERR, "Cannot initgroups() to user `%s': %m", user);
			exit (-1);
		}
		if (group && setgid(sw_gid)) {
			msyslog(LOG_ERR, "Cannot setgid() to group `%s': %m", group);
			exit (-1);
		}
		if (group && setegid(sw_gid)) {
			msyslog(LOG_ERR, "Cannot setegid() to group `%s': %m", group);
			exit (-1);
		}
		if (group) {
			if (0 != setgroups(1, &sw_gid)) {
				msyslog(LOG_ERR, "setgroups(1, %u) failed: %m",
                                        sw_gid);
				exit (-1);
			}
		}
		else if (pw)
			if (0 != initgroups(pw->pw_name, (gid_t)pw->pw_gid)) {
				msyslog(LOG_ERR,
                                        "initgroups(<%s>, %u) filed: %m",
                                        pw->pw_name, pw->pw_gid);
				exit (-1);
			}
		if (user && setuid(sw_uid)) {
			msyslog(LOG_ERR, "Cannot setuid() to user `%s': %m", user);
			exit (-1);
		}
		if (user && seteuid(sw_uid)) {
			msyslog(LOG_ERR, "Cannot seteuid() to user `%s': %m", user);
			exit (-1);
		}

#  if !defined(HAVE_LINUX_CAPABILITY) && !defined(HAVE_SOLARIS_PRIVS)
		/*
		 * for now assume that the privilege to bind to privileged ports
		 * is associated with running with uid 0 - should be refined on
		 * ports that allow binding to NTP_PORT with uid != 0
		 */
		nonroot |= (sw_uid != 0);  /* also notifies routing message listener */
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
					"cap_from_text(%s) failed: %m",
					captext);
				exit(-1);
			}
			if (-1 == cap_set_proc(caps)) {
				msyslog(LOG_ERR,
					"cap_set_proc() failed to drop root privs: %m");
				exit(-1);
			}
			cap_free(caps);
		}
#  endif	/* HAVE_LINUX_CAPABILITY */
#  ifdef HAVE_SOLARIS_PRIVS
		if (priv_delset(lowprivs, "proc_setid") == -1) {
			msyslog(LOG_ERR, "priv_delset() failed:%m");
			exit(-1);
		}
		if (setppriv(PRIV_SET, PRIV_PERMITTED, lowprivs) == -1) {
			msyslog(LOG_ERR, "setppriv() failed:%m");
			exit(-1);
		}
		priv_freeset(lowprivs);
		priv_freeset(highprivs);
#  endif /* HAVE_SOLARIS_PRIVS */
		root_dropped = true;
	}	/* if (droproot) */
# endif	/* ENABLE_DROPROOT */

#ifdef ENABLE_SECCOMP
/* libssecomp sandboxing */
// Working on ARM
// #if defined(HAVE_SECCOMP) && (defined(__x86_64__) || defined(__i386__))
#if defined(HAVE_SECCOMP)

#ifdef ENABLE_KILL_ON_TRAP
  #define MY_SCMP_ACT SCMP_ACT_KILL
#else
  #define MY_SCMP_ACT SCMP_ACT_TRAP
#endif
	scmp_filter_ctx ctx = seccomp_init(MY_SCMP_ACT);

        signal_no_reset(SIGSYS, catchTrap);


	if (NULL == ctx) {
		msyslog(LOG_ERR, "sandbox: seccomp_init() failed: %m");
		return nonroot;
		}
	else
		msyslog(LOG_DEBUG, "sandbox: seccomp_init() succeeded");

int scmp_sc[] = {
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
	SCMP_SYS(getrandom),	/* 3.17 kernel */
#endif
	SCMP_SYS(getitimer),
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
	SCMP_SYS(poll),
	SCMP_SYS(pselect6),
	SCMP_SYS(read),
	SCMP_SYS(recvfrom),
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
	SCMP_SYS(setitimer),
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
	SCMP_SYS(write),
        SCMP_SYS(unlink),

#ifdef ENABLE_DNS_LOOKUP
	SCMP_SYS(clone),	/* threads */
	SCMP_SYS(futex),	/* sem_xxx, used by threads */
	SCMP_SYS(kill),		/* generate signal */
	SCMP_SYS(madvise),
	SCMP_SYS(mprotect),
	SCMP_SYS(set_robust_list),
	SCMP_SYS(sendmmsg),	/* DNS lookup */
	SCMP_SYS(socketpair),
	SCMP_SYS(statfs),
	SCMP_SYS(uname),
#endif

#ifdef HAVE_UTMPX_H
	/* for setutxent and friends in libntp/systime.c */
        /* Writing time-changed msg to accounting file */
        SCMP_SYS(access),
#endif

#ifdef REFCLOCK
	SCMP_SYS(nanosleep),
#endif
#ifdef CLOCK_SHM
        SCMP_SYS(shmget),
        SCMP_SYS(shmat),
#endif

	SCMP_SYS(fcntl64),
	SCMP_SYS(fstat64),

/* Arch Linux */
	SCMP_SYS(getpid),
	SCMP_SYS(gettid),
	SCMP_SYS(geteuid),
	SCMP_SYS(ppoll),
	SCMP_SYS(sendmsg),

#ifdef __x86_64__
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
				    "sandbox: seccomp_rule_add() failed: %m");
			}
		}
	}

	if (seccomp_load(ctx) < 0)
		msyslog(LOG_ERR, "sandbox: seccomp_load() failed: %m");	
	else {
		msyslog(LOG_DEBUG, "sandbox: seccomp_load() succeeded");
	}
#endif /* HAVE_SECCOMP */
#endif /* ENABLE_SECCOMP */

	return nonroot;
}

#ifdef ENABLE_SECCOMP
#ifdef HAVE_SECCOMP
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
 *  use gdb, break on catchTrap, get a trace.
 *
 * Option two:
 *  use strace
 *  sudo strace -t -f -o<filename> <path-to-ntpd> <args>
 *  when it crashes, the last syscall will be at the end of the log file
 *
 */
static void catchTrap(int sig)
{
	UNUSED_ARG(sig);	/* signal number */
	msyslog(LOG_ERR, "SIGSYS: got a trap. Probably seccomp omission. Bailing.");
	exit(1);
}
#endif /* HAVE_SECCOMP */
#endif /* ENABLE_SECCOMP */


/* end */
