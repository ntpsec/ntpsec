/*****************************************************************************

ntp_intercept.c - capture and replay logic for NTP environment calls

Think of ntpd as a complex finite-state machine for transforming a
stream of input events to output events.  Events are of the
following kinds:

1. Startup, capturing command-line switches.

2. Configuration read.

3. Time reports from reference clocks.

4. Time calls to the host system clock.

5. Read and write of the system drift file.

6. Calls to the host's random-number generator.

7. Calls to adjtime/ntp_adjtime/adjtime to adjust the system clock.

8  Calls to ntp_set_tod to set the system clock.

9. Read of the system leapsecond file.

10. Packets incoming from NTP peers and others.

11. Packets outgoing to NTP peers and others.

12. Read of authkey file

13. Termination.

We must support two modes of operation.  In "capture" mode, ntpd
operates normally, logging all events.  In "replay" mode, ntpd accepts
an event-capture log and replays it, processing all input events in a
previous capture.

We say that test performance is *stable* when replay mode is
idempotent - that is, replaying an event-capture log produces an exact
copy of itself by duplicating the same output events.

When test performance is stable, we know two things: (1) we have
successfully captured all inputs of the system, and (2) the code
has experienced no functional regressions since the event capture.

We can regression-test the code by capturing logs of production
behavior and replaying them.  We can also hand-craft tests to probe
edge cases.  To support the latter case, it is highly desirable that the
event-capture format be a text stream in an eyeball-friendly,
readily-editable format.

== Implementation ==

ntpd needs two new switches: capture and replay.  The capture switch
says: to log all event calls to an event capture file in addition to
their normal behaviors.  This includes both read events (such as
refclock inputs) and write events (such as adjtimex calls).

The replay switch has more complex behavior. Interpret a capture
file. Mock all event calls with code that looks at each event
sequentially from the capture.  If a read call is performed, and the
next data in the log file is for that read call, the logged data is
returned for the call.  If a write call is performed, the call type and
call data is compared to the next log data.  If the next event doesn't
match the expected type or has different data, show the difference and
terminate - the replay failed.  Otherwise continue.

Replay succeeds if the event stream reaches the shutdown event with
no mismatches.

== Limitations ==

Reference-clock events are not yet intercepted.

*****************************************************************************/

#include <time.h>
#include <sys/time.h>

#include <config.h>

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libgen.h>
#include <sys/socket.h>
#include <netdb.h>
#include <inttypes.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_control.h"
#include "ntp_unixtime.h"
#include "ntp_config.h"
#include "ntp_crypto.h"
#include "ntp_assert.h"
#include "ntp_intercept.h"
#include "ntp_fp.h"
#include "lib_strbuf.h"
#include "ntp_stdlib.h"
#include "ntp_syscall.h"
#include "ntp_leapsec.h"

static intercept_mode mode = none;

static char linebuf[256];
static int lineno;

intercept_mode intercept_get_mode(void)
{
    return mode;
}

void intercept_set_mode(intercept_mode newmode)
{
    mode = newmode;
    if (mode != none) {
	syslogit = false;
	hashprefix = true;
	force_synchronous_dns = true;
    }
}

static void get_operation(const char *expect)
/* get the next (non-comment) line from the log */
{
    for (;;)
    {
	char *in = fgets(linebuf, sizeof(linebuf), stdin);

	++lineno;

	if (in == NULL) {
	    fputs("ntpd: replay failed, unexpected EOF\n", stderr);
	    exit(1);
	}
	    
	if (expect != NULL && strncmp(linebuf, expect, strlen(expect)) != 0) {
	    fprintf(stderr, "ntpd: replay failed on line %d, expected %s but saw %*s",
		    lineno, expect, (int)strlen(expect), linebuf);
	    exit(1);
	}
	    
	if (linebuf[0] != '#')
	    break;
    }
	
}

void intercept_argparse(int *argc, char ***argv)
{
    int i;
    const char *leader = "NTP replay version 1";

    for (i = 1; i < *argc; i++)
	if (strcmp((*argv)[i], "-y") == 0)
	    intercept_set_mode(capture);
	else if  (strcmp((*argv)[i], "-Y") == 0)
	    intercept_set_mode(replay);

    if (mode == capture)
    {
	printf("%s\n", leader);

	printf("startup");
	for (i = 1; i < *argc; i++)
	    if (strcmp((*argv)[i], "-y") != 0 && strcmp((*argv)[i], "-Y") != 0)
		printf(" %s", (*argv)[i]);
	putchar('\n');
    }
    else if (mode == replay)
    {
	char *cp;
	bool was_space = true;

	/* require a log first line that matches what we can interpret */
	get_operation(leader);
	
	get_operation("startup ");
	*argc = 0;
	for (cp = strdup(linebuf + 9); *cp; cp++) {
	    if (was_space && !isspace((int)*cp))
		(*argv)[(*argc)++] = cp;
	    was_space = isspace((int)*cp);
	    if (was_space)
		*cp = '\0';
	}
	/* coverity[leaked_storage] */
    }
}

