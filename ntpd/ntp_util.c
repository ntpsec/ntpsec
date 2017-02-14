/*
 * ntp_util.c - stuff I didn't have any other place for
 */
#include "config.h"

#include "ntpd.h"
#include "ntp_filegen.h"
#include "ntp_stdlib.h"
#include "ntp_assert.h"
#include "ntp_calendar.h"
#include "ntp_leapsec.h"
#include "lib_strbuf.h"

#include <stdio.h>
#include <libgen.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef HAVE_GETRUSAGE
#include <sys/time.h>
#include <sys/resource.h>
#endif

/*
 * Defines used by the leapseconds stuff
 */
/* #define	MAX_TAI	100			* max TAI offset (s) UNUSED */
/* #define	L_DAY	86400UL			* seconds per day UNUSED */
/* #define	L_YEAR	(L_DAY * 365)		* days per year UNUSED */
/* #define	L_LYEAR	(L_YEAR + L_DAY)	* days per leap year UNUSED */
/* #define	L_4YEAR	(L_LYEAR + 3 * L_YEAR)	* days per leap cycle UNUSED */
/* #define	L_CENT	(L_4YEAR * 25)		* days per century UNUSED */

/*
 * This contains odds and ends, including the hourly stats, various
 * configuration items, leapseconds stuff, etc.
 */
/*
 * File names
 */
static	char *key_file_name;		/* keys file name */
static char	  *leapfile_name;		/* leapseconds file name */
static struct stat leapfile_stat;	/* leapseconds file stat() buffer */
static bool        have_leapfile = false;
char	*stats_drift_file;		/* frequency file name */
static double wander_resid;		/* last frequency update */
double	wander_threshold = 1e-7;	/* initial frequency threshold */

/*
 * Statistics file stuff
 */
#ifndef NTP_VAR
# define NTP_VAR "/var/NTP/"		/* NOTE the trailing '/' */
#endif


char statsdir[MAXFILENAME] = NTP_VAR;
static FILEGEN clockstats;
static FILEGEN cryptostats;
static FILEGEN loopstats;
static FILEGEN peerstats;
static FILEGEN protostats;
static FILEGEN rawstats;
static FILEGEN sysstats;
static FILEGEN timingstats;
static FILEGEN usestats;

/*
 * This controls whether stats are written to the fileset. Provided
 * so that ntpq can turn off stats when the file system fills up. 
 */
bool stats_control;

/*
 * Last frequency written to file.
 */
static double prev_drift_comp;		/* last frequency update */

/*
 * Function prototypes
 */
static	void	record_sys_stats(void);
static	void	record_use_stats(void);
	void	ntpd_time_stepped(void);
static  void	check_leap_expiration(int, uint32_t, const time_t*);

/* 
 * Prototypes
 */
#ifdef DEBUG
void	uninit_util(void);
#endif

/*
 * uninit_util - free memory allocated by init_util
 */
#ifdef DEBUG
void
uninit_util(void)
{
	if (stats_drift_file) {
		free(stats_drift_file);
		stats_drift_file = NULL;
	}
	if (key_file_name) {
		free(key_file_name);
		key_file_name = NULL;
	}
	filegen_unregister("clockstats");
	filegen_unregister("cryptostats");
	filegen_unregister("loopstats");
	filegen_unregister("rawstats");
	filegen_unregister("sysstats");
	filegen_unregister("peerstats");
	filegen_unregister("protostats");
	filegen_unregister("timingstats");
	filegen_unregister("usestats");
}
#endif /* DEBUG */


/*
 * init_util - initialize the util module of ntpd
 */
void
init_util(void)
{
	filegen_register(statsdir, "clockstats",  &clockstats);
	filegen_register(statsdir, "cryptostats", &cryptostats);
	filegen_register(statsdir, "loopstats",	  &loopstats);
	filegen_register(statsdir, "rawstats",	  &rawstats);
	filegen_register(statsdir, "sysstats",	  &sysstats);
	filegen_register(statsdir, "peerstats",	  &peerstats);
	filegen_register(statsdir, "protostats",  &protostats);
	filegen_register(statsdir, "timingstats", &timingstats);
	filegen_register(statsdir, "usestats",	  &usestats);

	/*
	 * register with libntp ntp_set_tod() to call us back
	 * when time is stepped.
	 */
	step_callback = &ntpd_time_stepped;
#ifdef DEBUG
	atexit(&uninit_util);
#endif /* DEBUG */
}


