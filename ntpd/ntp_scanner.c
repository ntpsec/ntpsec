
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


/* Define a structure to hold the FSA for the keywords.
 * The structure is actually a trie
 */

struct state {
	char ch;		  /* Input character associated with the state */
	struct state *next_state; /* Next state to advance to on reading ch */
	struct state *next_char;  /* Pointer to next character associated with
				     the state */
	int token;		  /* Token to be returned on successful parse */
	int expect_string;	  /* A boolean flag, which when set, indicates
				     that the next token should be a string */
};


/* SCANNER GLOBAL VARIABLES 
 * ------------------------
 */

#define MAX_LEXEME 1024+1	/* The maximum size of a lexeme */
char yytext[MAX_LEXEME];	/* Buffer for storing the input text/lexeme */
struct state *key_scanner;	/* A FSA for recognizing keywords */
extern int input_from_file;

/* CONSTANTS 
 * ---------
 */
#define NON_ACCEPTING 0		/* A constant that depicts a non-accepting state */
#define NO_OF_SPECIAL_CHARS 8


/* SCANNER GLOBAL VARIABLES 
 * ------------------------
 */
char special_char[] =		/* This list of special characters */
{ '{', '}', '(', ')', ',', ';','|','=' };
 

/* FUNCTIONS
 * ---------
 */

char get_next_char(void);

/* Define a function to create the states of the scanner. This function
 * is used by the create_keyword_scanner function below.
 *
 * This function takes a suffix of a keyword, the token to be returned on
 * recognizing the complete keyword, and any pre-existing state that exists
 * for some other keyword that has the same prefix as the current one.
 */

struct state *
create_states(
	char *keyword, 
	int token, 
	int expect_string,
	struct state *pre_state
	)
{
	struct state *my_state;
	struct state *return_state = pre_state;

	struct state *prev_char = NULL;
	struct state *curr_char = pre_state;

	/* Find the correct position to insert the state. 
	 * All states should be in alphabetical order
	 */
	while (curr_char && (keyword[0] < curr_char->ch)) {
		prev_char = curr_char;
		curr_char = curr_char->next_char;
	}

	/* Check if a previously seen keyword has the same prefix as the 
	 * current keyword. If yes, simply use the state for that keyword
	 * as my_state 
	 */
	if (curr_char && (keyword[0] == curr_char->ch))
		my_state = curr_char;
	else {
		my_state = (struct state *) malloc(sizeof(struct state));
		my_state->ch = keyword[0];  /* Store the first character
					       of the keyword */
		my_state->next_state = NULL;
		my_state->next_char = curr_char;
		my_state->token = NON_ACCEPTING;  /* Not an accepting state */
		my_state->expect_string = NO_ARG; 

		if (prev_char) 
			prev_char->next_char = my_state;
		else
			return_state = my_state;
	}

	/* Check if the next character is '\0'.
	 * If yes, we are done with the recognition and this is an accepting
	 * state.
	 * If not, we need to continue scanning
	 */
	if (keyword[1] == '\0') {
		my_state->token = token;
		my_state->expect_string = expect_string;
	}
	else {
		my_state->next_state = create_states(&keyword[1], 
						     token, 
						     expect_string,
						     my_state->next_state);
	}

	return return_state;
}



/* Define a function that takes a list of (keyword, token) values and
 * creates a keywords scanner out of it.
 */

