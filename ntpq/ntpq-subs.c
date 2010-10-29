/*
 * ntpq-subs.c - subroutines which are called to perform ntpq commands.
 */
#include <config.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>

#include "ntpq.h"
#include "ntpq-opts.h"

extern char *	chosts[];
extern char	currenthost[];
extern int	numhosts;
int 		maxhostlen;

/*
 * Declarations for command handlers in here
 */
static associd_t checkassocid	(u_int32);
static	struct varlist *findlistvar (struct varlist *, char *);
static	void	doaddvlist	(struct varlist *, const char *);
static	void	dormvlist	(struct varlist *, const char *);
static	void	doclearvlist	(struct varlist *);
static	void	makequerydata	(struct varlist *, int *, char *);
static	int	doquerylist	(struct varlist *, int, associd_t, int, 
				 u_short *, int *, const char **);
static	void	doprintvlist	(struct varlist *, FILE *);
static	void	addvars 	(struct parse *, FILE *);
static	void	rmvars		(struct parse *, FILE *);
static	void	clearvars	(struct parse *, FILE *);
static	void	showvars	(struct parse *, FILE *);
static	int	dolist		(struct varlist *, associd_t, int, int,
				 FILE *);
static	void	readlist	(struct parse *, FILE *);
static	void	writelist	(struct parse *, FILE *);
static	void	readvar 	(struct parse *, FILE *);
static	void	writevar	(struct parse *, FILE *);
static	void	clocklist	(struct parse *, FILE *);
static	void	clockvar	(struct parse *, FILE *);
static	int	findassidrange	(u_int32, u_int32, int *, int *);
static	void	mreadlist	(struct parse *, FILE *);
static	void	mreadvar	(struct parse *, FILE *);
static	int	dogetassoc	(FILE *);
static	void	printassoc	(int, FILE *);
static	void	associations	(struct parse *, FILE *);
static	void	lassociations	(struct parse *, FILE *);
static	void	passociations	(struct parse *, FILE *);
static	void	lpassociations	(struct parse *, FILE *);

#ifdef	UNUSED
static	void	radiostatus (struct parse *, FILE *);
#endif	/* UNUSED */

static	void	pstatus 	(struct parse *, FILE *);
static	long	when		(l_fp *, l_fp *, l_fp *);
static	char *	prettyinterval	(char *, size_t, long);
static	int	doprintpeers	(struct varlist *, int, int, int, const char *, FILE *, int);
static	int	dogetpeers	(struct varlist *, associd_t, FILE *, int);
static	void	dopeers 	(int, FILE *, int);
static	void	peers		(struct parse *, FILE *);
static	void	lpeers		(struct parse *, FILE *);
static	void	doopeers	(int, FILE *, int);
static	void	opeers		(struct parse *, FILE *);
static	void	lopeers 	(struct parse *, FILE *);
static	void	config		(struct parse *, FILE *);
static	void	saveconfig	(struct parse *, FILE *);
static	void	config_from_file(struct parse *, FILE *);
static	void	mrulist		(struct parse *, FILE *);
static	void	ifstats		(struct parse *, FILE *);
static	void	sysstats	(struct parse *, FILE *);
static	void	monstats	(struct parse *, FILE *);

/*
 * Commands we understand.	Ntpdc imports this.
 */
struct xcmd opcmds[] = {
	{ "saveconfig", saveconfig, { NTP_STR, NO, NO, NO },
		{ "filename", "", "", ""}, 
		"save ntpd configuration to file, . for current config file"},
	{ "associations", associations, {  NO, NO, NO, NO },
	  { "", "", "", "" },
	  "print list of association ID's and statuses for the server's peers" },
	{ "passociations", passociations,   {  NO, NO, NO, NO },
	  { "", "", "", "" },
	  "print list of associations returned by last associations command" },
	{ "lassociations", lassociations,   {  NO, NO, NO, NO },
	  { "", "", "", "" },
	  "print list of associations including all client information" },
	{ "lpassociations", lpassociations, {  NO, NO, NO, NO },
	  { "", "", "", "" },
	  "print last obtained list of associations, including client information" },
	{ "addvars",    addvars,    { NTP_STR, NO, NO, NO },
	  { "name[=value][,...]", "", "", "" },
	  "add variables to the variable list or change their values" },
	{ "rmvars", rmvars,     { NTP_STR, NO, NO, NO },
	  { "name[,...]", "", "", "" },
	  "remove variables from the variable list" },
	{ "clearvars",  clearvars,  { NO, NO, NO, NO },
	  { "", "", "", "" },
	  "remove all variables from the variable list" },
	{ "showvars",   showvars,   { NO, NO, NO, NO },
	  { "", "", "", "" },
	  "print variables on the variable list" },
	{ "readlist",   readlist,   { OPT|NTP_UINT, NO, NO, NO },
	  { "assocID", "", "", "" },
	  "read the system or peer variables included in the variable list" },
	{ "rl",     readlist,   { OPT|NTP_UINT, NO, NO, NO },
	  { "assocID", "", "", "" },
	  "read the system or peer variables included in the variable list" },
	{ "writelist",  writelist,  { OPT|NTP_UINT, NO, NO, NO },
	  { "assocID", "", "", "" },
	  "write the system or peer variables included in the variable list" },
	{ "readvar", readvar,    { OPT|NTP_UINT, OPT|NTP_STR, OPT|NTP_STR, OPT|NTP_STR, },
	  { "assocID", "varname1", "varname2", "varname3" },
	  "read system or peer variables" },
	{ "rv",      readvar,    { OPT|NTP_UINT, OPT|NTP_STR, OPT|NTP_STR, OPT|NTP_STR, },
	  { "assocID", "varname1", "varname2", "varname3" },
	  "read system or peer variables" },
	{ "writevar",   writevar,   { NTP_UINT, NTP_STR, NO, NO },
	  { "assocID", "name=value,[...]", "", "" },
	  "write system or peer variables" },
	{ "mreadlist",  mreadlist,  { NTP_UINT, NTP_UINT, NO, NO },
	  { "assocID", "assocID", "", "" },
	  "read the peer variables in the variable list for multiple peers" },
	{ "mrl",    mreadlist,  { NTP_UINT, NTP_UINT, NO, NO },
	  { "assocID", "assocID", "", "" },
	  "read the peer variables in the variable list for multiple peers" },
	{ "mreadvar",   mreadvar,   { NTP_UINT, NTP_UINT, OPT|NTP_STR, NO },
	  { "assocID", "assocID", "name=value[,...]", "" },
	  "read peer variables from multiple peers" },
	{ "mrv",    mreadvar,   { NTP_UINT, NTP_UINT, OPT|NTP_STR, NO },
	  { "assocID", "assocID", "name=value[,...]", "" },
	  "read peer variables from multiple peers" },
	{ "clocklist",  clocklist,  { OPT|NTP_UINT, NO, NO, NO },
	  { "assocID", "", "", "" },
	  "read the clock variables included in the variable list" },
	{ "cl",     clocklist,  { OPT|NTP_UINT, NO, NO, NO },
	  { "assocID", "", "", "" },
	  "read the clock variables included in the variable list" },
	{ "clockvar",   clockvar,   { OPT|NTP_UINT, OPT|NTP_STR, NO, NO },
	  { "assocID", "name=value[,...]", "", "" },
	  "read clock variables" },
	{ "cv",     clockvar,   { OPT|NTP_UINT, OPT|NTP_STR, NO, NO },
	  { "assocID", "name=value[,...]", "", "" },
	  "read clock variables" },
	{ "pstatus",    pstatus,    { NTP_UINT, NO, NO, NO },
	  { "assocID", "", "", "" },
	  "print status information returned for a peer" },
	{ "peers",  peers,      { OPT|IP_VERSION, NO, NO, NO },
	  { "-4|-6", "", "", "" },
	  "obtain and print a list of the server's peers [IP version]" },
	{ "lpeers", lpeers,     { OPT|IP_VERSION, NO, NO, NO },
	  { "-4|-6", "", "", "" },
	  "obtain and print a list of all peers and clients [IP version]" },
	{ "opeers", opeers,     { OPT|IP_VERSION, NO, NO, NO },
	  { "-4|-6", "", "", "" },
	  "print peer list the old way, with dstadr shown rather than refid [IP version]" },
	{ "lopeers", lopeers,   { OPT|IP_VERSION, NO, NO, NO },
	  { "-4|-6", "", "", "" },
	  "obtain and print a list of all peers and clients showing dstadr [IP version]" },
	{ ":config", config,   { NTP_STR, NO, NO, NO },
	  { "<configuration command line>", "", "", "" },
	  "send a remote configuration command to ntpd" },
	{ "config-from-file", config_from_file, { NTP_STR, NO, NO, NO },
	  { "<configuration filename>", "", "", "" },
	  "configure ntpd using the configuration filename" },
	{ "mrulist", mrulist, { OPT|NTP_STR, OPT|NTP_STR, OPT|NTP_STR, OPT|NTP_STR },
	  { "tag=value", "tag=value", "tag=value", "tag=value" },
	  "display the list of most recently seen source addresses, tags mincount=... resall=0x... resany=0x..." },
	{ "ifstats", ifstats, { NO, NO, NO, NO },
	  { "", "", "", "" },
	  "show statistics for each local address ntpd is using" },
	{ "sysstats", sysstats, { NO, NO, NO, NO },
	  { "", "", "", "" },
	  "display system uptime and packet counts" },
	{ "monstats", monstats, { NO, NO, NO, NO },
	  { "", "", "", "" },
	  "display monitor (mrulist) counters and limits" },
	{ 0,		0,		{ NO, NO, NO, NO },
	  { "-4|-6", "", "", "" }, "" }
};


/*
 * Variable list data space
 */
#define MAXLINE		512	/* maximum length of a line */
#define MAXLIST		64	/* maximum variables in list */
#define LENHOSTNAME	256	/* host name limit */

#define MRU_GOT_COUNT	0x1
#define MRU_GOT_LAST	0x2
#define MRU_GOT_FIRST	0x4
#define MRU_GOT_MV	0x8
#define MRU_GOT_RS	0x10
#define MRU_GOT_ADDR	0x20
#define MRU_GOT_ALL	(MRU_GOT_COUNT | MRU_GOT_LAST | MRU_GOT_FIRST \
			 | MRU_GOT_MV | MRU_GOT_RS | MRU_GOT_ADDR)

/*
 * mrulist() depends on MRUSORT_DEF and MRUSORT_RDEF being the first two
 */
typedef enum mru_sort_order_tag {
	MRUSORT_DEF = 0,	/* lstint ascending */
	MRUSORT_R_DEF,		/* lstint descending */
	MRUSORT_AVGINT,		/* avgint ascending */
	MRUSORT_R_AVGINT,	/* avgint descending */
	MRUSORT_ADDR,		/* IPv4 asc. then IPv6 asc. */
	MRUSORT_R_ADDR,		/* IPv6 desc. then IPv4 desc. */
	MRUSORT_COUNT,		/* hit count ascending */
	MRUSORT_R_COUNT,	/* hit count descending */
	MRUSORT_MAX,		/* special: count of this enum */
} mru_sort_order;

const char * const mru_sort_keywords[MRUSORT_MAX] = {
	"lstint",		/* MRUSORT_DEF */
	"-lstint",		/* MRUSORT_R_DEF */
	"avgint",		/* MRUSORT_AVGINT */
	"-avgint",		/* MRUSORT_R_AVGINT */
	"addr",			/* MRUSORT_ADDR */
	"-addr",		/* MRUSORT_R_ADDR */
	"count",		/* MRUSORT_COUNT */
	"-count",		/* MRUSORT_R_COUNT */
};

typedef int (*qsort_cmp)(const void *, const void *);

/*
 * Old CTL_PST defines for version 2.
 */
#define OLD_CTL_PST_CONFIG		0x80
#define OLD_CTL_PST_AUTHENABLE		0x40
#define OLD_CTL_PST_AUTHENTIC		0x20
#define OLD_CTL_PST_REACH		0x10
#define OLD_CTL_PST_SANE		0x08
#define OLD_CTL_PST_DISP		0x04

