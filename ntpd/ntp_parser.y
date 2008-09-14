/* ntp_parser.y
 *
 * The parser for the NTP configuration file.
 *
 * Written By: Sachin Kamboj
 *             University of Delaware
 *             Newark, DE 19711
 * Copyright (c) 2006
 */

%{
  #ifdef HAVE_CONFIG_H
  # include <config.h>
  #endif

  #include "ntpd.h"
  #include "ntp_machine.h"
  #include "ntp.h"
  #include "ntp_stdlib.h"
  #include "ntp_filegen.h"
  #include "ntp_data_structures.h"
  #include "ntp_scanner.h"
  #include "ntp_config.h"
  #include "ntp_crypto.h"

  #include "ntpsim.h"		/* HMS: Do we really want this all the time? */
                                /* SK: It might be a good idea to always
				   include the simulator code. That way
				   someone can use the same configuration file
				   for both the simulator and the daemon
				*/

  /*  #include "ntp_parser.h"     SK: Arent't we generating this using bison?
				   This was not an issue when we were
				   directly including the source file. However,
				   we will need a seperate description of the
				   tokens in the scanner.
				*/


  struct FILE_INFO *ip_file;   /* Pointer to the configuration file stream */

  #define YYERROR_VERBOSE
  void yyerror (char *msg);
  extern int input_from_file;  /* 0=input from ntpq>config command buffer */
  extern int cryptosw;
%}

%union {
    char   *String;
    double  Double;
    int     Integer;
    void   *VoidPtr;
    queue  *Queue;
    struct attr_val *Attr_val;
    struct address_node *Address_node;
    struct setvar_node *Set_var;

    /* Simulation types */
    server_info *Sim_server;
    script_info *Sim_script;
}

/*** TERMINALS ***/
%token		T_Age
%token		T_Allan
%token		T_Auth
%token		T_Autokey
%token		T_Automax
%token		T_Average
%token		T_Bclient
%token		T_Beacon
%token		T_Bias
%token		T_Broadcast
%token		T_Broadcastclient
%token		T_Broadcastdelay
%token		T_Burst
%token		T_Calibrate
%token		T_Calldelay
%token		T_Ceiling
%token		T_Clockstats
%token		T_Cohort
%token		T_ControlKey
%token		T_Crypto
%token		T_Cryptostats
%token		T_Day
%token		T_Default
%token		T_Disable
%token		T_Discard
%token		T_Dispersion
%token <Double>	T_Double
%token		T_Driftfile
%token <Double>	T_WanderThreshold
%token		T_Enable
%token		T_End
%token		T_False
%token		T_File
%token		T_Filegen
%token		T_Flag1
%token		T_Flag2
%token		T_Flag3
%token		T_Flag4
%token		T_Flake
%token		T_Floor
%token		T_Freq
%token		T_Fudge
%token		T_Host
%token		T_Huffpuff
%token      T_IPv4_address
%token      T_IPv6_address
%token		T_Iburst
%token		T_Ident
%token		T_Ignore
%token		T_Includefile
%token <Integer> T_Integer
%token		T_Interface
%token		T_Kernel
%token		T_Key
%token		T_Keys
%token		T_Keysdir
%token		T_Kod
%token		T_Leap
%token		T_Leapfile
%token		T_Limited
%token		T_Link
%token		T_Logconfig
%token		T_Logfile
%token		T_Loopstats
%token		T_Lowpriotrap
%token		T_Manycastclient
%token		T_Manycastserver
%token		T_Mask
%token		T_Maxclock
%token		T_Maxdist
%token		T_Maxhop
%token		T_Maxpoll
%token		T_Minclock
%token		T_Mindist
%token		T_Minimum
%token		T_Minpoll
%token		T_Minsane
%token          T_Mode
%token		T_Monitor
%token		T_Month
%token		T_Multicastclient
%token		T_Nolink
%token		T_Nomodify
%token		T_None
%token		T_Nopeer
%token		T_Noquery
%token		T_Noselect
%token		T_Noserve
%token		T_Notrap
%token		T_Notrust
%token		T_Novolley
%token		T_Ntp
%token		T_Ntpport
%token		T_Orphan
%token		T_Panic
%token		T_Peer
%token		T_Peerstats
%token		T_Phone
%token		T_Pid
%token		T_Pidfile
%token          T_Pool
%token		T_Port
%token		T_Pps
%token		T_Preempt
%token		T_Prefer
%token		T_Protostats
%token		T_Pw
%token		T_Qos
%token		T_RandFile
%token		T_Rawstats
%token		T_Refid
%token		T_Requestkey
%token		T_Restrict
%token		T_Revoke
%token		T_Server
%token		T_Setvar
%token		T_Sign
%token		T_Statistics
%token		T_Stats
%token		T_Statsdir
%token		T_Step
%token		T_Stepout
%token		T_Stratum
%token <String> T_String
%token		T_Sysstats
%token		T_Tick
%token		T_Time1
%token		T_Time2
%token		T_Timingstats
%token		T_Tinker
%token		T_Tos
%token		T_Trap
%token		T_True
%token		T_Trustedkey
%token		T_Ttl
%token		T_Type
%token		T_Version
%token		T_Week
%token		T_Xleave
%token		T_Year
%token          T_Flag     /* Not an actual token */
%token          T_Void     /* Not an actual token */
%token          T_EOC


