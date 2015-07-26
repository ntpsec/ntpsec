/*
 * ntpd.c - main program for the fixed point NTP daemon
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_stdlib.h"
#include <ntp_random.h>

#include "ntp_config.h"
#include "ntp_syslog.h"
#include "ntp_assert.h"
#include "isc/error.h"
#include "isc/formatcheck.h"
#include "iosignal.h"

#ifdef SIM
# include "ntpsim.h"
#endif

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
#include <signal.h>
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif /* HAVE_SYS_IOCTL_H */
#if defined(HAVE_RTPRIO)
# ifdef HAVE_SYS_LOCK_H
#  include <sys/lock.h>
# endif
# include <sys/rtprio.h>
#else
# ifdef HAVE_PLOCK
#  ifdef HAVE_SYS_LOCK_H
#	include <sys/lock.h>
#  endif
# endif
#endif
#include <sched.h>
#include <sys/mman.h>

#include <termios.h>

#include <getopt.h>

#ifdef SYS_DOMAINOS
# include <apollo/base.h>
#endif /* SYS_DOMAINOS */


#include "recvbuff.h"

#ifdef _AIX
# include <ulimit.h>
#endif /* _AIX */

#ifdef SCO5_CLOCK
# include <sys/ci/ciioctl.h>
#endif

#ifdef HAVE_DROPROOT
# include <ctype.h>
# include <grp.h>
# include <pwd.h>
#ifdef HAVE_LINUX_CAPABILITIES
# include <sys/capability.h>
# include <sys/prctl.h>
#endif /* HAVE_LINUX_CAPABILITIES */
#if defined(HAVE_PRIV_H) && defined(HAVE_SOLARIS_PRIVS)
# include <priv.h>
#endif /* HAVE_PRIV_H */
#endif /* HAVE_DROPROOT */

#if defined (LIBSECCOMP) && (KERN_SECCOMP)
/* # include <sys/types.h> */
# include <sys/resource.h>
# include <seccomp.h>
#endif /* LIBSECCOMP and KERN_SECCOMP */

#ifdef HAVE_DNSREGISTRATION
# include <dns_sd.h>
DNSServiceRef mdns;
#endif

#ifdef HAVE_SETPGRP_0
# define ntp_setpgrp(x, y)	setpgrp()
#else
# define ntp_setpgrp(x, y)	setpgrp(x, y)
#endif

#ifdef HAVE_SOLARIS_PRIVS
# define LOWPRIVS "basic,sys_time,net_privaddr,proc_setid,!proc_info,!proc_session,!proc_exec"
static priv_set_t *lowprivs = NULL;
static priv_set_t *highprivs = NULL;
#endif /* HAVE_SOLARIS_PRIVS */
/*
 * Scheduling priority we run at
 */
#define NTPD_PRIO	(-12)

int priority_done = 2;		/* 0 - Set priority */
				/* 1 - priority is OK where it is */
				/* 2 - Don't set priority */
				/* 1 and 2 are pretty much the same */

bool listen_to_virtual_ips = true;
long wait_sync = -1;
static char *logfilename;
bool opt_ipv4, opt_ipv6;
const char *explicit_config;

/*
 * No-fork flag.  If set, we do not become a background daemon.
 */
bool nofork = false;		/* Fork by default */

#ifdef HAVE_DNSREGISTRATION
/*
 * mDNS registration flag. If set, we attempt to register with the
 * mDNS system, but only after we have synched the first time. If the
 * attempt fails, then try again once per minute for up to 5
 * times. After all, we may be starting before mDNS.
 */
bool mdnsreg = false;
int mdnstries = 5;
#endif  /* HAVE_DNSREGISTRATION */

#ifdef HAVE_DROPROOT
bool droproot;
bool root_dropped;
char *user;		/* User to switch to */
char *group;		/* group to switch to */
const char *chrootdir;	/* directory to chroot to */
uid_t sw_uid;
gid_t sw_gid;
char *endp;
struct group *gr;
struct passwd *pw;
#endif /* HAVE_DROPROOT */

#ifdef HAVE_WORKING_FORK
int	waitsync_fd_to_close = -1;	/* -w/--wait-sync */
#endif

/*
 * Initializing flag.  All async routines watch this and only do their
 * thing when it is clear.
 */
bool initializing;

/*
 * Version declaration
 */
extern const char *Version;

extern int config_priority_override, config_priority;

char const *progname;

#ifdef HAVE_NETINFO
extern bool	check_netinfo;
#endif

bool was_alarmed;

#ifdef DECL_SYSCALL
/*
 * We put this here, since the argument profile is syscall-specific
 */
extern int syscall	(int, ...);
#endif /* DECL_SYSCALL */


#if !defined(SIM) && defined(SIGDIE1)
static	void	finish		(int);
#endif

#if !defined(SIM) && defined(HAVE_WORKING_FORK)
static int	wait_child_sync_if	(int, long);
#endif

#if !defined(SIM) && !defined(SYS_WINNT)
# ifdef	DEBUG
static	void	moredebug	(int);
static	void	lessdebug	(int);
# else	/* !DEBUG follows */
static	void	no_debug	(int);
# endif	/* !DEBUG */
#endif	/* !SIM && !SYS_WINNT */

int	saved_argc;
char **	saved_argv;

#ifndef SIM
int		ntpdmain		(int, char **);
static void	set_process_priority	(void);
static void	assertion_failed	(const char *, int,
					 isc_assertiontype_t,
					 const char *)
			__attribute__	((__noreturn__));
