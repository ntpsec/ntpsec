/*
 * ntpd.c - main program for the fixed point NTP daemon
 */

#include "config.h"

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_stdlib.h"

#include "ntp_config.h"
#include "ntp_syslog.h"
#include "ntp_assert.h"
#include "ntp_auth.h"
#ifdef ENABLE_DNS_LOOKUP
#include "ntp_dns.h"
#endif
#include "isc_error.h"

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif /* HAVE_SYS_IOCTL_H */
#include <sched.h>
#include <sys/mman.h>

#include <termios.h>

#include "recvbuff.h"

void catchQuit (int sig);
static volatile int signo = 0;
/* In an ideal world, 'finish_safe()' would declared as noreturn... */
static	void	finish_safe	(int)
			__attribute__	((__noreturn__));

#ifdef SIGDANGER
# include <ulimit.h>
#endif /* SIGDANGER */

#if defined(HAVE_DNS_SD_H)
# include <dns_sd.h>
static DNSServiceRef mdns;
#endif

static void check_minsane(void);

static bool need_priority = false;
static bool config_priority_override = false;
static int config_priority;

bool listen_to_virtual_ips = true;

static char *logfilename;
static bool opt_ipv4, opt_ipv6;
static const char *explicit_config;
static bool explicit_interface;
static bool nofork = false;		/* Fork by default */
static bool dumpopts;
static long wait_sync = -1;
static const char *driftfile, *pidfile;

#if defined(HAVE_DNS_SD_H)
/*
 * mDNS registration flag. If set, we attempt to register with the
 * mDNS system, but only after we have synched the first time. If the
 * attempt fails, then try again once per minute for up to 5
 * times. After all, we may be starting before mDNS.
 */
static bool mdnsreg = false;
int mdnstries = 5;
#endif  /* HAVE_DNS_SD_H */

static bool droproot = false;
static char *user;		/* User to switch to */
static char *group;		/* group to switch to */
static const char *chrootdir;	/* directory to chroot to */

#ifdef HAVE_WORKING_FORK
int	waitsync_fd_to_close = -1;	/* -w/--wait-sync */
#endif

const char *progname;

#if defined(HAVE_WORKING_FORK)
static int	wait_child_sync_if	(int, long);
#endif

static	void	catchHUP	(int);
#ifdef ENABLE_DNS_LOOKUP
static	void	catchDNS	(int);
#endif

# ifdef	DEBUG
static	void	moredebug	(int);
static	void	lessdebug	(int);
# else	/* !DEBUG follows */
static	void	no_debug	(int);
# endif	/* !DEBUG */

static int	saved_argc;
static char **	saved_argv;

static int	ntpdmain(int, char **) __attribute__((noreturn));
static void	mainloop		(void)
			__attribute__	((__noreturn__));
static void	set_process_priority	(void);
static  void    close_all_beyond(int);
static  void    close_all_except(int);


#define ALL_OPTIONS "46abc:dD:f:gGhi:I:k:l:LmnNp:Pqr:Rs:t:u:U:Vw:xzZ"
static const struct option longoptions[] = {
    { "ipv4",		    0, 0, '4' },
    { "ipv6",		    0, 0, '6' },
    { "assert",	            0, 0, 'a' },
    { "configfile",	    1, 0, 'c' },
    { "debug",		    0, 0, 'd' },
    { "set-debug-level",    1, 0, 'D' },
    { "driftile",	    1, 0, 'f' },
    { "panicgate",	    0, 0, 'g' },
    { "help",	            0, 0, 'h' },
    { "jaildir",	    1, 0, 'i' },
    { "interface",	    1, 0, 'I' },
    { "keyfile",	    1, 0, 'k' },
    { "logfile",	    1, 0, 'l' },
    { "mdns",	            0, 0, 'm' },
    { "novirtualips",	    0, 0, 'L' },
    { "nofork",		    0, 0, 'n' },
    { "nice",		    0, 0, 'N' },
    { "pidfile",	    1, 0, 'p' },
    { "priority",	    1, 0, 'P' },
    { "quit",		    0, 0, 'q' },
    { "dumpopts",	    0, 0, 'R' },
    { "statsdir",	    1, 0, 's' },
    { "trustedkey",	    1, 0, 't' },
    { "user",		    1, 0, 'u' },
    { "updateinterval",	    1, 0, 'U' },
    { "var",		    1, 0, 'z' },
    { "dvar",		    1, 0, 'Z' },
    { "slew",		    0, 0, 'x' },
    { "version",	    0, 0, 'V' },
    { NULL,                 0, 0, '\0'},
};