/* NTP Simulator Tokens */
%token     T_Simulate
%token     T_Beep_Delay
%token     T_Sim_Duration
%token     T_Server_Offset
%token     T_Duration
%token     T_Freq_Offset
%token     T_Wander
%token     T_Jitter
%token     T_Prop_Delay
%token     T_Proc_Delay



/*** NON-TERMINALS ***/
%type   <VoidPtr>       access_control_flag
%type	<Queue>	        ac_flag_list
%type	<Address_node>	address
%type	<Queue>	        address_list
%type	<Integer>	boolean
%type	<Integer>	client_type
%type	<Attr_val>	crypto_command
%type	<Queue>		crypto_command_line
%type	<Queue>	        crypto_command_list
%type	<Attr_val>	discard_option
%type	<Queue>	        discard_option_list
%type	<Attr_val>	filegen_option
%type	<Queue>	        filegen_option_list
%type	<Integer>	filegen_type
%type	<Attr_val>	fudge_factor
%type	<Queue>	        fudge_factor_list
%type	<Queue>	        integer_list
%type	<Address_node>	ip_address
%type	<Attr_val>	log_config_command
%type	<Queue>	        log_config_list
%type	<Double>	number
%type	<Attr_val>	option
%type	<Queue>	        option_list
%type   <VoidPtr>       stat
%type	<Queue>	        stats_list
%type	<Queue>	        string_list
%type	<Attr_val>	system_option
%type	<Queue>	        system_option_list
%type	<Attr_val>	tinker_option
%type	<Queue>	        tinker_option_list
%type	<Attr_val>	tos_option
%type	<Queue>	        tos_option_list
%type	<Attr_val>	trap_option
%type	<Queue>	        trap_option_list
%type   <Set_var>       variable_assign

/* NTP Simulator non-terminals */
%type   <Queue>         sim_init_statement_list
%type   <Attr_val>      sim_init_statement
%type   <Queue>         sim_server_list
%type   <Sim_server>    sim_server
%type   <Double>        sim_server_offset
%type   <Address_node>  sim_server_name
%type   <Queue>         sim_act_list
%type   <Sim_script>    sim_act
%type   <Queue>         sim_act_stmt_list
%type   <Attr_val>      sim_act_stmt

%%

/* ntp.conf
 * Configuration File Grammar
 * --------------------------
 */

configuration
	:	command_list
	;

command_list
	:	command_list command T_EOC
	|	command T_EOC
                /* I will need to incorporate much more fine grained
                 * error messages. The following should suffice for
                 * the time being.
                 */
        |       error T_EOC
                {
					if (input_from_file == 1) {
			msyslog(LOG_ERR, "parse error %s line %d ignored\n",
                            ip_file->fname, ip_file->line_no);
			} else if (input_from_file != 0)
				msyslog(LOG_ERR,
				    "parse: bad boolean input flag\n");
                }
	;

