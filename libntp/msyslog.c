/*
 * msyslog - either send a message to the terminal or print it on
 *	     the standard output.
 *
 * Converted to use varargs, much better ... jks
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <stdio.h>

#include "ntp_types.h"
#include "ntp_string.h"
#include "ntp_syslog.h"
#include "ntp_stdlib.h"

#ifdef SYS_WINNT
# include <stdarg.h>
# include "..\ports\winnt\libntp\log.h"
# include "..\ports\winnt\libntp\messages.h"
#endif

int syslogit = 1;

FILE *syslog_file = NULL;

u_long ntp_syslogmask =  ~ (u_long) 0;

#ifdef SYS_WINNT
static char separator = '\\';
#else
static char separator = '/';
#endif /* SYS_WINNT */
extern	char *progname;

#if defined(__STDC__) || defined(HAVE_STDARG_H)
void msyslog(int level, const char *fmt, ...)
#else /* defined(__STDC__) || defined(HAVE_STDARG_H) */
     /*VARARGS*/
     void msyslog(va_alist)
     va_dcl
#endif /* defined(__STDC__) || defined(HAVE_STDARG_H) */
{
#if defined(__STDC__) || defined(HAVE_STDARG_H)
#else
	int level;
	const char *fmt;
#endif
	va_list ap;
	char buf[1025], nfmt[256];
	register char c;
	register char *n, *prog;
	register const char *f;

	/*
	 * Save the error value as soon as possible
	 */
#ifdef SYS_WINNT
	int olderrno = GetLastError();
#else
	int olderrno = errno;
#endif
	char *err;

#if defined(__STDC__) || defined(HAVE_STDARG_H)
	va_start(ap, fmt);
#else
	va_start(ap);

	level = va_arg(ap, int);
	fmt = va_arg(ap, char *);
#endif

	n = nfmt;
	f = fmt;
	while ((c = *f++) != '\0' && c != '\n' && n < &nfmt[252]) {
		if (c != '%') {
			*n++ = c;
			continue;
		}
		if ((c = *f++) != 'm') {
			*n++ = '%';
			*n++ = c;
			continue;
		}
		err = 0;
		err = strerror(olderrno);
		if ((n + strlen(err)) < &nfmt[254]) {
			strcpy(n, err);
			n += strlen(err);
		}
	}
#if !defined(VMS)
	if (!syslogit)
#endif /* VMS */
	    *n++ = '\n';
	*n = '\0';

	vsnprintf(buf, sizeof(buf), nfmt, ap);
#if DEBUG
	if (debug)
		printf("msyslog: %s\n", buf);
#endif
#if !defined(VMS) && !defined (SYS_VXWORKS)
	if (syslogit)
	    syslog(level, "%s", buf);
	else
#endif /* VMS  && SYS_VXWORKS*/
	{
		FILE *out_file = syslog_file ? syslog_file
			: level <= LOG_ERR ? stderr : stdout;
		/* syslog() provides the timestamp, so if we're not using
		   syslog, we must provide it. */
		prog = strrchr(progname, separator);
		if (prog == NULL)
		    prog = progname;
		else
		    prog++;
		(void) fprintf(out_file, "%s ", humanlogtime ());
		(void) fprintf(out_file, "%s[%d]: %s", prog, (int)getpid(), buf);
		fflush (out_file);
	}
	va_end(ap);
}
