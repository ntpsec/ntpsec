/* ntp_config.c
 *
 * This file contains the ntpd configuration code.
 *
 * Written By:	Sachin Kamboj
 *		University of Delaware
 *		Newark, DE 19711
 * Some parts borrowed from the older ntp_config.c
 * Copyright (c) 2006
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-clause
*/

#include "config.h"

#ifdef HAVE_NETINFO_NI_H
# include <netinfo/ni.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>

#include "isc_netaddr.h"

#include "ntp.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_refclock.h"
#include "ntp_filegen.h"
#include "ntp_stdlib.h"
#include "lib_strbuf.h"
#include "ntp_assert.h"
#include "ntp_dns.h"
#include "ntp_auth.h"

/*
 * [Classic Bug 467]: Some linux headers collide with CONFIG_PHONE and
 * CONFIG_KEYS so #include these later.
 */
#include "ntp_config.h"
#include "ntp_scanner.h"
#include "ntp_parser.tab.h"

/* Hack to dance around bug in older Bison.  See Classic Issue 287 */
/* Similar for yydebug below */
#ifndef yyparse
  int yyparse (void);
#endif

/* NetInfo configuration locations */
#ifdef HAVE_NETINFO_NI_H
#define NETINFO_CONFIG_DIR "/config/ntp"
#endif

/*
 * Selection algorithm tuning parameters
 */

/*
 * In the past, we told reference clocks from network peers by giving
 * the reference clocks magic address of a particular special form
 * ntpd itself, the filtering that used to be done based on this magic
 * address prefix is now done using a flag set on incoming packets.
 * In ntpq, the filtering is replaced by asking the server how a
 * peer's name should be displayed.
 *
 * Address filtering is still done in this file so we can tell which old-style
 * config declarations refer to clocks. When that syntax is retired, drop these.
 */
#define	REFCLOCK_ADDR	0x7f7f0000	/* 127.127.0.0 */
#define	REFCLOCK_MASK	0xffff0000	/* 255.255.0.0 */

#define	ISREFCLOCKADR(srcadr)					\
	(IS_IPV4(srcadr) &&					\
	 (SRCADR(srcadr) & REFCLOCK_MASK) == REFCLOCK_ADDR)

/*
 * Macros to determine the clock type and unit numbers from a
 * refclock magic address
 */
#define	REFCLOCKTYPE(srcadr)	((SRCADR(srcadr) >> 8) & 0xff)
#define REFCLOCKUNIT(srcadr)	(SRCADR(srcadr) & 0xff)


/* list of servers from command line for config_peers() */
int	cmdline_server_count = 0;
char **	cmdline_servers;

/*
 * FIXME: ugly globals, only created to avoid wiring in option-parsing cruft.
 * These are symptoms of deeper factoring issues; the things they're
 * controlling, deep down inside that configuration parsing, should
 * not be happening where they are.
 */
bool have_interface_option;

/*
 * "logconfig" building blocks
 */
struct masks {
	const char * const	name;
	const uint32_t		mask;
};

static struct masks logcfg_class[] = {
	{ "clock",	NLOG_OCLOCK },
	{ "peer",	NLOG_OPEER },
	{ "sync",	NLOG_OSYNC },
	{ "sys",	NLOG_OSYS },
	{ NULL,		0 }
};

/* logcfg_noclass_items[] masks are complete and must not be shifted */
static struct masks logcfg_noclass_items[] = {
	{ "allall",		NLOG_SYSMASK | NLOG_PEERMASK | NLOG_CLOCKMASK | NLOG_SYNCMASK },
	{ "allinfo",		NLOG_SYSINFO | NLOG_PEERINFO | NLOG_CLOCKINFO | NLOG_SYNCINFO },
	{ "allevents",		NLOG_SYSEVENT | NLOG_PEEREVENT | NLOG_CLOCKEVENT | NLOG_SYNCEVENT },
	{ "allstatus",		NLOG_SYSSTATUS | NLOG_PEERSTATUS | NLOG_CLOCKSTATUS | NLOG_SYNCSTATUS },
	{ "allstatistics",	NLOG_SYSSTATIST | NLOG_PEERSTATIST | NLOG_CLOCKSTATIST | NLOG_SYNCSTATIST },
	/* the remainder are misspellings of clockall, peerall, sysall, and syncall. */
	{ "allclock",		(NLOG_INFO | NLOG_STATIST | NLOG_EVENT | NLOG_STATUS) << NLOG_OCLOCK },
	{ "allpeer",		(NLOG_INFO | NLOG_STATIST | NLOG_EVENT | NLOG_STATUS) << NLOG_OPEER },
	{ "allsys",		(NLOG_INFO | NLOG_STATIST | NLOG_EVENT | NLOG_STATUS) << NLOG_OSYS },
	{ "allsync",		(NLOG_INFO | NLOG_STATIST | NLOG_EVENT | NLOG_STATUS) << NLOG_OSYNC },
	{ NULL,			0 }
};

/* logcfg_class_items[] masks are shiftable by NLOG_O* counts */
static struct masks logcfg_class_items[] = {
	{ "all",		NLOG_INFO | NLOG_EVENT | NLOG_STATUS | NLOG_STATIST },
	{ "info",		NLOG_INFO },
	{ "events",		NLOG_EVENT },
	{ "status",		NLOG_STATUS },
	{ "statistics",		NLOG_STATIST },
	{ NULL,			0 }
};

/* Limits */
#define MAXPHONE	10	/* maximum number of phone strings */
/* #define MAXPPS	20	* maximum length of PPS device string UNUSED */

/*
 * Miscellaneous macros
 */
#ifdef HAVE_NETINFO_NI_H
# define ISEOL(c)	((c) == '#' || (c) == '\n' || (c) == '\0')
# define ISSPACE(c)	((c) == ' ' || (c) == '\t')
#endif

#define _UC(str)	((char *)(intptr_t)(str))

/*
 * Definitions of things either imported from or exported to outside
 */
config_tree cfgt;			/* Parser output stored here */
static struct config_tree_tag *cfg_tree_history;    /* History of configs */
char	*sys_phone[MAXPHONE] = {NULL};	/* ACTS phone numbers */

static char default_ntp_signd_socket[] =
#ifdef MSSNTP_PATH
					MSSNTP_PATH;
#else
					"";
#endif
char *ntp_signd_socket = default_ntp_signd_socket;
#ifdef HAVE_NETINFO_NI_H
static struct netinfo_config_state *config_netinfo = NULL;
static bool check_netinfo = true;
#endif /* HAVE_NETINFO_NI_H */

#ifdef HAVE_NETINFO_NI_H
/*
 * NetInfo configuration state
 */
struct netinfo_config_state {
	void *domain;		/* domain with config */
	ni_id config_dir;	/* ID config dir      */
	int prop_index;		/* current property   */
	int val_index;		/* current value      */
	char **val_list;	/* value list         */
};
#endif

struct REMOTE_CONFIG_INFO remote_config;  /* Remote configuration buffer and
					     pointer info */
/* FUNCTION PROTOTYPES */

static void init_syntax_tree(config_tree *);
static void apply_enable_disable(attr_val_fifo *q, int enable);

static void free_auth_node(config_tree *);
static void free_all_config_trees(void);

static struct peer *peer_config(sockaddr_u *, const char *,
				 endpt *, int, struct peer_ctl *);

static void free_config_access(config_tree *);
static void free_config_auth(config_tree *);
static void free_config_fudge(config_tree *);
static void free_config_logconfig(config_tree *);
static void free_config_monitor(config_tree *);
static void free_config_nic_rules(config_tree *);
static void free_config_peers(config_tree *);
static void free_config_phone(config_tree *);
static void free_config_reset_counters(config_tree *);
static void free_config_rlimit(config_tree *);
static void free_config_setvar(config_tree *);
static void free_config_system_opts(config_tree *);
static void free_config_tinker(config_tree *);
static void free_config_tos(config_tree *);
static void free_config_unpeers(config_tree *);
static void free_config_vars(config_tree *);

static void free_config_tree(config_tree *ptree);

static void destroy_restrict_node(restrict_node *my_node);
static bool is_sane_resolved_address(sockaddr_u *peeraddr, int hmode);
static void save_and_apply_config_tree(bool from_file);
static void destroy_int_fifo(int_fifo *);
#define FREE_INT_FIFO(pf)			\
	do {					\
		destroy_int_fifo(pf);		\
		(pf) = NULL;			\
	} while (0)
static void destroy_string_fifo(string_fifo *);
#define FREE_STRING_FIFO(pf)			\
	do {					\
		destroy_string_fifo(pf);		\
		(pf) = NULL;			\
	} while (0)
static void destroy_attr_val_fifo(attr_val_fifo *);
#define FREE_ATTR_VAL_FIFO(pf)			\
	do {					\
		destroy_attr_val_fifo(pf);	\
		(pf) = NULL;			\
	} while (0)
static void destroy_filegen_fifo(filegen_fifo *);
#define FREE_FILEGEN_FIFO(pf)			\
	do {					\
		destroy_filegen_fifo(pf);	\
		(pf) = NULL;			\
	} while (0)
static void destroy_restrict_fifo(restrict_fifo *);
#define FREE_RESTRICT_FIFO(pf)			\
	do {					\
		destroy_restrict_fifo(pf);	\
		(pf) = NULL;			\
	} while (0)
static void destroy_setvar_fifo(setvar_fifo *);
#define FREE_SETVAR_FIFO(pf)			\
	do {					\
		destroy_setvar_fifo(pf);	\
		(pf) = NULL;			\
	} while (0)
static void destroy_addr_opts_fifo(addr_opts_fifo *);
#define FREE_ADDR_OPTS_FIFO(pf)			\
	do {					\
		destroy_addr_opts_fifo(pf);	\
		(pf) = NULL;			\
	} while (0)

static void config_logconfig(config_tree *);
static void config_monitor(config_tree *);
static void config_rlimit(config_tree *);
static void config_system_opts(config_tree *);
static void config_tinker(config_tree *);
static void config_tos(config_tree *);
static void config_logfile(config_tree *);
static void config_vars(config_tree *);

static void config_ntpd(config_tree *, bool input_from_file);
static void config_auth(config_tree *);
static void config_access(config_tree *);
static void config_mdnstries(config_tree *);
static void config_phone(config_tree *);
static void config_setvar(config_tree *);
static void config_fudge(config_tree *);
static void config_peers(config_tree *);
static void config_unpeers(config_tree *);
static void config_nic_rules(config_tree *, bool input_from_file);
static void config_reset_counters(config_tree *);

enum gnn_type {
	t_UNK,		/* Unknown */
	t_REF,		/* Refclock */
	t_MSK		/* Network Mask */
};

static uint32_t get_pfxmatch(const char **, struct masks *);
static uint32_t get_match(const char *, struct masks *);
static uint32_t get_logmask(const char *);
static int getnetnum(const char *num, sockaddr_u *addr);
static void fix_node_cidr(restrict_node *my_node);


/* FUNCTIONS FOR INITIALIZATION
 * ----------------------------
 */

static void
free_auth_node(
	config_tree *ptree
	)
{
	if (ptree->auth.keys) {
		free(ptree->auth.keys);
		ptree->auth.keys = NULL;
	}

	if (ptree->auth.ntp_signd_socket) {
		free(ptree->auth.ntp_signd_socket);
		ptree->auth.ntp_signd_socket = NULL;
	}
}


static void
init_syntax_tree(
	config_tree *ptree
	)
{
	ZERO(*ptree);
	ptree->mdnstries = 5;
}


static void
free_all_config_trees(void)
{
	config_tree *ptree;
	config_tree *pnext;

	ptree = cfg_tree_history;

	while (ptree != NULL) {
		pnext = ptree->link;
		free_config_tree(ptree);
		ptree = pnext;
	}
}