static void	library_fatal_error	(const char *, int, 
					 const char *, va_list)
					ISC_FORMAT_PRINTF(3, 0);
static void	library_unexpected_error(const char *, int,
					 const char *, va_list)
					ISC_FORMAT_PRINTF(3, 0);
#endif	/* !SIM */

static bool explicit_interface;

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

	while ((op = getopt(argc, argv,
			    "46aAbc:dD:f:gGi:I:k:l:LmMnNpPqQ:r:s:t:u:UvVw:x")) != -1) {

	    switch (op) {
	    case '4':
		opt_ipv4 = true;
		break;
	    case '6':
		opt_ipv6 = true;
		break;
	    case 'a':
		proto_config(PROTO_AUTHENTICATE, 1, 0.0, NULL);
		break;
	    case 'A':
		proto_config(PROTO_AUTHENTICATE, 0, 0.0, NULL);
		break;
	    case 'b':
		proto_config(PROTO_BROADCLIENT, 1, 0.0, NULL);
		break;
	    case 'c':
		explicit_config = optarg;
		break;
	    case 'd':
#ifdef DEBUG
		++debug;
#endif
		nofork = true;
		break;
	    case 'D':
#ifdef DEBUG
		debug = atoi(optarg);
#endif
		break;
	    case 'f':
		stats_config(STATS_FREQ_FILE, optarg);
		break;
	    case 'g':
		allow_panic = true;
		break;
	    case 'G':
		force_step_once = true;
		break;
	    case 'i':
#ifdef HAVE_DROPROOT
		droproot = true;
		chrootdir = optarg;
#endif
		break;
	    case 'I':
		/*
		 * --interface, listen on specified interfaces
		 */
		for (; optind < argc; optind++) {
			sockaddr_u	addr;
			if (argv[optind][0] == '-')
			    break;
		    	add_nic_rule(
				is_ip_address(argv[optind], AF_UNSPEC, &addr)
					? MATCH_IFADDR
					: MATCH_IFNAME,
				argv[optind], -1, ACTION_LISTEN);
		}
		explicit_interface = true;
		break;
	    case 'k':
		getauthkeys(optarg);
		break;
	    case 'l':
		logfilename = optarg;
		break;
	    case 'L':
		listen_to_virtual_ips = false;
		break;
	    case 'm':
#ifdef HAVE_DNSREGISTRATION
		mdnsreg = true;
#endif  /* HAVE_DNSREGISTRATION */
		break;
	    case 'M':
# ifdef SYS_WINNT
		set_mm_timer(MM_TIMER_HIRES);
# endif
		break;
	    case 'n':
		nofork = true;
		break;
	    case 'N':
		priority_done = 0;
		break;
	    case 'p':
		stats_config(STATS_PID_FILE, optarg);
		break;
	    case 'P':
		config_priority = atoi(optarg);
		config_priority_override = 1;
		priority_done = 0;
		break;
	    case 'q':
		mode_ntpdate = true;
		nofork = true; 
		break;
	    case 'Q':	/* savequit - undocumented/disabled(?) in NTP Classic */
		saveconfigquit = true;
		saveconfigfile = optarg;
		nofork = true; 
		break;
	    case 'r':
		{
		double tmp;
		if (sscanf(optarg, "%lf", &tmp) != 1) {
			msyslog(LOG_ERR,
				"command line broadcast delay value %s undecodable",
				optarg);
			printf("Bailout1\n");	
			exit(0);
		} else {
			proto_config(PROTO_BROADDELAY, 0, tmp, NULL);
		}
		}
		break;
	    case 's':
		stats_config(STATS_STATSDIR, optarg);
		break;
	    case 't':
		for (; optind < argc; optind++)
		{
		    u_long tkey = (int)atol(argv[optind]);
		    if (tkey == 0 || tkey > NTP_MAXKEY) {
			msyslog(LOG_ERR,
				"command line trusted key %s is invalid",
				argv[optind]);
			printf("Bailout2\n");	
			exit(0);
		    } else {
			authtrust(tkey, 1);
		    }
	        }
		break;
	    case 'u':
#ifdef HAVE_DROPROOT
		droproot = true;
		user = estrdup(optarg);
		group = strrchr(user, ':');
		if (group != NULL) {
			size_t	len;

			*group++ = '\0'; /* get rid of the ':' */
			len = group - user;
			group = estrdup(group);
			user = erealloc(user, len);
		}
#endif
		break;
	    case 'U':
		{
		    long val = atol(argv[optind]);

		    if (val >= 0)
			    interface_interval = val;
		    else {
			    fprintf(stderr,
				    "command line interface update interval %ld must not be negative\n",
				    val);
			    msyslog(LOG_ERR,
				    "command line interface update interval %ld must not be negative",
				    val);
			    printf("Bailout3\n");	
			    exit(0);
		    }
		}
		break;
	    case 'v':
		for (; optind < argc; optind++) {
		    if (argv[optind][0] == '-')
			break;
		    set_sys_var(argv[optind], strlen(argv[optind]) + 1, RW);
		}
		break;
	    case 'V':
		for (; optind < argc; optind++) {
		    if (argv[optind][0] == '-')
			break;
		    set_sys_var(argv[optind], strlen(argv[optind])+1,
				(u_short) (RW | DEF));
		}
		break;
	    case 'w':
		wait_sync = strtod(optarg, NULL);
		break;
	    case 'x':
		loop_config(LOOP_MAX, 600);
		break;
	    default :
		break;
	    } /*switch*/
	}

	/*
	 * Two port-specific options for Windows (USEPCC and PCCFREQ)
	 * have been omitted until the Windows port can be beaten back
	 * into shape.
	 */

	/*
	 * Sanity checks and derived options
	 */

	/* save list of servers from cmd line for config_peers() use */
	if (optind < argc) {
		cmdline_server_count = argc - optind;
		cmdline_servers = argv + optind;
	}
}

