/*
 * keyword-gen.c -- generate keyword scanner finite state machine and
 *		    keyword_text array.
 *		    This program is run to generate ntp_keyword.h
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <ntp_stdlib.h>
#include <ntp_config.h>
#include "../libntp/lib_strbuf.h"
#include "ntp_scanner.h"
#include "ntp_parser.h"


#ifdef QSORT_USES_VOID_P
typedef const void *	QSORTP;
#else
typedef char *		QSORTP;
#endif

/* Define a structure to hold a (keyword, token) pair */
struct key_tok {
	char *	key;		/* Keyword */
	int	token;		/* Associated Token */
	follby	followedby;	/* nonzero indicates the next token(s)
				   forced to be string(s) */
};

struct key_tok ntp_keywords[] = {
{ "automax",		T_Automax,		FOLLBY_TOKEN },
{ "broadcast",		T_Broadcast,		FOLLBY_STRING },
{ "broadcastclient",	T_Broadcastclient,	FOLLBY_TOKEN },
{ "broadcastdelay",	T_Broadcastdelay,	FOLLBY_TOKEN },
{ "calldelay",		T_Calldelay,		FOLLBY_TOKEN },
{ "disable",		T_Disable,		FOLLBY_TOKEN },
{ "driftfile",		T_Driftfile,		FOLLBY_STRING },
{ "enable",		T_Enable,		FOLLBY_TOKEN },
{ "end",		T_End,			FOLLBY_TOKEN },
{ "filegen",		T_Filegen,		FOLLBY_TOKEN },
{ "fudge",		T_Fudge,		FOLLBY_STRING },
{ "includefile",	T_Includefile,		FOLLBY_STRING },
{ "leapfile",		T_Leapfile,		FOLLBY_STRING },
{ "logconfig",		T_Logconfig,		FOLLBY_STRINGS_TO_EOC },
{ "logfile",		T_Logfile,		FOLLBY_STRING },
{ "manycastclient",	T_Manycastclient,	FOLLBY_STRING },
{ "manycastserver",	T_Manycastserver,	FOLLBY_STRINGS_TO_EOC },
{ "multicastclient",	T_Multicastclient,	FOLLBY_STRINGS_TO_EOC },
{ "peer",		T_Peer,			FOLLBY_STRING },
{ "phone",		T_Phone,		FOLLBY_STRINGS_TO_EOC },
{ "pidfile",		T_Pidfile,		FOLLBY_STRING },
{ "pool",		T_Pool,			FOLLBY_STRING },
{ "discard",		T_Discard,		FOLLBY_TOKEN },
{ "restrict",		T_Restrict,		FOLLBY_TOKEN },
{ "server",		T_Server,		FOLLBY_STRING },
{ "setvar",		T_Setvar,		FOLLBY_STRING },
{ "statistics",		T_Statistics,		FOLLBY_TOKEN },
{ "statsdir",		T_Statsdir,		FOLLBY_STRING },
{ "tick",		T_Tick,			FOLLBY_TOKEN },
{ "tinker",		T_Tinker,		FOLLBY_TOKEN },
{ "tos",		T_Tos,			FOLLBY_TOKEN },
{ "trap",		T_Trap,			FOLLBY_STRING },
{ "unconfig",		T_Unconfig,		FOLLBY_STRING },
{ "unpeer",		T_Unpeer,		FOLLBY_STRING },
/* authentication_command */
{ "controlkey",		T_ControlKey,		FOLLBY_TOKEN },
{ "crypto",		T_Crypto,		FOLLBY_TOKEN },
{ "keys",		T_Keys,			FOLLBY_STRING },
{ "keysdir",		T_Keysdir,		FOLLBY_STRING },
{ "ntpsigndsocket",	T_NtpSignDsocket,	FOLLBY_STRING },
{ "requestkey",		T_Requestkey,		FOLLBY_TOKEN },
{ "revoke",		T_Revoke,		FOLLBY_TOKEN },
{ "trustedkey",		T_Trustedkey,		FOLLBY_TOKEN },
/* IPv4/IPv6 protocol override flag */
{ "-4",			T_Ipv4_flag,		FOLLBY_TOKEN },
{ "-6",			T_Ipv6_flag,		FOLLBY_TOKEN },
/* option */
{ "autokey",		T_Autokey,		FOLLBY_TOKEN },
{ "bias",		T_Bias,			FOLLBY_TOKEN },
{ "burst",		T_Burst,		FOLLBY_TOKEN },
{ "iburst",		T_Iburst,		FOLLBY_TOKEN },
{ "key",		T_Key,			FOLLBY_TOKEN },
{ "maxpoll",		T_Maxpoll,		FOLLBY_TOKEN },
{ "minpoll",		T_Minpoll,		FOLLBY_TOKEN },
{ "mode",		T_Mode,			FOLLBY_TOKEN },
{ "noselect",		T_Noselect,		FOLLBY_TOKEN },
{ "preempt",		T_Preempt,		FOLLBY_TOKEN },
{ "true",		T_True,			FOLLBY_TOKEN },
{ "prefer",		T_Prefer,		FOLLBY_TOKEN },
{ "ttl",		T_Ttl,			FOLLBY_TOKEN },
{ "version",		T_Version,		FOLLBY_TOKEN },
{ "xleave",		T_Xleave,		FOLLBY_TOKEN },
/* crypto_command */
{ "host",		T_Host,			FOLLBY_STRING },
{ "ident",		T_Ident,		FOLLBY_STRING },
{ "pw",			T_Pw,			FOLLBY_STRING },
{ "randfile",		T_Randfile,		FOLLBY_STRING },
{ "sign",		T_Sign,			FOLLBY_STRING },
/*** MONITORING COMMANDS ***/
/* stat */
{ "clockstats",		T_Clockstats,		FOLLBY_TOKEN },
{ "cryptostats",	T_Cryptostats,		FOLLBY_TOKEN },
{ "loopstats",		T_Loopstats,		FOLLBY_TOKEN },
{ "peerstats",		T_Peerstats,		FOLLBY_TOKEN },
{ "rawstats",		T_Rawstats,		FOLLBY_TOKEN },
{ "sysstats", 		T_Sysstats,		FOLLBY_TOKEN },
{ "protostats",		T_Protostats,		FOLLBY_TOKEN },
{ "timingstats",	T_Timingstats,		FOLLBY_TOKEN },
/* filegen_option */
{ "file",		T_File,			FOLLBY_STRING },
{ "link",		T_Link,			FOLLBY_TOKEN },
{ "nolink",		T_Nolink,		FOLLBY_TOKEN },
{ "type",		T_Type,			FOLLBY_TOKEN },
/* filegen_type */
{ "age",		T_Age,			FOLLBY_TOKEN },
{ "day",		T_Day,			FOLLBY_TOKEN },
{ "month",		T_Month,		FOLLBY_TOKEN },
{ "none",		T_None,			FOLLBY_TOKEN },
{ "pid",		T_Pid,			FOLLBY_TOKEN },
{ "week",		T_Week,			FOLLBY_TOKEN },
{ "year",		T_Year,			FOLLBY_TOKEN },
/*** ORPHAN MODE COMMANDS ***/
/* tos_option */
{ "minclock",		T_Minclock,		FOLLBY_TOKEN },
{ "maxclock",		T_Maxclock,		FOLLBY_TOKEN },
{ "minsane",		T_Minsane,		FOLLBY_TOKEN },
{ "floor",		T_Floor,		FOLLBY_TOKEN },
{ "ceiling",		T_Ceiling,		FOLLBY_TOKEN },
{ "cohort",		T_Cohort,		FOLLBY_TOKEN },
{ "mindist",		T_Mindist,		FOLLBY_TOKEN },
{ "maxdist",		T_Maxdist,		FOLLBY_TOKEN },
{ "beacon",		T_Beacon,		FOLLBY_TOKEN },
{ "orphan",		T_Orphan,		FOLLBY_TOKEN },
/* access_control_flag */
{ "default",		T_Default,		FOLLBY_TOKEN },
{ "flake",		T_Flake,		FOLLBY_TOKEN },
{ "ignore",		T_Ignore,		FOLLBY_TOKEN },
{ "limited",		T_Limited,		FOLLBY_TOKEN },
{ "mssntp",		T_Mssntp,		FOLLBY_TOKEN },
{ "kod",		T_Kod,			FOLLBY_TOKEN },
{ "lowpriotrap",	T_Lowpriotrap,		FOLLBY_TOKEN },
{ "mask",		T_Mask,			FOLLBY_TOKEN },
{ "nomodify",		T_Nomodify,		FOLLBY_TOKEN },
{ "nopeer",		T_Nopeer,		FOLLBY_TOKEN },
{ "noquery",		T_Noquery,		FOLLBY_TOKEN },
{ "noserve",		T_Noserve,		FOLLBY_TOKEN },
{ "notrap",		T_Notrap,		FOLLBY_TOKEN },
{ "notrust",		T_Notrust,		FOLLBY_TOKEN },
{ "ntpport",		T_Ntpport,		FOLLBY_TOKEN },
/* discard_option */
{ "average",		T_Average,		FOLLBY_TOKEN },
{ "minimum",		T_Minimum,		FOLLBY_TOKEN },
{ "monitor",		T_Monitor,		FOLLBY_TOKEN },
/* fudge_factor */
{ "flag1",		T_Flag1,		FOLLBY_TOKEN },
{ "flag2",		T_Flag2,		FOLLBY_TOKEN },
{ "flag3",		T_Flag3,		FOLLBY_TOKEN },
{ "flag4",		T_Flag4,		FOLLBY_TOKEN },
{ "refid",		T_Refid,		FOLLBY_STRING },
{ "stratum",		T_Stratum,		FOLLBY_TOKEN },
{ "time1",		T_Time1,		FOLLBY_TOKEN },
{ "time2",		T_Time2,		FOLLBY_TOKEN },
/* system_option */
{ "auth",		T_Auth,			FOLLBY_TOKEN },
{ "bclient",		T_Bclient,		FOLLBY_TOKEN },
{ "calibrate",		T_Calibrate,		FOLLBY_TOKEN },
{ "kernel",		T_Kernel,		FOLLBY_TOKEN },
{ "ntp",		T_Ntp,			FOLLBY_TOKEN },
{ "stats",		T_Stats,		FOLLBY_TOKEN },
/* tinker_option */
{ "step",		T_Step,			FOLLBY_TOKEN },
{ "panic",		T_Panic,		FOLLBY_TOKEN },
{ "dispersion",		T_Dispersion,		FOLLBY_TOKEN },
{ "stepout",		T_Stepout,		FOLLBY_TOKEN },
{ "allan",		T_Allan,		FOLLBY_TOKEN },
{ "huffpuff",		T_Huffpuff,		FOLLBY_TOKEN },
{ "freq",		T_Freq,			FOLLBY_TOKEN },
/* miscellaneous_command */
{ "port",		T_Port,			FOLLBY_TOKEN },
{ "interface",		T_Interface,		FOLLBY_TOKEN },
{ "qos",		T_Qos,			FOLLBY_TOKEN },
{ "saveconfigdir",	T_Saveconfigdir,	FOLLBY_STRING },
/* interface_command (ignore and interface already defined) */
{ "nic",		T_Nic,			FOLLBY_TOKEN },
{ "all",		T_All,			FOLLBY_TOKEN },
{ "ipv4",		T_Ipv4,			FOLLBY_TOKEN },
{ "ipv6",		T_Ipv6,			FOLLBY_TOKEN },
{ "wildcard",		T_Wildcard,		FOLLBY_TOKEN },
{ "listen",		T_Listen,		FOLLBY_TOKEN },
{ "drop",		T_Drop,			FOLLBY_TOKEN },
/* simulator commands */
{ "simulate",		T_Simulate,		FOLLBY_TOKEN },
{ "simulation_duration",T_Sim_Duration,		FOLLBY_TOKEN },
{ "beep_delay",		T_Beep_Delay,		FOLLBY_TOKEN },
{ "duration",		T_Duration,		FOLLBY_TOKEN },
{ "server_offset",	T_Server_Offset,	FOLLBY_TOKEN },
{ "freq_offset",	T_Freq_Offset,		FOLLBY_TOKEN },
{ "wander",		T_Wander,		FOLLBY_TOKEN },
{ "jitter",		T_Jitter,		FOLLBY_TOKEN },
{ "prop_delay",		T_Prop_Delay,		FOLLBY_TOKEN },
{ "proc_delay",		T_Proc_Delay,		FOLLBY_TOKEN },
};

