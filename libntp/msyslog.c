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
#include "ntp_debug.h"
#include "ntp_string.h"
#include "ntp_syslog.h"

#ifdef SYS_WINNT
# include <stdarg.h>
# include "..\ports\winnt\libntp\messages.h"
#endif


int	syslogit = 1;
int	msyslog_term = FALSE;	/* duplicate to stdout/err */
FILE *	syslog_file;
char *	syslog_fname;
char *	syslog_abs_fname;

u_long ntp_syslogmask =  ~(u_long)0;	/* libntp default is all lit */

extern	char *	progname;

/* Declare the local functions */
void	addto_syslog	(int, const char *);
void	format_errmsg	(char *, size_t, const char *, int);


/*
 * errno_to_str() - a thread-safe strerror() replacement.
 *		    Hides the varied signatures of strerror_r().
 *		    For Windows, we have:
 *			#define errno_to_str isc_strerror
 */
#ifndef errno_to_str
void
errno_to_str(
	int	err,
	char *	buf,
	size_t	bufsiz
	)
{
# if defined(STRERROR_R_CHAR_P) || !HAVE_DECL_STRERROR_R
	char *	pstatic;

#  ifdef STRERROR_R_CHAR_P
	/*
	 * For older GNU strerror_r, the return value either points to
	 * buf, or to static storage.  We want the result always in buf
	 */
	pstatic = strerror_r(err, buf, bufsiz);
#  else
	pstatic = strerror(err);
#  endif
	if (pstatic != buf)
		strncpy(buf, pstatic, bufsiz);
# else
	int	rc;

	rc = strerror_r(err, buf, bufsiz);
	if (rc < 0)
		snprintf(buf, bufsiz, "strerror_r(%d): errno %d",
			 err, errno);
# endif
}
#endif	/* errno_to_str */


/*
 * addto_syslog()
 * This routine adds the contents of a buffer to the syslog or an
 * application-specific logfile.
 */
void
addto_syslog(
	int		level,
	const char *	msg
	)
{
	static char *	prevcall_progname;
	static char *	prog;
	const char	nl[] = "\n";
	const char	empty[] = "";
	FILE *		term_file;
	int		log_to_term;
	int		log_to_file;
	const char *	nl_or_empty;
	const char *	human_time;

	/* setup program basename static var prog if needed */
	if (progname != prevcall_progname) {
		prevcall_progname = progname;
		prog = strrchr(progname, DIR_SEP);
		if (prog != NULL)
			prog++;
		else
			prog = progname;
	}

	log_to_term = msyslog_term;
	log_to_file = FALSE;
#if !defined(VMS) && !defined(SYS_VXWORKS)
	if (syslogit)
		syslog(level, "%s", msg);
	else
#endif
		if (syslog_file != NULL)
			log_to_file = TRUE;
		else
			log_to_term = TRUE;
#if DEBUG
	if (debug > 0)
		log_to_term = TRUE;
#endif
	if (!(log_to_file || log_to_term))
		return;

	/* syslog() adds the timestamp, name, and pid */
	human_time = humanlogtime();

	/* syslog() adds trailing \n if not present */
	if ('\n' != msg[strlen(msg) - 1])
		nl_or_empty = nl;
	else
		nl_or_empty = empty;

	if (log_to_term) {
		term_file = (level <= LOG_ERR)
				? stderr
				: stdout;
		fprintf(term_file, "%s %s[%d]: %s%s", human_time, prog,
			(int)getpid(), msg, nl_or_empty);
		fflush(term_file);
	}

	if (log_to_file) {
		fprintf(syslog_file, "%s %s[%d]: %s%s", human_time,
			prog, (int)getpid(), msg, nl_or_empty);
		fflush(syslog_file);
	}
}


void
format_errmsg(
	char *		nfmt,
	size_t		lennfmt,
	const char *	fmt,
	int		errval
	)
{
	char errmsg[256];
	char c;
	char *n;
	const char *f;
	size_t len;

	n = nfmt;
	f = fmt;
	while ((c = *f++) != '\0' && n < (nfmt + lennfmt - 1)) {
		if (c != '%') {
			*n++ = c;
			continue;
		}
		if ((c = *f++) != 'm') {
			*n++ = '%';
			if ('\0' == c)
				break;
			*n++ = c;
			continue;
		}
		errno_to_str(errval, errmsg, sizeof(errmsg));
		len = strlen(errmsg);

		/* Make sure we have enough space for the error message */
		if ((n + len) < (nfmt + lennfmt - 1)) {
			memcpy(n, errmsg, len);
			n += len;
		}
	}
	*n = '\0';
}