static void ntpd_usage(void)
{
#define P(x)	fputs(x, stderr)
    P("USAGE:  ntpd [ -<flag> [<val>] | --<name>[{=| }<val>] ]...\n");
    P("  Flg Arg Option-Name    Description\n");
    P("   -4 no  ipv4           Force IPv4 DNS name resolution\n");
    P("				- prohibits the option 'ipv6'\n");
    P("   -6 no  ipv6           Force IPv6 DNS name resolution\n");
    P("				- prohibits the option 'ipv4'\n");
    P("   -a no  assert         REQUIRE(false) to test assert handler\n");
    P("   -c Str configfile     configuration file name\n");
    P("   -d no  debug-level    Increase output debug message level\n");
    P("				- may appear multiple times\n");
    P("   -D Str set-debug-level Set the output debug message level\n");
    P("				- may appear multiple times\n");
    P("   -f Str driftfile      frequency drift file name\n");
    P("   -g no  panicgate      Allow the first adjustment to be Big\n");
    P("				- may appear multiple times\n");
    P("   -h no  --help         Display usage summary of options and exit.\n");
    P("   -i Str jaildir        Jail directory\n");
    P("   -I Str interface      Listen on an interface name or address\n");
    P("				- may appear multiple times\n");
    P("   -k Str keyfile        path to symmetric keys\n");
    P("   -l Str logfile        path to the log file\n");
    P("   -L no  novirtualips   Do not listen to virtual interfaces\n");
    P("   -m no                 Enable mDNS registration\n");
    P("   -n no  nofork         Do not fork\n");
    P("   -N no  nice           Run at high priority\n");
    P("   -p Str pidfile        path to the PID file\n");
    P("   -P Num priority       Process priority\n");
    P("   -q no  quit           Set the time and quit\n");
    P("   -r Str propagationdelay Broadcast/propagation delay\n");
    P("   -s Str statsdir       Statistics file location\n");
    P("   -t Str trustedkey     Trusted key number\n");
    P("				- may appear multiple times\n");
    P("   -u Str user           Run as userid (or userid:groupid)\n");
    P("   -U Num uinterval      interval in secs between scans for new or dropped interfaces\n");
    P("      Str var            make ARG an ntp variable (RW)\n");
    P("				- may appear multiple times\n");
    P("      Str dvar           make ARG an ntp variable (RW|DEF)\n");
    P("				- may appear multiple times\n");
    P("   -x no  slew           Slew up to 600 seconds\n");
    P("   -V no  version        Output version information and exit\n");
    P("   -h no  help           Display extended usage information and exit\n");
#ifdef REFCLOCK
    P("This version was compiled with the following clock drivers:\n");
    int ct = 0;
    for (int dtype = 1; dtype < (int)num_refclock_conf; dtype++)
	if (refclock_conf[dtype]->basename != NULL)
	{
	    fprintf(stderr, "%12s", refclock_conf[dtype]->basename);
	    if (((++ct % 5) == 0))
		fputc('\n', stderr);
	}
    if (ct % 5)
	fputc('\n', stderr);
#endif /* REFCLOCK */
#undef P
}