const char *	current_keyword;/* for error reporting */
int		sst_free;	/* next unused entry index */
scan_state	sst[2048];	/* scanner FSM state entries */
char *		symb[1024];	/* map token ID to symbolic name */

/* for libntp */
const char *	progname = "keyword-gen";
volatile int	debug = 1;

int		main			(int, char **);
static void	generate_preamble	(void);
static void	generate_fsm		(void);
static void	generate_token_text	(void);
static int	create_keyword_scanner	(void);
static int	create_scan_states	(char *, int, follby, int);
int		compare_key_tok_id	(QSORTP, QSORTP);
int		compare_key_tok_text	(QSORTP, QSORTP);
void		populate_symb		(char *);
const char *	symbname		(int);


int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Usage:\n%s t_header.h\n", argv[0]);
		exit(1);
	}
	populate_symb(argv[1]);

	generate_preamble();
	generate_token_text();
	generate_fsm();

	return 0;
}


static void
generate_preamble(void)
{
	time_t now;
	char timestamp[128];
	char preamble[] =
"/*\n"
" * ntp_keyword.h\n"
" * \n"
" * NOTE: edit this file with caution, it is generated by keyword-gen.c\n"
" *\t Generated %s UTC	  diff_ignore_line\n"
" *\n"
" */\n"
"#include \"ntp_scanner.h\"\n"
"#include \"ntp_parser.h\"\n"
"\n";

	time(&now);
	if (!strftime(timestamp, sizeof(timestamp),
		      "%Y-%m-%d %H:%M:%S", gmtime(&now)))
		timestamp[0] = '\0';

	printf(preamble, timestamp);
}


