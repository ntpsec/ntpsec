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

#include "ntp.h"
#include "ntp_string.h"
#include "ntp_syslog.h"

#ifdef SYS_WINNT
# include <stdarg.h>
# include "..\ports\winnt\libntp\messages.h"
#endif


extern	char *	progname;

int	syslogit = 1;
int	msyslog_term = FALSE;	/* duplicate to stdout/err */
FILE *	syslog_file = NULL;
u_long	ntp_syslogmask =  ~(u_long)0;

/* Declare the local functions */
void	addto_syslog	(int, const char *);
void	format_errmsg   (char *, int, const char *, int);


/*
 * This routine adds the contents of a buffer to the log
 */
void
addto_syslog(
	int		level,
	const char *	buf
	)
{
	static char *	prevcall_progname;
	static char *	prog;
	FILE *		term_file;
	int		log_to_term;
	const char *	human_time;

	human_time = humanlogtime();
	log_to_term = msyslog_term;
	/* setup program basename static var prog if needed */
	if (progname != prevcall_progname) {
		prevcall_progname = progname;
		prog = strrchr(progname, DIR_SEP);
		if (prog != NULL)
			prog++;
		else
			prog = progname;
	}

#if !defined(VMS) && !defined(SYS_VXWORKS)
	if (syslogit)
		syslog(level, "%s", buf);
	else
#endif /* VMS  && SYS_VXWORKS*/
	if (syslog_file != NULL) {
		/* syslog() provides the timestamp, program, and pid */
		fprintf(syslog_file, "%s %s[%d]: %s", human_time, prog,
			(int)getpid(), buf);
		fflush(syslog_file);
	} else {
		log_to_term = TRUE;
	}
#if DEBUG
	if (debug > 0)
		log_to_term = TRUE;
#endif
	if (log_to_term) {
		term_file = (level <= LOG_ERR)
				? stderr
				: stdout;
		fprintf(term_file, "%s %s[%d]: %s", human_time, prog,
			(int)getpid(), buf);
		fflush(term_file);
	}
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
	if (n > nfmt && '\n' != *(n - 1))
		*n++ = '\n';
	*n = '\0';
}

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
	va_end(ap);
	addto_syslog(level, buf);
}
