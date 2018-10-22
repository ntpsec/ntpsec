/* ntp_parser.y
 *
 * The parser for the NTP configuration file.
 *
 * Written By:	Sachin Kamboj
 *		University of Delaware
 *		Newark, DE 19711
 * Copyright (c) 2006
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-clause
 */

%{
  #include "config.h"

  #include "ntp.h"
  #include "ntpd.h"
  #include "ntp_machine.h"
  #include "ntp_stdlib.h"
  #include "ntp_filegen.h"
  #include "ntp_scanner.h"
  #include "ntp_config.h"

  #define YYMALLOC	emalloc
  #define YYFREE	free
  #define YYERROR_VERBOSE
  #define YYMAXDEPTH	1000	/* stop the madness sooner */
  void yyerror(const char *msg);
%}

/*
 * Enable generation of token names array even without YYDEBUG.
 * We access via token_name() defined below.
 */
%token-table

%union {
	char *			String;
	double			Double;
	int			Integer;
	unsigned		U_int;
	gen_fifo *		Generic_fifo;
	attr_val *		Attr_val;
	attr_val_fifo *		Attr_val_fifo;
	int_fifo *		Int_fifo;
	string_fifo *		String_fifo;
	address_node *		Address_node;
	address_fifo *		Address_fifo;
	setvar_node *		Set_var;
}

/* Terminals (do not appear left of colon) */
%token	<Integer>	T_Age
%token	<Integer>	T_All
%token	<Integer>	T_Allan
%token	<Integer>	T_Allpeers
%token	<Integer>	T_Auth
%token	<Integer>	T_Average
%token	<Integer>	T_Baud
%token	<Integer>	T_Bias
%token	<Integer>	T_Burst
%token	<Integer>	T_Calibrate
%token	<Integer>	T_Ceiling
%token	<Integer>	T_Clock
%token	<Integer>	T_Clockstats
%token	<Integer>	T_Cohort
%token	<Integer>	T_ControlKey
%token	<Integer>	T_Ctl
%token	<Integer>	T_Day
%token	<Integer>	T_Default
%token	<Integer>	T_Disable
%token	<Integer>	T_Discard
%token	<Integer>	T_Dispersion
%token	<Double>	T_Double		/* Not a token */
%token	<Integer>	T_Driftfile
%token	<Integer>	T_Drop
%token	<Integer>	T_Dscp
%token	<Integer>	T_Ellipsis	/* "..." not "ellipsis" */
%token	<Integer>	T_Enable
%token	<Integer>	T_End
%token	<Integer>	T_False
%token	<Integer>	T_File
%token	<Integer>	T_Filegen
%token	<Integer>	T_Filenum
%token	<Integer>	T_Flag1
%token	<Integer>	T_Flag2
%token	<Integer>	T_Flag3
%token	<Integer>	T_Flag4
%token	<Integer>	T_Flake
%token	<Integer>	T_Floor
%token	<Integer>	T_Freq
%token	<Integer>	T_Fudge
%token	<Integer>	T_Holdover
%token	<Integer>	T_Huffpuff
%token	<Integer>	T_Iburst
%token	<Integer>	T_Ignore
%token	<Integer>	T_Incalloc
%token	<Integer>	T_Incmem
%token	<Integer>	T_Initalloc
%token	<Integer>	T_Initmem
%token	<Integer>	T_Includefile
%token	<Integer>	T_Integer		/* Not a token, used as tag */
%token	<Integer>	T_Interface
%token	<Integer>	T_Intrange		/* Not a token, used as tag */
%token	<Integer>	T_Io
%token	<Integer>	T_Ipv4
%token	<Integer>	T_Ipv4_flag
%token	<Integer>	T_Ipv6
%token	<Integer>	T_Ipv6_flag
%token	<Integer>	T_Kernel
%token	<Integer>	T_Key
%token	<Integer>	T_Keys
%token	<Integer>	T_Kod
%token	<Integer>	T_Mssntp
%token	<Integer>	T_Leapfile
%token	<Integer>	T_Leapsmearinterval
%token	<Integer>	T_Limited
%token	<Integer>	T_Link
%token	<Integer>	T_Listen
%token	<Integer>	T_Logconfig
%token	<Integer>	T_Logfile
%token	<Integer>	T_Loopstats
%token	<Integer>	T_Mask
%token	<Integer>	T_Maxage
%token	<Integer>	T_Maxclock
%token	<Integer>	T_Maxdepth
%token	<Integer>	T_Maxdisp
%token	<Integer>	T_Maxdist
%token	<Integer>	T_Maxmem
%token	<Integer>	T_Maxpoll
%token	<Integer>	T_Mdnstries
%token	<Integer>	T_Mem
%token	<Integer>	T_Memlock
%token	<Integer>	T_Minage
%token	<Integer>	T_Minclock
%token	<Integer>	T_Mindepth
%token	<Integer>	T_Mindist
%token	<Integer>	T_Minimum
%token	<Integer>	T_Minpoll
%token	<Integer>	T_Minsane
%token	<Integer>	T_Mode
%token	<Integer>	T_Monitor
%token	<Integer>	T_Month
%token	<Integer>	T_Mru
%token	<Integer>	T_Nic
%token	<Integer>	T_Nolink
%token	<Integer>	T_Nomodify
%token	<Integer>	T_Nomrulist
%token	<Integer>	T_None
%token	<Integer>	T_Nonvolatile
%token	<Integer>	T_Nopeer
%token	<Integer>	T_Noquery
%token	<Integer>	T_Noselect
%token	<Integer>	T_Noserve
%token	<Integer>	T_Notrap
%token	<Integer>	T_Notrust
%token	<Integer>	T_Ntp
%token	<Integer>	T_Ntpport
%token	<Integer>	T_NtpSignDsocket
%token	<Integer>	T_Orphan
%token	<Integer>	T_Orphanwait
%token	<Integer>	T_Panic
%token	<Integer>	T_Path
%token	<Integer>	T_Peer
%token	<Integer>	T_Peerstats
%token	<Integer>	T_Phone
%token	<Integer>	T_Pid
%token	<Integer>	T_Pidfile
%token	<Integer>	T_Pool
%token	<Integer>	T_Ppspath
%token	<Integer>	T_Prefer
%token	<Integer>	T_Protostats
%token	<Integer>	T_Rawstats
%token	<Integer>	T_Refclock
%token	<Integer>	T_Refid
%token	<Integer>	T_Requestkey
%token	<Integer>	T_Reset
%token	<Integer>	T_Restrict
%token	<Integer>	T_Rlimit
%token	<Integer>	T_Saveconfigdir
%token	<Integer>	T_Server
%token	<Integer>	T_Setvar
%token	<Integer>	T_Source
%token	<Integer>	T_Stacksize
%token	<Integer>	T_Statistics
%token	<Integer>	T_Stats
%token	<Integer>	T_Statsdir
%token	<Integer>	T_Step
%token	<Integer>	T_Stepback
%token	<Integer>	T_Stepfwd
%token	<Integer>	T_Stepout
%token	<Integer>	T_Stratum
%token	<Integer>	T_Subtype
%token	<String>	T_String		/* Not a token */
%token	<Integer>	T_Sys
%token	<Integer>	T_Sysstats
%token	<Integer>	T_Tick
%token	<Integer>	T_Time1
%token	<Integer>	T_Time2
%token	<Integer>	T_Timer
%token	<Integer>	T_Timingstats
%token	<Integer>	T_Tinker
%token	<Integer>	T_Tos
%token	<Integer>	T_True
%token	<Integer>	T_Trustedkey
%token	<Integer>	T_Type
%token	<Integer>	T_U_int			/* Not a token */
%token	<Integer>	T_Unit
%token	<Integer>	T_Unconfig
%token	<Integer>	T_Unpeer
%token	<Integer>	T_Unrestrict
%token	<Integer>	T_Usestats
%token	<Integer>	T_Version
%token	<Integer>	T_WanderThreshold	/* Not a token, used as tag */
%token	<Integer>	T_Week
%token	<Integer>	T_Wildcard
%token	<Integer>	T_Year
%token	<Integer>	T_Flag			/* Not a token, used as tag */
%token	<Integer>	T_EOC

