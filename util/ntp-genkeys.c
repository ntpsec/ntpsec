/*
 * Program to generate cryptographic keys for NTP clients and servers
 *
 * TODO:
 * - do we need to make a symlink to the private key file?
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_crypto.h"
#include "ntp_stdlib.h"
#include "ntp_filegen.h"
#include "ntp_config.h"
#include "ntp_cmdargs.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef OPENSSL
# include "openssl/evp.h"
# include "openssl/err.h"
# include "openssl/rand.h"
# include "openssl/pem.h"
# include "openssl/x509.h"
#endif /* OPENSSL */

/*
 * Cryptodefines
 */
#define	MODULUSLEN	512	/* length of RSA/DSA modulus */
#define	PRIMELEN	512	/* length of DH prime */
#define	MD5KEYS		16	/* number of MD5 keys generated */
#define	PATH_MAX	255	/* max file name length */
#define	JAN_1970	ULONG_CONST(2208988800) /* NTP seconds at the epoch */
#define YEAR		((long)60*60*24*365) /* one year in seconds */
/*
 * Prototypes
 */
void	crypto_config P((int, char *));
FILE	*fheader P((char *, char *, char *, u_long, char *));	/* construct file header */
void	getCmdOpts P((int, char**));
int	genkeys  P((void));
int	genrest  P((void));
int	genthings  P((void));
char	*getpath P((char *, char *, char *));
void	set_filenames P((void));
void	sub_token P((char *, char *, char *, char *));
void	usage	 P((void));

#ifdef OPENSSL
u_long	asn2ntp	 P((ASN1_TIME *));	/* ASN.1 time format to NTP seconds */
void	cb	 P((int, int, void *));	/* callback routine */
void	genkey_dsa P((char *, char *, char *, u_long));
void	genkey_rsa P((char *, char *, char *, u_long));
int	x509	 P((u_char *, EVP_PKEY *, EVP_MD *, int)); /* generate req/cert */
#endif /* OPENSSL */

/*
 * Program variables
 */
struct timeval tv;		/* initialization vector */
u_long	ntptime;		/* NTP epoch */
char	hostname[PATH_MAX];	/* host name */
extern	char *keysdir;
#ifdef OPENSSL
long	d0, d1, d2, d3;		/* callback counters */
#endif /* OPENSSL */

char *f1_keys;			/* Visible MD5 key file name */
char f2_keys[PATH_MAX];		/* timestamped */

char *f0_privatekey = "ntpkey_key_HOSTNAME";
char f1_privatekey[PATH_MAX];
char f2_privatekey[PATH_MAX];

char *f0_signkey = "ntpkey_sign_HOSTNAME";
char f1_signkey[PATH_MAX];	/* from ntp.conf */
char f2_signkey[PATH_MAX];	/* generated filename */
char f3_signkey[PATH_MAX];	/* generate new certs using this key */

char *f0_cert = "ntpkey_cert_HOSTNAME";
char f1_cert[PATH_MAX];
char f2_cert[PATH_MAX];

char tmp_name[PATH_MAX];

/* Stubs and hacks so we can link with ntp_config.o */

struct interface *any_interface; /* default interface */
u_long  client_limit;
u_long  client_limit_period;
keyid_t ctl_auth_keyid;		/* keyid used to authenticate write requests */
u_long  current_time;		/* current time (s) */
volatile int debug = 0;		/* debugging flag */
keyid_t info_auth_keyid;	/* keyid used to authenticate requests */
char *  req_file;		/* name of the file with configuration info */
keyid_t req_keyid;		/* request keyid */
u_long  sys_automax;		/* maximum session key lifetime */
int     sys_bclient;		/* we set our time to broadcasts */
int     sys_manycastserver;	/* 1 => respond to manycast client pkts */
u_char  sys_minpoll;		/* min poll interval (log2 s) */
u_char  sys_revoke;		/* keys revoke timeout */
l_fp    sys_revoketime;
u_char	sys_ttl[1];
int	sys_ttlmax;
const char *Version = "";	/* version declaration */


struct peer *
peer_config(
	struct sockaddr_in *srcadr,
	struct interface *dstadr,
	int hmode,
	int version,
	int minpoll,
	int maxpoll,
	u_int flags,
	int ttl,
	keyid_t key,
	u_char *keystr
	)
{
	static struct peer foo;

	if (debug > 1) printf("peer_config...\n");
	return &foo;
}


void
set_sys_var(
	char *data,
	u_long size,
	int def
	)
{
	if (debug > 1) printf("set_sys_var...\n");
	return;
}


void
ntp_intres (void)
{
	if (debug > 1) printf("ntp_intres...\n");
	return;
}


int
ctlsettrap(
	struct sockaddr_in *raddr,
	struct interface *linter,
	int traptype,
	int version
	)
{
	if (debug > 1) printf("ctlsettrap...\n");
	return 0;
}


