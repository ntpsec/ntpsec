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
static	void	doaddvlist	(struct varlist *, char *);
static	void	dormvlist	(struct varlist *, char *);
static	void	doclearvlist	(struct varlist *);
static	void	makequerydata	(struct varlist *, int *, char *);
static	int	doquerylist	(struct varlist *, int, associd_t, int, 
				 u_short *, int *, char **);
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
static	int	doprintpeers	(struct varlist *, int, int, int, char *, FILE *, int);
static	int	dogetpeers	(struct varlist *, associd_t, FILE *, int);
static	void	dopeers 	(int, FILE *, int);
static	void	peers		(struct parse *, FILE *);
static	void	lpeers		(struct parse *, FILE *);
static	void	doopeers	(int, FILE *, int);
static	void	opeers		(struct parse *, FILE *);
static	void	lopeers 	(struct parse *, FILE *);
static  void	config		(struct parse *, FILE *);
static 	void 	saveconfig	(struct parse *, FILE *);
static  void	config_from_file(struct parse *, FILE *);
static  void	mrulist		(struct parse *, FILE *);


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
	{ "rv",     readvar,    { OPT|NTP_UINT, OPT|NTP_STR, OPT|NTP_STR, OPT|NTP_STR, },
	  { "assocID", "varname1", "varname2", "varname3" },
	  "read system or peer variables" },
	{ "rv",     readvar,    { OPT|NTP_UINT, OPT|NTP_STR, OPT|NTP_STR, OPT|NTP_STR, },
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
	{ "mrulist", mrulist, { OPT|NTP_STR, NO, NO, NO },
	  { "<IP address>", "", "", "" },
	  "display the list of most recently seen source addresses" },
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
static mru mru_list;		/* listhead */
static mru **hash_table;
static mru *	add_mru	(mru *);


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
 * findlistvar - look for the named variable in a list and return if found
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
	char *vars
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

		if (vl->name == 0) {
			vl->name = estrdup(name);
		} else if (vl->value != 0) {
			free(vl->value);
			vl->value = 0;
		}

		if (value != 0)
			vl->value = estrdup(value);
	}
}


/*
 * dormvlist - remove variable(s) from the variable list
 */