/*** NON-TERMINALS ***/
%type	<Integer>	access_control_flag
%type	<Int_fifo>	ac_flag_list
%type	<Address_node>	address
%type	<Integer>	address_fam
%type	<Integer>	boolean
%type	<Integer>	client_type
%type	<Integer>	counter_set_keyword
%type	<Int_fifo>	counter_set_list
%type	<Attr_val>	discard_option
%type	<Integer>	discard_option_keyword
%type	<Attr_val_fifo>	discard_option_list
%type	<Integer>	enable_disable
%type	<Attr_val>	filegen_option
%type	<Attr_val_fifo>	filegen_option_list
%type	<Integer>	filegen_type
%type	<Attr_val>	fudge_factor
%type	<Integer>	fudge_factor_bool_keyword
%type	<Integer>	fudge_factor_dbl_keyword
%type	<Attr_val_fifo>	fudge_factor_list
%type	<Attr_val_fifo>	integer_list_range
%type	<Attr_val>	integer_list_range_elt
%type	<Attr_val>	integer_range
%type	<Integer>	nic_rule_action
%type	<Integer>	interface_command
%type	<Integer>	interface_nic
%type	<Address_node>	ip_address
%type	<Integer>	link_nolink
%type	<Attr_val>	log_config_command
%type	<Attr_val_fifo>	log_config_list
%type	<Integer>	misc_cmd_dbl_keyword
%type	<Integer>	misc_cmd_int_keyword
%type	<Integer>	misc_cmd_str_keyword
%type	<Integer>	misc_cmd_str_lcl_keyword
%type	<Attr_val>	mru_option
%type	<Integer>	mru_option_keyword
%type	<Attr_val_fifo>	mru_option_list
%type	<Integer>	nic_rule_class
%type	<Double>	number
%type	<Attr_val>	option
%type	<Attr_val>	option_flag
%type	<Integer>	option_flag_keyword
%type	<Attr_val_fifo>	option_list
%type	<Attr_val>	option_boolean
%type	<Integer>	option_bool_keyword
%type	<Attr_val>	option_double
%type	<Integer>	option_double_keyword
%type	<Attr_val>	option_int
%type	<Integer>	option_int_keyword
%type	<Attr_val>	option_string
%type	<Integer>	optional_unit
%type	<Integer>	reset_command
%type	<Integer>	rlimit_option_keyword
%type	<Attr_val>	rlimit_option
%type	<Attr_val_fifo>	rlimit_option_list
%type	<Integer>	stat
%type	<Int_fifo>	stats_list
%type	<String_fifo>	string_list
%type	<Attr_val>	system_option
%type	<Integer>	system_option_flag_keyword
%type	<Integer>	system_option_local_flag_keyword
%type	<Attr_val_fifo>	system_option_list
%type	<Integer>	t_default_or_zero
%type	<Integer>	tinker_option_keyword
%type	<Attr_val>	tinker_option
%type	<Attr_val_fifo>	tinker_option_list
%type	<Attr_val>	tos_option
%type	<Integer>	tos_option_dbl_keyword
%type	<Integer>	tos_option_int_keyword
%type	<Attr_val_fifo>	tos_option_list
%type	<Integer>	unpeer_keyword
%type	<Set_var>	variable_assign
%type	<Integer>	restrict_prefix

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
	|	error T_EOC
		{
			/* I will need to incorporate much more fine grained
			 * error messages. The following should suffice for
			 * the time being.
			 */
			struct FILE_INFO * ip_ctx = lex_current();
			msyslog(LOG_ERR,
				"CONFIG: syntax error in %s line %d, column %d",
				ip_ctx->fname,
				ip_ctx->errpos.nline,
				ip_ctx->errpos.ncol);
		}
	;