static void
parse_cmdline_opts(
	int argc,
	char **argv
	)
{
	static bool	parsed = false;

	if (parsed)
	    return;

	int op;

	while ((op = ntp_getopt_long(argc, argv,
				     ALL_OPTIONS, longoptions, NULL)) != -1) {

	    switch (op) {
	    case '4':
		opt_ipv4 = true;
		break;
	    case '6':
		opt_ipv6 = true;
		break;
	    case 'a':
		fputs("Testing assert failure.\n", stderr);
		REQUIRE(false);
		break;
	    case 'b':
		fputs("ERROR: Obsolete and unsupported broadcast option -b\n",
                      stderr);
		ntpd_usage();
		exit(1);
		break;
	    case 'c':
		if (ntp_optarg != NULL)
			explicit_config = ntp_optarg;
		break;
	    case 'd':
#ifdef DEBUG
		++debug;
#endif
		nofork = true;
		break;
	    case 'D':
#ifdef DEBUG
		if (ntp_optarg != NULL)
			debug = atoi(ntp_optarg);
#endif
		break;
	    case 'f':
		if (ntp_optarg != NULL)
			driftfile = ntp_optarg;
		break;
	    case 'g':
		clock_ctl.allow_panic = true;
		break;
	    case 'G':
		clock_ctl.force_step_once = true;
		break;
	    case 'h':
		ntpd_usage();
		exit(0);
	    case 'i':
#ifdef ENABLE_DROPROOT
		droproot = true;
		if (ntp_optarg != NULL)
			chrootdir = ntp_optarg;
#endif
		break;
	    case 'I':
		explicit_interface = true;
		/* processing deferred */
		break;
	    case 'k':
		/* defer */
		break;
	    case 'l':
		if (ntp_optarg != NULL)
			logfilename = ntp_optarg;
		break;
	    case 'L':
		listen_to_virtual_ips = false;
		break;
	    case 'm':
#if defined(HAVE_DNS_SD_H)
		mdnsreg = true;
#endif  /* HAVE_DNS_SD_H */
		break;
	    case 'M':
		/* defer */
		break;
	    case 'n':
		nofork = true;
		break;
	    case 'N':
		need_priority = true;
		break;
	    case 'p':
		if (ntp_optarg != NULL)
			pidfile = ntp_optarg;
		break;
	    case 'P':
		if (ntp_optarg != NULL) {
			config_priority = atoi(ntp_optarg);
			config_priority_override = true;
			need_priority = true;
		}
		break;
	    case 'q':
		clock_ctl.mode_ntpdate = true;
		nofork = true;
		break;
	    case 'r':
		fputs("ERROR: Obsolete and unsupported broadcast option -r\n",
                      stderr);
		ntpd_usage();
		exit(1);
	        break;
	    case 'R':	/* undocumented -- dump CLI options for testing */
		dumpopts = true;
		nofork = true;
		break;
	    case 's':
		if (ntp_optarg != NULL)
			strlcpy(statsdir, ntp_optarg, sizeof(statsdir));
		break;
	    case 't':
		/* defer */
		break;
	    case 'u':
#ifdef ENABLE_DROPROOT
		if (ntp_optarg != NULL) {
                        char *gp;

			droproot = true;
                        if ( user ) {
			    fputs("ERROR: more than one -u given.\n",
				  stderr);
			    ntpd_usage();
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
	    case 'U':
		if (ntp_optarg != NULL)
		{
			long val = atol(ntp_optarg);

			if (val >= 0)
				interface_interval = (int)val;
			else {
				fprintf(stderr,
					"command line interface update interval %ld must not be negative\n",
					val);
				msyslog(LOG_ERR,
					"CONFIG: command line interface update interval %ld must not be negative",
					val);
				exit(0);
			}
		}
		break;
	    case 'V':
		printf("%s\n", ntpd_version());
		exit(0);
	    case 'w':
		if (ntp_optarg != NULL)
			wait_sync = (long)strtod(ntp_optarg, NULL);
		break;
	    case 'x':
		/* defer */
		break;
	    case 'z':
		/* defer */
		break;
	    case 'Z':
		/* defer */
		break;
	    default:
		fputs("Unknown command line switch or missing argument.\n",
                      stderr);
		ntpd_usage();
		exit(1);
	    } /*switch*/
	}

	/*
	 * Sanity checks and derived options
	 */

	/* save list of servers from cmd line for config_peers() use */
	if (ntp_optind < argc) {
		cmdline_server_count = argc - ntp_optind;
		cmdline_servers = argv + ntp_optind;
	}
}

#ifdef NO_MAIN_ALLOWED
CALL(ntpd,"ntpd",ntpdmain);
#else	/* !NO_MAIN_ALLOWED follows */
int
main(
	int argc,
	char *argv[]
	)
{
	return ntpdmain(argc, argv);
}
#endif /* !NO_MAIN_ALLOWED */

#ifdef SIGDANGER
/*
 * Signal handler for SIGDANGER. (AIX)
 */
static void
catch_danger(int signo)
{
	msyslog(LOG_INFO, "ERR: setpgid(): %m");
	/* Make the system believe we'll free something, but don't do it! */
	return;
}
#endif /* SIGDANGER */

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
#ifdef HAVE_SCHED_SETSCHEDULER
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
		if ( sched_setscheduler(0, SCHED_FIFO, &sched) == -1 )
			msyslog(LOG_ERR, "INIT: sched_setscheduler(): %m");
		else
			need_priority = false;
	}
#endif
	if (need_priority)
		msyslog(LOG_ERR, "INIT: set_process_priority: No way found to improve our priority");
}

const char *ntpd_version(void)
{
    static char versionbuf[64];
    snprintf(versionbuf, sizeof(versionbuf),
	     "ntpd ntpsec-%s", NTPSEC_VERSION_EXTENDED);
    return versionbuf;
}

/*
 * Main program.  Initialize us, disconnect us from the tty if necessary,
 * and loop waiting for I/O and/or timer expiries.
 */