static void
generate_fsm(void)
{
	int initial_state;
	int i;

	/* 
	 * Sort ntp_keywords in alphabetical keyword order.  This is
	 * not necessary, but minimizes nonfunctional changes in the
	 * generated finite state machine when keywords are modified.
	 */
	qsort(ntp_keywords, COUNTOF(ntp_keywords),
	      sizeof(ntp_keywords[0]), compare_key_tok_text);

	initial_state = create_keyword_scanner();

	fprintf(stderr,
		"%d keywords consumed %d states of %d max.\n",
		(int)COUNTOF(ntp_keywords),
		sst_free - 1,
		(int)COUNTOF(sst) - 1);

	printf("#define SCANNER_INIT_S %d\n\n", initial_state);

	printf("const scan_state sst[%d] = {\n"
	       "/*\t\t{ ch,\tf-by,\t\t token,\tmatch,\tother\t}\t*/\n"
	       "/* %-5d */\t{ 0 },", 
	       sst_free,
	       0);

	for (i = 1; i < sst_free; i++) {
		printf("\n/* %-5d */\t{ '%c',\t%d,%20s,\t%5u,\t%5u\t}",
		       i,
		       sst[i].ch,
		       sst[i].followedby,
		       symbname(sst[i].finishes_token),
		       sst[i].match_next_s,
		       sst[i].other_next_s);
		if (i + 1 < sst_free)
			printf(",");
	}

	printf("\n};\n\n");
}