command :	/* NULL STATEMENT */
	|	server_command
	|	unpeer_command
	|	other_mode_command
	|	authentication_command
	|	monitoring_command
	|	access_control_command
	|	orphan_mode_command
	|	fudge_command
	|	refclock_command
	|	rlimit_command
	|	system_option_command
	|	tinker_command
	|	miscellaneous_command
	;

/* Server Commands
 * ---------------
 */

server_command
	:	client_type address option_list
		{
			peer_node *my_node;

			my_node = create_peer_node($1, $2, $3);
			APPEND_G_FIFO(cfgt.peers, my_node);
		}
	;

client_type
	:	T_Server
	|	T_Pool
	|	T_Peer
	;

address
	:	ip_address
	|	address_fam T_String
			{ $$ = create_address_node($2, $1); }
	;

ip_address
	:	T_String
			{ $$ = create_address_node($1, AF_UNSPEC); }
	;

address_fam
	:	T_Ipv4_flag
			{ $$ = AF_INET; }
	|	T_Ipv6_flag
			{ $$ = AF_INET6; }
	;

option_list
	:	/* empty list */
			{ $$ = NULL; }
	|	option_list option
		{
			$$ = $1;
			APPEND_G_FIFO($$, $2);
		}
	;

option
	:	option_flag
	|	option_int
	|	option_double
	|	option_string
	|	option_boolean
	;

option_flag
	:	option_flag_keyword
			{ $$ = create_attr_ival(T_Flag, $1); }
	;

option_flag_keyword
	:	T_Burst
	|	T_Iburst
	|	T_Noselect
	|	T_Prefer
	|	T_True
	;

option_int
	:	option_int_keyword T_Integer
			{ $$ = create_attr_ival($1, $2); }
	|	option_int_keyword T_U_int
			{ $$ = create_attr_uval($1, (unsigned int)$2); }
	|	T_Stratum T_Integer
		{
			if ($2 >= 0 && $2 <= STRATUM_UNSPEC) {
				$$ = create_attr_ival($1, $2);
			} else {
				$$ = NULL;
				yyerror("fudge factor: stratum value out of bounds, ignored");
			}
		}
	;

option_int_keyword
	:	T_Key
	|	T_Minpoll
	|	T_Maxpoll
	|	T_Mode
	|	T_Subtype
	|	T_Version
	|	T_Baud
	|	T_Holdover
	;

option_double
	:	option_double_keyword number
			{ $$ = create_attr_dval($1, $2); }
	;

option_boolean
	:	option_bool_keyword boolean
			{ $$ = create_attr_ival($1, $2); }
	;