static void
free_config_tree(
	config_tree *ptree
	)
{
	if (ptree->source.value.s != NULL)
		free(ptree->source.value.s);

	free_config_auth(ptree);
	free_config_tos(ptree);
	free_config_monitor(ptree);
	free_config_access(ptree);
	free_config_tinker(ptree);
	free_config_rlimit(ptree);
	free_config_system_opts(ptree);
	free_config_logconfig(ptree);
	free_config_phone(ptree);
	free_config_setvar(ptree);
	free_config_fudge(ptree);
	free_config_vars(ptree);
	free_config_peers(ptree);
	free_config_unpeers(ptree);
	free_config_nic_rules(ptree);
	free_config_reset_counters(ptree);
	free_auth_node(ptree);

	free(ptree);
}

/* generic fifo routines for structs linked by 1st member */
void *
append_gen_fifo(
	void *fifo,
	void *entry
	)
{
	gen_fifo *pf;
	gen_node *pe;

	pf = fifo;
	pe = entry;
	if (NULL == pf)
		pf = emalloc_zero(sizeof(*pf));
	else
		CHECK_FIFO_CONSISTENCY(*pf);
	if (pe != NULL)
		LINK_FIFO(*pf, pe, link);
	CHECK_FIFO_CONSISTENCY(*pf);

	return pf;
}


void *
concat_gen_fifos(
	void *first,
	void *second
	)
{
	gen_fifo *pf1;
	gen_fifo *pf2;

	pf1 = first;
	pf2 = second;
	if (NULL == pf1)
		return pf2;
	if (NULL == pf2)
		return pf1;

	CONCAT_FIFO(*pf1, *pf2, link);
	free(pf2);

	return pf1;
}


/* FUNCTIONS FOR CREATING NODES ON THE SYNTAX TREE
 * -----------------------------------------------
 */

attr_val *
create_attr_dval(
	int attr,
	double value
	)
{
	attr_val *my_val;

	my_val = emalloc_zero(sizeof(*my_val));
	my_val->attr = attr;
	my_val->value.d = value;
	my_val->type = T_Double;

	return my_val;
}


attr_val *
create_attr_ival(
	int attr,
	int value
	)
{
	attr_val *my_val;

	my_val = emalloc_zero(sizeof(*my_val));
	my_val->attr = attr;
	my_val->value.i = value;
	my_val->type = T_Integer;

	return my_val;
}


attr_val *
create_attr_uval(
	int		attr,
	unsigned int	value
	)
{
	attr_val *my_val;

	my_val = emalloc_zero(sizeof(*my_val));
	my_val->attr = attr;
	my_val->value.u = value;
	my_val->type = T_U_int;

	return my_val;
}


attr_val *
create_attr_rangeval(
	int	attr,
	int	first,
	int	last
	)
{
	attr_val *my_val;

	my_val = emalloc_zero(sizeof(*my_val));
	my_val->attr = attr;
	my_val->value.r.first = first;
	my_val->value.r.last = last;
	my_val->type = T_Intrange;

	return my_val;
}


attr_val *
create_attr_sval(
	int attr,
	const char *s
	)
{
	attr_val *my_val;

	my_val = emalloc_zero(sizeof(*my_val));
	my_val->attr = attr;
	if (NULL == s)			/* free() hates NULL */
		s = estrdup("");
	my_val->value.s = _UC(s);
	my_val->type = T_String;

	return my_val;
}


int_node *
create_int_node(
	int val
	)
{
	int_node *i_n;

	i_n = emalloc_zero(sizeof(*i_n));
	i_n->i = val;

	return i_n;
}


string_node *
create_string_node(
	char *str
	)
{
	string_node *sn;

	sn = emalloc_zero(sizeof(*sn));
	sn->s = str;

	return sn;
}


address_node *
create_address_node(
	char *	addr,
	int	type
	)
{
	address_node *my_node;

	REQUIRE(NULL != addr);
	REQUIRE(AF_INET == type ||
		    AF_INET6 == type || AF_UNSPEC == type);
	my_node = emalloc_zero(sizeof(*my_node));
	my_node->address = addr;
	my_node->type = (unsigned short)type;

	return my_node;
}


void
destroy_address_node(
	address_node *my_node
	)
{
	if (NULL == my_node)
		return;
	REQUIRE(NULL != my_node->address);

	free(my_node->address);
	free(my_node);
}


peer_node *
create_peer_node(
	int		hmode,
	address_node *	addr,
	attr_val_fifo *	options
	)
{
	peer_node *my_node;
	attr_val *option;
	int errflag = false;

	my_node = emalloc_zero(sizeof(*my_node));

	/* Initialize node values to default */

	my_node->ctl.version = NTP_VERSION;
	my_node->ctl.minpoll = NTP_MINDPOLL;
        /* can not set maxpoll default yet, it may be NTP_MAXDPOLL or minpoll */
	my_node->ctl.maxpoll = NTP_MAXPOLL_UNK;
	my_node->ctl.bias = 0;

	/* Now set the node to the read values */
	my_node->host_mode = hmode;
	my_node->addr = addr;

	/* The options FIFO is consumed and reclaimed here */
	if (options != NULL)
		CHECK_FIFO_CONSISTENCY(*options);
	while (options != NULL) {
		UNLINK_FIFO(option, *options, link);
		if (NULL == option) {
			free(options);
			break;
		}

		/* Check the kind of option being set */
		switch (option->attr) {

		case T_Flag:
			switch (option->value.i) {
			default:
				INSIST(0);
				break;

			case T_Burst:
				my_node->ctl.flags |= FLAG_BURST;
				break;

			case T_Iburst:
				my_node->ctl.flags |= FLAG_IBURST;
				break;

			case T_Noselect:
				my_node->ctl.flags |= FLAG_NOSELECT;
				break;

			case T_Prefer:
				my_node->ctl.flags |= FLAG_PREFER;
				break;

			case T_True:
				my_node->ctl.flags |= FLAG_TRUE;
				break;
			}
			break;

		case T_Minpoll:
			if (option->value.i < NTP_MINPOLL ) {
				msyslog(LOG_INFO,
					"CONFIG: minpoll: provided value (%d) is too small [%d-%d])",
					option->value.i, NTP_MINPOLL,
					NTP_MAXPOLL);
				my_node->ctl.minpoll = NTP_MINPOLL;
			} else if (option->value.i > NTP_MAXPOLL) {
				msyslog(LOG_INFO,
					"CONFIG: minpoll: provided value (%d) is too large [%d-%d])",
					option->value.i, NTP_MINPOLL,
					NTP_MAXPOLL);
				my_node->ctl.minpoll = NTP_MAXPOLL;
			} else {
				my_node->ctl.minpoll =
					(uint8_t)option->value.u;
			}
			break;

		case T_Maxpoll:
			if (option->value.i < NTP_MINPOLL ) {
			    msyslog(LOG_INFO,
				"CONFIG: maxpoll: value (%d) is too small [%d-%d])",
				option->value.i, NTP_MINPOLL,
				NTP_MAXPOLL);
			    my_node->ctl.maxpoll = NTP_MINPOLL;
			} else if ( option->value.i > NTP_MAXPOLL) {
			    msyslog(LOG_INFO,
				"CONFIG: maxpoll: value (%d) is too large [%d-%d])",
				option->value.i, NTP_MINPOLL,
				NTP_MAXPOLL);
			    my_node->ctl.maxpoll = NTP_MAXPOLL;
			} else {
				my_node->ctl.maxpoll =
					(uint8_t)option->value.u;
			}
			break;

		case T_Subtype:
		case T_Mode:
			my_node->ctl.mode = option->value.u;
			break;

		case T_Key:
			if (option->value.u >= NTP_MAXKEY) {
				msyslog(LOG_ERR, "CONFIG: key: invalid argument");
				errflag = true;
			} else {
				my_node->ctl.peerkey =
					(keyid_t)option->value.u;
			}
			break;

		case T_Version:
			if (option->value.u >= UCHAR_MAX) {
				msyslog(LOG_ERR, "CONFIG: version: invalid argument");
				errflag = true;
			} else {
				my_node->ctl.version =
					(uint8_t)option->value.u;
			}
			break;

		case T_Bias:
			my_node->ctl.bias = option->value.d;
			break;

#ifdef REFCLOCK
		case T_Path:
			my_node->ctl.path = estrdup(option->value.s);
			break;

		case T_Ppspath:
			my_node->ctl.ppspath = estrdup(option->value.s);
			break;

		case T_Baud:
			my_node->ctl.baud = option->value.u;
			break;

			/*
			 * Past this point are options the old syntax
			 * handled in fudge processing. They're parsed
			 * here to avoid ordering constraints.
			 */

		case T_Time1:
			my_node->clock_stat.haveflags |= CLK_HAVETIME1;
			my_node->clock_stat.fudgetime1 = option->value.d;
			break;

		case T_Time2:
			my_node->clock_stat.haveflags |= CLK_HAVETIME2;
			my_node->clock_stat.fudgetime2 = option->value.d;
			break;

		case T_Stratum:
			my_node->clock_stat.haveflags |= CLK_HAVEVAL1;
			my_node->clock_stat.fudgeval1 = option->value.i;
			break;

		case T_Refid:
			my_node->clock_stat.haveflags |= CLK_HAVEVAL2;
			my_node->clock_stat.fudgeval2 = 0;
			memcpy(&my_node->clock_stat.fudgeval2,
			       option->value.s,
			       min(strlen(option->value.s), REFIDLEN));
			break;

		case T_Flag1:
			my_node->clock_stat.haveflags |= CLK_HAVEFLAG1;
			if (option->value.i)
				my_node->clock_stat.flags |= CLK_FLAG1;
			else
				my_node->clock_stat.flags &= ~CLK_FLAG1;
			break;

		case T_Flag2:
			my_node->clock_stat.haveflags |= CLK_HAVEFLAG2;
			if (option->value.i)
				my_node->clock_stat.flags |= CLK_FLAG2;
			else
				my_node->clock_stat.flags &= ~CLK_FLAG2;
			break;

		case T_Flag3:
			my_node->clock_stat.haveflags |= CLK_HAVEFLAG3;
			if (option->value.i)
				my_node->clock_stat.flags |= CLK_FLAG3;
			else
				my_node->clock_stat.flags &= ~CLK_FLAG3;
			break;

		case T_Flag4:
			my_node->clock_stat.haveflags |= CLK_HAVEFLAG4;
			if (option->value.i)
				my_node->clock_stat.flags |= CLK_FLAG4;
			else
				my_node->clock_stat.flags &= ~CLK_FLAG4;
			break;

		case T_Holdover:
			my_node->clock_stat.flags |= CLK_HOLDOVER;
			break;
#endif /* REFCLOCK */

		default:
			msyslog(LOG_ERR,
				"CONFIG: Unknown peer/server option token %s",
				token_name(option->attr));
			errflag = true;
		}
		free(option);
	}

	/* Check if errors were reported. If yes, ignore the node */
	if (errflag) {
		free(my_node);
		my_node = NULL;
	}

	return my_node;
}


unpeer_node *
create_unpeer_node(
	address_node *addr
	)
{
	unpeer_node *	my_node;
	unsigned int	u;
	char *		pch;

	my_node = emalloc_zero(sizeof(*my_node));

	/*
	 * From the parser's perspective an association ID fits into
	 * its generic T_String definition of a name/address "address".
	 * We treat all valid 16-bit numbers as association IDs.
	 */
	pch = addr->address;
	while (*pch && isdigit((unsigned char)*pch))
		pch++;

	if (!*pch
	    && 1 == sscanf(addr->address, "%u", &u)
	    && u <= ASSOCID_MAX) {
		my_node->assocID = (associd_t)u;
		destroy_address_node(addr);
		my_node->addr = NULL;
	} else {
		my_node->assocID = 0;
		my_node->addr = addr;
	}

	return my_node;
}

