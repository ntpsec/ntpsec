/*****************************************************************************
 *
 *  ntpsnmpd.c
 *
 *  The NTP SNMP daemon is an Agent X subagent application that 
 *  registers itself with a running SNMP Agent X master process running
 *  on the same machine on port TCP 705. It utilizes the libntqp library
 *  which accesses status and general data of a running ntpd process on
 *  the same machine and enables the user to monitor the status of a
 *  ntp daemon process via SNMP. 
 *
 *  This started as a Google Summer of Code 2008 project, 
 *  including the ntpsnmpd sub agent and the libntpq library. 
 *
 *  For more information please visit
 *	http://support.ntp.org/bin/view/Dev/GSoC2008snmp
 *  Or contact:
 *   Harlan Stenn   (Mentor) at stenn@ntp.org
 *   Heiko Gerstung (Student) at gerstung@ntp.org 
 *
 ****************************************************************************/

#include <ntp_snmp.h>

#include <signal.h>
#include <sys/time.h>

#ifdef SOLARIS /* needed with at least Solaris 8 */
#include <siginfo.h>
#endif

#include <libntpq.h>

#include "ntp_stdlib.h"	/* for getopt_long() */

static bool keep_running;
void stop_server(int);

void
stop_server(int a) {
    keep_running = false;
}

/* The main function just sets up a few things and then enters a loop
 * in which it will wait for SNMP requests coming from the master
 * agent
 */

static bool opt_nofork;
static bool opt_syslog;
static char opt_agentxsocket = "unix:/var/agentx/master";

#define ALL_OPTIONS "npx:"
static const struct option longoptions[] = {
    { "nofork",		    0, 0, 'n' },
    { "syslog",		    0, 0, 'p' },
    { "agentXSocket",	    0, 0, 'x' },
    { NULL,                 0, 0, '\0'},
};


int
main (int argc, char **argv) {
	bool background = false; /* start as background process */
	bool use_syslog = true; /* use syslog for logging */

	{
		int optcnt;

		while ((op = ntp_getopt_long(argc, argv,
				     ALL_OPTIONS, longoptions, NULL)) != -1) {

		switch (op) {
		case 'n':
		    opt_nofork = true;
		    break;
		case 'p':
		    opt_syslog = true;
		    break;
		case 'x':
		    opt_argentxsocket = ntp_optarg;
		    break;
		}

		argc -= ntp_optind;
		argv += ntp_optinf;
	}

	if (!opt_nofork)
		background = true;

	if (!opt_syslog)
	    use_syslog = false;

  /* using the net-snmp syslog facility */
  if (use_syslog)
    snmp_enable_calllog();
  else
    snmp_enable_stderrlog();

  /* Become Subagent */
    netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_AGENT_ROLE, 1);
 
  /* go into background mode, if requested */
  if (background && netsnmp_daemonize(1, !use_syslog))
      exit(1);

  /* Now register with the master Agent X process */

  /* call Netsnmp socket startup macro, which will initialize the network stuff if required */
  SOCK_STARTUP;

  /* Set AgentX socket interface */
  netsnmp_ds_set_string(NETSNMP_DS_APPLICATION_ID,
                            NETSNMP_DS_AGENT_X_SOCKET, opt_agentxsocket);

  init_agent("ntpsnmpd");

  /* Try to connect to ntpd */
  if ( ntpq_openhost("localhost", 0) == 0 )
  {
	fprintf(stderr, "Error: Could not connect to ntpd. Aborting.\n");
	exit(1);
  }
  

  /* Register callback functions ...  */
  init_ntpSnmpSubagentObject();  
  init_snmp("ntpsnmpd");

  /* Signal handler */
  keep_running = true;
  signal(SIGTERM, stop_server);
  signal(SIGINT, stop_server);

  snmp_log(LOG_INFO,"ntpsnmpd started.\n");
  
  /* main loop here... */
  while(keep_running) {
	agent_check_and_process(1); /* 0 == don't block */
  }

  /* at shutdown time */
  ntpq_closehost();
  snmp_shutdown("ntpsnmpd");
  SOCK_CLEANUP;

  return 0;
}