command :	/* NULL STATEMENT */
	|	server_command
	|	other_mode_command
	|	authentication_command
	|	monitoring_command
	|	access_control_command
	|	orphan_mode_command
	|	fudge_command
	|	system_option_command
	|	tinker_command
	|	miscellaneous_command
        |       simulate_command
	;

/* Server Commands
 * ---------------
 */

server_command
        :	client_type address option_list
                    {
                        struct peer_node *my_node =  create_peer_node($1, $2, $3);
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
        |	client_type address
                    {
                        struct peer_node *my_node = create_peer_node($1, $2, NULL);
                        if (my_node)
                            enqueue(my_config.peers, my_node);
                    }
	;

client_type
        :	T_Server          { $$ = T_Server; }
        |       T_Pool            { $$ = T_Pool; }
	|	T_Peer            { $$ = T_Peer; }
        |	T_Broadcast       { $$ = T_Broadcast; }
	|	T_Manycastclient  { $$ = T_Manycastclient; }
	;

address
	:	ip_address   { $$ = $1; }
	|   T_IPv4_address T_String { $$ = create_address_node($2, AF_INET); }
	|	T_IPv6_address T_String { $$ = create_address_node($2, AF_INET6); }
	;

ip_address
    :	T_String { $$ = create_address_node($1, 0); }
	;

option_list
	:	option_list option         { $$ = enqueue($1, $2); }
	|	option  { $$ = enqueue_in_new_queue($1); }
	;

option
/*        :	/* Null Statement { $$ = NULL; } */
        :	T_Autokey      { $$ = create_attr_ival(T_Flag, FLAG_SKEY); }
	|	T_Bias number        { $$ = create_attr_dval(T_Bias, $2); }
	|	T_Burst        { $$ = create_attr_ival(T_Flag, FLAG_BURST); }
	|	T_Iburst       { $$ = create_attr_ival(T_Flag, FLAG_IBURST); }
	|	T_Key T_Integer      { $$ = create_attr_ival(T_Key, $2); }
	|	T_Minpoll T_Integer  { $$ = create_attr_ival(T_Minpoll, $2); }
	|	T_Maxpoll T_Integer  { $$ = create_attr_ival(T_Maxpoll, $2); }
	|	T_Noselect     { $$ = create_attr_ival(T_Flag, FLAG_NOSELECT);}
	|	T_Preempt      { $$ = create_attr_ival(T_Flag, FLAG_PREEMPT); }
	|	T_Prefer       { $$ = create_attr_ival(T_Flag, FLAG_PREFER); }
	|	T_True         { $$ = create_attr_ival(T_Flag, FLAG_TRUE); }
	|	T_Xleave       { $$ = create_attr_ival(T_Flag, FLAG_XLEAVE); }
	|	T_Ttl T_Integer     { $$ = create_attr_ival(T_Ttl, $2); }
        |       T_Mode T_Integer    { $$ = create_attr_ival(T_Mode, $2); }
	|	T_Version T_Integer { $$ = create_attr_ival(T_Version, $2); }
	;


/* Other Modes
 * (broadcastclient manycastserver multicastclient)
 * ------------------------------------------------
 */

other_mode_command
        :	T_Broadcastclient
                    { my_config.broadcastclient = SIMPLE; }
        |	T_Broadcastclient T_Novolley
                    { my_config.broadcastclient = NOVOLLEY;  }
	|	T_Manycastserver address_list
                    { append_queue(my_config.manycastserver, $2);  }
	|	T_Multicastclient address_list
                    { append_queue(my_config.multicastclient, $2);  }
	;



/* Authentication Commands
 * -----------------------
 */

authentication_command
	:	T_Autokey T_Integer
                    { my_config.auth.autokey = $2;  }
        |	T_ControlKey T_Integer
                    { my_config.auth.control_key = $2;  }
        |	T_Crypto crypto_command_line
			{ if (my_config.auth.crypto_cmd_list != NULL)
					append_queue(my_config.auth.crypto_cmd_list, $2);
				else
					my_config.auth.crypto_cmd_list = $2;
			cryptosw++;
			}
	|	T_Keys T_String
                    { my_config.auth.keys = $2;  }
	|	T_Keysdir T_String
                    { my_config.auth.keysdir = $2;  }
	|	T_Requestkey T_Integer
                    { my_config.auth.requested_key = $2;  }
	|	T_Trustedkey integer_list
                    { my_config.auth.trusted_key_list = $2;  }
	;

crypto_command_line
	:	crypto_command_list
	|	/* Null list */
		    { $$ = NULL; }
	;

crypto_command_list
        :	crypto_command_list crypto_command  { $$ = enqueue($1, $2); }
	|	crypto_command { $$ = enqueue_in_new_queue($1); }
	;

crypto_command
	:	T_Host	T_String
                    { $$ = create_attr_sval(CRYPTO_CONF_PRIV, $2); }
	|	T_Ident	T_String
                    { $$ = create_attr_sval(CRYPTO_CONF_IDENT, $2); }
	|	T_Pw T_String
                    { $$ = create_attr_sval(CRYPTO_CONF_PW, $2); }
	|	T_RandFile T_String
                    { $$ = create_attr_sval(CRYPTO_CONF_RAND, $2); }
	|	T_Revoke T_Integer
                    { my_config.auth.revoke = $2;  }
	|	T_Sign	T_String
                    { $$ = create_attr_sval(CRYPTO_CONF_SIGN, $2); }
	;


/* Orphan Mode Commands
 * --------------------
 */

orphan_mode_command
        :	T_Tos tos_option_list
                    { append_queue(my_config.orphan_cmds,$2);  }
	;

tos_option_list
        :	tos_option_list tos_option  { $$ = enqueue($1, $2); }
	|	tos_option { $$ = enqueue_in_new_queue($1); }
	;

tos_option
	:	T_Ceiling T_Integer
                    { $$ = create_attr_dval(PROTO_CEILING, (double)$2); }
	|	T_Floor T_Integer
                    { $$ = create_attr_dval(PROTO_FLOOR, (double)$2); }
	|	T_Cohort boolean
                    { $$ = create_attr_dval(PROTO_COHORT, (double)$2); }
	|	T_Orphan T_Integer
                    { $$ = create_attr_dval(PROTO_ORPHAN, (double)$2); }
	|	T_Mindist number
                    { $$ = create_attr_dval(PROTO_MINDISP, $2); }
	|	T_Maxdist number
                    { $$ = create_attr_dval(PROTO_MAXDIST, $2); }
	|	T_Minclock number
                    { $$ = create_attr_dval(PROTO_MINCLOCK, $2); }
	|	T_Maxclock number
                    { $$ = create_attr_dval(PROTO_MAXCLOCK, $2); }
	|	T_Minsane T_Integer
                    { $$ = create_attr_dval(PROTO_MINSANE, (double)$2); }
	|	T_Beacon T_Integer
                    { $$ = create_attr_dval(PROTO_BEACON, (double)$2); }
	|	T_Maxhop T_Integer
                    { $$ = create_attr_dval(PROTO_MAXHOP, (double)$2); }
	;
	

/* Monitoring Commands
 * -------------------
 */

monitoring_command
        :	T_Statistics stats_list
                    { append_queue(my_config.stats_list, $2);  }
	|	T_Statsdir T_String
                    { my_config.stats_dir = $2;  }
	|	T_Filegen stat filegen_option_list
                    {
                        enqueue(my_config.filegen_opts,
                                create_filegen_node($2, $3));
                    }
	;

stats_list
	:	stats_list stat  { $$ = enqueue($1, $2); }
	|	stat    { $$ = enqueue_in_new_queue($1); }
	;

stat
	:	T_Clockstats
                    { $$ = create_pval("clockstats"); }
	|	T_Cryptostats
                    { $$ = create_pval("cryptostats"); }
	|	T_Loopstats
                    { $$ = create_pval("loopstats"); }
	|	T_Peerstats
                    { $$ = create_pval("peerstats"); }
	|	T_Rawstats
                    { $$ = create_pval("rawstats"); }
        |	T_Sysstats
                    { $$ = create_pval("sysstats"); }
        |	T_Timingstats
                    { $$ = create_pval("timingstats"); }
	|	T_Protostats
		    { $$ = create_pval("protostats"); }
	;

filegen_option_list
	:	filegen_option_list filegen_option  { $$ = enqueue($1, $2); }
	|	filegen_option  { $$ = enqueue_in_new_queue($1); }
	;

filegen_option
	:	T_File T_String     { $$ = create_attr_sval(T_File, $2); }
	|	T_Type filegen_type { $$ = create_attr_ival(T_Type, $2); }
        |	T_Link      { $$ = create_attr_ival(T_Flag, T_Link); }
	|	T_Nolink    { $$ = create_attr_ival(T_Flag, T_Nolink); }
	|	T_Enable    { $$ = create_attr_ival(T_Flag, T_Enable); }
	|	T_Disable   { $$ = create_attr_ival(T_Flag, T_Disable); }
	;

filegen_type
	:	T_None     { $$ = FILEGEN_NONE; }
        |	T_Pid      { $$ = FILEGEN_PID; }
        |	T_Day      { $$ = FILEGEN_DAY; }
        |	T_Week     { $$ = FILEGEN_WEEK; }
        |	T_Month    { $$ = FILEGEN_MONTH; }
	|	T_Year     { $$ = FILEGEN_YEAR; }
	|	T_Age      { $$ = FILEGEN_AGE; }
	;


/* Access Control Commands
 * -----------------------
 */

access_control_command
        :	T_Discard discard_option_list
                    {   append_queue(my_config.discard_opts, $2); }
        |	T_Restrict address ac_flag_list
                    {
                        enqueue(my_config.restrict_opts,
                                create_restrict_node($2, NULL, $3, ip_file->line_no));
                    }
        |       T_Restrict T_Default ac_flag_list
                    {
                        enqueue(my_config.restrict_opts,
                                create_restrict_node(NULL, NULL, $3, ip_file->line_no));
                    }
	|	T_Restrict ip_address T_Mask ip_address ac_flag_list
                    {
                        enqueue(my_config.restrict_opts,
                                create_restrict_node($2, $4, $5, ip_file->line_no));
                    }
	;

ac_flag_list
        :	/* Null statement */ { $$ = create_queue(); }
        |       ac_flag_list access_control_flag  { $$ = enqueue($1, $2); }
	;

access_control_flag
	:	T_Flake        { $$ = create_ival(RES_TIMEOUT); }
	|	T_Ignore       { $$ = create_ival(RES_IGNORE); }
        |	T_Kod          { $$ = create_ival(RES_KOD); }
	|	T_Limited      { $$ = create_ival(RES_LIMITED); }
	|	T_Lowpriotrap  { $$ = create_ival(RES_LPTRAP); }
	|	T_Nomodify     { $$ = create_ival(RES_NOMODIFY); }
	|	T_Nopeer       { $$ = create_ival(RES_NOPEER); }
	|	T_Noquery      { $$ = create_ival(RES_NOQUERY); }
	|	T_Noserve      { $$ = create_ival(RES_DONTSERVE); }
	|	T_Notrap       { $$ = create_ival(RES_NOTRAP); }
	|	T_Notrust      { $$ = create_ival(RES_DONTTRUST); }
	|	T_Ntpport      { $$ = create_ival(RESM_NTPONLY); }
        |	T_Version      { $$ = create_ival(RES_VERSION); }
	;

discard_option_list
	:	discard_option_list discard_option  { $$ = enqueue($1, $2); }
	|	discard_option  { $$ = enqueue_in_new_queue($1); }
	;

discard_option
        :	T_Average T_Integer   { $$ = create_attr_ival(T_Average, $2); }
	|	T_Minimum T_Integer   { $$ = create_attr_ival(T_Minimum, $2); }
	|	T_Monitor T_Integer   { $$ = create_attr_ival(T_Monitor, $2); }
	;

/* Fudge Commands
 * --------------
 */

fudge_command
	:	T_Fudge address fudge_factor_list
                    { enqueue(my_config.fudge, create_addr_opts_node($2, $3));  }
	;

fudge_factor_list
	:	fudge_factor_list fudge_factor  { enqueue($1, $2); }
	|	fudge_factor  { $$ = enqueue_in_new_queue($1); }
	;
	
fudge_factor
	:	T_Time1 number
                    { $$ = create_attr_dval(CLK_HAVETIME1, $2); }
	|	T_Time2 number
                    { $$ = create_attr_dval(CLK_HAVETIME2, $2); }
	|	T_Stratum T_Integer
                    { $$ = create_attr_ival(CLK_HAVEVAL1,  $2); }
	|	T_Refid T_String
                    { $$ = create_attr_sval(CLK_HAVEVAL2,  $2); }
        |	T_Flag1 boolean
                    { $$ = create_attr_ival(CLK_HAVEFLAG1, $2); }
	|	T_Flag2	boolean
                    { $$ = create_attr_ival(CLK_HAVEFLAG2, $2); }
	|	T_Flag3	boolean
                    { $$ = create_attr_ival(CLK_HAVEFLAG3, $2); }
        |	T_Flag4 boolean
                    { $$ = create_attr_ival(CLK_HAVEFLAG4, $2); }
	;

/* Command for System Options
 * --------------------------
 */

system_option_command
        :	T_Enable system_option_list
                    { append_queue(my_config.enable_opts,$2);  }
	|	T_Disable system_option_list
                    { append_queue(my_config.disable_opts,$2);  }
	;

system_option_list
        :	system_option_list system_option  { $$ = enqueue($1, $2); }
	|	system_option  { $$ = enqueue_in_new_queue($1); }
	;

system_option
        :	T_Auth      { $$ = create_attr_ival(T_Flag, PROTO_AUTHENTICATE); }
	|	T_Bclient   { $$ = create_attr_ival(T_Flag, PROTO_BROADCLIENT); }
	|	T_Calibrate { $$ = create_attr_ival(T_Flag, PROTO_CAL); }
	|	T_Kernel    { $$ = create_attr_ival(T_Flag, PROTO_KERNEL); }
	|	T_Monitor   { $$ = create_attr_ival(T_Flag, PROTO_MONITOR); }
	|	T_Ntp       { $$ = create_attr_ival(T_Flag, PROTO_NTP); }
/*	|	T_Pps  */
	|	T_Stats     { $$ = create_attr_ival(T_Flag, PROTO_FILEGEN); }
	;

/* Tinker Commands
 * ---------------
 */

tinker_command
        :	T_Tinker tinker_option_list  { append_queue(my_config.tinker, $2);  }
	;

tinker_option_list
	:	tinker_option_list tinker_option  { $$ = enqueue($1, $2); }
	|	tinker_option { $$ = enqueue_in_new_queue($1); }
	;

tinker_option
        :	T_Allan number      { $$ = create_attr_dval(LOOP_ALLAN, $2); }
	|	T_Dispersion number { $$ = create_attr_dval(LOOP_PHI, $2); }
	|	T_Freq number       { $$ = create_attr_dval(LOOP_FREQ, $2); }
	|	T_Huffpuff number   { $$ = create_attr_dval(LOOP_HUFFPUFF, $2); }
	|	T_Panic number      { $$ = create_attr_dval(LOOP_PANIC, $2); }
	|	T_Step number       { $$ = create_attr_dval(LOOP_MAX, $2); }
	|	T_Stepout number    { $$ = create_attr_dval(LOOP_MINSTEP, $2); }
	;

/* Miscellaneous Commands
 * ----------------------
 */

miscellaneous_command
        :	T_Includefile T_String command
                {
                    if (curr_include_level >= MAXINCLUDELEVEL) {
                        fprintf(stderr, "getconfig: Maximum include file level exceeded.\n");
                        msyslog(LOG_ERR, "getconfig: Maximum include file level exceeded.");
                    }
                    else {
                        fp[curr_include_level + 1] = F_OPEN(FindConfig($2), "r");
                        if (fp[curr_include_level + 1] == NULL) {
                            fprintf(stderr, "getconfig: Couldn't open <%s>\n", FindConfig($2));
                            msyslog(LOG_ERR, "getconfig: Couldn't open <%s>", FindConfig($2));
                        }
                        else
                            ip_file = fp[++curr_include_level];
                    }
                }
	|	T_End
                {
                    while (curr_include_level != -1)
                        FCLOSE(fp[curr_include_level--]);
                }

        |	T_Broadcastdelay number
                    { enqueue(my_config.vars, create_attr_dval(T_Broadcastdelay, $2));  }
	|	T_Calldelay T_Integer
                    { enqueue(my_config.vars, create_attr_ival(T_Calldelay, $2));  }
	|	T_Tick number
                    { enqueue(my_config.vars, create_attr_dval(T_Tick, $2));  }
	|	T_Driftfile drift_parm
         { /* Null action, possibly all null parms */ }
	|	T_Leapfile T_String
		{ enqueue(my_config.vars, create_attr_sval(T_Leapfile, $2)); }

	|	T_Pidfile T_String
                    { enqueue(my_config.vars, create_attr_sval(T_Pidfile, $2));  }
	|	T_Logfile T_String
                    { enqueue(my_config.vars, create_attr_sval(T_Logfile, $2));  }
	|	T_Automax T_Integer
                    { enqueue(my_config.vars, create_attr_ival(T_Automax, $2));  }

        |	T_Logconfig log_config_list
                    { append_queue(my_config.logconfig, $2);  }
	|	T_Phone string_list
                    { append_queue(my_config.phone, $2);  }
	|	T_Setvar variable_assign
                    { enqueue(my_config.setvar, $2);  }
	|	T_Trap ip_address trap_option_list
                    { enqueue(my_config.trap, create_addr_opts_node($2, $3));  }
	|	T_Ttl integer_list
                    { append_queue(my_config.ttl, $2); }
	|	T_Qos T_String
                    { enqueue(my_config.qos, create_attr_sval(T_Qos, $2)); }
	;	
drift_parm
	:	T_String
		{ enqueue(my_config.vars, create_attr_sval(T_Driftfile, $1)); }
	|   T_String T_Double
		{ enqueue(my_config.vars, create_attr_dval(T_WanderThreshold, $2));
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, $1)); }
	|	{ /* Null driftfile,  indicated by null string "\0" */
		  enqueue(my_config.vars, create_attr_sval(T_Driftfile, "\0")); }
	;

