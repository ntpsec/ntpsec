/*
 * Program to generate cryptographic keys for NTP clients and servers
 *
 * This program generates files "ntpkey_<type>_<hostname>.<filestamp>",
 * where <type> is the file type, <hostname> is the generating host and
 * <filestamp> is the NTP seconds in decimal format. The NTP programs
 * expect generic names such as "ntpkey_<type>_whimsy.udel.edu" with the
 * connection being maintained by soft links.
 *
 * Files are prefixed with a header giving the name and date of creation
 * followed by a type-specific descriptive label and PEM-encoded data
 * string compatible with programs of the OpenSSL library.
 *
 * Note that private keys can be password encrypted as per OpenSSL
 * conventions.
 *
 * The file types include
 *
 * ntpkey_MD5key_<hostname>.<filestamp>
 * 	MD5 (128-bit) keys used to compute message digests in symmetric
 *	key cryptography
 *
 * ntpkey_RSAkey_<hostname>.<filestamp>
 *	RSA private/public host key pair used for public key signatures
 *	and data encryption
 *
 * ntpkey_DSAkey_<hostname>.<filestamp>
 *	DSA private/public sign key pair used for public key signatures,
 *	but not data encryption
 *
 * ntpkey_IFFpar_<hostname>.<filestamp>
 *	Schnorr (IFF) parameters used to verify trusted group membership
 *
 * ntpkey_GQpar_<hostname>.<filestamp>,
 * ntpkey_GQkey_<hostname>.<filestamp>
 *	Guillou-Quisquater (GQ) parameters and keys used to verify
 *	trusted group membership
 *
 * ntpkey_MVkey_<hostname>.<filestamp>,
 * ntpkey_MPkey<n>_<hostname>.<filestamp>
 *	Mu-Varadharajan (MV) parameters and keys used to verify	trusted
 *	 group membership
 *
 * ntpkey_XXXcert_<hostname>.<filestamp>
 *	X509v3 certificate using RSA or DSA public keys and signatures.
 *	XXX is a code identifying the message digest and signature
 *	encryption algorithm
 *
 * Available digest/signature schemes
 *
 * RSA:	RSA-MD2, RSA-MD5, RSA-SHA, RSA-SHA1, RSA-MDC2, EVP-RIPEMD160
 * DSA:	DSA-SHA, DSA-SHA1
 *
 * The links produced include
 *
 * ntpkey_key_<hostname> (RSA)
 *	Public host key used for cookie encryption and digital
 *	signatures if a sign key is not present. The public key value of
 *	the key is disclosed in certificates only if signed by this key.
 *
 * ntpkey_sign_<hostname> (RSA or DSA)
 *	Public sign key used for digital signatures. The public key
 *	value of the key is disclosed in certificates signed by this
 *	key unless declared otherwise.
 *
 * ntpkey_iff_<hostname> (IFF)
 *	Private IFF key used to securely confirm identity to other
 *	members of the group. No value of this key is disclosed except
 *	to other members of the same group.
 *
 * ntpkey_gq_<hostname> (GQ)
 *	Public GQ key used to securely confirm identity. The public key
 *	value of the key is disclosed in certificates signed by this
 *	key unless declared otherwise.
 *
 * Note: Once in a while because of some statistical fluke this program
 * fails to generate and verify some cryptographic data, as indicated by
 * exit status -1. In this case simply run the program again. If the
 * program does complete with return code 0, the data are correct as
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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#include "ntp_types.h"
#include "l_stdlib.h"

#ifdef OPENSSL
#include "openssl/bn.h"
#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/rand.h"
#include "openssl/pem.h"
#include "openssl/x509v3.h"
#include <openssl/objects.h>
#endif /* OPENSSL */

/*
 * Cryptodefines
 */
#define	MD5KEYS		16	/* number of MD5 keys generated */
#define	JAN_1970	ULONG_CONST(2208988800) /* NTP seconds */
#define YEAR		((long)60*60*24*365) /* one year in seconds */
#define MAXFILENAME	256	/* max file name length */
#define MAXHOSTNAME	256	/* max host name length */
#ifdef OPENSSL
#define	PLEN		512	/* default prime modulus size (bits) */

/*
 * Strings used in X509v3 extension fields
 */
#define KEY_USAGE		"digitalSignature,keyCertSign"
#define BASIC_CONSTRAINTS	"critical,CA:TRUE"
#define EXT_KEY_PRIVATE		"private"
#define EXT_KEY_TRUST		"trustRoot"
#endif /* OPENSSL */

/*
 * Prototypes
 */
FILE	*fheader	P((const char *, const char *));
void	fslink		P((const char *, const char *));
int	gen_md5		P((char *));
#ifdef OPENSSL
EVP_PKEY *gen_rsa	P((char *));
EVP_PKEY *gen_dsa	P((char *));
EVP_PKEY *gen_iff	P((char *));
RSA	*gen_gqpar	P((char *));
RSA	*gen_gqkey	P((char *, RSA *));
void	gen_mv		P((char *));
int	x509		P((EVP_PKEY *, const EVP_MD *, char *, char *));
void	cb		P((int, int, void *));
EVP_PKEY *genkey	P((char *, char *));
u_long	asn2ntp		P((ASN1_TIME *));
#endif /* OPENSSL */

/*
 * Program variables
 */
extern char *optarg;		/* command line argument */
int	debug = 0;		/* debug, not de bug */
int	rval;			/* return status */
u_int	modulus = PLEN;		/* prime modulus size (bits) */
int	nkeys = 0;		/* MV keys */
time_t	epoch;			/* Unix epoch (seconds) since 1970 */
char	*hostname;		/* host name (subject name) */
char	*trustname;		/* trusted host name (issuer name) */
char	filename[MAXFILENAME + 1]; /* file name */
char	*passwd = NULL;		/* private key password */
#ifdef OPENSSL
long	d0, d1, d2, d3;		/* callback counters */
#endif /* OPENSSL */

/*
 * Main program
 */