static int
ntpdmain(
	int argc,
	char *argv[]
	)
{
	mode_t		uv;
	uid_t		uid;
# if defined(HAVE_WORKING_FORK)
	int		pipe_fds[2];
	int		rc;
	int		exit_code;
#  ifdef SIGDANGER
	struct sigaction sa;
#  endif
# endif	/* HAVE_WORKING_FORK*/
	int op;

	uv = umask(0);
	if (uv)
		umask(uv);
	else
		umask(022);
	saved_argc = argc;
	saved_argv = argv;
	progname = argv[0];
	parse_cmdline_opts(argc, argv);
# ifdef DEBUG
	setvbuf(stdout, NULL, _IOLBF, 0);
# endif

	init_logging(progname, NLOG_SYNCMASK, true);
	/* honor -l/--logfile option to log to a file */
	if (logfilename != NULL) {
		syslogit = false;
		termlogit = false;
		change_logfile(logfilename, false);
	} else {
		if (nofork)
		    termlogit = true;
		if (dumpopts)
			syslogit = false;
	}

	if (!dumpopts)
	{
		char buf[1024];	/* Secret knowledge of msyslog buf length */
		char *cp = buf;

		msyslog(LOG_NOTICE, "INIT: %s: Starting", ntpd_version());

		/* Note that every arg has an initial space character */
		snprintf(cp, sizeof(buf), "Command line:");
		cp += strlen(cp);

		for (int i = 0; i < saved_argc ; ++i) {
			snprintf(cp, sizeof(buf) - (size_t)(cp - buf),
				" %s", saved_argv[i]);
			cp += strlen(cp);
		}
		msyslog(LOG_INFO, "INIT: %s", buf);
	}

	uid = getuid();
	if (uid && !dumpopts) {
		termlogit = true;
		msyslog(LOG_ERR,
			"INIT: must be run as root, not uid %ld", (long)uid);
		exit(1);
	}

	set_prettydate_pivot(time(NULL));

# ifdef HAVE_WORKING_FORK
	/* make sure the FDs are initialised */
	pipe_fds[0] = -1;
	pipe_fds[1] = -1;

	if (wait_sync <= 0)
	    wait_sync = 0;
	else {
	    /* -w requires a fork() even with debug > 0 */
	    nofork = false;
	    if (pipe(pipe_fds)) {
		termlogit = true;
		exit_code = (errno) ? errno : -1;
		msyslog(LOG_ERR,
			"INIT: Pipe creation failed for --wait-sync: %m");
		exit(exit_code);
	    }
	    waitsync_fd_to_close = pipe_fds[1];
	}
# endif	/* HAVE_WORKING_FORK */

	init_network();
	/*
	 * Detach us from the terminal.  May need an #ifndef GIZMO.
	 */
	if (!nofork) {

# ifdef HAVE_WORKING_FORK
		rc = fork();
		if (-1 == rc) {
			exit_code = (errno) ? errno : -1;
			msyslog(LOG_ERR, "INIT: fork: %m");
			exit(exit_code);
		}
		if (rc > 0) {
			/* parent */
			exit_code = wait_child_sync_if(pipe_fds[0],
						       wait_sync);
			exit(exit_code);
		}

		/*
		 * child/daemon
		 * close all open files excepting waitsync_fd_to_close.
		 * msyslog() unreliable until after init_logging().
		 */
		termlogit = false;    /* do not use stderr after fork */
		closelog();
		close_all_except(waitsync_fd_to_close);
		INSIST(0 == open("/dev/null", 0) && 1 == dup2(0, 1) \
			&& 2 == dup2(0, 2));

		init_logging(progname, 0, true);
		/* we lost our logfile (if any) daemonizing */
		setup_logfile(logfilename);

		if (setsid() == (pid_t)-1)
			msyslog(LOG_ERR, "INIT: setsid(): %m");
#  ifdef SIGDANGER
		/* Don't get killed by low-on-memory signal. */
		sa.sa_handler = catch_danger;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		sigaction(SIGDANGER, &sa, NULL);
#  endif	/* SIGDANGER */
# endif		/* HAVE_WORKING_FORK */
	}

	/*
	 * Set up signals we pay attention to locally.
	 */
	signal_no_reset(SIGINT, catchQuit);
	signal_no_reset(SIGQUIT, catchQuit);
	signal_no_reset(SIGTERM, catchQuit);
	signal_no_reset(SIGHUP, catchHUP);
	signal_no_reset(SIGBUS, catchQuit);  /* FIXME: It's broken, can't continue. */
#ifdef ENABLE_DNS_LOOKUP
	signal_no_reset(SIGDNS, catchDNS);
#endif

# ifdef DEBUG
	signal_no_reset(MOREDEBUGSIG, moredebug);
	signal_no_reset(LESSDEBUGSIG, lessdebug);
# else
	signal_no_reset(MOREDEBUGSIG, no_debug);
	signal_no_reset(LESSDEBUGSIG, no_debug);
# endif	/* DEBUG */

	/*
	 * Set up signals we should never pay attention to.
	 */
	signal_no_reset(SIGPIPE, SIG_IGN);

	/*
	 * Call the init_ routines to initialize the data structures.
	 *
	 * Exactly what command-line options are we expecting here?
	 */
	ssl_init();
	auth_init();
	init_util();
	init_restrict();
	init_mon();
	init_control();
	init_peer();
# ifdef REFCLOCK
	init_refclock();
# endif
	set_process_priority();
	init_proto(!dumpopts);		/* Call at high priority */
	init_io();
	init_loopfilter();
	init_readconfig();	/* see readconfig() */
	mon_start(MON_ON);	/* monitor on by default now	  */
				/* turn off in config if unwanted */

	/*
	 * Some option settings have to be deferred until after
	 * the library initialization sequence.
	 */
	ntp_optind = 1;
	while ((op = ntp_getopt_long(argc, argv, ALL_OPTIONS,
				     longoptions, NULL)) != -1) {
	    switch (op) {
            case '4':
            case '6':
                /* handled elsewhere */
                break;
	    case 'b':
		fputs("ERROR: Obsolete and unsupported broadcast option -b\n",
                      stderr);
		ntpd_usage();
		exit(1);
		break;
            case 'c':
            case 'd':
            case 'D':
                /* handled elsewhere */
                break;
	    case 'f':
		stats_config(STATS_FREQ_FILE, driftfile);
		break;
            case 'g':
            case 'h':
            case 'i':
                /* handled elsewhere */
                break;
	    case 'I':
		if (ntp_optarg != NULL)
	        {
		    sockaddr_u	addr;
		    add_nic_rule(
			is_ip_address(ntp_optarg, AF_UNSPEC, &addr)
			? MATCH_IFADDR
			: MATCH_IFNAME,
			ntp_optarg, -1, ACTION_LISTEN);
	        }
		break;
	    case 'k':
		if (ntp_optarg != NULL)
			set_keys_file(ntp_optarg);
		break;
            case 'l':
            case 'L':
            case 'm':
            case 'n':
            case 'N':
                /* handled elsewhere */
                break;
	    case 'p':
		stats_config(STATS_PID_FILE, pidfile);
		break;
            case 'P':
            case 'q':
                /* handled elsewhere */
                break;
	    case 'r':
		fputs("ERROR: Obsolete and unsupported broadcast option -r\n",
                      stderr);
		ntpd_usage();
		exit(1);
		break;
            case 'R':
                /* handled elsewhere */
                break;
	    case 's':
		stats_config(STATS_STATSDIR, statsdir);
		break;
	    case 't':
		if (ntp_optarg != NULL)
		{
		    unsigned long tkey = (unsigned long)atol(ntp_optarg);
		    if (tkey == 0 || tkey > NTP_MAXKEY) {
			msyslog(LOG_ERR,
				"INIT: command line trusted key %s is invalid",
				ntp_optarg);
			exit(1);
		    } else {
			set_trustedkey((keyid_t)tkey);
		    }
	        }
		break;
	    case 'u':
	    case 'U':
	    case 'v':
	    case 'w':
                /* handled elsewhere */
                break;
	    case 'x':
		loop_config(LOOP_MAX, 600);
		break;
	    case 'z':
		if (ntp_optarg != NULL)
			set_sys_var(ntp_optarg, strlen(ntp_optarg) + 1, RW);
		break;
	    case 'Z':
		if (ntp_optarg != NULL)
			set_sys_var(ntp_optarg, strlen(ntp_optarg) + 1,
                                    (unsigned short) (RW | DEF));
                break;
            default:
		msyslog(LOG_ERR, "INIT: Unknown option: %c", (char)op);
		exit(1);
	    }
	}

	/*
	 * ntpd's working set is never going to be large relative to memory
	 * availability on modern machines. Do what chrony does and indulge it;
	 * we get some latency improvement that way.
	 * Need to do this before droproot.
	 */
	{
	    struct rlimit rlim;
	    rlim.rlim_max = rlim.rlim_cur = RLIM_INFINITY;
#ifdef RLIMIT_MEMLOCK
	    if (setrlimit(RLIMIT_MEMLOCK, &rlim) < 0)
		msyslog(LOG_WARNING, "INIT: setrlimit() failed: not locking into RAM");
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
		disable_dynamic_updates = true;
		msyslog(LOG_INFO, "INIT: running as non-root disables dynamic interface tracking");
	}
#endif

     	/* use this to test if option setting gives expected results */
	if (dumpopts) {
	    if (explicit_config)
		fprintf(stdout, "conffile \"%s\";\n", explicit_config);
#ifdef DEBUG
	    fprintf(stdout, "#debug = %d\n", debug);
#endif /* DEBUG */
	    if (driftfile)
		fprintf(stdout, "driftfile \"%s\";\n", driftfile);
	    fprintf(stdout, "#allow_panic = %s\n",
		    clock_ctl.allow_panic ? "true" : "false");
	    fprintf(stdout, "#force_step_once = %s\n",
		    clock_ctl.force_step_once ? "true" : "false");
#ifdef ENABLE_DROPROOT
	    if (chrootdir)
		fprintf(stdout, "#chrootdir = \"%s\";\n", chrootdir);
	    if (user)
		fprintf(stdout, "#user = %s\n", user);
	    if (group)
		fprintf(stdout, "#group = %s\n", group);
#endif
	    /* FIXME: dump interfaces */
	    /* FIXME: dump authkeys */
	    if (logfilename)
		fprintf(stdout, "logfile \"%s\";\n", logfilename);
	    fprintf(stdout, "#listen_to_virtual_ips = %s\n",
		    listen_to_virtual_ips ? "true" : "false");
#if defined(HAVE_DNS_SD_H)
	    fprintf(stdout, "#mdnsreg = %s\n",
		    mdnsreg ? "true" : "false");
#endif  /* HAVE_DNS_SD_H */
	    if (pidfile)
		fprintf(stdout, "pidfile \"%s\";\n", pidfile);
	    /* FIXME: dump priority */
	    fprintf(stdout, "#mode_ntpdate = %s\n",
		    clock_ctl.mode_ntpdate ? "true" : "false");
	    if (statsdir[0])
		fprintf(stdout, "statsdir \"%s\";\n", statsdir);
	    fprintf(stdout, "#interface_interval = %d\n", interface_interval);
	    /* FIXME: dump variable settings */
	    exit(0);
	}

	if (ipv4_works && ipv6_works) {
		if (opt_ipv4)
			ipv6_works = false;
		else if (opt_ipv6)
			ipv4_works = false;
	} else if (!ipv4_works && !ipv6_works) {
		msyslog(LOG_ERR, "INIT: Neither IPv4 nor IPv6 networking detected, fatal.");
		exit(1);
	} else if (opt_ipv4 && !ipv4_works)
		msyslog(LOG_WARNING, "INIT: -4/--ipv4 ignored, IPv4 networking not found.");
	else if (opt_ipv6 && !ipv6_works)
		msyslog(LOG_WARNING, "INIT: -6/--ipv6 ignored, IPv6 networking not found.");

	/*
	 * Get the configuration.
	 */
	have_interface_option = (!listen_to_virtual_ips || explicit_interface);
	readconfig(getconfig(explicit_config));
	check_minsane();
        if ( 8 > sizeof(time_t) ) {
	    msyslog(LOG_ERR, "INIT: This system has a 32-bit time_t.");
	    msyslog(LOG_ERR, "INIT: This ntpd will fail on 2038-01-19T03:14:07Z.");
        }

	loop_config(LOOP_DRIFTINIT, 0);
	report_event(EVNT_SYSRESTART, NULL, NULL);

#ifndef ENABLE_EARLY_DROPROOT
	/* drop root privileges */
	if (sandbox(droproot, user, group, chrootdir, interface_interval!=0) && interface_interval) {
		interface_interval = 0;
		msyslog(LOG_INFO, "INIT: running as non-root disables dynamic interface tracking");
	}
#endif

	if (access(statsdir, W_OK) != 0) {
	    msyslog(LOG_ERR, "statistics directory %s does not exist or is unwriteable, error %s", statsdir, strerror(errno));
	}

	mainloop();
        /* unreachable, mainloop() never returns */
}