int
mvsnprintf(
	char *		buf,
	size_t		bufsiz,
	const char *	fmt,
	va_list		ap
	)
{
	char	nfmt[256];
	int	errval;
	int	rc;

	/*
	 * Save the error value as soon as possible
	 */
#ifdef SYS_WINNT
	errval = GetLastError();
	if (NO_ERROR == errval)
#endif /* SYS_WINNT */
		errval = errno;

	format_errmsg(nfmt, sizeof(nfmt), fmt, errval);

	rc = vsnprintf(buf, bufsiz, nfmt, ap);
	/* terminate buf, as MS vsnprintf() does not when truncating */
	buf[bufsiz - 1] = '\0';

	return rc;
}


int
mvfprintf(
	FILE *		fp,
	const char *	fmt,
	va_list		ap
	)
{
	char	nfmt[256];
	int	errval;
	int	rc;

	/*
	 * Save the error value as soon as possible
	 */
#ifdef SYS_WINNT
	errval = GetLastError();
	if (NO_ERROR == errval)
#endif /* SYS_WINNT */
		errval = errno;

	format_errmsg(nfmt, sizeof(nfmt), fmt, errval);
	rc = vfprintf(fp, nfmt, ap);

	return rc;
}


int
mfprintf(
	FILE *		fp,
	const char *	fmt,
	...
	)
{
	va_list		ap;
	int		rc;

	va_start(ap, fmt);
	rc = mvfprintf(fp, fmt, ap);
	va_end(ap);

	return rc;
}


int
mprintf(
	const char *	fmt,
	...
	)
{
	va_list		ap;
	int		rc;

	va_start(ap, fmt);
	rc = mvfprintf(stderr, fmt, ap);
	va_end(ap);

	return rc;
}


int
msnprintf(
	char *		buf,
	size_t		bufsiz,
	const char *	fmt,
	...
	)
{
	va_list	ap;
	size_t	rc;

	va_start(ap, fmt);
	rc = mvsnprintf(buf, bufsiz, fmt, ap);
	va_end(ap);

	return rc;
}


void
msyslog(
	int		level,
	const char *	fmt,
	...
	)
{
	char	buf[1024];
	va_list	ap;

	va_start(ap, fmt);
	mvsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	addto_syslog(level, buf);
}


/*
 * Initialize the logging
 *
 * Called once per process, including forked children.
 */
void
init_logging(
	const char *	name,
	u_long		def_syslogmask,
	const char *	version_str,
	int		is_daemon
	)
{
	static int	was_daemon;
	const char *	cp;
	const char *	pname;

	/*
	 * ntpd defaults to only logging sync-category events, when
	 * NLOG() is used to conditionalize.  Other libntp clients
	 * leave it alone so that all NLOG() conditionals will fire.
	 * This presumes all bits lit in ntp_syslogmask can't be
	 * configured via logconfig and all lit is thereby a sentinel
	 * that ntp_syslogmask is still at its default from libntp,
	 * keeping in mind this function is called in forked children
	 * where it has already been called in the parent earlier.
	 */
	if (~(u_long)0 == ntp_syslogmask && 0 != def_syslogmask)
		ntp_syslogmask = def_syslogmask; /* set more via logconfig */

	/*
	 * Logging.  This may actually work on the gizmo board.  Find a name
	 * to log with by using the basename
	 */
	cp = strrchr(name, DIR_SEP);
	if (NULL == cp)
		pname = name;
	else
		pname = 1 + cp;	/* skip DIR_SEP */
	progname = estrdup(pname);
#ifdef SYS_WINNT			/* strip ".exe" */
	cp = strrchr(progname, '.');
	if (NULL != cp && !strcasecmp(cp, ".exe"))
		progname[cp - progname] = '\0';
#endif

#if !defined(VMS)

	if (is_daemon)
		was_daemon = TRUE;
# ifndef LOG_DAEMON
	openlog(progname, LOG_PID);
# else /* LOG_DAEMON */

#  ifndef LOG_NTP
#	define	LOG_NTP LOG_DAEMON
#  endif
	openlog(progname, LOG_PID | LOG_NDELAY, (was_daemon) 
						    ? LOG_NTP
						    : 0);
#  ifdef DEBUG
	if (debug)
		setlogmask(LOG_UPTO(LOG_DEBUG));
	else
#  endif /* DEBUG */
		setlogmask(LOG_UPTO(LOG_DEBUG)); /* @@@ was INFO */
# endif /* LOG_DAEMON */
#endif	/* !VMS */

	if (NULL != version_str)
		msyslog(LOG_NOTICE, "%s", version_str);
}


/*
 * change_logfile()
 *
 * Used to change from syslog to a logfile, or from one logfile to
 * another, and to reopen logfiles after forking.  On systems where
 * ntpd forks, deals with converting relative logfile paths to
 * absolute (root-based) because we reopen logfiles after the current
 * directory has changed.
 */
