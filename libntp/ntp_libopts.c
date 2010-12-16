/*
 * ntp_libopts.c
 *
 * Common code interfacing with Autogen's libopts command-line option
 * processing.
 */
#include <stdio.h>
#include <stddef.h>
#include "ntp_libopts.h"
#include "autoopts/options.h"
#include "ntp_stdlib.h"

extern const char *Version;	/* version.c for each program */


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
	u_char		Opts[sizeof(*pOpts)];
	char **		ppzFullVersion;
	char *		pzNewFV;
	const char *	pzAutogenFV;
	size_t		octets;
	int		rc;

	memcpy(Opts, pOpts, sizeof(Opts));
	ppzFullVersion = (char **)(Opts + offsetof(struct options,
						   pzFullVersion));
	pzAutogenFV = *ppzFullVersion;
	octets = strlen(pzAutogenFV) +
		 1 +	/* '\n' */
		 strlen(Version) +
		 1;	/* '\0' */
	pzNewFV = emalloc(octets);
	snprintf(pzNewFV, octets, "%s\n%s", pzAutogenFV, Version);
	*ppzFullVersion = pzNewFV;
	rc = optionProcess((tOptions *)Opts, argc, argv);
	free(pzNewFV);

	return rc;
}