#define OLD_CTL_PST_SEL_REJECT		0
#define OLD_CTL_PST_SEL_SELCAND 	1
#define OLD_CTL_PST_SEL_SYNCCAND	2
#define OLD_CTL_PST_SEL_SYSPEER 	3

char flash2[] = " .+*    "; /* flash decode for version 2 */
char flash3[] = " x.-+#*o"; /* flash decode for peer status version 3 */

struct varlist {
	char *name;
	char *value;
} g_varlist[MAXLIST] = { { 0, 0 } };

/*
 * Imported from ntpq.c
 */
extern int showhostnames;
extern int rawmode;
extern struct servent *server_entry;
extern struct association assoc_cache[];
extern int numassoc;
extern u_char pktversion;
extern struct ctl_var peer_var[];

typedef struct mru_tag mru;
struct mru_tag {
	mru *		hlink;	/* next in hash table bucket */
	DECL_DLIST_LINK(mru, mlink);
	int		count;
	l_fp		last;
	l_fp		first;
	u_char		mode;
	u_char		ver;
	u_short		rs;
	sockaddr_u	addr;
};

typedef struct ifstats_row_tag {
	u_int		ifnum;
	sockaddr_u	addr;
	sockaddr_u	bcast;
	int		enabled;
	u_int		flags;
	int		mcast_count;
	char		name[32];
	int		peer_count;
	int		received;
	int		sent;
	int		send_errors;
	u_int		ttl;
	u_int		uptime;
} ifstats_row;

typedef struct var_display_collection_tag {
	const char * const tag;		/* system variable */
	const char * const display;	/* descriptive text */
	u_char type;			/* NTP_STR, etc */
	union {
		char *str;
	} v;				/* retrieved value */
} vdc;

/*
 * other static function prototypes
 */
static mru *	add_mru(mru *);
static int	collect_mru_list(const char *, l_fp *);
static int	fetch_nonce(char *, size_t);
static int	qcmp_mru_avgint(const void *, const void *);
static int	qcmp_mru_r_avgint(const void *, const void *);
static int	qcmp_mru_addr(const void *, const void *);
static int	qcmp_mru_r_addr(const void *, const void *);
static int	qcmp_mru_count(const void *, const void *);
static int	qcmp_mru_r_count(const void *, const void *);
static void	validate_ifnum(u_int, int, ifstats_row *);
static void	another_ifstats_field(int *, ifstats_row *, FILE *);
static void	collect_display_vdc(vdc *vdc_table, FILE *fp);

/*
 * static globals
 */
static u_int	mru_count;
static mru	mru_list;		/* listhead */
static mru **	hash_table;

/*
 * qsort comparison function table for mrulist().  The first two
 * entries are NULL because they are handled without qsort().
 */
const static qsort_cmp mru_qcmp_table[MRUSORT_MAX] = {
	NULL,			/* MRUSORT_DEF unused */
	NULL,			/* MRUSORT_R_DEF unused */
	&qcmp_mru_avgint,	/* MRUSORT_AVGINT */
	&qcmp_mru_r_avgint,	/* MRUSORT_R_AVGINT */
	&qcmp_mru_addr,		/* MRUSORT_ADDR */
	&qcmp_mru_r_addr,	/* MRUSORT_R_ADDR */
	&qcmp_mru_count,	/* MRUSORT_COUNT */
	&qcmp_mru_r_count,	/* MRUSORT_R_COUNT */
};

/*
 * checkassocid - return the association ID, checking to see if it is valid
 */
static associd_t
checkassocid(
	u_int32 value
	)
{
	if (value == 0 || value > ASSOCID_MAX) {
		fprintf(stderr, "***Invalid association ID specified\n");
		return 0;
	}
	return (associd_t)value;
}


/*
 * findlistvar - Look for the named variable in a varlist.  If found,
 *		 return a pointer to it.  Otherwise, if the list has
 *		 slots available, return the pointer to the first free
 *		 slot, or NULL if it's full.
 */
static struct varlist *
findlistvar(
	struct varlist *list,
	char *name
	)
{
	struct varlist *vl;

	for (vl = list; vl < list + MAXLIST && vl->name != NULL; vl++)
		if (!strcmp(name, vl->name))
			return vl;
	if (vl < list + MAXLIST)
		return vl;

	return NULL;
}


/*
 * doaddvlist - add variable(s) to the variable list
 */
static void
doaddvlist(
	struct varlist *vlist,
	const char *vars
	)
{
	register struct varlist *vl;
	int len;
	char *name;
	char *value;

	len = strlen(vars);
	while (nextvar(&len, &vars, &name, &value)) {
		vl = findlistvar(vlist, name);
		if (vl == 0) {
			(void) fprintf(stderr, "Variable list full\n");
			return;
		}

		if (vl->name == NULL) {
			vl->name = estrdup(name);
		} else if (vl->value != NULL) {
			free(vl->value);
			vl->value = NULL;
		}

		if (value != NULL)
			vl->value = estrdup(value);
	}
}


/*
 * dormvlist - remove variable(s) from the variable list
 */
static void
dormvlist(
	struct varlist *vlist,
	const char *vars
	)
{
	register struct varlist *vl;
	int len;
	char *name;
	char *value;

	len = strlen(vars);
	while (nextvar(&len, &vars, &name, &value)) {
		vl = findlistvar(vlist, name);
		if (vl == 0 || vl->name == 0) {
			(void) fprintf(stderr, "Variable `%s' not found\n",
				       name);
		} else {
			free((void *)vl->name);
			if (vl->value != 0)
			    free(vl->value);
			for ( ; (vl+1) < (g_varlist + MAXLIST)
				      && (vl+1)->name != 0; vl++) {
				vl->name = (vl+1)->name;
				vl->value = (vl+1)->value;
			}
			vl->name = vl->value = 0;
		}
	}
}


/*
 * doclearvlist - clear a variable list
 */
static void
doclearvlist(
	struct varlist *vlist
	)
{
	register struct varlist *vl;

	for (vl = vlist; vl < vlist + MAXLIST && vl->name != 0; vl++) {
		free((void *)vl->name);
		vl->name = 0;
		if (vl->value != 0) {
			free(vl->value);
			vl->value = 0;
		}
	}
}


/*
 * makequerydata - form a data buffer to be included with a query
 */
static void
makequerydata(
	struct varlist *vlist,
	int *datalen,
	char *data
	)
{
	register struct varlist *vl;
	register char *cp, *cpend;
	register int namelen, valuelen;
	register int totallen;

	cp = data;
	cpend = data + *datalen;

	for (vl = vlist; vl < vlist + MAXLIST && vl->name != 0; vl++) {
		namelen = strlen(vl->name);
		if (vl->value == 0)
			valuelen = 0;
		else
			valuelen = strlen(vl->value);
		totallen = namelen + valuelen + (valuelen != 0) + (cp != data);
		if (cp + totallen > cpend)
			break;

		if (cp != data)
			*cp++ = ',';
		memcpy(cp, vl->name, (size_t)namelen);
		cp += namelen;
		if (valuelen != 0) {
			*cp++ = '=';
			memcpy(cp, vl->value, (size_t)valuelen);
			cp += valuelen;
		}
	}
	*datalen = cp - data;
}


/*
 * doquerylist - send a message including variables in a list
 */
static int
doquerylist(
	struct varlist *vlist,
	int op,
	associd_t associd,
	int auth,
	u_short *rstatus,
	int *dsize,
	const char **datap
	)
{
	char data[CTL_MAX_DATA_LEN];
	int datalen;

	datalen = sizeof(data);
	makequerydata(vlist, &datalen, data);

	return doquery(op, associd, auth, datalen, data, rstatus, dsize,
		       datap);
}


/*
 * doprintvlist - print the variables on a list
 */
static void
doprintvlist(
	struct varlist *vlist,
	FILE *fp
	)
{
	register struct varlist *vl;

	if (vlist->name == 0) {
		(void) fprintf(fp, "No variables on list\n");
	} else {
		for (vl = vlist; vl < vlist + MAXLIST && vl->name != 0; vl++) {
			if (vl->value == 0) {
				(void) fprintf(fp, "%s\n", vl->name);
			} else {
				(void) fprintf(fp, "%s=%s\n",
						   vl->name, vl->value);
			}
		}
	}
}

/*
 * addvars - add variables to the variable list
 */
/*ARGSUSED*/
static void
addvars(
	struct parse *pcmd,
	FILE *fp
	)
{
	doaddvlist(g_varlist, pcmd->argval[0].string);
}


/*
 * rmvars - remove variables from the variable list
 */
/*ARGSUSED*/
static void
rmvars(
	struct parse *pcmd,
	FILE *fp
	)
{
	dormvlist(g_varlist, pcmd->argval[0].string);
}


/*
 * clearvars - clear the variable list
 */
/*ARGSUSED*/
static void
clearvars(
	struct parse *pcmd,
	FILE *fp
	)
{
	doclearvlist(g_varlist);
}


/*
 * showvars - show variables on the variable list
 */
/*ARGSUSED*/
static void
showvars(
	struct parse *pcmd,
	FILE *fp
	)
{
	doprintvlist(g_varlist, fp);
}


/*
 * dolist - send a request with the given list of variables
 */
static int
dolist(
	struct varlist *vlist,
	associd_t associd,
	int op,
	int type,
	FILE *fp
	)
{
	const char *datap;
	int res;
	int dsize;
	u_short rstatus;
	int quiet;

	/*
	 * if we're asking for specific variables don't include the
	 * status header line in the output.
	 */
	if (old_rv)
		quiet = 0;
	else
		quiet = (vlist->name != NULL);

	res = doquerylist(vlist, op, associd, 0, &rstatus, &dsize, &datap);

	if (res != 0)
		return 0;

	if (numhosts > 1)
		(void) fprintf(fp, "server=%s ", currenthost);
	if (dsize == 0) {
		if (associd == 0)
			(void) fprintf(fp, "No system%s variables returned\n",
				   (type == TYPE_CLOCK) ? " clock" : "");
		else
			(void) fprintf(fp,
				   "No information returned for%s association %u\n",
				   (type == TYPE_CLOCK) ? " clock" : "", associd);
		return 1;
	}

	if (!quiet)
		fprintf(fp,"associd=%d ",associd);
	printvars(dsize, datap, (int)rstatus, type, quiet, fp);
	return 1;
}


/*
 * readlist - send a read variables request with the variables on the list
 */
static void
readlist(
	struct parse *pcmd,
	FILE *fp
	)
{
	associd_t associd;

	if (pcmd->nargs == 0) {
		associd = 0;
	} else {
	  /* HMS: I think we want the u_int32 target here, not the u_long */
		if (pcmd->argval[0].uval == 0)
			associd = 0;
		else if ((associd = checkassocid(pcmd->argval[0].uval)) == 0)
			return;
	}

	(void) dolist(g_varlist, associd, CTL_OP_READVAR,
			  (associd == 0) ? TYPE_SYS : TYPE_PEER, fp);
}


/*
 * writelist - send a write variables request with the variables on the list
 */
static void
writelist(
	struct parse *pcmd,
	FILE *fp
	)
{
	const char *datap;
	int res;
	associd_t associd;
	int dsize;
	u_short rstatus;

	if (pcmd->nargs == 0) {
		associd = 0;
	} else {
		/* HMS: Do we really want uval here? */
		if (pcmd->argval[0].uval == 0)
			associd = 0;
		else if ((associd = checkassocid(pcmd->argval[0].uval)) == 0)
			return;
	}

	res = doquerylist(g_varlist, CTL_OP_WRITEVAR, associd, 1, &rstatus,
			  &dsize, &datap);

	if (res != 0)
		return;

	if (numhosts > 1)
		(void) fprintf(fp, "server=%s ", currenthost);
	if (dsize == 0)
		(void) fprintf(fp, "done! (no data returned)\n");
	else {
		(void) fprintf(fp,"associd=%d ",associd);
		printvars(dsize, datap, (int)rstatus,
			  (associd != 0) ? TYPE_PEER : TYPE_SYS, 0, fp);
	}
	return;
}