static bool pump(const char *fn, const char *lead, const char *trail, FILE *ofp)
{
	FILE *fp = fopen(fn, "r");
	if (fp == NULL)
	    return false;
	else
	{
	    int c;

	    fputs(lead, ofp);
	    while ((c = fgetc(fp)) != EOF)
		fputc(c, ofp);
	    fclose(fp);
	    fputs(trail, ofp);
	    return true;
	}
}

static void file_replay(const char *configfile, char *delimiter, char *tempfile)
{
    FILE *tfp;

    tfp = fopen(tempfile, "w");
    if (tfp == NULL) {
	fprintf(stderr,
		"ntpd: replay failed, can't copy %s\n", configfile);
	exit(1);
    }
    for (;;) {
	char *nextline = fgets(linebuf, sizeof(linebuf), stdin);

	++lineno;
	if (nextline == NULL) {
	    fprintf(stderr,
		    "ntpd: replay failed, unexpected EOF at line %d\n", lineno);
	    exit(1);
	}
	if (strncmp(linebuf, delimiter, strlen(delimiter)) == 0)
	    break;
	fputs(linebuf, tfp);
    }
    fclose(tfp);
}

bool intercept_getaddrinfo(char *hname, sockaddr_u *peeraddrp)
{
    if (mode == replay) {
	char addr[BUFSIZ], ip[BUFSIZ];
	int retval;

	get_operation("getaddrinfo");
	if (sscanf(linebuf, "getaddrinfo %s %s %d", addr, ip, &retval) != 3) {
	    fprintf(stderr, "ntpd: garbled getaddrinfo format, line %d\n", lineno);
	    exit(1);
	}
	if (strcmp(hname, addr) != 0) {
	    fprintf(stderr, "ntpd: saw hostname %s, when expecting %s\n",
		    addr, hname);
	    exit(1);
	}
	if (retval == 0) {
	    fprintf(stderr, "ntpd: hostname %s lookup failed\n",
		    addr);
	    exit(1);
	}
	if (!is_ip_address(ip, AF_UNSPEC, peeraddrp)) {
	    fprintf(stderr, "ntpd: invalid IP address in getaddrinfo at line %d\n", lineno);
	    exit(1);
	}
	return true;
    } else {
	int a_info;
	size_t octets;
	struct addrinfo		hints, *res;

	ZERO(hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	a_info = getaddrinfo(hname, "ntp", &hints, &res);
	if (a_info == EAI_NONAME
#ifdef EAI_NODATA
	    || a_info == EAI_NODATA
#endif
	    ) {
	    hints.ai_flags = AI_CANONNAME;
	    hints.ai_flags |= AI_ADDRCONFIG;
	    a_info = getaddrinfo(hname, "ntp", &hints, &res);
	}
	if (mode == capture)
	    printf("getaddrinfo %s %s %d\n",
		   hname,
		   socktoa((sockaddr_u *)res->ai_addr),
		   a_info == 0);
	if (a_info != 0) {
	    msyslog(LOG_ERR,
		    "hostname %s can not be used (%s), please use IP address.",
		    hname, gai_strerror(a_info));
	    return false;
	} else {
	    INSIST(res != NULL);
	    memset(peeraddrp, '\0', sizeof(*peeraddrp));
	    octets = min(sizeof(*peeraddrp), res->ai_addrlen);
	    memcpy(peeraddrp, res->ai_addr, octets);
	    freeaddrinfo(res);
	    return true;
	}
    }
}

void intercept_getconfig(const char *configfile)
{
    if (mode == replay) {
	char tempfile[PATH_MAX];

	stats_control = false;	/* suppress writing stats files */
	get_operation("startconfig");
	snprintf(tempfile, sizeof(tempfile), ".fake_ntp_config_%d", getpid());
	file_replay(configfile, "endconfig", tempfile);
	readconfig(getconfig(tempfile));
	unlink(tempfile);
    } else {
	/* this can be null if the default config doesn't exist */
	configfile = getconfig(configfile);

	if (configfile != NULL && mode == capture)
	    pump(configfile, "startconfig\n", "endconfig\n", stdout);

	readconfig(configfile);
    }
}

void intercept_get_systime(const char *legend, l_fp *now)
{
    struct timespec ts;	/* seconds and nanoseconds */

    if (mode == replay) {
	int sec, subsec;
	char expecting[BUFSIZ];
	get_operation("systime ");
	if (sscanf(linebuf, "systime %s %d.%d", expecting, &sec, &subsec) != 3) {
	    fprintf(stderr, "ntpd: garbled systime format, line %d\n", lineno);
	    exit(1);
	}
	else if (strcmp(legend, expecting) != 0) {
	    fprintf(stderr, "ntpd: expected systime %s on line %d but saw %s\n",
		    expecting, lineno, legend);
	    exit(1);
	}
	ts.tv_sec = sec;
	ts.tv_nsec = subsec;
	normalize_time(ts, 0, now);
    } else {
	get_ostime(&ts);
	normalize_time(ts, sys_fuzz > 0.0 ? ntp_random() : 0, now);
	if (mode == capture)
	    printf("systime %s %ld.%ld\n",
		   legend, (long)ts.tv_sec, (long)ts.tv_nsec);

    }
}

long intercept_ntp_random(const char *legend)
{
    long roll;

    if (mode == replay) {
	char expecting[BUFSIZ];
	/*
	 * Presently we're only using this as a check on the call sequence,
	 * as all the environment-altering functions that call ntp_random()
	 * are themselves intercepted.
	 */
	get_operation("random ");
	if (sscanf(linebuf, "random %s %ld", expecting, &roll) != 2) {
	    fprintf(stderr, "ntpd: garbled random format, line %d\n", lineno);
	    exit(1);
	}
	else if (strcmp(legend, expecting) != 0) {
	    fprintf(stderr, "ntpd: expected random %s from line %d but saw %s\n",
		    expecting, lineno, legend);
	    exit(1);
	}
	return roll;
    } else {
	roll = ntp_random();

	if (mode == capture)
	    printf("random %s %ld\n", legend, roll);
    }

    return roll;
}

bool intercept_drift_read(const char *drift_file, double *drift)
{
    if (mode == replay) {
	float df;
	get_operation("drift-read ");
	if (strstr(linebuf, "false") != NULL)
	    return false;
	/*
	 * Weirdly, sscanf has no format for reading doubles.
	 * This could cause an obscure bug in replay if drift 
	 * ever requires 53 bits of precision as opposed to 24
	 * (and that's assuming IEEE-754, otherwise things could
	 * get hairier).
	 */ 
	if (sscanf(linebuf, "drift-read %f'", &df) != 1) {
	    fprintf(stderr, "ntpd: garbled drift-read format, line %d\n",lineno);
	    exit(1);
	}
	*drift = df;
    } else {
	FILE *fp;

	if ((fp = fopen(drift_file, "r")) == NULL) {
	    if (mode == capture)
		printf("drift-read false\n");
	    return false;
	}

	if (fscanf(fp, "%lf", drift) != 1) {
	    msyslog(LOG_ERR,
		    "format error frequency file %s",
		    drift_file);
	    fclose(fp);
	    if (mode == capture)
		printf("drift-read false\n");
	    return false;
	}
	fclose(fp);

	if (mode == capture)
	    printf("drift-read %.3f\n", *drift);

    }

    return true;
}

void intercept_drift_write(char *driftfile, double drift)
{
    if (mode == replay) {
	/*
	 * We don't want to actually mess with the system's timekeeping in
	 * replay mode, so just check that we're writing out the same drift. 
	 */
	float df;
	get_operation("drift-write ");
	/* See the comment of drift-read checking. */ 
	if (sscanf(linebuf, "drift-write %f'", &df) != 1) {
	    fprintf(stderr, "ntpd: garbled drift-write format, line %d\n",lineno);
	    exit(1);
	}
	/* beware spurious failures here due to float imprecision */
	if (df != drift) {
	    fprintf(stderr, "ntpd: expected drift %f but saw %f, line %d\n",
		    drift, df, lineno);
	    exit(1);
	}
    } else {
	int fd;
	char tmpfile[PATH_MAX], driftcopy[PATH_MAX];
	char driftval[32];

	strlcpy(driftcopy, driftfile, PATH_MAX);
	strlcpy(tmpfile, dirname(driftcopy), sizeof(tmpfile));
	strlcat(tmpfile, "/driftXXXXXX", sizeof(tmpfile));
	/* coverity[secure_temp] Warning is bogus on POSIX-compliant systems*/
	if ((fd = mkstemp(tmpfile)) < 0) {
	    msyslog(LOG_ERR, "frequency file %s: %m", tmpfile);
	    return;
	}
	snprintf(driftval, sizeof(driftval), "%.3f\n", drift);
	IGNORE(write(fd, driftval, strlen(driftval)));
	(void)close(fd);
	/* atomic */
#ifdef SYS_WINNT
	if (_unlink(driftfile)) /* rename semantics differ under NT */
	    msyslog(LOG_WARNING,
		    "Unable to remove prior drift file %s, %m",
		    driftfile);
#endif /* SYS_WINNT */
	if (rename(tmpfile, driftfile))
	    msyslog(LOG_WARNING,
		    "Unable to rename temp drift file %s to %s, %m",
		    tmpfile, driftfile);

	if (mode == capture)
	    printf("drift-write %.3f\n", drift);
    }
}

int intercept_adjtime(const struct timeval *ntv, struct timeval *otv)
/* old-fashioned BSD call for systems with no PLL */
{
    if (mode == replay) {
	struct timeval rntv, rotv;
	long nsec, nusec, osec, ousec;
	get_operation("adjtime ");
	/* bletch - likely to foo up on 32-bit machines */
	if (sscanf(linebuf, "adjtime %ld %ld %ld %ld",
		   &nsec, &nusec, &osec, &ousec) != 4)
	{
	    fprintf(stderr, "ntpd: garbled adjtime format, line %d\n", lineno);
	    exit(1);
	}
	/* avoid compiler warnings due to time_t having an unexpected length */
	rntv.tv_sec = nsec;
	rntv.tv_usec = nusec;
	rotv.tv_sec = osec;
	rotv.tv_usec = ousec;
	if (ntv->tv_sec != rntv.tv_sec
	    || ntv->tv_usec != rntv.tv_usec
	    || otv->tv_sec != rotv.tv_sec
	    || otv->tv_usec != rotv.tv_usec)
	{
	    fprintf(stderr, "ntpd: adjtime expected %ld.%ld/%ld.%ld but saw %ld.%ld/%ld.%ld, line %d\n",
		    (long)rntv.tv_sec,
		    (long)rntv.tv_usec,
		    (long)rotv.tv_sec,
		    (long)rotv.tv_usec,  
		    (long)ntv->tv_sec,
		    (long)ntv->tv_usec,
		    (long)otv->tv_sec,
		    (long)otv->tv_usec,  
		    lineno);
	    exit(1);
	}
    } else {
	if (mode == capture)
	    printf("adjtime %ld %ld %ld %ld\n",
		   (long)ntv->tv_sec, (long)ntv->tv_usec,
		   (long)otv->tv_sec, (long)otv->tv_usec);

	return adjtime(ntv, otv);
    }
    return 0;
}

#ifdef HAVE_KERNEL_PLL
int intercept_ntp_adjtime(struct timex *tx)
/* for newer systems with PLLs */
{
#define ADJFMT "%u %ld %ld %ld %ld %i %ld %ld %ld %ld %ld %i %ld %ld %ld %ld"
#define ADJDUMP(x, buf)			\
    snprintf(buf, sizeof(buf), ADJFMT,	\
	     (x)->modes,			\
	     (x)->offset,			\
	     (x)->freq,			\
	     (x)->maxerror,		\
	     (x)->esterror,		\
	     (x)->status,			\
	     (x)->constant,		\
	     (x)->precision,		\
	     (x)->tolerance,		\
	     (x)->ppsfreq,		\
	     (x)->jitter,			\
	     (x)->shift,			\
	     (x)->jitcnt,			\
	     (x)->calcnt,			\
	     (x)->errcnt,			\
	     (x)->stbcnt)

    char txdump[BUFSIZ], rtxdump[BUFSIZ];
    ADJDUMP(tx, txdump);
    int res = 0;

    if (mode == replay)
    {
	struct timex rtx;
	get_operation("ntp_adjtime ");
	if (sscanf(linebuf, "ntp_adtime " ADJFMT " %d",
		   &rtx.modes,
		   &rtx.offset,
		   &rtx.freq,
		   &rtx.maxerror,
		   &rtx.esterror,
		   &rtx.status,
		   &rtx.constant,
		   &rtx.precision,
		   &rtx.tolerance,
		   &rtx.ppsfreq,
		   &rtx.jitter,
		   &rtx.shift,
		   &rtx.jitcnt,
		   &rtx.calcnt,
		   &rtx.errcnt,
		   &rtx.stbcnt,
		   &res) != 17)
	{
	    fprintf(stderr, "ntpd: garbled ntp_adtime format, line %d\n",
		    lineno);
	    exit(1);
	}

	ADJDUMP(&rtx, rtxdump);
	if (strcmp(txdump, rtxdump) != 0) {
	    printf("ntpd: adjtime mismatch at line %d, saw %s\n",
		   lineno, txdump);
	    exit(1);
	}
    } else {
	res = ntp_adjtime(tx);

	if (mode == capture)
	    printf("ntp_adjtime " ADJFMT " %d\n",
		   tx->modes,
		   tx->offset,
		   tx->freq,
		   tx->maxerror,
		   tx->esterror,
		   tx->status,
		   tx->constant,
		   tx->precision,
		   tx->tolerance,
		   tx->ppsfreq,
		   tx->jitter,
		   tx->shift,
		   tx->jitcnt,
		   tx->calcnt,
		   tx->errcnt,
		   tx->stbcnt,
		   res
		);
    }

    return res;
#undef ADJFMT
#undef ADJDUMP
}
#endif

int intercept_set_tod(struct timespec *tvs)
{
    char newset[BUFSIZ];
    snprintf(newset, sizeof(newset),
	     "set_tod %ld %ld\n", (long)tvs->tv_sec, (long)tvs->tv_nsec);
    
    if (mode == replay) {
	get_operation("set_tod");
	if (strcmp(linebuf, newset) != 0) {
	    fprintf(stderr, "ntpd: line %d, set_tod mismatch saw %s\n",
		    lineno, newset);
	    exit(1);
	}
	return 0;
    }
    else {
	if (mode == capture)
	    fputs(newset, stdout);
	return ntp_set_tod(tvs);
    }
}

bool
intercept_leapsec_load_file(
	const char  *leapsecfile,
	struct stat *sb_old,
	bool   force,
	bool   logall)
{
    bool loaded = true;

    if (mode == replay) {
	char tempfile[PATH_MAX];

	get_operation("startleapsec");
	snprintf(tempfile, sizeof(tempfile), ".fake_leapsec_file_%d", getpid());
	file_replay(leapsecfile, "endleapsec", tempfile);
	loaded = leapsec_load_file(tempfile, sb_old, force, logall);
	unlink(tempfile);
    } else {
	loaded = leapsec_load_file(leapsecfile, sb_old, force, logall);

	if (mode == capture)
	    pump(leapsecfile, "startleapsec\n", "endleapsec\n", stdout);
    }

    return loaded;
}

static char *lfpdump(l_fp *fp)
{
    char *buf;
    uint64_t	np;

    LIB_GETBUF(buf);

    np = fp->l_ui;
    np <<= FRACTION_PREC;
    np |= fp->l_uf;

    snprintf(buf, LIB_BUFLENGTH, "%" PRIu64, np);

    return buf;
}

static void packet_dump(char *buf, size_t buflen,
			sockaddr_u *dest, struct pkt *pkt, size_t len)
{
    size_t i;
    /*
     * Format is three tokens: source address, packet, MAC token. 
     *
     * FIXME: struct pkt fields are in network byte order. Need to
     * add htonl()/ntohl() calls here.
     */
    snprintf(buf, buflen, "%s %d:%d:%d:%d:%u:%u:%u:%s:%s:%s:%s ",
	   socktoa(dest),
	   pkt->li_vn_mode, pkt->stratum, pkt->ppoll, pkt->precision,
	   pkt->rootdelay, pkt->rootdisp,
	   pkt->refid,
	   lfpdump(&pkt->reftime), lfpdump(&pkt->org),
	   lfpdump(&pkt->rec), lfpdump(&pkt->xmt));

    if (len == LEN_PKT_NOMAC)
	strlcat(buf, "nomac", buflen);
    else
	/* dump MAC as len - LEN_PKT_NOMAC chars in hex */
	for (i = 0; i + LEN_PKT_NOMAC < len; i++) {
	    snprintf(buf + strlen(buf), buflen - strlen(buf),
		     "%02x", pkt->exten[i]);
	}
}

void intercept_sendpkt(const char *legend,
		  sockaddr_u *dest, struct interface *ep, int ttl,
		  void *pkt, int len)
{
    char pkt_dump[BUFSIZ], newpacket[BUFSIZ];

    /* FIXME: packet_dump expects a well formed struct pkt, but this
       function is also called from ntp_control.c with other data.
       To avoid a possible out-of-bounds read just shortcut to calling
       sendpkt when len < 48. */
    if(len < (int)LEN_PKT_NOMAC) {
      sendpkt(dest, ep, ttl, pkt, len);
      return;
    }

    packet_dump(pkt_dump, sizeof(pkt_dump), dest, (struct pkt*)pkt, len);
    snprintf(newpacket, sizeof(newpacket), "sendpkt %s %s\n", legend, pkt_dump);

    if (mode == replay)
    {
	get_operation("sendpkt ");
	if (strcmp(linebuf, newpacket) != 0) {
	    fprintf(stderr, "ntpd: line %d, sendpkt mismatch saw %s\n",
		    lineno, pkt_dump);
	    exit(1);
	}
    } else {
	sendpkt(dest, ep, ttl, pkt, len);

	if (mode == capture)
	    fputs(newpacket, stdout);
    }
}

ssize_t intercept_recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen)
{
    char pkt_dump[BUFSIZ];
    ssize_t recvlen;

    if (mode == replay)
    {
	get_operation("recvfrom ");
	/* FIXME: replay implementation here */
	recvlen = 0;  /* squish compiler warning */
    } else {
	char *cp;
	snprintf(pkt_dump, sizeof(pkt_dump),
		 "recvfrom %d %0x %s",
		 sockfd, flags, socktoa((sockaddr_u *)src_addr));
	for (cp = (char *)buf; cp < (char *)buf + len; cp++)
	    snprintf(pkt_dump + strlen(pkt_dump),
		     sizeof(pkt_dump) - strlen(pkt_dump),
		     "%0x", *cp);
	strlcat(pkt_dump, "\n", sizeof(pkt_dump));
    
	recvlen = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);

	if (mode == capture)
	    fputs(pkt_dump, stdout);
    }

    return recvlen;
}

