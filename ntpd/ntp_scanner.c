
/* ntp_scanner.c
 *
 * The source code for a simple lexical analyzer. 
 *
 * Written By:	Sachin Kamboj
 *		University of Delaware
 *		Newark, DE 19711
 * Copyright (c) 2006
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "ntp_config.h"
#include "ntpsim.h"
#include "ntp_scanner.h"
#include "ntp_parser.h"
#include "ntp_debug.h"


/* Define a structure to hold the FSA for the keywords.
 * The structure is actually a trie
 */

struct state {
	char ch;		  /* Input character associated with the state */
	struct state *next_state; /* Next state to advance to on reading ch */
	struct state *next_char_s;/* State to check having read any other */
	int token;		  /* Token to be returned on successful parse */
	follby followedby;	  /* Forces the next token(s) to T_String */
};


/* SCANNER GLOBAL VARIABLES 
 * ------------------------
 */

#define MAX_LEXEME (1024 + 1)	/* The maximum size of a lexeme */
char yytext[MAX_LEXEME];	/* Buffer for storing the input text/lexeme */
struct state *key_scanner;	/* A FSA for recognizing keywords */
extern int input_from_file;


struct key_tok ntp_keyword_list[] = {
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
{ "disable",		T_Disable,		FOLLBY_TOKEN },
{ "enable",		T_Enable,		FOLLBY_TOKEN },
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
{ "maxhop",		T_Maxhop,		FOLLBY_TOKEN },
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
{ "version",		T_Version,		FOLLBY_TOKEN },
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
{ "monitor",		T_Monitor,		FOLLBY_TOKEN },
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
{ NULL,			0,			0	     } };


/* CONSTANTS 
 * ---------
 */
#define NON_ACCEPTING 0		/* A constant that depicts a non-accepting state */


/* SCANNER GLOBAL VARIABLES 
 * ------------------------
 */
const char special_chars[] = "{}(),;|=";


/* FUNCTIONS
 * ---------
 */

char get_next_char(void);
static int is_keyword(char *lexeme, follby *pfollowedby);


/* Define a function to create the states of the scanner. This function
 * is used by the create_keyword_scanner function below.
 *
 * This function takes a suffix of a keyword, the token to be returned on
 * recognizing the complete keyword, and any pre-existing state that exists
 * for some other keyword that has the same prefix as the current one.
 */
struct state *
create_states(
	char *	text, 
	int	token, 
	follby	followedby,
	struct state *pre_state
	)
{
	struct state *my_state;
	struct state *return_state;
	struct state *prev_char_s;
	struct state *curr_char_s;

	return_state = pre_state;
	curr_char_s = pre_state;
	prev_char_s = NULL;

	/* Find the correct position to insert the state. 
	 * All states should be in alphabetical order
	 */
	while (curr_char_s != NULL && (text[0] < curr_char_s->ch)) {
		prev_char_s = curr_char_s;
		curr_char_s = curr_char_s->next_char_s;
	}

	/* Check if a previously seen keyword has the same prefix as the 
	 * current keyword. If yes, simply use the state for that keyword
	 * as my_state 
	 */
	if (curr_char_s && (text[0] == curr_char_s->ch))
		my_state = curr_char_s;
	else {
		my_state = emalloc(sizeof(*my_state));
		/* Store the first character of the keyword */
		my_state->ch = text[0]; 
		my_state->next_state = NULL;
		my_state->next_char_s = curr_char_s;
		/* Not an accepting state */
		my_state->token = NON_ACCEPTING;
		my_state->followedby = FOLLBY_TOKEN; 

		if (prev_char_s) 
			prev_char_s->next_char_s = my_state;
		else
			return_state = my_state;
	}

	/* Check if the next character is '\0'.
	 * If yes, we are done with the recognition and this is an accepting
	 * state.
	 * If not, we need to continue scanning
	 */
	if ('\0' == text[1]) {
		my_state->token = token;
		my_state->followedby = followedby;
	} else
		my_state->next_state = 
		    create_states(&text[1], token, followedby,
				  my_state->next_state);

	return return_state;
}