/*
 * readvar - send a read variables request with the specified variables
 */
static void
readvar(
	struct parse *pcmd,
	FILE *fp
	)
{
	associd_t associd;
	struct varlist tmplist[MAXLIST];
	int tmpcount;
	int i;

	/* HMS: uval? */
	if (pcmd->nargs == 0 || pcmd->argval[0].uval == 0)
		associd = 0;
	else if ((associd = checkassocid(pcmd->argval[0].uval)) == 0)
		return;

	memset(tmplist, 0, sizeof(tmplist));
	if (pcmd->nargs > 1) {
		tmpcount = pcmd->nargs - 1;
		for (i = 0; i < tmpcount; i++)
			doaddvlist(tmplist, pcmd->argval[1 + i].string);
	}

	dolist(tmplist, associd, CTL_OP_READVAR,
	       (associd == 0) 
		   ? TYPE_SYS
		   : TYPE_PEER,
	       fp);

	doclearvlist(tmplist);
}


/*
 * writevar - send a write variables request with the specified variables
 */
static void
writevar(
	struct parse *pcmd,
	FILE *fp
	)
{
	const char *datap;
	int res;
	associd_t associd;
	int dsize;
	u_short rstatus;
	struct varlist tmplist[MAXLIST];

	/* HMS: uval? */
	if (pcmd->argval[0].uval == 0)
		associd = 0;
	else if ((associd = checkassocid(pcmd->argval[0].uval)) == 0)
		return;

	memset((char *)tmplist, 0, sizeof(tmplist));
	doaddvlist(tmplist, pcmd->argval[1].string);

	res = doquerylist(tmplist, CTL_OP_WRITEVAR, associd, 1, &rstatus,
			  &dsize, &datap);

	doclearvlist(tmplist);

	if (res != 0)
		return;

	if (numhosts > 1)
		(void) fprintf(fp, "server=%s ", currenthost);
	if (dsize == 0)
		(void) fprintf(fp, "done! (no data returned)\n");
	else {
		(void) fprintf(fp,"associd=%d ",associd);
		printvars(dsize, datap, (int)rstatus,
			  (associd != 0)
			      ? TYPE_PEER 
			      : TYPE_SYS, 
			  0, fp);
	}
	return;
}


/*
 * clocklist - send a clock variables request with the variables on the list
 */
static void
clocklist(
	struct parse *pcmd,
	FILE *fp
	)
{
	associd_t associd;

	/* HMS: uval? */
	if (pcmd->nargs == 0) {
		associd = 0;
	} else {
		if (pcmd->argval[0].uval == 0)
			associd = 0;
		else if ((associd = checkassocid(pcmd->argval[0].uval)) == 0)
			return;
	}

	dolist(g_varlist, associd, CTL_OP_READCLOCK, TYPE_CLOCK, fp);
}


/*
 * clockvar - send a clock variables request with the specified variables
 */
static void
clockvar(
	struct parse *pcmd,
	FILE *fp
	)
{
	associd_t associd;
	struct varlist tmplist[MAXLIST];

	/* HMS: uval? */
	if (pcmd->nargs == 0 || pcmd->argval[0].uval == 0)
		associd = 0;
	else if ((associd = checkassocid(pcmd->argval[0].uval)) == 0)
		return;

	memset((char *)tmplist, 0, sizeof(tmplist));
	if (pcmd->nargs >= 2)
		doaddvlist(tmplist, pcmd->argval[1].string);

	dolist(tmplist, associd, CTL_OP_READCLOCK, TYPE_CLOCK, fp);

	doclearvlist(tmplist);
}


/*
 * findassidrange - verify a range of association ID's
 */
static int
findassidrange(
	u_int32 assid1,
	u_int32 assid2,
	int *from,
	int *to
	)
{
	register int i;
	int f, t;

	if (assid1 == 0 || assid1 > 65535) {
		(void) fprintf(stderr,
				   "***Invalid association ID %lu specified\n", (u_long)assid1);
		return 0;
	}

	if (assid2 == 0 || assid2 > 65535) {
	fprintf(stderr,
	    "***Invalid association ID %lu specified\n", (u_long)assid2);
		return 0;
	}

	f = t = -1;
	for (i = 0; i < numassoc; i++) {
		if (assoc_cache[i].assid == assid1) {
			f = i;
			if (t != -1)
				break;
		}
		if (assoc_cache[i].assid == assid2) {
			t = i;
			if (f != -1)
				break;
		}
	}

	if (f == -1 || t == -1) {
		(void) fprintf(stderr,
				   "***Association ID %lu not found in list\n",
				   (f == -1) ? (u_long)assid1 : (u_long)assid2);
		return 0;
	}

	if (f < t) {
		*from = f;
		*to = t;
	} else {
		*from = t;
		*to = f;
	}
	return 1;
}



/*
 * mreadlist - send a read variables request for multiple associations
 */
static void
mreadlist(
	struct parse *pcmd,
	FILE *fp
	)
{
	int i;
	int from;
	int to;

	/* HMS: uval? */
	if (!findassidrange(pcmd->argval[0].uval, pcmd->argval[1].uval,
				&from, &to))
		return;

	for (i = from; i <= to; i++) {
		if (i != from)
			(void) fprintf(fp, "\n");
		if (!dolist(g_varlist, (int)assoc_cache[i].assid,
				CTL_OP_READVAR, TYPE_PEER, fp))
			return;
	}
	return;
}


/*
 * mreadvar - send a read variables request for multiple associations
 */
static void
mreadvar(
	struct parse *pcmd,
	FILE *fp
	)
{
	int i;
	int from;
	int to;
	struct varlist tmplist[MAXLIST];

	/* HMS: uval? */
	if (!findassidrange(pcmd->argval[0].uval, pcmd->argval[1].uval,
				&from, &to))
		return;

	memset((char *)tmplist, 0, sizeof(tmplist));
	if (pcmd->nargs >= 3)
		doaddvlist(tmplist, pcmd->argval[2].string);

	for (i = from; i <= to; i++) {
		if (i != from)
			(void) fprintf(fp, "\n");
		if (!dolist(g_varlist, (int)assoc_cache[i].assid,
				CTL_OP_READVAR, TYPE_PEER, fp))
			break;
	}
	doclearvlist(tmplist);
	return;
}


/*
 * dogetassoc - query the host for its list of associations
 */
static int
dogetassoc(
	FILE *fp
	)
{
	const char *datap;
	const u_short *pus;
	int res;
	int dsize;
	u_short rstatus;

	res = doquery(CTL_OP_READSTAT, 0, 0, 0, (char *)0, &rstatus,
			  &dsize, &datap);

	if (res != 0)
		return 0;

	if (dsize == 0) {
		if (numhosts > 1)
			(void) fprintf(fp, "server=%s ", currenthost);
		(void) fprintf(fp, "No association ID's returned\n");
		return 0;
	}

	if (dsize & 0x3) {
		if (numhosts > 1)
			fprintf(stderr, "server=%s ", currenthost);
		fprintf(stderr,
			"***Server returned %d octets, should be multiple of 4\n",
			dsize);
		return 0;
	}

	numassoc = 0;
	while (dsize > 0) {
		pus = (const void *)datap;
		assoc_cache[numassoc].assid = ntohs(*pus);
		datap += sizeof(u_short);
		pus = (const void *)datap;
		assoc_cache[numassoc].status = ntohs(*pus);
		datap += sizeof(u_short);
		if (debug)
			fprintf(stderr, "[%u] ",
				assoc_cache[numassoc].assid);
		if (++numassoc >= MAXASSOC)
			break;
		dsize -= sizeof(u_short) + sizeof(u_short);
	}
	if (debug)
		fprintf(stderr, "\n%d associations total\n", numassoc);
	sortassoc();
	return 1;
}


/*
 * printassoc - print the current list of associations
 */
static void
printassoc(
	int showall,
	FILE *fp
	)
{
	register char *bp;
	int i;
	u_char statval;
	int event;
	u_long event_count;
	const char *conf;
	const char *reach;
	const char *auth;
	const char *condition = "";
	const char *last_event;
	const char *cnt;
	char buf[128];

	if (numassoc == 0) {
		(void) fprintf(fp, "No association ID's in list\n");
		return;
	}

	/*
	 * Output a header
	 */
	(void) fprintf(fp,
			   "\nind assid status  conf reach auth condition  last_event cnt\n");
	(void) fprintf(fp,
			   "===========================================================\n");
	for (i = 0; i < numassoc; i++) {
		statval = (u_char) CTL_PEER_STATVAL(assoc_cache[i].status);
		if (!showall && !(statval & (CTL_PST_CONFIG|CTL_PST_REACH)))
			continue;
		event = CTL_PEER_EVENT(assoc_cache[i].status);
		event_count = CTL_PEER_NEVNT(assoc_cache[i].status);
		if (statval & CTL_PST_CONFIG)
			conf = "yes";
		else
			conf = "no";
		if (statval & CTL_PST_BCAST) {
			reach = "none";
			if (statval & CTL_PST_AUTHENABLE)
				auth = "yes";
			else
				auth = "none";
		} else {
			if (statval & CTL_PST_REACH)
				reach = "yes";
			else
				reach = "no";
			if (statval & CTL_PST_AUTHENABLE) {
				if (statval & CTL_PST_AUTHENTIC)
					auth = "ok ";
				else
					auth = "bad";
			} else {
				auth = "none";
			}
		}
		if (pktversion > NTP_OLDVERSION) {
			switch (statval & 0x7) {

			case CTL_PST_SEL_REJECT:
				condition = "reject";
				break;

			case CTL_PST_SEL_SANE:
				condition = "falsetick";
				break;

			case CTL_PST_SEL_CORRECT:
				condition = "excess";
				break;

			case CTL_PST_SEL_SELCAND:
				condition = "outlyer";
				break;

			case CTL_PST_SEL_SYNCCAND:
				condition = "candidate";
				break;

			case CTL_PST_SEL_EXCESS:
				condition = "backup";
				break;

			case CTL_PST_SEL_SYSPEER:
				condition = "sys.peer";
				break;

			case CTL_PST_SEL_PPS:
				condition = "pps.peer";
				break;
			}
		} else {
			switch (statval & 0x3) {

			case OLD_CTL_PST_SEL_REJECT:
				if (!(statval & OLD_CTL_PST_SANE))
					condition = "insane";
				else if (!(statval & OLD_CTL_PST_DISP))
					condition = "hi_disp";
				else
					condition = "";
				break;

			case OLD_CTL_PST_SEL_SELCAND:
				condition = "sel_cand";
				break;

			case OLD_CTL_PST_SEL_SYNCCAND:
				condition = "sync_cand";
				break;

			case OLD_CTL_PST_SEL_SYSPEER:
				condition = "sys_peer";
				break;
			}
		}
		switch (PEER_EVENT|event) {

		case PEVNT_MOBIL:
			last_event = "mobilize";
			break;

		case PEVNT_DEMOBIL:
			last_event = "demobilize";
			break;

		case PEVNT_REACH:
			last_event = "reachable";
			break;

		case PEVNT_UNREACH:
			last_event = "unreachable";
			break;

		case PEVNT_RESTART:
			last_event = "restart";
			break;

		case PEVNT_REPLY:
			last_event = "no_reply";
			break;

		case PEVNT_RATE:
			last_event = "rate_exceeded";
			break;

		case PEVNT_DENY:
			last_event = "access_denied";
			break;

		case PEVNT_ARMED:
			last_event = "leap_armed";
			break;

		case PEVNT_NEWPEER:
			last_event = "sys_peer";
			break;

		case PEVNT_CLOCK:
			last_event = "clock_alarm";
			break;

		default:
			last_event = "";
			break;
		}
		cnt = uinttoa(event_count);
		snprintf(buf, sizeof(buf),
			 "%3d %5u  %04x   %3.3s  %4s  %4.4s %9.9s %11s %2s",
			 i + 1, assoc_cache[i].assid,
			 assoc_cache[i].status, conf, reach, auth,
			 condition, last_event, cnt);
		bp = buf + strlen(buf);
		while (bp > buf && ' ' == bp[-1])
			--bp;
		bp[0] = '\0';
		fprintf(fp, "%s\n", buf);
	}
}


