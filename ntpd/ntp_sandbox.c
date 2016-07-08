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

#include <config.h>

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

#ifdef HAVE_LINUX_SECCOMP_H
# include <linux/seccomp.h>
# include <linux/filter.h>
# include <linux/audit.h>
#endif /* HAVE_LINUX_SECCOMP_H */

#ifdef ENABLE_DROPROOT
bool root_dropped;
uid_t sw_uid;
gid_t sw_gid;
char *endp;
struct group *gr;
struct passwd *pw;
#endif /* ENABLE_DROPROOT */

#include "ntp_syslog.h"
#include "ntp_stdlib.h"

bool sandbox(const bool droproot,
	     const char *user, const char *group,
	     const char *chrootdir,
	     bool want_dynamic_interface_tracking)
{
#ifndef HAVE_LINUX_CAPABILITY
	UNUSED_ARG(want_dynamic_interface_tracking);
#endif /* HAVE_LINUX_CAPABILITY */
	bool nonroot = false;
#if !defined(HAVE_LINUX_CAPABILITY) && !defined(HAVE_SOLARIS_PRIVS) && !defined(HAVE_SYS_CLOCKCTL)
	if (droproot) {
		msyslog(LOG_ERR,
			"root can't be dropped due to missing capabilities.");
		exit(-1);
	}
#endif /* !defined(HAVE_LINUX_CAPABILITY) && !defined(HAVE_SOLARIS_PRIVS)  && !defined(HAVE_SYS_CLOCKCTL) */
# ifdef ENABLE_DROPROOT
	if (droproot) {
		/* Drop super-user privileges and chroot now if the OS supports this */
#  ifdef HAVE_LINUX_CAPABILITY
		/* set flag: keep privileges across setuid() call. */
		if (prctl( PR_SET_KEEPCAPS, 1L, 0L, 0L, 0L ) == -1) {
			msyslog( LOG_ERR, "prctl( PR_SET_KEEPCAPS, 1L ) failed: %m" );
			exit(-1);
		}
#  elif HAVE_SOLARIS_PRIVS
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
				msyslog(LOG_ERR, "Cannot chdir() to`root after chroot(): %m");
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
		if (user && initgroups(user, sw_gid)) {
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
				msyslog(LOG_ERR, "setgroups(1, %d) failed: %m", sw_gid);
				exit (-1);
			}
		}
		else if (pw)
			if (0 != initgroups(pw->pw_name, pw->pw_gid)) {
				msyslog(LOG_ERR, "initgroups(<%s>, %d) filed: %m", pw->pw_name, pw->pw_gid);
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
			char *captext;
			
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

/* libssecomp sandboxing */
#if defined(HAVE_LINUX_SECCOMP_H) && (defined(__x86_64__) || defined(__i386__))
	scmp_filter_ctx ctx;

	if ((ctx = seccomp_init(SCMP_ACT_KILL)) < 0)
		msyslog(LOG_ERR, "%s: seccomp_init(SCMP_ACT_KILL) failed: %m", __func__);
	else {
		msyslog(LOG_DEBUG, "%s: seccomp_init(SCMP_ACT_KILL) succeeded", __func__);
	}

#ifdef __x86_64__
int scmp_sc[] = {
	SCMP_SYS(adjtimex),
	SCMP_SYS(bind),
	SCMP_SYS(brk),
	SCMP_SYS(chdir),
	SCMP_SYS(clock_gettime),
	SCMP_SYS(clock_settime),
	SCMP_SYS(close),
	SCMP_SYS(connect),
	SCMP_SYS(exit_group),
	SCMP_SYS(fstat),
	SCMP_SYS(fsync),
	SCMP_SYS(futex),
	SCMP_SYS(getitimer),
	SCMP_SYS(getsockname),
	SCMP_SYS(ioctl),
	SCMP_SYS(lseek),
	SCMP_SYS(madvise),
	SCMP_SYS(mmap),
	SCMP_SYS(munmap),
	SCMP_SYS(open),
	SCMP_SYS(poll),
	SCMP_SYS(read),
	SCMP_SYS(recvmsg),
	SCMP_SYS(rename),
	SCMP_SYS(rt_sigaction),
	SCMP_SYS(rt_sigprocmask),
	SCMP_SYS(rt_sigreturn),
	SCMP_SYS(select),
	SCMP_SYS(sendto),
	SCMP_SYS(setitimer),
	SCMP_SYS(setsid),
	SCMP_SYS(socket),
	SCMP_SYS(stat),
	SCMP_SYS(time),
	SCMP_SYS(write),
};
#endif
#ifdef __i386__
int scmp_sc[] = {
	SCMP_SYS(_newselect),
	SCMP_SYS(adjtimex),
	SCMP_SYS(brk),
	SCMP_SYS(chdir),
	SCMP_SYS(clock_gettime),
	SCMP_SYS(clock_settime),
	SCMP_SYS(close),
	SCMP_SYS(exit_group),
	SCMP_SYS(fsync),
	SCMP_SYS(futex),
	SCMP_SYS(getitimer),
	SCMP_SYS(madvise),
	SCMP_SYS(mmap),
	SCMP_SYS(mmap2),
	SCMP_SYS(munmap),
	SCMP_SYS(open),
	SCMP_SYS(poll),
	SCMP_SYS(read),
	SCMP_SYS(rename),
	SCMP_SYS(rt_sigaction),
	SCMP_SYS(rt_sigprocmask),
	SCMP_SYS(select),
	SCMP_SYS(setitimer),
	SCMP_SYS(setsid),
	SCMP_SYS(sigprocmask),
	SCMP_SYS(sigreturn),
	SCMP_SYS(socketcall),
	SCMP_SYS(stat64),
	SCMP_SYS(time),
	SCMP_SYS(write),
};
#endif
	{
		int i;

		for (i = 0; i < COUNTOF(scmp_sc); i++) {
			if (seccomp_rule_add(ctx,
			    SCMP_ACT_ALLOW, scmp_sc[i], 0) < 0) {
				msyslog(LOG_ERR,
				    "%s: seccomp_rule_add() failed: %m",
				    __func__);
			}
		}
	}

	if (seccomp_load(ctx) < 0)
		msyslog(LOG_ERR, "%s: seccomp_load() failed: %m",
		    __func__);	
	else {
		msyslog(LOG_DEBUG, "%s: seccomp_load() succeeded", __func__);
	}
#endif /* HAVE_LINUX_SECCOMP_H */

	return nonroot;
}

/* end */