/* Define a function that takes a list of (keyword, token) values and
 * creates a keywords scanner out of it.
 */

struct state *
create_keyword_scanner(void)
{
	struct state *scanner;
	struct key_tok *keyword_list;

	keyword_list = ntp_keyword_list;
	scanner = NULL;
	while (keyword_list->key != NULL) {
		scanner = create_states(keyword_list->key, 
					keyword_list->token, 
					keyword_list->followedby,
					scanner);
		++keyword_list;
	}
	return scanner;
}


/* Define a function to delete the keyword scanner, freeing all the allocated
 * memory
 */
void
delete_keyword_scanner(
	struct state *my_key_scanner
	)
{
	if (my_key_scanner) {
		delete_keyword_scanner(my_key_scanner->next_char_s);
		delete_keyword_scanner(my_key_scanner->next_state);
		free(my_key_scanner);
	}
}


/* Define a function to print the keyword scanner built using the
 * above functions.
 */

void
print_keyword_scanner(
	struct state *my_key_scanner,
	int pos
	)
{
	static char lexeme[MAX_LEXEME];
	struct state *curr_state = my_key_scanner;
	while (curr_state != NULL) {
		lexeme[pos] = curr_state->ch;
		if (curr_state->token != NON_ACCEPTING) {
			lexeme[pos + 1] = '\0';
			printf("%s\n", lexeme);
		}
		if (curr_state->next_state != NULL)
			print_keyword_scanner(curr_state->next_state, pos + 1);
		curr_state = curr_state->next_char_s;
	}
}


/*
 * keyword() - Return the keyword associated with token T_ identifier
 */
char *
keyword(
	int token
	)
{
	struct key_tok *keyword_list;

	

	for (keyword_list = ntp_keyword_list;
	     keyword_list->key != NULL;
	     keyword_list++) 
		if (keyword_list->token == token)
			return keyword_list->key;

	return "(keyword not found)";
}


/* FILE INTERFACE
 * --------------
 * We define a couple of wrapper functions around the standard C fgetc
 * and ungetc functions in order to include positional bookkeeping
 */

struct FILE_INFO *
F_OPEN(
	const char *path,
	const char *mode
	)
{
	struct FILE_INFO *my_info;

	my_info = emalloc(sizeof *my_info);

	my_info->line_no = 1;
	my_info->col_no = 0;
	my_info->prev_line_col_no = 0;
	my_info->prev_token_col_no = 0;
	my_info->fname = path;

	my_info->fd = fopen(path, mode);
	if (NULL == my_info->fd) {
		free(my_info);
		return NULL;
	}
	return my_info;
}

int
FGETC(
	struct FILE_INFO *stream
	)
{
	int ch = fgetc(stream->fd);

	++stream->col_no;
	if (ch == '\n') {
		stream->prev_line_col_no = stream->col_no;
		++stream->line_no;
		stream->col_no = 1;
	}
	return ch;
}

/* BUGS: 1. Function will fail on more than one line of pushback
 *       2. No error checking is done to see if ungetc fails
 * SK: I don't think its worth fixing these bugs for our purposes ;-)
 */
int
UNGETC(
	int ch,
	struct FILE_INFO *stream
	)
{
	if (ch == '\n') {
		stream->col_no = stream->prev_line_col_no;
		stream->prev_line_col_no = -1;
		--stream->line_no;
	}
	--stream->col_no;
	return ungetc(ch, stream->fd);
}

int
FCLOSE(
	struct FILE_INFO *stream
	)
{
	int ret_val = fclose(stream->fd);

	if (!ret_val)
		free(stream);
	return ret_val;
}

/* STREAM INTERFACE 
 * ----------------
 * Provide a wrapper for the stream functions so that the
 * stream can either read from a file or from a character
 * array. 
 * NOTE: This is not very efficient for reading from character
 * arrays, but needed to allow remote configuration where the
 * configuration command is provided through ntpq.
 * 
 * The behavior of there two functions is determined by the 
 * input_from_file flag.
 */

