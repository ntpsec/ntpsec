/*
 * Program to generate cryptographic keys for ntp clients and servers
 *
 * This program generates password encrypted data files for use with
 * the Network Time Protocol Version 4. Files are prefixed with a
 * header giving the name and date of creation followed by a
 * type-specific descriptive label and PEM-encoded data structure
 * compatible with programs of the OpenSSL library.
 *
 * All file names are like "ntpkey_<type>_<hostname>.<filestamp>", where
 * <type> is the file type, <hostname> the generating host name and
 * <filestamp> the generation time in NTP seconds. The NTP programs
 * expect generic names such as "ntpkey_<type>_whimsy.udel.edu" with the
 * association maintained by soft links. Following is a list of file
 * types; the first line is the file name and the second link name.
 *
 * ntpkey_MD5key_<hostname>.<filestamp>
 * 	MD5 (128-bit) keys used to compute message digests in symmetric
 *	key cryptography
 *
 * ntpkey_RSAhost_<hostname>.<filestamp>
 * ntpkey_host_<hostname>
 *	RSA private/public host key pair used for public key signatures
 *
 * ntpkey_RSAsign_<hostname>.<filestamp>
 * ntpkey_sign_<hostname>
 *	RSA private/public sign key pair used for public key signatures
 *
 * ntpkey_DSAsign_<hostname>.<filestamp>
 * ntpkey_sign_<hostname>
 *	DSA Private/public sign key pair used for public key signatures
 *
 * Available digest/signature schemes
 *
 * RSA:	RSA-MD2, RSA-MD5, RSA-SHA, RSA-SHA1, RSA-MDC2, EVP-RIPEMD160
 * DSA:	DSA-SHA, DSA-SHA1
 *
 * ntpkey_XXXcert_<hostname>.<filestamp>
 * ntpkey_cert_<hostname>
 *	X509v3 certificate using RSA or DSA public keys and signatures.
 *	XXX is a code identifying the message digest and signature
 *	encryption algorithm
 *
 * Identity schemes. The key type par is used for the challenge; the key
 * type key is used for the response.
 *
 * ntpkey_IFFkey_<groupname>.<filestamp>
 * ntpkey_iffkey_<groupname>
 *	Schnorr (IFF) identity parameters and keys
 *
 * ntpkey_GQkey_<groupname>.<filestamp>,
 * ntpkey_gqkey_<groupname>
 *	Guillou-Quisquater (GQ) identity parameters and keys
 *
 * ntpkey_MVkeyX_<groupname>.<filestamp>,
 * ntpkey_mvkey_<groupname>
 *	Mu-Varadharajan (MV) identity parameters and keys
 *
 * Note: Once in a while because of some statistical fluke this program
 * fails to generate and verify some cryptographic data, as indicated by
 * exit status -1. In this case simply run the program again. If the
 * program does complete with exit code 0, the data are correct as
 * verified.
 *
 * These cryptographic routines are characterized by the prime modulus
 * size in bits. The default value of 512 bits is a compromise between
 * cryptographic strength and computing time and is ordinarily
 * considered adequate for this application. The routines have been
 * tested with sizes of 256, 512, 1024 and 2048 bits. Not all message
 * digest and signature encryption schemes work with sizes less than 512
 * bits. The computing time for sizes greater than 2048 bits is
 * prohibitive on all but the fastest processors. An UltraSPARC Blade
 * 1000 took something over nine minutes to generate and verify the
 * values with size 2048. An old SPARC IPC would take a week.
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
#include "ntp_unixtime.h"

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
#include <ssl_applink.c>

#define _UC(str)	((char *)(intptr_t)(str))
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
void	followlink	(char *, size_t);

/*
 * Program variables
 */
extern char *optarg;		/* command line argument */
char	*progname;
u_int	lifetime = DAYSPERYEAR;	/* certificate lifetime (days) */
int	nkeys;			/* MV keys */
time_t	epoch;			/* Unix epoch (seconds) since 1970 */
u_int	fstamp;			/* NTP filestamp */
char	hostbuf[MAXHOSTNAME + 1];
char	*hostname = NULL;	/* host, used in cert filenames */
char	*groupname = NULL;	/* group name */
char	certnamebuf[2 * sizeof(hostbuf)];
char	*certname = NULL;	/* certificate subject/issuer name */
char	*passwd1 = NULL;	/* input private key password */
char	*passwd2 = NULL;	/* output private key password */
char	filename[MAXFILENAME + 1]; /* file name */

#ifdef SYS_WINNT
BOOL init_randfile();

/*
 * Don't try to follow symbolic links on Windows.  Assume link == file.
 */
int
readlink(
	char *	link,
	char *	file,
	int	len
	)
{
	return strlen(file);
}

