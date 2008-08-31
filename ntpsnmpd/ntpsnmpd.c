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

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <signal.h>
#include <ntpSnmpSubagentObject.h>
#include <sys/time.h>
#include <libntpq.h>
#ifdef SOLARIS /* needed with at least Solaris 8 */
#include <siginfo.h>
#endif

static int keep_running;

RETSIGTYPE
stop_server(int a) {
    keep_running = 0;
}

/* The main function just sets up a few things and then enters a loop in which it will 
 * wait for SNMP requests coming from the master agent 
 */

int
main (int argc, char **argv) {
  int background = 0; /* start as background process */
  int syslog = 1; /* use syslog for logging */
  char varvalue[1024];
	

  /* using the net-snmp syslog facility */
  if (syslog)
    snmp_enable_calllog();
  else
    snmp_enable_stderrlog();

  /* Become Subagent */
    netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_AGENT_ROLE, 1);
 
  /* go into background mode, if requested */
  if (background && netsnmp_daemonize(1, !syslog))
      exit(1);

  /* Now register with the master Agent X process */

  /* call Netsnmp socket startup macro, which will initialize the network stuff if required */
  SOCK_STARTUP;

  /* Set AgentX socket interface */
  netsnmp_ds_set_string(NETSNMP_DS_APPLICATION_ID,
                            NETSNMP_DS_AGENT_X_SOCKET, "tcp:localhost:705");

  init_agent("ntpsnmpd");

  /* Try to connect to ntpd */
  if ( ntpq_openhost("localhost") == 0 )
  {
	fprintf(stderr, "Error: Could not connect to ntpd. Aborting.\n");
	exit(1);
  }
  

  /* Register callback functions ...  */
  init_ntpSnmpSubagentObject();  
  init_snmp("ntpsnmpd");

  /* Signal handler */
  keep_running = 1;
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