char
get_next_char(
	void
	)
{
	char ch;

	if (input_from_file)
		return (char)FGETC(ip_file);
	else {
		if (remote_config.buffer[remote_config.pos] == '\0') 
			return EOF;
		else {
			ip_file->col_no++;
			ch = remote_config.buffer[remote_config.pos++];
			if (ch == '\n') {
				ip_file->prev_line_col_no = ip_file->col_no;
				++ip_file->line_no;
				ip_file->col_no = 1;
			}
			return ch;
		}
	}
}

void
push_back_char(
	int ch
	)
{
	if (input_from_file)
		UNGETC(ch, ip_file);
	else {
		if (ch == '\n') {
			ip_file->col_no = ip_file->prev_line_col_no;
			ip_file->prev_line_col_no = -1;
			--ip_file->line_no;
		}
		--ip_file->col_no;

		remote_config.pos--;
	}
}

 

/* STATE MACHINES 
 * --------------
 */

/* Keywords */
static int
is_keyword(
	char *lexeme,
	follby *pfollowedby
	)
{
	struct state *curr_state = key_scanner;
	int token = NON_ACCEPTING;
	int i;

	for (i = 0; lexeme[i]; ++i) {
		while (curr_state && (lexeme[i] != curr_state->ch))
			curr_state = curr_state->next_char_s;

		if (curr_state && (lexeme[i] == curr_state->ch)) {
			*pfollowedby = curr_state->followedby;
			token = curr_state->token;
			curr_state = curr_state->next_state;
		}
		else {
			*pfollowedby = FOLLBY_TOKEN;
			token = NON_ACCEPTING;
			break;
		}
	}
	return token;
}


/* Integer */
static int
is_integer(
	char *lexeme
	)
{
	int i = 0;

	/* Allow a leading minus sign */
	if (lexeme[i] == '-')
		++i;

	/* Check that all the remaining characters are digits */
	for (; lexeme[i]; ++i) {
		if (!isdigit(lexeme[i]))
			return 0;
	}
	return 1;
}


/* Double */
static int
is_double(
	char *lexeme
	)
{
	int num_digits = 0;  /* Number of digits read */
	int i;

	i = 0;

	/* Check for an optional '+' or '-' */
	if ('+' == lexeme[i] || '-' == lexeme[i])
		i++;

	/* Read the integer part */
	for (; lexeme[i] && isdigit(lexeme[i]); i++)
		num_digits++;

	/* Check for the required decimal point */
	if ('.' == lexeme[i])
		i++;
	else
		return 0;

	/* Check for any digits after the decimal point */
	for (; lexeme[i] && isdigit(lexeme[i]); i++)
		num_digits++;

	/*
	 * The number of digits in both the decimal part and the
	 * fraction part must not be zero at this point 
	 */
	if (!num_digits)
		return 0;

	/* Check if we are done */
	if (!lexeme[i])
		return 1;

	/* There is still more input, read the exponent */
	if ('e' == tolower(lexeme[i]))
		i++;
	else
		return 0;

	/* Read an optional Sign */
	if ('+' == lexeme[i] || '-' == lexeme[i])
		i++;

	/* Now read the exponent part */
	while (lexeme[i] && isdigit(lexeme[i]))
		i++;

	/* Check if we are done */
	if (!lexeme[i])
		return 1;
	else
		return 0;
}


/* is_special() - Test whether a character is a token */
static inline int
is_special(
	int ch
	)
{
	return (int)strchr(special_chars, ch);
}


static int
is_EOC(
	int ch
	)
{
	if ((old_config_style && (ch == '\n')) ||
	    (!old_config_style && (ch == ';')))
		return 1;
	return 0;
}