variable_assign
    :	T_String '=' T_String T_Default
                    { $$ = create_setvar_node($1, $3, DEF); }
	|	T_String '=' T_String
                    { $$ = create_setvar_node($1, $3, 0); }
	;


trap_option_list
	:	trap_option_list trap_option { $$ = enqueue($1, $2); }
	|	trap_option  { $$ = enqueue_in_new_queue($1); }
	;

trap_option
        :	T_Port T_Integer     { $$ = create_attr_ival(T_Port, $2); }
	|	T_Interface ip_address { $$ = create_attr_pval(T_Interface, $2); }
	;

	
log_config_list
	:	log_config_list log_config_command { $$ = enqueue($1, $2); }
	|	log_config_command  { $$ = enqueue_in_new_queue($1); }
	;

log_config_command
        :       T_String
                {
                    char prefix = $1[0];
                    char *type = &($1[1]);
                    if (prefix != '+' && prefix != '-' && prefix != '=') {
                        yyerror("Logconfig prefix is not '+', '-' or '='\n");
                    }
                    else
                        $$ = create_attr_sval(prefix, type);
                }
	;

/* Miscellaneous Rules
 * -------------------
 */

integer_list
        :	integer_list T_Integer  { $$ = enqueue($1, create_ival($2)); }
        |	T_Integer { $$ = enqueue_in_new_queue(create_ival($1)); }
	;

