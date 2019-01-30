/*
 * ntp_util.c - stuff I didn't have any other place for
 */
#include "config.h"

#include "lib_strbuf.h"
#include "ntp_assert.h"
#include "ntp_calendar.h"
#include "ntp_config.h"
#include "ntp_filegen.h"
#include "ntp_leapsec.h"
#include "ntp_stdlib.h"
#include "ntp_stdlib.h"
#include "ntp_auth.h"
#include "ntpd.h"
#include "timespecops.h"

#include <stdio.h>
#include <libgen.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <inttypes.h>

#include <sys/time.h>
#include <sys/resource.h>

/*
 * Defines used by file logging
 */
#define MJD_1970        40587           /* MJD for 1 Jan 1970 */

/*
 * This contains odds and ends, including the hourly stats, various
 * configuration items, leapseconds stuff, etc.
 */
/*
 * File names
 */
static	char *key_file_name;		/* keys file name */
static char *leapfile_name;		/* leapseconds file name */
static struct stat leapfile_stat;	/* leapseconds file stat() buffer */
static bool have_leapfile = false;
char *stats_drift_file;			/* frequency file name */
static double wander_resid;		/* last frequency update */
double	wander_threshold = 1e-7;	/* initial frequency threshold */
static char *timespec_to_MJDtime(const struct timespec *);

/*
 * Statistics file stuff
 */
#ifndef NTP_VAR
# define NTP_VAR "/var/NTP/"		/* NOTE the trailing '/' */
#endif


char statsdir[MAXFILENAME] = NTP_VAR;
static FILEGEN clockstats;
static FILEGEN loopstats;
static FILEGEN peerstats;
static FILEGEN protostats;
static FILEGEN rawstats;
static FILEGEN sysstats;
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
static  void	check_leap_expiration(bool, time_t);

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
	filegen_unregister("loopstats");
	filegen_unregister("rawstats");
	filegen_unregister("sysstats");
	filegen_unregister("peerstats");
	filegen_unregister("protostats");
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
	filegen_register(statsdir, "loopstats",	  &loopstats);
	filegen_register(statsdir, "rawstats",	  &rawstats);
	filegen_register(statsdir, "sysstats",	  &sysstats);
	filegen_register(statsdir, "peerstats",	  &peerstats);
	filegen_register(statsdir, "protostats",  &protostats);
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
 * drift_write - write drift to file, speeds up restart
 * drift can be off significantly if the system has cooled off
 *
 * This used to use mkstemp, but that uses mode 600, user ntp
 * apparmor keeps root from reading that during initialization
 * See Issue #409
 * We don't need a unique filename.  No other job is writing driftfile.
 * There is no security problem with reading the drift file since
 * you can get the data via ntp_adjtime(2) or ntptime(8).
 */
static void drift_write(char *driftfile, double drift)
{
	FILE *new;
	char tempfile[PATH_MAX];
	strlcpy(tempfile, driftfile, sizeof(tempfile));
	strlcat(tempfile, "-tmp", sizeof(tempfile));
	if ((new = fopen(tempfile, "w")) == NULL) {
	    msyslog(LOG_ERR, "LOG: frequency file %s: %m", tempfile);
	    return;
	}
	fprintf(new, "%.6f\n", drift);
	(void)fclose(new);
	/* atomic */
	if (rename(tempfile, driftfile))
	    msyslog(LOG_WARNING,
		    "LOG: Unable to rename temp drift file %s to %s, %m",
		    tempfile, driftfile);
}


/*
 * write_stats - hourly: sysstats, usestats, and maybe drift
 */
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
	        DPRINT(1, ("write_stats: frequency %.6lf thresh %.6lf, freq %.6lf\n",
			   (prev_drift_comp - drift_comp) * US_PER_S,
			   wander_resid * US_PER_S, drift_comp * US_PER_S));
		if (fabs(prev_drift_comp - drift_comp) < wander_resid) {
			wander_resid *= 0.5;
			return;
		}
		prev_drift_comp = drift_comp;
		wander_resid = wander_threshold;
		drift_write(stats_drift_file, drift_comp * US_PER_S);
	}
}


static bool drift_read(const char *drift_file, double *drift)
{
	FILE *fp;
	if ((fp = fopen(drift_file, "r")) == NULL) {
	    return false;
	}
	if (fscanf(fp, "%lf", drift) != 1) {
	    msyslog(LOG_ERR,
		    "LOG: format error frequency file %s",
		    drift_file);
	    fclose(fp);
	    return false;
	}
	fclose(fp);
	return true;
}

/*
 * stats_config - configure the stats operation
 */