filegen_node *
create_filegen_node(
	int		filegen_token,
	attr_val_fifo *	options
	)
{
	filegen_node *my_node;

	my_node = emalloc_zero(sizeof(*my_node));
	my_node->filegen_token = filegen_token;
	my_node->options = options;

	return my_node;
}


restrict_node *
create_restrict_node(
	const int       mode,
	address_node *	addr,
	address_node *	mask,
	int_fifo *	flags,
	int		line_no
	)
{
	restrict_node *my_node;

	my_node = emalloc_zero(sizeof(*my_node));
	my_node->mode = mode;
	my_node->addr = addr;
	my_node->mask = mask;
	my_node->flags = flags;
	my_node->line_no = line_no;

	return my_node;
}


static void
destroy_restrict_node(
	restrict_node *my_node
	)
{
	/* With great care, free all the memory occupied by
	 * the restrict node
	 */
	destroy_address_node(my_node->addr);
	destroy_address_node(my_node->mask);
	destroy_int_fifo(my_node->flags);
	free(my_node);
}


static void
destroy_int_fifo(
	int_fifo *	fifo
	)
{
	int_node *	i_n;

	if (fifo != NULL) {
		for (;;) {
			UNLINK_FIFO(i_n, *fifo, link);
			if (i_n == NULL)
				break;
			free(i_n);
		}
		free(fifo);
	}
}


static void
destroy_string_fifo(
	string_fifo *	fifo
	)
{
	string_node *	sn;

	if (fifo != NULL) {
		for (;;) {
			UNLINK_FIFO(sn, *fifo, link);
			if (sn == NULL)
				break;
			free(sn->s);
			free(sn);
		}
		free(fifo);
	}
}


static void
destroy_attr_val_fifo(
	attr_val_fifo *	av_fifo
	)
{
	attr_val *	av;

	if (av_fifo != NULL) {
		for (;;) {
			UNLINK_FIFO(av, *av_fifo, link);
			if (av == NULL)
				break;
			if (T_String == av->type)
				free(av->value.s);
			free(av);
		}
		free(av_fifo);
	}
}


static void
destroy_filegen_fifo(
	filegen_fifo *	fifo
	)
{
	filegen_node *	fg;

	if (fifo != NULL) {
		for (;;) {
			UNLINK_FIFO(fg, *fifo, link);
			if (fg == NULL)
				break;
			destroy_attr_val_fifo(fg->options);
			free(fg);
		}
		free(fifo);
	}
}


static void
destroy_restrict_fifo(
	restrict_fifo *	fifo
	)
{
	restrict_node *	rn;

	if (fifo != NULL) {
		for (;;) {
			UNLINK_FIFO(rn, *fifo, link);
			if (rn == NULL)
				break;
			destroy_restrict_node(rn);
		}
		free(fifo);
	}
}


static void
destroy_setvar_fifo(
	setvar_fifo *	fifo
	)
{
	setvar_node *	sv;

	if (fifo != NULL) {
		for (;;) {
			UNLINK_FIFO(sv, *fifo, link);
			if (sv == NULL)
				break;
			free(sv->var);
			free(sv->val);
			free(sv);
		}
		free(fifo);
	}
}


static void
destroy_addr_opts_fifo(
	addr_opts_fifo *	fifo
	)
{
	addr_opts_node *	aon;

	if (fifo != NULL) {
		for (;;) {
			UNLINK_FIFO(aon, *fifo, link);
			if (aon == NULL)
				break;
			destroy_address_node(aon->addr);
			destroy_attr_val_fifo(aon->options);
			free(aon);
		}
		free(fifo);
	}
}


setvar_node *
create_setvar_node(
	char *	var,
	char *	val,
	int	isdefault
	)
{
	setvar_node *	my_node;
	char *		pch;

	/* do not allow = in the variable name */
	pch = strchr(var, '=');
	if (NULL != pch)
		*pch = '\0';

	/* Now store the string into a setvar_node */
	my_node = emalloc_zero(sizeof(*my_node));
	my_node->var = var;
	my_node->val = val;
	my_node->isdefault = isdefault;

	return my_node;
}


nic_rule_node *
create_nic_rule_node(
	int match_class,
	char *if_name,	/* interface name or numeric address */
	int action
	)
{
	nic_rule_node *my_node;

	REQUIRE(match_class != 0 || if_name != NULL);

	my_node = emalloc_zero(sizeof(*my_node));
	my_node->match_class = match_class;
	my_node->if_name = if_name;
	my_node->action = action;

	return my_node;
}


addr_opts_node *
create_addr_opts_node(
	address_node *	addr,
	attr_val_fifo *	options
	)
{
	addr_opts_node *my_node;

	my_node = emalloc_zero(sizeof(*my_node));
	my_node->addr = addr;
	my_node->options = options;

	return my_node;
}

/* FUNCTIONS FOR PERFORMING THE CONFIGURATION
 * ------------------------------------------
 */

static void
config_auth(
	config_tree *ptree
	)
{
	attr_val *	my_val;
	int		first;
	int		last;
	int		i;
	int		count;

	/* ntp_signd_socket Command */
	if (ptree->auth.ntp_signd_socket) {
		if (ntp_signd_socket != default_ntp_signd_socket)
			free(ntp_signd_socket);
		ntp_signd_socket = estrdup(ptree->auth.ntp_signd_socket);
	}

	/*
	 * Count the number of trusted keys to preallocate storage and
	 * size the hash table.
	 */
	count = 0;
	my_val = HEAD_PFIFO(ptree->auth.trusted_key_list);
	for (; my_val != NULL; my_val = my_val->link) {
		if (T_Integer == my_val->type) {
			first = my_val->value.i;
			if (first > 1 && first <= NTP_MAXKEY)
				count++;
		} else {
			REQUIRE(T_Intrange == my_val->type);
			first = my_val->value.r.first;
			last = my_val->value.r.last;
			if (!(first > last || first < 1 ||
			    last > NTP_MAXKEY)) {
				count += 1 + last - first;
			}
		}
	}
	if (0 < count)
		msyslog(LOG_INFO, "Found %d trusted keys.", count);
	auth_prealloc(count);

	/* Keys Command */
	if (ptree->auth.keys)
		getauthkeys(ptree->auth.keys);

	/* Control Key Command */
	if (ptree->auth.control_key)
		ctl_auth_keyid = (keyid_t)ptree->auth.control_key;

	/* Trusted Key Command */
	my_val = HEAD_PFIFO(ptree->auth.trusted_key_list);
	for (; my_val != NULL; my_val = my_val->link) {
		if (T_Integer == my_val->type) {
			first = my_val->value.i;
			if (first >= 1 && first <= NTP_MAXKEY) {
				authtrust((keyid_t)first, true);
			} else {
				msyslog(LOG_NOTICE,
					"CONFIG: Ignoring invalid trustedkey %d, min 1 max %d.",
					first, NTP_MAXKEY);
			}
		} else {
			first = my_val->value.r.first;
			last = my_val->value.r.last;
			if (first > last || first < 1 ||
			    last > NTP_MAXKEY) {
				msyslog(LOG_NOTICE,
					"CONFIG: Ignoring invalid trustedkey range %d ... %d, min 1 max %d.",
					first, last, NTP_MAXKEY);
			} else {
				for (i = first; i <= last; i++) {
					authtrust((keyid_t)i, true);
				}
			}
		}
	}
}


static void
free_config_auth(
	config_tree *ptree
	)
{
	destroy_attr_val_fifo(ptree->auth.trusted_key_list);
	ptree->auth.trusted_key_list = NULL;
}


static void
config_tos(
	config_tree *ptree
	)
{
	attr_val *	tos;
	int		item;
	double		val;

	item = -1;	/* quiet warning */
	tos = HEAD_PFIFO(ptree->orphan_cmds);
	for (; tos != NULL; tos = tos->link) {
		val = tos->value.d;
		switch(tos->attr) {

		default:
			INSIST(0);
			break;

		case T_Ceiling:
			if (val > STRATUM_UNSPEC - 1) {
				msyslog(LOG_WARNING,
					"CONFIG: Using maximum tos ceiling %d, %g requested",
					STRATUM_UNSPEC - 1, val);
				val = STRATUM_UNSPEC - 1;
			}
			item = PROTO_CEILING;
			break;

		case T_Floor:
			item = PROTO_FLOOR;
			break;

		case T_Orphan:
			item = PROTO_ORPHAN;
			break;

		case T_Orphanwait:
			item = PROTO_ORPHWAIT;
			break;

		case T_Mindist:
			item = PROTO_MINDISP;
			break;

		case T_Maxdisp:
			item = PROTO_MAXDISP;
			break;

		case T_Maxdist:
			item = PROTO_MAXDIST;
			break;

		case T_Minclock:
			item = PROTO_MINCLOCK;
			break;

		case T_Maxclock:
			item = PROTO_MAXCLOCK;
			break;

		case T_Minsane:
			item = PROTO_MINSANE;
			break;

		}
		proto_config(item, 0, val);
	}
}


static void
free_config_tos(
	config_tree *ptree
	)
{
	FREE_ATTR_VAL_FIFO(ptree->orphan_cmds);
}


static void
config_monitor(
	config_tree *ptree
	)
{
	int_node *pfilegen_token;
	const char *filegen_string;
	const char *filegen_file;
	FILEGEN *filegen;
	filegen_node *my_node;
	attr_val *my_opts;
	int filegen_type;
	int filegen_flag;

	/* Set the statistics directory */
	if (ptree->stats_dir)
		stats_config(STATS_STATSDIR, ptree->stats_dir);

	/* NOTE:
	 * Calling filegen_get is brain dead. Doing a string
	 * comparison to find the relavant filegen structure is
	 * expensive.
	 *
	 * Through the parser, we already know which filegen is
	 * being specified. Hence, we should either store a
	 * pointer to the specified structure in the syntax tree
	 * or an index into a filegen array.
	 *
	 * Need to change the filegen code to reflect the above.
	 */

	/* Turn on the specified statistics */
	pfilegen_token = HEAD_PFIFO(ptree->stats_list);
	for (; pfilegen_token != NULL; pfilegen_token = pfilegen_token->link) {
		filegen_string = keyword(pfilegen_token->i);
		filegen = filegen_get(filegen_string);
		if (NULL == filegen) {
			msyslog(LOG_ERR,
				"CONFIG: stats %s unrecognized",
				filegen_string);
			continue;
		}
		DPRINT(4, ("enabling filegen for %s statistics '%s%s'\n",
			   filegen_string, filegen->dir,
			   filegen->fname));
		filegen_flag = filegen->flag;
		filegen_flag |= FGEN_FLAG_ENABLED;
		filegen_config(filegen, statsdir, filegen_string,
			       filegen->type, (unsigned int)filegen_flag);
	}

	/* Configure the statistics with the options */
	my_node = HEAD_PFIFO(ptree->filegen_opts);
	for (; my_node != NULL; my_node = my_node->link) {
		filegen_string = keyword(my_node->filegen_token);
		filegen = filegen_get(filegen_string);
		if (NULL == filegen) {
			msyslog(LOG_ERR,
				"CONFIG: filegen category '%s' unrecognized",
				filegen_string);
			continue;
		}
		filegen_file = filegen_string;

		/* Initialize the filegen variables to their pre-configuration states */
		filegen_flag = filegen->flag;
		filegen_type = filegen->type;

		/* "filegen ... enabled" is the default (when filegen is used) */
		filegen_flag |= FGEN_FLAG_ENABLED;

		my_opts = HEAD_PFIFO(my_node->options);
		for (; my_opts != NULL; my_opts = my_opts->link) {
			switch (my_opts->attr) {

			case T_File:
				filegen_file = my_opts->value.s;
				break;

			case T_Type:
				switch (my_opts->value.i) {

				default:
					INSIST(0);
					break;

				case T_None:
					filegen_type = FILEGEN_NONE;
					break;

				case T_Pid:
					filegen_type = FILEGEN_PID;
					break;

				case T_Day:
					filegen_type = FILEGEN_DAY;
					break;

				case T_Week:
					filegen_type = FILEGEN_WEEK;
					break;

				case T_Month:
					filegen_type = FILEGEN_MONTH;
					break;

				case T_Year:
					filegen_type = FILEGEN_YEAR;
					break;

				case T_Age:
					filegen_type = FILEGEN_AGE;
					break;
				}
				break;

			case T_Flag:
				switch (my_opts->value.i) {

				case T_Link:
					filegen_flag |= FGEN_FLAG_LINK;
					break;

				case T_Nolink:
					filegen_flag &= ~FGEN_FLAG_LINK;
					break;

				case T_Enable:
					filegen_flag |= FGEN_FLAG_ENABLED;
					break;

				case T_Disable:
					filegen_flag &= ~FGEN_FLAG_ENABLED;
					break;

				default:
					msyslog(LOG_ERR,
						"CONFIG: Unknown filegen flag token %d",
						my_opts->value.i);
					exit(1);
				}
				break;

			default:
				msyslog(LOG_ERR,
					"CONFIG: Unknown filegen option token %d",
					my_opts->attr);
				exit(1);
			}
		}
		filegen_config(filegen, statsdir, filegen_file,
			       (unsigned int)filegen_type, (unsigned int)filegen_flag);
	}
}