void
crypto_config(
	int item,		/* configuration item */
	char *cp		/* file name */
	)
{
	switch (item) {
#ifdef OPENSSL
	    case CRYPTO_CONF_PRIV:
		if (debug > 1)
			printf("crypto_config: PRIVATEKEY/<%d> <%s>\n",
			       item, cp);
		f0_privatekey = strdup(cp);
		break;
	    case CRYPTO_CONF_SIGN:
		if (debug > 1)
			printf("crypto_config: SIGNKEY/<%d> <%s>\n",
			       item, cp);
		f0_signkey = strdup(cp);
		break;
	    case CRYPTO_CONF_KEYS:
		if (debug > 1)
			printf("crypto_config: KEYSDIR/<%d> <%s>\n", item, cp);
		keysdir = strdup(cp);
		break;
	    case CRYPTO_CONF_CERT:
		if (debug > 1)
			printf("crypto_config: CERT/<%d> <%s>\n", item, cp);
		f0_cert = strdup(cp);
		break;
#endif /* OPENSSL */
	    default:
		if (debug > 1)
			printf("crypto_config: <%d> <%s>\n", item, cp);
		break;
	}
	return;
}


struct interface *
findinterface(
	struct sockaddr_in *addr
	)
{
 	if (debug > 1) printf("findinterface...\n");
	return 0;
}


void
refclock_control(
	struct sockaddr_in *srcadr,
	struct refclockstat *in,
	struct refclockstat *out
	)
{
	if (debug > 1) printf("refclock_control...\n");
	return;
}


void
loop_config(
	int item,
	double freq
	)
{
	if (debug > 1) printf("loop_config...\n");
	return;
}


void
filegen_config(
	FILEGEN *gen,
	char    *basename,
	u_int   type,
	u_int   flag
	)
{
	if (debug > 1) printf("filegen_config...\n");
	return;
}


void
stats_config(
	int item,
	char *invalue	/* only one type so far */
	)
{
	if (debug > 1) printf("stats_config...\n");
	return;
}


void
hack_restrict(
	int op,
	struct sockaddr_in *resaddr,
	struct sockaddr_in *resmask,
	int mflags,
	int flags
	)
{
	if (debug > 1) printf("hack_restrict...\n");
	return;
}


void
kill_asyncio (void)
{
	if (debug > 1) printf("kill_asyncio...\n");
	return;
}


void
proto_config(
	int item,
	u_long value,
	double dvalue
	)
{
	if (debug > 1) printf("proto_config...\n");
	return;
}

void
getauthkeys(
	char *keyfile
	)
{
	if (debug > 1) printf("getauthkeys: got <%s>\n", keyfile);
	f1_keys = strdup(keyfile);
	return;
}


FILEGEN *
filegen_get(
	char *name
	)
{
	if (debug > 1) printf("filegen_get...\n");
	return 0;
}


/* End of stubs and hacks */


/*
 * Main program
 *
 * This program generates files "ntpkey_<type>_<hostname>.<filestamp>",
 * where <type> is the file type, <hostname> is the generating host and
 * <filestamp> is the NTP seconds in decimal format. The NTP programs
 * expect generic names such as "ntpkey_key_whimsy.udel.edu" with the
 * connection being maintained by soft links.
 *
 * Files are prefixed with a header giving the name and date of creation
 * followed by a type-specific descriptive label and PEM-encoded data
 * string compatible with programs of the OpenSSL distribution.
 *
 * Command-line arguments:
 *
 *	-[cC] {DSA_{SHA1,SHA},RSA_{MD2,MD5,MDC2,RIPEMD160,SHA1,SHA}}...
 *			default: [RSA_MD5]
 *			C means generate a link from ntpkey_cert_HOSTNAME
 *			First one in the list gets the link.
 *	-d		Generate the DH files
 *	-f config.file	Parse the given config file (instead of reading
 *			/etc/ntp.conf?) for the directory and file names.
 *	-m		Generate the MD5 symmetric key file
 *	-[rR] RSA	Generate the rsakey (a host key, RSA only)
 *			R means generate a link from ntpkey_key_HOSTNAME
 *	-[sS] {DSA,RSA}	Generate a sign key
 *			S means generate a link from ntpkey_sign_HOSTNAME
 *	-t stampfile	Write the filestamp extension to the stampfile.
 *	-v		Be verbose
 *	hostname ...	Generate keys for the listed hostnames
 *			NB: USE OF THIS IS A VIOLATION OF SECURITY PROTOCOLS
 *
 * Note that if multiple hostnames are specified we have to make a choice
 * about the filenames we get by reading an ntp.conf file.
 */

#define GEN_DSA		1
#define GEN_RSA		2
#define GEN_CERT	4
#define GEN_LINK	8
#define GEN_DSA_L	(GEN_DSA | GEN_LINK)
#define GEN_RSA_L	(GEN_RSA | GEN_LINK)
#define GEN_CERT_L	(GEN_CERT | GEN_LINK)

struct certlist {
	char *cert_name;
	unsigned int cert_bit;
};

struct certlist certlist[] = {
#define CERT_DSA_SHA1	0x01
	{ "DSA_SHA1",	CERT_DSA_SHA1 },
#define CERT_DSA_SHA	0x02
	{ "DSA_SHA",	CERT_DSA_SHA },
#ifdef HAVE_EVP_MD2
#define CERT_RSA_MD2	0x04
	{ "RSA_MD2",	CERT_RSA_MD2 },
#endif
#define CERT_RSA_MD5	0x08
	{ "RSA_MD5",	CERT_RSA_MD5 },
#ifdef HAVE_EVP_MDC2
#define CERT_RSA_MDC2	0x10
	{ "RSA_MDC2",	CERT_RSA_MDC2 },
#endif
#define CERT_RSA_RIPEMD160	0x20
	{ "RSA_RIPEMD160",	CERT_RSA_RIPEMD160 },
#define CERT_RSA_SHA	0x40
	{ "RSA_SHA",	CERT_RSA_SHA },
#define CERT_RSA_SHA1	0x80
	{ "RSA_SHA1",	CERT_RSA_SHA1 },
	{ 0,		0 }
};