string_list
        :	string_list T_String   { $$ = enqueue($1, create_pval($2)); }
        |	T_String { $$ = enqueue_in_new_queue(create_pval($1)); }
	;

address_list
        :	address_list address  { $$ = enqueue($1, $2); }
        |	address { $$ = enqueue_in_new_queue($1); }
	;

boolean
        :	T_Integer
                {
                    if ($1 != 0 && $1 != 1) {
                        yyerror("Integer value is not boolean (0 or 1). Assuming 1");
                        $$ = 1;
                    }
                    else
                        $$ = $1;
                }
        |	T_True     { $$ = 1; }
        |	T_False    { $$ = 0; }
	;

number
        :	T_Integer { $$ = (double)$1; }
        |	T_Double  { $$ = $1; }
	;

/* Simulator Configuration Commands
 * --------------------------------
 */

simulate_command
        :    sim_conf_start '{' sim_init_statement_list sim_server_list '}'
             {
                 my_config.sim_details = create_sim_node($3, $4);

                 /* Reset the old_config_style variable */
                 old_config_style = 1;
             }
        ;

/* The following is a terrible hack to get the configuration file to
 * treat newlines as whitespace characters within the simulation.
 * This is needed because newlines are significant in the rest of the
 * configuration file.
 */
sim_conf_start
        :    T_Simulate { old_config_style = 0; }
        ;

