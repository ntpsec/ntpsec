/*
 * cmd_args.c = command-line argument processing
 */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ntpd.h"
#include "ntp_stdlib.h"
#include "ntp_cmdargs.h"

#ifdef SIM
# include "ntpsim.h"
# include "ntpdsim-opts.h"
#else
# include "ntpd-opts.h"
#endif /* SIM */

/*
 * Definitions of things either imported from or exported to outside
 */
extern char const *progname;
extern const char *specific_interface;
extern int default_ai_family;

#ifdef HAVE_NETINFO
extern int	check_netinfo;
#endif


/*
 * getstartup - search through the options looking for a debugging flag
 */
void
getstartup(
	int argc,
	char *argv[]
	)
{
#if 0
	int errflg;
	extern int priority_done;
	int c;

	/*
	 * "This is a big hack.	We don't really want to read command line
	 * configuration until everything else is initialized, since
	 * the ability to configure the system may depend on storage
	 * and the like having been initialized."
	 *
	 * Now we crack the options first and process them later, as
	 * needed...
	 *
	 * "Except that we also don't want to initialize anything until
	 * after detaching from the terminal, ..."
	 *
	 * Why?
	 *
	 * "... but we won't know to do that until we've parsed the command
	 * line.  Do that now, crudely, and do it again later."
	 *
	 * Again, now we crack in one place and process in another.
	 *
	 * Our ntp_getopt() is explicitly reusable, by the
	 * way.  Your own mileage may vary.
	 *
	 * This hack is even called twice (to allow complete logging to file)
	 */

	errflg = 0;

	/*
	 * Decode argument list
	 */
	while ((c = ntp_getopt(argc, argv, ntp_options)) != EOF)
	    switch (c) {
#ifdef DEBUG
		case 'd':
		    ++debug;
		    break;
		case 'D':
		    debug = (int)atol(ntp_optarg);
		    printf("Debug1: %s -> %x = %d\n", ntp_optarg, debug, debug);
		    break;
#else
		case 'd':
		case 'D':
		    msyslog(LOG_ERR, "ntpd not compiled with -DDEBUG option - no DEBUG support");
		    fprintf(stderr, "ntpd not compiled with -DDEBUG option - no DEBUG support\n");
		    ++errflg;
		    break;
#endif
		case 'L':
		    listen_to_virtual_ips = 0;
		    if(ntp_optarg)
			specific_interface = ntp_optarg; 
		    break;
		case 'l':
			{
				FILE *new_file;

				if(strcmp(ntp_optarg, "stderr") == 0)
					new_file = stderr;
				else if(strcmp(ntp_optarg, "stdout") == 0)
					new_file = stdout;
				else
					new_file = fopen(ntp_optarg, "a");
				if (new_file != NULL) {
					NLOG(NLOG_SYSINFO)
						msyslog(LOG_NOTICE, "logging to file %s", ntp_optarg);
					if (syslog_file != NULL &&
						fileno(syslog_file) != fileno(new_file))
						(void)fclose(syslog_file);

					syslog_file = new_file;
					syslogit = 0;
				}
				else
					msyslog(LOG_ERR,
						"Cannot open log file %s",
						ntp_optarg);
			}
			break;

		case 'n':
		case 'q':
		    ++nofork;
		    break;
 
		case 'N':
		    priority_done = 0;
		    break;
			
		case '?':
		    ++errflg;
		    break;

		case '-':
			if ( ! strcmp(ntp_optarg, "version") ) {
				printf("%.80s: %.80s\n", progname, Version);
				exit(0);
			} else if ( ! strcmp(ntp_optarg, "help") ) {
				optionUsage(&myOptions, 0);
			} else if ( ! strcmp(ntp_optarg, "copyright") ) {
				printf("unknown\n");
				exit(0);
			} else {
				fprintf(stderr, "%.80s: Error unknown argument '--%.80s'\n",
					progname,
					ntp_optarg);
				exit(12);
			}
			break;

		default:
			break;
	    }

	if (errflg || ntp_optind != argc) {
		optionUsage(&myOptions, 2);
	}
	ntp_optind = 0;	/* reset ntp_optind to restart ntp_getopt */

#ifdef DEBUG
	if (debug) {
#ifdef HAVE_SETVBUF
		static char buf[BUFSIZ];
		setvbuf(stdout, buf, _IOLBF, BUFSIZ);
#else
		setlinebuf(stdout);
#endif
	}
#endif
#endif
}

/*
 * getCmdOpts - get command line options
 */
