/*
 * Program to generate cryptographic keys for NTP clients and servers
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
#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/rand.h"
#include "openssl/pem.h"
#include "openssl/x509.h"
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
void	usage	 P((void));
void	getCmdOpts P((int, char**));
int	genkeys  P((void));
FILE	*fheader P((u_char *));	/* construct file header */
#ifdef OPENSSL
int	x509	 P((u_char *, EVP_PKEY *, EVP_MD *)); /* generate req/cert */
void	cb	 P((int, int, void *));	/* callback routine */
u_long	asn2ntp	 P((ASN1_TIME *));	/* ASN.1 time format to NTP seconds */
#endif /* OPENSSL */

/*
 * Program variables
 */
struct timeval tv;		/* initialization vector */
u_long	ntptime;		/* NTP epoch */
u_char	hostname[PATH_MAX];	/* host name */
#ifdef OPENSSL
long	d0, d1, d2, d3;		/* callback counters */
#endif /* OPENSSL */

char *f1_keysdir = NTP_KEYSDIR;

char *f1_keys;			/* Visible MD5 key file name */
char *f2_keys;			/* timestamped */
char *f3_keys;			/* previous filename */

char *f1_privatekey;
char *f2_privatekey;
char *f3_privatekey;

char *f1_signkey;
char *f2_signkey;
char *f3_signkey;

char *f1_cert;
char *f2_cert;
char *f3_cert;


/* Stubs and hacks so we can link with ntp_config.o */

struct interface *any_interface; /* default interface */
u_long  client_limit;
u_long  client_limit_period;
keyid_t ctl_auth_keyid;		/* keyid used to authenticate write requests */
u_long  current_time;		/* current time (s) */
volatile int debug = 1;		/* debugging flag */
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
	if (debug > 1) printf("peer_config...\n");
	return 0;
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


#ifdef PUBKEY
void
crypto_config(
	int item,		/* configuration item */
	char *cp		/* file name */
	)
{
	switch (item) {
	    case CRYPTO_CONF_PRIV:
		if (debug > 0) printf("crypto_config: PRIVATEKEY/<%d> <%s>\n", item, cp);
		f1_privatekey = strdup(cp);
		break;
	    case CRYPTO_CONF_SIGN:
		if (debug > 0) printf("crypto_config: SIGNKEY/<%d> <%s>\n", item, cp);
		f1_signkey = strdup(cp);
		break;
	    case CRYPTO_CONF_KEYS:
		if (debug > 0) printf("crypto_config: KEYSDIR/<%d> <%s>\n", item, cp);
		f1_keysdir = strdup(cp);
		break;
	    case CRYPTO_CONF_CERT:
		if (debug > 0) printf("crypto_config: CERT/<%d> <%s>\n", item, cp);
		f1_cert = strdup(cp);
		break;
	    default:
		if (debug > 1) printf("crypto_config: <%d> <%s>\n", item, cp);
		break;
	}
	return;
}
#endif


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
	if (debug > 0) printf("getauthkeys: got <%s>\n", keyfile);
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
 */

#define GEN_DSA		1
#define GEN_RSA		2
#define GEN_LINK	4
#define GEN_CERT	8
#define GEN_DSA_L	(GEN_DSA | GEN_LINK)
#define GEN_RSA_L	(GEN_RSA | GEN_LINK)

char *certname = "RSA_MD5";	/* What cert do we use for links? */
char *config_file = CONFIG_FILE; /* Default location for ntp.conf */
int gen_cert;			/* Generate a cert? */
int gen_dh;			/* Generate DH files? */
int gen_rsa;			/* Generate an RSA key? */
int gen_sign;			/* Generate a sign key? */
int gen_skf;			/* Generate symmetric key file? */
char *stampfile;		/* Stampfile name */
int verbose;			/* Be verbose? */

char *certlist[] = {
	"DSA_SHA1",
	"DSA_SHA",
#ifdef HAVE_EVP_MD2
	"RSA_MD2",
#endif
	"RSA_MD5",
#ifdef HAVE_EVP_MDC2
	"RSA_MDC2",
#endif
	"RSA_RIPEMD160",
	"RSA_SHA",
	"RSA_SHA1",
	0
};