int certlist_n = (sizeof certlist / sizeof *certlist) - 1;

char *cl_name;			/* What cert do we use for links? */
unsigned int cl_bit;		/* config bit for cert link */
char *config_file = CONFIG_FILE; /* Default location for ntp.conf */
int gen_cert;			/* Generate a cert? */
unsigned int gen_certs;		/* bitmap of certs to build */
int gen_dh;			/* Generate DH files? */
int gen_rsa;			/* Generate an RSA key? */
int gen_sign;			/* Generate a sign key? */
int gen_skf;			/* Generate symmetric key file? */
char *stampfile;		/* Stampfile name */
int verbose;			/* Be verbose? */

char const *progname;

extern int call_resolver;


void
usage (
	void
	)
{
	int i;

	printf("Usage:%s [ -[cC] {", progname);
	for (i = 0; i < certlist_n; ++i)
		printf("%s%s", (i) ? "," : "", certlist[i].cert_name);
	printf("}... ]\n");
	printf("\t[ -d ] [ -f config.file ] [ -m ] [ -[rR] RSA ] [ -[sS] {DSA,RSA} ]\n");
	printf("\t[ -t stampfile ] [ -v ] [ hostname ... ]\n");
	exit(1);
}


void
sub_token (
	char *src,
	char *token,
	char *rep,
	char *dst
	)
{
	char *bot = strstr(src, token);
	size_t t_len = strlen(token);

	if (bot) {
		*dst = '\0';
		strncat(dst, src, (bot - src));
		strcat(dst, rep);
		strcat(dst, src + (bot - src) + t_len);
	} else {
		strcpy(dst, src);
	}
	return;
}


void
set_filenames ( void )
{
	sub_token(f0_cert,	 "HOSTNAME", hostname, f1_cert);
	sub_token(f0_privatekey, "HOSTNAME", hostname, f1_privatekey);
	sub_token(f0_signkey,	 "HOSTNAME", hostname, f1_signkey);
}


void
getCmdOpts (
	int argc,
	char *argv[]
	)
{
	int i, j, errflag = 0;
	int gotone = 0;

	progname = argv[0];

	while ((i = ntp_getopt(argc, argv, "c:C:df:mr:R:s:S:t:v")) != EOF)
		switch (i) {

		    case 'C':	/* Generate a symlink and */
			gen_cert = GEN_LINK;
			/* FALL THROUGH */
		    case 'c':	/* Generate a cert */
			gen_certs = 0;
			++gotone;
			/* process each item in the list */
			{
				for (j = 0; j < certlist_n; ++j)
					if (!strcmp(certlist[j].cert_name,
						    ntp_optarg)) {
						if (0 == cl_name) {
							cl_name = ntp_optarg;
							cl_bit = certlist[j].cert_bit;
						}
						gen_certs |= certlist[j].cert_bit;
						break;
					}
				if (j == certlist_n) {
					/* Show the offender? */
					++errflag;
				}
			}
			gen_cert |= GEN_CERT;
			break;
		    case 'd':	/* Create the DH files */
			++gen_dh;
			++gotone;
			break;
		    case 'f':	/* Where is the config file? */
			config_file = ntp_optarg;
			break;
		    case 'm':	/* Generate MD5 symmetric key file */
			++gen_skf;
			++gotone;
			break;

		    case 'R':	/* Generate a symlink and */
			gen_rsa = GEN_LINK;
		    case 'r':	/* Generate an RSA key */
			++gotone;
			/* "RSA" is the only allowed/required arg */
			if (!strcmp(ntp_optarg, "RSA"))
				gen_rsa |= GEN_RSA;
			else
				++errflag;
			break;

		    case 'S':	/* Generate a symlink and */
			gen_sign = GEN_LINK;
			/* FALL THROUGH */
		    case 's':	/* Generate a sign key */
			gen_sign &= GEN_LINK;
			++gotone;
			/* DSA or RSA are the only two things allowed here */
			if (!strcmp(ntp_optarg, "DSA"))
				gen_sign |= GEN_DSA;
			else if (!strcmp(ntp_optarg, "RSA"))
				gen_sign |= GEN_RSA;
			else
				++errflag;
			break;
		    case 't':	/* write the filestamp extension */
			stampfile = ntp_optarg;
			break;
		    case 'v':	/* Be verbose */
			++verbose;
			break;
		    case '?':
			++errflag;
			break;
		}

	/*
	 * If we're generating:
	 * - a cert and a link AND an rsakey and no link
	 * maybe we should squawk.
	 */

	/*
	 * If we're generating a signkey and a cert, make sure they are of
	 * compatible types.
	 */

	if (errflag)
		usage();

	if (!gotone)
		gen_certs = CERT_RSA_MD5;
}