char *
quote_if_needed(char *str)
{
	char *ret;
	size_t len;
	size_t octets;

	len = strlen(str);
	octets = len + 2 + 1;
	ret = emalloc(octets);
	if ('"' != str[0] 
	    && (strcspn(str, special_chars) < len 
		|| strchr(str, ' ') != NULL)) {
		snprintf(ret, octets, "\"%s\"", str);
	} else
		strncpy(ret, str, octets);

	return ret;
}


static int
create_string_token(
	char *lexeme
	)
{
	char *pch;

	/*
	 * ignore end of line whitespace
	 */
	pch = lexeme;
	while (*pch && isspace(*pch))
		pch++;

	if (!*pch) {
		yylval.Integer = T_EOC;
		return yylval.Integer;
	}

	yylval.String = estrdup(lexeme);
	return T_String;
}


/*
 * yylex() - function that does the actual scanning.
 * Bison expects this function to be called yylex and for it to take no
 * input and return an int.
 * Conceptually yylex "returns" yylval as well as the actual return
 * value representing the token or type.
 */
int
yylex(
	void
	)
{
	int i, instring = 0;
	int yylval_was_set = 0;
	int token;		/* The return value/the recognized token */
	int ch;
	static follby followedby = FOLLBY_TOKEN;

	do {
		/* Ignore whitespace at the beginning */
		while (EOF != (ch = get_next_char()) &&
		       isspace(ch) &&
		       !is_EOC(ch))
			; /* Null Statement */

		if (EOF == ch) {

			if (!input_from_file || !curr_include_level) 
				return 0;

			FCLOSE(fp[curr_include_level]);
			ip_file = fp[--curr_include_level];
			token = T_EOC;
			goto normal_return;

		} else if (is_EOC(ch)) {

			/* end FOLLBY_STRINGS_TO_EOC effect */
			followedby = FOLLBY_TOKEN;
			token = T_EOC;
			goto normal_return;

		} else if (is_special(ch) && FOLLBY_TOKEN == followedby) {
			/* special chars are their own token values */
			token = ch;
			/*
			 * '=' implies a single string following as in:
			 * setvar Owner = "The Boss" default
			 * This could alternatively be handled by
			 * removing '=' from special_chars and adding
			 * it to the keyword table.
			 */
			if ('=' == ch)
				followedby = FOLLBY_STRING;
			yytext[0] = (char)ch;
			yytext[1] = '\0';
			goto normal_return;
		} else
			push_back_char(ch);

		/* save the position of start of the token */
		ip_file->prev_token_line_no = ip_file->line_no;
		ip_file->prev_token_col_no = ip_file->col_no;

		/* Read in the lexeme */
		i = 0;
		while (EOF != (yytext[i] = get_next_char())) {

			/* Break on whitespace or a special character */
			if (isspace(yytext[i]) || is_EOC(ch) 
			    || '"' == yytext[i]
			    || (FOLLBY_TOKEN == followedby
				&& is_special(yytext[i])))
				break;

			/* Read the rest of the line on reading a start
			   of comment character */
			if ('#' == yytext[i]) {
				while (EOF != (yytext[i] = get_next_char())
				       && '\n' != yytext[i])
					; /* Null Statement */
				break;
			}

			i++;
			if (i >= COUNTOF(yytext))
				goto lex_too_long;
		}
		/* Pick up all of the string inside between " marks, to
		 * end of line.  If we make it to EOL without a
		 * terminating " assume it for them.
		 *
		 * XXX - HMS: I'm not sure we want to assume the closing "
		 */
		if ('"' == yytext[i]) {
			instring = 1;
			while (EOF != (yytext[i] = get_next_char()) &&
			       yytext[i] != '"' && yytext[i] != '\n') {
				i++;
				if (i >= COUNTOF(yytext))
					goto lex_too_long;
			}
			/*
			 * yytext[i] will be pushed back as not part of
			 * this lexeme, but any closing quote should
			 * not be pushed back, so we read another char.
			 */
			if ('"' == yytext[i])
				yytext[i] = get_next_char();
		}
		/* Pushback the last character read that is not a part
		 * of this lexeme.
		 * If the last character read was an EOF, pushback a
		 * newline character. This is to prevent a parse error
		 * when there is no newline at the end of a file.
		 */
		if (EOF == yytext[i])
			push_back_char('\n');
		else
			push_back_char(yytext[i]); 
		yytext[i] = '\0';
	} while (i == 0);

	/* Now return the desired token */
	
	/* First make sure that the parser is *not* expecting a string
	 * as the next token (based on the previous token that was
	 * returned) and that we haven't read a string.
	 */
	
	if (followedby == FOLLBY_TOKEN && !instring) {
		token = is_keyword(yytext, &followedby);
		if (token)
			goto normal_return;
		else if (is_integer(yytext)) {
			yylval_was_set = 1;
			errno = 0;
			if ((yylval.Integer = strtol(yytext, NULL, 10)) == 0
			    && ((errno == EINVAL) || (errno == ERANGE))) {
				msyslog(LOG_ERR, 
					"Integer cannot be represented: %s",
					yytext);
				exit(1);
			} else {
				token = T_Integer;
				goto normal_return;
			}
		}
		else if (is_double(yytext)) {
			yylval_was_set = 1;
			errno = 0;
			if ((yylval.Double = atof(yytext)) == 0 && errno == ERANGE) {
				msyslog(LOG_ERR,
					"Double too large to represent: %s",
					yytext);
				exit(1);
			} else {
				token = T_Double;
				goto normal_return;
			}
		} else {
			/* Default: Everything is a string */
			yylval_was_set = 1;
			token = create_string_token(yytext);
			goto normal_return;
		}
	}

	/*
	 * Either followedby is not FOLLBY_TOKEN or this lexeme is part
	 * of a string.  Hence, we need to return T_String.
	 * 
	 * _Except_ we might have a -4 or -6 flag on a an association
	 * configuration line (server, peer, pool, etc.).
	 *
	 * This is a terrible hack, but the grammar is ambiguous so we
	 * don't have a choice.  [SK]
	 *
	 * The ambiguity is in the keyword scanner, not ntp_parser.y.
	 * We do not require server addresses be quoted in ntp.conf,
	 * complicating the scanner's job.  To avoid trying (and
	 * failing) to match an IP address or DNS name to a keyword,
	 * the association keywords use FOLLBY_STRING in the keyword
	 * table, which tells the scanner to force the next token to be
	 * a T_String, so it does not try to match a keyword but rather
	 * expects a string when -4/-6 modifiers to server, peer, etc.
	 * are encountered.
	 * restrict -4 and restrict -6 parsing works correctly without
	 * this hack, as restrict uses FOLLBY_TOKEN.  [DH]
	 */
	if ('-' == yytext[0]) {
		if ('4' == yytext[1]) {
			token = T_Ipv4_flag;
			goto normal_return;
		} else if ('6' == yytext[1]) {
			token = T_Ipv6_flag;
			goto normal_return;
		}
	}

	instring = 0;
	if (FOLLBY_STRING == followedby)
		followedby = FOLLBY_TOKEN;

	yylval_was_set = 1;
	token = create_string_token(yytext);

normal_return:
	if (T_EOC == token)
		DPRINTF(4,("\t<end of command>\n"));
	else
		DPRINTF(4, ("yylex: lexeme '%s' -> %s\n", yytext,
			    token_name(token)));

	if (!yylval_was_set)
		yylval.Integer = token;

	return token;

lex_too_long:
	yytext[min(sizeof(yytext) - 1, 50)] = 0;
	msyslog(LOG_ERR, 
		"configuration item on line %d longer than limit of %d, began with '%s'",
		ip_file->line_no, sizeof(yytext) - 1, yytext);

	/*
	 * If we hit the length limit reading the startup configuration
	 * file, abort.
	 */
	if (input_from_file)
		exit(sizeof(yytext) - 1);

	/*
	 * If it's runtime configuration via ntpq :config treat it as
	 * if the configuration text ended before the too-long lexeme,
	 * hostname, or string.
	 */
	yylval.Integer = 0;
	return 0;
}