/*
 * Process incoming packets until exit or interrupted.
 */
static void mainloop(void)
{
	init_timer();

	for (;;) {
		if (sig_flags.sawQuit)
			finish_safe(signo);

		if (!sig_flags.sawALRM) {
			// FIXME: Check other flags
			/*
			 * Nothing to do.  Wait for something.
			 */
			io_handler();
		}

		if (sig_flags.sawALRM) {
			/*
			 * Out here, signals are unblocked.  Call timer routine
			 * to process expiry.
			 */
		    sig_flags.sawALRM = false;
			timer();
		}

#ifdef ENABLE_DNS_LOOKUP
		if (sig_flags.sawDNS) {
			sig_flags.sawDNS = false;
			dns_check();
		}
#endif

		/*
		 * Check files
		 */
		if (sig_flags.sawHUP) {
			sig_flags.sawHUP = false;
			msyslog(LOG_INFO, "LOG: Saw SIGHUP");

			reopen_logfile();

			{
			time_t tnow;
			time(&tnow);
			check_leap_file(false, tnow);
			}
		}

		/*
		 * Go around again
		 */

# if defined(HAVE_DNS_SD_H)
		if (mdnsreg && (current_time - mdnsreg ) > 60 && mdnstries && sys_vars.sys_leap != LEAP_NOTINSYNC) {
			mdnsreg = current_time;
			msyslog(LOG_INFO, "INIT: Attempting to register mDNS");
			if ( DNSServiceRegister (&mdns, 0, 0, NULL, "_ntp._udp", NULL, NULL,
			    htons(NTP_PORT), 0, NULL, NULL, NULL) != kDNSServiceErr_NoError ) {
				if (!--mdnstries) {
					msyslog(LOG_ERR, "INIT: Unable to register mDNS, giving up.");
				} else {
					msyslog(LOG_INFO, "INIT: Unable to register mDNS, will try later.");
				}
			} else {
				msyslog(LOG_INFO, "INIT: mDNS service registered.");
				mdnsreg = false;
			}
		}
# endif /* HAVE_DNS_SD_H */

	}
}