int
main(
	int argc,
	char *argv[]
	)
{
	int i;

#ifdef OPENSSL
	if (SSLeay() != OPENSSL_VERSION_NUMBER) {
		printf("OpenSSL version mismatch. Built against %lx, you have %lx\n",
			OPENSSL_VERSION_NUMBER, SSLeay());
		exit(1);
	} else {
		printf("OpenSSL version %lx\n", SSLeay());
	}
#endif

	call_resolver = 0;	/* Skip the resolver when cracking ntp.conf */
	getconfig(argc, argv);	/* ntpd/ntp_config.c */

	/*
	 * Initialize the timestamp.
	 */
	gettimeofday(&tv, 0);
	ntptime = tv.tv_sec + JAN_1970;

	i = 0;
	if (ntp_optind == argc) { /* No more args - generate key for us */
		if (-1 == gethostname(hostname, sizeof(hostname))) {
			perror("gethostname()");
			exit(1);
		}

		set_filenames();

		i |= genthings();
	} else while (ntp_optind < argc) {
		strncpy(hostname, argv[ntp_optind], sizeof hostname);

		set_filenames();

		i |= genthings();

		++ntp_optind;
	}
	/*
	 * If there were no problems and we want to update the stampfile,
	 * do so.
	 */
	if (stampfile && *stampfile && !i) {
		FILE *fp;

		fp = fopen(stampfile, "a");
		if (fp == NULL) {
			fprintf(stderr, "%s: fopen(%s) failed: %s\n",
				progname, stampfile, strerror(errno));
			exit(1);
		}
		fprintf(fp, "%lu\n", ntptime);
		fclose(fp);
	}
	return(0);
}


int
genthings(
	void
	)
{
	int rc = 0;
	int i;
	char *cp;


	printf("Generating things for %s...\n", hostname);

	/*
	 * call genkeys to generate any needed xSA keys.
	 *
	 * Make sure f1_signkey is pointing to the correct sign key.
	 */

	rc |= genkeys();

	/*
	 * call genrest to generate the rest of the stuff.
	 */

	rc |= genrest();

	if (gen_certs) {
		printf("Some requested certs were not made:\n");
		for (i = 0; i < certlist_n; ++i) {
			if (gen_certs & certlist[i].cert_bit)
				printf("%s\n", certlist[i].cert_name);
		}
	}

	/*
	 * make the (sym)links.
	 */

	/* keysdir if f1_ doesn't begin with '/' */

	if (gen_rsa & GEN_LINK) {
		cp = getpath(keysdir, f1_privatekey, NULL);
		if (verbose)
			printf("rsakey symlink(%s, %s)\n", f2_privatekey, cp);
		(void)unlink(cp);
		if (symlink(f2_privatekey, cp)) {
			fprintf(stderr, "rsakey symlink(%s, %s): %s\n",
				f2_privatekey, cp, strerror(errno));
		}
	}
	if (gen_sign & GEN_LINK) {
		cp = getpath(keysdir, f1_signkey, NULL);
		if (verbose)
			printf("signkey symlink(%s, %s)\n",
			       f2_signkey, cp);
		(void)unlink(cp);
		if (symlink(f2_signkey, cp)) {
			fprintf(stderr, "signkey symlink(%s, %s): %s\n",
				f2_signkey, cp, strerror(errno));
		}
	}
	if (gen_cert & GEN_LINK) {
		cp = getpath(keysdir, f1_cert, NULL);
		if (verbose)
			printf("cert symlink(%s, %s)\n",
			       f2_cert, cp);
		(void)unlink(cp);
		if (symlink(f2_cert, cp)) {
			fprintf(stderr, "cert symlink(%s, %s): %s\n",
				f2_cert, cp, strerror(errno));
		}
	}

	/* Might we need a link to the MD5 keyfile? */

	return rc;
}