int
main(
	int	argc,		/* command line options */
	char	**argv
	)
{
	struct timeval tv;	/* initialization vector */
#ifdef OPENSSL
	X509	*cert = NULL;	/* X509 certificate */
	EVP_PKEY *pkey_host = NULL; /* host key */
	EVP_PKEY *pkey_sign = NULL; /* sign key */
	EVP_PKEY *pkey_iff = NULL; /* iff group key */
	EVP_PKEY *pkey = NULL;	/* temp sign key */
	RSA	*rsa_gqpar = NULL; /* GQ parameters */
	RSA	*rsa_gqkey = NULL; /* GQ key */
	const EVP_MD *ectx;	/* EVP digest */
	char	hostbuf[MAXHOSTNAME + 1];
	char	pathbuf[MAXFILENAME + 1];
	FILE	*str;		/* file handle */
	int	md5key = 0;	/* MD5 keys */
	int	hostkey = 0;	/* RSA keys */
	int	iffkey = 0;	/* IFF keys */
	int	gqpar = 0;	/* GQ parameters */
	int	gqkey = 0;	/* GQ keys */
	char	*sign = NULL;	/* sign key */
	const char *scheme = NULL; /* digest/signature scheme */
	char	*exten = NULL;	/* private extension */
	char	*grpkey = NULL;	/* identity extension */
	int	nid;		/* X509 digest/signature scheme */
#endif /* OPENSSL */
	u_int	temp;

#ifdef OPENSSL
	if (SSLeay() != OPENSSL_VERSION_NUMBER) {
		printf("OpenSSL version mismatch. Built against %lx, you have %lx\n",
			OPENSSL_VERSION_NUMBER, SSLeay());
		return (-1);
	} else {
		printf("OpenSSL version %lx\n", SSLeay());
	}
#endif /* OPENSSL */


	/*
	 * Process options, initialize host name and timestamp.
	 */
	gethostname(hostbuf, MAXHOSTNAME);
	hostname = hostbuf;
	trustname = hostbuf;
	gettimeofday(&tv, 0);
	epoch = tv.tv_sec;
	rval = 0;
	while ((temp = getopt(argc, argv,
	    "c:de:GgHIi:Mm:Pp:S:s:TV:")) != -1) {
		switch(temp) {

		/*
		 * -c select public certificate type
		 */
		case 'c':
			scheme = optarg;
			continue;

		/*
		 * -d debug
		 */
		case 'd':
			debug++;
			continue;

		/*
		 * -e X509v3 key usage extension (!danger!)
		 */
		case 'e':
			exten = optarg;
			continue;

		/*
		 * -G generate GQ parameters (GQ scheme)
		 */
		case 'G':
			gqpar++;
			continue;

		/*
		 * -g generate GQ keys (GQ scheme)
		 */
		case 'g':
			gqkey++;
			continue;

		/*
		 * -H generate host key (RSA)
		 */
		case 'H':
			hostkey++;
			continue;

		/*
		 * -I generate IFF parameters (IFF scheme)
		 */
		case 'I':
			iffkey++;
			continue;

		/*
		 * -i set issuer name
		 */
		case 'i':
			trustname = optarg;

		/*
		 * -M generate MD5 keys
		 */
		case 'M':
			md5key++;
			continue;


		/*
		 * -m select modulus (256-2048)
		 */
		case 'm':
			if (sscanf(optarg, "%d", &modulus) != 1)
				printf("invalid option -m %s\n",
				    optarg);	
			continue;
		
		/*
		 * -P generate private certificate (PC scheme)
		 */
		case 'P':
			exten = EXT_KEY_PRIVATE;
			continue;

		/*
		 * -p private key password
		 */
		case 'p':
			passwd = optarg;
			continue;

		/*
		 * -S generate sign key (RSA or DSA)
		 */
		case 'S':
			sign = optarg;
			continue;

		/*
		 * -s set subject name
		 */
		case 's':
			hostname = optarg;
		
		/*
		 * -T trusted certificate (TC scheme)
		 */
		case 'T':
			exten = EXT_KEY_TRUST;
			continue;

		/*
		 * -V <keys> Mu-Varadharajan (MV scheme)
		 */
		case 'V':
			if (sscanf(optarg, "%d", &nkeys) != 1)
				printf("invalid option -V %s\n",
				    optarg);
			continue;

		/*
		 * None of the above.
		 */
		default:
			printf("unknown option %c\n", temp);
			continue;
		}
	}

#ifdef OPENSSL
	/*
	 * Seed random number generator and grow weeds.
	 */
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	if (RAND_file_name(pathbuf, MAXFILENAME) == NULL) {
		printf("RAND_file_name %s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		return (-1);
	}
	temp = RAND_load_file(pathbuf, -1);
	if (temp == 0) {
		printf("RAND_load_file %s not found or empty\n",
		    pathbuf);
		return (-1);
	}
	printf("Random seed file %s %u bytes\n", pathbuf, temp);
	RAND_add(&epoch, sizeof(epoch), 4.0);

	/*
	 * Generate new parameters and keys as requested. These replace
	 * any values already generated.
	 */
	if (md5key)
		gen_md5("MD5");
	if (hostkey)
		pkey_host = genkey("RSA", "host");
	if (sign != NULL)
		pkey_sign = genkey(sign, "sign");
	if (iffkey)
		pkey_iff = gen_iff("iff");
	if (gqpar)
		rsa_gqpar = gen_gqpar("gqpar");
	if (nkeys > 0)
		gen_mv("mvkey");

	/*
	 * If there is no new host key, look for an existing one. If not
	 ( found, create it.
	 */
	while (pkey_host == NULL) {
		sprintf(filename, "ntpkey_host_%s", hostname);
		if ((str = fopen(filename, "r")) != NULL) {
			pkey_host = PEM_read_PrivateKey(str, NULL, NULL,
			    passwd);

			fclose(str);
			readlink(filename, filename, sizeof(filename));
			if (pkey_host == NULL) {
				printf("Host key\n%s\n",
				    ERR_error_string(ERR_get_error(),
				    NULL));
			} else {
				printf("Using host key %s\n",
				    filename);
				break;
			}
		}
		if ((pkey_host = genkey("RSA", "host")) == NULL)
			break;
	}

	/*
	 * If there is no new sign key, look for an existing one. If not
	 * found, use the host key instead.
	 */
	pkey = pkey_sign;
	while (pkey_sign == NULL) {
		sprintf(filename, "ntpkey_sign_%s", hostname);
		if ((str = fopen(filename, "r")) != NULL) {
			pkey_sign = PEM_read_PrivateKey(str, NULL, NULL,
			    passwd);
			fclose(str);
			readlink(filename, filename, sizeof(filename));
			if (pkey_sign == NULL) {
				printf("Sign key\n%s\n",
				    ERR_error_string(ERR_get_error(),
				    NULL));
			} else {
				pkey = pkey_sign;
				printf("Using sign key %s\n",
				    filename);
				break;
			}
		}
		pkey = pkey_host;
		printf("Using host key as sign key\n");
		break;
	}

	/*
	 * If there is no IFF group key, look for an existing one.
	 */
	if (pkey_iff == NULL) {
		sprintf(filename, "ntpkey_iffpar_%s", hostname);
		if ((str = fopen(filename, "r")) != NULL) {
			pkey_iff = PEM_read_PrivateKey(str, NULL, NULL,
			    passwd);
			fclose(str);
			readlink(filename, filename, sizeof(filename));
			if (pkey_iff == NULL) {
				printf("IFFpar\n%s\n",
				    ERR_error_string(ERR_get_error(),
				    NULL));
			} else {
				printf("Using IFF group key %s\n",
				    filename);
			}
		}
	}

	/*
	 * If there is no GQ group key, look for an existing one.
	 */
	if (rsa_gqpar == NULL) {
		sprintf(filename, "ntpkey_gqpar_%s", hostname);
		if ((str = fopen(filename, "r")) != NULL) {
			rsa_gqpar = PEM_read_RSAPrivateKey(str, NULL,
			    NULL, passwd);
			fclose(str);
			readlink(filename, filename, sizeof(filename));
			if (rsa_gqpar == NULL) {
				printf("GQpar\n%s\n",
				    ERR_error_string(ERR_get_error(),
				    NULL));
			} else {
				printf("Using GQ group key %s\n",
				    filename);
			}
		}
	}

	/*
	 * If there is a GQ group key, create GQ private/public keys.
	 */
	if (rsa_gqpar != NULL) {
		rsa_gqkey = gen_gqkey("gq", rsa_gqpar);
		grpkey = BN_bn2hex(rsa_gqkey->e);
		}

	/*
	 * Generate a X509v3 certificate.
	 */
	while (scheme == NULL) {
		sprintf(filename, "ntpkey_cert_%s", hostname);
		if ((str = fopen(filename, "r")) != NULL) {
			cert = PEM_read_X509(str, NULL, NULL, NULL);
			fclose(str);
			readlink(filename, filename, sizeof(filename));
			if (cert == NULL) {
				printf("Cert \n%s\n",
				    ERR_error_string(ERR_get_error(),
				    NULL));
			} else {
				nid = OBJ_obj2nid(
				 cert->cert_info->signature->algorithm);
				scheme = OBJ_nid2sn(nid);
				printf("Using scheme %s from %s\n",
				    scheme, filename);
				break;
			}
		}
		scheme = "RSA-MD5";
	}
	if (pkey != NULL) {
		ectx = EVP_get_digestbyname(scheme);
		if (ectx == NULL) {
			printf(
			    "Invalid digest/signature combination %s\n",
			    scheme);
			rval = -1;
		} else {
			x509(pkey, ectx, grpkey, exten);
		}
	}

	/*
	 * Return the marbles.
	 */
	if (grpkey != NULL)
		OPENSSL_free(grpkey);
	if (pkey_host != NULL)
		EVP_PKEY_free(pkey_host);
	if (pkey_sign != NULL)
		EVP_PKEY_free(pkey_sign);
	if (pkey_iff != NULL)
		EVP_PKEY_free(pkey_iff);
	if (rsa_gqpar != NULL)
		RSA_free(rsa_gqpar);
	if (rsa_gqkey != NULL)
		RSA_free(rsa_gqkey);

#endif /* OPENSSL */
	return (rval);
}


#if 0
/*
 * Generate random MD5 key with password.
 */
int
gen_md5(
	char	*id		/* file name id */
	)
{
	BIGNUM	*key;
	BIGNUM	*keyid;
	FILE	*str;
	u_char	bin[16];

	printf("Generating MD5 keys...\n");
	str = fheader("MD5key", hostname);
	keyid = BN_new(); key = BN_new();
	BN_rand(keyid, 16, -1, 0);
	BN_rand(key, 128, -1, 0);
	BN_bn2bin(key, bin);
	PEM_write_fp(str, MD5, NULL, bin);
	fclose(str);
	fslink(id, hostname);
	return (1);
}


#else
/*
 * Generate semi-random MD5 keys compatible with NTPv3 and NTPv4
 */
int
gen_md5(
	char	*id		/* file name id */
	)
{
	u_char	md5key[16];	/* MD5 key */
	FILE	*str;
	u_int	temp;
	int	i, j;

	printf("Generating MD5 keys...\n");
	str = fheader("MD5key", hostname);
	srandom(epoch);
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
	fslink(id, hostname);
	return (1);
}
#endif /* OPENSSL */


#ifdef OPENSSL
/*
 * Generate RSA public/private keys
 */
EVP_PKEY *			/* public/private key pair */
gen_rsa(
	char	*id		/* file name id */
	)
{
	EVP_PKEY *pkey;		/* public/private key pair */
	RSA	*rsa;		/* RSA parameters and key pair */
	FILE	*str;

	printf("Generating RSA keys (%d bits)...\n", modulus);
	rsa = RSA_generate_key(modulus, 3, cb, "RSA");
	printf("\n");
	if (rsa == NULL) {
		printf("RSA generate keys fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		return (NULL);
	}

	/*
	 * For signature encryption it is not necessary that the RSA
	 * parameters be strictly groomed and once in a while the
	 * modulus turns out to be non-prime. Just for grins, we check
	 * the primality.
	 */
	if (!RSA_check_key(rsa)) {
		printf("Invalid RSA key\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		RSA_free(rsa);
		rval = -1;
		return (NULL);
	}

	/*
	 * Write the RSA parameters and keys as a RSA private key
	 * encoded in PEM.
	 */
	str = fheader("RSAkey", hostname);
	PEM_write_RSAPrivateKey(str, rsa, passwd ? EVP_des_cbc() : NULL,
	    NULL, 0, NULL, passwd);
	fclose(str);
	if (debug)
		RSA_print_fp(stdout, rsa, 0);
	fslink(id, hostname);
	pkey = EVP_PKEY_new();
	EVP_PKEY_assign_RSA(pkey, rsa);
	return (pkey);
}

 
/*
 * Generate DSA public/private keys
 */
EVP_PKEY *			/* public/private key pair */
gen_dsa(
	char	*id		/* file name id */
	)
{
	EVP_PKEY *pkey;		/* public/private key pair */
	DSA	*dsa;		/* DSA parameters */
	u_char	seed[20];	/* seed for parameters */
	FILE	*str;

	/*
	 * Generate DSA parameters.
	 */
	printf("Generating DSA parameters (%d bits)...\n", modulus);
	RAND_bytes(seed, sizeof(seed));
	dsa = DSA_generate_parameters(modulus, seed, sizeof(seed), NULL,
	    NULL, cb, "DSA");
	printf("\n");
	if (dsa == NULL) {
		printf("DSA generate parameters fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		rval = -1;
		return (NULL);
	}

	/*
	 * Generate DSA keys.
	 */
	printf("Generating DSA keys (%d bits)...\n", modulus);
	if (!DSA_generate_key(dsa)) {
		printf("DSA generate keys fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		DSA_free(dsa);
		rval = -1;
		return (NULL);
	}

	/*
	 * Write the DSA parameters and keys as a DSA private key
	 * encoded in PEM.
	 */
	str = fheader("DSAkey", hostname);
	PEM_write_DSAPrivateKey(str, dsa, passwd ? EVP_des_cbc() : NULL,
	    NULL, 0, NULL, passwd);
	fclose(str);
	if (debug)
		DSA_print_fp(stdout, dsa, 0);
	fslink(id, hostname);
	pkey = EVP_PKEY_new();
	EVP_PKEY_assign_DSA(pkey, dsa);
	return (pkey);
}


/*
 * Generate Schnorr (IFF) parameters and public/private keys
 *
 * The Schnorr (IFF)identity scheme is intended for use when
 * certificates are generated by some other trusted certificate
 * authority and the group key cannot be conveyed in the certificate
 * itself. For this purpose, new generations of IFF values must be
 * securely transmitted to all members of the group before use.
 *
 * The IFF values hide in a DSA cuckoo structure which uses the same
 * parameters. The values are used by an identity scheme based on DSA
 * cryptography and described in Stimson p. 285. The p is a 512-bit
 * prime, g a generator of Zp and q a 160-bit prime that divides p - 1
 * and is a qth root of 1 mod p; that is, g^q = 1 mod p. The TA rolls a
 * private random a less than q, then computes public g^(q - a). These
 * values are shared among all group members but not revealed in
 * certificate or message data. Alice challenges Bob to confirm identity
 * using the protocol described below.
 */
EVP_PKEY *			/* IFF parameters and keys */
gen_iff(
	char	*id		/* file name id */
	)
{
	EVP_PKEY *pkey;		/* for PEM structure */
	DSA	*dsa;		/* DSA parameters */
	u_char	seed[20];	/* seed for parameters */
	BN_CTX	*ctx;		/* BN working space */
	BIGNUM	*a, *k, *r, *bn, *bk; /* BN temp */
	FILE	*str;
	u_int	temp;

	/*
	 * Generate DSA parameters for use as IFF parameters.
	 */
	printf("Generating IFF parameters (%d bits)...\n",
	    modulus);
	RAND_bytes(seed, sizeof(seed));
	dsa = DSA_generate_parameters(modulus, seed, sizeof(seed), NULL,
	    NULL, cb, "IFF");
	printf("\n");
	if (dsa == NULL) {
		printf("DSA generate parameters fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		rval = -1;
		return (NULL);;
	}

	/*
	 * Generate the private and public keys. The DSA parameters and
	 * these keys are distributed to all members of the group.
	 */
	printf("Generating IFF keys (%d bits)...\n", modulus);
	a = BN_new(); k = BN_new(); r = BN_new();
	bn = BN_new(); bk = BN_new(); ctx = BN_CTX_new();
	BN_rand(a, BN_num_bits(dsa->q), -1, 0);	/* a */
	BN_mod(a, a, dsa->q, ctx);
	BN_sub(bn, dsa->q, a);
	BN_mod_exp(bn, dsa->g, bn, dsa->p, ctx); /* g^(q - a) mod p */
	BN_mod_exp(bk, dsa->g, a, dsa->p, ctx);	/* g^a mod p */
	BN_mod_mul(bk, bk, bn, dsa->p, ctx);
	temp = BN_is_one(bk);

	printf("Confirm g^(q - a) g^a = 1 mod p: %s\n", temp == 1 ?
	    "yes" : "no");
	if (!temp) {
		BN_free(a); BN_free(k); BN_free(r);
		BN_free(bn); BN_free(bk); BN_CTX_free(ctx);
		rval = -1;
		return (NULL);
	}
	dsa->priv_key = BN_dup(a);
	dsa->pub_key = BN_dup(bn);

	/*
	 * Here is a trial round of the protocol. First, Alice rolls
	 * random r ( 0 < r < q) and sends it to Bob.
	 */
	BN_rand(r, BN_num_bits(dsa->q), -1, 0);	/* r, 0 < r < q */
	BN_mod(r, r, dsa->q, ctx);

	/*
	 * Bob rolls random k (0 < k < q), computes k + a r mod q and
	 * g^k, then sends (k, g) to Alice.
	 */
	BN_rand(k, BN_num_bits(dsa->q), -1, 0);	/* k, 0 < k < q  */
	BN_mod(k, k, dsa->q, ctx);
	BN_mod_mul(bn, a, r, dsa->q, ctx);	/* a r mod q */
	BN_add(bn, bn, k);
	BN_mod(bn, bn, dsa->q, ctx);		/* k + a r mod q */

	/*
	 * Alice computes g^(k + a r) g^(q - a) r and verifies the
	 * result is equal to g.
	 */
	BN_mod_exp(bn, dsa->g, bn, dsa->p, ctx); /* g^(k + a r) mod p */
	BN_mod_exp(bk, dsa->pub_key, r, dsa->p, ctx); /* g^(q - a) r */
	BN_mod_mul(bn, bk, bn, dsa->p, ctx);	/* product mod p */
	BN_mod_exp(bk, dsa->g, k, dsa->p, ctx);	/* g^k mod p */
	temp = BN_cmp(bk, bn);
	printf("Confirm g^k = g^(k + a r) g^(q - a) r: %s\n", temp ==
	    0 ? "yes" : "no");
	BN_free(a); BN_free(k);	BN_free(r);
	BN_free(bn); BN_free(bk); BN_CTX_free(ctx);
	if (temp != 0) {
		DSA_free(dsa);
		rval = -1;
		return (NULL);
	}

	/*
	 * Write the IFF parameters as a DSA private key encoded in PEM.
	 * Note, this is not encrypted.
	 */
	pkey = EVP_PKEY_new();
	EVP_PKEY_assign_DSA(pkey, dsa);
	str = fheader("IFFpar", trustname);
	PEM_write_DSAPrivateKey(str, dsa, passwd ? EVP_des_cbc() : NULL,
	    NULL, 0, NULL, passwd);
	fclose(str);
	if (debug)
		DSA_print_fp(stdout, dsa, 0);
	fslink(id, trustname);
	return (pkey);
}


/*
 * Generate Guillou-Quisquater (GQ) parameters and keys
 *
 * The Guillou-Quisquater (GQ) identity scheme is intended for use when
 * the parameters, keys and certificates are generated by this program.
 * The scheme uses a certificate extension field do convey the public
 * key of a particular group identified by a group key known only to
 * members of the group. The scheme is self contained and independent of
 * new generations of host keys, sign keys and certificates.
 *
 * The GQ parameters hide in a RSA cuckoo structure which uses the same
 * parameters. The values are used by an identity scheme based on RSA
 * cryptography and described in Stimson p. 300 (with errors). The 512-
 * bit public modulus is n = p q, where p and q are secret large primes.
 * The TA rolls random group key b as security parameter and RSA
 * exponent. These values are shared among all group members but not
 * revealed in certificate or message data.
 *
 * When rolling new certificates, a member recomputes the private and
 * public keys. The private key u is a random roll, while the public key
 * is the inverse obscured by the group key v = (u^-1)^b. These values
 * replace the private and public keys normally generated by the RSA
 * scheme. Alice challenges Bob to confirm identity using the protocol
 * described below.
 */
/*
 * Generate Guillou-Quisquater (GQ) parameters
 */
RSA *				/* RSA cuckoo nest */
gen_gqpar(
	char	*id		/* file name id */
	)
{
	RSA	*rsapar;	/* GQ parameters */
	BN_CTX	*ctx;		/* BN working space */
	FILE	*str;

	/*
	 * Generate RSA parameters for use as GQ parameters.
	 */
	printf("Generating GQ parameters (%d bits)...\n", modulus);
	rsapar = RSA_generate_key(modulus, 3, cb, "GQ");
	printf("\n");
	if (rsapar == NULL) {
		printf("RSA generate keys fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		rval = -1;
		return (NULL);
	}

	/*
	 * Generate the group key b, which is saved in the e member of
	 * the RSA structure. These values are distributed to all
	 * members of the group, but shielded from all other groups.
	 */
	ctx = BN_CTX_new();
	BN_rand(rsapar->e, BN_num_bits(rsapar->n), -1, 0); /* b */
	BN_mod(rsapar->e, rsapar->e, rsapar->n, ctx);

	/*
	 * Write the GQ parameters and group key as a RSA private key
	 * encoded in PEM.
	 */
	str = fheader("GQpar", trustname);
	PEM_write_RSAPrivateKey(str, rsapar, passwd ? EVP_des_cbc() :
	    NULL, NULL, 0, NULL, passwd);
	fclose(str);
	if (debug)
		RSA_print_fp(stdout, rsapar, 0);
	fslink(id, trustname);
	return (rsapar);
}


/*
 * Generate Guillou-Quisquater (GQ) public/private keys
 */
RSA *				/* GQ public/private key pair */
gen_gqkey(
	char	*id,		/* file name id */
	RSA	*rsapar		/* GQ parameters */
	)
{
	RSA	*rsa;		/* GQ public/private key pair */
	BN_CTX	*ctx;		/* BN working space */
	BIGNUM	*u, *v, *g, *k, *r, *y; /* BN temps */
	FILE	*str;
	u_int	temp;

	/*
	 * Generate GQ key. Note that the group key b is the e member of
	 * the GQ parameters.
	 */
	printf("Generating GQ keys (%d bits)...\n", modulus);
	ctx = BN_CTX_new(); u = BN_new(); v = BN_new();
	g = BN_new(); k = BN_new(); r = BN_new(); y = BN_new();
	rsa = RSA_new();

	/*
	 * When generating his certificate, Bob rolls random private key
	 * u and inverse u^-1.
	 */
	BN_rand(u, BN_num_bits(rsapar->n), -1, 0); /* u */
	BN_mod(u, u, rsapar->n, ctx);
	BN_mod_inverse(v, u, rsapar->n, ctx);	/* u^-1 mod n */
	BN_mod_mul(k, v, u, rsapar->n, ctx);

	/*
	 * Bob computes the public key v = (u^-1)^b, which is saved in
	 * an extension field on his certificate. We check that u^b v =
	 * 1 mod n.
	 */
	BN_mod_exp(v, v, rsapar->e, rsapar->n, ctx);
	BN_mod_exp(g, u, rsapar->e, rsapar->n, ctx); /* u^b */
	BN_mod_mul(g, g, v, rsapar->n, ctx); /* u^b (u^-1)^b */
	temp = BN_is_one(g);
	printf("Confirm u^b (u^-1)^b = 1 mod n: %s\n", temp ? "yes" :
	    "no");
	if (!temp) {
		BN_free(u); BN_free(v);
		BN_free(g); BN_free(k); BN_free(r); BN_free(y);
		BN_CTX_free(ctx);
		RSA_free(rsa);
		rval = -1;
		return (NULL);
	}
	rsa->n = BN_dup(u);			/* private key */
	rsa->e = BN_dup(v);			/* public key */

	/*
	 * Here is a trial run of the protocol. First, Alice fetches
	 * Bob's certificate, then rolls random r (0 < r < n) and sends
	 * it to him.
	 */
	BN_rand(r, BN_num_bits(rsapar->n), -1, 0);	/* r */
	BN_mod(r, r, rsapar->n, ctx);

	/*
	 * Bob rolls random k (0 < k < n), computes y = k u^r mod n and
	 * g = k^b mod n, then sends (y, g) to Alice. 
	 */
	BN_rand(k, BN_num_bits(rsapar->n), -1, 0);	/* k */
	BN_mod(k, k, rsapar->n, ctx);
	BN_mod_exp(y, u, r, rsapar->n, ctx);	/* u^r mod n */
	BN_mod_mul(y, k, y, rsapar->n, ctx);	/* y = k u^r mod n */
	BN_mod_exp(g, k, rsapar->e, rsapar->n, ctx); /* g = k^b mod n */

	/*
	 * Alice computes v^r y^b mod n and verifies the result is equal
	 * to g.
	 */
	BN_mod_exp(v, v, r, rsapar->n, ctx);	/* v^r mod n */
	BN_mod_exp(y, y, rsapar->e, rsapar->n, ctx); /* y^b mod n */
	BN_mod_mul(y, v, y, rsapar->n, ctx);	/* v^r y^b mod n */
	temp = BN_cmp(y, g);
	printf("Confirm g^k = v^r y^b mod n: %s\n", temp == 0 ?
	    "yes" : "no");
	BN_CTX_free(ctx); BN_free(u); BN_free(v);
	BN_free(g); BN_free(k); BN_free(r); BN_free(y);
	if (temp != 0) {
		RSA_free(rsa);
		rval = -1;
		return (NULL);
	}

	/*
	 * Write the GQ public/private keys as a RSA public key encoded
	 * in PEM.
	 */
	str = fheader("GQkey", trustname);
	PEM_write_RSAPublicKey(str, rsa);
	fclose(str);
	if (debug)
		RSA_print_fp(stdout, rsa, 0);
	fslink(id, trustname);
	return (rsa);
}


/*
 * Generate Mu-Varadharajan (MV) parameters and keys
 *
 * The Generate Mu-Varadharajan (MV) cryptosystem is intended for use
 * where there is one encryption key for the server and a separate
 * decryption key for each client. It operates something like a
 * pay-per-view satellite broadcasting system where the view session key
 * is encrypted by the broadcaster and the decryption keys are held in a
 * tamperproof set-top box.
 *
 * The MV parameters and private encryption key hide in a DSA cuckoo
 * structure which uses the same parameters. The values are used in an
 * encryption scheme based on DSA cryptography and a polynomial formed
 * from the expansion of product terms (x - x[j]), as described in: Mu,
 * Y., and V. Varadharajan: Robust and Secure Broadcasting, Proc.
 * Indocrypt 2001, 223-231.
 *
 * The p is a 512-bit prime, g a generator of Zp and q a 160-bit prime
 * that divides p - 1 and is a qth root of 1 mod p; that is, g^q = 1 mod
 * p. A set of 160-bit values x[j], j = 1...n, are generated as the
 * zeros of a polynomial of order n. The product terms (x - x[j]) are
 * expanded to form coefficients in powers of x[i] mod q, i = 0...n.
 * These are used as exponents of the generator g mod p to generate the
 * private encryption key A. The pair (gbar, ghat) of public values and
 * the pairs (xbar[j], xhat[j]) of private values are used to construct
 * the decryption keys. The devil is in the details.
 *
 * This routine generates an encryption file including the prime modulus
 * p, encryption key A and public key (gbar, ghat). It then generates 
 * decryption files including the prime modulus, public key and private
 * key (xbar[j], xhat[j]) for each client. The server encrypts a block
 * y = A^x; the jth client decrypts x = (gbar^xhat[j] ghat^xbar[j])^y.
 */
void
gen_mv(
	char	*id		/* file name id */
	)
{
	DSA	*dsa;		/* DSA parameters */
	BN_CTX	*ctx;		/* BN working space */
	BIGNUM	**x;		/* private key vector */
	BIGNUM	**a;		/* coefficient vector */
	BIGNUM	**g;		/* public key vector */
	BIGNUM	**xbar;		/* private key vector 1 */
	BIGNUM	**xhat;		/* private key vector 2 */
	BIGNUM	*b;		/* group key */
	BIGNUM	*binverse;	/* inverse group key */
	BIGNUM	*biga;		/* mysterious capital letter */
	BIGNUM	*k;		/* random roll */
	BIGNUM	*gbar;		/* public key 1 */
	BIGNUM	*ghat;		/* public key 2 */
	BIGNUM	*u, *v, *w;	/* BN scratch */
	u_char	seed[20];	/* seed for parameters */
	int	i, j, n;
	FILE	*str;
	u_int	temp;
	char	ident[20];

	/*
	 * Generate DSA parameters for use as MV parameters.
	 */
	printf("Generating MV parameters (%d bits)...\n", modulus);
	n = nkeys;
	RAND_bytes(seed, sizeof(seed));
	dsa = DSA_generate_parameters(modulus, seed, sizeof(seed),
	    NULL, NULL, cb, "MV");
	printf("\n");
	if (dsa == NULL) {
		printf("MV generate parameters fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		rval = -1;
		return;
	}

	/*
	 * Generate random polynomial roots mod q.
	 */
	printf("Generating polynomial roots (%d bits)...\n",
	    BN_num_bits(dsa->q));
	ctx = BN_CTX_new(); u = BN_new(); v = BN_new(); w = BN_new();
	n = nkeys;
	x = malloc((n + 1) * sizeof(BIGNUM));
	for (j = 1; j <= n; j++) {
		x[j] = BN_new();
		BN_rand(x[j], BN_num_bits(dsa->q), -1, 0);
		BN_mod(x[j], x[j], dsa->q, ctx);
	}

	/*
	 * Generate polynomial coefficients a[i], i = 0...n, from the
	 * expansion of root products (x - x[j]), j = 1...n. The method
	 * is a present from Charlie Boncelet.
	 */
	printf("Generating polynomial coefficients for %d keys\n", n); 
	a = malloc((n + 1) * sizeof(BIGNUM));
	for (i = 0; i <= n; i++) {
		a[i] = BN_new();
		BN_one(a[i]);
	}
	for (j = 1; j <= n; j++) {
		BN_zero(w);
		for (i = 0; i < j; i++) {
			u = BN_dup(dsa->q);
			BN_mod_mul(v, a[i], x[j], dsa->q, ctx);
			BN_sub(u, u, v);
			BN_add(u, u, w);
			w = BN_dup(a[i]);
			BN_mod(a[i], u, dsa->q, ctx);
		}
	}

	/*
	 * Verify sum(a[i] x^i) = 0 for all j. By design, the polynomial
	 * has n zeros at x = x[j].
	 */
	temp = 1;
	for (j = 1; j <= n; j++) {
		BN_zero(u);
		for (i = 0; i <= n; i++) {
			BN_set_word(v, i);
			BN_mod_exp(v, x[j], v, dsa->q, ctx);
			BN_mod_mul(v, v, a[i], dsa->q, ctx);
			BN_add(u, u, v);
		}
		BN_mod(u, u, dsa->q, ctx);
		if (!BN_is_zero(u))
			temp = 0;
	}
	printf("Confirm sum(a[i] x[j]^i) = 0 for all i, j: %s\n", temp ?
	    "yes" : "no");

	/*
	 * Generate g[i] = g^a[i] for all i and the generator g.
	 */
	printf("Generating g[i] parameters\n");
	g = malloc((n + 1) * sizeof(BIGNUM));
	for (i = 0; i <= n; i++) {
		g[i] = BN_new();
		BN_mod_exp(g[i], dsa->g, a[i], dsa->p,
		    ctx);
	}

	/*
	 * Verify prod(g[i]^(x[j]^i)) = 1 for all i, j.
	 */
	temp = 1;
	for (j = 1; j <= n; j++) {
		BN_one(u);
		for (i = 0; i <= n; i++) {
			BN_set_word(v, i);
			BN_mod_exp(v, x[j], v, dsa->q, ctx);
			BN_mod_exp(v, g[i], v, dsa->p, ctx);
			BN_mod_mul(u, u, v, dsa->p, ctx);
		}
		if (!BN_is_one(u))
			temp = 0;
	}
	printf("Confirm prod(g[i]^(x[j]^i)) = 1 for all i, j: %s\n",
	    temp ? "yes" : "no");

	/*
	 * Make 512-bit encryption key A and 160-bit nonce pair b and
	 * b^-1.
	 */
	biga = BN_new();
	BN_one(biga);
	for (j = 1; j <= n; j++) {
		for (i = 0; i < n; i++) {
			BN_set_word(v, i);
			BN_mod_exp(v, x[j], v, dsa->q, ctx);
			BN_mod_exp(v, g[i], v, dsa->p, ctx);
			BN_mod_mul(biga, biga, v, dsa->p, ctx);
		}
	}
	b = BN_new(); binverse = BN_new();
	BN_rand(b, BN_num_bits(dsa->q), -1, 0);
	BN_mod(b, b, dsa->q, ctx);
	BN_mod_inverse(binverse, b, dsa->q, ctx);
	BN_mod_mul(v, b, binverse, dsa->q, ctx);
	printf("Confirm b b^-1 = 1: %s\n", BN_is_one(v) ?
	    "yes" : "no");

	/*
	 * Make 160-bit decryption keys (xbar[j], xhat[j]) for all j.
	 */
	xbar = malloc((n + 1) * sizeof(BIGNUM));
	xhat = malloc((n + 1) * sizeof(BIGNUM));
	for (j = 1; j <= n; j++) {
		xbar[j] = BN_new(); xhat[j] = BN_new();
		BN_zero(xbar[j]);
		for (i = 1; i <= n; i++) {
			if (i == j)
				continue;
			BN_set_word(v, n);
			BN_mod_exp(u, x[i], v, dsa->q, ctx);
			BN_add(xbar[j], xbar[j], u);
		}
		BN_mod_mul(xbar[j], xbar[j], binverse, dsa->q, ctx);
		BN_set_word(v, n);
		BN_mod_exp(xhat[j], x[j], v, dsa->q, ctx);
	}

	/*
	 * Verify A g^(b xbar[j]) g^xhat[j] = 1 for all j.
	 */
	temp = 1;
	for (j = 1; j <= n; j++) {
		BN_mod_mul(u, b, xbar[j], dsa->q, ctx);
		BN_mod_exp(u, dsa->g, u, dsa->p, ctx);
		BN_mod_exp(v, dsa->g, xhat[j], dsa->p, ctx);
		BN_mod_mul(u, u, v, dsa->p, ctx);
		BN_mod_mul(u, u, biga, dsa->p, ctx);
		if (!BN_is_one(u))
			temp = 0;
	}
	printf("Confirm A g^b xbar[j] g^xhat[j] = 1 for all j: %s\n",
	    temp ? "yes" : "no");

	/*
	 * Make 512-bit values A = A^k, gbar = g^k and ghat = g^bk.
	 */
	k = BN_new();
	BN_rand(k, BN_num_bits(dsa->q), -1, 0);
	BN_mod(k, k, dsa->q, ctx);
	BN_mod_exp(biga, biga, k, dsa->p, ctx);
	gbar = BN_new(); ghat = BN_new();
	BN_mod_exp(gbar, dsa->g, k, dsa->p, ctx);
	BN_mod_mul(u, k, b, dsa->q, ctx);
	BN_mod_exp(ghat, dsa->g, u, dsa->p, ctx);

	/*
	 * Verify A gbar^xbar[j] ghat^xhat[j] = 1 for all j.
	 */
	temp = 1;
	for (j = 1; j <= n; j++) {
		BN_mod_exp(v, gbar, xhat[j], dsa->p, ctx);
		BN_mod_exp(u, ghat, xbar[j], dsa->p, ctx);
		BN_mod_mul(u, u, v, dsa->p, ctx);
		BN_mod_mul(u, biga, u, dsa->p, ctx);
		if (!BN_is_one(u))
			temp = 0;
	}
	printf(
	    "Confirm A gbar^xbar[j] ghat^xhat[j] = 1 for all j: %s\n",
	    temp ? "yes" : "no");

	/*
	 * We now have the modulus p, encryption key A, public key
	 * (gbar^k, ghat^bk) and a set of decryption keys (xbar[j],
	 * xhat[j]) for all j. The broadcaster parameters and keys are
	 * contained in a DSA cuckoo structure:
	 *
	 * p		modulus p
	 * q		private encryption key A
	 * g		diffusion factor k (not used)
	 * priv_key	public key 1 gbar
	 * pub_key	public key 2 ghat
	 */
	BN_copy(dsa->q, biga);
	BN_copy(dsa->g, k);
	dsa->priv_key = BN_dup(gbar);
	dsa->pub_key = BN_dup(ghat);

	/*
	 * Write the parameters and public key as a DSA private key
	 * encoded in PEM. This is used only by the broadcaster(s).
	 */
	str = fheader("MVkey", trustname);
	PEM_write_DSAPrivateKey(str, dsa, passwd ? EVP_des_cbc() :
	    NULL, NULL, 0, NULL, passwd);
	fclose(str);
	if (debug)
		DSA_print_fp(stdout, dsa, 0);
	fslink(id, trustname);

	/*
	 * Write the parameters and private key (xbar[j], xhat[j]) for
	 * all j as a DSA private key encoded in PEM. It is used only by
	 * the designated recipient(s) who pay a suitably outrageous fee
	 * for the service.
	 *
	 * The receiver parameters and keys are contained in the DSA
	 * cuckoo structure:
	 *
	 * p		modulus p
	 * q		private key 1 xbar[j]
	 * g		private key 2 xhat[j]
	 * priv_key	public key 1 gbar
	 * pub_key	public key 2 ghat
	 */
	for (j = 1; j <= n; j++) {
		BN_copy(dsa->q, xbar[j]);
		BN_copy(dsa->g, xhat[j]);
		BN_free(xbar[j]); BN_free(xhat[j]); BN_free(x[j]);

		/*
		 * Write the MV public key as a DSA private key encoded
		 * in PEM. In this context the public key is really
		 * public only to the designated recipients and denied
		 * to all others.
		 */
#if 0
		sprintf(ident, "MVkey%d", j);
		str = fheader(ident, trustname);
		PEM_write_DSAPrivateKey(str, dsa, passwd ?
		    EVP_des_cbc() : NULL, NULL, 0, NULL, passwd);
		fclose(str);
		if (debug)
			DSA_print_fp(stdout, dsa, 0);
#endif
	}

	/*
	 * Free the coutries.
	 */
	for (i = 0; i <= n; i++) {
		BN_free(a[i]);
		BN_free(g[i]);
	}
	BN_free(u); BN_free(v); BN_free(w); BN_CTX_free(ctx);
	BN_free(b); BN_free(binverse); BN_free(biga); BN_free(k);
	BN_free(gbar); BN_free(ghat);

	/*
	 * Free the world.
	 */
	free(x); free(a); free(g); free(xbar); free(xhat);
	DSA_free(dsa);
	return;
}


/*
 * Generate X509v3 self-signed certificate.
 *
 * The certificate consists of the version number, serial number,
 * validity interval, issuer name, subject name and public key. For a
 * self-signed certificate, the issuer name is the same as the subject
 * name and these items are signed using the subject private key. The
 * validity interval extends from the current time to the same time one
 * year hence. For NTP purposes, it is convenient to use the NTP seconds
 * of the current time as the serial number.
 */
int
x509	(
	EVP_PKEY *pkey,		/* generic signature algorithm */
	const EVP_MD *md,	/* generic digest algorithm */
	char	*gqpub,		/* identity extension (hex string) */
	char	*exten		/* private cert extension */
	)
{
	X509	*cert;		/* X509 certificate */
	X509_NAME *subj;	/* distinguished (common) name */
	X509_EXTENSION *ex;	/* X509v3 extension */
	FILE	*str;		/* file handle */
	ASN1_INTEGER *serial;	/* serial number */
	const char *id;		/* digest/signature scheme name */
	u_char	pathbuf[MAXFILENAME + 1];

	/*
	 * Generate X509 self-signed certificate.
	 *
	 * Set the certificate serial to the NTP seconds for grins. Set
	 * the version to 3. Set the subject name and issuer name to the
	 * subject name in the request. Set the initial validity to the
	 * current time and the final validity one year hence. 
	 */
	id = OBJ_nid2sn(md->pkey_type);
	printf("Generating certificate %s\n", id);
	cert = X509_new();
	X509_set_version(cert, 2L);
	serial = ASN1_INTEGER_new();
	ASN1_INTEGER_set(serial, epoch + JAN_1970);
	X509_set_serialNumber(cert, serial);
	ASN1_INTEGER_free(serial);
	X509_gmtime_adj(X509_get_notBefore(cert), 0L);
	X509_gmtime_adj(X509_get_notAfter(cert), YEAR);
	subj = X509_get_subject_name(cert);
	X509_NAME_add_entry_by_txt(subj, "commonName", MBSTRING_ASC,
	    hostname, strlen(hostname), -1, 0);
	subj = X509_get_issuer_name(cert);
	X509_NAME_add_entry_by_txt(subj, "commonName", MBSTRING_ASC,
	    trustname, strlen(trustname), -1, 0);
	if (!X509_set_pubkey(cert, pkey)) {
		printf("Assign key fails\n%s\n",
		ERR_error_string(ERR_get_error(), NULL));
		X509_free(cert);
		rval = -1;
		return (0);
	}

	/*
	 * Add X509v3 extensions if present. These represent the minimum
	 * set defined in RFC3280 less the certificate_policy extension,
	 * which is seriously obfuscated in OpenSSL.
	 */
	/*
	 * The basic_constraints extension CA:TRUE allows servers to
	 * sign client certficitates.
	 */
	printf("%s: %s\n", LN_basic_constraints, BASIC_CONSTRAINTS);
	ex = X509V3_EXT_conf_nid(NULL, NULL, NID_basic_constraints,
	    BASIC_CONSTRAINTS);
	if (!X509_add_ext(cert, ex, -1)) {
		printf("Add extension field fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		rval = -1;
		return (0);
	}
	X509_EXTENSION_free(ex);

	/*
	 * The key_usage extension designates the purposes the key can
	 * be used for.
	 */
	printf("%s: %s\n", LN_key_usage, KEY_USAGE);
	ex = X509V3_EXT_conf_nid(NULL, NULL, NID_key_usage, KEY_USAGE);
	if (!X509_add_ext(cert, ex, -1)) {
		printf("Add extension field fails\n%s\n",
		    ERR_error_string(ERR_get_error(), NULL));
		rval = -1;
		return (0);
	}
	X509_EXTENSION_free(ex);
	/*
	 * The subject_key_identifier is used for the GQ group key.
	 * This should not be controversial.
	 */
	if (gqpub != NULL) {
		printf("%s\n", LN_subject_key_identifier);
		ex = X509V3_EXT_conf_nid(NULL, NULL,
		    NID_subject_key_identifier, gqpub);
		if (!X509_add_ext(cert, ex, -1)) {
			printf("Add extension field fails\n%s\n",
			    ERR_error_string(ERR_get_error(), NULL));
			rval = -1;
			return (0);
		}
		X509_EXTENSION_free(ex);
	}

	/*
	 * The extended key usage extension is used for special purpose
	 * here. The semantics probably do not conform to the designer's
	 * intent and will likely change in future.
	 * 
	 * "trustRoot" designates a root authority
	 * "private" designates a private certificate
	 */
	if (exten != NULL) {
		printf("%s: %s\n", LN_ext_key_usage, exten);
		ex = X509V3_EXT_conf_nid(NULL, NULL,
		    NID_ext_key_usage, exten);
		if (!X509_add_ext(cert, ex, -1)) {
			printf("Add extension field fails\n%s\n",
			    ERR_error_string(ERR_get_error(), NULL));
			rval = -1;
			return (0);
		}
		X509_EXTENSION_free(ex);
	}

	/*
	 * Sign and verify.
	 */
	X509_sign(cert, pkey, md);
	if (!X509_verify(cert, pkey)) {
		printf("Verify %s certificate fails\n%s\n", id,
		    ERR_error_string(ERR_get_error(), NULL));
		X509_free(cert);
		rval = -1;
		return (0);
	}

	/*
	 * Write the certificate encoded in PEM.
	 */
	sprintf(pathbuf, "%scert", id);
	str = fheader(pathbuf, hostname);
	PEM_write_X509(str, cert);
	fclose(str);
	if (debug)
		X509_print_fp(stdout, cert);
	X509_free(cert);
	fslink("cert", hostname);
	return (1);
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
	 * Extract time string YYMMDDHHMMSSZ from ASN.1 time structure.
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
		printf("%s\t\t\t\t%d %d %lu\r", (char *)chr, n1, n2,
		    d2);
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
 * Generate key
 */
EVP_PKEY *			/* public/private key pair */
genkey(
	char	*type,		/* key type (RSA or DSA) */
	char	*id		/* file name id */
	)
{
	if (type == NULL)
		return (NULL);
	if (strcmp(type, "RSA") == 0)
		return (gen_rsa(id));

	else if (strcmp(type, "DSA") == 0)
		return (gen_dsa(id));

	printf("Invalid %s key type %s\n", id, type);
	rval = -1;
	return (NULL);
}


/*
 * Generate file header
 */
FILE *
fheader	(
	const char *id,		/* file name id */
	const char *name	/* owner name */
	)
{
	FILE	*str;		/* file handle */

	sprintf(filename, "ntpkey_%s_%s.%lu", id, name, epoch +
	    JAN_1970);
	if ((str = fopen(filename, "w")) == NULL) {
		perror("Write");
		exit (-1);
	}
	fprintf(str, "# %s\n# %s", filename, ctime(&epoch));
	return (str);
}


/*
 * Generate symbolic links
 */
void
fslink(
	const char *id,		/* file name id */
	const char *name	/* owner name */
	)
{
	char	linkname[MAXFILENAME]; /* link name */
	int	temp;

	sprintf(linkname, "ntpkey_%s_%s", id, name);
	remove(linkname);
	temp = symlink(filename, linkname);
	if (temp < 0)
		perror(id);
	printf("Generating new %s file and link\n", id);
	printf("%s->%s\n", linkname, filename);
}