/*
 * finish_safe - exit gracefully
 */
static void
finish_safe(
	int sig
	)
{
	const char *sig_desc;

	sig_desc = strsignal(sig);
	if (sig_desc == NULL)
		sig_desc = "";
	msyslog(LOG_NOTICE, "ERR: %s exiting on signal %d (%s)", progname,
		sig, sig_desc);
	/* See Classic Bugs 2513 and Bug 2522 re the unlink of PIDFILE */
# if defined(HAVE_DNS_SD_H)
	if (mdns != NULL)
		DNSServiceRefDeallocate(mdns);
# endif
	peer_cleanup();
	exit(0);
}

void
catchQuit(
	int	sig
	)
{
	sig_flags.sawQuit = true;
	signo = sig;
}

/*
 * catchHUP - set flag to check files
 */
static void catchHUP(int sig)
{
	UNUSED_ARG(sig);
	sig_flags.sawHUP = true;
}

#ifdef ENABLE_DNS_LOOKUP
/*
 * catchDNS - set flag to process answer DNS lookup
 */
static void catchDNS(int sig)
{
	UNUSED_ARG(sig);
	sig_flags.sawDNS = true;
}
#endif

/*
 * wait_child_sync_if - implements parent side of -w/--wait-sync
 */
# ifdef HAVE_WORKING_FORK
static int
wait_child_sync_if(
	int	pipe_read_fd,
	long	wait_sync1
	)
{
	int	rc;
	int	exit_code;
	time_t	wait_end_time;
	time_t	cur_time;
	time_t	wait_rem;
	fd_set	readset;
	struct timespec wtimeout;

	if (0 == wait_sync1)
		return 0;

	/* waitsync_fd_to_close used solely by child */
	close(waitsync_fd_to_close);
	wait_end_time = time(NULL) + wait_sync1;
	do {
		cur_time = time(NULL);
		wait_rem = (wait_end_time > cur_time)
				? (wait_end_time - cur_time)
				: 0;
		wtimeout.tv_sec = wait_rem;
		wtimeout.tv_nsec = 0;
		FD_ZERO(&readset);
		FD_SET(pipe_read_fd, &readset);
		rc = pselect(pipe_read_fd + 1, &readset, NULL, NULL,
			     &wtimeout, NULL);
		if (-1 == rc) {
			if (EINTR == errno)
				continue;
			exit_code = (errno) ? errno : -1;
			msyslog(LOG_ERR,
				"ERR: --wait-sync select failed: %m");
			return exit_code;
		}
		if (0 == rc) {
			/*
			 * select() indicated a timeout, but in case
			 * its timeouts are affected by a step of the
			 * system clock, select() again with a zero
			 * timeout to confirm.
			 */
			FD_ZERO(&readset);
			FD_SET(pipe_read_fd, &readset);
			wtimeout.tv_sec = 0;
			wtimeout.tv_nsec = 0;
			rc = pselect(pipe_read_fd + 1, &readset, NULL,
				     NULL, &wtimeout, NULL);
			if (0 == rc)	/* select() timeout */
				break;
			else		/* readable */
				return 0;
		} else			/* readable */
			return 0;
	} while (wait_rem > 0);

	fprintf(stderr, "%s: -w/--wait-sync %ld timed out.\n",
		progname, wait_sync1);
	return ETIMEDOUT;
}
# endif	/* HAVE_WORKING_FORK */