/* Define a function to create the states of the scanner. This function
 * is used by the create_keyword_scanner function below.
 *
 * This function takes a suffix of a keyword, the token to be returned on
 * recognizing the complete keyword, and any pre-existing state that exists
 * for some other keyword that has the same prefix as the current one.
 */
static int
create_scan_states(
	char *	text, 
	int	token, 
	follby	followedby,
	int	prev_state
	)
{
	int my_state;
	int return_state;
	int prev_char_s;
	int curr_char_s;

	return_state = prev_state;
	curr_char_s = prev_state;
	prev_char_s = 0;

	/* Find the correct position to insert the state. 
	 * All states should be in alphabetical order
	 */
	while (curr_char_s && (text[0] < sst[curr_char_s].ch)) {
		prev_char_s = curr_char_s;
		curr_char_s = sst[curr_char_s].other_next_s;
	}

	/* 
	 * Check if a previously seen keyword has the same prefix as
	 * the current keyword.  If so, simply use the state for that
	 * keyword as my_state, otherwise, allocate a new state.
	 */
	if (curr_char_s && (text[0] == sst[curr_char_s].ch)) {
		my_state = curr_char_s;
		if ('\0' == text[1]) {
			fprintf(stderr,
				"Duplicate entries for keyword '%s' in"
				" keyword_gen.c ntp_keywords[].\n",
				current_keyword);
			exit(2);
		}
	} else {
		my_state = sst_free++;
		if (my_state >= COUNTOF(sst)) {
			fprintf(stderr,
				"fatal, keyword scanner state array "
				"sst[%d] is too small, modify\n"
				"keyword-gen.c to increase.\n",
				(int)COUNTOF(sst));
			exit(3);
		}
		/* Store the next character of the keyword */
		sst[my_state].ch = text[0]; 
		sst[my_state].other_next_s = curr_char_s;

		if (prev_char_s)
			sst[prev_char_s].other_next_s = my_state;
		else
			return_state = my_state;
	}

	/* Check if the next character is '\0'.
	 * If yes, we are done with the recognition and this is an accepting
	 * state.
	 * If not, we need to continue scanning
	 */
	if ('\0' == text[1]) {
		sst[my_state].finishes_token = (u_short)token;
		sst[my_state].followedby = (char)followedby;
	} else
		sst[my_state].match_next_s = 
		    create_scan_states(
			&text[1],
			token,
			followedby,
			sst[my_state].match_next_s);

	return return_state;
}