/*
 * Don't try to create symbolic links on Windows, that is supported on
 * Vista and later only.  Instead, if CreateHardLink is available (XP
 * and later), hardlink the linkname to the original filename.  On
 * earlier systems, user must rename file to match expected link for
 * ntpd to find it.  To allow building a ntpkeygen.exe which loads on
 * Windows pre-XP, runtime link to CreateHardLinkA().
 */
int
symlink(
	char *	filename,
	char*	linkname
	)
{
	typedef BOOL (WINAPI *PCREATEHARDLINKA)(
		__in LPCSTR	lpFileName,
		__in LPCSTR	lpExistingFileName,
		__reserved LPSECURITY_ATTRIBUTES lpSA
		);
	static PCREATEHARDLINKA pCreateHardLinkA;
	static int		tried;
	HMODULE			hDll;
	FARPROC			pfn;
	int			link_created;
	int			saved_errno;

	if (!tried) {
		tried = TRUE;
		hDll = LoadLibrary("kernel32");
		pfn = GetProcAddress(hDll, "CreateHardLinkA");
		pCreateHardLinkA = (PCREATEHARDLINKA)pfn;
	}

	if (NULL == pCreateHardLinkA) {
		errno = ENOSYS;
		return -1;
	}

	link_created = (*pCreateHardLinkA)(linkname, filename, NULL);
	
	if (link_created)
		return 0;

	saved_errno = GetLastError();	/* yes we play loose */
	mfprintf(stderr, "Create hard link %s to %s failed: %m\n",
		 linkname, filename);
	errno = saved_errno;
	return -1;
}

void
InitWin32Sockets() {
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2,0);
	if (WSAStartup(wVersionRequested, &wsaData))
	{
		fprintf(stderr, "No useable winsock.dll\n");
		exit(1);
	}
}
#endif /* SYS_WINNT */


/*
 * followlink() - replace filename with its target if symlink.
 *
 * Some readlink() implementations do not null-terminate the result.
 */
void
followlink(
	char *	fname,
	size_t	bufsiz
	)
{
	int len;

	REQUIRE(bufsiz > 0);

	len = readlink(fname, fname, (int)bufsiz);
	if (len < 0 ) {
		fname[0] = '\0';
		return;
	}
	if (len > (int)bufsiz - 1)
		len = (int)bufsiz - 1;
	fname[len] = '\0';
}

#define ALL_OPTIONS "b:c:dD:e:G:HI:i:l:Mm:P:p:q:S:s:T:V:x:"
static const struct option longoptions[] = {
    { "imbits",		    1, 0, 'i' },
    { "cipher",		    1, 0, 'C' },
    { "debug",		    0, 0, 'd' },
    { "set-debug-level",    1, 0, 'D' },
    { "md5key",		    0, 0, 'M' },
    { "modulus",	    1, 0, 'm' },
    { NULL,                 0, 0, '\0'},
};

static char *opt_imbits = NULL;
static bool opt_md5key = false;
static int opt_modulus = -1;

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
	int	md5key = 0;	/* generate MD5 keys */
	int     op;

	progname = argv[0];

	init_lib();

#ifdef SYS_WINNT
	/* Initialize before OpenSSL checks */
	InitWin32Sockets();
	if (!init_randfile())
		fprintf(stderr, "Unable to initialize .rnd file\n");
	ssl_applink();
#endif

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
	passwd1 = hostbuf;
	passwd2 = NULL;
	clock_gettime(CLOCK_REALTIME, &ts);
	epoch = ts.tv_sec;
	fstamp = (u_int)(epoch + JAN_1970);

	while ((op = ntp_getopt_long(argc, argv,
				     ALL_OPTIONS, longoptions, NULL)) != -1) {

	    switch (op) {
	    case 'b':
		opt_imbits = ntp_optarg;
		break;
	    case 'd':
#ifdef DEBUG
		++debug;
#endif
		break;
	    case 'D':
#ifdef DEBUG
		debug = atoi(ntp_optarg);
#endif
		break;
	    case 'M':
		opt_md5key = true;
		break;
	    case 'm':
		opt_modulus = atoi(ntp_optarg);
		break;
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

	if (opt_md5key)
		md5key++;

	/*
	 * Create new unencrypted MD5 keys file if requested. If this
	 * option is selected, ignore all other options.
	 */
	if (md5key) {
		gen_md5("md5");
		exit (0);
	}

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
        if (strcmp(ulink, "md5") == 0) {
		strlcpy(linkname,"ntp.keys",sizeof(linkname));
        } else {
          snprintf(linkname, sizeof(linkname), "ntpkey_%s_%s", ulink,
                   hostname);
        }
	(void)remove(linkname);		/* The symlink() line below matters */
	temp = symlink(filename, linkname);
	if (temp < 0)
		perror(file);
	fprintf(stderr, "Generating new %s file and link\n", ulink);
	fprintf(stderr, "%s->%s\n", linkname, filename);
	fprintf(str, "# %s\n# %s\n", filename, ctime_r(&epoch, ascbuf));
	return (str);
}
