/*
 * ntp_stdlib.h - Prototypes for NTP lib.
 */
#ifndef GUARD_NTP_STDLIB_H
#define GUARD_NTP_STDLIB_H

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdarg.h>

#include "declcond.h"	/* ntpd uses ntpd/declcond.h, others include/ */
#include "ntp_net.h"
#include "ntp_debug.h"
#include "ntp_malloc.h"
#include "ntp_syslog.h"

#ifdef HAVE_BSD_STRING_H
#include <bsd/string.h>
#endif

#ifndef __sysloglike
#define __sysloglike(fmt, args) ;
#endif /* __sysloglike */

#ifdef __GNUC__
#define NTP_PRINTF(fmt, args) __attribute__((__format__(__printf__, fmt, args)))
#else
#define NTP_PRINTF(fmt, args)
#endif

extern const char *ntpd_version(void);

extern	int	mprintf(const char *, ...) __sysloglike(1, 2);
extern	int	mvsnprintf(char *, size_t, const char *, va_list)
			__sysloglike(3, 0);
extern	void	msyslog(int, const char *, ...) __sysloglike(2, 3);
extern	void	init_logging	(const char *, uint32_t, int);
extern	int	change_logfile	(const char *, bool);
extern	void	reopen_logfile  (void);
extern	void	setup_logfile	(const char *);

extern	int	clocktime	(int, int, int, int, int, time_t, uint32_t, uint32_t *, uint32_t *);
extern	void	init_network	(void);
extern	int	ymd2yd		(int, int, int);

/* getopt.c */
struct option {
    const char* name;
    int has_arg;
    int* flag;
    int val;
};

int ntp_getopt(int argc, char *const argv[], const char *optstring);

int ntp_getopt_long(int argc, char* const argv[], const char *optstring,
		    const struct option *longopts, int *longindex);

/* mac_md5encrypt.c */
extern	uint32_t	addr2refid	(sockaddr_u *);

/* emalloc.c */
#ifndef EREALLOC_CALLSITE	/* ntp_malloc.h defines */
extern	void *	ereallocz	(void *, size_t, size_t, int);
extern	void *	oreallocarray	(void *optr, size_t nmemb, size_t size);
#define	erealloczsite(p, n, o, z, f, l) ereallocz((p), (n), (o), (z))
#define	emalloc(n)		ereallocz(NULL, (n), 0, false)
#define	emalloc_zero(c)		ereallocz(NULL, (c), 0, true)
#define	erealloc(p, c)		ereallocz((p), (c), 0, false)
#define erealloc_zero(p, n, o)	ereallocz((p), (n), (o), true)
#define ereallocarray(p, n, s)	oreallocarray((p), (n), (s))
#define eallocarray(n, s)	oreallocarray(NULL, (n), (s))
extern	char *	estrdup_impl(const char *);
#define	estrdup(s)		estrdup_impl(s)
#else
extern	void *	ereallocz	(void *, size_t, size_t, int,
				 const char *, int);
extern	void *	oreallocarray	(void *optr, size_t nmemb, size_t size,
				 const char *, int);
#define erealloczsite		ereallocz
#define	emalloc(c)		ereallocz(NULL, (c), 0, false, \
					  __FILE__, __LINE__)
#define	emalloc_zero(c)		ereallocz(NULL, (c), 0, true, \
					  __FILE__, __LINE__)
#define	erealloc(p, c)		ereallocz((p), (c), 0, false,	\
					  __FILE__, __LINE__)
#define	erealloc_zero(p, n, o)	ereallocz((p), n, (o), true,	\
					  __FILE__, __LINE__)
#define ereallocarray(p, n, s)	oreallocarray((p), (n), (s), \
 					  __FILE__, __LINE__)
#define eallocarray(n, s)	oreallocarray(NULL, (n), (s), \
 					  __FILE__, __LINE__)
extern	char *	estrdup_impl(const char *, const char *, int);
#define	estrdup(s) estrdup_impl((s), __FILE__, __LINE__)
#endif


extern	const char * eventstr	(int);
extern	const char * ceventstr	(int);
extern	const char * res_match_flags(unsigned short);
extern	const char * res_access_flags(unsigned short);
extern	const char * k_st_flags	(uint32_t);
extern	char *	statustoa	(int, int);
extern	sockaddr_u * netof6	(sockaddr_u *);
extern	const char * socktoa	(const sockaddr_u *);
extern	const char * sockporttoa(const sockaddr_u *);
extern	unsigned short	sock_hash(const sockaddr_u *) __attribute__((pure));
extern	const char *refid_str	(uint32_t, int);

extern	int	decodenetnum	(const char *, sockaddr_u *);

extern	void	signal_no_reset (int, void (*func)(int));
extern void signal_no_reset1(int, void (*func)(int, siginfo_t *, void *));

extern	void	getauthkeys 	(const char *);

/*
 * Variable declarations for libntp.
 */

/* getopt.c */
extern char *	ntp_optarg;		/* global argument pointer */
extern int	ntp_optind;		/* global argv index */

/* lib_strbuf.c */
extern bool	ipv4_works;
extern bool	ipv6_works;

/* ssl_init.c */
extern	void	ssl_init	(void);

/* strl-obsd.c */
#ifndef HAVE_STRLCPY		/* + */
/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
extern	size_t	strlcpy(char *dst, const char *src, size_t siz);
#endif
#ifndef HAVE_STRLCAT		/* + */
/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 */
extern	size_t	strlcat(char *dst, const char *src, size_t siz);
#endif

/* ntp_proto.c */
extern double	measured_tick;		/* non-overridable sys_tick */

/* systime.c */
extern double	sys_tick;		/* tick size or time to read */
extern double	sys_fuzz;		/* min clock read latency */
extern bool	trunc_os_clock;		/* sys_tick > measured_tick */

/* use these as return values for sort-comparison functions */
#define COMPARE_GREATERTHAN	1
#define COMPARE_EQUAL		0
#define COMPARE_LESSTHAN	-1

/* hack to ignore GCC Unused Result */
#define IGNORE(r) do{if(r){}}while(0)

extern bool sandbox(const bool droproot, char *user, const char *group,
	     const char *chrootdir, bool want_dynamic_interface_tracking);

#endif	/* GUARD_NTP_STDLIB_H */
