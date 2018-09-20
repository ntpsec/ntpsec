/*
 * keyword-gen.c -- generate keyword scanner finite state machine and
 *		    keyword_text array.
 *
 * This program is run to generate ntp_keyword.h
 */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ntp_scanner.h"
#include "ntp_parser.tab.h"


/* Define a structure to hold a (keyword, token) pair */
struct key_tok {
	const char * key;	/* Keyword */
	unsigned short	token;		/* Associated Token */
	follby	followedby;	/* nonzero indicates the next token(s)
				   forced to be string(s) */
};

struct key_tok ntp_keywords[] = {
{ "...",		T_Ellipsis,		FOLLBY_TOKEN },
{ "allpeers",		T_Allpeers,		FOLLBY_TOKEN },
{ "bias",		T_Bias,			FOLLBY_TOKEN },
{ "baud",		T_Baud,			FOLLBY_TOKEN },
{ "clock",		T_Clock,		FOLLBY_STRING },
{ "ctl",		T_Ctl,			FOLLBY_TOKEN },
{ "disable",		T_Disable,		FOLLBY_TOKEN },
{ "driftfile",		T_Driftfile,		FOLLBY_STRING },
{ "dscp",		T_Dscp,			FOLLBY_TOKEN },
{ "enable",		T_Enable,		FOLLBY_TOKEN },
{ "end",		T_End,			FOLLBY_TOKEN },
{ "filegen",		T_Filegen,		FOLLBY_TOKEN },
{ "fudge",		T_Fudge,		FOLLBY_STRING },
{ "holdover",		T_Holdover,		FOLLBY_TOKEN },
{ "io",			T_Io,			FOLLBY_TOKEN },
{ "includefile",	T_Includefile,		FOLLBY_STRING },
{ "leapfile",		T_Leapfile,		FOLLBY_STRING },
{ "leapsmearinterval",	T_Leapsmearinterval,	FOLLBY_TOKEN },
{ "logconfig",		T_Logconfig,		FOLLBY_STRINGS_TO_EOC },
{ "logfile",		T_Logfile,		FOLLBY_STRING },
{ "mem",		T_Mem,			FOLLBY_TOKEN },
{ "path",		T_Path,			FOLLBY_STRING },
{ "peer",		T_Peer,			FOLLBY_STRING },
{ "phone",		T_Phone,		FOLLBY_STRINGS_TO_EOC },
{ "pidfile",		T_Pidfile,		FOLLBY_STRING },
{ "pool",		T_Pool,			FOLLBY_STRING },
{ "ppspath",		T_Ppspath,		FOLLBY_STRING },
{ "discard",		T_Discard,		FOLLBY_TOKEN },
{ "reset",		T_Reset,		FOLLBY_TOKEN },
{ "restrict",		T_Restrict,		FOLLBY_TOKEN },
{ "refclock",		T_Refclock,		FOLLBY_STRING },
{ "rlimit",		T_Rlimit,		FOLLBY_TOKEN },
{ "server",		T_Server,		FOLLBY_STRING },
{ "setvar",		T_Setvar,		FOLLBY_STRING },
{ "statistics",		T_Statistics,		FOLLBY_TOKEN },
{ "statsdir",		T_Statsdir,		FOLLBY_STRING },
{ "sys",		T_Sys,			FOLLBY_TOKEN },
{ "tick",		T_Tick,			FOLLBY_TOKEN },
{ "timer",		T_Timer,		FOLLBY_TOKEN },
{ "tinker",		T_Tinker,		FOLLBY_TOKEN },
{ "tos",		T_Tos,			FOLLBY_TOKEN },
{ "unconfig",		T_Unconfig,		FOLLBY_STRING },
{ "unit",		T_Unit,			FOLLBY_TOKEN },
{ "unpeer",		T_Unpeer,		FOLLBY_STRING },
{ "unrestrict",		T_Unrestrict,		FOLLBY_TOKEN },
/* authentication_command */
{ "controlkey",		T_ControlKey,		FOLLBY_TOKEN },
{ "requestkey",		T_Requestkey,		FOLLBY_TOKEN }, /* dummy */
{ "keys",		T_Keys,			FOLLBY_STRING },
{ "ntpsigndsocket",	T_NtpSignDsocket,	FOLLBY_STRING },
{ "trustedkey",		T_Trustedkey,		FOLLBY_TOKEN },
/* IPv4/IPv6 protocol override flag */
{ "-4",			T_Ipv4_flag,		FOLLBY_TOKEN },
{ "-6",			T_Ipv6_flag,		FOLLBY_TOKEN },
/* option */
{ "burst",		T_Burst,		FOLLBY_TOKEN },
{ "iburst",		T_Iburst,		FOLLBY_TOKEN },
{ "key",		T_Key,			FOLLBY_TOKEN },
{ "maxpoll",		T_Maxpoll,		FOLLBY_TOKEN },
{ "mdnstries",		T_Mdnstries,		FOLLBY_TOKEN },
{ "minpoll",		T_Minpoll,		FOLLBY_TOKEN },
{ "mode",		T_Mode,			FOLLBY_TOKEN },
{ "noselect",		T_Noselect,		FOLLBY_TOKEN },
{ "true",		T_True,			FOLLBY_TOKEN },
{ "prefer",		T_Prefer,		FOLLBY_TOKEN },
{ "subtype",		T_Subtype,		FOLLBY_TOKEN },
{ "version",		T_Version,		FOLLBY_TOKEN },
/*** MONITORING COMMANDS ***/
/* stat */
{ "clockstats",		T_Clockstats,		FOLLBY_TOKEN },
{ "loopstats",		T_Loopstats,		FOLLBY_TOKEN },
{ "peerstats",		T_Peerstats,		FOLLBY_TOKEN },
{ "protostats",		T_Protostats,		FOLLBY_TOKEN },
{ "rawstats",		T_Rawstats,		FOLLBY_TOKEN },
{ "sysstats", 		T_Sysstats,		FOLLBY_TOKEN },
{ "usestats",		T_Usestats,		FOLLBY_TOKEN },
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
{ "mindist",		T_Mindist,		FOLLBY_TOKEN },
{ "maxdisp",		T_Maxdisp,		FOLLBY_TOKEN },
{ "maxdist",		T_Maxdist,		FOLLBY_TOKEN },
{ "orphan",		T_Orphan,		FOLLBY_TOKEN },
{ "orphanwait",		T_Orphanwait,		FOLLBY_TOKEN },
{ "nonvolatile",	T_Nonvolatile,		FOLLBY_TOKEN },
/* access_control_flag */
{ "default",		T_Default,		FOLLBY_TOKEN },
{ "source",		T_Source,		FOLLBY_TOKEN },
{ "flake",		T_Flake,		FOLLBY_TOKEN },
{ "ignore",		T_Ignore,		FOLLBY_TOKEN },
{ "limited",		T_Limited,		FOLLBY_TOKEN },
{ "mssntp",		T_Mssntp,		FOLLBY_TOKEN },
{ "kod",		T_Kod,			FOLLBY_TOKEN },
{ "mask",		T_Mask,			FOLLBY_TOKEN },
{ "nomodify",		T_Nomodify,		FOLLBY_TOKEN },
{ "nomrulist",		T_Nomrulist,		FOLLBY_TOKEN },
{ "nopeer",		T_Nopeer,		FOLLBY_TOKEN },
{ "noquery",		T_Noquery,		FOLLBY_TOKEN },
{ "notrap",		T_Notrap,		FOLLBY_TOKEN },
{ "noserve",		T_Noserve,		FOLLBY_TOKEN },
{ "notrust",		T_Notrust,		FOLLBY_TOKEN },
{ "ntpport",		T_Ntpport,		FOLLBY_TOKEN },
/* discard_option */
{ "average",		T_Average,		FOLLBY_TOKEN },
{ "minimum",		T_Minimum,		FOLLBY_TOKEN },
{ "monitor",		T_Monitor,		FOLLBY_TOKEN },
/* mru_option */
{ "incalloc",		T_Incalloc,		FOLLBY_TOKEN },
{ "incmem",		T_Incmem,		FOLLBY_TOKEN },
{ "initalloc",		T_Initalloc,		FOLLBY_TOKEN },
{ "initmem",		T_Initmem,		FOLLBY_TOKEN },
{ "mindepth",		T_Mindepth,		FOLLBY_TOKEN },
{ "maxdepth",		T_Maxdepth,		FOLLBY_TOKEN },
{ "maxage",		T_Maxage,		FOLLBY_TOKEN },
{ "minage",		T_Minage,		FOLLBY_TOKEN },
{ "maxmem",		T_Maxmem,		FOLLBY_TOKEN },
{ "mru",		T_Mru,			FOLLBY_TOKEN },
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
{ "calibrate",		T_Calibrate,		FOLLBY_TOKEN },
{ "kernel",		T_Kernel,		FOLLBY_TOKEN },
{ "ntp",		T_Ntp,			FOLLBY_TOKEN },
{ "stats",		T_Stats,		FOLLBY_TOKEN },
/* rlimit_option */
{ "memlock",		T_Memlock,		FOLLBY_TOKEN },
{ "stacksize",		T_Stacksize,		FOLLBY_TOKEN },
{ "filenum",		T_Filenum,		FOLLBY_TOKEN },
/* tinker_option */
{ "step",		T_Step,			FOLLBY_TOKEN },
{ "stepback",		T_Stepback,		FOLLBY_TOKEN },
{ "stepfwd",		T_Stepfwd,		FOLLBY_TOKEN },
{ "panic",		T_Panic,		FOLLBY_TOKEN },
{ "dispersion",		T_Dispersion,		FOLLBY_TOKEN },
{ "stepout",		T_Stepout,		FOLLBY_TOKEN },
{ "allan",		T_Allan,		FOLLBY_TOKEN },
{ "huffpuff",		T_Huffpuff,		FOLLBY_TOKEN },
{ "freq",		T_Freq,			FOLLBY_TOKEN },
/* miscellaneous_command */
{ "interface",		T_Interface,		FOLLBY_TOKEN },
/* interface_command (ignore and interface already defined) */
{ "nic",		T_Nic,			FOLLBY_TOKEN },
{ "all",		T_All,			FOLLBY_TOKEN },
{ "ipv4",		T_Ipv4,			FOLLBY_TOKEN },
{ "ipv6",		T_Ipv6,			FOLLBY_TOKEN },
{ "wildcard",		T_Wildcard,		FOLLBY_TOKEN },
{ "listen",		T_Listen,		FOLLBY_TOKEN },
{ "drop",		T_Drop,			FOLLBY_TOKEN },
};

