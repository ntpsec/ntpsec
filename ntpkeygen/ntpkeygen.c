/*
 * Program to generate cryptographic keys for ntp clients and servers
 *
 * All file names are like "ntpkey_<type>_<hostname>.<filestamp>", where
 * <type> is the file type, <hostname> the generating host name and
 * <filestamp> the generation time in NTP seconds. The NTP programs
 * expect generic names such as "ntpkey_<type>_whimsy.udel.edu" with the
 * association maintained by soft links. Following is a list of file
 * types.
 *
 * ntpkey_MD5key_<hostname>.<filestamp>
 * 	MD5 (128-bit) keys used to compute message digests in symmetric
 *	key cryptography
 *
 * The OpenSSL library used by this program expects a random seed file.
 * As described in the OpenSSL documentation, the file name defaults to
 * first the RANDFILE environment variable in the user's home directory
 * and then .rnd in the user's home directory.
 */

#include <config.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "ntp.h"
#include "ntp_stdlib.h"
#include "ntp_assert.h"
#include "ntp_calendar.h"

#ifdef HAVE_OPENSSL
#include "openssl/bn.h"
#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/rand.h"
#include "openssl/pem.h"
#include "openssl/x509v3.h"
#include <openssl/objects.h>
#endif	/* HAVE_OPENSSL */
#include <sodium.h>

/*
 * Cryptodefines
 */
#define	MD5KEYS		10	/* number of keys generated of each type */
#define	MD5SIZE		20	/* maximum key size */

/*
 * Prototypes
 */
FILE	*fheader	(const char *, const char *, const char *);
bool	gen_md5		(const char *);

/*
 * Program variables
 */
extern char *optarg;		/* command line argument */
char	*progname;
time_t	epoch;			/* Unix epoch (seconds) since 1970 */
u_int	fstamp;			/* NTP filestamp */
char	hostbuf[MAXHOSTNAME + 1];
char	*hostname = NULL;	/* host, used in cert filenames */
char	*groupname = NULL;	/* group name */
char	filename[MAXFILENAME + 1]; /* file name */

#define ALL_OPTIONS "Mh"
static const struct option longoptions[] = {
    { "md5key",		    0, 0, 'M' },
    { "help",		    0, 0, 'h' },
    { NULL,                 0, 0, '\0'},
};

/*
 * Main program
 */
int
main(
	int	argc,		/* command line options */
	char	**argv
	)
{
	struct timespec ts;	/* initialization vector */
	int     op;

	progname = argv[0];

#ifdef HAVE_OPENSSL
	ssl_check_version();
#endif	/* HAVE_OPENSSL */

	sodium_init();

	/*
	 * Process options, initialize host name and timestamp.
	 * gethostname() won't null-terminate if hostname is exactly the
	 * length provided for the buffer.
	 */
	gethostname(hostbuf, sizeof(hostbuf) - 1);
	hostbuf[COUNTOF(hostbuf) - 1] = '\0';
	hostname = hostbuf;
	groupname = hostbuf;
	clock_gettime(CLOCK_REALTIME, &ts);
	epoch = ts.tv_sec;
	fstamp = (u_int)(epoch + JAN_1970);

	while ((op = ntp_getopt_long(argc, argv,
				     ALL_OPTIONS, longoptions, NULL)) != -1) {

	    switch (op) {
	    case 'M':
		/* dummy MD5 option for backwards compatibility */
		break;
	    case 'h':
		fprintf(stderr, "usage: ntpkeygen [-M]\n" );
		exit(0);
	    default:
		fprintf(stderr, "usage: ntpkeygen [-M]\n" );
		exit(1);
	    }
	}
		
	argc -= optind;
	argv += optind;

#ifdef HAVE_OPENSSL
	if (SSLeay() == SSLEAY_VERSION_NUMBER)
		fprintf(stderr, "Using OpenSSL version %s\n",
			SSLeay_version(SSLEAY_VERSION));
	else
		fprintf(stderr, "Built against OpenSSL %s, using version %s\n",
			OPENSSL_VERSION_TEXT, SSLeay_version(SSLEAY_VERSION));
#endif /* HAVE_OPENSSL */

	gen_md5("md5");

	exit(0);
}


/*
 * Generate semi-random MD5 keys compatible with NTPv3 and NTPv4. Also,
 * if OpenSSL is around, generate random SHA1 keys compatible with
 * symmetric key cryptography.
 */
bool
gen_md5(
	const char *id		/* file name id */
	)
{
	uint8_t	md5key[MD5SIZE + 1];	/* MD5 key */
	FILE	*str;
	int	i, j;
#ifdef HAVE_OPENSSL
	uint8_t	keystr[MD5SIZE];
	uint8_t	hexstr[2 * MD5SIZE + 1];
	uint8_t	hex[] = "0123456789abcdef";
#endif	/* HAVE_OPENSSL */

	str = fheader("MD5key", id, groupname);
	for (i = 1; i <= MD5KEYS; i++) {
		for (j = 0; j < MD5SIZE; j++) {
			uint8_t temp;

			while (1) {
				randombytes_buf(
				    &temp, sizeof(temp));
				if (temp == '#')
					continue;

				if (temp > 0x20 && temp < 0x7f)
					break;
			}
			md5key[j] = temp;
		}
		md5key[j] = '\0';
		fprintf(str, "%2d MD5 %s  # MD5 key\n", i,
		    md5key);
	}
#ifdef HAVE_OPENSSL
	for (i = 1; i <= MD5KEYS; i++) {
		RAND_bytes(keystr, sizeof(keystr));
		for (j = 0; j < MD5SIZE; j++) {
			hexstr[2 * j] = hex[keystr[j] >> 4];
			hexstr[2 * j + 1] = hex[keystr[j] & 0xf];
		}
		hexstr[2 * MD5SIZE] = '\0';
		fprintf(str, "%2d SHA1 %s  # SHA1 key\n", i + MD5KEYS,
		    hexstr);
	}
#endif	/* HAVE_OPENSSL */
	fclose(str);
	return true;
}

/*
 * Generate file header and link
 */
FILE *
fheader	(
	const char *file,	/* file name id */
	const char *ulink,	/* linkname */
	const char *owner	/* owner name */
	)
{
	FILE	*str;		/* file handle */
	char	linkname[MAXFILENAME]; /* link name */
	int	temp;
        mode_t  orig_umask;
	char ascbuf[BUFSIZ];
        
	snprintf(filename, sizeof(filename), "ntpkey_%s_%s.%u", file,
	    owner, fstamp); 
        orig_umask = umask( S_IWGRP | S_IRWXO );
        str = fopen(filename, "w");
        (void) umask(orig_umask);
	if (str == NULL) {
		perror("Write");
		exit (-1);
	}
	/* NAMECHANGE */
	strlcpy(linkname,"ntp.keys",sizeof(linkname));
	(void)remove(linkname);		/* The symlink() line below matters */
	temp = symlink(filename, linkname);
	if (temp < 0)
		perror(file);
	fprintf(stderr, "Generating new %s file and link\n", ulink);
	fprintf(stderr, "%s->%s\n", linkname, filename);
	fprintf(str, "# %s\n# %s\n", filename, ctime_r(&epoch, ascbuf));
	return (str);
}