sim_init_statement_list
        :    sim_init_statement_list sim_init_statement T_EOC { $$ = enqueue($1, $2); }
        |    sim_init_statement T_EOC                         { $$ = enqueue_in_new_queue($1); }
        ;

sim_init_statement
        :    T_Beep_Delay '=' number   { $$ = create_attr_dval(T_Beep_Delay, $3); }
        |    T_Sim_Duration '=' number { $$ = create_attr_dval(T_Sim_Duration, $3); }
        ;

sim_server_list
        :    sim_server_list sim_server { $$ = enqueue($1, $2); }
        |    sim_server                 { $$ = enqueue_in_new_queue($1); }
        ;

sim_server
        :    sim_server_name '{' sim_server_offset sim_act_list '}'
                 { $$ = create_sim_server($1, $3, $4); }
        ;

sim_server_offset
        :    T_Server_Offset '=' number T_EOC { $$ = $3; }
        ;

sim_server_name
        :    T_Server '=' address { $$ = $3; }
        ;

sim_act_list
        :    sim_act_list  sim_act { $$ = enqueue($1, $2); }
        |    sim_act               { $$ = enqueue_in_new_queue($1); }
        ;

sim_act
        :    T_Duration '=' number '{' sim_act_stmt_list '}'
                 { $$ = create_sim_script_info($3, $5); }
        ;

