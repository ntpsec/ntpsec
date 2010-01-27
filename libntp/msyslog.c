/*
 * msyslog - either send a message to the terminal or print it on
 *	     the standard output.
 *
 * Converted to use varargs, much better ... jks
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>
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
# include "..\ports\winnt\libntp\messages.h"
#endif

int syslogit = 1;

FILE *	syslog_file;
char *	syslog_fname;
char *	syslog_abs_fname;

u_long ntp_syslogmask =  ~(u_long)0;	/* libntp default is all lit */

extern	char *progname;

/* Declare the local functions */
void	addto_syslog	(int, char *);
void	format_errmsg   (char *, int, const char *, int);


/*
 * This routine adds the contents of a buffer to the log
 */
void
addto_syslog(int level, char * buf)
{
	static const char *	last_progname;
	static const char *	prog;
	FILE *			out_file;

#if !defined(VMS) && !defined (SYS_VXWORKS)
	if (syslogit) {
		syslog(level, "%s", buf);
		out_file = NULL;
	} else
#endif /* VMS  && SYS_VXWORKS*/
	{
		out_file = (syslog_file) 
			       ? syslog_file
			       : (level <= LOG_ERR)
				     ? stderr 
				     : stdout;
		if (last_progname != progname) {
			last_progname = progname;
			prog = strrchr(progname, DIR_SEP);
			if (NULL == prog)
				prog = progname;
			else
				prog++;
		}
		/* 
		 * syslog() provides the timestamp, so if we're not
		 * using syslog, we must provide it.
		 */
		fprintf(out_file, "%s %s[%d]: %s", humanlogtime(),
				  prog, (int)getpid(), buf);
		fflush (out_file);
	}
#if DEBUG
	if (debug && out_file != stdout && out_file != stderr)
		printf("addto_syslog: %s", buf);
#endif
}


void
format_errmsg(char *nfmt, int lennfmt, const char *fmt, int errval)
{
	register char c;
	register char *n;
	register const char *f;
	size_t len;
	char *err;

	n = nfmt;
	f = fmt;
	while ((c = *f++) != '\0' && n < (nfmt + lennfmt - 2)) {
		if (c != '%') {
			*n++ = c;
			continue;
		}
		if ((c = *f++) != 'm') {
			*n++ = '%';
			*n++ = c;
			continue;
		}
		err = strerror(errval);
		len = strlen(err);

		/* Make sure we have enough space for the error message */
		if ((n + len) < (nfmt + lennfmt - 2)) {
			memcpy(n, err, len);
			n += len;
		}
	}
	/*
	 * syslog adds a trailing \n if not present, do the same so we
	 * have the same behavior with syslog and a log file.
	 */
	if (n > nfmt && '\n' != n[-1])
		*n++ = '\n';
	*n = '\0';
}


void msyslog(int level, const char *fmt, ...)
{
	va_list ap;
	char buf[1025], nfmt[256];
	int errval;

	/*
	 * Save the error value as soon as possible
	 */
	errval = errno;

#ifdef SYS_WINNT
	errval = GetLastError();
	if (NO_ERROR == errval)
		errval = errno;
#endif /* SYS_WINNT */

#if defined(__STDC__) || defined(HAVE_STDARG_H)
	va_start(ap, fmt);
#else
	va_start(ap);

	level = va_arg(ap, int);
	fmt = va_arg(ap, char *);
#endif
	format_errmsg(nfmt, sizeof(nfmt), fmt, errval);

	vsnprintf(buf, sizeof(buf), nfmt, ap);
	addto_syslog(level, buf);
	va_end(ap);
}
