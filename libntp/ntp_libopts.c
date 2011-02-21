/*
 * ntp_libopts.c
 *
 * Common code interfacing with Autogen's libopts command-line option
 * processing.
 */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stddef.h>
#include "ntp_libopts.h"
#include "ntp_stdlib.h"

extern const char *Version;	/* version.c for each program */

typedef struct options_fv_tag {
	u_char		padding[offsetof(tOptions, pzFullVersion)];
	const char *	pzFullVersion;
} options_fv;

typedef union options_const_trick_tag {
	tOptions	orig;	/* for alignment */
	options_fv	fv;
} options_const_trick;

/*
 * ntpOptionProcess() is a clone of libopts' optionProcess which
 * overrides the --version output, appending detail from version.c
 * which was not available at Autogen time.
 */
int
ntpOptionProcess(
	tOptions *	pOpts,
	int		argc,
	char **		argv
	)
{
	options_const_trick *	pOpts_fv;
	const char *		pzAutogenFV;
	char *			pzNewFV;
	size_t			octets;
	int			rc;

	pOpts_fv = (void *)pOpts;
	pzAutogenFV = pOpts_fv->fv.pzFullVersion;
	octets = strlen(pzAutogenFV) +
		 1 +	/* '\n' */
		 strlen(Version) +
		 1;	/* '\0' */
	pzNewFV = emalloc(octets);
	snprintf(pzNewFV, octets, "%s\n%s", pzAutogenFV, Version);
	pOpts_fv->fv.pzFullVersion = pzNewFV;
	rc = optionProcess(pOpts, argc, argv);
	pOpts_fv->fv.pzFullVersion = pzAutogenFV;
	free(pzNewFV);

	return rc;
}