/*
 * associations - get, record and print a list of associations
 */
/*ARGSUSED*/
static void
associations(
	struct parse *pcmd,
	FILE *fp
	)
{
	if (dogetassoc(fp))
		printassoc(0, fp);
}


/*
 * lassociations - get, record and print a long list of associations
 */
/*ARGSUSED*/
static void
lassociations(
	struct parse *pcmd,
	FILE *fp
	)
{
	if (dogetassoc(fp))
		printassoc(1, fp);
}


/*
 * passociations - print the association list
 */
/*ARGSUSED*/
static void
passociations(
	struct parse *pcmd,
	FILE *fp
	)
{
	printassoc(0, fp);
}


/*
 * lpassociations - print the long association list
 */
/*ARGSUSED*/
static void
lpassociations(
	struct parse *pcmd,
	FILE *fp
	)
{
	printassoc(1, fp);
}


/*
 *  saveconfig - dump ntp server configuration to server file
 */
static void
saveconfig(
	struct parse *pcmd,
	FILE *fp
	)
{
	const char *datap;
	int res;
	int dsize;
	u_short rstatus;

	if (0 == pcmd->nargs)
		return;
	
	res = doquery(CTL_OP_SAVECONFIG, 0, 1,
		      strlen(pcmd->argval[0].string),
		      pcmd->argval[0].string, &rstatus, &dsize,
		      &datap);

	if (res != 0)
		return;

	if (0 == dsize)
		fprintf(fp, "(no response message, curiously)");
	else
		fprintf(fp, "%.*s", dsize, datap);
}


#ifdef	UNUSED
/*
 * radiostatus - print the radio status returned by the server
 */
/*ARGSUSED*/
static void
radiostatus(
	struct parse *pcmd,
	FILE *fp
	)
{
	char *datap;
	int res;
	int dsize;
	u_short rstatus;

	res = doquery(CTL_OP_READCLOCK, 0, 0, 0, (char *)0, &rstatus,
			  &dsize, &datap);

	if (res != 0)
		return;

	if (numhosts > 1)
		(void) fprintf(fp, "server=%s ", currenthost);
	if (dsize == 0) {
		(void) fprintf(fp, "No radio status string returned\n");
		return;
	}

	asciize(dsize, datap, fp);
}
#endif	/* UNUSED */

/*
 * pstatus - print peer status returned by the server
 */
static void
pstatus(
	struct parse *pcmd,
	FILE *fp
	)
{
	const char *datap;
	int res;
	associd_t associd;
	int dsize;
	u_short rstatus;

	/* HMS: uval? */
	if ((associd = checkassocid(pcmd->argval[0].uval)) == 0)
		return;

	res = doquery(CTL_OP_READSTAT, associd, 0, 0, (char *)0, &rstatus,
			  &dsize, &datap);

	if (res != 0)
		return;

	if (numhosts > 1)
		(void) fprintf(fp, "server=%s ", currenthost);
	if (dsize == 0) {
		(void) fprintf(fp,
				   "No information returned for association %u\n",
				   associd);
		return;
	}

	(void) fprintf(fp,"associd=%d ",associd);
	printvars(dsize, datap, (int)rstatus, TYPE_PEER, 0, fp);
}


/*
 * when - print how long its been since his last packet arrived
 */
static long
when(
	l_fp *ts,
	l_fp *rec,
	l_fp *reftime
	)
{
	l_fp *lasttime;

	if (rec->l_ui != 0)
		lasttime = rec;
	else if (reftime->l_ui != 0)
		lasttime = reftime;
	else
		return 0;

	return (ts->l_ui - lasttime->l_ui);
}


/*
 * Pretty-print an interval into the given buffer, in a human-friendly format.
 */
static char *
prettyinterval(
	char *buf,
	size_t cb,
	long diff
	)
{
	if (diff <= 0) {
		buf[0] = '-';
		buf[1] = 0;
		return buf;
	}

	if (diff <= 2048) {
		snprintf(buf, cb, "%ld", diff);
		return buf;
	}

	diff = (diff + 29) / 60;
	if (diff <= 300) {
		snprintf(buf, cb, "%ldm", diff);
		return buf;
	}

	diff = (diff + 29) / 60;
	if (diff <= 96) {
		snprintf(buf, cb, "%ldh", diff);
		return buf;
	}

	diff = (diff + 11) / 24;
	snprintf(buf, cb, "%ldd", diff);
	return buf;
}

static char
decodeaddrtype(
	sockaddr_u *sock
	)
{
	char ch = '-';
	u_int32 dummy;

	switch(AF(sock)) {
	case AF_INET:
		dummy = SRCADR(sock);
		ch = (char)(((dummy&0xf0000000)==0xe0000000) ? 'm' :
			((dummy&0x000000ff)==0x000000ff) ? 'b' :
			((dummy&0xffffffff)==0x7f000001) ? 'l' :
			((dummy&0xffffffe0)==0x00000000) ? '-' :
			'u');
		break;
	case AF_INET6:
		if (IN6_IS_ADDR_MULTICAST(PSOCK_ADDR6(sock)))
			ch = 'm';
		else
			ch = 'u';
		break;
	default:
		ch = '-';
		break;
	}
	return ch;
}

/*
 * A list of variables required by the peers command
 */
struct varlist opeervarlist[] = {
	{ "srcadr",	0 },	/* 0 */
	{ "dstadr",	0 },	/* 1 */
	{ "stratum",	0 },	/* 2 */
	{ "hpoll",	0 },	/* 3 */
	{ "ppoll",	0 },	/* 4 */
	{ "reach",	0 },	/* 5 */
	{ "delay",	0 },	/* 6 */
	{ "offset",	0 },	/* 7 */
	{ "jitter",	0 },	/* 8 */
	{ "dispersion", 0 },	/* 9 */
	{ "rec",	0 },	/* 10 */
	{ "reftime",	0 },	/* 11 */
	{ "srcport",	0 },	/* 12 */
	{ "hmode",	0 },	/* 13 */
	{ 0,		0 }
};

struct varlist peervarlist[] = {
	{ "srcadr",	0 },	/* 0 */
	{ "refid",	0 },	/* 1 */
	{ "stratum",	0 },	/* 2 */
	{ "hpoll",	0 },	/* 3 */
	{ "ppoll",	0 },	/* 4 */
	{ "reach",	0 },	/* 5 */
	{ "delay",	0 },	/* 6 */
	{ "offset",	0 },	/* 7 */
	{ "jitter",	0 },	/* 8 */
	{ "dispersion", 0 },	/* 9 */
	{ "rec",	0 },	/* 10 */
	{ "reftime",	0 },	/* 11 */
	{ "srcport",	0 },	/* 12 */
	{ "hmode",	0 },	/* 13 */
	{ "srchost",	0 },	/* 14 */
	{ 0,		0 }
};

#define HAVE_SRCADR	0
#define HAVE_DSTADR	1
#define HAVE_REFID	1
#define HAVE_STRATUM	2
#define HAVE_HPOLL	3
#define HAVE_PPOLL	4
#define HAVE_REACH	5
#define HAVE_DELAY	6
#define HAVE_OFFSET	7
#define HAVE_JITTER	8
#define HAVE_DISPERSION 9
#define HAVE_REC	10
#define HAVE_REFTIME	11
#define HAVE_SRCPORT	12
#define HAVE_HMODE	13
#define HAVE_SRCHOST	14
#define MAX_HAVE 	14

/*
 * Decode an incoming data buffer and print a line in the peer list
 */
static int
doprintpeers(
	struct varlist *pvl,
	int associd,
	int rstatus,
	int datalen,
	const char *data,
	FILE *fp,
	int af
	)
{
	char *name;
	char *value = NULL;
	int i;
	int c;
	int len;
	sockaddr_u srcadr;
	sockaddr_u dstadr;
	sockaddr_u dum_store;
	long hmode = 0;
	u_long srcport = 0;
	char *dstadr_refid = "0.0.0.0";
	size_t drlen;
	u_long stratum = 0;
	long ppoll = 0;
	long hpoll = 0;
	u_long reach = 0;
	l_fp estoffset;
	l_fp estdelay;
	l_fp estjitter;
	l_fp estdisp;
	l_fp reftime;
	l_fp rec;
	l_fp ts;
	u_char havevar[MAX_HAVE + 1];
	u_long poll_sec;
	char type = '?';
	char refid_string[10];
	char whenbuf[8], pollbuf[8];
	char clock_name[LENHOSTNAME];

	get_systime(&ts);
	
	memset(havevar, 0, sizeof(havevar));
	ZERO_SOCK(&srcadr);
	ZERO_SOCK(&dstadr);
	clock_name[0] = '\0';

	/* Initialize by zeroing out estimate variables */
	memset(&estoffset, 0, sizeof(estoffset));
	memset(&estdelay, 0, sizeof(estdelay));
	memset(&estjitter, 0, sizeof(estjitter));
	memset(&estdisp, 0, sizeof(estdisp));

	while (nextvar(&datalen, &data, &name, &value)) {
		i = findvar(name, peer_var, 1);
		if (i == 0)
			continue;	/* don't know this one */
		switch (i) {
		case CP_SRCADR:
			if (decodenetnum(value, &srcadr))
				havevar[HAVE_SRCADR] = 1;
			break;
		case CP_SRCHOST:
			if (pvl == peervarlist) {
				len = strlen(value);
				if (2 < len &&
				    (size_t)len < sizeof(clock_name)) {
					/* strip quotes */
					value++;
					len -= 2;
					memcpy(clock_name, value, len);
					clock_name[len] = '\0';
					havevar[HAVE_SRCHOST] = 1;
				}
			}
			break;
		case CP_DSTADR:
			if (decodenetnum(value, &dum_store)) {
				type = decodeaddrtype(&dum_store);
				if (pvl == opeervarlist) {
					havevar[HAVE_DSTADR] = 1;
					dstadr = dum_store;
					dstadr_refid = stoa(&dstadr);
				}
			}
			break;
		case CP_HMODE:
			if (decodeint(value, &hmode))
				havevar[HAVE_HMODE] = 1;
			break;
		case CP_REFID:
			if (pvl == peervarlist) {
				havevar[HAVE_REFID] = 1;
				if (*value == '\0') {
					dstadr_refid = "";
				} else if (strlen(value) <= 4) {
					refid_string[0] = '.';
					strncpy(&refid_string[1], value, sizeof(refid_string) - 1);
					i = strlen(refid_string);
					refid_string[i] = '.';
					refid_string[i+1] = '\0';
					dstadr_refid = refid_string;
				} else if (decodenetnum(value, &dstadr)) {
					if (ISREFCLOCKADR(&dstadr))
						dstadr_refid =
						    refnumtoa(&dstadr);
					else
						dstadr_refid =
						    stoa(&dstadr);
				} else {
					havevar[HAVE_REFID] = 0;
				}
			}
			break;
		case CP_STRATUM:
			if (decodeuint(value, &stratum))
				havevar[HAVE_STRATUM] = 1;
			break;
		case CP_HPOLL:
			if (decodeint(value, &hpoll)) {
				havevar[HAVE_HPOLL] = 1;
				if (hpoll < 0)
					hpoll = NTP_MINPOLL;
			}
			break;
		case CP_PPOLL:
			if (decodeint(value, &ppoll)) {
				havevar[HAVE_PPOLL] = 1;
				if (ppoll < 0)
					ppoll = NTP_MINPOLL;
			}
			break;
		case CP_REACH:
			if (decodeuint(value, &reach))
				havevar[HAVE_REACH] = 1;
			break;
		case CP_DELAY:
			if (decodetime(value, &estdelay))
				havevar[HAVE_DELAY] = 1;
			break;
		case CP_OFFSET:
			if (decodetime(value, &estoffset))
				havevar[HAVE_OFFSET] = 1;
			break;
		case CP_JITTER:
			if (pvl == peervarlist)
				if (decodetime(value, &estjitter))
					havevar[HAVE_JITTER] = 1;
			break;
		case CP_DISPERSION:
			if (decodetime(value, &estdisp))
				havevar[HAVE_DISPERSION] = 1;
			break;
		case CP_REC:
			if (decodets(value, &rec))
				havevar[HAVE_REC] = 1;
			break;
		case CP_SRCPORT:
			if (decodeuint(value, &srcport))
				havevar[HAVE_SRCPORT] = 1;
			break;
		case CP_REFTIME:
			havevar[HAVE_REFTIME] = 1;
			if (!decodets(value, &reftime))
				L_CLR(&reftime);
			break;
		default:
			break;
		}
	}

	/*
	 * hmode gives the best guidance for the t column.  If the response
	 * did not include hmode we'll use the old decodeaddrtype() result.
	 */
	switch (hmode) {

	case MODE_BCLIENT:
		/* broadcastclient or multicastclient */
		type = 'b';
		break;

	case MODE_BROADCAST:
		/* broadcast or multicast server */
		if (IS_MCAST(&srcadr))
			type = 'M';
		else
			type = 'B';
		break;

	case MODE_CLIENT:
		if (ISREFCLOCKADR(&srcadr))
			type = 'l';	/* local refclock*/
		else if (SOCK_UNSPEC(&srcadr))
			type = 'p';	/* pool */
		else if (IS_MCAST(&srcadr))
			type = 'a';	/* manycastclient */
		else
			type = 'u';	/* unicast */
		break;

	case MODE_ACTIVE:
		type = 's';		/* symmetric */
		break;
	}

	/*
	 * Got everything, format the line
	 */
	poll_sec = 1 << min(ppoll, hpoll);
	if (pktversion > NTP_OLDVERSION)
		c = flash3[CTL_PEER_STATVAL(rstatus) & 0x7];
	else
		c = flash2[CTL_PEER_STATVAL(rstatus) & 0x3];
	if (numhosts > 1)
		fprintf(fp, "%-*s ", maxhostlen, currenthost);
	if (AF_UNSPEC == af || AF(&srcadr) == af) {
		if (!havevar[HAVE_SRCHOST])
			strncpy(clock_name, nntohost(&srcadr),
				sizeof(clock_name));
		fprintf(fp, "%c%-15.15s ", c, clock_name);
		drlen = strlen(dstadr_refid);
		makeascii(drlen, dstadr_refid, fp);
		while (drlen++ < 15)
			fputc(' ', fp);
		fprintf(fp,
			" %2ld %c %4.4s %4.4s  %3lo  %7.7s %8.7s %7.7s\n",
			stratum, type,
			prettyinterval(whenbuf, sizeof(whenbuf),
				       when(&ts, &rec, &reftime)),
			prettyinterval(pollbuf, sizeof(pollbuf), 
				       (int)poll_sec),
			reach, lfptoms(&estdelay, 3),
			lfptoms(&estoffset, 3),
			(havevar[HAVE_JITTER])
			    ? lfptoms(&estjitter, 3)
			    : lfptoms(&estdisp, 3));
		return (1);
	}
	else
		return(1);
}