/*
 * check_minsane - check peers to see if minsane should be bigger
 *
 * This is just a first cut.  It should probably fixup things automagically.
 * We also need to do similar for maxclock when running a pool command.
 *
 * With 2 working servers:
 *   if they don't agree, you can't tell which one is correct
 * With 3 working servers, 2 can outvote a falseticker
 * With 4 servers, you still have 3 if one is down.
 */
static void check_minsane()
{
    struct peer *peer;
    int servers = 0;

    if (sys_minsane > 1) return;  /* already adjusted, assume reasonable */

    for (peer = peer_list; peer != NULL; peer = peer->p_link) {
	if (peer->cfg.flags & FLAG_NOSELECT) continue;
	servers++;
	if (peer->cast_flags & MDF_POOL) {
	    /* pool server */
	    servers = sys_maxclock;
	    break;
	}
	/* ?? multicast and such */
    }

    if (servers >= 5)
	msyslog(LOG_ERR, "SYNC: Found %d servers, suggest minsane at least 3", servers);
    else if (servers == 4)
        msyslog(LOG_ERR, "SYNC: Found 4 servers, suggest minsane of 2");

}

# ifdef DEBUG

/*
 * moredebug - increase debugging verbosity
 */
static void
moredebug(
	int sig
	)
{
	int saved_errno = errno;

	UNUSED_ARG(sig);
	if (debug < 255) /* SPECIAL DEBUG */
	{
		debug++;
		msyslog(LOG_DEBUG, "LOG: debug raised to %d", debug);
	}
	errno = saved_errno;
}


