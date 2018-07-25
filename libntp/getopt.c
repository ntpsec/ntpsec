#include <string.h>

#include "config.h"

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_stdlib.h"

#define no_argument 0
#define required_argument 1
/* const int optional_argument = 2; UNUSED */

char* ntp_optarg;
static int ntp_optopt;
/* The variable ntp_optind [...] shall be initialized to 1 by the system. */
int ntp_optind = 1;
static int ntp_opterr;

/* Implemented based on [1] and [2] for optional arguments.
   ntp_optopt is handled FreeBSD-style, per [3].
   Other GNU and FreeBSD extensions are purely accidental.

[1] http://pubs.opengroup.org/onlinepubs/000095399/functions/getopt.html
[2] http://www.kernel.org/doc/man-pages/online/pages/man3/getopt.3.html
[3] http://www.freebsd.org/cgi/man.cgi?query=getopt&sektion=3&manpath=FreeBSD+9.0-RELEASE
*/
int ntp_getopt(int argc, char *const argv[], const char *optstring)
{
    int optchar = -1;
    const char* optdecl = NULL;
    static char* optcursor = NULL;	/* might not need to be static */

    ntp_optarg = NULL;
    ntp_opterr = 0;
    ntp_optopt = 0;

    /* Unspecified, but we need it to avoid overrunning the argv bounds. */
    if (ntp_optind >= argc) {
	optcursor = NULL;
	return -1;
    }

    /* If, when getopt() is called argv[ntp_optind] is a null pointer, getopt()
       shall return -1 without changing ntp_optind. */
    if (argv[ntp_optind] == NULL) {
	optcursor = NULL;
	return -1;
    }


    /* If, when getopt() is called *argv[ntp_optind]  is not the character '-',
       getopt() shall return -1 without changing ntp_optind. */
    if (*argv[ntp_optind] != '-') {
	optcursor = NULL;
	return -1;
    }

    /* If, when getopt() is called argv[ntp_optind] points to the string "-",
       getopt() shall return -1 without changing ntp_optind. */
    if (strcmp(argv[ntp_optind], "-") == 0) {
	optcursor = NULL;
	return -1;
    }

    /* If, when getopt() is called argv[ntp_optind] points to the string "--",
       getopt() shall return -1 after incrementing ntp_optind. */
    if (strcmp(argv[ntp_optind], "--") == 0) {
	++ntp_optind;
	optcursor = NULL;
	return -1;
    }

    if (optcursor == NULL || *optcursor == '\0')
	optcursor = argv[ntp_optind] + 1;

    optchar = *optcursor;

    /* FreeBSD: The variable ntp_optopt saves the last known option character
       returned by getopt(). */
    ntp_optopt = optchar;

    /* The getopt() function shall return the next option character (if one is
       found) from argv that matches a character in optstring, if there is
       one that matches. */
    optdecl = strchr(optstring, optchar);
    if (optdecl) {
	/* [I]f a character is followed by a colon, the option takes an
	   argument. */
	if (optdecl[1] == ':') {
	    ntp_optarg = ++optcursor;
	    if (*ntp_optarg == '\0') {
		/* GNU extension: Two colons mean an option takes an
		   optional arg; if there is text in the current argv-element
		   (i.e., in the same word as the option name itself, for example,
		   "-oarg"), then it is returned in ntp_optarg, otherwise ntp_optarg is set
		   to zero. */
		if (optdecl[2] != ':') {

		    /* If the option was the last character in the
		       string pointed to by an element of argv, then
		       ntp_optarg shall contain the next element of argv,
		       and ntp_optind shall be incremented by 2. If the
		       resulting value of ntp_optind is greater than argc,
		       this indicates a missing option-argument, and
		       getopt() shall return an error indication.

		       Otherwise, ntp_optarg shall point to the string
		       following the option character in that element
		       of argv, and ntp_optind shall be incremented by 1.
		    */
		    if (++ntp_optind < argc) {
			ntp_optarg = argv[ntp_optind];
		    } else {
			/* If it detects a missing option-argument, it shall return the
			   colon character ( ':' ) if the first character of optstring
			   was a colon, or a question-mark character ( '?' ) otherwise.
			*/
			ntp_optarg = NULL;
			optchar = (optstring[0] == ':') ? ':' : '?';
		    }
		} else {
		    ntp_optarg = NULL;
		}
	    }

	    optcursor = NULL;
	}
    } else {
	/* If getopt() encounters an option character that is not contained in
	   optstring, it shall return the question-mark ( '?' ) character. */
	optchar = '?';
    }

    if (optcursor == NULL || *++optcursor == '\0')
	++ntp_optind;

    return optchar;
}

/* Implementation based on [1].

[1] http://www.kernel.org/doc/man-pages/online/pages/man3/getopt.3.html
*/
int ntp_getopt_long(int argc, char *const argv[], const char *optstring,
		    const struct option *longopts, int *longindex)
{
    const struct option* o = longopts;
    const struct option* match = NULL;
    int num_matches = 0;
    size_t argument_name_length = 0;
    const char* current_argument = NULL;
    int retval = -1;

    ntp_optarg = NULL;
    ntp_optopt = 0;

    if (ntp_optind >= argc)
	return -1;

    if (strlen(argv[ntp_optind]) < 3 || strncmp(argv[ntp_optind], "--", 2) != 0)
	return ntp_getopt(argc, argv, optstring);

    /* It's an option; starts with -- and is longer than two chars. */
    current_argument = argv[ntp_optind] + 2;
    argument_name_length = strcspn(current_argument, "=");
    for (; o->name; ++o) {
	if (strncmp(o->name, current_argument, argument_name_length) == 0) {
	    match = o;
	    ++num_matches;
	}
    }

    if (num_matches == 1) {
	/* If longindex is not NULL, it points to a variable which is set to the
	   index of the long option relative to longopts. */
	if (longindex)
	    *longindex = (int)(match - longopts);

	/* If flag is NULL, then getopt_long() shall return val.
	   Otherwise, getopt_long() returns 0, and flag shall point to
	   a variable which shall be set to val if the option is
	   found, but left unchanged if the option is not found. */
	if (match->flag)
	    *(match->flag) = match->val;

	retval = match->flag ? 0 : match->val;

	if (match->has_arg != no_argument) {
	    ntp_optarg = strchr(argv[ntp_optind], '=');
	    if (ntp_optarg != NULL)
		++ntp_optarg;

	    if (match->has_arg == required_argument) {
		/* Only scan the next argv for required
		   arguments. Behavior is not specified, but has been
		   observed with Ubuntu and Mac OSX. */
		if (ntp_optarg == NULL && ++ntp_optind < argc) {
		    ntp_optarg = argv[ntp_optind];
		}

		if (ntp_optarg == NULL)
		    retval = ':';
	    }
	} else if (strchr(argv[ntp_optind], '=')) {
	    /* An argument was provided to a non-argument option.  I
	       haven't seen this specified explicitly, but both GNU
	       and BSD-based implementations show this behavior.
	    */
	    retval = '?';
	}
    } else {
	/* Unknown option or ambiguous match. */
	retval = '?';
    }

    ++ntp_optind;
    return retval;
}