static void
free_config_monitor(
	config_tree *ptree
	)
{
	if (ptree->stats_dir) {
		free(ptree->stats_dir);
		ptree->stats_dir = NULL;
	}

	FREE_INT_FIFO(ptree->stats_list);
	FREE_FILEGEN_FIFO(ptree->filegen_opts);
}


static void
config_access(
	config_tree *ptree
	)
{
	static bool		warned_signd;
	attr_val *		my_opt;
	restrict_node *		my_node;
	int_node *		curr_flag;
	sockaddr_u		addr;
	sockaddr_u		mask;
	struct addrinfo		hints;
	struct addrinfo *	ai_list = NULL;
	struct addrinfo *	pai;
	int			rc;
	bool			restrict_default;
	unsigned short		flags;
	unsigned short		mflags;
	bool			range_err;
	const char *		signd_warning =
#ifdef ENABLE_MSSNTP
	    "MS-SNTP signd operations currently block ntpd degrading service to all clients.";
#else
	    "mssntp restrict bit ignored, this ntpd was configured without --enable-mssntp.";
#endif

	/* Configure the mru options */
	my_opt = HEAD_PFIFO(ptree->mru_opts);
	for (; my_opt != NULL; my_opt = my_opt->link) {

		range_err = false;

		switch (my_opt->attr) {

		case T_Incalloc:
			if (0 <= my_opt->value.i)
				mon_data.mru_incalloc = my_opt->value.u;
			else
				range_err = true;
			break;

		case T_Incmem:
			if (0 <= my_opt->value.i)
				mon_data.mru_incalloc = (my_opt->value.u * 1024U)
						/ sizeof(mon_entry);
			else
				range_err = true;
			break;

		case T_Initalloc:
			if (0 <= my_opt->value.i)
				mon_data.mru_initalloc = my_opt->value.u;
			else
				range_err = true;
			break;

		case T_Initmem:
			if (0 <= my_opt->value.i)
				mon_data.mru_initalloc = (my_opt->value.u * 1024U)
						 / sizeof(mon_entry);
			else
				range_err = true;
			break;

		case T_Mindepth:
			if (0 <= my_opt->value.i)
				mon_data.mru_mindepth = my_opt->value.u;
			else
				range_err = true;
			break;

		case T_Maxage:
			mon_data.mru_maxage = my_opt->value.i;
			break;

		case T_Minage:
			mon_data.mru_minage = my_opt->value.i;
			break;

		case T_Maxdepth:
			if (0 <= my_opt->value.i)
				mon_data.mru_maxdepth = my_opt->value.u;
			else
				mon_data.mru_maxdepth = UINT_MAX;
			break;

		case T_Maxmem:
			if (0 <= my_opt->value.i)
				mon_data.mru_maxdepth = (my_opt->value.u * 1024U) /
					       sizeof(mon_entry);
			else
				mon_data.mru_maxdepth = UINT_MAX;
			break;

		default:
			msyslog(LOG_ERR,
				"CONFIG: Unknown mru option %s (%d)",
				keyword(my_opt->attr), my_opt->attr);
			exit(1);
		}
		if (range_err)
			msyslog(LOG_ERR,
				"CONFIG: mru %s %d out of range, ignored.",
				keyword(my_opt->attr), my_opt->value.i);
	}

	/* Configure the discard options */
	my_opt = HEAD_PFIFO(ptree->discard_opts);
	for (; my_opt != NULL; my_opt = my_opt->link) {

		switch (my_opt->attr) {

		case T_Average:
			if (0 <= my_opt->value.i &&
			    my_opt->value.i <= UCHAR_MAX)
				ntp_minpoll = (uint8_t)my_opt->value.u;
			else
				msyslog(LOG_ERR,
					"CONFIG: discard average %d out of range, ignored.",
					my_opt->value.i);
			break;

		case T_Minimum:
			ntp_minpkt = my_opt->value.i;
			break;

		case T_Monitor:
			mon_data.mon_age = my_opt->value.i;
			break;

		default:
			msyslog(LOG_ERR,
				"CONFIG: Unknown discard option %s (%d)",
				keyword(my_opt->attr), my_opt->attr);
			exit(1);
		}
	}

	/* Configure the restrict options */
	my_node = HEAD_PFIFO(ptree->restrict_opts);
	for (; my_node != NULL; my_node = my_node->link) {
		if (ai_list != NULL) {
                        /* we do this here, to not need at every continue */
			freeaddrinfo(ai_list);
			ai_list = NULL;
                }
		/* Parse the flags */
		flags = 0;
		mflags = 0;

		curr_flag = HEAD_PFIFO(my_node->flags);
		for (; curr_flag != NULL; curr_flag = curr_flag->link) {
			switch (curr_flag->i) {

			default:
				INSIST(0);
				break;

			case T_Ntpport:
				mflags |= RESM_NTPONLY;
				break;

			case T_Source:
				mflags |= RESM_SOURCE;
				break;

			case T_Flake:
				flags |= RES_FLAKE;
				break;

			case T_Ignore:
				flags |= RES_IGNORE;
				break;

			case T_Kod:
				flags |= RES_KOD;
				break;

			case T_Mssntp:
				flags |= RES_MSSNTP;
				break;

			case T_Limited:
				flags |= RES_LIMITED;
				break;

			case T_Nomodify:
				flags |= RES_NOMODIFY;
				break;

			case T_Nomrulist:
				flags |= RES_NOMRULIST;
				break;

			case T_Nopeer:
				flags |= RES_NOPEER;
				break;

			case T_Noquery:
				flags |= RES_NOQUERY;
				break;

			case T_Noserve:
				flags |= RES_DONTSERVE;
				break;

			case T_Notrap:
				/*
				 * No-op - included for backward compatibility
				 * with all the world's boilerplate ntp.conf
				 * files.
				 */
				flags |= RES_NOTRAP;
				break;

			case T_Notrust:
				flags |= RES_DONTTRUST;
				break;

			case T_Version:
				flags |= RES_VERSION;
				break;
			}
		}

		if ((RES_MSSNTP & flags) && !warned_signd) {
			warned_signd = true;
			fprintf(stderr, "%s\n", signd_warning);
			msyslog(LOG_WARNING, "CONFIG: %s", signd_warning);
		}

		/* It would be swell if we could identify the line number */
		if ((RES_KOD & flags) && !(RES_LIMITED & flags)) {
			const char *kod_where = (my_node->addr)
					  ? my_node->addr->address
					  : (mflags & RESM_SOURCE)
					    ? "source"
					    : "default";
			const char *kod_warn = "KOD does nothing without LIMITED.";

			fprintf(stderr, "restrict %s: %s\n", kod_where, kod_warn);
			msyslog(LOG_WARNING, "CONFIG: restrict %s: %s", kod_where, kod_warn);
		}

		if (RES_NOTRAP & flags) {
			const char *notrap_where = (my_node->addr)
					  ? my_node->addr->address
					  : (mflags & RESM_SOURCE)
					    ? "source"
					    : "default";

			msyslog(LOG_WARNING, "CONFIG: restrict %s: notrap keyword is ignored.", notrap_where);
		}

		ZERO_SOCK(&addr);
		pai = NULL;
		restrict_default = false;

		if (NULL == my_node->addr) {
			ZERO_SOCK(&mask);
			if (!(RESM_SOURCE & mflags)) {
				/*
				 * The user specified a default rule
				 * without a -4 / -6 qualifier, add to
				 * both lists
				 */
				restrict_default = true;
			} else {
				/* apply "restrict source ..." */
				DPRINT(1, ("restrict source template mflags %x flags %x\n",
					   mflags, flags));
				hack_restrict(RESTRICT_FLAGS, NULL,
					      NULL, mflags, flags, 0);
				continue;
			}
		} else {

			/* Resolve the specified address */
                        /* CIDR notation? */
                        /* will overwrite my_node->mask-> address with CIDR */
                        fix_node_cidr(my_node);
                        /* type is always zero, AF_INET */
			AF(&addr) = (unsigned short)my_node->addr->type;

			if (getnetnum(my_node->addr->address,
				      &addr) != 1) {
				/*
				 * Attempt a blocking lookup.  This
				 * is in violation of the nonblocking
				 * design of ntpd's mainline code.  The
				 * alternative of running without the
				 * restriction until the name resolved
				 * seems worse.
				 * Ideally some scheme could be used for
				 * restrict directives in the startup
				 * ntp.conf to delay starting up the
				 * protocol machinery until after all
				 * restrict hosts have been resolved.
				 */
				ZERO(hints);
				hints.ai_protocol = IPPROTO_UDP;
				hints.ai_socktype = SOCK_DGRAM;
				hints.ai_family = my_node->addr->type;
				rc = getaddrinfo(my_node->addr->address,
						 "ntp", &hints,
						 &ai_list);
				if (rc) {
					msyslog(LOG_ERR,
						"CONFIG: restrict: ignoring line %d, address/host '%s' unusable.",
						my_node->line_no,
						my_node->addr->address);
					continue;
				}
				INSIST(ai_list != NULL);
				pai = ai_list;
				INSIST(pai->ai_addr != NULL);
				INSIST(sizeof(addr) >=
					   pai->ai_addrlen);
				memcpy(&addr, pai->ai_addr,
				       pai->ai_addrlen);
				INSIST(AF_INET == AF(&addr) ||
					   AF_INET6 == AF(&addr));
			}

			SET_HOSTMASK(&mask, AF(&addr));

			/* Resolve the mask */
			if (my_node->mask) {
				ZERO_SOCK(&mask);
				AF(&mask) = my_node->mask->type;
				if (getnetnum(my_node->mask->address,
					      &mask) != 1) {
					msyslog(LOG_ERR,
						"CONFIG: restrict: ignoring line %d, mask '%s' unusable.",
						my_node->line_no,
						my_node->mask->address);
					continue;
				}
			}
		}

		/* Set the flags */
		if (restrict_default) {
			AF(&addr) = AF_INET;
			AF(&mask) = AF_INET;
			hack_restrict(RESTRICT_FLAGS, &addr,
				      &mask, mflags, flags, 0);
			AF(&addr) = AF_INET6;
			AF(&mask) = AF_INET6;
		}

		do {
			int op;
			if (my_node->mode == T_Restrict)
				op = RESTRICT_FLAGS;
			else if (my_node->mode == T_Unrestrict
					&& flags == 0 && mflags == 0)
				op = RESTRICT_REMOVE;
			else if (my_node->mode == T_Unrestrict)
				op = RESTRICT_UNFLAG;
			else
				continue;	/* should never happen */
			hack_restrict(op, &addr,
				      &mask, mflags, flags, 0);
			if (pai != NULL &&
			    NULL != (pai = pai->ai_next)) {
				INSIST(pai->ai_addr != NULL);
				INSIST(sizeof(addr) >=
					   pai->ai_addrlen);
				ZERO_SOCK(&addr);
				memcpy(&addr, pai->ai_addr,
				       pai->ai_addrlen);
				INSIST(AF_INET == AF(&addr) ||
					   AF_INET6 == AF(&addr));
				SET_HOSTMASK(&mask, AF(&addr));
			}
		} while (pai != NULL);

	}
	if (ai_list != NULL) {
                /* coverity thinks this can happen, so just in case */
		freeaddrinfo(ai_list);
		ai_list = NULL;
	}
	/* coverity[leaked_storage] */
}