int
change_logfile(
	const char *	fname,
	const char *	version_str
	)
{
	FILE *		new_file;
	const char *	log_fname;
	char *		abs_fname;
#if !defined(SYS_WINNT) && !defined(SYS_VXWORKS) && !defined(VMS)
	char		curdir[512];
	size_t		cd_octets;
	size_t		octets;
#endif	/* POSIX */

	NTP_REQUIRE(fname != NULL);
	log_fname = fname;

	/*
	 * In a forked child of a parent which is logging to a file
	 * instead of syslog, syslog_file will be NULL and both
	 * syslog_fname and syslog_abs_fname will be non-NULL.
	 * If we are given the same filename previously opened
	 * and it's still open, there's nothing to do here.
	 */
	if (syslog_file != NULL && syslog_fname != NULL &&
	    (log_fname == syslog_fname ||
	     0 == strcmp(syslog_fname, log_fname)))
		return 0;

	if (0 == strcmp(log_fname, "stderr")) {
		new_file = stderr;
		abs_fname = estrdup(log_fname);
	} else if (0 == strcmp(log_fname, "stdout")) {
		new_file = stdout;
		abs_fname = estrdup(log_fname);
	} else {
		if (syslog_fname != NULL &&
		    0 == strcmp(log_fname, syslog_fname))
			log_fname = syslog_abs_fname;
#if !defined(SYS_WINNT) && !defined(SYS_VXWORKS) && !defined(VMS)
		if (log_fname != syslog_abs_fname &&
		    DIR_SEP != log_fname[0] &&
		    0 != strcmp(log_fname, "stderr") &&
		    0 != strcmp(log_fname, "stdout") &&
		    NULL != getcwd(curdir, sizeof(curdir))) {
			cd_octets = strlen(curdir);
			/* trim any trailing '/' */
			if (cd_octets > 1 &&
			    DIR_SEP == curdir[cd_octets - 1])
				cd_octets--;
			octets = cd_octets;
			octets += 1;	/* separator '/' */
			octets += strlen(log_fname);
			octets += 1;	/* NUL terminator */
			abs_fname = emalloc(octets);
			snprintf(abs_fname, octets, "%.*s%c%s",
				 (int)cd_octets, curdir, DIR_SEP,
				 log_fname);
		} else
#endif
			abs_fname = estrdup(log_fname);
		TRACE(1, ("attempting to open log %s\n", abs_fname));
		new_file = fopen(abs_fname, "a");
	}

	if (NULL == new_file) {
		free(abs_fname);
		return -1;
	}

	/* leave a pointer in the old log */
	if (syslogit || log_fname != syslog_abs_fname)
		msyslog(LOG_NOTICE, "switching logging to file %s",
			abs_fname);

	if (syslog_file != NULL &&
	    syslog_file != stderr && syslog_file != stdout &&
	    fileno(syslog_file) != fileno(new_file))
		fclose(syslog_file);
	syslog_file = new_file;
	if (log_fname != syslog_abs_fname) {
		if (syslog_abs_fname != NULL &&
		    syslog_abs_fname != syslog_fname)
			free(syslog_abs_fname);
		if (syslog_fname != NULL)
			free(syslog_fname);
		syslog_fname = estrdup(log_fname);
		syslog_abs_fname = abs_fname;
	}
	syslogit = FALSE;
	if (NULL != version_str)
		msyslog(LOG_NOTICE, "%s", version_str);

	return 0;
}


/*
 * setup_logfile()
 *
 * Redirect logging to a file if requested with -l/--logfile or via
 * ntp.conf logfile directive.
 *
 * This routine is invoked three different times in the sequence of a
 * typical daemon ntpd with DNS lookups to do.  First it is invoked in
 * the original ntpd process, then again in the daemon after closing
 * all descriptors.  In both of those cases, ntp.conf has not been
 * processed, so only -l/--logfile will trigger logfile redirection in
 * those invocations.  Finally, if DNS names are resolved, the worker
 * child invokes this routine after its fork and close of all
 * descriptors.  In this case, ntp.conf has been processed and any
 * "logfile" directive needs to be honored in the child as well.
 */
void
setup_logfile(
	const char *	name,
	const char *	version_str
	)
{
	if (NULL == syslog_fname && NULL != name) {
		if (-1 == change_logfile(name, version_str))
			msyslog(LOG_ERR, "Cannot open log file %s, %m",
				name);
		return ;
	} 
	if (NULL == syslog_fname)
		return;

	if (-1 == change_logfile(syslog_fname, version_str))
		msyslog(LOG_ERR, "Cannot reopen log file %s, %m",
			syslog_fname);
}