sim_act_stmt_list
        :    sim_act_stmt_list sim_act_stmt T_EOC { $$ = enqueue($1, $2); }
        |    sim_act_stmt T_EOC                   { $$ = enqueue_in_new_queue($1); }
        ;

sim_act_stmt
        :    T_Freq_Offset '=' number
                 { $$ = create_attr_dval(T_Freq_Offset, $3); }
        |    T_Wander '=' number
                 { $$ = create_attr_dval(T_Wander, $3); }
        |    T_Jitter '=' number
                 { $$ = create_attr_dval(T_Jitter, $3); }
        |    T_Prop_Delay '=' number
                 { $$ = create_attr_dval(T_Prop_Delay, $3); }
        |    T_Proc_Delay '=' number
                 { $$ = create_attr_dval(T_Proc_Delay, $3); }
        ;


%%

/* KEYWORDS
 * --------
 */

void yyerror (char *msg)
{
    int retval;
    if (input_from_file)
        msyslog(LOG_ERR, "%s\n", msg);
    else {
        /* Save the error message in the correct buffer */
        retval = snprintf(remote_config.err_msg + remote_config.err_pos,
                          MAXLINE - remote_config.err_pos,
                          "%s\n", msg);

        /* Increment the value of err_pos */
        if (retval > 0)
            remote_config.err_pos += retval;

        /* Increment the number of errors */
        ++remote_config.no_errors;
    }
}


/* Initial Testing function -- ignore
int main(int argc, char *argv[])
{
    ip_file = FOPEN(argv[1], "r");
    if (!ip_file) {
        fprintf(stderr, "ERROR!! Could not open file: %s\n", argv[1]);
    }
    key_scanner = create_keyword_scanner(keyword_list);
    print_keyword_scanner(key_scanner, 0);
    yyparse();
    return 0;
}
*/