static void
free_config_access(
	config_tree *ptree
	)
{
	FREE_ATTR_VAL_FIFO(ptree->mru_opts);
	FREE_ATTR_VAL_FIFO(ptree->discard_opts);
	FREE_RESTRICT_FIFO(ptree->restrict_opts);
}


static void
config_rlimit(
	config_tree *ptree
	)
{
	attr_val *	rlimit_av;

	rlimit_av = HEAD_PFIFO(ptree->rlimit);
	for (; rlimit_av != NULL; rlimit_av = rlimit_av->link) {
		switch (rlimit_av->attr) {

		default:
			INSIST(0);
			break;

		case T_Memlock:
			/* ignore, for backward compatibility */
			break;

		case T_Stacksize:
#if defined(RLIMIT_STACK)
			ntp_rlimit(RLIMIT_STACK,
				   (rlim_t)(rlimit_av->value.i * 4096),
				   4096,
				   "4k");
#else
			/* STDERR as well would be fine... */
			msyslog(LOG_WARNING, "CONFIG: 'rlimit stacksize' specified but is not available on this system.");
#endif /* RLIMIT_STACK */
			break;

		case T_Filenum:
#if defined(RLIMIT_NOFILE)
			ntp_rlimit(RLIMIT_NOFILE,
				  (rlim_t)(rlimit_av->value.i),
				  1,
				  "");
#else
			/* STDERR as well would be fine... */
			msyslog(LOG_WARNING, "CONFIG: 'rlimit filenum' specified but is not available on this system.");
#endif /* RLIMIT_NOFILE */
			break;

		}
	}
}


static void
config_tinker(
	config_tree *ptree
	)
{
	attr_val *	tinker;
	int		item;

	item = -1;	/* quiet warning */
	tinker = HEAD_PFIFO(ptree->tinker);
	for (; tinker != NULL; tinker = tinker->link) {
		switch (tinker->attr) {

		default:
			INSIST(0);
			break;

		case T_Allan:
			item = LOOP_ALLAN;
			break;

		case T_Dispersion:
			item = LOOP_PHI;
			break;

		case T_Freq:
			item = LOOP_FREQ;
			break;

		case T_Huffpuff:
			item = LOOP_HUFFPUFF;
			break;

		case T_Panic:
			item = LOOP_PANIC;
			break;

		case T_Step:
			item = LOOP_MAX;
			break;

		case T_Stepback:
			item = LOOP_MAX_BACK;
			break;

		case T_Stepfwd:
			item = LOOP_MAX_FWD;
			break;

		case T_Stepout:
			item = LOOP_MINSTEP;
			break;

		case T_Tick:
			item = LOOP_TICK;
			break;
		}
		loop_config(item, tinker->value.d);
	}
}


static void
free_config_rlimit(
	config_tree *ptree
	)
{
	FREE_ATTR_VAL_FIFO(ptree->rlimit);
}

static void
free_config_tinker(
	config_tree *ptree
	)
{
	FREE_ATTR_VAL_FIFO(ptree->tinker);
}


/*
 * config_nic_rules - apply interface listen/ignore/drop items
 */
static void
config_nic_rules(
	config_tree *ptree,
	bool input_from_file
	)
{
	nic_rule_node *	curr_node;
	sockaddr_u	addr;
	nic_rule_match	match_type;
	nic_rule_action	action;
	char *		if_name;
	char *		pchSlash;
	int		prefixlen;
	int		addrbits;

	curr_node = HEAD_PFIFO(ptree->nic_rules);

	if (curr_node != NULL && have_interface_option) {
		msyslog(LOG_ERR,
			"CONFIG: interface/nic rules are not allowed with --interface (-I) or --novirtualips (-L)%s",
			(input_from_file) ? ", exiting" : "");
		if (input_from_file)
			exit(1);
		else
			return;
	}

	for (; curr_node != NULL; curr_node = curr_node->link) {
		prefixlen = -1;
		if_name = curr_node->if_name;
		if (if_name != NULL)
			if_name = estrdup(if_name);

		switch (curr_node->match_class) {

		default:
			INSIST(false);
			break;

		case 0:
			/*
			 * 0 is out of range for valid token T_...
			 * and in a nic_rules_node indicates the
			 * interface descriptor is either a name or
			 * address, stored in if_name in either case.
			 */
			INSIST(if_name != NULL);
			pchSlash = strchr(if_name, '/');
			if (pchSlash != NULL)
				*pchSlash = '\0';
			if (is_ip_address(if_name, AF_UNSPEC, &addr)) {
				match_type = MATCH_IFADDR;
				if (pchSlash != NULL
				    && 1 == sscanf(pchSlash + 1, "%d",
					    &prefixlen)) {
					addrbits = 8 *
					    SIZEOF_INADDR(AF(&addr));
					prefixlen = max(-1, prefixlen);
					prefixlen = min(prefixlen,
							addrbits);
				}
			} else {
				match_type = MATCH_IFNAME;
				if (pchSlash != NULL)
					*pchSlash = '/';
			}
			break;

		case T_All:
			match_type = MATCH_ALL;
			break;

		case T_Ipv4:
			match_type = MATCH_IPV4;
			break;

		case T_Ipv6:
			match_type = MATCH_IPV6;
			break;

		case T_Wildcard:
			match_type = MATCH_WILDCARD;
			break;
		}

		switch (curr_node->action) {

		default:
			/*
			 * this assignment quiets a gcc "may be used
			 * uninitialized" warning and is here for no
			 * other reason.
			 */
			action = ACTION_LISTEN;
			INSIST(false);
			break;

		case T_Listen:
			action = ACTION_LISTEN;
			break;

		case T_Ignore:
			action = ACTION_IGNORE;
			break;

		case T_Drop:
			action = ACTION_DROP;
			break;
		}

		add_nic_rule(match_type, if_name, prefixlen,
			     action);
		timer_interfacetimeout(current_time + 2);
		if (if_name != NULL)
			free(if_name);
	}
}


static void
free_config_nic_rules(
	config_tree *ptree
	)
{
	nic_rule_node *curr_node;

	if (ptree->nic_rules != NULL) {
		for (;;) {
			UNLINK_FIFO(curr_node, *ptree->nic_rules, link);
			if (NULL == curr_node)
				break;
			free(curr_node->if_name);
			free(curr_node);
		}
		free(ptree->nic_rules);
		ptree->nic_rules = NULL;
	}
}


static void
apply_enable_disable(
	attr_val_fifo *	fifo,
	int		enable
	)
{
	attr_val *curr_flag;
	int option;

	for (curr_flag = HEAD_PFIFO(fifo);
	     curr_flag != NULL;
	     curr_flag = curr_flag->link) {

		option = curr_flag->value.i;
		switch (option) {

		default:
			msyslog(LOG_ERR,
				"CONFIG: can not apply enable/disable token %d, unknown",
				option);
			break;

		case T_Calibrate:
			proto_config(PROTO_CAL, (unsigned long)enable, 0.);
			break;

		case T_Kernel:
			proto_config(PROTO_KERNEL, (unsigned long)enable, 0.);
			break;

		case T_Monitor:
			proto_config(PROTO_MONITOR, (unsigned long)enable, 0.);
			break;

		case T_Ntp:
			proto_config(PROTO_NTP, (unsigned long)enable, 0.);
			break;

		case T_Stats:
			proto_config(PROTO_FILEGEN, (unsigned long)enable, 0.);
			break;

		}
	}
}


static void
config_system_opts(
	config_tree *ptree
	)
{
	apply_enable_disable(ptree->enable_opts, 1);
	apply_enable_disable(ptree->disable_opts, 0);
}


static void
free_config_system_opts(
	config_tree *ptree
	)
{
	FREE_ATTR_VAL_FIFO(ptree->enable_opts);
	FREE_ATTR_VAL_FIFO(ptree->disable_opts);
}


static void
config_logconfig(
	config_tree *ptree
	)
{
	attr_val *	my_lc;

	my_lc = HEAD_PFIFO(ptree->logconfig);
	for (; my_lc != NULL; my_lc = my_lc->link) {
		switch (my_lc->attr) {

		case '+':
			ntp_syslogmask |= get_logmask(my_lc->value.s);
			break;

		case '-':
			ntp_syslogmask &= ~get_logmask(my_lc->value.s);
			break;

		case '=':
			ntp_syslogmask = get_logmask(my_lc->value.s);
			break;
		default:
			INSIST(0);
			break;
		}
	}
}


static void
free_config_logconfig(
	config_tree *ptree
	)
{
	FREE_ATTR_VAL_FIFO(ptree->logconfig);
}


static void
config_phone(
	config_tree *ptree
	)
{
	size_t		i;
	string_node *	sn;

	i = 0;
	sn = HEAD_PFIFO(ptree->phone);
	for (; sn != NULL; sn = sn->link) {
		/* need to leave array entry for NULL terminator */
		if (i < COUNTOF(sys_phone) - 1) {
			sys_phone[i++] = estrdup(sn->s);
			sys_phone[i] = NULL;
		} else {
			msyslog(LOG_INFO,
				"CONFIG: Number of phone entries exceeds %zu. Ignoring phone %s...",
				(COUNTOF(sys_phone) - 1), sn->s);
		}
	}
}

static void
config_mdnstries(
	config_tree *ptree
	)
{
#if defined(HAVE_DNS_SD_H)
	mdnstries = ptree->mdnstries;
#else
	UNUSED_ARG(ptree);
#endif  /* HAVE_DNS_SD_H */
}

static void
free_config_phone(
	config_tree *ptree
	)
{
	FREE_STRING_FIFO(ptree->phone);
}


static void
config_setvar(
	config_tree *ptree
	)
{
	setvar_node *my_node;
	size_t	varlen, vallen, octets;
	char *	str;

	str = NULL;
	my_node = HEAD_PFIFO(ptree->setvar);
	for (; my_node != NULL; my_node = my_node->link) {
		varlen = strlen(my_node->var);
		vallen = strlen(my_node->val);
		octets = varlen + vallen + 1 + 1;
		str = erealloc(str, octets);
		snprintf(str, octets, "%s=%s", my_node->var,
			 my_node->val);
		set_sys_var(str, octets, (my_node->isdefault)
						? DEF
						: 0);
	}
	if (str != NULL)
		free(str);
}


static void
free_config_setvar(
	config_tree *ptree
	)
{
	FREE_SETVAR_FIFO(ptree->setvar);
}