typedef struct big_scan_state_tag {
	char		ch;		/* Character this state matches on */
	char		followedby;	/* Forces next token(s) to T_String */
	unsigned short	finishes_token;	/* nonzero ID if last keyword char */
	unsigned short	match_next_s;	/* next state to check matching ch */
	unsigned short	other_next_s;	/* next state to check if not ch */
} big_scan_state;

/*
 * Note: to increase MAXSTATES beyond 2048, be aware it is currently
 * crammed into 11 bits in scan_state form.  Raising to 4096 would be
 * relatively easy by storing the followedby value in a separate
 * array with one entry per token, and shrinking the char value to
 * 7 bits to free a bit for accepting/non-accepting.  More than 4096
 * states will require expanding scan_state beyond 32 bits each.
 */
#define MAXSTATES	2048
#define MAX_TOK_LEN	63

const char *	current_keyword;/* for error reporting */
static big_scan_state	sst[MAXSTATES];	/* scanner FSM state entries */
static unsigned short	sst_highwater;	/* next entry index to consider */
char *		symb[1024];	/* map token ID to symbolic name */

/* for libntp */
const char *	progname = "keyword-gen";

int			main			(int, char **);
static void		generate_preamble	(void);
static void		generate_fsm		(void);
static void		generate_token_text	(void);
static unsigned short	create_keyword_scanner	(void);
static unsigned short	create_scan_states	(const char *,
						 unsigned short, follby,
						 unsigned short);