/*
 * hourly_stats - print some interesting stats
 */
#define IGNORE(r) do{if(r){}}while(0)

void drift_write(char *driftfile, double drift)
{
	int fd;
	char tmpfile[PATH_MAX], driftcopy[PATH_MAX];
	char driftval[32];
	strlcpy(driftcopy, driftfile, PATH_MAX);
	strlcpy(tmpfile, dirname(driftcopy), sizeof(tmpfile));
	strlcat(tmpfile, "/driftXXXXXX", sizeof(tmpfile));
	/* coverity[secure_temp] Warning is bogus on POSIX-compliant systems */
	if ((fd = mkstemp(tmpfile)) < 0) {
	    msyslog(LOG_ERR, "frequency file %s: %m", tmpfile);
	    return;
	}
	snprintf(driftval, sizeof(driftval), "%.6f\n", drift);
	IGNORE(write(fd, driftval, strlen(driftval)));
	(void)close(fd);
	/* atomic */
	if (rename(tmpfile, driftfile))
	    msyslog(LOG_WARNING,
		    "Unable to rename temp drift file %s to %s, %m",
		    tmpfile, driftfile);
}
void
write_stats(void)
{
	record_sys_stats();
	record_use_stats();
	if (stats_drift_file != 0) {

		/*
		 * When the frequency file is written, initialize the
		 * prev_drift_comp and wander_resid. Thereafter,
		 * reduce the wander_resid by half each hour. When
		 * the difference between the prev_drift_comp and
		 * drift_comp is less than the wander_resid, update
		 * the frequncy file. This minimizes the file writes to
		 * nonvolaile storage.
		 */
#ifdef DEBUG
		if (debug)
			printf("write_stats: frequency %.6lf thresh %.6lf, freq %.6lf\n",
			    (prev_drift_comp - drift_comp) * 1e6, wander_resid *
			    1e6, drift_comp * 1e6);
#endif
		if (fabs(prev_drift_comp - drift_comp) < wander_resid) {
			wander_resid *= 0.5;
			return;
		}
		prev_drift_comp = drift_comp;
		wander_resid = wander_threshold;
		drift_write(stats_drift_file, drift_comp * 1e6);
	}
}


/*
 * stats_config - configure the stats operation
 */
bool drift_read(const char *drift_file, double *drift)
{
	FILE *fp;
	if ((fp = fopen(drift_file, "r")) == NULL) {
	    return false;
	}
	if (fscanf(fp, "%lf", drift) != 1) {
	    msyslog(LOG_ERR,
		    "format error frequency file %s",
		    drift_file);
	    fclose(fp);
	    return false;
	}
	fclose(fp);
	return true;
}