static void
dormvlist(
	struct varlist *vlist,
	char *vars
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
	char **datap
	)
{
	char data[CTL_MAX_DATA_LEN];
	int datalen;

	datalen = sizeof(data);
	makequerydata(vlist, &datalen, data);

	return doquery(op, associd, auth, datalen, data, rstatus,
			   dsize, datap);
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
	char *datap;
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
	char *datap;
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
	char *datap;
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
	char *datap;
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
			(void) fprintf(stderr, "server=%s ", currenthost);
		(void) fprintf(stderr,
				   "***Server returned %d octets, should be multiple of 4\n",
				   dsize);
		return 0;
	}

	numassoc = 0;
	while (dsize > 0) {
		assoc_cache[numassoc].assid = ntohs(*((u_short *)datap));
		datap += sizeof(u_short);
		assoc_cache[numassoc].status = ntohs(*((u_short *)datap));
		datap += sizeof(u_short);
		if (debug)
			fprintf(stderr, "[%u] ", assoc_cache[numassoc].assid);
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
	char *datap;
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
	else {
		datap[dsize] = '\0';
		fprintf(fp, "%s", datap);
	}
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
	char *datap;
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
	char *data,
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
	long hmode = 0;
	u_long srcport = 0;
	char *dstadr_refid = "0.0.0.0";
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
		sockaddr_u dum_store;

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
		if (!havevar[HAVE_SRCHOST] || !SOCK_UNSPEC(&srcadr))
			strncpy(clock_name, nntohost(&srcadr),
				sizeof(clock_name));
		
		fprintf(fp,
			"%c%-15.15s %-15.15s %2ld %c %4.4s %4.4s  %3lo  %7.7s %8.7s %7.7s\n",
			c, clock_name, dstadr_refid, stratum, type,
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
	char *datap;
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
	char *rdata;
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
	rdata[rsize] = '\0';

	col = -1;
	if (1 == sscanf(rdata, "column %d syntax error", &col)
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
	printf("%s\n", rdata);
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
	char *rdata;
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
		rdata[rsize] = '\0';
		printf("Line No: %d %s: %s", i, rdata, config_cmd);
	}
	printf("Done sending file\n");
	fclose(config_fd);
}


/*
 * add_mru	add and entry to mru list, hash table, and allocate
 *		and return a replacement.
 */
static mru *
add_mru(
	mru *add
	)
{
	u_short hash;
	mru *mon;
	mru *unlinked;
	int first_ts_matches;

	hash = NTP_HASH_ADDR(&add->addr);
	/* see if we have it among previously received entries */
	for (mon = hash_table[hash]; mon != NULL; mon = mon->hlink)
		if (SOCK_EQ(&mon->addr, &add->addr))
			break;
	if (mon != NULL) {
		first_ts_matches = L_ISEQU(&add->first, &mon->first);
		NTP_INSIST(first_ts_matches);
		UNLINK_DLIST(mon, mlink);
		UNLINK_SLIST(unlinked, hash_table[hash], mon, hlink, mru);
		NTP_INSIST(unlinked == mon);
		if (debug)
			fprintf(stderr,
				"add_mru duplicate removed %s %08x.%08x -> %08x.%08x\n",
				sptoa(&mon->addr), mon->last.l_ui,
				mon->last.l_uf, add->last.l_ui,
				add->last.l_uf);
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
	if (NULL == mon)
		mon = emalloc(sizeof(*mon));
	memset(mon, 0, sizeof(*mon));
	return mon;
}


static void 
mrulist(
	struct parse *pcmd,
	FILE *fp
	)
{
	u_char got;		/* MRU_GOT_* bits */
#define MGOT(bit)				\
	do {					\
		got |= (bit);			\
		if (MRU_GOT_ALL == got) {	\
			got = 0;		\
			mon = add_mru(mon);	\
			ci++;			\
		}				\
	} while (0)
	const char ts_fmt[] = "0x%08x.%08x";
	size_t cb;
	mru *mon;
	mru *head;
	mru *recent;
	int list_complete;
	char buf[128];
	char req_buf[CTL_MAX_DATA_LEN];
	char *req;
	char *req_end;
	int chars;
	int qres;
	u_short rstatus;
	int rsize;
	char *rdata;
	int limit;
	char *tag;
	char *val;
	int si;		/* server index in response */
	int ci;		/* client (our) index for validation */
	int ri;		/* request index (.# suffix) */
	int mv;
	l_fp now;
	l_fp newest;
	l_fp last_older;
	l_fp interval;
	sockaddr_u addr_older;
	double favgint;
	double flstint;
	int avgint;
	int lstint;
	int have_now;
	int have_addr_older;
	int have_last_older;
	u_short hash;
	mru *unlinked;

	list_complete = FALSE;
	INIT_DLIST(mru_list, mlink);
	cb = NTP_HASH_SIZE * sizeof(*hash_table);
	hash_table = emalloc(cb);
	memset(hash_table, 0, cb);
	cb = sizeof(*mon);
	mon = emalloc(cb);
	memset(mon, 0, cb);
	memset(&now, 0, sizeof(now));
	memset(&last_older, 0, sizeof(last_older));
	have_now = FALSE;
	got = 0;
	ri = 0;

	limit = 2;
	snprintf(req_buf, sizeof(req_buf), "limit=%d", limit);

	while (TRUE) {
		if (debug)
			fprintf(stderr, "READ_MRU: %s\n", req_buf);
		qres = doqueryex(CTL_OP_READ_MRU, 0, 0, strlen(req_buf),
			         req_buf, &rstatus, &rsize, &rdata, TRUE);

		if (CERR_UNKNOWNVAR == qres && ri > 0) {
			/*
			 * None of the supplied prior entries match, so
			 * toss them from our list and try again.
			 */
			if (debug)
				printf("no overlap between %d prior entries and server MRU list\n",
				       ri);
			while (ri--) {
				recent = HEAD_DLIST(mru_list, mlink);
				NTP_INSIST(recent != NULL);
				if (debug)
					printf("tossing prior entry %s to resync\n",
					       sptoa(&recent->addr));
				UNLINK_DLIST(recent, mlink);
				hash = NTP_HASH_ADDR(&recent->addr);
				UNLINK_SLIST(unlinked, hash_table[hash],
					     recent, hlink, mru);
				NTP_INSIST(unlinked == recent);
				free(recent);
			}
		} else if (qres) {
			show_error_msg(qres, 0);
			return;
		}
		if (!qres && rawmode)
			printvars(rsize, rdata, rstatus, TYPE_SYS, 1, stdout);
		ci = 0;
		have_addr_older = FALSE;
		have_last_older = FALSE;
		while (!qres && nextvar(&rsize, &rdata, &tag, &val)) {
			if (debug > 1)
				fprintf(stderr, "nextvar gave: %s = %s\n", tag, val);
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
					if (2 != sscanf(val, ts_fmt,
						        &last_older.l_ui,
							&last_older.l_uf)) {
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
						if (2 != sscanf(val, ts_fmt,
								&newest.l_ui,
								&newest.l_uf) ||
						    !L_ISEQU(&newest, &head->last)) {
							fprintf(stderr,
								"last.newest %s mismatches %08x.%08x",
								val, head->last.l_ui, head->last.l_uf);
							goto cleanup_return;
						}
					}
					list_complete = TRUE;
				} else if (1 != sscanf(tag, "last.%d", &si)
					   || si != ci ||
					   2 != sscanf(val, ts_fmt,
						       &mon->last.l_ui,
						       &mon->last.l_uf))
					goto nomatch;
				else
					MGOT(MRU_GOT_LAST);
				break;

			case 'f':
				if (1 != sscanf(tag, "first.%d", &si) ||
				    si != ci ||
				    2 != sscanf(val, ts_fmt,
					        &mon->first.l_ui,
					        &mon->first.l_uf))
					goto nomatch;
				MGOT(MRU_GOT_FIRST);
				break;

			case 'n':
				if (strcmp(tag, "now") ||
				    2 != sscanf(val, ts_fmt, &now.l_ui,
					        &now.l_uf))
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
		
		if (list_complete) {
			NTP_INSIST(0 == ri || have_addr_older);
			break;
		}
		/*
		 * Snooze for a second between queries to let ntpd catch
		 * up with other duties.
		 */
		sleep(1);
		/*
		 * prepare next query with as many address and last-seen
		 * timestamps as will fit in a single packet.
		 */
		req = req_buf;
		req_end = req_buf + sizeof(req_buf);
#define REQ_ROOM	(req_end - req)
		snprintf(req, REQ_ROOM, "limit=%d", limit);
		req += strlen(req);

		for (ri = 0, recent = HEAD_DLIST(mru_list, mlink);
		     recent != NULL;
		     ri++, recent = NEXT_DLIST(mru_list, recent, mlink)) {

			snprintf(buf, sizeof(buf),
					 ", addr.%d=%s, last.%d=0x%08x.%08x",
					 ri, sptoa(&recent->addr), ri,
					 recent->last.l_ui,
					 recent->last.l_uf);
			chars = strlen(buf);
			if (REQ_ROOM - chars < 1)
				break;
			memcpy(req, buf, chars + 1);
			req += chars;
		}
		NTP_INSIST(ri > 0 || NULL == recent);
	}

	/* display the results */
	if (rawmode)
		goto cleanup_return;


	printf(	"lstint avgint rstr m v  count rport remote address\n"
		"==============================================================================\n");
		/* '-' x 78 */
	ITER_DLIST_BEGIN(mru_list, recent, mlink, mru)
		interval = now;
		L_SUB(&interval, &recent->last);
		LFPTOD(&interval, flstint);
		lstint = (int)(flstint + 0.5);
		interval = recent->last;
		L_SUB(&interval, &recent->first);
		LFPTOD(&interval, favgint);
		favgint /= recent->count;
		avgint = (int)(favgint + 0.5);
		printf("%6d %6d %4hx %d %d %6d %5hu %s\n",
		       lstint, avgint, recent->rs, (int)recent->mode,
		       (int)recent->ver, recent->count,
		       SRCPORT(&recent->addr), nntohost(&recent->addr));
	ITER_DLIST_END()

cleanup_return:
	if (mon != NULL)
		free(mon);
	ITER_DLIST_BEGIN(mru_list, recent, mlink, mru)
		free(recent);
	ITER_DLIST_END()
	free(hash_table);
	hash_table = NULL;
	INIT_DLIST(mru_list, mlink);
}