#ifdef SIM
int
main(
	int argc,
	char *argv[]
	)
{
	progname = argv[0];
	parse_cmdline_opts(argc, argv);

#ifdef DEBUG
	DPRINTF(1, ("%s\n", Version));
#endif

	return ntpsim(argc, argv);
}
#else	/* !SIM follows */
#ifdef NO_MAIN_ALLOWED
CALL(ntpd,"ntpd",ntpdmain);
#else	/* !NO_MAIN_ALLOWED follows */
#ifndef SYS_WINNT
int
main(
	int argc,
	char *argv[]
	)
{
	return ntpdmain(argc, argv);
}
#endif /* !SYS_WINNT */
#endif /* !NO_MAIN_ALLOWED */
#endif /* !SIM */

#ifdef _AIX
/*
 * OK. AIX is different than solaris in how it implements plock().
 * If you do NOT adjust the stack limit, you will get the MAXIMUM
 * stack size allocated and PINNED with you program. To check the
 * value, use ulimit -a.
 *
 * To fix this, we create an automatic variable and set our stack limit
 * to that PLUS 32KB of extra space (we need some headroom).
 *
 * This subroutine gets the stack address.
 *
 * Grover Davidson and Matt Ladendorf
 *
 */
static char *
get_aix_stack(void)
{
	char ch;
	return (&ch);
}

/*
 * Signal handler for SIGDANGER.
 */
static void
catch_danger(int signo)
{
	msyslog(LOG_INFO, "ntpd: setpgid(): %m");
	/* Make the system believe we'll free something, but don't do it! */
	return;
}
#endif /* _AIX */

/*
 * Set the process priority
 */
#ifndef SIM
static void
set_process_priority(void)
{

# ifdef DEBUG
	if (debug > 1)
		msyslog(LOG_DEBUG, "set_process_priority: %s: priority_done is <%d>",
			((priority_done)
			 ? "Leave priority alone"
			 : "Attempt to set priority"
				),
			priority_done);
# endif /* DEBUG */

	if (!priority_done) {
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
			msyslog(LOG_ERR, "sched_setscheduler(): %m");
		else
			++priority_done;
	}
# ifdef HAVE_RTPRIO
#  ifdef RTP_SET
	if (!priority_done) {
		struct rtprio srtp;

		srtp.type = RTP_PRIO_REALTIME;	/* was: RTP_PRIO_NORMAL */
		srtp.prio = 0;		/* 0 (hi) -> RTP_PRIO_MAX (31,lo) */

		if (rtprio(RTP_SET, getpid(), &srtp) < 0)
			msyslog(LOG_ERR, "rtprio() error: %m");
		else
			++priority_done;
	}
#  else	/* !RTP_SET follows */
	if (!priority_done) {
		if (rtprio(0, 120) < 0)
			msyslog(LOG_ERR, "rtprio() error: %m");
		else
			++priority_done;
	}
#  endif	/* !RTP_SET */
# endif	/* HAVE_RTPRIO */
# if defined(NTPD_PRIO) && NTPD_PRIO != 0
	if (!priority_done) {
		if (-1 == setpriority(PRIO_PROCESS, 0, NTPD_PRIO))
			msyslog(LOG_ERR, "setpriority() error: %m");
		else
			++priority_done;
	}
# endif	/* NTPD_PRIO && NTPD_PRIO != 0 */
	if (!priority_done)
		msyslog(LOG_ERR, "set_process_priority: No way found to improve our priority");
}
#endif	/* !SIM */


/*
 * Main program.  Initialize us, disconnect us from the tty if necessary,
 * and loop waiting for I/O and/or timer expiries.
 */