void
getCmdOpts(
	int argc,
	char *argv[]
	)
{
	extern const char *config_file;
	int errflg;
	tOptions *myOptions =
#ifdef SIM
				&ntpdsimOptions
#else
				&ntpdOptions
#endif
				;

	/*
	 * Initialize, initialize
	 */
	errflg = 0;

	switch (WHICH_IDX_IPV4) {
	    case INDEX_OPT_IPV4:
		default_ai_family = AF_INET;
		break;
	    case INDEX_OPT_IPV6:
		default_ai_family = AF_INET6;
		break;
	    default:
		/* ai_fam_templ = ai_fam_default;	*/
		break;
	}

	if (HAVE_OPT( AUTHREQ ))
		proto_config(PROTO_AUTHENTICATE, 1, 0., NULL);

	if (HAVE_OPT( AUTHNOREQ ))
		proto_config(PROTO_AUTHENTICATE, 0, 0., NULL);

	if (HAVE_OPT( BCASTSYNC ))
		proto_config(PROTO_BROADCLIENT, 1, 0., NULL);

	if (HAVE_OPT( CONFIGFILE )) {
		config_file = OPT_ARG( CONFIGFILE );
#ifdef HAVE_NETINFO
		check_netinfo = 0;
#endif
	}

	if (HAVE_OPT( DRIFTFILE ))
		stats_config(STATS_FREQ_FILE, OPT_ARG( DRIFTFILE ));

	if (HAVE_OPT( PANICGATE ))
		allow_panic = TRUE;

	if (HAVE_OPT( JAILDIR )) {
#ifdef HAVE_DROPROOT
			droproot = 1;
			chrootdir = OPT_ARG( JAILDIR );
#else
			errflg++;
#endif
	}

	if (HAVE_OPT( KEYFILE ))
		getauthkeys(OPT_ARG( KEYFILE ));

	if (HAVE_OPT( PIDFILE ))
		stats_config(STATS_PID_FILE, OPT_ARG( PIDFILE ));

	if (HAVE_OPT( QUIT ))
		mode_ntpdate = TRUE;

	if (HAVE_OPT( PROPAGATIONDELAY ))
		do {
			double tmp;
			const char *my_ntp_optarg = OPT_ARG( PROPAGATIONDELAY );

			if (sscanf(my_ntp_optarg, "%lf", &tmp) != 1) {
				msyslog(LOG_ERR,
					"command line broadcast delay value %s undecodable",
					my_ntp_optarg);
			} else {
				proto_config(PROTO_BROADDELAY, 0, tmp, NULL);
			}
		} while (0);

	if (HAVE_OPT( STATSDIR ))
		stats_config(STATS_STATSDIR, OPT_ARG( STATSDIR ));

	if (HAVE_OPT( TRUSTEDKEY )) {
		int           ct = STACKCT_OPT(  TRUSTEDKEY );
		const char**  pp = STACKLST_OPT( TRUSTEDKEY );

		do  {
			u_long tkey;
			const char* p = *pp++;

			tkey = (int)atol(p);
			if (tkey <= 0 || tkey > NTP_MAXKEY) {
				msyslog(LOG_ERR,
				    "command line trusted key %s is invalid",
				    p);
			} else {
				authtrust(tkey, 1);
			}
		} while (--ct > 0);
	}

	if (HAVE_OPT( USER )) {
#ifdef HAVE_DROPROOT
		char *ntp_optarg = OPT_ARG( USER );

		droproot = 1;
		user = malloc(strlen(ntp_optarg) + 1);
		if (user == NULL) {
			errflg++;
		} else {
			(void)strncpy(user, ntp_optarg, strlen(ntp_optarg) + 1);
			group = rindex(user, ':');
			if (group)
				*group++ = '\0'; /* get rid of the ':' */
		}
#else
		errflg++;
#endif
	}

	if (HAVE_OPT( VAR )) {
		int           ct = STACKCT_OPT(  VAR );
		const char**  pp = STACKLST_OPT( VAR );

		do  {
			const char* my_ntp_optarg = *pp++;

			set_sys_var(my_ntp_optarg, strlen(my_ntp_optarg)+1,
			    (u_short) (RW));
		} while (--ct > 0);
	}

	if (HAVE_OPT( DVAR )) {
		int           ct = STACKCT_OPT(  DVAR );
		const char**  pp = STACKLST_OPT( DVAR );

		do  {
			const char* my_ntp_optarg = *pp++;

			set_sys_var(my_ntp_optarg, strlen(my_ntp_optarg)+1,
			    (u_short) (RW | DEF));
		} while (--ct > 0);
	}

	if (HAVE_OPT( SLEW ))
		clock_max = 600;

	if (HAVE_OPT( UPDATEINTERVAL )) {
		long val = OPT_VALUE_UPDATEINTERVAL;
		extern int interface_interval;
			  
		if ((val >= 60) || (val == 0))
			interface_interval = val;
		else {
			msyslog(LOG_ERR,
				"command line interface update interval %ld must be 0 or longer than 60 seconds",
				      val);
			errflg++;
		}
	}
#ifdef SIM
	if (HAVE_OPT( SIMBROADCASTDELAY ))
		sscanf(OPT_ARG( SIMBROADCASTDELAY ), "%lf", &ntp_node.bdly);

	if (HAVE_OPT( PHASENOISE ))
		sscanf(OPT_ARG( PHASENOISE ), "%lf", &ntp_node.snse);

	if (HAVE_OPT( SIMSLEW ))
		sscanf(OPT_ARG( SIMSLEW ), "%lf", &ntp_node.slew);

	if (HAVE_OPT( SERVERTIME ))
		sscanf(OPT_ARG( SERVERTIME ), "%lf", &ntp_node.clk_time);

	if (HAVE_OPT( ENDSIMTIME ))
		sscanf(OPT_ARG( ENDSIMTIME ), "%lf", &ntp_node.sim_time);

	if (HAVE_OPT( FREQERR ))
		sscanf(OPT_ARG( FREQERR ), "%lf", &ntp_node.ferr);

	if (HAVE_OPT( WALKNOISE ))
		sscanf(OPT_ARG( WALKNOISE ), "%lf", &ntp_node.fnse);

	if (HAVE_OPT( NDELAY ))
		sscanf(OPT_ARG( NDELAY ), "%lf", &ntp_node.ndly);

	if (HAVE_OPT( PDELAY ))
		sscanf(OPT_ARG( PDELAY ), "%lf", &ntp_node.pdly);

#endif /* SIM */

	if (errflg || argc) {
		printf("argc is <%d>\n", argc);
		optionUsage(myOptions, 2);
	}
	return;
}