static int		compare_key_tok_id	(const void *, const void *);
static int		compare_key_tok_text	(const void *, const void *);
static void		populate_symb		(char *);
static const char*	symbname		(unsigned short);


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
	struct tm tmbuf;
	char preamble[] =
"/*\n"
" * ntp_keyword.h\n"
" * \n"
" * NOTE: edit this file with caution, it is generated by keyword-gen.c\n"
" *\t Generated %s UTC	  diff_ignore_line\n"
" *\n"
" */\n"
"#include \"ntp_scanner.h\"\n"
"#include \"ntp_parser.tab.h\"\n"
"\n";

	time(&now);
	if (!strftime(timestamp, sizeof(timestamp),
		      "%Y-%m-%d %H:%M:%S", gmtime_r(&now, &tmbuf)))
		timestamp[0] = '\0';

	printf(preamble, timestamp);
}


static void
generate_fsm(void)
{
	char rprefix[MAX_TOK_LEN + 1];
	char prefix[MAX_TOK_LEN + 1];
	char token_id_comment[16 + MAX_TOK_LEN + 1];
	size_t prefix_len;
	char *p;
	char *r;
	unsigned short initial_state;
	unsigned short this_state;
	unsigned short state;
	unsigned short i;
	unsigned short token;

	/*
	 * Sort ntp_keywords in alphabetical keyword order.  This is
	 * not necessary, but minimizes nonfunctional changes in the
	 * generated finite state machine when keywords are modified.
	 */
	qsort(ntp_keywords, COUNTOF(ntp_keywords),
	      sizeof(ntp_keywords[0]), compare_key_tok_text);

	/*
	 * To save space, reserve the state array entry matching each
	 * token number for its terminal state, so the token identifier
	 * does not need to be stored in each state, but can be
	 * recovered trivially.  To mark the entry reserved,
	 * finishes_token is nonzero.
	 */

	for (i = 0; i < COUNTOF(ntp_keywords); i++) {
		token = ntp_keywords[i].token;
		if (1 > token || token >= COUNTOF(sst)) {
			fprintf(stderr,
				"keyword-gen sst[%u] too small "
				"for keyword '%s' id %d\n",
				(int)COUNTOF(sst),
				ntp_keywords[i].key,
				token);
			exit(4);
		}
		sst[token].finishes_token = token;
	}

	initial_state = create_keyword_scanner();

	fprintf(stderr,
		"%d keywords consumed %d states of %d max.\n",
		(int)COUNTOF(ntp_keywords),
		sst_highwater - 1,
		(int)COUNTOF(sst) - 1);

	printf("#define SCANNER_INIT_S %d\n\n", initial_state);

	printf("const scan_state sst[%d] = {\n"
	       "/*SS_T( ch,\tf-by, match, other ),\t\t\t\t */\n"
	       "  0,\t\t\t\t      /* %5d %-17s */\n",
	       sst_highwater,
	       0, "");

	for (i = 1; i < sst_highwater; i++) {

		/* verify fields will fit */
		if (sst[i].followedby & ~0x3) {
			fprintf(stderr,
				"keyword-gen internal error "
				"sst[%d].followedby %d too big\n",
				i, sst[i].followedby);
			exit(7);
		}

		if (sst_highwater <= sst[i].match_next_s
		    || sst[i].match_next_s & ~0x7ff) {
			fprintf(stderr,
				"keyword-gen internal error "
				"sst[%d].match_next_s %d too big\n",
				i, sst[i].match_next_s);
			exit(8);
		}

		if (sst_highwater <= sst[i].other_next_s
		    || sst[i].other_next_s & ~0x7ff) {
			fprintf(stderr,
				"keyword-gen internal error "
				"sst[%d].other_next_s %d too big\n",
				i, sst[i].other_next_s);
			exit(9);
		}

		if (sst[i].finishes_token) {
			snprintf(token_id_comment,
				 sizeof(token_id_comment), "%5d %-17s",
				 i, symbname(sst[i].finishes_token));
			if (i != sst[i].finishes_token) {
				fprintf(stderr,
					"keyword-gen internal error "
					"entry %d finishes token %d\n",
					i, sst[i].finishes_token);
				exit(5);
			}
		} else {
		/*
		 * Determine the keyword prefix that leads to this
		 * state.  This is expensive but keyword-gen is run
		 * only when it changes.
		 *
		 * Scan the state array iteratively looking for a state
		 * which leads to the current one, collecting matching
		 * characters along the way.  There is only one such
		 * path back to the starting state given the way our
		 * scanner state machine is built and the practice of
		 * using the spelling of the keyword as its T_* token
		 * identifier, which results in never having two
		 * spellings result in the same T_* value.
		 */
			prefix_len = 0;
			this_state = i;
			do {
				for (state = 1; state < sst_highwater; state++)
					if (sst[state].other_next_s == this_state) {
						this_state = state;
						break;
					} else if (sst[state].match_next_s == this_state) {
						this_state = state;
						rprefix[prefix_len] = sst[state].ch;
						prefix_len++;
						break;
					}
			} while (this_state != initial_state);

			if (prefix_len) {
				/* reverse rprefix into prefix */
				p = prefix + prefix_len;
				r = rprefix;
				while (r < rprefix + prefix_len)
					*--p = *r++;
			}
			prefix[prefix_len] = '\0';

			snprintf(token_id_comment,
				 sizeof(token_id_comment), "%5d %-17s",
				 i, (initial_state == i)
					? "[initial state]"
					: prefix);
		}

		printf("  S_ST( '%c',\t%d,    %5u, %5u )%s /* %s */\n",
		       sst[i].ch,
		       sst[i].followedby,
		       sst[i].match_next_s,
		       sst[i].other_next_s,
		       (i + 1 < sst_highwater)
			   ? ","
			   : " ",
		       token_id_comment);
	}

	printf("};\n\n");
}


