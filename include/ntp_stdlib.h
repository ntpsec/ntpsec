/*
 * ntp_stdlib.h - Prototypes for NTP lib.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include "ntp_rfc2553.h"

#include "ntp_types.h"
#include "ntp_string.h"
#include "l_stdlib.h"

/*
 * Handle gcc __attribute__ if available.
 */
#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 5) || (defined(__STRICT_ANSI__))
#  define __attribute__(Spec) /* empty */
# endif
/* The __-protected variants of `format' and `printf' attributes
   are accepted by gcc versions 2.6.4 (effectively 2.7) and later.  */
# if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7)
#  define __format__ format
#  define __printf__ printf
# endif
#endif

# include <stdarg.h>
extern	void	msyslog		(int, const char *, ...)
				__attribute__((__format__(__printf__, 2, 3)));
extern	void	netsyslog	(int, const char *, ...)
				__attribute__((__format__(__printf__, 2, 3)));

extern	void	auth_delkeys	(void);
extern	int	auth_havekey	(keyid_t);
extern	int	authdecrypt	(keyid_t, u_int32 *, int, int);
extern	int	authencrypt	(keyid_t, u_int32 *, int);
extern	int	authhavekey	(keyid_t);
extern	int	authistrusted	(keyid_t);
extern	int	authreadkeys	(const char *);
extern	void	authtrust	(keyid_t, u_long);
extern	int	authusekey	(keyid_t, int, const u_char *);

extern	u_long	calyearstart	(u_long);
extern	const char *clockname	(int);
extern	int	clocktime	(int, int, int, int, int, u_long, u_long *, u_int32 *);
#if !defined(_MSC_VER) || !defined(_DEBUG)
extern	void *	emalloc		(u_int);
#else
#define		emalloc(size)	debug_emalloc(size, __FILE__, __LINE__)
extern	void *	debug_emalloc	(u_int, char *, int);
#endif
extern	int	ntp_getopt	(int, char **, const char *);
extern	void	init_auth	(void);
extern	void	init_lib	(void);
extern	struct savekey *auth_findkey (keyid_t);
extern	int	auth_moremem	(void);
extern	int	ymd2yd		(int, int, int);

extern	int	MD5authdecrypt	(u_char *, u_int32 *, int, int);
extern	int	MD5authencrypt	(u_char *, u_int32 *, int);
extern	void	MD5auth_setkey	(keyid_t, const u_char *, const int);
extern	u_int32	addr2refid	(struct sockaddr_storage *);

extern	int	atoint		(const char *, long *);
extern	int	atouint		(const char *, u_long *);
extern	int	hextoint	(const char *, u_long *);
extern	char *	humanlogtime	(void);
extern	char *	inttoa		(long);
extern	char *	mfptoa		(u_long, u_long, short);
extern	char *	mfptoms		(u_long, u_long, short);
extern	const char * modetoa	(int);
extern  const char * eventstr   (int);
extern  const char * ceventstr  (int);
extern	char *	statustoa	(int, int);
extern  const char * sysstatstr (int);
extern  const char * peerstatstr (int);
extern  const char * clockstatstr (int);
extern	struct sockaddr_storage* netof (struct sockaddr_storage*);
extern	char *	numtoa		(u_int32);
extern	char *	numtohost	(u_int32);
extern char * socktoa           (struct sockaddr_storage *);
extern char * socktohost        (struct sockaddr_storage *);
extern	int	octtoint	(const char *, u_long *);
extern	u_long	ranp2		(int);
extern	char *	refnumtoa	(struct sockaddr_storage *);
extern	int	tsftomsu	(u_long, int);
extern	char *	uinttoa		(u_long);

extern	int	decodenetnum	(const char *, struct sockaddr_storage *);

extern	const char *	FindConfig	(const char *);

extern	void	signal_no_reset (int, RETSIGTYPE (*func)(int));

extern	void	getauthkeys 	(const char *);
extern	void	auth_agekeys	(void);
extern	void	rereadkeys	(void);

/*
 * Variable declarations for libntp.
 */

/*
 * Defined by any program.
 */
extern volatile int debug;		/* debugging flag */

/* authkeys.c */
extern u_long	authkeynotfound;	/* keys not found */
extern u_long	authkeylookups;		/* calls to lookup keys */
extern u_long	authnumkeys;		/* number of active keys */
extern u_long	authkeyexpired;		/* key lifetime expirations */
extern u_long	authkeyuncached;	/* cache misses */
extern u_long	authencryptions;	/* calls to encrypt */
extern u_long	authdecryptions;	/* calls to decrypt */

extern int	authnumfreekeys;

/*
 * The key cache. We cache the last key we looked at here.
 */
extern keyid_t	cache_keyid;		/* key identifier */
extern u_char *	cache_key;		/* key pointer */
extern u_int	cache_keylen;		/* key length */

/* getopt.c */
extern char *	ntp_optarg;		/* global argument pointer */
extern int	ntp_optind;		/* global argv index */

/* machines.c */
extern const char *set_tod_using;

/* mexit.c */
#if defined SYS_WINNT || defined SYS_CYGWIN32
extern HANDLE	hServDoneEvent;
#endif

/* systime.c */
extern double	sys_tick;		/* adjtime() resolution */

/* version.c */
extern const char *Version;		/* version declaration */
