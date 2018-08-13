/*
 * msyslog - either send a message to the terminal or print it on
 *	     the standard output.
 *
 * Converted to use varargs, much better ... jks
 */

#include "config.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "ntp.h"
#include "ntp_debug.h"
#include "ntp_syslog.h"
#include "lib_strbuf.h"

/* Use XSI-compliant strerror_r(3), prototype in string.h.
 * Don't try moving this further up, else hilarity will ensue...
 */
#undef _GNU_SOURCE
#include <string.h>

/* start out with syslog and stderr, otherwise startup errors lost */
bool    syslogit = true;        /* log messages to syslog */
bool    termlogit = true;       /* duplicate to stdout/err */
bool	termlogit_pid = true;
bool	msyslog_include_timestamp = true;

static FILE *	syslog_file;
static char *	syslog_fname;
static char *	syslog_abs_fname;

/* libntp default ntp_syslogmask is all bits lit */
#define INIT_NTP_SYSLOGMASK	~(uint32_t)0
uint32_t ntp_syslogmask = INIT_NTP_SYSLOGMASK;

extern	char *	progname;

/* Declare the local functions */
static	int	mvfprintf(FILE *, const char *, va_list) NTP_PRINTF(2, 0);
const char *	humanlogtime(void);
static void	addto_syslog	(int, const char *);
#ifndef VSNPRINTF_PERCENT_M
static	void	errno_to_str(int, char *, size_t);
void	format_errmsg	(char *, size_t, const char *, int);

/* format_errmsg() is under #ifndef VSNPRINTF_PERCENT_M above */
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


/*
 * errno_to_str() - a thread-safe strerror() replacement.
 *		    Hides the varied signatures of strerror_r().
 */
static void
errno_to_str(
	int	err,
	char *	buf,
	size_t	bufsiz
	)
{
	int rc;

	rc = strerror_r(err, buf, bufsiz);
	if (rc)
		snprintf(buf, bufsiz, "strerror_r(%d): errno %d",
			 err, errno);
}
#endif	/* VSNPRINTF_PERCENT_M */


/* We don't want to clutter up the log with the year and day of the week,
   etc.; just the minimal date and time.  */
const char *
humanlogtime(void)
{
	char *		bp;
	time_t		cursec;
	struct tm	tmbuf, *tm;

	cursec = time(NULL);
	tm = localtime_r(&cursec, &tmbuf);
	if (!tm)
		return "-- --- --:--:--";

	bp = lib_getbuf();

#ifdef ENABLE_CLASSIC_MODE
	const char * const months[12] = {
	    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

	snprintf(bp, LIB_BUFLENGTH, "%2d %s %02d:%02d:%02d",
		 tm->tm_mday, months[tm->tm_mon],
		 tm->tm_hour, tm->tm_min, tm->tm_sec);
#else
	/* ISO 8601 is a better format, sort order equals time order */
	snprintf(bp, LIB_BUFLENGTH, "%04d-%02d-%02dT%02d:%02d:%02d",
		 tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec);
#endif /* ENABLE_CLASSIC_MODE */

	return bp;
}


/*
 * addto_syslog()
 * This routine adds the contents of a buffer to the syslog or an
 * application-specific logfile.
 */
static void
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
	bool		log_to_term;
	bool		log_to_file;
	int		pid;
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

	log_to_term = termlogit;
	log_to_file = false;
	if (syslogit)
		syslog(level, "%s", msg);
	else
		if (syslog_file != NULL)
			log_to_file = true;
#if defined(DEBUG) && DEBUG
	if (debug > 0) /* SPECIAL DEBUG */
		log_to_term = true;
#endif
	if (!(log_to_file || log_to_term))
		return;

	/* syslog() adds the timestamp, name, and pid */
	if (msyslog_include_timestamp)
		human_time = humanlogtime();
	else	/* suppress gcc pot. uninit. warning */
		human_time = NULL;
	if (termlogit_pid || log_to_file)
		pid = getpid();
	else	/* suppress gcc pot. uninit. warning */
		pid = -1;

	/* syslog() adds trailing \n if not present */
	if ('\n' != msg[strlen(msg) - 1])
		nl_or_empty = nl;
	else
		nl_or_empty = empty;

	if (log_to_term) {
		term_file = (level <= LOG_ERR)
				? stderr
				: stdout;
		if (msyslog_include_timestamp)
			fprintf(term_file, "%s ", human_time);
		if (termlogit_pid)
			fprintf(term_file, "%s[%d]: ", prog, pid);
		fprintf(term_file, "%s%s", msg, nl_or_empty);
		fflush(term_file);
	}

	if (log_to_file) {
		if (msyslog_include_timestamp)
			fprintf(syslog_file, "%s ", human_time);
		fprintf(syslog_file, "%s[%d]: %s%s", prog, pid, msg,
			nl_or_empty);
		fflush(syslog_file);
	}
}