/* Define a function to create the states of the scanner. This function
 * is used by the create_keyword_scanner function below.
 *
 * This function takes a suffix of a keyword, the token to be returned on
 * recognizing the complete keyword, and any pre-existing state that exists
 * for some other keyword that has the same prefix as the current one.
 */
static unsigned short
create_scan_states(
	const char * text,
	unsigned short	token,
	follby	followedby,
	unsigned short	prev_state
	)
{
	unsigned short my_state;
	unsigned short return_state;
	unsigned short prev_char_s;
	unsigned short curr_char_s;

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
		do
			my_state = sst_highwater++;
		while (my_state < COUNTOF(sst)
		       && sst[my_state].finishes_token);
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
		sst[my_state].followedby = FOLLBY_NON_ACCEPTING;

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
		sst[my_state].finishes_token = (unsigned short)token;
		sst[my_state].followedby = (char)followedby;

		if (sst[token].finishes_token != (unsigned short)token) {
			fprintf(stderr,
				"fatal, sst[%d] not reserved for %s.\n",
				token, symbname(token));
			exit(6);
		}
		/* relocate so token id is sst[] index */
		if (my_state != token) {
			sst[token] = sst[my_state];
			ZERO(sst[my_state]);
			do
				sst_highwater--;
			while (sst[sst_highwater].finishes_token);
			my_state = token;
			if (prev_char_s)
				sst[prev_char_s].other_next_s = my_state;
			else
				return_state = my_state;
		}
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

static unsigned short
create_keyword_scanner(void)
{
	unsigned short scanner;
	unsigned short i;

	sst_highwater = 1;	/* index 0 invalid, unused */
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
	unsigned short lowest_id;
	unsigned short highest_id;
	unsigned short id_count;
	unsigned short id;
	unsigned short i;

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


static int
compare_key_tok_id(
	const void *a1,
	const void *a2
	)
{
	const struct key_tok *p1 = a1;
	const struct key_tok *p2 = a2;

	if (p1->token == p2->token)
		return COMPARE_EQUAL;

	if (p1->token < p2->token)
		return COMPARE_LESSTHAN;
	else
		return COMPARE_GREATERTHAN;
}


static int
compare_key_tok_text(
	const void *a1,
	const void *a2
	)
{
	const struct key_tok *p1 = a1;
	const struct key_tok *p2 = a2;

	return strcmp(p1->key, p2->key);
}


/*
 * populate_symb() - populate symb[] lookup array with symbolic token
 *		     names such that symb[T_Age] == "T_Age", etc.
 */
static void
populate_symb(
	char *header_file
	)
{
	FILE *	yh;
	char	line[2 * MAX_TOK_LEN];
	char	name[2 * MAX_TOK_LEN];
	int	token;

	yh = fopen(header_file, "r");
	if (NULL == yh) {
		perror("unable to open yacc/bison header file");
		exit(4);
	}

	while (NULL != fgets(line, sizeof(line), yh))
		if (2 == sscanf(line, "#define %s %d", name, &token)
		    && 'T' == name[0] && '_' == name[1] && token >= 0
		    && token < (int)COUNTOF(symb)) {

			symb[token] = strdup(name);
			if (strlen(name) > MAX_TOK_LEN) {
				fprintf(stderr,
					"MAX_TOK_LEN %d too small for '%s'\n"
					"Edit keyword-gen.c to raise.\n",
					MAX_TOK_LEN, name);
				exit(10);
			}
		}
	fclose(yh);
}


static const char *
symbname(
	unsigned short token
	)
{
	static char buf[20];
	char *name;

	if (token < COUNTOF(symb) && symb[token] != NULL) {
		name = symb[token];
	} else {
		snprintf(buf, sizeof(buf), "%d", token);
		name = buf;
	}

	return name;
}
