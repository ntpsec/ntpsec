/*
 * ntp_workimpl.h - selects worker child implementation
 */
#ifndef GUARD_NTP_WORKIMPL_H
#define GUARD_NTP_WORKIMPL_H

/*
 * Some systems do not support fork() and don't have an alternate
 * threads implementation of ntp_intres.  Such systems are limited
 * to using numeric IP addresses.
 */
#if defined(SYS_WINNT)
# define WORK_THREAD
#else
# define WORK_THREAD
# define WORK_PIPE
#endif

#if defined(WORK_THREAD)
# define WORKER
#endif

#endif	/* GUARD_!NTP_WORKIMPL_H */