int
genkeys(
	void
	)
{
#ifdef OPENSSL
	char	pathbuf[PATH_MAX];
	u_int	temp;
	char	*cp;
	int	gotsignkey;

	/*
	 * Here's the rub: The sign key defaults to the rsakey.  The rsakey
	 * must be an RSA key, the sign key may be RSA or DSA.  Therefore,
	 * the sign key will use a different timestamp to make sure that an
	 * RSA encrypt key and an RSA sign key don't have filename
	 * collisions.
	 */

	/*
	 * Seed random number generator and grow weeds.
	 */
	if (RAND_file_name(pathbuf, PATH_MAX) == NULL) {
		printf("RAND_file_name %s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		exit (-1);
	}
	temp = RAND_load_file(pathbuf, -1);
	if (temp == 0) {
		printf("RAND_load_file <%s>: \n\t%s\n",
		    pathbuf,
		    ERR_error_string(ERR_get_error(), NULL));
		exit (-1);
	}
	printf("Random seed file %s bytes %u\n", pathbuf, temp);
	temp = random();
	RAND_add(&temp, sizeof(temp), 4.0);
	RAND_write_file(pathbuf);   
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();

	/*
	 * If we're making an RSA sign key and an RSA encrypt key we
	 * must wiggle the timestamp to avoid a collision.  Wiggle the
	 * encrypt key in this case.
	 */
	if (gen_rsa) {
		if (gen_sign & GEN_RSA)
			genkey_rsa("rsakey", f1_privatekey, f2_privatekey,
				   ntptime - 1);
		else
			genkey_rsa("rsakey", f1_privatekey, f2_privatekey,
				   ntptime);
	}
	if (gen_sign) {
		if (gen_sign & GEN_RSA)
			genkey_rsa("signkey", f1_signkey, f2_signkey, ntptime);
		else
			genkey_dsa("signkey", f1_signkey, f2_signkey, ntptime);
	}

	/*
	 * Make sure f3_signkey is pointing to the right file.
	 *
	 * - If we built a sign key, use it.
	 * - If ntp.conf specifies or defaults an existing sign key, use it.
	 * - If we built an rsakey, use it.
	 * - If ntp.conf specifies or defaults an existing rsakey, use it.
	 */
	gotsignkey = 0;
	if (!gotsignkey && *f2_signkey) { /* We built a sign key - use it. */
		cp = getpath(keysdir, f1_signkey, f2_signkey);
		if (!cp)
			exit(-1);
		strcpy(f3_signkey, cp);
		if (debug > 1)
			printf("f3: GS1: <%s>\n", f3_signkey);
		++gotsignkey;
	}
	if (!gotsignkey && *f1_signkey)	{ /* Use sign key from ntp.conf */
		struct stat sb;

		cp = getpath(keysdir, f1_signkey, NULL);
		if (!cp)
			exit(-1);
		if (stat(cp, &sb) == 0) {
			strcpy(f3_signkey, cp);
			if (debug > 1)
				printf("f3: ES: <%s>\n", f3_signkey);
			++gotsignkey;
		}
	}
	if (!gotsignkey && *f2_privatekey) { /* We built an rsakey - use it. */
		cp = getpath(keysdir, f1_privatekey, f2_privatekey);
		if (!cp)
			exit(-1);
		strcpy(f3_signkey, cp);
		if (debug > 1)
			printf("f3: GR1: <%s>\n", f3_signkey);
		++gotsignkey;
	}
	if (!gotsignkey && *f1_privatekey) { /* Use rsakey from ntp.conf */
		struct stat sb;

		cp = getpath(keysdir, f1_privatekey, NULL);
		if (!cp)
			exit(-1);
		if (stat(cp, &sb) == 0) {
			strcpy(f3_signkey, cp);
			if (debug > 1)
				printf("f3: ER: <%s>\n", f3_signkey);
			++gotsignkey;
		}
	}

	if (!gotsignkey)
		printf("No key found - no certs can be generated.\n");
#endif /* OPENSSL */

	return (0);
}


#ifdef OPENSSL
void
genkey_rsa (
	char *what,		/* What type of RSA key? */
	char *f1_key,		/* file path */
	char *f2_key,		/* target */
	u_long tstamp		/* timestamp */
	)
{
	EVP_PKEY *pkey;		/* public/private keys */
	FILE	*str;
	RSA	*rsa;		/* RSA keys */

	/*
	 * Generate random RSA keys.
	 */
	printf("Generating RSA %s (%d bits)...\n", what, MODULUSLEN);
	rsa = RSA_generate_key(MODULUSLEN, 3, cb, "RSA_keys");
	printf("\n");
	if (rsa == NULL) {
		printf("RSA generate keys fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		exit (-1);
	}

	/*
	 * For signature encryption it is not necessary that the RSA
	 * parameters be strictly groomed and once in a while the
	 * modulus turns out to be non-prime. Just for grins, we check
	 * the primality. If this fails, disregard or run the program
	 * again.
	 */
	if (!RSA_check_key(rsa)) {
		printf("Invalid RSA key\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
	}
	pkey = EVP_PKEY_new();
	EVP_PKEY_assign_RSA(pkey, rsa);
	str = fheader("RSAkey", keysdir, f1_key, tstamp, f2_key);
	PEM_write_RSAPrivateKey(str, rsa, NULL, NULL, 0, NULL, NULL);
	fclose(str);
/*
	RSA_print_fp(stdout, pkey->pkey.rsa, 0);
*/
	free(pkey);
	return;
}


void
genkey_dsa(
	char *what,		/* What type of RSA key? */
	char *f1_key,		/* file path */
	char *f2_key,		/* target */
	u_long tstamp		/* timestamp */
	)
{
	EVP_PKEY *pkey;		/* public/private keys */
	FILE	*str;
	DSA	*dsa_params;	/* DSA parameters */
	u_char	seed[20];	/* seed for DSA parameters */

	/*
	 * Generate DSA parameters.
	 */
	printf("Generating DSA %s parameters (%d bits)...\n", what,
	       MODULUSLEN);
	dsa_params = DSA_generate_parameters(MODULUSLEN, seed,
	    sizeof(seed), NULL, NULL, cb, "DSA_params");
	printf("\n");
	if (dsa_params == NULL) {
		printf("DSA generate parameters fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		 exit (-1);
	}
	str = fheader("DSApar", keysdir, f1_key, tstamp, tmp_name);
	PEM_write_DSAparams(str, dsa_params);
	fclose(str);

	/*
	 * Generate DSA keys. Note, the digest algorithms that work with
	 * DSS (DSA) are DSS and DSS1.
	 */
	printf("Generating DSA %s (%d bits)...\n", what, MODULUSLEN);
	if (!DSA_generate_key(dsa_params)) {
		printf("DSA generate keys fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		exit (-1);
	}
	pkey = EVP_PKEY_new();
	EVP_PKEY_assign_DSA(pkey, dsa_params);
	str = fheader("DSAkey", keysdir, f1_key, ntptime, f2_key);
	PEM_write_DSAPrivateKey(str, dsa_params, NULL, NULL, 0, NULL,
	    NULL);
	fclose(str);
/*
	DSA_print_fp(stdout, pkey->pkey.dsa, 0);
*/
	free(pkey);
	return;
}
#endif /* OPENSSL */


int
genrest(
	void
	)
{
#ifdef OPENSSL
	EVP_PKEY *pkey;		/* public/private keys */
	DH	*dh_params;	/* Diffie-Hellman parameters */
	int	codes;		/* DH check codes */
	/* Vars from crypto_key(): */
	char	*cp = f1_signkey;
	char	filename[MAXFILENAME]; /* name of rsa key file */
	char	linkname[MAXFILENAME]; /* file link (for filestamp) */
	u_char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
	tstamp_t fstamp;		/* filestamp */
	int	rval;
	u_char	*ptr;
#endif /* OPENSSL */
	u_char	md5key[16];
	FILE	*str;
	u_int	temp;
	int	i, j;

	if (gen_skf) {
		/*
		 * Generate semi-random MD5 keys.
		 */
		printf("Generating MD5 keys...\n");
		srandom((u_int)tv.tv_usec);
		str = fheader("MD5key", keysdir, f1_keys, ntptime, f2_keys);
		for (i = 1; i <= MD5KEYS; i++) {
			for (j = 0; j < 16; j++) {
				while (1) {
					temp = random() & 0xff;
					if (temp == '#')
						continue;
					if (temp > 0x20 && temp < 0x7f)
						break;
				}
				md5key[j] = (u_char)temp;
			}
			md5key[16] = '\0';
			fprintf(str, "%2d MD5 %16s	# MD5 key\n", i,
				md5key);
		}
		fclose(str);
	}

#ifdef OPENSSL
	if (gen_certs) {
		/*
		 * This routine must do a file-read to get the key that will
		 * be used to generate the certs.  We can check this key to
		 * make sure it's the right type, and squawk if the sign key
		 * type does not match the requested cert types.
		 */

		str = fopen(f3_signkey, "r");
		if (str == NULL) {
			fprintf(stderr, "fopen(%s) failed: %s\n", f3_signkey,
				strerror(errno));
			return (NULL);
		}

		/*
		 * Read PEM-encoded key.
		 */
		pkey = PEM_read_PrivateKey(str, NULL, NULL, NULL);
		fclose(str);
		if (pkey == NULL) {
			msyslog(LOG_ERR, "crypto_key %s",
				ERR_error_string(ERR_get_error(), NULL));
			return (NULL);
		}

		/*
		 * Extract filestamp if present.
		 */
		rval = readlink(filename, linkname, MAXFILENAME - 1);
		if (rval > 0) {
			linkname[rval] = '\0';
			ptr = strrchr(linkname, '.');
		} else {
			ptr = strrchr(filename, '.');
		}
		if (ptr != NULL)
			sscanf(++ptr, "%u", &fstamp);
		else
			fstamp = 0;
# if 0
		vp->fstamp = htonl(fstamp);
# endif
		sprintf(statstr, "%s link %d fs %u mod %d", cp, rval, fstamp,
			EVP_PKEY_size(pkey) * 8);
# if 0
		record_crypto_stats(NULL, statstr);
# endif
# ifdef DEBUG
		if (debug)
			printf("crypto_key: %s\n", statstr);
		if (debug > 1) {
			if (EVP_MD_type(pkey) == EVP_PKEY_DSA)
				DSA_print_fp(stdout, pkey->pkey.dsa, 0);
			else
				RSA_print_fp(stdout, pkey->pkey.rsa, 0);
		}
# endif

		if (EVP_MD_type(pkey) == EVP_PKEY_RSA) {
			/*
			 * For signature encryption it is not necessary that
			 * the RSA parameters be strictly groomed and once
			 * in a while the modulus turns out to be
			 * non-prime. Just for grins, we check the
			 * primality. If this fails, disregard or run the
			 * program again.
			 */
			if (!RSA_check_key(pkey->pkey.rsa)) {
				printf("Invalid RSA key\n%s\n",
				       ERR_error_string(ERR_get_error(),
							NULL));
			}
# if 0
			RSA_print_fp(stdout, pkey->pkey.rsa, 0);
# endif
			/*
			 * Generate the X509 certificate request. The digest
			 * algorithms that work with RSA are MD2, MD5, SHA,
			 * SHA1, MDC2 and RIPEMD160.
			 */
#ifdef HAVE_EVP_MD2
			if (gen_certs & CERT_RSA_MD2) {
				x509("RSA_MD2", pkey, EVP_md2(),
				     cl_bit & CERT_RSA_MD2);
				gen_certs &= ~CERT_RSA_MD2;
			}
#endif
			if (gen_certs & CERT_RSA_MD5) {
				x509("RSA_MD5", pkey, EVP_md5(),
				     cl_bit & CERT_RSA_MD5);
				gen_certs &= ~CERT_RSA_MD5;
			}
			if (gen_certs & CERT_RSA_SHA) {
				x509("RSA_SHA", pkey, EVP_sha(),
				     cl_bit & CERT_RSA_SHA);
				gen_certs &= ~CERT_RSA_SHA;
			}
			if (gen_certs & CERT_RSA_SHA1) {
				x509("RSA_SHA1", pkey, EVP_sha1(),
				     cl_bit & CERT_RSA_SHA1);
				gen_certs &= ~CERT_RSA_SHA1;
			}
#ifdef HAVE_EVP_MDC2
			if (gen_certs & CERT_RSA_MDC2) {
				x509("RSA_MDC2", pkey, EVP_mdc2(),
				     cl_bit & CERT_RSA_MDC2);
				gen_certs &= ~CERT_RSA_MDC2;
			}
#endif
			if (gen_certs & CERT_RSA_RIPEMD160) {
				x509("RSA_RIPEMD160", pkey, EVP_ripemd160(),
				     cl_bit & CERT_RSA_RIPEMD160);
				gen_certs &= ~CERT_RSA_RIPEMD160;
			}
		}

		if (EVP_MD_type(pkey) == EVP_PKEY_DSA) {
# if 0
			DSA_print_fp(stdout, pkey->pkey.dsa, 0);
# endif
			/*
			 * Generate the X509 certificate request. The digest
			 * algorithms that work with DSS (DSA) are DSS and
			 * DSS1.
			 */
			if (gen_certs & CERT_DSA_SHA) {
				x509("DSA_SHA", pkey, EVP_dss(),
				     cl_bit & CERT_DSA_SHA);
				gen_certs &= ~CERT_DSA_SHA;
			}
			if (gen_certs & CERT_DSA_SHA1) {
				x509("DSA_SHA1", pkey, EVP_dss1(),
				     cl_bit & CERT_DSA_SHA1);
				gen_certs &= ~CERT_DSA_SHA1;
			}
		}

		free(pkey);
	}

	if (gen_dh) {
		/*
		 * Generate Diffie-Hellman parameters.
		 */
		printf("Generating DH parameters (%d bits)...\n", MODULUSLEN);
		dh_params = DH_generate_parameters(PRIMELEN, 2, cb, "DH");
		printf("\n");
		if (dh_params == NULL) {
			printf("DH generate parameters fails\n%s\n",
			       ERR_error_string(ERR_get_error(), NULL));
			exit (-1);
		}
		DH_generate_key(dh_params);
		if (!DH_check(dh_params, &codes)) {
			printf("Invalid DH parameters\n");
			exit (-1);
		}
		pkey = EVP_PKEY_new();
		EVP_PKEY_assign_DH(pkey, dh_params);
		str = fheader("DHpar", keysdir, f1_signkey, ntptime, tmp_name);
		PEM_write_DHparams(str, dh_params);
		fclose(str);
		free(pkey);
	}
#endif /* OPENSSL */

	return (0);
}


#ifdef OPENSSL
/*
 * Generate X509 certificate request and X509 self-signed certificate.
 * Note: At present, this works only for RSA signatures; DSA signatures
 * come a segfault. Later.
 *
 * The certificate request and certificate are saved as files in the
 * format described in the main program. The request consists of the
 * ASN.1 encoding of the version number, subject name, public key and
 * signature. The version number is 1, although this may change in
 * future. The subject name is the host name running this program and
 * the public key is the RSA or DSA key of the subject as selected by
 * the caller.
 *
 * The certificate consists of the ASN.1 encoding of the version number,
 * subject name and public key of the request and in addition the serial
 * number, issuer name and validity interval. For a self-signed
 * certificate, the issuer name is the same as the subject name and
 * these items are signed using the subject private key. The validity
 * interval extends from the current time to the same time one year
 * hence. For NTP purposes, it is convenient to use the NTP seconds of
 * the current time as the serial number.
 */
int
x509	(
	u_char	*id,		/* host name */
	EVP_PKEY *pkey,		/* generic key algorithm */
	EVP_MD *md,		/* generic digest algorithm */
	int certlink		/* Is this cert a link target? */
	)
{
#ifdef GEN_REQ
	X509_REQ *req;		/* X509 certificate request */
#endif
	X509	*cert;		/* X509 certificate */
	X509_NAME *subj;	/* distinguished (common) name */
	FILE	*str;		/* file handle */
	ASN1_INTEGER *serial;	/* serial number */
	char	pathbuf[PATH_MAX];

#ifdef GEN_REQ
	/*
	 * Generate, sign and verify X509 certificate request.
	 */
	printf("%s certificate request for %s key type %d\n", id,
	    hostname, EVP_MD_type(pkey));
	req = X509_REQ_new();
	X509_REQ_set_version(req, 0L);
	subj = X509_REQ_get_subject_name(req);
	X509_NAME_add_entry_by_txt(subj, "commonName", MBSTRING_ASC,
	    hostname, strlen(hostname), -1, 0);
	X509_REQ_set_pubkey(req, pkey);
	X509_REQ_sign(req, pkey, md);
	if (!X509_REQ_verify(req, pkey)) {
		printf("Verify request fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		return (-1);
	}

	/*
	 * Write request for offline processing.
	 */
	sprintf(pathbuf, "%sreq", id);
	str = fheader(pathbuf, keysdir, f1_cert, ntptime, tmp_name);
	PEM_write_X509_REQ(str, req);
	fclose(str);
#endif

	/*
	 * Generate X509 self-signed certificate.
	 *
	 * Set the certificate serial to the NTP seconds for grins. Set
	 * the version to 3 if attributes are included and to 0
	 * otherwise. Set the subject name and issuer name to the
	 * subject name in the request. Set the initial validity to the
	 * current time and the final validity
	 * one year hence. 
	 */
	cert = X509_new();
	X509_set_version(cert, 0L);
	serial = ASN1_INTEGER_new();
	ASN1_INTEGER_set(serial, ntptime);
	X509_set_serialNumber(cert, serial);
	X509_gmtime_adj(X509_get_notBefore(cert), 0L);
	X509_gmtime_adj(X509_get_notAfter(cert), YEAR);
	subj = X509_get_subject_name(cert);
	X509_NAME_add_entry_by_txt(subj, "commonName", MBSTRING_ASC,
	    hostname, strlen(hostname), -1, 0);
	subj = X509_get_issuer_name(cert);
	X509_NAME_add_entry_by_txt(subj, "commonName", MBSTRING_ASC,
	    hostname, strlen(hostname), -1, 0);
	if (!X509_set_pubkey(cert, pkey)) {
		printf("Assign key fails\n%s\n",
		ERR_error_string(ERR_get_error(), NULL));
		return (-1);
	}
	X509_sign(cert, pkey, md);
	if (!X509_verify(cert, pkey)) {
		printf("Verify certificate fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		return (-1);
	}

	/*
	 * Write certificate for offline processing.
	 */
	sprintf(pathbuf, "%scert", id);
	str = fheader(pathbuf, keysdir, f1_cert, ntptime, (certlink)
		      ? f2_cert
		      : tmp_name);

	PEM_write_X509(str, cert);
/*
	X509_print_fp(stdout, cert);
*/
	/*
	 * Give back the goodies.
	 */
	ASN1_INTEGER_free(serial);
	X509_free(cert);
#ifdef GEN_REQ
	X509_REQ_free(req);
#endif
	fclose(str);
	return (0);
}


/*
 * asn2ntp - convert ASN1_TIME time structure to NTP time
 */
u_long
asn2ntp	(
	ASN1_TIME *asn1time	/* pointer to ASN1_TIME structure */
	)
{
	char	*v;		/* pointer to ASN1_TIME string */
	struct	tm tm;		/* time decode structure time */

	/*
	 * Extract time string YYMMDDHHMMSSZ from ASN1 time structure.
	 * Note that the YY, MM, DD fields start with one, the HH, MM,
	 * SS fiels start with zero and the Z character should be 'Z'
	 * for UTC. Also note that years less than 50 map to years
	 * greater than 100. Dontcha love ASN.1?
	 */
	if (asn1time->length > 13)
		return (-1);
	v = (char *)asn1time->data;
	tm.tm_year = (v[0] - '0') * 10 + v[1] - '0';
	if (tm.tm_year < 50)
		tm.tm_year += 100;
	tm.tm_mon = (v[2] - '0') * 10 + v[3] - '0' - 1;
	tm.tm_mday = (v[4] - '0') * 10 + v[5] - '0';
	tm.tm_hour = (v[6] - '0') * 10 + v[7] - '0';
	tm.tm_min = (v[8] - '0') * 10 + v[9] - '0';
	tm.tm_sec = (v[10] - '0') * 10 + v[11] - '0';
	tm.tm_wday = 0;
	tm.tm_yday = 0;
	tm.tm_isdst = 0;
	return (mktime(&tm) + JAN_1970);
}


/*
 * Callback routine
 */
void
cb	(
	int	n1,		/* arg 1 */
	int	n2,		/* arg 2 */
	void	*chr		/* arg 3 */
	)
{
	switch (n1) {
	case 0:
		d0++;
		printf("%s %d %d %lu\r", (char *)chr, n1, n2, d0);
		break;
	case 1:
		d1++;
		printf("%s\t\t%d %d %lu\r", (char *)chr, n1, n2, d1);
		break;
	case 2:
		d2++;
		printf("%s\t\t\t\t%d %d %lu\r", (char *)chr, n1, n2, d2);
		break;
	case 3:
		d3++;
		printf("%s\t\t\t\t\t\t%d %d %lu\r", (char *)chr, n1, n2,
		    d3);
		break;
	}
}
#endif /* OPENSSL */


/*
 * Generate file header.
 *
 * We need to write this to the correct place:
 * - If the old filename starts with a /, use it as-is.
 * - else prepend the keysdir/
 * save this - it's the location of the (sym)link "source".
 * save the name of the "target".
 * write to dirname(source)/target
 */
FILE *
fheader	(
	char	*id,		/* ident string */
	char	*kdir,		/* key directory */
	char	*fpath,		/* file path */
	u_long	tstamp,		/* timestamp */
	char	*tgt		/* target */
	)
{
	FILE	*str;		/* file handle */
	char	*filename;	/* file name */

	if (debug > 1)
		printf("fheader: id <%s> kdir <%s> fpath <%s> t %lu\n",
		       id, kdir, fpath, tstamp);

	sprintf(tgt, "ntpkey_%s_%s.%lu", id, hostname, tstamp);
	filename = getpath(kdir, fpath, tgt);
	if (!filename)
		return NULL;

	if (debug > 1)
		printf("fheader: tgt <%s> filename <%s>\n", tgt, filename);

	str = fopen(filename, "w");
	if (str)
		fprintf(str, "# %s\n# %s", tgt, ctime(&tv.tv_sec));
	else {
		fprintf(stderr, "%s: fheader: fopen(%s) failed: %s\n",
			progname, filename, strerror(errno));
		exit(1);
	}
	return(str);
}


char *
getpath (
	char *gp_dir,
	char *gp_path,
	char *gp_file
	)
{
	static char filename[PATH_MAX]; /* file name */
	char	*cp;

	if (debug > 1)
		printf("getpath: gp_dir <%s> gp_path <%s> gp_file <%s>\n",
		       gp_dir, gp_path, (gp_file) ? gp_file : "(null)");

	if (*gp_path == '/')
		strcpy(filename, gp_path);
	else
		snprintf(filename, sizeof filename, "%s/%s", gp_dir, gp_path);
	if (gp_file) {
		cp = strrchr(filename, '/');
		if (cp)
			++cp, *cp = '\0';
		else {
			fprintf(stderr, "%s: getpath: no / in <%s>!\n",
				progname, filename);
			return NULL;
		}
		strcat(cp, gp_file);
	}
	return filename;
}