#undef	HAVE_SRCADR
#undef	HAVE_DSTADR
#undef	HAVE_STRATUM
#undef	HAVE_PPOLL
#undef	HAVE_HPOLL
#undef	HAVE_REACH
#undef	HAVE_ESTDELAY
#undef	HAVE_ESTOFFSET
#undef	HAVE_JITTER
#undef	HAVE_ESTDISP
#undef	HAVE_REFID
#undef	HAVE_REC
#undef	HAVE_SRCPORT
#undef	HAVE_REFTIME
#undef	MAX_HAVE


/*
 * dogetpeers - given an association ID, read and print the spreadsheet
 *		peer variables.
 */
static int
dogetpeers(
	struct varlist *pvl,
	associd_t associd,
	FILE *fp,
	int af
	)
{
	const char *datap;
	int res;
	int dsize;
	u_short rstatus;

#ifdef notdef
	res = doquerylist(pvl, CTL_OP_READVAR, associd, 0, &rstatus,
			  &dsize, &datap);
#else
	/*
	 * Damn fuzzballs
	 */
	res = doquery(CTL_OP_READVAR, associd, 0, 0, (char *)0, &rstatus,
			  &dsize, &datap);
#endif

	if (res != 0)
		return 0;

	if (dsize == 0) {
		if (numhosts > 1)
			(void) fprintf(stderr, "server=%s ", currenthost);
		(void) fprintf(stderr,
				   "***No information returned for association %d\n",
				   associd);
		return 0;
	}

	return doprintpeers(pvl, associd, (int)rstatus, dsize, datap, fp, af);
}


/*
 * peers - print a peer spreadsheet
 */
static void
dopeers(
	int showall,
	FILE *fp,
	int af
	)
{
	register int i;
	char fullname[LENHOSTNAME];
	sockaddr_u netnum;

	if (!dogetassoc(fp))
		return;

	for (i = 0; i < numhosts; ++i) {
		if (getnetnum(chosts[i], &netnum, fullname, af))
			if ((int)strlen(fullname) > maxhostlen)
				maxhostlen = strlen(fullname);
	}
	if (numhosts > 1)
		fprintf(fp, "%-*.*s ", maxhostlen, maxhostlen, "server");
	fprintf(fp,
		"     remote           refid      st t when poll reach   delay   offset  jitter\n");
	if (numhosts > 1)
		for (i = 0; i <= maxhostlen; ++i)
			fprintf(fp, "=");
	fprintf(fp,
		"==============================================================================\n");

	for (i = 0; i < numassoc; i++) {
		if (!showall &&
		    !(CTL_PEER_STATVAL(assoc_cache[i].status)
		      & (CTL_PST_CONFIG|CTL_PST_REACH))) {
			if (debug)
				fprintf(stderr, "eliding [%d]\n", assoc_cache[i].assid);
			continue;
		}
		if (!dogetpeers(peervarlist, (int)assoc_cache[i].assid, fp, af)) {
			return;
		}
	}
	return;
}


/*
 * peers - print a peer spreadsheet
 */
/*ARGSUSED*/
static void
peers(
	struct parse *pcmd,
	FILE *fp
	)
{
	int af = 0;

	if (pcmd->nargs == 1) {
		if (pcmd->argval->ival == 6)
			af = AF_INET6;
		else
			af = AF_INET;
	}
	dopeers(0, fp, af);
}


/*
 * lpeers - print a peer spreadsheet including all fuzzball peers
 */
/*ARGSUSED*/
static void
lpeers(
	struct parse *pcmd,
	FILE *fp
	)
{
	int af = 0;

	if (pcmd->nargs == 1) {
		if (pcmd->argval->ival == 6)
			af = AF_INET6;
		else
			af = AF_INET;
	}
	dopeers(1, fp, af);
}


/*
 * opeers - print a peer spreadsheet
 */
static void
doopeers(
	int showall,
	FILE *fp,
	int af
	)
{
	register int i;
	char fullname[LENHOSTNAME];
	sockaddr_u netnum;

	if (!dogetassoc(fp))
		return;

	for (i = 0; i < numhosts; ++i) {
		if (getnetnum(chosts[i], &netnum, fullname, af))
			if ((int)strlen(fullname) > maxhostlen)
				maxhostlen = strlen(fullname);
	}
	if (numhosts > 1)
		(void) fprintf(fp, "%-*.*s ", maxhostlen, maxhostlen, "server");
	(void) fprintf(fp,
			   "     remote           local      st t when poll reach   delay   offset    disp\n");
	if (numhosts > 1)
		for (i = 0; i <= maxhostlen; ++i)
		(void) fprintf(fp, "=");
	(void) fprintf(fp,
			   "==============================================================================\n");

	for (i = 0; i < numassoc; i++) {
		if (!showall &&
			!(CTL_PEER_STATVAL(assoc_cache[i].status)
			  & (CTL_PST_CONFIG|CTL_PST_REACH)))
			continue;
		if (!dogetpeers(opeervarlist, (int)assoc_cache[i].assid, fp, af)) {
			return;
		}
	}
	return;
}


/*
 * opeers - print a peer spreadsheet the old way
 */
/*ARGSUSED*/
static void
opeers(
	struct parse *pcmd,
	FILE *fp
	)
{
	int af = 0;

	if (pcmd->nargs == 1) {
		if (pcmd->argval->ival == 6)
			af = AF_INET6;
		else
			af = AF_INET;
	}
	doopeers(0, fp, af);
}


/*
 * lopeers - print a peer spreadsheet including all fuzzball peers
 */
/*ARGSUSED*/
static void
lopeers(
	struct parse *pcmd,
	FILE *fp
	)
{
	int af = 0;

	if (pcmd->nargs == 1) {
		if (pcmd->argval->ival == 6)
			af = AF_INET6;
		else
			af = AF_INET;
	}
	doopeers(1, fp, af);
}


/* 
 * config - send a configuration command to a remote host
 */
static void 
config (
	struct parse *pcmd,
	FILE *fp
	)
{
	char *cfgcmd;
	u_short rstatus;
	int rsize;
	const char *rdata;
	char *resp;
	int res;
	int col;
	int i;

	cfgcmd = pcmd->argval[0].string;

	if (debug > 2)
		fprintf(stderr, 
			"In Config\n"
			"Keyword = %s\n"
			"Command = %s\n", pcmd->keyword, cfgcmd);

	res = doquery(CTL_OP_CONFIGURE, 0, 1, strlen(cfgcmd), cfgcmd,
		      &rstatus, &rsize, &rdata);

	if (res != 0)
		return;

	if (rsize > 0 && '\n' == rdata[rsize - 1])
		rsize--;

	resp = emalloc(rsize + 1);
	memcpy(resp, rdata, rsize);
	resp[rsize] = '\0';

	col = -1;
	if (1 == sscanf(resp, "column %d syntax error", &col)
	    && col >= 0 && (size_t)col <= strlen(cfgcmd) + 1) {
		if (interactive) {
			printf("______");	/* "ntpq> " */
			printf("________");	/* ":config " */
		} else
			printf("%s\n", cfgcmd);
		for (i = 1; i < col; i++)
			putchar('_');
		printf("^\n");
	}
	printf("%s\n", resp);
	free(resp);
}


/* 
 * config_from_file - remotely configure an ntpd daemon using the
 * specified configuration file
 * SK: This function is a kludge at best and is full of bad design
 * bugs:
 * 1. ntpq uses UDP, which means that there is no guarantee of in-order,
 *    error-free delivery. 
 * 2. The maximum length of a packet is constrained, and as a result, the
 *    maximum length of a line in a configuration file is constrained. 
 *    Longer lines will lead to unpredictable results.
 * 3. Since this function is sending a line at a time, we can't update
 *    the control key through the configuration file (YUCK!!)
 */
static void 
config_from_file (
	struct parse *pcmd,
	FILE *fp
	)
{
	u_short rstatus;
	int rsize;
	const char *rdata;
	int res;
	FILE *config_fd;
	char config_cmd[MAXLINE];
	size_t config_len;
	int i;
	int retry_limit;

	if (debug > 2)
		fprintf(stderr,
			"In Config\n"
			"Keyword = %s\n"
			"Filename = %s\n", pcmd->keyword,
			pcmd->argval[0].string);

	config_fd = fopen(pcmd->argval[0].string, "r");
	if (NULL == config_fd) {
		printf("ERROR!! Couldn't open file: %s\n",
		       pcmd->argval[0].string);
		return;
	}

	printf("Sending configuration file, one line at a time.\n");
	i = 0;
	while (fgets(config_cmd, MAXLINE, config_fd) != NULL) {
		config_len = strlen(config_cmd);
		/* ensure even the last line has newline, if possible */
		if (config_len > 0 && 
		    config_len + 2 < sizeof(config_cmd) &&
		    '\n' != config_cmd[config_len - 1])
			config_cmd[config_len++] = '\n';
		++i;
		retry_limit = 2;
		do 
			res = doquery(CTL_OP_CONFIGURE, 0, 1,
				      strlen(config_cmd), config_cmd,
				      &rstatus, &rsize, &rdata);
		while (res != 0 && retry_limit--);
		if (res != 0) {
			printf("Line No: %d query failed: %s", i,
			       config_cmd);
			printf("Subsequent lines not sent.\n");
			fclose(config_fd);
			return;
		}

		if (rsize > 0 && '\n' == rdata[rsize - 1])
			rsize--;
		if (rsize > 0 && '\r' == rdata[rsize - 1])
			rsize--;
		printf("Line No: %d %.*s: %s", i, rsize, rdata,
		       config_cmd);
	}
	printf("Done sending file\n");
	fclose(config_fd);
}