SOCKET intercept_open_socket(sockaddr_u *addr,
			     bool bcast, bool turn_off_reuse, endpt *interface)
{
    char open_dump[BUFSIZ];
    int sock;

    if (mode == replay)
    {
	get_operation("open_socket ");
	/* FIXME: replay implementation here */
	sock = 0;  /* squish compiler warnings */
    } else {
	snprintf(open_dump, sizeof(open_dump),
		 "open_socket %s %d %d %s\n",
		 socktoa(addr), bcast, turn_off_reuse, interface->name);

	sock = open_socket(addr, bcast, turn_off_reuse, interface);

	if (mode == capture)
	    fputs(open_dump, stdout);
    }
    return sock;
}

void intercept_replay(void)
{
    printf("# entering replay loop at line %d\n", lineno);
    for (;;) {
	get_operation(NULL);
	if (strncmp(linebuf, "finish", 7) == 0)
	    break;
	else if (strncmp(linebuf, "sendpkt ", 8) == 0)
	    /*
	     * If we get here, this is a sendpkt generated not by the protocol
	     * machine but by an initial association setup. No way to check it,
	     * so skip it.
	     */
	    continue;
	else
	{
	    char errbuf[BUFSIZ], *cp;
	    strlcpy(errbuf, linebuf, sizeof(errbuf));
	    cp = strchr(errbuf, ' ');
	    if (cp != NULL)
		*cp = '\0';
	    fprintf(stderr, "ntpd: unexpected operation '%s' at line %d\n",
		    errbuf, lineno);
	    exit(1);
	}
    }
}

void
intercept_getauthkeys(const char *authkeysfile)
{
    if (mode == replay) {
	char tempfile[PATH_MAX];

	get_operation("startauthkeys");
	snprintf(tempfile, sizeof(tempfile), ".fake_ntp_authkeys_%d", getpid());
	file_replay(authkeysfile, "endauthkeys", tempfile);
	getauthkeys(tempfile);
	unlink(tempfile);
    } else {
	getauthkeys(authkeysfile);

	if (mode == capture)
	    pump(authkeysfile, "startauthkeys\n", "endauthkeys\n", stdout);
    }
}

void intercept_exit(int sig)
{
    if (mode == capture)
	printf("finish %d\n", sig);

    exit(0);
}

/* end */