/* Define a function that takes a list of (keyword, token) values and
 * creates a keywords scanner out of it.
 */

static int
create_keyword_scanner(void)
{
	int scanner;
	int i;

	sst_free = 1;	/* index 0 invalid, unused */
	scanner = 0;

	for (i = 0; i < COUNTOF(ntp_keywords); i++) {
		current_keyword = ntp_keywords[i].key;
		scanner =
		    create_scan_states(
			ntp_keywords[i].key, 
			ntp_keywords[i].token, 
			ntp_keywords[i].followedby,
			scanner);
	}

	return scanner;
}


static void
generate_token_text(void)
{
	int lowest_id;
	int highest_id;
	int id_count;
	int id;
	int i;

	/* sort ntp_keywords in token ID order */
	qsort(ntp_keywords, COUNTOF(ntp_keywords),
	      sizeof(ntp_keywords[0]), compare_key_tok_id);

	lowest_id = ntp_keywords[0].token;
	highest_id = ntp_keywords[COUNTOF(ntp_keywords) - 1].token;
	id_count = highest_id - lowest_id + 1;

	printf("#define LOWEST_KEYWORD_ID %d\n\n", lowest_id);

	printf("const char * const keyword_text[%d] = {", id_count);

	id = lowest_id;
	i = 0;
	while (i < COUNTOF(ntp_keywords)) {
		while (id < ntp_keywords[i].token) {
			printf(",\n\t/* %-5d %5d %20s */\tNULL",
			       id - lowest_id, id, symbname(id));
			id++;
		}
		if (i > 0)
			printf(",");
		printf("\n\t/* %-5d %5d %20s */\t\"%s\"",
		       id - lowest_id, id, symbname(id), 
		       ntp_keywords[i].key);
		i++;
		id++;
	}

	printf("\n};\n\n");
}

	
int
compare_key_tok_id(
	QSORTP a1,
	QSORTP a2
	)
{
	const struct key_tok *p1 = (const void *)a1;
	const struct key_tok *p2 = (const void *)a2;

	if (p1->token == p2->token)
		return 0;

	if (p1->token < p2->token)
		return -1;
	else
		return 1;
}


int
compare_key_tok_text(
	QSORTP a1,
	QSORTP a2
	)
{
	const struct key_tok *p1 = (const void *)a1;
	const struct key_tok *p2 = (const void *)a2;

	return strcmp(p1->key, p2->key);
}


/*
 * populate_symb() - populate symb[] lookup array with symbolic token
 *		     names such that symb[T_Age] == "T_Age", etc.
 */
void
populate_symb(
	char *header_file
	)
{
	FILE *	yh;
	char	line[128];
	char	name[128];
	int	token;

	yh = fopen(header_file, "r");
	if (NULL == yh) {
		perror("unable to open yacc/bison header file");
		exit(4);
	}

	while (NULL != fgets(line, sizeof(line), yh))
		if (2 == sscanf(line, "#define %s %d", name, &token)
		    && 'T' == name[0] && '_' == name[1] && token >= 0
		    && token < COUNTOF(symb))

			symb[token] = estrdup(name);

	fclose(yh);
}


const char *
symbname(
	int token
	)
{
	char *name;

	if (token >= 0 && token < COUNTOF(symb) && symb[token] != NULL)
		return symb[token];

	LIB_GETBUF(name);
	snprintf(name, LIB_BUFLENGTH, "%d", token);
	return name;
}