static int
fetch_nonce(
	char *	nonce,
	size_t	cb_nonce
	)
{
	const char	nonce_eq[] = "nonce=";
	int		qres;
	u_short		rstatus;
	int		rsize;
	const char *	rdata;
	int		chars;

	/*
	 * Retrieve a nonce specific to this client to demonstrate to
	 * ntpd that we're capable of receiving responses to our source
	 * IP address, and thereby unlikely to be forging the source.
	 */
	qres = doquery(CTL_OP_REQ_NONCE, 0, 0, 0, NULL, &rstatus,
		       &rsize, &rdata);
	if (qres) {
		fprintf(stderr, "nonce request failed\n");
		return FALSE;
	}

	if (rsize <= sizeof(nonce_eq) - 1 ||
	    strncmp(rdata, nonce_eq, sizeof(nonce_eq) - 1)) {
		fprintf(stderr, "unexpected nonce response format: %.*s\n",
			rsize, rdata);
		return FALSE;
	}
	chars = rsize - (sizeof(nonce_eq) - 1);
	if (chars >= (int)cb_nonce)
		return FALSE;
	memcpy(nonce, rdata + sizeof(nonce_eq) - 1, chars);
	nonce[chars] = '\0';
	while (chars > 0 &&
	       ('\r' == nonce[chars - 1] || '\n' == nonce[chars - 1])) {
		chars--;
		nonce[chars] = '\0';
	}
	
	return TRUE;
}


/*
 * add_mru	Add and entry to mru list, hash table, and allocate
 *		and return a replacement.
 *		This is a helper for collect_mru_list().
 */
static mru *
add_mru(
	mru *add
	)
{
	u_short hash;
	mru *mon;
	mru *unlinked;


	hash = NTP_HASH_ADDR(&add->addr);
	/* see if we have it among previously received entries */
	for (mon = hash_table[hash]; mon != NULL; mon = mon->hlink)
		if (SOCK_EQ(&mon->addr, &add->addr))
			break;
	if (mon != NULL) {
		if (!L_ISGEQ(&add->first, &mon->first)) {
			fprintf(stderr,
				"add_mru duplicate %s new first ts %08x.%08x precedes prior %08x.%08x\n",
				sptoa(&add->addr), add->last.l_ui,
				add->last.l_uf, mon->last.l_ui,
				mon->last.l_uf);
			exit(1);

		}
		UNLINK_DLIST(mon, mlink);
		UNLINK_SLIST(unlinked, hash_table[hash], mon, hlink, mru);
		NTP_INSIST(unlinked == mon);
		if (debug)
			fprintf(stderr, "(updated from %08x.%08x) ",
				mon->last.l_ui,	mon->last.l_uf);
	}
	LINK_DLIST(mru_list, add, mlink);
	LINK_SLIST(hash_table[hash], add, hlink);
	if (debug)
		fprintf(stderr,
			"add_mru %08x.%08x c %d m %d v %d rest %x first %08x.%08x %s\n",
			add->last.l_ui, add->last.l_uf, add->count,
			(int)add->mode, (int)add->ver, (u_int)add->rs,
			add->first.l_ui, add->first.l_uf,
			sptoa(&add->addr));
	/* if we didn't update an existing entry, alloc replacement */
	if (NULL == mon) {
		mon = emalloc(sizeof(*mon));
		mru_count++;
	}
	memset(mon, 0, sizeof(*mon));

	return mon;
}


/* MGOT macro is specific to collect_mru_list() */
#define MGOT(bit)				\
	do {					\
		got |= (bit);			\
		if (MRU_GOT_ALL == got) {	\
			got = 0;		\
			mon = add_mru(mon);	\
			ci++;			\
		}				\
	} while (0)


static int
collect_mru_list(
	const char *parms,
	l_fp *	pnow
	)
{
	const u_int sleep_msecs = 30;
	static int ntpd_row_limit = MRU_ROW_LIMIT;
	int c_mru_l_rc;		/* this function's return code */
	u_char got;		/* MRU_GOT_* bits */
	size_t cb;
	mru *mon;
	mru *head;
	mru *recent;
	int list_complete;
	char nonce[128];
	char buf[128];
	char req_buf[CTL_MAX_DATA_LEN];
	char *req;
	char *req_end;
	int chars;
	int qres;
	u_short rstatus;
	int rsize;
	const char *rdata;
	int limit;
	char *tag;
	char *val;
	int si;		/* server index in response */
	int ci;		/* client (our) index for validation */
	int ri;		/* request index (.# suffix) */
	int mv;
	l_fp newest;
	l_fp last_older;
	sockaddr_u addr_older;
	int have_now;
	int have_addr_older; 
	int have_last_older;
	u_int restarted_count;
	u_int nonce_uses;
	u_short hash;
	mru *unlinked;

	if (!fetch_nonce(nonce, sizeof(nonce)))
		return FALSE;

	nonce_uses = 0;
	restarted_count = 0;
	mru_count = 0;
	INIT_DLIST(mru_list, mlink);
	cb = NTP_HASH_SIZE * sizeof(*hash_table);
	NTP_INSIST(NULL == hash_table);
	hash_table = emalloc(cb);
	memset(hash_table, 0, cb);

	c_mru_l_rc = FALSE;
	list_complete = FALSE;
	have_now = FALSE;
	got = 0;
	ri = 0;
	cb = sizeof(*mon);
	mon = emalloc(cb);
	memset(mon, 0, cb);
	memset(pnow, 0, sizeof(*pnow));
	memset(&last_older, 0, sizeof(last_older));

	limit = min(3 * MAXFRAGS, ntpd_row_limit);
	snprintf(req_buf, sizeof(req_buf), "nonce=%s, limit=%d%s",
		 nonce, limit, parms);
	nonce_uses++;

	while (TRUE) {
		if (debug)
			fprintf(stderr, "READ_MRU parms: %s\n", req_buf);

		qres = doqueryex(CTL_OP_READ_MRU, 0, 0, strlen(req_buf),
			         req_buf, &rstatus, &rsize, &rdata, TRUE);

		if (CERR_UNKNOWNVAR == qres && ri > 0) {
			/*
			 * None of the supplied prior entries match, so
			 * toss them from our list and try again.
			 */
			if (debug)
				fprintf(stderr,
					"no overlap between %d prior entries and server MRU list\n",
					ri);
			while (ri--) {
				recent = HEAD_DLIST(mru_list, mlink);
				NTP_INSIST(recent != NULL);
				if (debug)
					fprintf(stderr,
						"tossing prior entry %s to resync\n",
						sptoa(&recent->addr));
				UNLINK_DLIST(recent, mlink);
				hash = NTP_HASH_ADDR(&recent->addr);
				UNLINK_SLIST(unlinked, hash_table[hash],
					     recent, hlink, mru);
				NTP_INSIST(unlinked == recent);
				free(recent);
				mru_count--;
			}
			if (NULL == HEAD_DLIST(mru_list, mlink)) {
				restarted_count++;
				if (restarted_count > 8) {
					fprintf(stderr,
						"Giving up after 8 restarts from the beginning.\n"
						"With high-traffic NTP servers, this can occur if the\n"
						"MRU list is limited to less than about 16 seconds' of\n"
						"entries.  See the 'mru' ntp.conf directive to adjust.\n");
					goto cleanup_return;
				}
				if (debug)
					fprintf(stderr,
						"--->   Restarting from the beginning, retry #%u\n", 
						restarted_count);
			}
		} else if (CERR_UNKNOWNVAR == qres) {
			fprintf(stderr,
				"CERR_UNKNOWNVAR from ntpd but no priors given.\n");
			goto cleanup_return;
		} else if (CERR_BADVALUE == qres) {
			/* ntpd has lower cap on row limit */
			ntpd_row_limit--;
			limit = min(limit, ntpd_row_limit);
			if (debug)
				fprintf(stderr,
					"Row limit reduced to %d following CERR_BADVALUE.\n",
				        limit);
		} else if (ERR_INCOMPLETE == qres) {
			/*
			 * Reduce the number of rows to minimize effect
			 * of single lost packets.
			 */
			limit = max(2, limit / 2);
			if (debug)
				fprintf(stderr,
					"Row limit reduced to %d following incomplete response.\n",
					limit);
		} else if (qres) {
			show_error_msg(qres, 0);
			goto cleanup_return;
		}
		/*
		 * This is a cheap cop-out implementation of rawmode
		 * output for mrulist.  A better approach would be to
		 * dump similar output after the list is collected by
		 * ntpq with a continuous sequence of indexes.  This
		 * cheap approach has indexes resetting to zero for
		 * each query/response, and duplicates are not 
		 * coalesced.
		 */
		if (!qres && rawmode)
			printvars(rsize, rdata, rstatus, TYPE_SYS, 1, stdout);
		ci = 0;
		have_addr_older = FALSE;
		have_last_older = FALSE;
		while (!qres && nextvar(&rsize, &rdata, &tag, &val)) {
			if (debug > 1)
				fprintf(stderr, "nextvar gave: %s = %s\n",
					tag, val);
			switch(tag[0]) {

			case 'a':
				if (!strcmp(tag, "addr.older")) {
					if (!have_last_older) {
						fprintf(stderr,
							"addr.older %s before last.older\n",
							val);
						goto cleanup_return;
					}
					if (!decodenetnum(val, &addr_older)) {
						fprintf(stderr,
							"addr.older %s garbled\n",
							val);
						goto cleanup_return;
					}
					hash = NTP_HASH_ADDR(&addr_older);
					for (recent = hash_table[hash];
					     recent != NULL;
					     recent = recent->hlink)
						if (ADDR_PORT_EQ(
						      &addr_older,
						      &recent->addr))
							break;
					if (NULL == recent) {
						fprintf(stderr,
							"addr.older %s not in hash table\n",
							val);
						goto cleanup_return;
					}
					if (!L_ISEQU(&last_older,
						     &recent->last)) {
						fprintf(stderr,
							"last.older %08x.%08x mismatches %08x.%08x expected.\n",
							last_older.l_ui,
							last_older.l_uf,
							recent->last.l_ui,
							recent->last.l_uf);
						goto cleanup_return;
					}
					have_addr_older = TRUE;
				} else if (1 != sscanf(tag, "addr.%d", &si)
					   || si != ci)
					goto nomatch;
				else if (decodenetnum(val, &mon->addr))
					MGOT(MRU_GOT_ADDR);
				break;

			case 'l':
				if (!strcmp(tag, "last.older")) {
					if ('0' != val[0] ||
					    'x' != val[1] ||
					    !hextolfp(val + 2, &last_older)) {
						fprintf(stderr,
							"last.older %s garbled\n",
							val);
						goto cleanup_return;
					}
					have_last_older = TRUE;
				} else if (!strcmp(tag, "last.newest")) {
					if (0 != got) {
						fprintf(stderr,
							"last.newest %s before complete row, got = 0x%x\n",
							val, (u_int)got);
						goto cleanup_return;
					}
					if (!have_now) {
						fprintf(stderr,
							"last.newest %s before now=\n",
							val);
						goto cleanup_return;
					}
					head = HEAD_DLIST(mru_list, mlink);
					if (NULL != head) {
						if ('0' != val[0] ||
						    'x' != val[1] ||
						    !hextolfp(val + 2, &newest) ||
						    !L_ISEQU(&newest,
							     &head->last)) {
							fprintf(stderr,
								"last.newest %s mismatches %08x.%08x",
								val,
								head->last.l_ui,
								head->last.l_uf);
							goto cleanup_return;
						}
					}
					list_complete = TRUE;
				} else if (1 != sscanf(tag, "last.%d", &si) ||
					   si != ci || '0' != val[0] ||
					   'x' != val[1] ||
					   !hextolfp(val + 2, &mon->last))
					goto nomatch;
				else
					MGOT(MRU_GOT_LAST);
				break;

			case 'f':
				if (1 != sscanf(tag, "first.%d", &si) ||
				    si != ci || '0' != val[0] ||
				    'x' != val[1] ||
				    !hextolfp(val + 2, &mon->first))
					goto nomatch;
				MGOT(MRU_GOT_FIRST);
				break;

			case 'n':
				if (!strcmp(tag, "nonce")) {
					strncpy(nonce, val, sizeof(nonce));
					nonce_uses = 0;
					break; /* case */
				} else if (strcmp(tag, "now") ||
					   '0' != val[0] ||
					   'x' != val[1] ||
					    !hextolfp(val + 2, pnow))
					goto nomatch;
				have_now = TRUE;
				break;

			case 'c':
				if (1 != sscanf(tag, "ct.%d", &si) ||
				    si != ci ||
				    1 != sscanf(val, "%d", &mon->count)
				    || mon->count < 1)
					goto nomatch;
				MGOT(MRU_GOT_COUNT);
				break;

			case 'm':
				if (1 != sscanf(tag, "mv.%d", &si) ||
				    si != ci ||
				    1 != sscanf(val, "%d", &mv))
					goto nomatch;
				mon->mode = PKT_MODE(mv);
				mon->ver = PKT_VERSION(mv);
				MGOT(MRU_GOT_MV);
				break;

			case 'r':
				if (1 != sscanf(tag, "rs.%d", &si) ||
				    si != ci ||
				    1 != sscanf(val, "0x%hx", &mon->rs))
					goto nomatch;
				MGOT(MRU_GOT_RS);
				break;

			default:
			nomatch:	
				/* empty stmt */ ;
				/* ignore unknown tags */
			}
		}
		if (have_now)
			list_complete = TRUE;
		if (list_complete) {
			NTP_INSIST(0 == ri || have_addr_older);
			break;
		}
		/*
		 * Snooze for a bit between queries to let ntpd catch
		 * up with other duties.
		 */
#ifdef SYS_WINNT
		Sleep(sleep_msecs);
#elif !defined(HAVE_NANOSLEEP)
		sleep((sleep_msecs / 1000) + 1);
#else
		{
			struct timespec interv = { 0,
						   1000 * sleep_msecs };
			nanosleep(&interv, NULL);
		}
#endif
		/*
		 * If there were no errors, increase the number of rows
		 * to a maximum of 3 * MAXFRAGS (the most packets ntpq
		 * can handle in one response), on the assumption that
		 * no less than 3 rows fit in each packet, capped at 
		 * our best guess at the server's row limit.
		 */
		if (!qres)
			limit = min3(3 * MAXFRAGS, ntpd_row_limit,
				     max(limit + 1, limit * 33 / 32));
		/*
		 * prepare next query with as many address and last-seen
		 * timestamps as will fit in a single packet.
		 */
		req = req_buf;
		req_end = req_buf + sizeof(req_buf);
#define REQ_ROOM	(req_end - req)
		snprintf(req, REQ_ROOM, "nonce=%s, limit=%d%s", nonce,
			 limit, parms);
		req += strlen(req);
		nonce_uses++;
		if (nonce_uses >= 4) {
			if (!fetch_nonce(nonce, sizeof(nonce)))
				goto cleanup_return;
			nonce_uses = 0;
		}


		for (ri = 0, recent = HEAD_DLIST(mru_list, mlink);
		     recent != NULL;
		     ri++, recent = NEXT_DLIST(mru_list, recent, mlink)) {

			snprintf(buf, sizeof(buf),
				 ", addr.%d=%s, last.%d=0x%08x.%08x",
				 ri, sptoa(&recent->addr), ri,
				 recent->last.l_ui, recent->last.l_uf);
			chars = strlen(buf);
			if (REQ_ROOM - chars < 1)
				break;
			memcpy(req, buf, chars + 1);
			req += chars;
		}
	}

	c_mru_l_rc = TRUE;
	goto retain_hash_table;

cleanup_return:
	free(hash_table);
	hash_table = NULL;

retain_hash_table:
	if (mon != NULL)
		free(mon);

	return c_mru_l_rc;
}