static void
config_fudge(
	config_tree *ptree
	)
{
	addr_opts_node *curr_fudge;
# ifdef REFCLOCK
	attr_val *curr_opt;
	sockaddr_u addr_sock;
	address_node *addr_node;
	struct refclockstat clock_stat;
	bool err_flag;

	curr_fudge = HEAD_PFIFO(ptree->fudge);
	for (; curr_fudge != NULL; curr_fudge = curr_fudge->link) {
		err_flag = false;

		/* Get the reference clock address and
		 * ensure that it is sane
		 */
		addr_node = curr_fudge->addr;
		ZERO_SOCK(&addr_sock);
		if (getnetnum(addr_node->address, &addr_sock)
		    != 1) {
			err_flag = true;
			msyslog(LOG_ERR,
				"CONFIG: unrecognized fudge reference clock address %s, line ignored",
				socktoa(&addr_sock));
		}

		if (!ISREFCLOCKADR(&addr_sock)) {
			err_flag = true;
			msyslog(LOG_ERR,
				"CONFIG: inappropriate address %s for the fudge command, line ignored",
				socktoa(&addr_sock));
		}

		/* Parse all the options to the fudge command */
		ZERO(clock_stat);
		curr_opt = HEAD_PFIFO(curr_fudge->options);
		for (; curr_opt != NULL; curr_opt = curr_opt->link) {
			switch (curr_opt->attr) {

			case T_Time1:
				clock_stat.haveflags |= CLK_HAVETIME1;
				clock_stat.fudgetime1 = curr_opt->value.d;
				break;

			case T_Time2:
				clock_stat.haveflags |= CLK_HAVETIME2;
				clock_stat.fudgetime2 = curr_opt->value.d;
				break;

			case T_Stratum:
				clock_stat.haveflags |= CLK_HAVEVAL1;
				clock_stat.fudgeval1 = curr_opt->value.i;
				break;

			case T_Refid:
				clock_stat.haveflags |= CLK_HAVEVAL2;
				clock_stat.fudgeval2 = 0;
				memcpy(&clock_stat.fudgeval2,
				       curr_opt->value.s,
				       min(strlen(curr_opt->value.s), REFIDLEN));
				break;

			case T_Flag1:
				clock_stat.haveflags |= CLK_HAVEFLAG1;
				if (curr_opt->value.i)
					clock_stat.flags |= CLK_FLAG1;
				else
					clock_stat.flags &= ~CLK_FLAG1;
				break;

			case T_Flag2:
				clock_stat.haveflags |= CLK_HAVEFLAG2;
				if (curr_opt->value.i)
					clock_stat.flags |= CLK_FLAG2;
				else
					clock_stat.flags &= ~CLK_FLAG2;
				break;

			case T_Flag3:
				clock_stat.haveflags |= CLK_HAVEFLAG3;
				if (curr_opt->value.i)
					clock_stat.flags |= CLK_FLAG3;
				else
					clock_stat.flags &= ~CLK_FLAG3;
				break;

			case T_Flag4:
				clock_stat.haveflags |= CLK_HAVEFLAG4;
				if (curr_opt->value.i)
					clock_stat.flags |= CLK_FLAG4;
				else
					clock_stat.flags &= ~CLK_FLAG4;
				break;

			default:
				msyslog(LOG_ERR,
					"CONFIG: Unexpected fudge flag %s (%d) for %s",
					token_name(curr_opt->attr),
					curr_opt->attr, socktoa(&addr_sock));
				exit(curr_opt->attr ? curr_opt->attr : 1);
			}
		}
		if (!err_flag)
			refclock_control(&addr_sock, &clock_stat, NULL);
	}
# else
	curr_fudge = HEAD_PFIFO(ptree->fudge);
	if (curr_fudge != NULL)
		msyslog(LOG_ERR, "CONFIG: Fudge commands not supported: built without refclocks");
# endif
}


static void
free_config_fudge(
	config_tree *ptree
	)
{
	FREE_ADDR_OPTS_FIFO(ptree->fudge);
}


/* Clone of config_vars that only does log file. */
static void
config_logfile(
	config_tree *ptree
	)
{
	attr_val *curr_var;

	curr_var = HEAD_PFIFO(ptree->vars);
	for (; curr_var != NULL; curr_var = curr_var->link) {
		/* Determine which variable to set and set it */
		switch (curr_var->attr) {

		case T_Logfile:
			if (-1 == change_logfile(curr_var->value.s, true))
				msyslog(LOG_ERR,
					"CONFIG: Cannot open logfile %s: %m",
					curr_var->value.s);
			break;

		default:
			break;
		}
	}
}


static void
config_vars(
	config_tree *ptree
	)
{
	attr_val *curr_var;

	curr_var = HEAD_PFIFO(ptree->vars);
	for (; curr_var != NULL; curr_var = curr_var->link) {
		/* Determine which variable to set and set it */
		switch (curr_var->attr) {

		case T_Tick:
			loop_config(LOOP_TICK, curr_var->value.d);
			break;

		case T_Driftfile:
			if ('\0' == curr_var->value.s[0]) {
				stats_drift_file = 0;
				msyslog(LOG_INFO, "CONFIG: driftfile disabled");
			} else
				stats_config(STATS_FREQ_FILE, curr_var->value.s);
			break;

		case T_Dscp:
			/* DSCP is in the upper 6 bits of the IP TOS/DS field */
			qos = curr_var->value.i << 2;
			break;

		case T_WanderThreshold:		/* FALLTHROUGH */
		case T_Nonvolatile:
			wander_threshold = curr_var->value.d;
			break;

		case T_Leapfile:
			stats_config(STATS_LEAP_FILE, curr_var->value.s);
			break;

#ifdef ENABLE_LEAP_SMEAR
		case T_Leapsmearinterval:
			if (curr_var->value.i < 0) {
				msyslog(LOG_ERR, "CONFIG: negative leap smear interval ignored: %i", curr_var->value.i);
				break;
			}
			leap_smear_intv = curr_var->value.u;
			msyslog(LOG_INFO, "CONFIG: leap smear interval %u sec", leap_smear_intv);
			break;
#endif

		case T_Pidfile:
			stats_config(STATS_PID_FILE, curr_var->value.s);
			break;

		case T_Logfile:
			/* processed in config_logfile */
			break;

		default:
			msyslog(LOG_ERR,
				"CONFIG: config_vars(): unexpected token %d",
				curr_var->attr);
		}
	}
}


static void
free_config_vars(
	config_tree *ptree
	)
{
	FREE_ATTR_VAL_FIFO(ptree->vars);
}


/* Define a function to check if a resolved address is sane.
 * If yes, return true, else return false;
 */
static bool
is_sane_resolved_address(
	sockaddr_u *	peeraddr,
	int		hmode
	)
{
	if (!ISREFCLOCKADR(peeraddr) && ISBADADR(peeraddr)) {
		msyslog(LOG_ERR,
			"CONFIG: attempt to configure invalid address %s",
			socktoa(peeraddr));
		return false;
	}
	/*
	 * Shouldn't be able to specify multicast
	 * address for server/peer!
	 */
	if ((T_Server == hmode || T_Peer == hmode || T_Pool == hmode)
	    && IS_MCAST(peeraddr)) {
		msyslog(LOG_ERR,
			"CONFIG: attempt to configure invalid address %s",
			socktoa(peeraddr));
		return false;
	}

	if (IS_IPV6(peeraddr) && !ipv6_works)
		return false;

	/* Ok, all tests succeeded, now we can return true */
	return true;
}


/*
 * peer_config - configure a new association
 *
 * RETURN: a pointer to the new peer structure
 *         NULL if this would duplicate an existing peer
 */
struct peer *
peer_config(
	sockaddr_u *	srcadr,
	const char *	hostname,
	endpt *		dstadr,
	int		htype,
	struct peer_ctl  *ctl)
{
	uint8_t cast_flags;
	uint8_t hmode;

#ifndef ENABLE_DNS_LOOKUP
	if (NULL != hostname) {
		msyslog(LOG_ERR, "CONFIG: hostnames need DNS lookup: %s", hostname);
		return NULL;
	}
#endif

	/*
	 * We do a dirty little jig to figure the cast flags. This is
	 * probably not the best place to do this, at least until the
	 * configure code is rebuilt. Note only one flag can be set.
	 */
	switch (htype) {

	case T_Pool:
		cast_flags = MDF_POOL;
		hmode = MODE_CLIENT;
		ctl->flags &= ~FLAG_PREEMPT;
		break;

	case T_Peer:
		msyslog(LOG_ERR, "CONFIG: peer deprecated, treated as server: %s",
			NULL != hostname? hostname : socktoa(srcadr) );
		FALLTHRU
	case T_Server:
		cast_flags = MDF_UCAST;
		hmode = MODE_CLIENT;
		if (NULL != hostname)
			ctl->flags |= FLAG_DNS;
		break;

	default:
		msyslog(LOG_ERR, "CONFIG: peer_config, strange htype: %d", htype);
		cast_flags = MDF_UCAST;
		hmode = MODE_CLIENT;
	}

	/*
	 * Mobilize the association and initialize its variables. If
	 * emulating ntpdate, force iburst.  For pool,
	 * strip FLAG_PREEMPT as the prototype associations are not
	 * themselves preemptible, though the resulting associations
	 * are.
	 */
	ctl->flags |= FLAG_CONFIG;
	if (clock_ctl.mode_ntpdate)
		ctl->flags |= FLAG_IBURST;
	return newpeer(srcadr, hostname, dstadr, hmode,
		       ctl, cast_flags, true);
}


static void
config_peers(
	config_tree *ptree
	)
{
    sockaddr_u		peeraddr;
    peer_node *		curr_peer;

    /* add servers named on the command line with iburst implied */
    for ( ; cmdline_server_count > 0;
	cmdline_server_count--, cmdline_servers++) {
	     struct peer_ctl client_ctl;
	     /* so any new members we introduce will be zeroed */
	     memset(&client_ctl, '\0', sizeof(struct peer_ctl));
	     client_ctl.version = NTP_VERSION;
	     client_ctl.minpoll = NTP_MINDPOLL;
	     client_ctl.maxpoll = NTP_MAXPOLL_UNK;
	     client_ctl.flags = FLAG_IBURST;
	     client_ctl.mode = 0;
	     client_ctl.peerkey = 0;
	     client_ctl.bias = 0;

	    ZERO_SOCK(&peeraddr);
	    /*
	     * If we have a numeric address, we can safely
	     * proceed in the mainline with it.  Otherwise, hand
	     * the hostname off to the blocking child.
	     */
	    if (is_ip_address(*cmdline_servers, AF_UNSPEC, &peeraddr)) {
		SET_PORT(&peeraddr, NTP_PORT);
		if (is_sane_resolved_address(&peeraddr,
					     T_Server))
			peer_config(
				&peeraddr,
				NULL,
				NULL,
				T_Server,
				&client_ctl);
	    } else {
		/* we have a hostname to resolve */
		peer_config(
			&peeraddr,
			*cmdline_servers,
			NULL,
			T_Server,
			&client_ctl);
	    }
    }

    /* add associations from the configuration file */
    curr_peer = HEAD_PFIFO(ptree->peers);
    for (; curr_peer != NULL; curr_peer = curr_peer->link) {
	ZERO_SOCK(&peeraddr);

	if (T_Pool == curr_peer->host_mode) {
	    AF(&peeraddr) = curr_peer->addr->type;
	    peer_config(
		    &peeraddr,
		    curr_peer->addr->address,
		    NULL,
		    curr_peer->host_mode,
		    &curr_peer->ctl);
	/*
	 * If we have a numeric address, we can safely
	 * proceed in the mainline with it.
	 */
	} else if (is_ip_address(curr_peer->addr->address,
			  curr_peer->addr->type, &peeraddr)) {

	    SET_PORT(&peeraddr, NTP_PORT);
	    if (is_sane_resolved_address(&peeraddr, curr_peer->host_mode)) {
#ifdef REFCLOCK
		/* save maxpoll from config line
		 * newpeer smashes it
		 */
		uint8_t maxpoll = curr_peer->ctl.maxpoll;
#endif
		struct peer *peer = peer_config(
			&peeraddr,
			NULL,
			NULL,
			curr_peer->host_mode,
			&curr_peer->ctl);
		if ( NULL == peer )
		{
		    /* duplicate peer !?, ignore */
		    msyslog(LOG_INFO, "CONFIG: configpeers: Ignoring duplicate '%s'",
			socktoa(&peeraddr));
		    continue;
		}
		if (ISREFCLOCKADR(&peeraddr))
		{
#ifdef REFCLOCK
		    uint8_t clktype;
		    int unit;
		    /*
		     * We let the reference clock
		     * support do clock dependent
		     * initialization.  This
		     * includes setting the peer
		     * timer, since the clock may
		     * have requirements for this.
		     */
		    if (NTP_MAXPOLL_UNK == maxpoll)
			    /* default maxpoll for
			     * refclocks is minpoll
			     */
			    peer->cfg.maxpoll = peer->cfg.minpoll;
		    clktype = (uint8_t)REFCLOCKTYPE(&peer->srcadr);
		    unit = REFCLOCKUNIT(&peer->srcadr);

		    peer->cfg.path = curr_peer->ctl.path;
		    peer->cfg.ppspath = curr_peer->ctl.ppspath;
		    peer->cfg.baud = curr_peer->ctl.baud;
		    if (refclock_newpeer(clktype,
					  unit,
					  peer))
			    refclock_control(&peeraddr,
					     &curr_peer->clock_stat,
					     NULL);
		    else
			    /*
			     * Dump it, something screwed up
			     */
			    unpeer(peer);
#else /* REFCLOCK */
		    msyslog(LOG_ERR,
			 "INIT: ntpd was compiled without refclock support.");
		    unpeer(peer);
#endif /* REFCLOCK */
		}

	    }
	/* DNS lookup */
	} else {
	    AF(&peeraddr) = curr_peer->addr->type;
	    peer_config(
		&peeraddr,
		curr_peer->addr->address,
		NULL,
		curr_peer->host_mode,
		&curr_peer->ctl);
	}
    }
}