void
stats_config(
	int item,
	const char *invalue	/* only one type so far */
	)
{
	FILE	*fp;
	const char *value;
	int	len;
	double	new_drift = 0;
	l_fp	now;
	time_t  ttnow;

	value = invalue;

	switch (item) {

	/*
	 * Open and read frequency file.
	 */
	case STATS_FREQ_FILE:
		if (!value || (len = strlen(value)) == 0)
			break;

		stats_drift_file = erealloc(stats_drift_file, len + 1);
		memcpy(stats_drift_file, value, (size_t)(len+1));

		/*
		 * Open drift file and read frequency. If the file is
		 * missing or contains errors, tell the loop to reset.
		 */
		if (drift_read(stats_drift_file, &new_drift)) {
		    loop_config(LOOP_FREQ, new_drift);
		    prev_drift_comp = drift_comp;
		}
		break;

	/*
	 * Specify statistics directory.
	 */
	case STATS_STATSDIR:

		/* - 1 since value may be missing the DIR_SEP. */
		if (strlen(value) >= sizeof(statsdir) - 1) {
			msyslog(LOG_ERR,
			    "statsdir too long (>%d, sigh)",
			    (int)sizeof(statsdir) - 2);
		} else {
			bool add_dir_sep;
			int value_l;

			/* Add a DIR_SEP unless we already have one. */
			value_l = strlen(value);
			if (0 == value_l)
				add_dir_sep = false;
			else
				add_dir_sep = (DIR_SEP !=
				    value[value_l - 1]);

			if (add_dir_sep)
				snprintf(statsdir, sizeof(statsdir),
				    "%s%c", value, DIR_SEP);
			else
				snprintf(statsdir, sizeof(statsdir),
				    "%s", value);
			filegen_statsdir();
		}
		break;

	/*
	 * Open pid file.
	 */
	case STATS_PID_FILE:
		if ((fp = fopen(value, "w")) == NULL) {
			msyslog(LOG_ERR, "pid file %s: %m",
			    value);
			break;
		}
		fprintf(fp, "%d", (int)getpid());
		fclose(fp);
		break;

	/*
	 * Read leapseconds file.
	 *
	 * Note: Currently a leap file without SHA1 signature is
	 * accepted, but if there is a signature line, the signature
	 * must be valid or the file is rejected.
	 */
	case STATS_LEAP_FILE:
		if (!value || (len = strlen(value)) == 0)
			break;

		leapfile_name = erealloc(leapfile_name, len + 1);
		memcpy(leapfile_name, value, len + 1);

		if (leapsec_load_file(
			    leapfile_name, &leapfile_stat, true, true))
		{
			leap_signature_t lsig;

			get_systime(&now);
			time(&ttnow);
			leapsec_getsig(&lsig);
			mprintf_event(EVNT_TAI, NULL,
				      "%d leap %s %s %s",
				      lsig.taiof,
				      fstostr(lsig.ttime),
				      leapsec_expired(lfpuint(now), NULL)
					  ? "expired"
					  : "expires",
				      fstostr(lsig.etime));
			have_leapfile = true;

			/* force an immediate daily expiration check of
			 * the leap seconds table
			 */
			check_leap_expiration(true, lfpuint(now), &ttnow);
		}
		break;

	default:
		/* oh well */
		break;
	}
}

/*
 * record_peer_stats - write peer statistics to file
 *
 * file format:
 * day (MJD)
 * time (s past UTC midnight)
 * IP address (old format) or drivername(unit) (new format)
 * status word (hex)
 * offset
 * delay
 * dispersion
 * jitter
*/
void
record_peer_stats(
	struct peer *peer,
	int	status
	)
{
	l_fp	now;
	u_long	day;

	if (!stats_control)
		return;

	get_systime(&now);
	filegen_setup(&peerstats, lfpuint(now));
	day = lfpuint(now) / 86400 + MJD_1900;
	setlfpuint(now, lfpuint(now) % 86400);
	if (peerstats.fp != NULL) {
		fprintf(peerstats.fp,
		    "%lu %s %s %x %.9f %.9f %.9f %.9f\n", day,
		    ulfptoa(now, 3), socktoa(&peer->srcadr), status, peer->offset,
		    peer->delay, peer->disp, peer->jitter);
		fflush(peerstats.fp);
	}
}

static const char *
peerlabel(const struct peer *peer)
{
#if defined(REFCLOCK) && !defined(ENABLE_CLASSIC_MODE)
 	if (peer->procptr != NULL)
		return refclock_name(peer);
	else
#endif /* defined(REFCLOCK) && !defined(ENABLE_CLASSIC_MODE)*/
		return socktoa(&peer->srcadr);
}

/*
 * record_loop_stats - write loop filter statistics to file
 *
 * file format:
 * day (MJD)
 * time (s past midnight)
 * offset
 * frequency (PPM)
 * jitter
 * wnder (PPM)
 * time constant (log2)
 */