struct state *
create_keyword_scanner(
	struct key_tok *keyword_list
	)
{
	struct state *scanner = NULL;
	while (keyword_list->keyword != NULL) {
		scanner = create_states(keyword_list->keyword, 
					keyword_list->token, 
					keyword_list->expect_string,
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
		delete_keyword_scanner(my_key_scanner->next_char);
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
			printf("%s\n",lexeme);
		}
		if (curr_state->next_state != NULL)
			print_keyword_scanner(curr_state->next_state, pos + 1);
		curr_state = curr_state->next_char;
	}
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
	struct FILE_INFO *my_info = (struct FILE_INFO *)
		malloc(sizeof(struct FILE_INFO));

	if (my_info == NULL)
		return NULL;
	my_info->line_no = 0;
	my_info->col_no = 0;
	my_info->fname = path;

	if ((my_info->fd = fopen(path,mode)) == NULL) {
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
	if (input_from_file)
		return (char)FGETC(ip_file);
	else {
		if (remote_config.buffer[remote_config.pos] == '\0') 
			return EOF;
		else 
			return(remote_config.buffer[remote_config.pos++]);
	}
}

void
push_back_char(
	int ch
	)
{
	if (input_from_file)
		UNGETC(ch, ip_file);
	else 
		remote_config.pos--;
}

 

/* STATE MACHINES 
 * --------------
 */

/* Keywords */
static int
is_keyword(
	char *lexeme,
	int *expect_string
	)
{
	struct state *curr_state = key_scanner;
	int token = NON_ACCEPTING;
	int i;

	for (i = 0; lexeme[i]; ++i) {
		while (curr_state && (lexeme[i] != curr_state->ch))
			curr_state = curr_state->next_char;

		if (curr_state && (lexeme[i] == curr_state->ch)) {
			*expect_string = curr_state->expect_string;
			token = curr_state->token;
			curr_state = curr_state->next_state;
		}
		else {
			*expect_string = NO_ARG;
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
	int i;

	/* Initialize the state machine
	   int int_part = 1;
	   int frac_part = 0; 
	*/
	int no_digits = 0;  /* Number of digits read */

	i = 0;
	/* Check for an optional '+' or '-' */
	if (lexeme[i] == '+' || lexeme[i] == '-')
		++i;

	/* Read the integer part */
	for (; lexeme[i] && isdigit(lexeme[i]); ++i)
		++no_digits;

	/* Check for the required decimal point */
	if (lexeme[i] == '.')
		++i;
	else
		return 0;

	/* Check for any digits after the decimal point */
	for (; lexeme[i] && isdigit(lexeme[i]); ++i)
		++no_digits;

	/* The number of digits in both the decimal part and the fraction part
	   must not be zero at this point */
	if (no_digits == 0)
		return 0;

	/* Check if we are done */
	if (lexeme[i] == '\0')
		return 1;

	/* There is still more output, read the Exponent Part */
	if (lexeme[i] == 'e' || lexeme[i] == 'E') {
		++i;
	}
	else
		return 0;

	/* Read an optional Sign */
	if (lexeme[i] == '+' || lexeme[i] == '-')
		++i;

	/* Now read the exponent part */
	for (; lexeme[i] && isdigit(lexeme[i]); ++i)
		++no_digits;

	/* Check if we are done */
	if (lexeme[i] == '\0')
		return 1;
	else
		return 0;
}



/* Host Name */
/* static int is_host_name (char *lexeme) */
/* { */
/*	int i; */
/*	for (i = 0;lexeme[i];++i) */
/*	if (!isalnum(lexeme[i])) { */
		/* Check for two consequtive periods which are not allowed */
/*		if (lexeme[i] == '.' && lexeme[i + 1] != '.') */
/*			; */
/*		else */
/*			return 0; */
/*	} */
/*	return 1; */
/* } */


/* Define a function to test whether a character is a special character */

static int
is_special(
	int ch
	)
{
	int i;

	for (i = 0; i < NO_OF_SPECIAL_CHARS; ++i)
		if (ch == special_char[i])
			return 1;
	return 0;
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

	if (!*pch)
		return T_EOC;

	yylval.String = strdup(lexeme);
	if (!yylval.String) {
		fprintf(stderr, "Could not allocate memory for: %s\n",
			lexeme);
		exit(1);
	}
	else
		return T_String;
}


/* Define a function that does the actual scanning 
 * Bison expects this function to be called yylex and for it to take no 
 * input and return an int
 */

int
yylex()
{
	int i, instring = 0;
	int token;		/* The return value/the recognized token */
	int ch;
	static int expect_string = NO_ARG;

	do {
		/* Ignore whitespace at the beginning */
		while ((ch = get_next_char()) != EOF &&
		       isspace(ch) &&
		       !is_EOC(ch))
			; /* Null Statement */

		if (ch == EOF) {
			if (input_from_file == 0)
				return 0;
			if (!(curr_include_level > 0)) 
				return 0;
			else { 
				FCLOSE(fp[curr_include_level]);
				ip_file = fp[--curr_include_level]; 
				return T_EOC;
			}
		} else if (is_EOC(ch)) {
			expect_string = NO_ARG;   /* Reset expect_string */
			return T_EOC;
		}
		/* Check if the next character is a special character.
		 * If yes, return the special character.
		 */
		else if ((expect_string == NO_ARG) && is_special(ch))
			return ch;
		else
			push_back_char(ch);

		/* Read in the lexeme */
		for (i = 0;(i < MAX_LEXEME) && 
			 (yytext[i] = get_next_char()) != EOF; ++i) {

			/* Break on whitespace or a special character */
			if (isspace(yytext[i]) 
			    || ((expect_string == NO_ARG) && is_special(yytext[i]))
			    || is_EOC(ch) || yytext[i] == '"')
				break;

			/* Read the rest of the line on reading a start
			   of comment character */
			if (yytext[i] == '#') {
				while ((yytext[i] = get_next_char()) != EOF
				       && yytext[i] != '\n')
					; /* Null Statement */
				break;
			}
		}
		/* Pick up all of the string inside between " marks, to
		 * end of line.  If we make it to EOL without a
		 * terminating " assume it for them.
		 *
		 * XXX - HMS: I'm not sure we want to assume the closing "
		 */
		if (yytext[i] == '"') {
			instring = 1;
			while ((yytext[i] = get_next_char()) != EOF &&
			       yytext[i] != '"' && yytext[i] != '\n') {
				i++;
			}
			if (yytext[i] == '"')
				yytext[i] =  ' ';
		}
		/* Pushback the last character read that is not a part
		 * of this lexeme.
		 * If the last character read was an EOF, pushback a
		 * newline character. This is to prevent a parse error
		 * when there is no newline at the end of a file.
		 */
		if (yytext[i] == EOF)
			push_back_char('\n');
		else
			push_back_char(yytext[i]); 
		yytext[i] = '\0';
	} while (i == 0);

#ifdef DEBUG
	if (debug >= 3)
		printf ("yylex: Just Read: %s\n", yytext);
#endif

	/* Now return the desired token */
	
	/* First make sure that the parser is *not* expecting a string
	 * as the next token (based on the previous token that was
	 * returned) and that we haven't read a string.
	 */
	
	if ((expect_string == NO_ARG) &&  (!instring)) {
		token = is_keyword(yytext, &expect_string);
		if (token) 
			return token;
		else if (is_integer(yytext)) {
			errno = 0;
			if ((yylval.Integer = strtol(yytext,(char **) NULL, 10)) == 0
			    && ((errno == EINVAL) || (errno == ERANGE))) {
				fprintf(stderr,"Integer cannot be represented: %s\n",
					yytext);
				exit(1);
			}
			else
				return T_Integer;
		}
		else if (is_double(yytext)) {
			errno = 0;
			if ((yylval.Double = atof(yytext)) == 0 && errno == ERANGE) {
				fprintf(stderr, "Double too large to represent: %s\n",
					yytext);
				exit(1);
			}
			else
				return T_Double;
		}
		else /* Default: Everything is a string */
			return create_string_token(yytext);
	}
	else { 
		/* Either expect_string is 1 or this lexeme is part of a string.
		   Hence, we need to return T_String.
		   
		   ONLY EXCEPTION (sic), we might have a -4 or -6 flag.
		   This is a terrible hack, but the grammar is ambiguous so
		   we don't have a choice.
		*/
		if (strcmp(yytext, "-4") == 0)
			return T_IPv4_address;
		else if (strcmp(yytext, "-6") == 0)
			return T_IPv6_address;
		else {
			instring = 0;
			if (expect_string == SINGLE_ARG)
				expect_string = NO_ARG;
			return create_string_token(yytext);
		}
	}
	return 1;
}