option_string
	:	T_Refid T_String
			{ $$ = create_attr_sval($1, $2); }
	|	T_Path T_String
			{ $$ = create_attr_sval($1, $2); }
	|	T_Ppspath T_String
			{ $$ = create_attr_sval($1, $2); }
	;

option_double_keyword
	:	T_Bias
	|	T_Time1
	|	T_Time2
	;

option_bool_keyword
	:	T_Flag1
	|	T_Flag2
	|	T_Flag3
	|	T_Flag4
	;


/* unpeer commands
 * ---------------
 */

unpeer_command
	:	unpeer_keyword address
		{
			unpeer_node *my_node;

			my_node = create_unpeer_node($2);
			if (my_node)
				APPEND_G_FIFO(cfgt.unpeers, my_node);
		}
	|	unpeer_keyword T_Clock T_String optional_unit
		{
#ifdef REFCLOCK
			unpeer_node *my_node;

			my_node = create_unpeer_node(addr_from_typeunit($3, $4));
			if (my_node)
				APPEND_G_FIFO(cfgt.unpeers, my_node);
#else
			yyerror("no refclock support was compiled in.");
#endif /* REFCLOCK */
		}
	;
unpeer_keyword
	:	T_Unconfig
	|	T_Unpeer
	;


/* Other Modes
 * ------------------------------------------------
 */

other_mode_command
	:	T_Mdnstries T_Integer
			{ cfgt.mdnstries = $2; }
	;



/* Authentication Commands
 * -----------------------
 */

authentication_command
	:	T_ControlKey T_Integer
			{ cfgt.auth.control_key = $2; }
	|	T_Keys T_String
			{ cfgt.auth.keys = $2; }
	|	T_Requestkey T_Integer
			{
			    msyslog(LOG_WARNING,
				    "CONFIG: requestkey is a no-op because "
				    "ntpdc has been removed.");
			}
	|	T_Trustedkey integer_list_range
		{ CONCAT_G_FIFOS(cfgt.auth.trusted_key_list, $2); }
	|	T_NtpSignDsocket T_String
			{ cfgt.auth.ntp_signd_socket = $2; }
	;


/* Orphan Mode Commands
 * --------------------
 */

orphan_mode_command
	:	T_Tos tos_option_list
			{ CONCAT_G_FIFOS(cfgt.orphan_cmds, $2); }
	;

tos_option_list
	:	tos_option_list tos_option
		{
			$$ = $1;
			APPEND_G_FIFO($$, $2);
		}
	|	tos_option
		{
			$$ = NULL;
			APPEND_G_FIFO($$, $1);
		}
	;

tos_option
	:	tos_option_int_keyword T_Integer
			{ $$ = create_attr_dval($1, (double)$2); }
	|	tos_option_dbl_keyword number
			{ $$ = create_attr_dval($1, $2); }
	|	T_Cohort boolean
			{ $$ = create_attr_dval($1, (double)$2); }
	;

tos_option_int_keyword
	:	T_Ceiling
	|	T_Floor
	|	T_Orphan
	|	T_Orphanwait
	|	T_Minsane
	;

tos_option_dbl_keyword
	:	T_Mindist
	|	T_Maxdisp
	|	T_Maxdist
	|	T_Minclock
	|	T_Maxclock
	;


/* Monitoring Commands
 * -------------------
 */

monitoring_command
	:	T_Statistics stats_list
			{ CONCAT_G_FIFOS(cfgt.stats_list, $2); }
	|	T_Statsdir T_String
		{
			if (lex_from_file()) {
				cfgt.stats_dir = $2;
			} else {
				YYFREE($2);
				yyerror("statsdir remote configuration ignored");
			}
		}
	|	T_Filegen stat filegen_option_list
		{
			filegen_node *fgn;

			fgn = create_filegen_node($2, $3);
			APPEND_G_FIFO(cfgt.filegen_opts, fgn);
		}
	;

stats_list
	:	stats_list stat
		{
			$$ = $1;
			APPEND_G_FIFO($$, create_int_node($2));
		}
	|	stat
		{
			$$ = NULL;
			APPEND_G_FIFO($$, create_int_node($1));
		}
	;

stat
	:	T_Clockstats
	|	T_Loopstats
	|	T_Peerstats
	|	T_Rawstats
	|	T_Sysstats
	|	T_Protostats
	|	T_Timingstats
	|	T_Usestats
	;

filegen_option_list
	:	/* empty list */
			{ $$ = NULL; }
	|	filegen_option_list filegen_option
		{
			$$ = $1;
			APPEND_G_FIFO($$, $2);
		}
	;