void
stats_config(
	int item,
	const char *invalue	/* only one type so far */
	)
{
	FILE	*fp;
	const char *value;
	size_t	len;
	double	new_drift = 0;
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
		memcpy(stats_drift_file, value, len+1);

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
			    "LOG: statsdir too long (>%d, sigh)",
			    (int)sizeof(statsdir) - 2);
		} else {
			bool add_dir_sep;
			size_t value_l;

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
			msyslog(LOG_ERR, "LOG: pid file %s: %m",
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

			time(&ttnow);
			leapsec_getsig(&lsig);
			mprintf_event(EVNT_TAI, NULL,
				"%d leap %s %s %s",
				lsig.taiof,
				rfc3339time(lsig.ttime),
				leapsec_expired(ttnow)
					? "expired"
					: "expires",
				rfc3339time(lsig.etime));
			have_leapfile = true;

			/* force an immediate daily expiration check of
			 * the leap seconds table
			 */
			check_leap_expiration(true, ttnow);
		}
		break;

	default:
		/* oh well */
		break;
	}
}

/* timespec_to_MJDtime
 */

static char *
timespec_to_MJDtime(const struct timespec *ts)
{
	char *buf;
	unsigned long	day, sec, msec;

	buf = lib_getbuf();

	day = (unsigned long)ts->tv_sec / SECSPERDAY + MJD_1970;
	sec = (unsigned long)ts->tv_sec % SECSPERDAY;
	msec = (unsigned long)ts->tv_nsec / NS_PER_MS;  /* nano secs to milli sec */
	snprintf(buf, LIB_BUFLENGTH, "%lu %lu.%03lu", day, sec, msec);

	return buf;
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
	struct timespec now;

	if (!stats_control)
		return;

	clock_gettime(CLOCK_REALTIME, &now);
	filegen_setup(&peerstats, now.tv_sec);
	if (peerstats.fp != NULL) {
		fprintf(peerstats.fp,
		    "%s %s %x %.9f %.9f %.9f %.9f\n",
		    timespec_to_MJDtime(&now),
		    peerlabel(peer), (unsigned int)status, peer->offset,
		    peer->delay, peer->disp, peer->jitter);
		fflush(peerstats.fp);
	}
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
	struct timespec	now;

	if (!stats_control)
		return;

	clock_gettime(CLOCK_REALTIME, &now);
	filegen_setup(&loopstats, now.tv_sec);
	if (loopstats.fp != NULL) {
		fprintf(loopstats.fp, "%s %.9f %.6f %.9f %.6f %d\n",
		    timespec_to_MJDtime(&now),
		    offset, freq * US_PER_S, jitter,
		    wander * US_PER_S, spoll);
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
	struct timespec	now;

	if (!stats_control)
		return;

	clock_gettime(CLOCK_REALTIME, &now);
	filegen_setup(&clockstats, now.tv_sec);
	if (clockstats.fp != NULL) {
		fprintf(clockstats.fp, "%s %s %s\n",
		    timespec_to_MJDtime(&now), peerlabel(peer), text);
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
 * source IP address (old format) or drivername(unit) (new format)
 * destination peer address
 * t1 t2 t3 t4 timestamps
 * various other local statistics
 */
void
record_raw_stats(
	struct peer *peer,
	int	leap,
	int	version,
	int	mode,
	int	stratum,
	int	ppoll,
	int	precision,
	double	root_delay,	/* seconds */
	double	root_dispersion,/* seconds */
	uint32_t	refid,
	unsigned int	outcount
	)
{
	struct timespec	now;
	const sockaddr_u *dstaddr = peer->dstadr ? &peer->dstadr->sin : NULL;
	l_fp	t1 = peer->org_ts;	/* originate timestamp */
	l_fp	t2 = peer->rec;		/* receive timestamp */
	l_fp	t3 = peer->xmt;		/* transmit timestamp */
	l_fp	t4 = peer->dst;		/* destination timestamp */

	if (!stats_control)
		return;

	clock_gettime(CLOCK_REALTIME, &now);
	filegen_setup(&rawstats, now.tv_sec);
	if (rawstats.fp != NULL) {
		fprintf(rawstats.fp, "%s %s %s %s %s %s %s %d %d %d %d %d %d %.6f %.6f %s %u\n",
		    timespec_to_MJDtime(&now),
		    peerlabel(peer), dstaddr ?  socktoa(dstaddr) : "-",
		    ulfptoa(t1, 9), ulfptoa(t2, 9),
		    ulfptoa(t3, 9), ulfptoa(t4, 9),
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
	struct timespec	now;

	if (!stats_control)
		return;

	clock_gettime(CLOCK_REALTIME, &now);
	filegen_setup(&sysstats, now.tv_sec);
	if (sysstats.fp != NULL) {
		fprintf(sysstats.fp,
		    "%s %u %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64
		    " %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 "\n",
		    timespec_to_MJDtime(&now), current_time - stat_count.sys_stattime,
			stat_count.sys_received, stat_count.sys_processed,
			stat_count.sys_newversion, stat_count.sys_oldversion,
			stat_count.sys_restricted, stat_count.sys_badlength,
		    stat_count.sys_badauth, stat_count.sys_declined,
			stat_count.sys_limitrejected, stat_count.sys_kodsent);
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
	struct timespec	now;
	struct rusage usage;
	static struct rusage oldusage;
	/* Descriptions in NetBSD and FreeBSD are better than Linux
	 * man getrusage */

	if (!stats_control)
		return;

	clock_gettime(CLOCK_REALTIME, &now);
	filegen_setup(&usestats, now.tv_sec);
	if (usestats.fp != NULL) {
		double utime, stimex;
		getrusage(RUSAGE_SELF, &usage);
		utime =  usage.ru_utime.tv_usec - oldusage.ru_utime.tv_usec;
		utime /= 1E6;
		utime += usage.ru_utime.tv_sec -  oldusage.ru_utime.tv_sec;
		stimex =  usage.ru_stime.tv_usec - oldusage.ru_stime.tv_usec;
		stimex /= 1E6;
		stimex += usage.ru_stime.tv_sec -  oldusage.ru_stime.tv_sec;
		fprintf(usestats.fp,
		    "%s %u %.3f %.3f %ld %ld %ld %ld %ld %ld %ld %ld %ld\n",
		    timespec_to_MJDtime(&now), current_time - stat_count.use_stattime,
		    utime, stimex,
		    usage.ru_minflt -   oldusage.ru_minflt,
		    usage.ru_majflt -   oldusage.ru_majflt,
		    usage.ru_nswap -    oldusage.ru_nswap,
		    usage.ru_inblock -  oldusage.ru_inblock,
		    usage.ru_oublock -  oldusage.ru_oublock,
		    usage.ru_nvcsw -    oldusage.ru_nvcsw,
		    usage.ru_nivcsw -   oldusage.ru_nivcsw,
		    usage.ru_nsignals - oldusage.ru_nsignals,
		    usage.ru_maxrss );
		fflush(usestats.fp);
		oldusage = usage;
		stat_count.use_stattime = current_time;
	}
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
	struct timespec	now;

	if (!stats_control)
		return;

	clock_gettime(CLOCK_REALTIME, &now);
	filegen_setup(&protostats, now.tv_sec);
	if (protostats.fp != NULL) {
		fprintf(protostats.fp, "%s %s\n",
		    timespec_to_MJDtime(&now), str);
		fflush(protostats.fp);
	}
}



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
	bool   is_daily_check,
	time_t systime
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

	check_leap_expiration(is_daily_check, systime);
}

/*
 * check expiration of a loaded leap table
 */
static void
check_leap_expiration(
	bool   is_daily_check,
	time_t systime
	)
{
	static const char * const logPrefix = "leapsecond file";
	int  rc;

	/* test the expiration of the leap data and log with proper
	 * level and frequency (once/hour or once/day, depending on the
	 * state.
	 */
	rc = leapsec_daystolive(systime);
	if (rc == 0) {
		msyslog(LOG_WARNING,
			"CLOCK: %s ('%s'): will expire in less than one day",
			logPrefix, leapfile_name);
	} else if (is_daily_check && rc < 28) {
		if (rc < 0)
			msyslog(LOG_ERR,
				"CLOCK: %s ('%s'): expired less than %d day%s ago",
				logPrefix, leapfile_name, -rc, (rc == -1 ? "" : "s"));
		else
			msyslog(LOG_WARNING,
				"CLOCK: %s ('%s'): will expire in less than %d days",
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
	size_t len;

	len = strlen(keyfile);
	if (!len)
		return;

	key_file_name = erealloc(key_file_name, len + 1);
	memcpy(key_file_name, keyfile, len + 1);

	authreadkeys(key_file_name);
}

/*
 * ntpd_time_stepped is called back by step_systime(), allowing ntpd
 * to do any one-time processing necessitated by the step.
 */
void
ntpd_time_stepped(void)
{
	unsigned int saved_mon_enabled;

	/*
	 * flush the monitor MRU list which contains l_fp timestamps
	 * which should not be compared across the step.
	 */
	if (MON_OFF != mon_data.mon_enabled) {
		saved_mon_enabled = mon_data.mon_enabled;
		mon_stop(MON_OFF);
		mon_start((int)saved_mon_enabled);
	}
}