void
record_loop_stats(
	double	offset,		/* offset */
	double	freq,		/* frequency (PPM) */
	double	jitter,		/* jitter */
	double	wander,		/* wander (PPM) */
	int spoll
	)
{
	l_fp	now;
	u_long	day;

	if (!stats_control)
		return;

	get_systime(&now);
	filegen_setup(&loopstats, lfpuint(now));
	day = lfpuint(now) / 86400 + MJD_1900;
	setlfpuint(now, lfpuint(now) % 86400);
	if (loopstats.fp != NULL) {
		fprintf(loopstats.fp, "%lu %s %.9f %.6f %.9f %.6f %d\n",
		    day, ulfptoa(now, 3), offset, freq * 1e6, jitter,
		    wander * 1e6, spoll);
		fflush(loopstats.fp);
	}
}


/*
 * record_clock_stats - write clock statistics to file
 *
 * file format:
 * day (MJD)
 * time (s past midnight)
 * IP address (old format) or drivername(unit) new format
 * text message
 */
void
record_clock_stats(
	struct peer *peer,
	const char *text	/* timecode string */
	)
{
	l_fp	now;
	u_long	day;

	if (!stats_control)
		return;

	get_systime(&now);
	filegen_setup(&clockstats, lfpuint(now));
	day = lfpuint(now) / 86400 + MJD_1900;
	setlfpuint(now, lfpuint(now) % 86400);
	if (clockstats.fp != NULL) {
		fprintf(clockstats.fp, "%lu %s %s %s\n", day,
		    ulfptoa(now, 3), peerlabel(peer), text);
		fflush(clockstats.fp);
	}
}


/*
 * mprintf_clock_stats - write clock statistics to file with
 *			msnprintf-style formatting.
 */
int
mprintf_clock_stats(
	struct peer *peer,
	const char *fmt,
	...
	)
{
	va_list	ap;
	int	rc;
	char	msg[512];

	va_start(ap, fmt);
	rc = mvsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);
	if (stats_control)
		record_clock_stats(peer, msg);

	return rc;
}

/*
 * record_raw_stats - write raw timestamps to file
 *
 * file format
 * day (MJD)
 * time (s past midnight)
 * peer ip address
 * IP address old format) or drivername(unit) (new format)
 * t1 t2 t3 t4 timestamps
 */
void
record_raw_stats(
	sockaddr_u *srcadr,
	sockaddr_u *dstadr,
	l_fp	*t1,		/* originate timestamp */
	l_fp	*t2,		/* receive timestamp */
	l_fp	*t3,		/* transmit timestamp */
	l_fp	*t4,		/* destination timestamp */
	int	leap,
	int	version,
	int	mode,
	int	stratum,
	int	ppoll,
	int	precision,
	double	root_delay,	/* seconds */
	double	root_dispersion,/* seconds */
	uint32_t	refid,
	u_int	outcount

	)
{
	l_fp	now;
	u_long	day;

	if (!stats_control)
		return;

	get_systime(&now);
	filegen_setup(&rawstats, lfpuint(now));
	day = lfpuint(now) / 86400 + MJD_1900;
	setlfpuint(now, lfpuint(now) % 86400);
	if (rawstats.fp != NULL) {
		fprintf(rawstats.fp, "%lu %s %s %s %s %s %s %s %d %d %d %d %d %d %.6f %.6f %s %d\n",
		    day, ulfptoa(now, 3),
		    socktoa(srcadr), dstadr ?  socktoa(dstadr) : "-",
		    ulfptoa(*t1, 9), ulfptoa(*t2, 9),
		    ulfptoa(*t3, 9), ulfptoa(*t4, 9),
		    leap, version, mode, stratum, ppoll, precision,
		    root_delay, root_dispersion, refid_str(refid, stratum),
		    outcount);
		fflush(rawstats.fp);
	}
}


/*
 * record_sys_stats - write system statistics to file
 *
 * file format
 * day (MJD)
 * time (s past midnight)
 * time since reset
 * packets received
 * packets for this host
 * current version
 * old version
 * access denied
 * bad length or format
 * bad authentication
 * declined
 * rate exceeded
 * KoD sent
 */
