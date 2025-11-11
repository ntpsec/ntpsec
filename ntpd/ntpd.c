/*
 * ntpd.c - main program for the fixed point NTP daemon
 */

#include "config.h"
#include "ntp_stdlib.h"
#include <stdio.h>
#include <stdbool.h>
#include <sched.h>
#include <pthread.h>  /* For pthread_setschedparam() */
#include <sys/mman.h>

#ifdef SIGDANGER
# include <ulimit.h>
#endif /* SIGDANGER */


static bool need_priority = false;
static bool config_priority_override = false;
static int config_priority;

static bool dumpopts;

static char *user;		/* User to switch to */
static char *group;		/* group to switch to */
static bool droproot = false;
static const char *chrootdir;	/* directory to chroot to */

const char *progname;



static void	set_process_priority	(void);


#define ALL_OPTIONS "i:Nu:V"
static const struct option longoptions[] = {
    { "jaildir",	    1, 0, 'i' },
    { "nice",		    0, 0, 'N' },
    { "priority",	    1, 0, 'P' },
    { "user",		    1, 0, 'u' },
    { "version",	    0, 0, 'V' },
    { NULL,             0, 0, '\0'},
};


/*
 * Set the process priority
 */
static void
set_process_priority(void)
{
# ifdef DEBUG
	if (debug > 1) /* SPECIAL DEBUG */
		msyslog(LOG_DEBUG, "INIT: set_process_priority: %s",
			((!need_priority)
			 ? "Leave priority alone"
			 : "Attempt to set priority"
				));
# endif /* DEBUG */
	if (need_priority) {
		int pmax, pmin;
		struct sched_param sched;

		pmax = sched_get_priority_max(SCHED_FIFO);
		sched.sched_priority = pmax;
		if ( config_priority_override ) {
			pmin = sched_get_priority_min(SCHED_FIFO);
			if ( config_priority > pmax )
				sched.sched_priority = pmax;
			else if ( config_priority < pmin )
				sched.sched_priority = pmin;
			else
				sched.sched_priority = config_priority;
		}
		if ( pthread_setschedparam(pthread_self(), SCHED_FIFO, &sched) != 0 )
			msyslog(LOG_ERR, "INIT: pthread_setschedparam(): %s", strerror(errno));
		else
			need_priority = false;
	}
	if (need_priority)
		msyslog(LOG_ERR, "INIT: set_process_priority: No way found to improve our priority");
}

const char *ntpd_version_string = "ntpd ntpsec-" NTPSEC_VERSION_EXTENDED;
const char *ntpd_version(void)
{
    return ntpd_version_string;
}


/*
 * Main program.  Initialize us, disconnect us from the tty if necessary,
 * and loop waiting for I/O and/or timer expiries.
 */
int
main(
	int argc,
	char *argv[]
	)
{
	uid_t		uid;
	int op;
#ifdef SIGDANGER
	struct sigaction sa;
#endif

	progname = argv[0];

	uid = getuid();
	if (uid && !dumpopts) {
		termlogit = true;
		msyslog(LOG_ERR,
			"INIT: must be run as root, not uid %ld", (long)uid);
		exit(1);
	}
	/*
	 * Some option settings have to be deferred until after
	 * the library initialization sequence.
	 */
	ntp_optind = 1;
	while ((op = ntp_getopt_long(argc, argv, ALL_OPTIONS,
				     longoptions, NULL)) != -1) {
	    switch (op) {   
			case 'i':
			#ifdef ENABLE_DROPROOT
					droproot = true;
					if (ntp_optarg != NULL) {
						chrootdir = ntp_optarg;
					}
		    #endif
			break;
			case 'u':
		    #ifdef ENABLE_DROPROOT
			if (ntp_optarg != NULL) {
							char *gp;

				droproot = true;
							if ( user ) {
					fputs("ERROR: more than one -u given.\n",
					stderr);
					//ntpd_usage();
					exit(1);
							}
							user = estrdup(ntp_optarg);

				gp = strrchr(user, ':');
				if (gp) {
					*gp++ = '\0'; /* get rid of the ':' */
					group = estrdup(gp);
				} else {
									group  = NULL;
							}
			}
			#endif
			break;
			case 'N':
			need_priority = true;
			break;
			case 'P':
			if (ntp_optarg != NULL) {
				config_priority = atoi(ntp_optarg);
				config_priority_override = true;
				need_priority = true;
			}
			break;
			case 'V':
			printf("%s\n", ntpd_version());
			exit(0);
			default:
			break; 
	    }
	}

	set_process_priority();

	/*
	 * ntpd's working set is never going to be large relative to memory
	 * availability on modern machines. Do what chrony does and indulge it;
	 * we get some latency improvement that way.
	 * Need to do this before droproot.
	 */
	{
#ifdef RLIMIT_MEMLOCK
	    /* RLIMIT_MEMLOCK is Linux/BSD, not POSIX */
	    struct rlimit rlim;
	    rlim.rlim_max = rlim.rlim_cur = RLIM_INFINITY;
	    if (setrlimit(RLIMIT_MEMLOCK, &rlim) < 0)
		msyslog(LOG_DEBUG, "INIT: setrlimit(RLIMIT_CORE, [%ju, %ju]) failed",
			(uintmax_t)rlim.rlim_cur, (uintmax_t)rlim.rlim_max);
	    else
#endif

	    if (mlockall(MCL_CURRENT|MCL_FUTURE) < 0)
		msyslog(LOG_WARNING, "INIT: mlockall() failed: not locking into RAM");
	    else
		msyslog(LOG_INFO, "INIT: successfully locked into RAM");
	}

#ifdef ENABLE_EARLY_DROPROOT
	/* drop root privileges */
	/* This doesn't work on NetBSD or with SHM */
	if (sandbox(droproot, user, group, chrootdir, interface_interval!=0)) {
		interface_interval = 0;
		/*
		 * for now assume that the privilege to bind to privileged ports
		 * is associated with running with uid 0 - should be refined on
		 * ports that allow binding to NTP_PORT with uid != 0
		 */
		io_data.disable_dynamic_updates = true;
		msyslog(LOG_INFO, "INIT: running as non-root disables dynamic interface tracking");
	}
#endif
    
     // Execute the ntpd command with the provided arguments
	 msyslog(LOG_INFO, "INIT: Execute main ntpd");

	 execvp("ntpd_nonroot", argv);

	 // If execvp returns, there was an error
	 perror("execvp failed");


}