/*
 * qcmp_mru_addr - sort MRU entries by remote address.
 *
 * All IPv4 addresses sort before any IPv6, addresses are sorted by
 * value within address family.
 */
static int
qcmp_mru_addr(
	const void *v1,
	const void *v2
	)
{
	const mru * const *	ppm1 = v1;
	const mru * const *	ppm2 = v2;
	const mru *		pm1;
	const mru *		pm2;
	u_short			af1;
	u_short			af2;
	size_t			cmplen;
	size_t			addr_off;

	pm1 = *ppm1;
	pm2 = *ppm2;

	af1 = AF(&pm1->addr);
	af2 = AF(&pm2->addr);

	if (af1 != af2)
		return (AF_INET == af1)
			   ? -1
			   : 1;

	cmplen = SIZEOF_INADDR(af1);
	addr_off = (AF_INET == af1)
		      ? offsetof(struct sockaddr_in, sin_addr)
		      : offsetof(struct sockaddr_in6, sin6_addr);

	return memcmp((const char *)&pm1->addr + addr_off,
		      (const char *)&pm2->addr + addr_off,
		      cmplen);
}


static int
qcmp_mru_r_addr(
	const void *v1,
	const void *v2
	)
{
	return -qcmp_mru_addr(v1, v2);
}


/*
 * qcmp_mru_count - sort MRU entries by times seen (hit count).
 */
static int
qcmp_mru_count(
	const void *v1,
	const void *v2
	)
{
	const mru * const *	ppm1 = v1;
	const mru * const *	ppm2 = v2;
	const mru *		pm1;
	const mru *		pm2;

	pm1 = *ppm1;
	pm2 = *ppm2;
	
	return (pm1->count < pm2->count)
		   ? -1
		   : ((pm1->count == pm2->count)
			  ? 0
			  : 1);
}


static int
qcmp_mru_r_count(
	const void *v1,
	const void *v2
	)
{
	return -qcmp_mru_count(v1, v2);
}


/*
 * qcmp_mru_avgint - sort MRU entries by average interval.
 */
static int
qcmp_mru_avgint(
	const void *v1,
	const void *v2
	)
{
	const mru * const *	ppm1 = v1;
	const mru * const *	ppm2 = v2;
	const mru *		pm1;
	const mru *		pm2;
	l_fp			interval;
	double			avg1;
	double			avg2;

	pm1 = *ppm1;
	pm2 = *ppm2;

	interval = pm1->last;
	L_SUB(&interval, &pm1->first);
	LFPTOD(&interval, avg1);
	avg1 /= pm1->count;

	interval = pm2->last;
	L_SUB(&interval, &pm2->first);
	LFPTOD(&interval, avg2);
	avg2 /= pm2->count;

	if (avg1 < avg2)
		return -1;
	else if (avg1 > avg2)
		return 1;

	/* secondary sort on lstint - rarely tested */
	if (L_ISEQU(&pm1->last, &pm2->last))
		return 0;
	else if (L_ISGEQ(&pm1->last, &pm2->last))
		return -1;
	else
		return 1;
}


static int
qcmp_mru_r_avgint(
	const void *v1,
	const void *v2
	)
{
	return -qcmp_mru_avgint(v1, v2);
}


/*
 * mrulist - ntpq's mrulist command to fetch an arbitrarily large Most
 *	     Recently Used (seen) remote address list from ntpd.
 *
 * Similar to ntpdc's monlist command, but not limited to a single
 * request/response, and thereby not limited to a few hundred remote
 * addresses.
 *
 * See ntpd/ntp_control.c read_mru_list() for comments on the way
 * CTL_OP_READ_MRU is designed to be used.
 *
 * mrulist intentionally differs from monlist in the way the avgint
 * column is calculated.  monlist includes the time after the last
 * packet from the client until the monlist query time in the average,
 * while mrulist excludes it.  That is, monlist's average interval grows
 * over time for remote addresses not heard from in some time, while it
 * remains unchanged in mrulist.  This also affects the avgint value for
 * entries representing a single packet, with identical first and last
 * timestamps.  mrulist shows 0 avgint, monlist shows a value identical
 * to lstint.
 */
static void 
mrulist(
	struct parse *	pcmd,
	FILE *		fp
	)
{
	const char mincount_eq[] =	"mincount=";
	const char resall_eq[] =	"resall=";
	const char resany_eq[] =	"resany=";
	const char maxlstint_eq[] =	"maxlstint=";
	const char laddr_eq[] =		"laddr=";
	const char sort_eq[] =		"sort=";
	mru_sort_order order;
	const char * const *ppkeyword;
	char parms_buf[128];
	char buf[24];
	char *parms;
	char *arg;
	size_t cb;
	mru **sorted;
	mru **ppentry;
	mru *recent;
	l_fp now;
	l_fp interval;
	double favgint;
	double flstint;
	int avgint;
	int lstint;
	int i;

	order = MRUSORT_DEF;
	parms_buf[0] = '\0';
	parms = parms_buf;
	for (i = 0; i < pcmd->nargs; i++) {
		arg = pcmd->argval[i].string;
		if (arg != NULL) {
			cb = strlen(arg) + 1;
			if ((!strncmp(resall_eq, arg, sizeof(resall_eq)
			    - 1) || !strncmp(resany_eq, arg,
			    sizeof(resany_eq) - 1) || !strncmp(
			    mincount_eq, arg, sizeof(mincount_eq) - 1) 
			    || !strncmp(laddr_eq, arg, sizeof(laddr_eq)
			    - 1) || !strncmp(maxlstint_eq, arg,
			    sizeof(laddr_eq) - 1)) && parms + cb + 2 <=
			    parms_buf + sizeof(parms_buf)) {
				/* these are passed intact to ntpd */
				memcpy(parms, ", ", 2);
				parms += 2;
				memcpy(parms, arg, cb);
				parms += cb - 1;
			} else if (!strncmp(sort_eq, arg,
					    sizeof(sort_eq) - 1)) {
				arg += sizeof(sort_eq) - 1;
				for (ppkeyword = mru_sort_keywords;
				     ppkeyword < mru_sort_keywords +
					 COUNTOF(mru_sort_keywords);
				     ppkeyword++)
					if (!strcmp(*ppkeyword, arg))
						break;
				if (ppkeyword < mru_sort_keywords +
				    COUNTOF(mru_sort_keywords))
					order = ppkeyword -
						mru_sort_keywords;
			} else if (!strcmp("limited", arg) ||
				   !strcmp("kod", arg)) {
				/* transform to resany=... */
				snprintf(buf, sizeof(buf),
					 ", resany=0x%x",
					 ('k' == arg[0])
					     ? RES_KOD
					     : RES_LIMITED);
				cb = 1 + strlen(buf);
				if (parms + cb <
					parms_buf + sizeof(parms_buf)) {
					memcpy(parms, buf, cb);
					parms += cb - 1;
				}
			} else
				fprintf(stderr,
					"ignoring unrecognized mrulist parameter: %s\n",
					arg);
		}
	}
	parms = parms_buf;

	if (!collect_mru_list(parms, &now))
		return;

	/* display the results */
	if (rawmode)
		goto cleanup_return;

	/* construct an array of entry pointers in default order */
	sorted = emalloc(mru_count * sizeof(*sorted));
	ppentry = sorted;
	if (MRUSORT_R_DEF != order) {
		ITER_DLIST_BEGIN(mru_list, recent, mlink, mru)
			NTP_INSIST(ppentry < sorted + mru_count);
			*ppentry = recent;
			ppentry++;
		ITER_DLIST_END()
	} else {
		REV_ITER_DLIST_BEGIN(mru_list, recent, mlink, mru)
			NTP_INSIST(ppentry < sorted + mru_count);
			*ppentry = recent;
			ppentry++;
		REV_ITER_DLIST_END()
	}

	if (ppentry - sorted != (int)mru_count) {
		fprintf(stderr,
			"mru_count %u should match MRU list depth %d.\n",
			mru_count, ppentry - sorted);
		free(sorted);
		goto cleanup_return;
	}

	/* re-sort sorted[] if not default or reverse default */
	if (MRUSORT_R_DEF < order)
		qsort(sorted, mru_count, sizeof(sorted[0]),
		      mru_qcmp_table[order]);

	printf(	"lstint avgint rstr r m v  count rport remote address\n"
		"==============================================================================\n");
		/* '=' x 78 */
	for (ppentry = sorted; ppentry < sorted + mru_count; ppentry++) {
		recent = *ppentry;
		interval = now;
		L_SUB(&interval, &recent->last);
		LFPTOD(&interval, flstint);
		lstint = (int)(flstint + 0.5);
		interval = recent->last;
		L_SUB(&interval, &recent->first);
		LFPTOD(&interval, favgint);
		favgint /= recent->count;
		avgint = (int)(favgint + 0.5);
		fprintf(fp, "%6d %6d %4hx %c %d %d %6d %5hu %s\n",
			lstint, avgint, recent->rs,
			(RES_KOD & recent->rs)
			    ? 'K'
			    : (RES_LIMITED & recent->rs)
				  ? 'L'
				  : '.',
			(int)recent->mode, (int)recent->ver,
			recent->count, SRCPORT(&recent->addr),
			nntohost(&recent->addr));
		if (showhostnames)
			fflush(fp);
	}
	fflush(fp);
	if (debug) {
		fprintf(stderr,
			"--- completed, freeing sorted[] pointers\n");
		fflush(stderr);
	}
	free(sorted);

cleanup_return:
	if (debug) {
		fprintf(stderr, "... freeing MRU entries\n");
		fflush(stderr);
	}
	ITER_DLIST_BEGIN(mru_list, recent, mlink, mru)
		free(recent);
	ITER_DLIST_END()
	if (debug) {
		fprintf(stderr, "... freeing hash_table[]\n");
		fflush(stderr);
	}
	free(hash_table);
	hash_table = NULL;
	INIT_DLIST(mru_list, mlink);
}