void
record_sys_stats(void)
{
	l_fp	now;
	u_long	day;

	if (!stats_control)
		return;

	get_systime(&now);
	filegen_setup(&sysstats, lfpuint(now));
	day = lfpuint(now) / 86400 + MJD_1900;
	setlfpuint(now, lfpuint(now) % 86400);
	if (sysstats.fp != NULL) {
		fprintf(sysstats.fp,
		    "%lu %s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu\n",
		    day, ulfptoa(now, 3), current_time - sys_stattime,
		    sys_received, sys_processed, sys_newversion,
		    sys_oldversion, sys_restricted, sys_badlength,
		    sys_badauth, sys_declined, sys_limitrejected,
		    sys_kodsent);
		fflush(sysstats.fp);
		proto_clr_stats();
	}
}


/*
 * record_use_stats - write usage statistics to file
 *
 * file format
 * day (MJD)
 * time (s past midnight)
 * time since reset
 */
void record_use_stats(void)
{
#ifdef HAVE_GETRUSAGE
	l_fp	now;
	u_long	day;
	struct rusage usage;
	static struct rusage oldusage;
	/* Descriptions in NetBSD and FreeBSD are better than Linux
	 * man getrusage */

	if (!stats_control)
		return;

	get_systime(&now);
	filegen_setup(&usestats, lfpuint(now));
	day = lfpuint(now) / 86400 + MJD_1900;
	setlfpuint(now, lfpuint(now) % 86400);
	if (usestats.fp != NULL) {
		double utime, stime;
		getrusage(RUSAGE_SELF, &usage);
		utime =  usage.ru_utime.tv_usec - oldusage.ru_utime.tv_usec;
		utime /= 1E6;
		utime += usage.ru_utime.tv_sec -  oldusage.ru_utime.tv_sec;
		stime =  usage.ru_stime.tv_usec - oldusage.ru_stime.tv_usec;
		stime /= 1E6;
		stime += usage.ru_stime.tv_sec -  oldusage.ru_stime.tv_sec;
		fprintf(usestats.fp,
		    "%lu %s %lu %.3f %.3f %lu %lu %lu %lu %lu %lu %lu %lu %lu\n",
		    day, ulfptoa(now, 3), current_time - use_stattime,
		    utime, stime,
		    usage.ru_minflt -  oldusage.ru_minflt,
		    usage.ru_majflt -  oldusage.ru_majflt,
		    usage.ru_nswap -   oldusage.ru_nswap,
		    usage.ru_inblock - oldusage.ru_inblock,
		    usage.ru_oublock - oldusage.ru_oublock,
		    usage.ru_nvcsw -    oldusage.ru_nvcsw,
		    usage.ru_nivcsw -   oldusage.ru_nivcsw,
		    usage.ru_nsignals - oldusage.ru_nsignals,
		    usage.ru_maxrss );
		fflush(usestats.fp);
		oldusage = usage;
		use_stattime = current_time;
	}
#endif /* HAVE_GETRUSAGE */
}


/*
 * record_proto_stats - write system statistics to file
 *
 * file format
 * day (MJD)
 * time (s past midnight)
 * text message
 */
void
record_proto_stats(
	char	*str		/* text string */
	)
{
	l_fp	now;
	u_long	day;

	if (!stats_control)
		return;

	get_systime(&now);
	filegen_setup(&protostats, lfpuint(now));
	day = lfpuint(now) / 86400 + MJD_1900;
	setlfpuint(now, lfpuint(now) % 86400);
	if (protostats.fp != NULL) {
		fprintf(protostats.fp, "%lu %s %s\n", day,
		    ulfptoa(now, 3), str);
		fflush(protostats.fp);
	}
}

#ifdef ENABLE_DEBUG_TIMING
/*
 * record_timing_stats - write timing statistics to file
 *
 * file format:
 * day (mjd)
 * time (s past midnight)
 * text message
 */
void
record_timing_stats(
	const char *text	/* text message */
	)
{
	static unsigned int flshcnt;
	l_fp	now;
	u_long	day;

	if (!stats_control)
		return;

	get_systime(&now);
	filegen_setup(&timingstats, lfpuint(now));
	day = lfpuint(now) / 86400 + MJD_1900;
	setlfpuint(now, lfpuint(now) % 86400);
	if (timingstats.fp != NULL) {
		fprintf(timingstats.fp, "%lu %s %s\n", day, lfptoa(now,
		    3), text);
		if (++flshcnt % 100 == 0)
			fflush(timingstats.fp);
	}
}
#endif