static void
free_config_peers(
	config_tree *ptree
	)
{
	peer_node *curr_peer;

	if (ptree->peers != NULL) {
		for (;;) {
			UNLINK_FIFO(curr_peer, *ptree->peers, link);
			if (NULL == curr_peer)
				break;
			destroy_address_node(curr_peer->addr);
			free(curr_peer);
		}
		free(ptree->peers);
		ptree->peers = NULL;
	}
}


static void
config_unpeers(
	config_tree *ptree
	)
{
	sockaddr_u		peeraddr;
	unpeer_node *		curr_unpeer;
	struct peer *		p;
	const char *		name;
	int			rc;

	curr_unpeer = HEAD_PFIFO(ptree->unpeers);
	for (; curr_unpeer != NULL; curr_unpeer = curr_unpeer->link) {
		/*
		 * Either AssocID will be zero, and we unpeer by name/
		 * address addr, or it is nonzero and addr NULL.
		 */
		if (curr_unpeer->assocID) {
			p = findpeerbyassoc(curr_unpeer->assocID);
			if (p != NULL) {
				msyslog(LOG_NOTICE, "CONFIG: unpeered %s",
					socktoa(&p->srcadr));
				peer_clear(p, "GONE", true);
				unpeer(p);
			}

			continue;
		}

		if (curr_unpeer->addr == NULL) {
			msyslog(LOG_ERR, "CONFIG: invalid address in unpeer command");
			continue;
		}

		ZERO(peeraddr);
		AF(&peeraddr) = curr_unpeer->addr->type;
		name = curr_unpeer->addr->address;
		rc = getnetnum(name, &peeraddr);
		/* Do we have a numeric address? */
		if (rc > 0) {
			DPRINT(1, ("unpeer: searching for %s\n",
				   socktoa(&peeraddr)));
			p = findexistingpeer(&peeraddr, NULL, NULL, -1);
			if (p != NULL) {
				msyslog(LOG_NOTICE, "CONFIG: unpeered %s",
					socktoa(&peeraddr));
				peer_clear(p, "GONE", true);
				unpeer(p);
			}

			continue;
		}

		/* It's not a numeric IP address... */

		/* It's a hostname. */
		p = findexistingpeer(NULL, name, NULL, -1);
		if (p != NULL) {
			msyslog(LOG_NOTICE, "CONFIG: unpeered %s", name);
			peer_clear(p, "GONE", true);
			unpeer(p);
		}
	}
}


static void
free_config_unpeers(
	config_tree *ptree
	)
{
	unpeer_node *curr_unpeer;

	if (ptree->unpeers != NULL) {
		for (;;) {
			UNLINK_FIFO(curr_unpeer, *ptree->unpeers, link);
			if (NULL == curr_unpeer)
				break;
			destroy_address_node(curr_unpeer->addr);
			free(curr_unpeer);
		}
		free(ptree->unpeers);
	}
}


static void
config_reset_counters(
	config_tree *ptree
	)
{
	int_node *counter_set;

	for (counter_set = HEAD_PFIFO(ptree->reset_counters);
	     counter_set != NULL;
	     counter_set = counter_set->link) {
		switch (counter_set->i) {
		default:
			DPRINT(1, ("config_reset_counters %s (%d) invalid\n",
				   keyword(counter_set->i), counter_set->i));
			break;

		case T_Allpeers:
			peer_all_reset();
			break;

		case T_Auth:
			auth_reset_stats(current_time);
			break;

		case T_Ctl:
			ctl_clr_stats();
			break;

		case T_Io:
			io_clr_stats();
			break;

		case T_Mem:
			peer_clr_stats();
			break;

		case T_Sys:
			proto_clr_stats();
			break;

		case T_Timer:
			timer_clr_stats();
			break;
		}
	}
}


static void
free_config_reset_counters(
	config_tree *ptree
	)
{
	FREE_INT_FIFO(ptree->reset_counters);
}


static void
config_ntpd(
	config_tree *ptree,
	bool input_from_files
	)
{

/* Do this early so most errors go to new log file */
/* Command line arg is earlier. */
	config_logfile(ptree);

	config_nic_rules(ptree, input_from_files);
	config_monitor(ptree);
	config_auth(ptree);
	config_tos(ptree);
	config_access(ptree);
	config_tinker(ptree);
	config_rlimit(ptree);
	config_system_opts(ptree);
	config_logconfig(ptree);
	config_phone(ptree);
	config_mdnstries(ptree);
	config_setvar(ptree);
	config_vars(ptree);

	io_open_sockets();

	config_peers(ptree);
	config_unpeers(ptree);
	config_fudge(ptree);
	config_reset_counters(ptree);
}


/*
 * config_remotely() - implements ntpd side of ntpq :config
 */
void
config_remotely(
	sockaddr_u *	remote_addr
	)
{
	char origin[128];

	snprintf(origin, sizeof(origin), "remote config from %s",
		 socktoa(remote_addr));
	lex_init_stack(origin, NULL); /* no checking needed... */
	init_syntax_tree(&cfgt);
	yyparse();
	lex_drop_stack();

	cfgt.source.attr = CONF_SOURCE_NTPQ;
	cfgt.timestamp = time(NULL);
	cfgt.source.value.s = estrdup(socktoa(remote_addr));

	DPRINT(1, ("Finished Parsing!!\n"));

	save_and_apply_config_tree(false);
}


/*
 * getconfig() - return name of configuration file e.g /etc/ntp.conf
 */
const char *
getconfig(const char *explicit_config)
{
	const char *config_file;

	config_file = CONFIG_FILE;

	if (explicit_config) {
#ifdef HAVE_NETINFO_NI_H
	    check_netinfo = false;
#endif
	    config_file = explicit_config;
	}

	return config_file;
}

/*
 * init_readconfig() - init for readconfig
 */
void init_readconfig(void)
{
	init_syntax_tree(&cfgt);
}

/*
 * readconfig() - process startup configuration file
 */
void readconfig(const char *config_file)
{
	char	line[256];
	char	dirpath[PATH_MAX];
	int	srccount;
	/*
	 * install a non default variable with this daemon version
	 */
	snprintf(line, sizeof(line), "daemon_version=\"%s\"", ntpd_version());
	set_sys_var(line, strlen(line) + 1, RO);

	atexit(free_all_config_trees);

#ifdef DEBUG
#ifndef yydebug
	/* See comment above for yyparse */
	extern int yydebug;
#endif
	yydebug = !!(debug >= 5);
#endif

	/* Moved to init_readconfig so command lines can contribute info
	 * init_syntax_tree(&cfgt);
	 */
	srccount = 0;

	/* parse the plain config file if it exists */
	if (lex_init_stack(config_file, "r")) {
		msyslog(LOG_INFO, "CONFIG: readconfig: parsing file: %s", config_file);
	 	yyparse();
		++srccount;
		//cfgt.source.value.s = estrdup(config_file);
	}

	/* parse configs in parallel subdirectory if that exists */
	reparent(dirpath, sizeof(dirpath), config_file, CONFIG_DIR);
	if (is_directory(dirpath) && lex_push_file(dirpath)) {
		msyslog(LOG_INFO, "CONFIG: readconfig: parsing directory: %s", config_file);
	    yyparse();
	    ++srccount;
	}

	if (srccount == 0) {
#ifdef HAVE_NETINFO_NI_H
		/* If there is no config_file, try NetInfo. */
	    if (check_netinfo)
		config_netinfo = get_netinfo_config();
#endif /* HAVE_NETINFO_NI_H */
	    io_open_sockets();
	}

	lex_drop_stack();

	DPRINT(1, ("Finished Parsing!!\n"));

	//cfgt.source.attr = CONF_SOURCE_FILE;
	cfgt.timestamp = time(NULL);

	save_and_apply_config_tree(true);

#ifdef HAVE_NETINFO_NI_H
	if (config_netinfo)
		free_netinfo_config(config_netinfo);
#endif /* HAVE_NETINFO_NI_H */
}


/* hooks for ntpd.c */

void set_keys_file(char* keys)
{
	cfgt.auth.keys = estrdup(keys);
};

void set_trustedkey(keyid_t tkey)
{
	attr_val *val = create_attr_ival('i', tkey);
	attr_val *val2 = NULL;
	APPEND_G_FIFO(val2, val);
	CONCAT_G_FIFOS(cfgt.auth.trusted_key_list, val2);
};



void
save_and_apply_config_tree(bool input_from_file)
{
	config_tree *ptree;
	config_tree *punlinked;

	/*
	 * Keep all the configuration trees applied since startup in
	 * a list that can be used to dump the configuration back to
	 * a text file.
	 */
	ptree = emalloc(sizeof(*ptree));
	memcpy(ptree, &cfgt, sizeof(*ptree));
	ZERO(cfgt);

	LINK_TAIL_SLIST(cfg_tree_history, ptree, link, config_tree);

	/* The actual configuration done depends on whether we are
	 * configuring the simulator or the daemon. Perform a check
	 * and call the appropriate function as needed.
	 */

	config_ntpd(ptree, input_from_file);

	UNLINK_SLIST(punlinked, cfg_tree_history, ptree, link,
		     config_tree);
	INSIST(punlinked == ptree);
	free_config_tree(ptree);
}


/* FUNCTIONS COPIED FROM THE OLDER ntp_config.c
 * --------------------------------------------
 */


/*
 * get_pfxmatch - find value for prefixmatch
 * and update char * accordingly
 */