int
mvsnprintf(
	char *		buf,
	size_t		bufsiz,
	const char *	fmt,
	va_list		ap
	)
{
#ifndef VSNPRINTF_PERCENT_M
	char		nfmt[256];
#else
	const char *	nfmt = fmt;
#endif
	int		errval;

	/*
	 * Save the error value as soon as possible
	 */
	errval = errno;

#ifndef VSNPRINTF_PERCENT_M
	format_errmsg(nfmt, sizeof(nfmt), fmt, errval);
#else
	errno = errval;
#endif
	return vsnprintf(buf, bufsiz, nfmt, ap);
}


static int
mvfprintf(
	FILE *		fp,
	const char *	fmt,
	va_list		ap
	)
{
#ifndef VSNPRINTF_PERCENT_M
	char		nfmt[256];
#else
	const char *	nfmt = fmt;
#endif
	int		errval;

	/*
	 * Save the error value as soon as possible
	 */
	errval = errno;

#ifndef VSNPRINTF_PERCENT_M
	format_errmsg(nfmt, sizeof(nfmt), fmt, errval);
#else
	errno = errval;
#endif
	return vfprintf(fp, nfmt, ap);
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
	rc = mvfprintf(stdout, fmt, ap);
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
	uint32_t		def_syslogmask,
	int		is_daemon
	)
{
	static bool	was_daemon;
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
	 * Forked children pass 0 for def_syslogmask.
	 */
	if (INIT_NTP_SYSLOGMASK == ntp_syslogmask &&
	    0 != def_syslogmask)
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

	if (is_daemon)
		was_daemon = true;
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
	if (debug) /* SPECIAL DEBUG */
		setlogmask(LOG_UPTO(LOG_DEBUG));
	else
#  endif /* DEBUG */
		setlogmask(LOG_UPTO(LOG_DEBUG)); /* @@@ was INFO */
# endif /* LOG_DAEMON */
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
	bool		leave_crumbs
	)
{
	FILE *		new_file;
	const char *	log_fname;
	char *		abs_fname;
	char		curdir[512];
	size_t		cd_octets;
	size_t		octets;

	//REQUIRE(fname != NULL);
	log_fname = fname;

	/*
	 * In a forked child of a parent which is logging to a file
	 * instead of syslog, syslog_file will be NULL and both
	 * syslog_fname and syslog_abs_fname will be non-NULL.
	 * If we are given the same filename previously opened
	 * and it's still open, there's nothing to do here.
	 */
	if (syslog_file != NULL && syslog_fname != NULL &&
	    0 == strcmp(syslog_fname, log_fname))
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
			abs_fname = estrdup(log_fname);
		TPRINT(1, ("attempting to open log %s\n", abs_fname));
		new_file = fopen(abs_fname, "a");
	}

	if (NULL == new_file) {
		free(abs_fname);
		return -1;
	}

	/* leave a pointer in the old log */
	if (leave_crumbs && (syslogit || log_fname != syslog_abs_fname))
		msyslog(LOG_NOTICE, "LOG: switching logging to file %s",
			abs_fname);

	if (syslog_file != NULL &&
	    syslog_file != stderr && syslog_file != stdout &&
	    fileno(syslog_file) != fileno(new_file))
		fclose(syslog_file);
	syslog_file = new_file;
	if (log_fname == syslog_abs_fname) {
		free(abs_fname);
	} else {
		if (syslog_abs_fname != NULL &&
		    syslog_abs_fname != syslog_fname)
			free(syslog_abs_fname);
		if (syslog_fname != NULL)
			free(syslog_fname);
		syslog_fname = estrdup(log_fname);
		syslog_abs_fname = abs_fname;
	}
	syslogit = false;

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
	const char *	name
	)
{
	if (NULL == syslog_fname && NULL != name) {
		if (-1 == change_logfile(name, true))
			msyslog(LOG_ERR, "LOG: Cannot open log file %s, %m",
				name);
		return ;
	}
	if (NULL == syslog_fname)
		return;

	if (-1 == change_logfile(syslog_fname, false))
		msyslog(LOG_ERR, "LOG: Cannot reopen log file %s, %m",
			syslog_fname);
}

/*
 * reopen_logfile()
 *
 * reopen current logfile in case the old file has been renamed by logrotate
 *
 */

void
reopen_logfile(void)
{
	FILE *  new_file;

	if (NULL == syslog_file) {
		return;  /* no log file, no clutter */
	}

	new_file = fopen(syslog_fname, "a");
	if (NULL == new_file) {
		msyslog(LOG_ERR, "LOG: reopen_logfile: couldn't open %s %m",
                        syslog_fname);
		return;
	}

	/* This is a hack to avoid cluttering the log if we would reuse
	 * the same file all over again.
	 * change_logfile compares filenos.  That doesn't work.
	 * Can't check for a new file using a length of 0 since
	 * newsyslog on FreeBSD puts a "logfile turned over" message there.
	 * This seems to work.
	 */
	if (ftell(syslog_file) == ftell(new_file)) {
		/* just for debugging */
		msyslog(LOG_INFO, "LOG: reopen_logfile: same length, ignored");
		fclose(new_file);
		return;
	}

	msyslog(LOG_INFO, "LOG: reopen_logfile: closing old file");
	fclose(syslog_file);
	syslog_file = new_file;
	msyslog(LOG_INFO, "LOG: reopen_logfile: using %s", syslog_fname);
}