/*
 * check_leap_file - See if the leapseconds file has been updated.
 *
 * Returns: n/a
 *
 * Note: This loads a new leapfile on the fly. Currently a leap file
 * without SHA1 signature is accepted, but if there is a signature line,
 * the signature must be valid or the file is rejected.
 */
void
check_leap_file(
	int           is_daily_check,
	uint32_t      ntptime       ,
	const time_t *systime
	)
{
	/* just do nothing if there is no leap file */
	if ( ! (leapfile_name && *leapfile_name))
		return;
	
	/* try to load leapfile, force it if no leapfile loaded yet */
	if (leapsec_load_file(
		    leapfile_name, &leapfile_stat,
		    !have_leapfile, is_daily_check))
		have_leapfile = true;
	else if (!have_leapfile)
		return;

	check_leap_expiration(is_daily_check, ntptime, systime);
}

/*
 * check expiration of a loaded leap table
 */
static void
check_leap_expiration(
	int           is_daily_check,
	uint32_t      ntptime       ,
	const time_t *systime
	)
{
	static const char * const logPrefix = "leapsecond file";
	int  rc;

	/* test the expiration of the leap data and log with proper
	 * level and frequency (once/hour or once/day, depending on the
	 * state.
	 */
	rc = leapsec_daystolive(ntptime, systime);	
	if (rc == 0) {
		msyslog(LOG_WARNING,
			"%s ('%s'): will expire in less than one day",
			logPrefix, leapfile_name);
	} else if (is_daily_check && rc < 28) {
		if (rc < 0)
			msyslog(LOG_ERR,
				"%s ('%s'): expired less than %d day%s ago",
				logPrefix, leapfile_name, -rc, (rc == -1 ? "" : "s"));
		else
			msyslog(LOG_WARNING,
				"%s ('%s'): will expire in less than %d days",
				logPrefix, leapfile_name, 1+rc);
	}
}


/*
 * getauthkeys - read the authentication keys from the specified file
 */
void
getauthkeys(
	const char *keyfile
	)
{
	int len;

	len = strlen(keyfile);
	if (!len)
		return;
	
	key_file_name = erealloc(key_file_name, len + 1);
	memcpy(key_file_name, keyfile, len + 1);

	authreadkeys(key_file_name);
}


/*
 * rereadkeys - read the authentication key file over again.
 */
void
rereadkeys(void)
{
	if (NULL != key_file_name)
		authreadkeys(key_file_name);
}


#ifdef __UNUSED__
/*
 * ntp_exit - document explicitly that ntpd has exited
 */
void
ntp_exit(int retval)
{
	msyslog(LOG_ERR, "EXITING with return code %d", retval);
	exit(retval);
}
#endif

/*
 * fstostr - prettyprint NTP seconds
 */
char * fstostr(
	time_t	ntp_stamp
	)
{
	char *		buf;
	struct calendar tm;

	LIB_GETBUF(buf);
	if (ntpcal_ntp_to_date(&tm, (uint32_t)ntp_stamp, NULL) < 0)
		snprintf(buf, LIB_BUFLENGTH, "ntpcal_ntp_to_date: %ld: range error",
			 (long)ntp_stamp);
	else
		snprintf(buf, LIB_BUFLENGTH, "%04d%02d%02d%02d%02d",
			 tm.year, tm.month, tm.monthday,
			 tm.hour, tm.minute);
	return buf;
}


/*
 * ntpd_time_stepped is called back by step_systime(), allowing ntpd
 * to do any one-time processing necessitated by the step.
 */
void
ntpd_time_stepped(void)
{
	u_int saved_mon_enabled;

	/*
	 * flush the monitor MRU list which contains l_fp timestamps
	 * which should not be compared across the step.
	 */
	if (MON_OFF != mon_enabled) {
		saved_mon_enabled = mon_enabled;
		mon_stop(MON_OFF);
		mon_start(saved_mon_enabled);
	}
}