filegen_option
	:	T_File T_String
		{
			if (lex_from_file()) {
				$$ = create_attr_sval($1, $2);
			} else {
				$$ = NULL;
				YYFREE($2);
				yyerror("filegen file remote config ignored");
			}
		}
	|	T_Type filegen_type
		{
			if (lex_from_file()) {
				$$ = create_attr_ival($1, $2);
			} else {
				$$ = NULL;
				yyerror("filegen type remote config ignored");
			}
		}
	|	link_nolink
		{
			const char *err;

			if (lex_from_file()) {
				$$ = create_attr_ival(T_Flag, $1);
			} else {
				$$ = NULL;
				if (T_Link == $1)
					err = "filegen link remote config ignored";
				else
					err = "filegen nolink remote config ignored";
				yyerror(err);
			}
		}
	|	enable_disable
			{ $$ = create_attr_ival(T_Flag, $1); }
	;

link_nolink
	:	T_Link
	|	T_Nolink
	;

enable_disable
	:	T_Enable
	|	T_Disable
	;

filegen_type
	:	T_None
	|	T_Pid
	|	T_Day
	|	T_Week
	|	T_Month
	|	T_Year
	|	T_Age
	;


/* Access Control Commands
 * -----------------------
 */

restrict_prefix
	: T_Restrict
	| T_Unrestrict
	;

