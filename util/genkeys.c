
/*
 * Program to generate cryptographic keys for NTP clients and servers
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "ntp_machine.h"
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
 */
int
main(
	int argc,
	char *argv[]
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
	 * Initialize host name and timestamp.
	 */
	gethostname(hostname, sizeof(hostname));
	gettimeofday(&tv, 0);
	ntptime = tv.tv_sec + JAN_1970;

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
	 * Generate the X509v3 certificate request. The digest algorithms
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
	 * Generate the X509v3 certificate request. The digest algorithms
	 * that work with DSS (DSA) are DSS and DSS1.
	 */
	x509("DSA_SHA", pkey, EVP_dss());
	x509("DSA_SHA1", pkey, EVP_dss1());
	free(pkey);

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
#endif /* OPENSSL */
	return (0);
}

#ifdef OPENSSL
/*
 * Generate X509v3 certificate request and X509v3 self-signed certificate.
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
	X509_REQ_set_version(req, 2L);
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
	X509_set_version(cert, 2L);
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
		printf("%s %d %d %lu\r", chr, n1, n2, d0);
		break;
	case 1:
		d1++;
		printf("%s\t\t%d %d %lu\r", chr, n1, n2, d1);
		break;
	case 2:
		d2++;
		printf("%s\t\t\t\t%d %d %lu\r", chr, n1, n2, d2);
		break;
	case 3:
		d3++;
		printf("%s\t\t\t\t\t\t%d %d %lu\r", chr, n1, n2,
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