#ifndef SIM
int
ntpdmain(
	int argc,
	char *argv[]
	)
{
	l_fp		now;
	struct recvbuf *rbuf;
	mode_t		uv;
	uid_t		uid;
# if defined(HAVE_WORKING_FORK)
	int		pipe_fds[2];
	int		rc;
	int		exit_code;
#  ifdef _AIX
	struct sigaction sa;
#  endif
	int		fid;
# endif	/* HAVE_WORKING_FORK*/
# ifdef SCO5_CLOCK
	int		fd;
	int		zero;
# endif

	uv = umask(0);
	if (uv)
		umask(uv);
	else
		umask(022);
	saved_argc = argc;
	saved_argv = argv;
	progname = argv[0];
	initializing = true;		/* mark that we are initializing */
	parse_cmdline_opts(argc, argv);
# ifdef DEBUG
	setvbuf(stdout, NULL, _IOLBF, 0);
# endif

	init_logging(progname, NLOG_SYNCMASK, true);
	/* honor -l/--logfile option to log to a file */
	if (logfilename != NULL) {
		syslogit = false;
		change_logfile(logfilename, false);
	} else {
		if (nofork)
			msyslog_term = true;
		if (saveconfigquit)
			syslogit = false;
	}
	msyslog(LOG_NOTICE, "%s: Starting", Version);

	{
		int i;
		char buf[1024];	/* Secret knowledge of msyslog buf length */
		char *cp = buf;

		/* Note that every arg has an initial space character */
		snprintf(cp, sizeof(buf), "Command line:");
		cp += strlen(cp);

		for (i = 0; i < saved_argc ; ++i) {
			snprintf(cp, sizeof(buf) - (cp - buf),
				" %s", saved_argv[i]);
			cp += strlen(cp);
		}
		msyslog(LOG_INFO, "%s", buf);
	}

	/*
	 * Install trap handlers to log errors and assertion failures.
	 * Default handlers print to stderr which doesn't work if detached.
	 */
	isc_assertion_setcallback(assertion_failed);
	isc_error_setfatal(library_fatal_error);
	isc_error_setunexpected(library_unexpected_error);

	/* MPE lacks the concept of root */
# if !defined(MPE)
	uid = getuid();
	if (uid && !saveconfigquit) {
		msyslog_term = true;
		msyslog(LOG_ERR,
			"must be run as root, not uid %ld", (long)uid);
		exit(1);
	}
# endif

# ifdef HAVE_WORKING_FORK
	if (wait_sync <= 0)
	    wait_sync = 0;
	else {
	    /* -w requires a fork() even with debug > 0 */
	    nofork = false;
	    if (pipe(pipe_fds)) {
		exit_code = (errno) ? errno : -1;
		msyslog(LOG_ERR,
			"Pipe creation failed for --wait-sync: %m");
		exit(exit_code);
	    }
	    waitsync_fd_to_close = pipe_fds[1];
	}
# endif	/* HAVE_WORKING_FORK */

	init_lib();
# ifdef SYS_WINNT
	/*
	 * Start interpolation thread, must occur before first
	 * get_systime()
	 */
	init_winnt_time();
# endif
	/*
	 * Initialize random generator and public key pair
	 */
	get_systime(&now);

	ntp_srandom((int)(now.l_i * now.l_uf));

	/*
	 * Detach us from the terminal.  May need an #ifndef GIZMO.
	 */
	if (!nofork) {

# ifdef HAVE_WORKING_FORK
		rc = fork();
		if (-1 == rc) {
			exit_code = (errno) ? errno : -1;
			msyslog(LOG_ERR, "fork: %m");
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
		closelog();
		if (syslog_file != NULL) {
			fclose(syslog_file);
			syslog_file = NULL;
			syslogit = true;
		}
		close_all_except(waitsync_fd_to_close);
		INSIST(0 == open("/dev/null", 0) && 1 == dup2(0, 1) \
			&& 2 == dup2(0, 2));

		init_logging(progname, 0, true);
		/* we lost our logfile (if any) daemonizing */
		setup_logfile(logfilename);

#  ifdef SYS_DOMAINOS
		{
			uid_$t puid;
			status_$t st;

			proc2_$who_am_i(&puid);
			proc2_$make_server(&puid, &st);
		}
#  endif	/* SYS_DOMAINOS */
		if (setsid() == (pid_t)-1)
			msyslog(LOG_ERR, "setsid(): %m");
#  ifdef _AIX
		/* Don't get killed by low-on-memory signal. */
		sa.sa_handler = catch_danger;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		sigaction(SIGDANGER, &sa, NULL);
#  endif	/* _AIX */
# endif		/* HAVE_WORKING_FORK */
	}

# ifdef SCO5_CLOCK
	/*
	 * SCO OpenServer's system clock offers much more precise timekeeping
	 * on the base CPU than the other CPUs (for multiprocessor systems),
	 * so we must lock to the base CPU.
	 */
	fd = open("/dev/at1", O_RDONLY);		
	if (fd >= 0) {
		zero = 0;
		if (ioctl(fd, ACPU_LOCK, &zero) < 0)
			msyslog(LOG_ERR, "cannot lock to base CPU: %m");
		close(fd);
	}
# endif

	/* Setup stack size in preparation for locking pages in memory. */
	ntp_rlimit(RLIMIT_STACK, DFLT_RLIMIT_STACK * 4096, 4096, "4k");
#ifdef RLIMIT_MEMLOCK
	/*
	 * The default RLIMIT_MEMLOCK is very low on Linux systems.
	 * Unless we increase this limit malloc calls are likely to
	 * fail if we drop root privilege.  To be useful the value
	 * has to be larger than the largest ntpd resident set size.
	 */
	ntp_rlimit(RLIMIT_MEMLOCK, DFLT_RLIMIT_MEMLOCK * 1024 * 1024, 1024 * 1024, "MB");
#endif	/* RLIMIT_MEMLOCK */

	/*
	 * Set up signals we pay attention to locally.
	 */
# ifdef SIGDIE1
	signal_no_reset(SIGDIE1, finish);
	signal_no_reset(SIGDIE2, finish);
	signal_no_reset(SIGDIE3, finish);
	signal_no_reset(SIGDIE4, finish);
# endif
# ifdef SIGBUS
	signal_no_reset(SIGBUS, finish);
# endif

# if !defined(SYS_WINNT) && !defined(VMS)
#  ifdef DEBUG
	(void) signal_no_reset(MOREDEBUGSIG, moredebug);
	(void) signal_no_reset(LESSDEBUGSIG, lessdebug);
#  else
	(void) signal_no_reset(MOREDEBUGSIG, no_debug);
	(void) signal_no_reset(LESSDEBUGSIG, no_debug);
#  endif	/* DEBUG */
# endif	/* !SYS_WINNT && !VMS */

	/*
	 * Set up signals we should never pay attention to.
	 */
# ifdef SIGPIPE
	signal_no_reset(SIGPIPE, SIG_IGN);
# endif

	/*
	 * Call the init_ routines to initialize the data structures.
	 *
	 * Exactly what command-line options are we expecting here?
	 */
	INIT_SSL();
	init_auth();
	init_util();
	init_restrict();
	init_mon();
	init_timer();
	init_request();
	init_control();
	init_peer();
# ifdef REFCLOCK
	init_refclock();
# endif
	set_process_priority();
	init_proto();		/* Call at high priority */
	init_io();
	init_loopfilter();
	mon_start(MON_ON);	/* monitor on by default now	  */
				/* turn off in config if unwanted */

	/*
	 * Get the configuration.
	 */
	have_interface_option = (!listen_to_virtual_ips || explicit_interface);
	getconfig(explicit_config);

	if (do_memlock) {
		/*
		 * lock the process into memory
		 */
		if (!saveconfigquit &&
		    0 != mlockall(MCL_CURRENT|MCL_FUTURE))
			msyslog(LOG_ERR, "mlockall(): %m");
	}

	loop_config(LOOP_DRIFTINIT, 0);
	report_event(EVNT_SYSRESTART, NULL, NULL);
	initializing = false;

	if (ipv4_works && ipv6_works) {
		if (opt_ipv4)
			ipv6_works = false;
		else if (opt_ipv6)
			ipv4_works = false;
	} else if (!ipv4_works && !ipv6_works) {
		msyslog(LOG_ERR, "Neither IPv4 nor IPv6 networking detected, fatal.");
		exit(1);
	} else if (opt_ipv4 && !ipv4_works)
		msyslog(LOG_WARNING, "-4/--ipv4 ignored, IPv4 networking not found.");
	else if (opt_ipv6 && !ipv6_works)
		msyslog(LOG_WARNING, "-6/--ipv6 ignored, IPv6 networking not found.");



# ifdef HAVE_DROPROOT
	if (droproot) {
		/* Drop super-user privileges and chroot now if the OS supports this */

#  ifdef HAVE_LINUX_CAPABILITIES
		/* set flag: keep privileges accross setuid() call (we only really need cap_sys_time): */
		if (prctl( PR_SET_KEEPCAPS, 1L, 0L, 0L, 0L ) == -1) {
			msyslog( LOG_ERR, "prctl( PR_SET_KEEPCAPS, 1L ) failed: %m" );
			exit(-1);
		}
#  elif HAVE_SOLARIS_PRIVS
		/* Nothing to do here */
#  else
		/* we need a user to switch to */
		if (user == NULL) {
			msyslog(LOG_ERR, "Need user name to drop root privileges (see -u flag!)" );
			exit(-1);
		}
#  endif	/* HAVE_LINUX_CAPABILITIES || HAVE_SOLARIS_PRIVS */

		if (user != NULL) {
			if (isdigit((unsigned char)*user)) {
				sw_uid = (uid_t)strtoul(user, &endp, 0);
				if (*endp != '\0')
					goto getuser;

				if ((pw = getpwuid(sw_uid)) != NULL) {
					free(user);
					user = estrdup(pw->pw_name);
					sw_gid = pw->pw_gid;
				} else {
					errno = 0;
					msyslog(LOG_ERR, "Cannot find user ID %s", user);
					exit (-1);
				}

			} else {
getuser:
				errno = 0;
				if ((pw = getpwnam(user)) != NULL) {
					sw_uid = pw->pw_uid;
					sw_gid = pw->pw_gid;
				} else {
					if (errno)
						msyslog(LOG_ERR, "getpwnam(%s) failed: %m", user);
					else
						msyslog(LOG_ERR, "Cannot find user `%s'", user);
					exit (-1);
				}
			}
		}
		if (group != NULL) {
			if (isdigit((unsigned char)*group)) {
				sw_gid = (gid_t)strtoul(group, &endp, 0);
				if (*endp != '\0')
					goto getgroup;
			} else {
getgroup:
				if ((gr = getgrnam(group)) != NULL) {
					sw_gid = gr->gr_gid;
				} else {
					errno = 0;
					msyslog(LOG_ERR, "Cannot find group `%s'", group);
					exit (-1);
				}
			}
		}

		if (chrootdir ) {
			/* make sure cwd is inside the jail: */
			if (chdir(chrootdir)) {
				msyslog(LOG_ERR, "Cannot chdir() to `%s': %m", chrootdir);
				exit (-1);
			}
			if (chroot(chrootdir)) {
				msyslog(LOG_ERR, "Cannot chroot() to `%s': %m", chrootdir);
				exit (-1);
			}
			if (chdir("/")) {
				msyslog(LOG_ERR, "Cannot chdir() to`root after chroot(): %m");
				exit (-1);
			}
		}
#  ifdef HAVE_SOLARIS_PRIVS
		if ((lowprivs = priv_str_to_set(LOWPRIVS, ",", NULL)) == NULL) {
			msyslog(LOG_ERR, "priv_str_to_set() failed:%m");
			exit(-1);
		}
		if ((highprivs = priv_allocset()) == NULL) {
			msyslog(LOG_ERR, "priv_allocset() failed:%m");
			exit(-1);
		}
		(void) getppriv(PRIV_PERMITTED, highprivs);
		(void) priv_intersect(highprivs, lowprivs);
		if (setppriv(PRIV_SET, PRIV_PERMITTED, lowprivs) == -1) {
			msyslog(LOG_ERR, "setppriv() failed:%m");
			exit(-1);
		}
#  endif /* HAVE_SOLARIS_PRIVS */
		if (user && initgroups(user, sw_gid)) {
			msyslog(LOG_ERR, "Cannot initgroups() to user `%s': %m", user);
			exit (-1);
		}
		if (group && setgid(sw_gid)) {
			msyslog(LOG_ERR, "Cannot setgid() to group `%s': %m", group);
			exit (-1);
		}
		if (group && setegid(sw_gid)) {
			msyslog(LOG_ERR, "Cannot setegid() to group `%s': %m", group);
			exit (-1);
		}
		if (group)
			setgroups(1, &sw_gid);
		else
			initgroups(pw->pw_name, pw->pw_gid);
		if (user && setuid(sw_uid)) {
			msyslog(LOG_ERR, "Cannot setuid() to user `%s': %m", user);
			exit (-1);
		}
		if (user && seteuid(sw_uid)) {
			msyslog(LOG_ERR, "Cannot seteuid() to user `%s': %m", user);
			exit (-1);
		}

#  if !defined(HAVE_LINUX_CAPABILITIES) && !defined(HAVE_SOLARIS_PRIVS)
		/*
		 * for now assume that the privilege to bind to privileged ports
		 * is associated with running with uid 0 - should be refined on
		 * ports that allow binding to NTP_PORT with uid != 0
		 */
		disable_dynamic_updates |= (sw_uid != 0);  /* also notifies routing message listener */
#  endif /* !HAVE_LINUX_CAPABILITIES && !HAVE_SOLARIS_PRIVS */

		if (disable_dynamic_updates && interface_interval) {
			interface_interval = 0;
			msyslog(LOG_INFO, "running as non-root disables dynamic interface tracking");
		}

#  ifdef HAVE_LINUX_CAPABILITIES
		{
			/*
			 *  We may be running under non-root uid now, but we still hold full root privileges!
			 *  We drop all of them, except for the crucial one or two: cap_sys_time and
			 *  cap_net_bind_service if doing dynamic interface tracking.
			 */
			cap_t caps;
			char *captext;
			
			captext = (0 != interface_interval)
				      ? "cap_sys_time,cap_net_bind_service=pe"
				      : "cap_sys_time=pe";
			caps = cap_from_text(captext);
			if (!caps) {
				msyslog(LOG_ERR,
					"cap_from_text(%s) failed: %m",
					captext);
				exit(-1);
			}
			if (-1 == cap_set_proc(caps)) {
				msyslog(LOG_ERR,
					"cap_set_proc() failed to drop root privs: %m");
				exit(-1);
			}
			cap_free(caps);
		}
#  endif	/* HAVE_LINUX_CAPABILITIES */
#  ifdef HAVE_SOLARIS_PRIVS
		if (priv_delset(lowprivs, "proc_setid") == -1) {
			msyslog(LOG_ERR, "priv_delset() failed:%m");
			exit(-1);
		}
		if (setppriv(PRIV_SET, PRIV_PERMITTED, lowprivs) == -1) {
			msyslog(LOG_ERR, "setppriv() failed:%m");
			exit(-1);
		}
		priv_freeset(lowprivs);
		priv_freeset(highprivs);
#  endif /* HAVE_SOLARIS_PRIVS */
		root_dropped = true;
		fork_deferred_worker();
	}	/* if (droproot) */
# endif	/* HAVE_DROPROOT */

/* libssecomp sandboxing */
#if defined (LIBSECCOMP) && (KERN_SECCOMP)
	scmp_filter_ctx ctx;

	if ((ctx = seccomp_init(SCMP_ACT_KILL)) < 0)
		msyslog(LOG_ERR, "%s: seccomp_init(SCMP_ACT_KILL) failed: %m", __func__);
	else {
		msyslog(LOG_DEBUG, "%s: seccomp_init(SCMP_ACT_KILL) succeeded", __func__);
	}

#ifdef __x86_64__
int scmp_sc[] = {
	SCMP_SYS(adjtimex),
	SCMP_SYS(bind),
	SCMP_SYS(brk),
	SCMP_SYS(chdir),
	SCMP_SYS(clock_gettime),
	SCMP_SYS(clock_settime),
	SCMP_SYS(close),
	SCMP_SYS(connect),
	SCMP_SYS(exit_group),
	SCMP_SYS(fstat),
	SCMP_SYS(fsync),
	SCMP_SYS(futex),
	SCMP_SYS(getitimer),
	SCMP_SYS(getsockname),
	SCMP_SYS(ioctl),
	SCMP_SYS(lseek),
	SCMP_SYS(madvise),
	SCMP_SYS(mmap),
	SCMP_SYS(munmap),
	SCMP_SYS(open),
	SCMP_SYS(poll),
	SCMP_SYS(read),
	SCMP_SYS(recvmsg),
	SCMP_SYS(rename),
	SCMP_SYS(rt_sigaction),
	SCMP_SYS(rt_sigprocmask),
	SCMP_SYS(rt_sigreturn),
	SCMP_SYS(select),
	SCMP_SYS(sendto),
	SCMP_SYS(setitimer),
	SCMP_SYS(setsid),
	SCMP_SYS(socket),
	SCMP_SYS(stat),
	SCMP_SYS(time),
	SCMP_SYS(write),
};
#endif
#ifdef __i386__
int scmp_sc[] = {
	SCMP_SYS(_newselect),
	SCMP_SYS(adjtimex),
	SCMP_SYS(brk),
	SCMP_SYS(chdir),
	SCMP_SYS(clock_gettime),
	SCMP_SYS(clock_settime),
	SCMP_SYS(close),
	SCMP_SYS(exit_group),
	SCMP_SYS(fsync),
	SCMP_SYS(futex),
	SCMP_SYS(getitimer),
	SCMP_SYS(madvise),
	SCMP_SYS(mmap),
	SCMP_SYS(mmap2),
	SCMP_SYS(munmap),
	SCMP_SYS(open),
	SCMP_SYS(poll),
	SCMP_SYS(read),
	SCMP_SYS(rename),
	SCMP_SYS(rt_sigaction),
	SCMP_SYS(rt_sigprocmask),
	SCMP_SYS(select),
	SCMP_SYS(setitimer),
	SCMP_SYS(setsid),
	SCMP_SYS(sigprocmask),
	SCMP_SYS(sigreturn),
	SCMP_SYS(socketcall),
	SCMP_SYS(stat64),
	SCMP_SYS(time),
	SCMP_SYS(write),
};
#endif
	{
		int i;

		for (i = 0; i < COUNTOF(scmp_sc); i++) {
			if (seccomp_rule_add(ctx,
			    SCMP_ACT_ALLOW, scmp_sc[i], 0) < 0) {
				msyslog(LOG_ERR,
				    "%s: seccomp_rule_add() failed: %m",
				    __func__);
			}
		}
	}

	if (seccomp_load(ctx) < 0)
		msyslog(LOG_ERR, "%s: seccomp_load() failed: %m",
		    __func__);	
	else {
		msyslog(LOG_DEBUG, "%s: seccomp_load() succeeded", __func__);
	}
#endif /* LIBSECCOMP and KERN_SECCOMP */

# ifdef HAVE_IO_COMPLETION_PORT

	for (;;) {
		GetReceivedBuffers();
# else /* normal I/O */

	BLOCK_IO_AND_ALARM();
	was_alarmed = false;

	for (;;) {
		if (alarm_flag) {	/* alarmed? */
			was_alarmed = true;
			alarm_flag = false;
		}

		if (!was_alarmed && !has_full_recv_buffer()) {
			/*
			 * Nothing to do.  Wait for something.
			 */
			io_handler();
		}

		if (alarm_flag) {	/* alarmed? */
			was_alarmed = true;
			alarm_flag = false;
		}

		if (was_alarmed) {
			UNBLOCK_IO_AND_ALARM();
			/*
			 * Out here, signals are unblocked.  Call timer routine
			 * to process expiry.
			 */
			timer();
			was_alarmed = false;
			BLOCK_IO_AND_ALARM();
		}

# endif		/* !HAVE_IO_COMPLETION_PORT */

# ifdef DEBUG_TIMING
		{
			l_fp pts;
			l_fp tsa, tsb;
			int bufcount = 0;

			get_systime(&pts);
			tsa = pts;
# endif
			rbuf = get_full_recv_buffer();
			while (rbuf != NULL) {
				if (alarm_flag) {
					was_alarmed = true;
					alarm_flag = false;
				}
				UNBLOCK_IO_AND_ALARM();

				if (was_alarmed) {
					/* avoid timer starvation during lengthy I/O handling */
					timer();
					was_alarmed = false;
				}

				/*
				 * Call the data procedure to handle each received
				 * packet.
				 */
				if (rbuf->receiver != NULL) {
# ifdef DEBUG_TIMING
					l_fp dts = pts;

					L_SUB(&dts, &rbuf->recv_time);
					DPRINTF(2, ("processing timestamp delta %s (with prec. fuzz)\n", lfptoa(&dts, 9)));
					collect_timing(rbuf, "buffer processing delay", 1, &dts);
					bufcount++;
# endif
					(*rbuf->receiver)(rbuf);
				} else {
					msyslog(LOG_ERR, "fatal: receive buffer callback NULL");
					abort();
				}

				BLOCK_IO_AND_ALARM();
				freerecvbuf(rbuf);
				rbuf = get_full_recv_buffer();
			}
# ifdef DEBUG_TIMING
			get_systime(&tsb);
			L_SUB(&tsb, &tsa);
			if (bufcount) {
				collect_timing(NULL, "processing", bufcount, &tsb);
				DPRINTF(2, ("processing time for %d buffers %s\n", bufcount, lfptoa(&tsb, 9)));
			}
		}
# endif

		/*
		 * Go around again
		 */

# ifdef HAVE_DNSREGISTRATION
		if (mdnsreg && (current_time - mdnsreg ) > 60 && mdnstries && sys_leap != LEAP_NOTINSYNC) {
			mdnsreg = current_time;
			msyslog(LOG_INFO, "Attempting to register mDNS");
			if ( DNSServiceRegister (&mdns, 0, 0, NULL, "_ntp._udp", NULL, NULL, 
			    htons(NTP_PORT), 0, NULL, NULL, NULL) != kDNSServiceErr_NoError ) {
				if (!--mdnstries) {
					msyslog(LOG_ERR, "Unable to register mDNS, giving up.");
				} else {	
					msyslog(LOG_INFO, "Unable to register mDNS, will try later.");
				}
			} else {
				msyslog(LOG_INFO, "mDNS service registered.");
				mdnsreg = false;
			}
		}
# endif /* HAVE_DNSREGISTRATION */

	}
	UNBLOCK_IO_AND_ALARM();
	return 1;
}
#endif	/* !SIM */


#if !defined(SIM) && defined(SIGDIE1)
/*
 * finish - exit gracefully
 */
static void
finish(
	int sig
	)
{
	const char *sig_desc;

	sig_desc = NULL;
	sig_desc = strsignal(sig);
	if (sig_desc == NULL)
		sig_desc = "";
	msyslog(LOG_NOTICE, "%s exiting on signal %d (%s)", progname,
		sig, sig_desc);
	/* See Bug 2513 and Bug 2522 re the unlink of PIDFILE */
# ifdef HAVE_DNSREGISTRATION
	if (mdns != NULL)
		DNSServiceRefDeallocate(mdns);
# endif
	peer_cleanup();
	exit(0);
}
#endif	/* !SIM && SIGDIE1 */


#ifndef SIM
/*
 * wait_child_sync_if - implements parent side of -w/--wait-sync
 */
# ifdef HAVE_WORKING_FORK
static int
wait_child_sync_if(
	int	pipe_read_fd,
	long	wait_sync
	)
{
	int	rc;
	int	exit_code;
	time_t	wait_end_time;
	time_t	cur_time;
	time_t	wait_rem;
	fd_set	readset;
	struct timeval wtimeout;

	if (0 == wait_sync) 
		return 0;

	/* waitsync_fd_to_close used solely by child */
	close(waitsync_fd_to_close);
	wait_end_time = time(NULL) + wait_sync;
	do {
		cur_time = time(NULL);
		wait_rem = (wait_end_time > cur_time)
				? (wait_end_time - cur_time)
				: 0;
		wtimeout.tv_sec = wait_rem;
		wtimeout.tv_usec = 0;
		FD_ZERO(&readset);
		FD_SET(pipe_read_fd, &readset);
		rc = select(pipe_read_fd + 1, &readset, NULL, NULL,
			    &wtimeout);
		if (-1 == rc) {
			if (EINTR == errno)
				continue;
			exit_code = (errno) ? errno : -1;
			msyslog(LOG_ERR,
				"--wait-sync select failed: %m");
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
			wtimeout.tv_usec = 0;
			rc = select(pipe_read_fd + 1, &readset, NULL,
				    NULL, &wtimeout);
			if (0 == rc)	/* select() timeout */
				break;
			else		/* readable */
				return 0;
		} else			/* readable */
			return 0;
	} while (wait_rem > 0);

	fprintf(stderr, "%s: -w/--wait-sync %ld timed out.\n",
		progname, wait_sync);
	return ETIMEDOUT;
}
# endif	/* HAVE_WORKING_FORK */


/*
 * assertion_failed - Redirect assertion failures to msyslog().
 */
static void
assertion_failed(
	const char *file,
	int line,
	isc_assertiontype_t type,
	const char *cond
	)
{
	isc_assertion_setcallback(NULL);    /* Avoid recursion */

	msyslog(LOG_ERR, "%s:%d: %s(%s) failed",
		file, line, isc_assertion_typetotext(type), cond);
	msyslog(LOG_ERR, "exiting (due to assertion failure)");

#if defined(DEBUG) && defined(SYS_WINNT)
	if (debug)
		DebugBreak();
#endif

	abort();
}


/*
 * library_fatal_error - Handle fatal errors from our libraries.
 */
static void
library_fatal_error(
	const char *file,
	int line,
	const char *format,
	va_list args
	)
{
	char errbuf[256];

	isc_error_setfatal(NULL);  /* Avoid recursion */

	msyslog(LOG_ERR, "%s:%d: fatal error:", file, line);
	vsnprintf(errbuf, sizeof(errbuf), format, args);
	msyslog(LOG_ERR, "%s", errbuf);
	msyslog(LOG_ERR, "exiting (due to fatal error in library)");

#if defined(DEBUG) && defined(SYS_WINNT)
	if (debug)
		DebugBreak();
#endif

	abort();
}


/*
 * library_unexpected_error - Handle non fatal errors from our libraries.
 */
# define MAX_UNEXPECTED_ERRORS 100
int unexpected_error_cnt = 0;
static void
library_unexpected_error(
	const char *file,
	int line,
	const char *format,
	va_list args
	)
{
	char errbuf[256];

	if (unexpected_error_cnt >= MAX_UNEXPECTED_ERRORS)
		return;	/* avoid clutter in log */

	msyslog(LOG_ERR, "%s:%d: unexpected error:", file, line);
	vsnprintf(errbuf, sizeof(errbuf), format, args);
	msyslog(LOG_ERR, "%s", errbuf);

	if (++unexpected_error_cnt == MAX_UNEXPECTED_ERRORS)
		msyslog(LOG_ERR, "Too many errors.  Shutting up.");

}
#endif	/* !SIM */

#if !defined(SIM) && !defined(SYS_WINNT)
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

	if (debug < 255)
	{
		debug++;
		msyslog(LOG_DEBUG, "debug raised to %d", debug);
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

	if (debug > 0)
	{
		debug--;
		msyslog(LOG_DEBUG, "debug lowered to %d", debug);
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

	msyslog(LOG_DEBUG, "ntpd not compiled for debugging (signal %d)", sig);
	errno = saved_errno;
}
# endif	/* !DEBUG */
#endif	/* !SIM && !SYS_WINNT */