access_control_command
	:	T_Discard discard_option_list
		{
			CONCAT_G_FIFOS(cfgt.discard_opts, $2);
		}
	|	T_Mru mru_option_list
		{
			CONCAT_G_FIFOS(cfgt.mru_opts, $2);
		}
	|	restrict_prefix address ac_flag_list
		{
			restrict_node *rn;

			rn = create_restrict_node($1, $2, NULL, $3,
						  lex_current()->curpos.nline);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
	|	restrict_prefix ip_address T_Mask ip_address ac_flag_list
		{
			restrict_node *rn;

			rn = create_restrict_node($1, $2, $4, $5,
						  lex_current()->curpos.nline);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
	|	restrict_prefix T_Default ac_flag_list
		{
			restrict_node *rn;

			rn = create_restrict_node($1, NULL, NULL, $3,
						  lex_current()->curpos.nline);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
	|	restrict_prefix T_Ipv4_flag T_Default ac_flag_list
		{
			restrict_node *rn;

			rn = create_restrict_node($1,
				create_address_node(
					estrdup("0.0.0.0"),
					AF_INET),
				create_address_node(
					estrdup("0.0.0.0"),
					AF_INET),
				$4,
				lex_current()->curpos.nline);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
	|	restrict_prefix T_Ipv6_flag T_Default ac_flag_list
		{
			restrict_node *rn;

			rn = create_restrict_node($1,
				create_address_node(
					estrdup("::"),
					AF_INET6),
				create_address_node(
					estrdup("::"),
					AF_INET6),
				$4,
				lex_current()->curpos.nline);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
	|	restrict_prefix T_Source ac_flag_list
		{
			restrict_node *	rn;

			APPEND_G_FIFO($3, create_int_node($2));
			rn = create_restrict_node(
				$1, NULL, NULL, $3, lex_current()->curpos.nline);
			APPEND_G_FIFO(cfgt.restrict_opts, rn);
		}
	;

ac_flag_list
	:	/* empty list is allowed */
			{ $$ = NULL; }
	|	ac_flag_list access_control_flag
		{
			$$ = $1;
			APPEND_G_FIFO($$, create_int_node($2));
		}
	;

access_control_flag
	:	T_Flake
	|	T_Ignore
	|	T_Kod
	|	T_Mssntp
	|	T_Limited
	|	T_Nomodify
	|	T_Nomrulist
	|	T_Nopeer
	|	T_Noquery
	|	T_Noserve
	|	T_Notrap
	|	T_Notrust
	|	T_Ntpport
	|	T_Version
	;

discard_option_list
	:	discard_option_list discard_option
		{
			$$ = $1;
			APPEND_G_FIFO($$, $2);
		}
	|	discard_option
		{
			$$ = NULL;
			APPEND_G_FIFO($$, $1);
		}
	;

discard_option
	:	discard_option_keyword T_Integer
			{ $$ = create_attr_ival($1, $2); }
	;

discard_option_keyword
	:	T_Average
	|	T_Minimum
	|	T_Monitor
	;

mru_option_list
	:	mru_option_list mru_option
		{
			$$ = $1;
			APPEND_G_FIFO($$, $2);
		}
	|	mru_option
		{
			$$ = NULL;
			APPEND_G_FIFO($$, $1);
		}
	;

mru_option
	:	mru_option_keyword T_Integer
			{ $$ = create_attr_ival($1, $2); }
	;

mru_option_keyword
	:	T_Incalloc
	|	T_Incmem
	|	T_Initalloc
	|	T_Initmem
	|	T_Maxage
	|	T_Minage
	|	T_Maxdepth
	|	T_Maxmem
	|	T_Mindepth
	;

/* Fudge Commands
 * --------------
 */

fudge_command
	:	T_Fudge address fudge_factor_list
		{
			addr_opts_node *aon;

			aon = create_addr_opts_node($2, $3);
			APPEND_G_FIFO(cfgt.fudge, aon);
		}
	;

fudge_factor_list
	:	fudge_factor_list fudge_factor
		{
			$$ = $1;
			APPEND_G_FIFO($$, $2);
		}
	|	fudge_factor
		{
			$$ = NULL;
			APPEND_G_FIFO($$, $1);
		}
	;

fudge_factor
	:	fudge_factor_dbl_keyword number
			{ $$ = create_attr_dval($1, $2); }
	|	fudge_factor_bool_keyword boolean
			{ $$ = create_attr_ival($1, $2); }
	|	T_Stratum T_Integer
		{
			if ($2 >= 0 && $2 <= 16) {
				$$ = create_attr_ival($1, $2);
			} else {
				$$ = NULL;
				yyerror("fudge factor: stratum value not in [0..16], ignored");
			}
		}
	|	T_Refid T_String
			{ $$ = create_attr_sval($1, $2); }
	;

fudge_factor_dbl_keyword
	:	T_Time1
	|	T_Time2
	;

fudge_factor_bool_keyword
	:	T_Flag1
	|	T_Flag2
	|	T_Flag3
	|	T_Flag4
	;

/* Refclock Command
 * ----------------
 */

refclock_command
	:	T_Refclock T_String optional_unit option_list
		{
#ifdef REFCLOCK
			address_node *fakeaddr = addr_from_typeunit($2, $3);
			peer_node *my_node = create_peer_node(T_Server, fakeaddr, $4);
			APPEND_G_FIFO(cfgt.peers, my_node);
#endif /* REFCLOCK */
		}
	;

optional_unit
	:
			{$$ = 0;}
	|
		T_Unit T_Integer
			{$$ = $2;}
	;


/* rlimit Commands
 * ---------------
 */

rlimit_command
	:	T_Rlimit rlimit_option_list
			{ CONCAT_G_FIFOS(cfgt.rlimit, $2); }
	;

rlimit_option_list
	:	rlimit_option_list rlimit_option
		{
			$$ = $1;
			APPEND_G_FIFO($$, $2);
		}
	|	rlimit_option
		{
			$$ = NULL;
			APPEND_G_FIFO($$, $1);
		}
	;

rlimit_option
	:	rlimit_option_keyword T_Integer
			{ $$ = create_attr_ival($1, $2); }
	;

rlimit_option_keyword
	:	T_Memlock
	|	T_Stacksize
	|	T_Filenum
	;


/* Command for System Options
 * --------------------------
 */

system_option_command
	:	T_Enable system_option_list
			{ CONCAT_G_FIFOS(cfgt.enable_opts, $2); }
	|	T_Disable system_option_list
			{ CONCAT_G_FIFOS(cfgt.disable_opts, $2); }
	;

system_option_list
	:	system_option_list system_option
		{
			$$ = $1;
			APPEND_G_FIFO($$, $2);
		}
	|	system_option
		{
			$$ = NULL;
			APPEND_G_FIFO($$, $1);
		}
	;

system_option
	:	system_option_flag_keyword
			{ $$ = create_attr_ival(T_Flag, $1); }
	|	system_option_local_flag_keyword
		{
			if (lex_from_file()) {
				$$ = create_attr_ival(T_Flag, $1);
			} else {
				char err_str[128];

				$$ = NULL;
				snprintf(err_str, sizeof(err_str),
					 "enable/disable %s remote configuration ignored",
					 keyword($1));
				yyerror(err_str);
			}
		}
	;

system_option_flag_keyword
	:	T_Calibrate
	|	T_Kernel
	|	T_Monitor
	|	T_Ntp
	;

system_option_local_flag_keyword
	:	T_Stats
	;

/* Tinker Commands
 * ---------------
 */

tinker_command
	:	T_Tinker tinker_option_list
			{ CONCAT_G_FIFOS(cfgt.tinker, $2); }
	;

tinker_option_list
	:	tinker_option_list tinker_option
		{
			$$ = $1;
			APPEND_G_FIFO($$, $2);
		}
	|	tinker_option
		{
			$$ = NULL;
			APPEND_G_FIFO($$, $1);
		}
	;

tinker_option
	:	tinker_option_keyword number
			{ $$ = create_attr_dval($1, $2); }
	;

tinker_option_keyword
	:	T_Allan
	|	T_Dispersion
	|	T_Freq
	|	T_Huffpuff
	|	T_Panic
	|	T_Step
	|	T_Stepback
	|	T_Stepfwd
	|	T_Stepout
	|	T_Tick
	;


/* Miscellaneous Commands
 * ----------------------
 */

miscellaneous_command
	:	interface_command
	|	reset_command
	|	misc_cmd_dbl_keyword number
		{
			attr_val *av;

			av = create_attr_dval($1, $2);
			APPEND_G_FIFO(cfgt.vars, av);
		}
	|	misc_cmd_int_keyword T_Integer
		{
			attr_val *av;

			av = create_attr_ival($1, $2);
			APPEND_G_FIFO(cfgt.vars, av);
		}
	|	misc_cmd_str_keyword T_String
		{
			attr_val *av;

			av = create_attr_sval($1, $2);
			APPEND_G_FIFO(cfgt.vars, av);
		}
	|	misc_cmd_str_lcl_keyword T_String
		{
			char error_text[64];
			attr_val *av;

			if (lex_from_file()) {
				av = create_attr_sval($1, $2);
				APPEND_G_FIFO(cfgt.vars, av);
			} else {
				YYFREE($2);
				snprintf(error_text, sizeof(error_text),
					 "%s remote config ignored",
					 keyword($1));
				yyerror(error_text);
			}
		}
	|	T_Includefile T_String command
		{
			if (!lex_from_file()) {
				YYFREE($2); /* avoid leak */
				yyerror("remote includefile ignored");
				break;
			}
			if (lex_level() > MAXINCLUDELEVEL) {
				fprintf(stderr, "getconfig(): Maximum include file level exceeded.\n");
				msyslog(LOG_ERR, "CONFIG: Maximum include file level exceeded.");
			} else {
				const char * path = $2;
				if (!lex_push_file(path)) {
					fprintf(stderr, "getconfig(): Couldn't open <%s>\n", path);
					msyslog(LOG_ERR, "CONFIG: Couldn't open <%s>", path);
				}
			}
			YYFREE($2); /* avoid leak */
		}
	|	T_End
			{ lex_flush_stack(); }
	|	T_Driftfile drift_parm
			{ /* see drift_parm below for actions */ }
	|	T_Logconfig log_config_list
			{ CONCAT_G_FIFOS(cfgt.logconfig, $2); }
	|	T_Phone string_list
			{ CONCAT_G_FIFOS(cfgt.phone, $2); }
	|	T_Setvar variable_assign
			{ APPEND_G_FIFO(cfgt.setvar, $2); }
	;

misc_cmd_dbl_keyword
	:	T_Nonvolatile
	|	T_Tick
	;

misc_cmd_int_keyword
	:	T_Dscp
	;

misc_cmd_int_keyword
	:	T_Leapsmearinterval
		{
#ifndef ENABLE_LEAP_SMEAR
			yyerror("Built without LEAP_SMEAR support.");
#endif
		}
	;

misc_cmd_str_keyword
	:	T_Leapfile
	;

misc_cmd_str_lcl_keyword
	:	T_Logfile
	|	T_Pidfile
	|	T_Saveconfigdir
	;

drift_parm
	:	T_String
		{
			if (lex_from_file()) {
				attr_val *av;
				av = create_attr_sval(T_Driftfile, $1);
				APPEND_G_FIFO(cfgt.vars, av);
			} else {
				YYFREE($1);
				yyerror("driftfile remote configuration ignored");
			}
		}
	|	T_String T_Double
		{
			attr_val *av;

			av = create_attr_sval(T_Driftfile, $1);
			APPEND_G_FIFO(cfgt.vars, av);
			av = create_attr_dval(T_WanderThreshold, $2);
			APPEND_G_FIFO(cfgt.vars, av);
		}
	|	/* Null driftfile,  indicated by empty string "" */
		{
			attr_val *av;

			av = create_attr_sval(T_Driftfile, estrdup(""));
			APPEND_G_FIFO(cfgt.vars, av);
		}
	;

variable_assign
	:	T_String '=' T_String t_default_or_zero
			{ $$ = create_setvar_node($1, $3, $4); }
	;

t_default_or_zero
	:	T_Default
	|	/* empty, no "default" modifier */
			{ $$ = 0; }
	;

log_config_list
	:	log_config_list log_config_command
		{
			$$ = $1;
			APPEND_G_FIFO($$, $2);
		}
	|	log_config_command
		{
			$$ = NULL;
			APPEND_G_FIFO($$, $1);
		}
	;

log_config_command
	:	T_String
		{
			char	prefix;
			char *	type;

			switch ($1[0]) {

			case '+':
			case '-':
			case '=':
				prefix = $1[0];
				type = $1 + 1;
				break;

			default:
				prefix = '=';
				type = $1;
			}

			$$ = create_attr_sval(prefix, estrdup(type));
			YYFREE($1);
		}
	;

interface_command
	:	interface_nic nic_rule_action nic_rule_class
		{
			nic_rule_node *nrn;

			nrn = create_nic_rule_node($3, NULL, $2);
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
	|	interface_nic nic_rule_action T_String
		{
			nic_rule_node *nrn;

			nrn = create_nic_rule_node(0, $3, $2);
			APPEND_G_FIFO(cfgt.nic_rules, nrn);
		}
	;

interface_nic
	:	T_Interface
	|	T_Nic
	;

nic_rule_class
	:	T_All
	|	T_Ipv4
	|	T_Ipv6
	|	T_Wildcard
	;

nic_rule_action
	:	T_Listen
	|	T_Ignore
	|	T_Drop
	;

reset_command
	:	T_Reset counter_set_list
			{ CONCAT_G_FIFOS(cfgt.reset_counters, $2); }
	;

counter_set_list
	:	counter_set_list counter_set_keyword
		{
			$$ = $1;
			APPEND_G_FIFO($$, create_int_node($2));
		}
	|	counter_set_keyword
		{
			$$ = NULL;
			APPEND_G_FIFO($$, create_int_node($1));
		}
	;

counter_set_keyword
	:	T_Allpeers
	|	T_Auth
	|	T_Ctl
	|	T_Io
	|	T_Mem
	|	T_Sys
	|	T_Timer
	;



/* Miscellaneous Rules
 * -------------------
 */

integer_list_range
	:	integer_list_range integer_list_range_elt
		{
			$$ = $1;
			APPEND_G_FIFO($$, $2);
		}
	|	integer_list_range_elt
		{
			$$ = NULL;
			APPEND_G_FIFO($$, $1);
		}
	;

integer_list_range_elt
	:	T_Integer
			{ $$ = create_attr_ival('i', $1); }
	|	integer_range
	;

integer_range
	:	'(' T_Integer T_Ellipsis T_Integer ')'
			{ $$ = create_attr_rangeval('-', $2, $4); }
	;

string_list
	:	string_list T_String
		{
			$$ = $1;
			APPEND_G_FIFO($$, create_string_node($2));
		}
	|	T_String
		{
			$$ = NULL;
			APPEND_G_FIFO($$, create_string_node($1));
		}
	;

boolean
	:	T_Integer
		{
			if ($1 != 0 && $1 != 1) {
				yyerror("Integer value is not boolean (0 or 1). Assuming 1");
				$$ = 1;
			} else {
				$$ = $1;
			}
		}
	|	T_True	{ $$ = 1; }
	|	T_False	{ $$ = 0; }
	;

number
	:	T_Integer	{ $$ = (double)$1; }
	|	T_Double
	;


%%

#ifdef REFCLOCK
address_node *
addr_from_typeunit(char *type, int unit)
{
	char addrbuf[1025];	/* NI_MAXHOSTS on Linux */
	int dtype;

	for (dtype = 1; dtype < (int)num_refclock_conf; dtype++)
	    if (refclock_conf[dtype]->basename != NULL && strcasecmp(refclock_conf[dtype]->basename, type) == 0)
		goto foundit;
	 msyslog(LOG_ERR, "CONFIG: Unknown driver name %s", type);
	 exit(1);
foundit:
	snprintf(addrbuf, sizeof(addrbuf), "127.127.%d.%d", dtype, unit);
	return create_address_node(estrdup(addrbuf), AF_INET);
}
#endif /* REFCLOCK */

void
yyerror(
	const char *msg
	)
{
	int retval;
	struct FILE_INFO * ip_ctx;

	ip_ctx = lex_current();
	ip_ctx->errpos = ip_ctx->tokpos;

	msyslog(LOG_ERR, "CONFIG: line %d column %d %s",
		ip_ctx->errpos.nline, ip_ctx->errpos.ncol, msg);
	if (lex_from_file()) {
                /* all is good, so far */
                return;
        }
        /* Uh, oh, got an error */

	/* Increment the number of errors */
	++remote_config.no_errors;

	/* Save the error message in the correct buffer */
	if ((MAXLINE - 10) < remote_config.err_pos) {
		/* err_msg already full, ignore this */
		return;
	}
	retval = snprintf(remote_config.err_msg + remote_config.err_pos,
			  (size_t)(MAXLINE - remote_config.err_pos),
			  "column %d %s", ip_ctx->errpos.ncol, msg);

	/* Increment the value of err_pos */
	if (retval > 0) {
		/* careful, retval is not bytes written, it is
		 * bytes that would have been written if space had
                 * been available */
		remote_config.err_pos += retval;
		if (MAXLINE < remote_config.err_pos) {
			/* err_msg overflowed! */
			remote_config.err_pos = MAXLINE;
		}
	}
}


/*
 * token_name - convert T_ token integers to text
 *		example: token_name(T_Server) returns "T_Server"
 */
const char *
token_name(
	int token
	)
{
	return yytname[YYTRANSLATE(token)];
}


/* Initial Testing function -- ignore */
#if 0
int main(int argc, char *argv[])
{
	ip_file = FOPEN(argv[1], "r");
	if (!ip_file)
		fprintf(stderr, "ERROR!! Could not open file: %s\n", argv[1]);
	yyparse();
	return 0;
}
#endif