/*
 * validate_ifnum - helper for ifstats()
 */
static void
validate_ifnum(
	u_int		ifnum,
	int		fields,
	ifstats_row *	prow
	)
{
	if (0 == fields)
		prow->ifnum = ifnum;
	else if (prow->ifnum != ifnum) {
		fprintf(stderr,
			"received interface index %u, expecting %u, aborting.\n",
			ifnum, prow->ifnum);
		exit(1);
	}
}


/*
 * another_ifstats_field - helper for ifstats()
 */
static void
another_ifstats_field(
	int *		pfields,
	ifstats_row *	prow,
	FILE *		fp
	)
{
	(*pfields)++;
	/* we understand 12 tags */
	if (12 > *pfields)
		return;
	/*
	"    interface name                                        send\n"
	" #  address/broadcast     drop flag ttl mc received sent failed peers   uptime\n"
	"==============================================================================\n");
	 */
	fprintf(fp,
		"%3u %-24.24s %c %4x %3d %2d %6d %6d %6d %5d %8d\n"
		"    %s\n",
		prow->ifnum, prow->name,
		(prow->enabled)
		    ? '.'
		    : 'D',
		prow->flags, prow->ttl, prow->mcast_count,
		prow->received, prow->sent, prow->send_errors,
		prow->peer_count, prow->uptime, sptoa(&prow->addr));
	if (!SOCK_UNSPEC(&prow->bcast))
		fprintf(fp, "    %s\n", sptoa(&prow->bcast));

	*pfields = 0;
	memset(prow, 0, sizeof(*prow));
}


/*
 * ifstats - ntpq -c ifstats modeled on ntpdc -c ifstats.
 */
static void 
ifstats(
	struct parse *	pcmd,
	FILE *		fp
	)
{
	const char	addr_fmt[] =	"addr.%u";
	const char	bcast_fmt[] =	"bcast.%u";
	const char	en_fmt[] =	"en.%u";	/* enabled */
	const char	flags_fmt[] =	"flags.%u";
	const char	mc_fmt[] =	"mc.%u";	/* mcast count */
	const char	name_fmt[] =	"name.%u";
	const char	pc_fmt[] =	"pc.%u";	/* peer count */
	const char	rx_fmt[] =	"rx.%u";
	const char	tl_fmt[] =	"tl.%u";	/* ttl */
	const char	tx_fmt[] =	"tx.%u";
	const char	txerr_fmt[] =	"txerr.%u";
	const char	up_fmt[] =	"up.%u";	/* uptime */
	const char *	datap;
	int		qres;
	int		dsize;
	u_short		rstatus;
	char *		tag;
	char *		val;
	int		fields;
	u_int		ifnum;
	u_int		ui;
	ifstats_row	row;
	int		comprende;
	size_t		len;

	qres = doquery(CTL_OP_READ_IFSTATS, 0, TRUE, 0, NULL, &rstatus,
		       &dsize, &datap);
	if (qres)	/* message already displayed */
		return;

	fprintf(fp,
		"    interface name                                        send\n"
		" #  address/broadcast     drop flag ttl mc received sent failed peers   uptime\n"
		"==============================================================================\n");
		/* '=' x 78 */

	memset(&row, 0, sizeof(row));
	fields = 0;
	ifnum = 0;
	ui = 0;
	while (nextvar(&dsize, &datap, &tag, &val)) {
		if (debug > 1)
			fprintf(stderr, "nextvar gave: %s = %s\n", tag,
				(NULL == val)
				    ? ""
				    : val);
		comprende = FALSE;
		switch(tag[0]) {

		case 'a':
			if (1 == sscanf(tag, addr_fmt, &ui) &&
			    decodenetnum(val, &row.addr))
				comprende = TRUE;
			break;

		case 'b':
			if (1 == sscanf(tag, bcast_fmt, &ui) &&
			    (NULL == val ||
			     decodenetnum(val, &row.bcast)))
				comprende = TRUE;
			break;

		case 'e':
			if (1 == sscanf(tag, en_fmt, &ui) &&
			    1 == sscanf(val, "%d", &row.enabled))
				comprende = TRUE;
			break;

		case 'f':
			if (1 == sscanf(tag, flags_fmt, &ui) &&
			    1 == sscanf(val, "0x%x", &row.flags))
				comprende = TRUE;
			break;

		case 'm':
			if (1 == sscanf(tag, mc_fmt, &ui) &&
			    1 == sscanf(val, "%d", &row.mcast_count))
				comprende = TRUE;
			break;

		case 'n':
			if (1 == sscanf(tag, name_fmt, &ui)) {
				/* strip quotes */
				len = strlen(val);
				if (len >= 2 &&
				    len - 2 < sizeof(row.name)) {
					len -= 2;
					memcpy(row.name, val + 1, len);
					row.name[len] = '\0';
					comprende = TRUE;
				}
			}
			break;

		case 'p':
			if (1 == sscanf(tag, pc_fmt, &ui) &&
			    1 == sscanf(val, "%d", &row.peer_count))
				comprende = TRUE;
			break;

		case 'r':
			if (1 == sscanf(tag, rx_fmt, &ui) &&
			    1 == sscanf(val, "%d", &row.received))
				comprende = TRUE;
			break;

		case 't':
			if (1 == sscanf(tag, tl_fmt, &ui) &&
			    1 == sscanf(val, "%d", &row.ttl))
				comprende = TRUE;
			else if (1 == sscanf(tag, tx_fmt, &ui) &&
				 1 == sscanf(val, "%d", &row.sent))
				comprende = TRUE;
			else if (1 == sscanf(tag, txerr_fmt, &ui) &&
				 1 == sscanf(val, "%d", &row.send_errors))
				comprende = TRUE;
			break;

		case 'u':
			if (1 == sscanf(tag, up_fmt, &ui) &&
			    1 == sscanf(val, "%d", &row.uptime))
				comprende = TRUE;
			break;
		}

		if (comprende) {
			validate_ifnum(ui, fields, &row);
			another_ifstats_field(&fields, &row, fp);
		}
	}
	fflush(fp);
}


/*
 * collect_display_vdc
 */
static void 
collect_display_vdc(
	vdc *vdc_table,
	FILE *fp
	)
{
	struct varlist vl[MAXLIST];
	vdc *pvdc;
	u_short rstatus;
	int rsize;
	const char *rdata;
	int qres;
	char *tag;
	char *val;

	memset(vl, 0, sizeof(vl));
	for (pvdc = vdc_table; pvdc->tag != NULL; pvdc++) {
		memset(&pvdc->v, 0, sizeof(pvdc->v));
		doaddvlist(vl, pvdc->tag);
	}
	qres = doquerylist(vl, CTL_OP_READVAR, 0, 0, &rstatus, &rsize,
			  &rdata);
	doclearvlist(vl);
	if (qres)
		return;		/* error msg already displayed */

	/*
	 * iterate over the response variables filling vdc_table with
	 * the retrieved values.
	 */
	while (nextvar(&rsize, &rdata, &tag, &val)) {
		for (pvdc = vdc_table; pvdc->tag != NULL; pvdc++)
			if (!strcmp(tag, pvdc->tag))
				break;
		if (pvdc->tag != NULL) {
			NTP_INSIST(NTP_STR == pvdc->type);
			pvdc->v.str = estrdup(val);
		}
	}

	/* and display */
	for (pvdc = vdc_table; pvdc->tag != NULL; pvdc++) {
		if (pvdc->v.str != NULL) {
			fprintf(fp, "%s  %s\n", pvdc->display,
				pvdc->v.str);
			free(pvdc->v.str);
		}
	}
}


/*
 * sysstats - implements ntpq -c sysstats modeled on ntpdc -c sysstats
 */
static void
sysstats(
	struct parse *pcmd,
	FILE *fp
	)
{
    static vdc sysstats_vdc[] = {
	{ "ss_uptime",		"uptime:               ", NTP_STR },
	{ "ss_reset",		"sysstats reset:       ", NTP_STR },
	{ "ss_received",	"packets received:     ", NTP_STR },
	{ "ss_thisver",		"current version:      ", NTP_STR },
	{ "ss_oldver",		"older version:        ", NTP_STR },
	{ "ss_badformat",	"bad length or format: ", NTP_STR },
	{ "ss_badauth",		"authentication failed:", NTP_STR },
	{ "ss_declined",	"declined:             ", NTP_STR },
	{ "ss_restricted",	"restricted:           ", NTP_STR },
	{ "ss_limited",		"rate limited:         ", NTP_STR },
	{ "ss_kodsent",		"KoD responses:        ", NTP_STR },
	{ "ss_processed",	"processed for time:   ", NTP_STR },
	{ NULL,			NULL,			  0	  }
    };

	collect_display_vdc(sysstats_vdc, fp);
}

	
/*
 * monstats - implements ntpq -c monstats
 */
static void
monstats(
	struct parse *pcmd,
	FILE *fp
	)
{
    static vdc monstats_vdc[] = {
	{ "mru_enabled",	"enabled:            ", NTP_STR },
	{ "mru_depth",		"addresses:          ", NTP_STR },
	{ "mru_deepest",	"peak addresses:     ", NTP_STR },
	{ "mru_maxdepth",	"maximum addresses:  ", NTP_STR },
	{ "mru_mindepth",	"reclaim above count:", NTP_STR },
	{ "mru_maxage",		"reclaim older than: ", NTP_STR },
	{ "mru_mem",		"kilobytes:          ", NTP_STR },
	{ "mru_maxmem",		"maximum kilobytes:  ", NTP_STR },
	{ NULL,			NULL,			 0	 }
    };

	collect_display_vdc(monstats_vdc, fp);
}