static uint32_t
get_pfxmatch(
	const char **	pstr,
	struct masks *	m
	)
{
	while (m->name != NULL) {
		if (strncmp(*pstr, m->name, strlen(m->name)) == 0) {
			*pstr += strlen(m->name);
			return m->mask;
		} else {
			m++;
		}
	}
	return 0;
}

/*
 * get_match - find logmask value
 */
static uint32_t
get_match(
	const char *	str,
	struct masks *	m
	)
{
	while (m->name != NULL) {
		if (strcmp(str, m->name) == 0)
			return m->mask;
		else
			m++;
	}
	return 0;
}

/*
 * get_logmask - build bitmask for ntp_syslogmask
 */
static uint32_t
get_logmask(
	const char *	str
	)
{
	const char *	t;
	uint32_t		offset;
	uint32_t		mask;

	mask = get_match(str, logcfg_noclass_items);
	if (mask != 0)
		return mask;

	t = str;
	offset = get_pfxmatch(&t, logcfg_class);
	mask   = get_match(t, logcfg_class_items);

	if (mask)
		return mask << offset;
	else
		msyslog(LOG_ERR, "CONFIG: logconfig: '%s' not recognized - ignored",
			str);

	return 0;
}


#ifdef HAVE_NETINFO_NI_H

/*
 * get_netinfo_config - find the nearest NetInfo domain with an ntp
 * configuration and initialize the configuration state.
 */
static struct netinfo_config_state *
get_netinfo_config(void)
{
	ni_status status;
	void *domain;
	ni_id config_dir;
	struct netinfo_config_state *config;

	if (ni_open(NULL, ".", &domain) != NI_OK) return NULL;

	while ((status = ni_pathsearch(domain, &config_dir, NETINFO_CONFIG_DIR)) == NI_NODIR) {
		void *next_domain;
		if (ni_open(domain, "..", &next_domain) != NI_OK) {
			ni_free(next_domain);
			break;
		}
		ni_free(domain);
		domain = next_domain;
	}
	if (status != NI_OK) {
		ni_free(domain);
		return NULL;
	}

	config = emalloc(sizeof(*config));
	config->domain = domain;
	config->config_dir = config_dir;
	config->prop_index = 0;
	config->val_index = 0;
	config->val_list = NULL;

	return config;
}


/*
 * free_netinfo_config - release NetInfo configuration state
 */
static void
free_netinfo_config(
	struct netinfo_config_state *config
	)
{
	ni_free(config->domain);
	free(config);
}


/*
 * gettokens_netinfo - return tokens from NetInfo
 */
static int
gettokens_netinfo (
	struct netinfo_config_state *config,
	char **tokenlist,
	int *ntokens
	)
{
	int prop_index = config->prop_index;
	int val_index = config->val_index;
	char **val_list = config->val_list;

	/*
	 * Iterate through each keyword and look for a property that matches it.
	 */
  again:
	if (!val_list) {
		for (; prop_index < COUNTOF(keywords); prop_index++)
		{
			ni_namelist namelist;
			struct keyword current_prop = keywords[prop_index];
			ni_index index;

			/*
			 * For each value associated in the property, we're going to return
			 * a separate line. We squirrel away the values in the config state
			 * so the next time through, we don't need to do this lookup.
			 */
			NI_INIT(&namelist);
			if (NI_OK == ni_lookupprop(config->domain,
			    &config->config_dir, current_prop.text,
			    &namelist)) {

				/* Found the property, but it has no values */
				if (namelist.ni_namelist_len == 0) continue;

				config->val_list =
				    eallocarray(
					(namelist.ni_namelist_len + 1),
					sizeof(char*));
				val_list = config->val_list;

				for (index = 0;
				     index < namelist.ni_namelist_len;
				     index++) {
					char *value;

					value = namelist.ni_namelist_val[index];
					val_list[index] = estrdup(value);
				}
				val_list[index] = NULL;

				break;
			}
			ni_namelist_free(&namelist);
		}
		config->prop_index = prop_index;
	}

	/* No list; we're done here. */
	if (!val_list)
		return CONFIG_UNKNOWN;

	/*
	 * We have a list of values for the current property.
	 * Iterate through them and return each in order.
	 */
	if (val_list[val_index]) {
		int ntok = 1;
		int quoted = 0;
		char *tokens = val_list[val_index];

		msyslog(LOG_INFO, "CONFIG: %s %s", keywords[prop_index].text, val_list[val_index]);

		(const char*)tokenlist[0] = keywords[prop_index].text;
		for (ntok = 1; ntok < MAXTOKENS; ntok++) {
			tokenlist[ntok] = tokens;
			while (!ISEOL(*tokens) && (!ISSPACE(*tokens) || quoted))
				quoted ^= (*tokens++ == '"');

			if (ISEOL(*tokens)) {
				*tokens = '\0';
				break;
			} else {		/* must be space */
				*tokens++ = '\0';
				while (ISSPACE(*tokens))
					tokens++;
				if (ISEOL(*tokens))
					break;
			}
		}

		if (ntok == MAXTOKENS) {
			/* HMS: chomp it to lose the EOL? */
			msyslog(LOG_ERR,
				"CONFIG: gettokens_netinfo: too many tokens.  Ignoring: %s",
				tokens);
		} else {
			*ntokens = ntok + 1;
		}

		config->val_index++;	/* HMS: Should this be in the 'else'? */

		return keywords[prop_index].keytype;
	}

	/* We're done with the current property. */
	prop_index = ++config->prop_index;

	/* Free val_list and reset counters. */
	for (val_index = 0; val_list[val_index]; val_index++)
		free(val_list[val_index]);
	free(val_list);
	val_list = config->val_list = NULL;
	val_index = config->val_index = 0;

	goto again;
}
#endif /* HAVE_NETINFO_NI_H */

/*
 * check my_node->addr for CIDR notation
 * if so, convert to old addr/mask notation and override mask
 */
static void
fix_node_cidr(
    restrict_node *my_node)
{
    address_node *addr;
    char mask_s[40], *mask_p;
    char *cidr_p;
    char *colon_p;
    char *endptr;
    long cidr_len;
    int i;
    unsigned a[8];

    REQUIRE(my_node);
    addr = my_node->addr;
    REQUIRE(addr);

    cidr_p = strrchr(addr->address, '/');
    if (!cidr_p) {
        /* not CIDR, leave silently */
        return;
    }
    *cidr_p++ = '\0'; /* remove the '/' and beyond from address */
    /* get CIDR as int */
    errno = 0;
    cidr_len = strtol(cidr_p, &endptr, 10);
    if ( errno || (endptr == cidr_p) ) {
        /* conversion fail, leave silently */
        return;
    }
    if ( 0 >= cidr_len ) {
        /* negative or zero?  leave silently */
	/* exiting on 0 avoids a bad shift warning from Coverity */
        return;
    }
    /* sadly, addr->type not previously set, look for colon */
    colon_p = strrchr(addr->address, ':');
    if (colon_p) {
        /* IPv6 */
        uint64_t mask_top = 0xFFFFFFFFFFFFFFFFU;
        uint64_t mask_bot = 0xFFFFFFFFFFFFFFFFU;

	if ( 128 < cidr_len ) {
	    /* out of range, leave silently */
	    return;
	}
        if ( 64 >= cidr_len ) {
	    mask_bot = 0;
	    mask_top <<= 64 - cidr_len ;
        } else {
	    mask_bot <<= 128 - cidr_len ;
        }
        for (i = 0; i < 4; i++)
                a[i] = mask_top >> (16 * (3 - i)) & 0xffffU;
        for (i = 0; i < 4; i++)
                a[i + 4] = mask_bot >> (16 * (3 - i)) & 0xffffU;

        snprintf(mask_s, sizeof(mask_s), "%x:%x:%x:%x:%x:%x:%x:%x",
                 a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
    } else {
        /* must be IPv4 */
        uint32_t mask_n = 0xFFFFFFFFU;

	if ( 32 < cidr_len ) {
	    /* out of range, leave silently */
	    return;
	}

        mask_n <<= 32 - cidr_len ;
        for (i = 0; i < 4; i++)
                a[i] = mask_n >> (8 * (3 - i)) & 0xff;

        snprintf(mask_s, sizeof(mask_s), "%u.%u.%u.%u",
                 a[0], a[1], a[2], a[3]);
    }

    /* lose old mask */
    destroy_address_node(my_node->mask);

    /* create mask node, yes AF_UNSPEC is weird... */
    mask_p = estrdup(mask_s);
    my_node->mask = create_address_node(mask_p, AF_UNSPEC);
}

/*
 * getnetnum - return a net number (this is crude, but careful)
 *
 * returns 1 for success, and mysteriously, 0 for most failures, and
 * -1 if the address found is IPv6 and we believe IPv6 isn't working.
 */
static int
getnetnum(
	const char *num,
	sockaddr_u *addr
	)
{

	REQUIRE(AF_UNSPEC == AF(addr) ||
		    AF_INET == AF(addr) ||
		    AF_INET6 == AF(addr));

	if (!is_ip_address(num, AF(addr), addr))
		return 0;

	if (IS_IPV6(addr) && !ipv6_works)
		return -1;

# ifdef ISC_PLATFORM_HAVESALEN
	addr->sa.sa_len = SIZEOF_SOCKADDR(AF(addr));
# endif
	SET_PORT(addr, NTP_PORT);

	DPRINT(2, ("getnetnum given %s, got %s\n", num, socktoa(addr)));

	return 1;
}

void
ntp_rlimit(
	int	rl_what,
	rlim_t	rl_value,
	int	rl_scale,
	const char *	rl_sstr
	)
{
	struct rlimit	rl;

#ifndef DEBUG
	UNUSED_ARG(rl_scale);
	UNUSED_ARG(rl_sstr);
#endif

	switch (rl_what) {
#ifdef RLIMIT_MEMLOCK
	    case RLIMIT_MEMLOCK:
		/* ignore - for backward compatibility only */
		break;
#endif /* RLIMIT_MEMLOCK */

#ifdef RLIMIT_NOFILE
	    case RLIMIT_NOFILE:
		/*
		 * For large systems the default file descriptor limit may
		 * not be enough.
		 */
		DPRINT(2, ("ntp_rlimit: NOFILE: %d %s\n",
			   (int)rl_value / rl_scale, rl_sstr));
		rl.rlim_cur = rl.rlim_max = rl_value;
		if (setrlimit(RLIMIT_NOFILE, &rl) == -1)
			msyslog(LOG_ERR, "CONFIG: Cannot set RLIMIT_NOFILE: %m");
		break;
#endif /* RLIMIT_NOFILE */

#ifdef RLIMIT_STACK
	    case RLIMIT_STACK:
		/*
		 * Provide a way to set the stack limit to something
		 * smaller, so that we don't lock a lot of unused
		 * stack memory.
		 */
		DPRINT(2, ("ntp_rlimit: STACK: %d %s pages\n",
			   (int)rl_value / rl_scale, rl_sstr));
		if (-1 == getrlimit(RLIMIT_STACK, &rl)) {
			msyslog(LOG_ERR, "CONFIG: getrlimit(RLIMIT_STACK) failed: %m");
		} else {
			if (rl_value > rl.rlim_max) {
				msyslog(LOG_WARNING,
					"CONFIG: ntp_rlimit: using maximum allowed stack limit %lu instead of %lu.",
					(unsigned long)rl.rlim_max,
					(unsigned long)rl_value);
				rl_value = rl.rlim_max;
			}
			rl.rlim_cur = rl_value;
			if (-1 == setrlimit(RLIMIT_STACK, &rl)) {
				msyslog(LOG_ERR,
					"CONFIG: ntp_rlimit: Cannot set RLIMIT_STACK: %m");
			}
		}
		break;
#endif /* RLIMIT_STACK */

	    default:
		INSIST(!"Unexpected setrlimit() case!");
		break;
	}
}
