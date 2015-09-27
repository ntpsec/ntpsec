/*
 * ntp_lineedit.c - generic interface to various line editing libs
 */
#include <config.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(HAVE_READLINE_HISTORY_H) &&	defined(HAVE_READLINE_READLINE_H)
# include <readline/readline.h>
# include <readline/history.h>
# define USE_LE_READLINE
#elif defined(HAVE_HISTEDIT_H)
# include <histedit.h>
# define USE_LE_EDITLINE
#else
# define USE_LE_NONE
#endif

#include "ntp.h"
#include "ntp_stdlib.h"
#include "ntp_lineedit.h"

#define MAXEDITLINE	512

/*
 * external references
 */

extern char *	progname;

/*
 * globals, private prototypes
 */

static int	ntp_readline_initted;
static char *	lineedit_prompt;


#ifdef USE_LE_EDITLINE
# ifndef H_SETSIZE
#  define H_SETSIZE H_EVENT
# endif
static EditLine *	ntp_el;
static History *	ntp_hist;
static HistEvent	hev;

char *	ntp_prompt_callback(EditLine *);
#endif	/* USE_LE_EDITLINE */


/*
 * ntp_readline_init - setup, set or reset prompt string
 */
int
ntp_readline_init(
	const char *	prompt
	)
{
	int	success;

	success = 1;

	if (prompt) {
		if (lineedit_prompt) 
			free(lineedit_prompt);
		lineedit_prompt = estrdup(prompt);
	}

#ifdef USE_LE_EDITLINE
	if (NULL == ntp_el) {

		/* Some older versions have only 3 args. */
		ntp_el = el_init(progname, stdin, stdout, stderr);
		if (ntp_el) {

			el_set(ntp_el, EL_PROMPT, ntp_prompt_callback);
			el_set(ntp_el, EL_EDITOR, "emacs");

			ntp_hist = history_init();

			if (NULL == ntp_hist) {

				mfprintf(stderr, "history_init(): %m\n");
				fflush(stderr);

				el_end(ntp_el);
				ntp_el = NULL;

				success = 0;

			} else {
				ZERO(hev);
#ifdef H_SETSIZE
				history(ntp_hist, &hev, H_SETSIZE, 128);
#endif
				el_set(ntp_el, EL_HIST, history,
				       ntp_hist);
				/* use any .editrc */
				el_source(ntp_el, NULL);
			}
		} else
			success = 0;
	}
#endif	/* USE_LE_EDITLINE */

	ntp_readline_initted = success;

	return success;
}


/*
 * ntp_readline_uninit - release resources
 */
void
ntp_readline_uninit(
	void
	)
{
#ifdef USE_LE_EDITLINE
	if (ntp_el) {
		el_end(ntp_el);
		ntp_el = NULL;

		history_end(ntp_hist);
		ntp_hist = NULL;
	}
#endif	/* USE_LE_EDITLINE */

	if (lineedit_prompt) {
		free(lineedit_prompt);
		lineedit_prompt = NULL;
	}

	ntp_readline_initted = 0;
}


/*
 * ntp_readline - read a line with the line editor available
 *
 * The string returned must be released with free()
 */

char *
ntp_readline(
	int *	pcount
	)
{
	char *		line;
#ifdef USE_LE_NONE
	char		line_buf[MAXEDITLINE];
#endif
#ifdef USE_LE_EDITLINE
	const char *	cline;
#endif

	if (!ntp_readline_initted)
		return NULL;

	*pcount = 0;

#ifdef USE_LE_READLINE
	line = readline(lineedit_prompt ? lineedit_prompt : "");
	if (NULL != line) {
		if (*line) {
			add_history(line);
		}
		*pcount = strlen(line);
	}
#endif	/* USE_LE_READLINE */

#ifdef USE_LE_EDITLINE
	cline = el_gets(ntp_el, pcount);

	if (NULL != cline) {
		history(ntp_hist, &hev, H_ENTER, cline);
		line = estrdup(cline);
	} else if (*pcount == -1) {
		line = NULL;
	} else {
		line = estrdup("");
	}
#endif	/* USE_LE_EDITLINE */

#ifdef USE_LE_NONE
					/* stone hammers */
	if (lineedit_prompt) {
		fputs(lineedit_prompt, stderr);
		fflush(stderr);
	}

	line = fgets(line_buf, sizeof(line_buf), stdin);
	if (NULL != line && *line) {
		*pcount = strlen(line);
		line = estrdup(line);
	} else
		line = NULL;

#endif	/* USE_LE_NONE */


	if (!line)			/* EOF */
		fputs("\n", stderr);

	return line;
}


#ifdef USE_LE_EDITLINE
/*
 * ntp_prompt_callback - return prompt string to el_gets()
 */
char *
ntp_prompt_callback(
	EditLine *el
	)
{
	UNUSED_ARG(el);

	return lineedit_prompt;
}
#endif /* USE_LE_EDITLINE */