int certlist_n = (sizeof certlist / sizeof *certlist) - 1;
char const *progname;


void
usage (
	void
	)
{
	int i;

	printf("Usage:%s [ -[cC] {", progname);
	for (i = 0; i < certlist_n; ++i)
		printf("%s%s", (i) ? "," : "", certlist[i]);
	printf("} ]\n");
	printf("\t[ -d ] [ -f config.file ] [ -m ] [ -[rR] RSA ] [ -[sS] {DSA,RSA} ]\n");
	printf("\t[ -t stampfile ] [ -v ] [ hostname ... ]\n");
	exit(1);
}


void
getCmdOpts (
	int argc,
	char *argv[]
	)
{
	int i, j, errflag = 0;

	progname = argv[0];

	certname = certlist[0];
	while ((i = ntp_getopt(argc, argv, "c:C:df:mr:R:s:S:t:v")) != EOF)
		switch (i) {

		    case 'C':	/* Generate a symlink and */
			gen_cert = GEN_LINK;
			/* FALL THROUGH */
		    case 'c':	/* Generate a cert */
			for (j = 0; j < certlist_n; ++j)
				if (!strcmp(certlist[j], ntp_optarg)) {
					certname = ntp_optarg;
					break;
				}
			if ( j == certlist_n)
				++errflag;
			gen_cert |= GEN_CERT;
			break;
		    case 'd':	/* Create the DH files */
			++gen_dh;
			break;
		    case 'f':	/* Where is the config file? */
			config_file = ntp_optarg;
			break;
		    case 'm':	/* Generate MD5 symmetric key file */
			++gen_skf;
			break;

		    case 'R':	/* Generate a symlink and */
			gen_rsa = GEN_LINK;
		    case 'r':	/* Generate an RSA key */
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
			/* DSA and RSA are the only two things allowed here */
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
	if (errflag)
		usage();
}

int
main(
	int argc,
	char *argv[]
	)
{
	int i;

	getconfig(argc, argv);	/* ntpd/ntp_config.c */

#ifdef OPENSSL
	if (SSLeay() != OPENSSL_VERSION_NUMBER) {
		printf("OpenSSL version mismatch. Built against %lx, you have %lx\n",
			OPENSSL_VERSION_NUMBER, SSLeay());
		exit(1);
	} else {
		printf("OpenSSL version %lx\n", SSLeay());
	}
#endif

	/*
	 * Initialize the timestamp.
	 */
	gettimeofday(&tv, 0);
	ntptime = tv.tv_sec + JAN_1970;

	i = 0;
	if (ntp_optind == argc) { /* No more args - generate key for us */
		gethostname(hostname, sizeof(hostname));

		i |= genkeys();
	} else while (ntp_optind < argc) {
		strncpy(hostname, argv[ntp_optind], sizeof hostname);

		i |= genkeys();

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
		fprintf(fp, "%ul\n", ntptime);
		fclose(fp);
	}
	return(0);
}


int
genkeys(
	void
	)
{
#ifdef OPENSSL
	EVP_PKEY *pkey;		/* public/private keys */
	RSA	*rsa;		/* RSA keys */
	DSA	*dsa_params;	/* DSA parameters */
	DH	*dh_params;	/* Diffie-Hellman parameters */
	u_char	seed[20];	/* seed for DSA parameters */
	int	codes;		/* DH check codes */
	char	pathbuf[PATH_MAX];
#endif /* OPENSSL */
	u_char	md5key[16];
	FILE	*str;
	u_int	temp;
	int	i, j;

	printf("Generating keys for %s...\n", hostname);

	if (gen_skf) {
		/*
		 * Generate semi-random MD5 keys.
		 */
		printf("Generating MD5 keys...\n");
		srandom((u_int)tv.tv_usec);
		str = fheader("MD5key");
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
	 * Generate random RSA keys.
	 */
	printf("Generating RSA keys (%d bits)...\n", MODULUSLEN);
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
	str = fheader("RSAkey");
	PEM_write_RSAPrivateKey(str, rsa, NULL, NULL, 0, NULL, NULL);
	fclose(str);
/*
	RSA_print_fp(stdout, pkey->pkey.rsa, 0);
*/
	/*
	 * Generate the X509 certificate request. The digest algorithms
	 * that work with RSA are MD2, MD5, SHA, SHA1, MDC2 and
	 * RIPEMD160.
	 */
#ifdef HAVE_EVP_MD2
	x509("RSA_MD2", pkey, EVP_md2());
#endif
	x509("RSA_MD5", pkey, EVP_md5());
	x509("RSA_SHA", pkey, EVP_sha());
	x509("RSA_SHA1", pkey, EVP_sha1());
#ifdef HAVE_EVP_MDC2
	x509("RSA_MDC2", pkey, EVP_mdc2());
#endif
	x509("RSA_RIPEMD160", pkey, EVP_ripemd160());
	free(pkey);

	/*
	 * Generate DSA parameters.
	 */
	printf("Generating DSA parameters (%d bits)...\n", MODULUSLEN);
	dsa_params = DSA_generate_parameters(MODULUSLEN, seed,
	    sizeof(seed), NULL, NULL, cb, "DSA_params");
	printf("\n");
	if (dsa_params == NULL) {
		printf("DSA generate parameters fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		 exit (-1);
	}
	str = fheader("DSApar");
	PEM_write_DSAparams(str, dsa_params);
	fclose(str);

	/*
	 * Generate DSA keys. Note, the digest algorithms that work with
	 * DSS (DSA) are DSS and DSS1.
	 */
	printf("Generating DSA keys (%d bits)...\n", MODULUSLEN);
	if (!DSA_generate_key(dsa_params)) {
		printf("DSA generate keys fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		exit (-1);
	}
	pkey = EVP_PKEY_new();
	EVP_PKEY_assign_DSA(pkey, dsa_params);
	str = fheader("DSAkey");
	PEM_write_DSAPrivateKey(str, dsa_params, NULL, NULL, 0, NULL,
	    NULL);
	fclose(str);
/*
	DSA_print_fp(stdout, pkey->pkey.dsa, 0);
*/
	/*
	 * Generate the X509 certificate request. The digest algorithms
	 * that work with DSS (DSA) are DSS and DSS1.
	 */
	x509("DSA_SHA", pkey, EVP_dss());
	x509("DSA_SHA1", pkey, EVP_dss1());
	free(pkey);

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
		str = fheader("DHpar");
		PEM_write_DHparams(str, dh_params);
		fclose(str);
		free(pkey);
	}
#endif /* OPENSSL */

	/*
	 * Make the links?
	 */

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
	EVP_MD *md		/* generic digest algorithm */
	)
{
	X509_REQ *req;		/* X509 certificate request */
	X509	*cert;		/* X509 certificate */
	X509_NAME *subj;	/* distinguished (common) name */
	FILE	*str;		/* file handle */
	ASN1_INTEGER *serial;	/* serial number */
	u_char	pathbuf[PATH_MAX];

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
	str = fheader(pathbuf);
	PEM_write_X509_REQ(str, req);
	fclose(str);

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
	str = fheader(pathbuf);
	PEM_write_X509(str, cert);
/*
	X509_print_fp(stdout, cert);
*/
	/*
	 * Give back the goodies.
	 */
	ASN1_INTEGER_free(serial);
	X509_free(cert);
	X509_REQ_free(req);
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
 * Generate file header
 */
FILE *
fheader	(
	u_char	*id		/* ident string */
	)
{
	FILE	*str;		/* file handle */
	char	filename[PATH_MAX]; /* file name */

	sprintf(filename, "ntpkey_%s_%s.%lu", id, hostname, ntptime);
	str = fopen(filename, "w");
	fprintf(str, "# %s\n# %s", filename, ctime(&tv.tv_sec));
	return(str);
}