/*
 * lessdebug - decrease debugging verbosity
 */
static void
lessdebug(
	int sig
	)
{
	int saved_errno = errno;

	UNUSED_ARG(sig);
	if (debug > 0) /* SPECIAL DEBUG */
	{
		debug--;
		msyslog(LOG_DEBUG, "LOG: debug lowered to %d", debug);
	}
	errno = saved_errno;
}

# else	/* !DEBUG follows */


/*
 * no_debug - We don't do the debug here.
 */
static void
no_debug(
	int sig
	)
{
	int saved_errno = errno;

	msyslog(LOG_DEBUG, "LOG: ntpd not compiled for debugging (signal %d)", sig);
	errno = saved_errno;
}
# endif	/* !DEBUG */

/*
 * close_all_except()
 *
 * Close all file descriptors except the given keep_fd.
 */
static void
close_all_except(
	int keep_fd
	)
{
	int fd;

	for (fd = 0; fd < keep_fd; fd++)
		close(fd);

	close_all_beyond(keep_fd);
}


/*
 * close_all_beyond()
 *
 * Close all file descriptors after the given keep_fd, which is the
 * highest fd to keep open.  See
 *
 * http://stackoverflow.com/questions/899038/getting-the-highest-allocated-file-descriptor
 */
static void
close_all_beyond(
	int keep_fd
	)
{
# ifdef HAVE_CLOSEFROM
	closefrom(keep_fd + 1);
# elif defined(F_CLOSEM)
	/*
	 * From 'Writing Reliable AIX Daemons,' SG24-4946-00,
	 * by Eric Agar (saves us from doing 32767 system
	 * calls)
	 */
	if (fcntl(keep_fd + 1, F_CLOSEM, 0) == -1)
		msyslog(LOG_ERR, "INIT: F_CLOSEM(%d): %m", keep_fd + 1);
# else  /* !HAVE_CLOSEFROM && !F_CLOSEM follows */
	int fd;
	int max_fd;

	/* includes POSIX case */
	max_fd = sysconf(_SC_OPEN_MAX);
	if (10000 < max_fd)
		msyslog(LOG_ERR, "INIT: close_all_beyond: closing %d files", max_fd);
	for (fd = keep_fd + 1; fd < max_fd; fd++)
		close(fd);
# endif /* !HAVE_CLOSEFROM && !F_CLOSEM */
}

