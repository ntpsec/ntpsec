/* ntp_config.c
 *
 * This file contains the ntpd configuration code.
 *
 * Written By:	Sachin Kamboj
 *		University of Delaware
 *		Newark, DE 19711
 * Some parts borrowed from the older ntp_config.c
 * Copyright (c) 2006
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_NETINFO
# include <netinfo/ni.h>
#endif

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_unixtime.h"
#include "ntp_refclock.h"
#include "ntp_filegen.h"
#include "ntp_stdlib.h"
#include "ntp_assert.h"
#include "ntpsim.h"
#include <ntp_random.h>
#include <isc/net.h>
#include <isc/result.h>

#include <stdio.h>
#include <ctype.h>
#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
#include <signal.h>
#ifndef SIGCHLD
# define SIGCHLD SIGCLD
#endif
#if !defined(VMS)
# ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
# endif
#endif /* VMS */

#ifdef SYS_WINNT
# include <io.h>
HANDLE ResolverEventHandle;
#else
int resolver_pipe_fd[2];  /* used to let the resolver process alert the parent process */
#endif /* SYS_WINNT */

/*
 * [Bug 467]: Some linux headers collide with CONFIG_PHONE and CONFIG_KEYS
 * so #include these later.
 */

#include "ntp_config.h"
#include "ntp_cmdargs.h"

#include "ntp_scanner.h"
#include "ntp_parser.h"
#include "ntp_data_structures.h"


/*
 * "logconfig" building blocks
 */
struct masks {
	const char	  *name;
	unsigned long mask;
};

static struct masks logcfg_class[] = {
	{ "clock",		NLOG_OCLOCK },
	{ "peer",		NLOG_OPEER },
	{ "sync",		NLOG_OSYNC },
	{ "sys",		NLOG_OSYS },
	{ (char *)0,	0 }
};

static struct masks logcfg_item[] = {
	{ "info",		NLOG_INFO },
	{ "allinfo",		NLOG_SYSINFO|NLOG_PEERINFO|NLOG_CLOCKINFO|NLOG_SYNCINFO },
	{ "events",		NLOG_EVENT },
	{ "allevents",		NLOG_SYSEVENT|NLOG_PEEREVENT|NLOG_CLOCKEVENT|NLOG_SYNCEVENT },
	{ "status",		NLOG_STATUS },
	{ "allstatus",		NLOG_SYSSTATUS|NLOG_PEERSTATUS|NLOG_CLOCKSTATUS|NLOG_SYNCSTATUS },
	{ "statistics",		NLOG_STATIST },
	{ "allstatistics",	NLOG_SYSSTATIST|NLOG_PEERSTATIST|NLOG_CLOCKSTATIST|NLOG_SYNCSTATIST },
	{ "allclock",		(NLOG_INFO|NLOG_STATIST|NLOG_EVENT|NLOG_STATUS)<<NLOG_OCLOCK },
	{ "allpeer",		(NLOG_INFO|NLOG_STATIST|NLOG_EVENT|NLOG_STATUS)<<NLOG_OPEER },
	{ "allsys",		(NLOG_INFO|NLOG_STATIST|NLOG_EVENT|NLOG_STATUS)<<NLOG_OSYS },
	{ "allsync",		(NLOG_INFO|NLOG_STATIST|NLOG_EVENT|NLOG_STATUS)<<NLOG_OSYNC },
	{ "all",		NLOG_SYSMASK|NLOG_PEERMASK|NLOG_CLOCKMASK|NLOG_SYNCMASK },
	{ (char *)0,	0 }
};

/* Limits */
#define MAXPHONE	10	/* maximum number of phone strings */
#define MAXPPS		20	/* maximum length of PPS device string */

/*
 * Miscellaneous macros
 */
#define STRSAME(s1, s2)	(*(s1) == *(s2) && strcmp((s1), (s2)) == 0)
#define ISEOL(c)	((c) == '#' || (c) == '\n' || (c) == '\0')
#define ISSPACE(c)	((c) == ' ' || (c) == '\t')
#define STREQ(a, b)	(*(a) == *(b) && strcmp((a), (b)) == 0)

#define KEY_TYPE_MD5	4

/*
 * File descriptor used by the resolver save routines, and temporary file
 * name.
 */
int call_resolver = 1;		/* ntp-genkeys sets this to 0, for example */
#ifndef SYS_WINNT
static char res_file[20];	/* enough for /tmp/ntpXXXXXX\0 */
#define RES_TEMPFILE	"/tmp/ntpXXXXXX"
#else
static char res_file[MAX_PATH];
#endif /* SYS_WINNT */

/*
 * Definitions of things either imported from or exported to outside
 */

int curr_include_level;			/* The current include level */
struct FILE_INFO *fp[MAXINCLUDELEVEL+1];
FILE *res_fp;
struct config_tree my_config;		/* Root of the configuration tree */
#if 0
short default_ai_family = AF_UNSPEC;	/* Default either IPv4 or IPv6 */
#else
short default_ai_family = AF_INET;	/* [Bug 891]: FIX ME */
#endif
char	*sys_phone[MAXPHONE] = {NULL};	/* ACTS phone numbers */
char	default_keysdir[] = NTP_KEYSDIR;
char	*keysdir = default_keysdir;	/* crypto keys directory */
#if defined(HAVE_SCHED_SETSCHEDULER)
int	config_priority_override = 0;
int	config_priority;
#endif

const char *config_file;
char default_ntp_signd_socket[] =
#ifdef NTP_SIGND_PATH
					NTP_SIGND_PATH;
#else
					"";
#endif
char *ntp_signd_socket = default_ntp_signd_socket;
#ifdef HAVE_NETINFO
struct netinfo_config_state *config_netinfo = NULL;
int check_netinfo = 1;
#endif /* HAVE_NETINFO */
#ifdef SYS_WINNT
char *alt_config_file;
LPTSTR temp;
char config_file_storage[MAX_PATH];
char alt_config_file_storage[MAX_PATH];
#endif /* SYS_WINNT */

#ifdef HAVE_NETINFO
/*
 * NetInfo configuration state
 */
struct netinfo_config_state {
	void *domain;		/* domain with config */
	ni_id config_dir;	/* ID config dir      */
	int prop_index;		/* current property   */
	int val_index;		/* current value      */
	char **val_list;       	/* value list         */
};
#endif

struct REMOTE_CONFIG_INFO remote_config;  /* Remote configuration buffer and
					     pointer info */
int input_from_file = 1;     /* A boolean flag, which when set, indicates that
			        the input is to be taken from the configuration
			        file, instead of the remote-configuration buffer
			     */

int old_config_style = 1;    /* A boolean flag, which when set,
			      * indicates that the old configuration
			      * format with a newline at the end of
			      * every command is being used
			      */
int	cryptosw;		/* crypto command called */

extern int sys_maxclock;
extern char *stats_drift_file;	/* name of the driftfile */
extern char *leapseconds_file_name; /*name of the leapseconds file */
#ifdef HAVE_IPTOS_SUPPORT
extern unsigned int qos;				/* QoS setting */
#endif /* HAVE_IPTOS_SUPPORT */

/* FUNCTION PROTOTYPES */

static void call_proto_config_from_list(queue *flag_list, int able_flag);
static void init_auth_node(void);
static void init_syntax_tree(void);
#ifdef DEBUG
       void free_syntax_tree(void);
#endif
double *create_dval(double val);
void destroy_restrict_node(struct restrict_node *my_node);
#if !defined(SIM)
static sockaddr_u *get_next_address(struct address_node *addr);
#endif

static void config_other_modes(void);
static void config_auth(void);
static void config_tos(void);
static void config_monitor(void);
static void config_access(void);
static void config_tinker(void);
static void config_system_opts(void);
static void config_logconfig(void);
static void config_phone(void);
static void config_qos(void);
static void config_setvar(void);
static void config_ttl(void);
static void config_trap(void);
static void config_fudge(void);
static void config_vars(void);
static int is_sane_resolved_address(sockaddr_u *peeraddr, int hmode);
static int get_correct_host_mode(int hmode);
static void config_peers(void);
static void config_unpeers(void);
static void config_ntpd(void);
#ifdef SIM
static void config_sim(void);
static void config_ntpdsim(void);
#endif
void getconfig(int argc,char *argv[]);
enum gnn_type {
	t_UNK,		/* Unknown */
	t_REF,		/* Refclock */
	t_MSK		/* Network Mask */
};

#define DESTROY_QUEUE(q)		\
do {					\
	if (q) {			\
		destroy_queue(q);	\
		(q) = NULL;		\
	}				\
} while (0)

static unsigned long get_pfxmatch(char **s,struct masks *m);
static unsigned long get_match(char *s,struct masks *m);
static unsigned long get_logmask(char *s);
static int getnetnum(const char *num,sockaddr_u *addr, int complain,
		     enum gnn_type a_type);
static int get_multiple_netnums(const char *num, sockaddr_u *addr,
				struct addrinfo **res, int complain,
				enum gnn_type a_type);
static void save_resolve(char *name, int mode, int version,
			 int minpoll, int maxpoll, u_int flags,int ttl,
			 keyid_t keyid,u_char *keystr);
static void abort_resolve(void);
static void do_resolve_internal(void);


/* FUNCTIONS FOR INITIALIZATION
 * ----------------------------
 */

static void
call_proto_config_from_list(
	queue *flag_list,
	int able
	)
{
	int flag;
	struct attr_val *curr_flag;

	while (!empty(flag_list)) {
		curr_flag = (struct attr_val *) dequeue(flag_list);
		flag = curr_flag->value.i;
		if (flag)
			proto_config(flag, able, 0., NULL);
		free_node(curr_flag);
	}
	return;
}

static void
init_auth_node(void)
{
	my_config.auth.autokey = 0;
	my_config.auth.control_key = 0;
	my_config.auth.crypto_cmd_list = NULL;
	my_config.auth.keys = NULL;
	my_config.auth.keysdir = NULL;
	my_config.auth.ntp_signd_socket = NULL;
	my_config.auth.request_key = 0;
	my_config.auth.revoke = 0;
	my_config.auth.trusted_key_list = NULL;
}

static void
init_syntax_tree(void)
{
	my_config.peers = create_queue();
	my_config.unpeers = create_queue();
	my_config.orphan_cmds = create_queue();

	my_config.broadcastclient = 0;
	my_config.manycastserver = create_queue();
	my_config.multicastclient = create_queue();

	my_config.stats_list = create_queue();
	my_config.stats_dir = NULL;
	my_config.filegen_opts = create_queue();

	my_config.discard_opts = create_queue();
	my_config.restrict_opts = create_queue();

	my_config.enable_opts = create_queue();
	my_config.disable_opts = create_queue();
	my_config.tinker = create_queue();
	my_config.fudge = create_queue();

	my_config.logconfig = create_queue();
	my_config.phone = create_queue();
	my_config.qos = create_queue();
	my_config.setvar = create_queue();
	my_config.ttl = create_queue();
	my_config.trap = create_queue();
	my_config.vars = create_queue();
	my_config.sim_details = NULL;

	init_auth_node();

#ifdef DEBUG
	atexit(free_syntax_tree);
#endif
}

#ifdef DEBUG
void
free_syntax_tree(void)
{
	DESTROY_QUEUE(my_config.peers);
	DESTROY_QUEUE(my_config.unpeers);
	DESTROY_QUEUE(my_config.orphan_cmds);

	DESTROY_QUEUE(my_config.manycastserver);
	DESTROY_QUEUE(my_config.multicastclient);

	DESTROY_QUEUE(my_config.stats_list);
	DESTROY_QUEUE(my_config.filegen_opts);

	DESTROY_QUEUE(my_config.discard_opts);
	DESTROY_QUEUE(my_config.restrict_opts);

	DESTROY_QUEUE(my_config.enable_opts);
	DESTROY_QUEUE(my_config.disable_opts);
	DESTROY_QUEUE(my_config.tinker);
	DESTROY_QUEUE(my_config.fudge);

	DESTROY_QUEUE(my_config.logconfig);
	DESTROY_QUEUE(my_config.phone);
	DESTROY_QUEUE(my_config.qos);
	DESTROY_QUEUE(my_config.setvar);
	DESTROY_QUEUE(my_config.ttl);
	DESTROY_QUEUE(my_config.trap);
	DESTROY_QUEUE(my_config.vars);
}
#endif /* DEBUG */

/* FUNCTIONS FOR CREATING NODES ON THE SYNTAX TREE
 * -----------------------------------------------
 */

queue *
enqueue_in_new_queue(
	void *my_node
	)
{
	queue *my_queue = create_queue();

	enqueue(my_queue, my_node);
	return my_queue;
}

struct attr_val *
create_attr_dval(
	int attr,
	double value
	)
{
	struct attr_val *my_val;

	my_val = get_node(sizeof *my_val);
	my_val->attr = attr;
	my_val->value.d = value;
	my_val->type = T_Double;
	return my_val;
}

struct attr_val *
create_attr_ival(
	int attr,
	int value
	)
{
	struct attr_val *my_val;

	my_val = get_node(sizeof *my_val);
	my_val->attr = attr;
	my_val->value.i = value;
	my_val->type = T_Integer;
	return my_val;
}

struct attr_val *
create_attr_sval(
	int attr,
	char *s
	)
{
	struct attr_val *my_val;

	my_val = get_node(sizeof *my_val);
	my_val->attr = attr;
	if (NULL == s)			/* free() hates NULL */
		s = estrdup("");
	my_val->value.s = s;
	my_val->type = T_String;
	return my_val;
}

struct attr_val *
create_attr_pval(
	int attr,
	void *p
	)
{
	struct attr_val *my_val;

	my_val = get_node(sizeof *my_val);
	my_val->attr = attr;
	my_val->value.p = p;
	my_val->type = T_Void;
	return my_val;
}

int *
create_ival(
	int val
	)
{
	int *p = get_node(sizeof *p);

	*p = val;
	return p;
}

double *
create_dval(
	double val
	)
{
	double *p = get_node(sizeof *p);

	*p = val;
	return p;
}

void **
create_pval(
	void *val
	)
{
	void **p = get_node(sizeof *p);

	*p = val;
	return p;
}

struct address_node *
create_address_node(
	char *addr,
	int type
	)
{
	struct address_node *my_node;
	struct isc_netaddr temp_isc_netaddr;

	NTP_REQUIRE(NULL != addr);
	
	my_node = get_node(sizeof *my_node);

	my_node->address = addr;
	if (type)
		my_node->type = type;
	else
		if (is_ip_address(addr, &temp_isc_netaddr)) 
			my_node->type = temp_isc_netaddr.family;
		else 
			my_node->type = default_ai_family;

	return my_node;
}


void
destroy_address_node(
	struct address_node *my_node
	)
{
	NTP_REQUIRE(NULL != my_node);
	NTP_REQUIRE(NULL != my_node->address);

	free(my_node->address);
	free_node(my_node);
}


struct peer_node *
create_peer_node(
	int hmode,
	struct address_node *addr,
	queue *options
	)
{
	struct peer_node* my_node;
	struct attr_val *my_val;
	int errflag = 0;

	my_node = get_node(sizeof *my_node);

	/* Initialze node values to default */
	my_node->minpoll = 0;
	my_node->maxpoll = 0;
	my_node->ttl = 0;
	my_node->peerversion = NTP_VERSION;
	my_node->peerkey = 0;
	my_node->peerflags = 0;
	my_node->bias = 0;

	/* Now set the node to the read values */
	my_node->host_mode = hmode;
	my_node->addr = addr;

	while (NULL != options && !empty(options)) {
		my_val = dequeue(options);

		/* Check the kind of option being set */
		switch(my_val->attr) {

		case T_Minpoll:
			if (my_val->value.i < NTP_MINPOLL) {
				msyslog(LOG_INFO,
					"minpoll: provided value (%d) is below minimum (%d)",
					my_val->value.i, NTP_MINPOLL);
				my_node->minpoll = NTP_MINPOLL;
			}
			else
				my_node->minpoll = my_val->value.i;
			break;

		case T_Maxpoll:
			if (my_val->value.i > NTP_MAXPOLL) {
				msyslog(LOG_INFO,
					"maxpoll: provided value (%d) is above maximum (%d)",
					my_val->value.i, NTP_MAXPOLL);
				my_node->maxpoll = NTP_MAXPOLL;
			}
			else
				my_node->maxpoll = my_val->value.i;
			break;

		case T_Ttl:
			if (my_node->ttl >= MAX_TTL) {
				msyslog(LOG_ERR, "ttl: invalid argument");
				errflag = 1;
			}
			else
				my_node->ttl = my_val->value.i;
			break;

		case T_Mode:
			my_node->ttl = my_val->value.i;
			break;

		case T_Key:
			my_node->peerkey = my_val->value.i;
			break;

		case T_Version:
			my_node->peerversion = my_val->value.i;
			break;

		case T_Flag:
			my_node->peerflags |= my_val->value.i;
			break;

		case T_Bias:
			my_node->bias = my_val->value.d;
			break;

		default:
			msyslog(LOG_ERR, 
				"Unknown peer/server option token %d",
				my_val->attr);
			errflag = 1;
		}
		free_node(my_val);
	}
	DESTROY_QUEUE(options);

	/* Check if errors were reported. If yes, ignore the node */
	if (errflag) {
		free_node(my_node);
		my_node = NULL;
	}
	return my_node;
}

struct unpeer_node *
create_unpeer_node(
	struct address_node *addr
	)
{
	struct unpeer_node* my_node;

	my_node = get_node(sizeof *my_node);

	my_node->addr = addr;

	return my_node;
}

struct filegen_node *
create_filegen_node(
	void **name,
	queue *options
	)
{
	struct filegen_node *my_node;
	
	my_node = get_node(sizeof *my_node);

	my_node->name = (char *) *name;
	free_node(name);

	my_node->options = options;
	return my_node;
}


struct restrict_node *
create_restrict_node(
	struct address_node *addr,
	struct address_node *mask,
	queue *flags,
	int line_no
	)
{
	struct restrict_node *my_node;
	
	my_node = get_node(sizeof *my_node);

	my_node->addr = addr;
	my_node->mask = mask;
	my_node->flags = flags;
	my_node->line_no = line_no;

	return my_node;
}

void
destroy_restrict_node(
	struct restrict_node *my_node
	)
{
	/* With great care, free all the memory occupied by
	 * the restrict node
	 */
	if (my_node->addr)
		destroy_address_node(my_node->addr);
	if (my_node->mask)
		destroy_address_node(my_node->mask);
	DESTROY_QUEUE(my_node->flags);
	free_node(my_node);
}


struct setvar_node *
create_setvar_node(
	char *var,
	char *val,
	u_short def
	)
{
	int len1 = strlen(var);
	int len2 = strlen(val);
	char *s = (char *) emalloc(len1 + len2 + 2);
	struct setvar_node *my_node;

	/* Copy the var = val to s */
	strcpy(s, var);
	s[len1] = '=';
	strcpy(&s[len1 + 1], val);
	s[len1+len2+1] = '\0';

	free(var);
	free(val);

	/* Now store the string and its length into a setvar_node */
	my_node = get_node(sizeof *my_node);
	my_node->data = s;
	my_node->len = len1 + len2 + 2;
	my_node->def = def;
	return my_node;
}

struct addr_opts_node *
create_addr_opts_node(
	struct address_node *addr,
	queue *options
	)
{
	struct addr_opts_node *my_node;

	my_node = get_node(sizeof *my_node);
	my_node->addr = addr;
	my_node->options = options;
	return my_node;
}

script_info *
create_sim_script_info(
	double duration,
	queue *script_queue
	)
{
#ifdef SIM
	return NULL;
#else
	script_info *my_info;
	struct attr_val *my_attr_val;

	my_info = get_node(sizeof *my_info);

	/* Initialize Script Info with default values*/
	my_info->duration = duration;
	my_info->freq_offset = 0;
	my_info->wander = 0;
	my_info->jitter = 0;
	my_info->prop_delay = NET_DLY;
	my_info->proc_delay = PROC_DLY;

	/* Traverse the script_queue and fill out non-default values */
	while (!empty(script_queue)) {
		my_attr_val = dequeue(script_queue);

		/* Set the desired value */
		switch (my_attr_val->attr) {

		case T_Freq_Offset:
			my_info->freq_offset = my_attr_val->value.d;
			break;

		case T_Wander:
			my_info->wander = my_attr_val->value.d;
			break;

		case T_Jitter:
			my_info->jitter = my_attr_val->value.d;
			break;

		case T_Prop_Delay:
			my_info->prop_delay = my_attr_val->value.d;
			break;

		case T_Proc_Delay:
			my_info->proc_delay = my_attr_val->value.d;
			break;

		default:
			msyslog(LOG_ERR, 
				"Unknown script token %d",
				my_attr_val->attr);
		}
		free_node(my_attr_val);
	}
	DESTROY_QUEUE(script_queue);
	return (my_info);
#endif
}


#if !defined(SIM)

#define ADDR_LENGTH 16 + 1

static sockaddr_u *
get_next_address(
	struct address_node *addr
	)
{
	const char addr_prefix[] = "192.168.0.";
	static int curr_addr_no = 1;
	char addr_string[ADDR_LENGTH];
	sockaddr_u *final_addr;
	struct addrinfo *ptr;
	int retval;
	
	final_addr = emalloc(sizeof *final_addr);

	if (addr->type == T_String) {
		snprintf(addr_string, ADDR_LENGTH, "%s%d", addr_prefix, curr_addr_no++);
		printf("Selecting ip address %s for hostname %s\n", addr_string, addr->address);
		retval = getaddrinfo(addr_string, "ntp", NULL, &ptr);
	} else
		retval = getaddrinfo(addr->address, "ntp", NULL, &ptr);

	if (!retval) {
		memcpy(final_addr, ptr->ai_addr, ptr->ai_addrlen);
		fprintf(stderr, "Successful in setting ip address of simulated server to: %s\n", stoa(final_addr));
	}
	else {
		fprintf(stderr, "ERROR!! Could not get a new address\n");
		exit(1);
	}
	freeaddrinfo(ptr);
	return final_addr;
}
#endif /* !SIM */


server_info *
create_sim_server(
	struct address_node *addr,
	double server_offset,
	queue *script
	)
{
#ifdef SIM
	return NULL;
#else
	server_info *my_info;

	my_info = get_node(sizeof *my_info);

	my_info->server_time = server_offset;
	my_info->addr = get_next_address(addr);
	my_info->script = script;
	my_info->curr_script = dequeue(my_info->script);
	return my_info;
#endif /* SIM */
}

struct sim_node *
create_sim_node(
	queue *init_opts,
	queue *servers
	)
{
	struct sim_node *my_node;
	
	my_node = get_node(sizeof *my_node);

	my_node->init_opts = init_opts;
	my_node->servers = servers;
	return my_node;
}


struct key_tok keyword_list[] = {
	{ "automax",		T_Automax,         NO_ARG },
	{ "broadcast",		T_Broadcast,       SINGLE_ARG },
	{ "broadcastclient",	T_Broadcastclient, NO_ARG },
	{ "broadcastdelay",	T_Broadcastdelay,  NO_ARG },
	{ "calldelay",		T_Calldelay,       NO_ARG },
	{ "disable",		T_Disable,         NO_ARG },
	{ "driftfile",		T_Driftfile,       SINGLE_ARG },
	{ "enable",		T_Enable,          NO_ARG },
	{ "end",		T_End,             NO_ARG },
	{ "filegen",		T_Filegen,         NO_ARG },
	{ "fudge",		T_Fudge,           SINGLE_ARG },
	{ "includefile",	T_Includefile,     SINGLE_ARG },
	{ "leapfile",		T_Leapfile,	   SINGLE_ARG },
	{ "logconfig",		T_Logconfig,       MULTIPLE_ARG },
	{ "logfile",		T_Logfile,         SINGLE_ARG },
	{ "manycastclient",	T_Manycastclient,  SINGLE_ARG },
	{ "manycastserver",	T_Manycastserver,  MULTIPLE_ARG },
	{ "multicastclient",	T_Multicastclient, MULTIPLE_ARG },
	{ "peer",		T_Peer,            SINGLE_ARG },
	{ "phone",		T_Phone,           MULTIPLE_ARG },
	{ "pidfile",		T_Pidfile,         SINGLE_ARG },
	{ "pool",		T_Pool,            SINGLE_ARG },
	{ "discard",		T_Discard,         NO_ARG },
	{ "restrict",		T_Restrict,        NO_ARG },
	{ "server",		T_Server,          SINGLE_ARG },
	{ "setvar",		T_Setvar,          NO_ARG },
	{ "statistics",		T_Statistics,      NO_ARG },
	{ "statsdir",		T_Statsdir,        SINGLE_ARG },
	{ "tick",		T_Tick,            NO_ARG },
	{ "tinker",		T_Tinker,          NO_ARG },
	{ "tos",		T_Tos,             NO_ARG },
	{ "trap",		T_Trap,            SINGLE_ARG },
	{ "unconfig",		T_Unconfig,        SINGLE_ARG },
	{ "unpeer",		T_Unpeer,          SINGLE_ARG },

/* authentication_command */
	{ "controlkey",		T_ControlKey,      NO_ARG },
	{ "crypto",		T_Crypto,          NO_ARG },
	{ "keys",		T_Keys,            SINGLE_ARG },
	{ "keysdir",		T_Keysdir,         SINGLE_ARG },
	{ "ntpsigndsocket",	T_NtpSignDsocket,  SINGLE_ARG },
	{ "requestkey",		T_Requestkey,      NO_ARG },
	{ "revoke",		T_Revoke,          NO_ARG },
	{ "trustedkey",		T_Trustedkey,      NO_ARG },
/* IPv4/IPv6 protocol override flag */
	{ "-4",			T_IPv4_flag,	   NO_ARG },
	{ "-6",			T_IPv6_flag,	   NO_ARG },
/* option */
	{ "autokey",		T_Autokey,         NO_ARG },
	{ "bias",		T_Bias,		   NO_ARG },
	{ "burst",		T_Burst,           NO_ARG },
	{ "iburst",		T_Iburst,          NO_ARG },
	{ "key",		T_Key,             NO_ARG },
	{ "maxpoll",		T_Maxpoll,         NO_ARG },
	{ "minpoll",		T_Minpoll,         NO_ARG },
	{ "mode",		T_Mode,            NO_ARG },
	{ "noselect",		T_Noselect,        NO_ARG },
	{ "preempt",		T_Preempt,         NO_ARG },
	{ "true",		T_True,            NO_ARG },
	{ "prefer",		T_Prefer,          NO_ARG },
	{ "ttl",		T_Ttl,             NO_ARG },
	{ "version",		T_Version,         NO_ARG },
	{ "xleave",		T_Xleave,	   NO_ARG },
/* crypto_command */
	{ "host",		T_Host,            SINGLE_ARG },
	{ "ident",		T_Ident,           SINGLE_ARG },
	{ "pw",			T_Pw,              SINGLE_ARG },
	{ "randfile",		T_RandFile,        SINGLE_ARG },
	{ "sign",		T_Sign,            SINGLE_ARG },
/*** MONITORING COMMANDS ***/
/* stat */
	{ "clockstats",		T_Clockstats,      NO_ARG },
	{ "cryptostats",	T_Cryptostats,     NO_ARG },
	{ "loopstats",		T_Loopstats,       NO_ARG },
	{ "peerstats",		T_Peerstats,       NO_ARG },
	{ "rawstats",		T_Rawstats,        NO_ARG },
	{ "sysstats", 		T_Sysstats,        NO_ARG },
	{ "protostats",		T_Protostats,	   NO_ARG },
	{ "timingstats",	T_Timingstats,	   NO_ARG },
/* filegen_option */
	{ "disable",		T_Disable,         NO_ARG },
	{ "enable",		T_Enable,          NO_ARG },
	{ "file",		T_File,            SINGLE_ARG },
	{ "link",		T_Link,            NO_ARG },
	{ "nolink",		T_Nolink,          NO_ARG },
	{ "type",		T_Type,            NO_ARG },
/* filegen_type */
	{ "age",		T_Age,             NO_ARG },
	{ "day",		T_Day,             NO_ARG },
	{ "month",		T_Month,           NO_ARG },
	{ "none",		T_None,            NO_ARG },
	{ "pid",		T_Pid,             NO_ARG },
	{ "week",		T_Week,            NO_ARG },
	{ "year",		T_Year,            NO_ARG },
/*** ORPHAN MODE COMMANDS ***/
/* tos_option */
	{ "minclock",		T_Minclock,        NO_ARG },
	{ "maxclock",		T_Maxclock,        NO_ARG },
	{ "minsane",		T_Minsane,         NO_ARG },
	{ "floor",		T_Floor,           NO_ARG },
	{ "ceiling",		T_Ceiling,         NO_ARG },
	{ "cohort",		T_Cohort,          NO_ARG },
	{ "mindist",		T_Mindist,         NO_ARG },
	{ "maxdist",		T_Maxdist,         NO_ARG },
	{ "maxhop",		T_Maxhop,          NO_ARG },
	{ "beacon",		T_Beacon,          NO_ARG },
	{ "orphan",		T_Orphan,          NO_ARG },
/* access_control_flag */
	{ "default",		T_Default,         NO_ARG },
	{ "flake",		T_Flake,	   NO_ARG },
	{ "ignore",		T_Ignore,          NO_ARG },
	{ "limited",		T_Limited,         NO_ARG },
	{ "kod",		T_Kod,             NO_ARG },
	{ "lowpriotrap",	T_Lowpriotrap,     NO_ARG },
	{ "mask",		T_Mask,            NO_ARG },
	{ "nomodify",		T_Nomodify,        NO_ARG },
	{ "nopeer",		T_Nopeer,          NO_ARG },
	{ "noquery",		T_Noquery,         NO_ARG },
	{ "noserve",		T_Noserve,         NO_ARG },
	{ "notrap",		T_Notrap,          NO_ARG },
	{ "notrust",		T_Notrust,         NO_ARG },
	{ "ntpport",		T_Ntpport,         NO_ARG },
	{ "version",		T_Version,         NO_ARG },
/* discard_option */
	{ "average",		T_Average,         NO_ARG },
	{ "minimum",		T_Minimum,         NO_ARG },
	{ "monitor",		T_Monitor,         NO_ARG },
/* fudge_factor */
	{ "flag1",		T_Flag1,           NO_ARG },
	{ "flag2",		T_Flag2,           NO_ARG },
	{ "flag3",		T_Flag3,           NO_ARG },
	{ "flag4",		T_Flag4,           NO_ARG },
	{ "refid",		T_Refid,           SINGLE_ARG },
	{ "stratum",		T_Stratum,         NO_ARG },
	{ "time1",		T_Time1,           NO_ARG },
	{ "time2",		T_Time2,           NO_ARG },
/* system_option */
	{ "auth",		T_Auth,            NO_ARG },
	{ "bclient",		T_Bclient,         NO_ARG },
	{ "calibrate",		T_Calibrate,       NO_ARG },
	{ "kernel",		T_Kernel,          NO_ARG },
	{ "monitor",		T_Monitor,         NO_ARG },
	{ "ntp",		T_Ntp,             NO_ARG },
	{ "stats",		T_Stats,           NO_ARG },
/* tinker_option */
	{ "step",		T_Step,            NO_ARG },
	{ "panic",		T_Panic,           NO_ARG },
	{ "dispersion",		T_Dispersion,      NO_ARG },
	{ "stepout",		T_Stepout,         NO_ARG },
	{ "allan",		T_Allan,           NO_ARG },
	{ "huffpuff",		T_Huffpuff,        NO_ARG },
	{ "freq",		T_Freq,            NO_ARG },
/* miscellaneous_command */
	{ "port",		T_Port,            NO_ARG },
	{ "interface",		T_Interface,       SINGLE_ARG },
	{ "qos",		T_Qos,		   NO_ARG },
/* simulaor commands */
	{ "simulate",		T_Simulate,        NO_ARG },
	{ "simulation_duration",T_Sim_Duration,	   NO_ARG },
	{ "beep_delay",		T_Beep_Delay,      NO_ARG },
	{ "duration",		T_Duration,        NO_ARG },
	{ "server_offset",	T_Server_Offset,   NO_ARG },
	{ "freq_offset",	T_Freq_Offset,     NO_ARG },
	{ "wander",		T_Wander,          NO_ARG },
	{ "jitter",		T_Jitter,          NO_ARG },
	{ "prop_delay",		T_Prop_Delay,      NO_ARG },
	{ "proc_delay",		T_Proc_Delay,      NO_ARG },
	{ NULL, 0, 0}
};


/* FUNCTIONS FOR PERFORMING THE CONFIGURATION
 * ------------------------------------------
 */

static void
config_other_modes(void)
{

	sockaddr_u addr_sock;
	struct address_node *addr_node;

	if (my_config.broadcastclient) {
		proto_config(PROTO_BROADCLIENT,
			     my_config.broadcastclient, 0., NULL);
		my_config.broadcastclient = 0;
	}

	/* Configure the many-cast servers */
	if (!empty(my_config.manycastserver)) {
		while (!empty(my_config.manycastserver)) {
			addr_node = dequeue(my_config.manycastserver);

			ZERO_SOCK(&addr_sock);
			AF(&addr_sock) = (u_short)addr_node->type;

			if (getnetnum(addr_node->address, &addr_sock,
			    1, t_UNK) == 1)
				proto_config(PROTO_MULTICAST_ADD, 0,
					     0., &addr_sock);

			destroy_address_node(addr_node);
		}
		sys_manycastserver = 1;
	}

	/* Configure the multicast clients */
	if (!empty(my_config.multicastclient)) {
		while (!empty(my_config.multicastclient)) {
			addr_node = dequeue(my_config.multicastclient);

			ZERO_SOCK(&addr_sock);
			AF(&addr_sock) = (u_short)addr_node->type;

			if (getnetnum(addr_node->address, &addr_sock,
			    1, t_UNK) == 1)
				proto_config(PROTO_MULTICAST_ADD, 0,
					     0., &addr_sock);

			destroy_address_node(addr_node);
		}
		proto_config(PROTO_MULTICAST_ADD, 1, 0., NULL);
	}
}


static void
config_auth(void)
{
	u_char rankey[9];
	struct attr_val *my_val;
	int *key_val;
	int i;

	/* Crypto Command */
	while (!empty(my_config.auth.crypto_cmd_list)) {
		my_val = dequeue(my_config.auth.crypto_cmd_list);
#ifdef OPENSSL
		crypto_config(my_val->attr, my_val->value.s);
#endif /* OPENSSL */
		free(my_val->value.s);
		free_node(my_val);
	}
	DESTROY_QUEUE(my_config.auth.crypto_cmd_list);

	/* Keysdir Command */
	if (my_config.auth.keysdir) {
		if (keysdir != default_keysdir)
			free(keysdir);
		keysdir = my_config.auth.keysdir;
		my_config.auth.keysdir = NULL;
	}


	/* ntp_signd_socket Command */
	if (my_config.auth.ntp_signd_socket) {
		if (ntp_signd_socket != default_ntp_signd_socket)
			free(ntp_signd_socket);
		ntp_signd_socket = my_config.auth.ntp_signd_socket;
		my_config.auth.ntp_signd_socket = NULL;
	}

#ifdef OPENSSL
	if (cryptosw) {
		crypto_setup();
		cryptosw = 0;
	}
#endif /* OPENSSL */

	/* Keys Command */
	if (my_config.auth.keys) {
		getauthkeys(my_config.auth.keys);
		free(my_config.auth.keys);
		my_config.auth.keys = NULL;
	}

	/* Control Key Command */
	if (my_config.auth.control_key != 0)
		ctl_auth_keyid = my_config.auth.control_key;

	/* Requested Key Command */
	if (my_config.auth.request_key) {
		DPRINTF(4, ("set info_auth_key to %08lx\n",
			    (u_long) my_config.auth.request_key));
		info_auth_keyid = (keyid_t) my_config.auth.request_key;
	}

	/* Trusted Key Command */
	while (!empty(my_config.auth.trusted_key_list)) {
		key_val = dequeue(my_config.auth.trusted_key_list);
		authtrust(*key_val, 1);
		free_node(key_val);
	}
	DESTROY_QUEUE(my_config.auth.trusted_key_list);

#ifdef OPENSSL
	/* Revoke Command */
	if (my_config.auth.revoke)
		sys_revoke = my_config.auth.revoke;
#endif /* OPENSSL */

#if !defined(VMS) && !defined(SYS_VXWORKS)
	/* find a keyid */
	if (info_auth_keyid == 0)
		req_keyid = 65535;
	else
		req_keyid = info_auth_keyid;

	/* if doesn't exist, make up one at random */
	if (!authhavekey(req_keyid)) {

		for (i = 0; i < (COUNTOF(rankey) - 1); i++)
			do
				rankey[i] = 
					(u_char)(ntp_random() & 0xff);
			while (!rankey[i]);

		rankey[COUNTOF(rankey) - 1] = 0;

		authusekey(req_keyid, KEY_TYPE_MD5, (u_char *)rankey);
		authtrust(req_keyid, 1);
		if (!authhavekey(req_keyid)) {
			msyslog(LOG_ERR, "getconfig: Couldn't generate"
					 " a valid random key!");
			exit(1);
		}
	}

	/* save keyid so we will accept config requests with it */
	info_auth_keyid = req_keyid;
#endif /* !defined(VMS) && !defined(SYS_VXWORKS) */

}

static void
config_tos(void)
{
	struct attr_val *tos;

	while (!empty(my_config.orphan_cmds)) {
		tos = dequeue(my_config.orphan_cmds);
		proto_config(tos->attr, 0, tos->value.d, NULL);
		free_node(tos);
	}
}

static void
config_monitor(void)
{
	char **filegen_string;
	FILEGEN *filegen;
	struct filegen_node *my_node;
	struct attr_val *my_opts;

	char *filegen_file;
	int filegen_type;
	int filegen_flag;

	/* Set the statistics directory */
	if (my_config.stats_dir) {
		stats_config(STATS_STATSDIR, my_config.stats_dir);
		free(my_config.stats_dir);
		my_config.stats_dir = NULL;
	}

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
	while (!empty(my_config.stats_list)) {
		filegen_string = dequeue(my_config.stats_list);
		filegen = filegen_get(*filegen_string);

		DPRINTF(4, ("enabling filegen for %s statistics "
			    "\"%s%s\"\n",
			    *filegen_string, filegen->prefix, 
			    filegen->basename));
		filegen->flag |= FGEN_FLAG_ENABLED;
		free_node(filegen_string);
	}

	/* Configure the statistics with the options */
	while (!empty(my_config.filegen_opts)) {
		my_node = dequeue(my_config.filegen_opts);
		filegen = filegen_get(my_node->name);

		/* Initilize the filegen variables to their pre-configurtion states */
		filegen_flag = filegen->flag;
		filegen_type = filegen->type;
		filegen_file = my_node->name;

		while (!empty(my_node->options)) {
			my_opts = dequeue(my_node->options);

			switch (my_opts->attr) {

			case T_File:
				filegen_file = my_opts->value.p;
				break;

			case T_Type:
				filegen_type = my_opts->value.i;
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
						"Unknown filegen flag "
						"token %d",
						my_opts->value.i);
					exit(1);
				}
				break;
			default:
				msyslog(LOG_ERR,
					"Unknown filegen option token "
					"%d", my_opts->attr);
				exit(1);
			}
			filegen_config(filegen, filegen_file, 
				       filegen_type, filegen_flag);
			free_node(my_opts);
		}
		free_node(my_node);
	}
}


static void
config_access(void)
{
	struct attr_val *	my_opt;
	struct restrict_node *	my_node;
	sockaddr_u		addr_sock;
	sockaddr_u		addr_mask;
	int			flags;
	int			mflags;
	int			restrict_default;

	/* Configure the discard options */
	while (!empty(my_config.discard_opts)) {
		my_opt = dequeue(my_config.discard_opts);

		switch(my_opt->attr) {

		case T_Average:
			ntp_minpoll = my_opt->value.i;
			break;

		case T_Minimum:
			ntp_minpkt = my_opt->value.i;
			break;

		case T_Monitor:
			mon_age = my_opt->value.i;
			break;

		default:
			msyslog(LOG_ERR,
				"Unknown discard option token %d",
				my_opt->attr);
			exit(1);
		}
		free_node(my_opt);
	}

	/* Configure the restrict options */
	while (!empty(my_config.restrict_opts)) {
		my_node = dequeue(my_config.restrict_opts);

		ZERO_SOCK(&addr_sock);

		if (NULL == my_node->addr) {
			/*
			 * The user specified a default rule without a
			 * -4 / -6 qualifier, add to both lists
			 */
			restrict_default = 1;
			ZERO_SOCK(&addr_mask);
		} else {
			restrict_default = 0;
			/* Resolve the specified address */
			AF(&addr_sock) = (u_short)my_node->addr->type;

			if (getnetnum(my_node->addr->address,
				      &addr_sock, 1, t_UNK) != 1) {

				msyslog(LOG_ERR,
					"restrict: error in address '%s' on line %d. Ignoring...",
					my_node->addr->address, my_node->line_no);
				destroy_restrict_node(my_node);
				continue;
			}

			SET_HOSTMASK(&addr_mask, AF(&addr_sock));

			/* Resolve the mask */
			if (my_node->mask) {
				ZERO_SOCK(&addr_mask);
				AF(&addr_mask) = (u_short)my_node->mask->type;
				if (getnetnum(my_node->mask->address, &addr_mask, 1, t_MSK) != 1) {

					msyslog(LOG_ERR,
						"restrict: error in mask '%s' on line %d. Ignoring...",
						my_node->mask->address, my_node->line_no);
					destroy_restrict_node(my_node);
					continue;
				}
			}
		}

		/* Parse the flags */
		flags = 0;
		mflags = 0;

		while (!empty(my_node->flags)) {
			int *curr_flag = dequeue(my_node->flags);
			if (RESM_NTPONLY == *curr_flag)
				mflags |= *curr_flag;
			else
				flags |= *curr_flag;
			free_node(curr_flag);
		}

		/* Set the flags */
		if (restrict_default) {
			AF(&addr_sock) = AF_INET;
			hack_restrict(RESTRICT_FLAGS, &addr_sock, &addr_mask,
				      mflags, flags);

			AF(&addr_sock) = AF_INET6;
		}

		hack_restrict(RESTRICT_FLAGS, &addr_sock, &addr_mask,
			      mflags, flags);

		destroy_restrict_node(my_node);
	}
}


static void
config_tinker(void)
{
	struct attr_val *tinker;

	while (!empty(my_config.tinker)) {
		tinker= (struct attr_val *) dequeue(my_config.tinker);
		loop_config(tinker->attr, tinker->value.d);
		free_node(tinker);
	}
}


static void
config_system_opts(void)
{

	call_proto_config_from_list(my_config.enable_opts, 1);
	call_proto_config_from_list(my_config.disable_opts, 0);

}

static void
config_logconfig(void)
{
	struct attr_val *my_logconfig;

	while(!empty(my_config.logconfig)) {
		my_logconfig = (struct attr_val *)
		    dequeue(my_config.logconfig);
		switch (my_logconfig->attr) {
		    case '+':
			ntp_syslogmask |= get_logmask(my_logconfig->value.s);
			break;
		    case '-':
			ntp_syslogmask &= ~get_logmask(my_logconfig->value.s);
			break;
		    case '=':
			ntp_syslogmask = get_logmask(my_logconfig->value.s);
			break;
		}
		free(my_logconfig->value.s);
		free_node(my_logconfig);
	}
}

static void
config_phone(void)
{
	int i = 0;
	char **s;

	while (!empty(my_config.phone)) {
		s = (char **) dequeue(my_config.phone);
		if (i < MAXPHONE)
			sys_phone[i++] = *s;
		else {
			msyslog(LOG_INFO,
				"phone: Number of phone entries exceeds %d. Ignoring phone %s...",
				MAXPHONE, *s);
			free(*s);
		}
		free_node(s);
	}
	if (i)
		sys_phone[i] = NULL;
}

static void
config_qos(void)
{
	struct attr_val *my_qosconfig;
	char *s;
#ifdef HAVE_IPTOS_SUPPORT
	unsigned int qtos = 0;
#endif

	while(!empty(my_config.qos)) {
		my_qosconfig = (struct attr_val *)
			dequeue(my_config.qos);
		s = my_qosconfig->value.s;
#ifdef HAVE_IPTOS_SUPPORT
		if (!strcmp(s, "lowdelay"))
			qtos = CONF_QOS_LOWDELAY;
		else if (!strcmp(s, "throughput"))
			qtos = CONF_QOS_THROUGHPUT;
		else if (!strcmp(s, "reliability"))
			qtos = CONF_QOS_RELIABILITY;
		else if (!strcmp(s, "mincost"))
			qtos = CONF_QOS_MINCOST;
#ifdef IPTOS_PREC_INTERNETCONTROL
		else if (!strcmp(s, "routine") || !strcmp(s, "cs0"))
			qtos = CONF_QOS_CS0;
		else if (!strcmp(s, "priority") || !strcmp(s, "cs1"))
			qtos = CONF_QOS_CS1;
		else if (!strcmp(s, "immediate") || !strcmp(s, "cs2"))
			qtos = CONF_QOS_CS2;
		else if (!strcmp(s, "flash") || !strcmp(s, "cs3"))
			qtos = CONF_QOS_CS3; 	/* overlapping prefix on keyword */
		if (!strcmp(s, "flashoverride") || !strcmp(s, "cs4"))
			qtos = CONF_QOS_CS4;
		else if (!strcmp(s, "critical") || !strcmp(s, "cs5"))
			qtos = CONF_QOS_CS5;
		else if(!strcmp(s, "internetcontrol") || !strcmp(s, "cs6"))
			qtos = CONF_QOS_CS6;
		else if (!strcmp(s, "netcontrol") || !strcmp(s, "cs7"))
			qtos = CONF_QOS_CS7;
#endif  /* IPTOS_PREC_INTERNETCONTROL */
		if (qtos == 0)
			msyslog(LOG_ERR, "parse error, qos %s not accepted\n", s);
		else
			qos = qtos;
#endif  /* HAVE IPTOS_SUPPORT */
		/*
		 * value is set, but not being effective. Need code to
		 * change   the current connections to notice. Might
		 * also  consider logging a message about the action.
		 * XXX msyslog(LOG_INFO, "QoS %s requested by config\n", s);
		 */
		free(s);
		free_node(my_qosconfig);
	}
}

static void
config_setvar(void)
{
	struct setvar_node *my_node;

	while (!empty(my_config.setvar)) {
		my_node = (struct setvar_node *) dequeue(my_config.setvar);
		set_sys_var(my_node->data, my_node->len, my_node->def);
		free(my_node->data);
		free_node(my_node);
	}
	DESTROY_QUEUE(my_config.setvar);
}

static void
config_ttl(void)
{
	int i = 0;
	int *curr_ttl;

	while (!empty(my_config.ttl)) {
		curr_ttl = (int *) dequeue(my_config.ttl);
		if (i < MAX_TTL)
			sys_ttl[i++] = (u_char)*curr_ttl;
		else
			msyslog(LOG_INFO,
				"ttl: Number of TTL entries exceeds %d. Ignoring TTL %d...",
				MAX_TTL, *curr_ttl);

		free_node(curr_ttl);
	}
	sys_ttlmax = i - 1;
}

static void
config_trap(void)
{

	struct addr_opts_node *curr_trap;
	struct attr_val *curr_opt;
	sockaddr_u addr_sock;
	sockaddr_u peeraddr;
	struct address_node *addr_node;
	struct interface *localaddr;
	u_short port_no;
	int err_flag;

	/* silence warning about addr_sock potentially uninitialized */
	AF(&addr_sock) = AF_UNSPEC;
	port_no = 0;
	localaddr = NULL;


	while (!empty(my_config.trap)) {
		err_flag = 0;
		curr_trap = dequeue(my_config.trap);

		while (!empty(curr_trap->options)) {
			curr_opt = dequeue(curr_trap->options);
			if (T_Port == curr_opt->attr) {
				if (curr_opt->value.i < 1 
				    || curr_opt->value.i > USHRT_MAX) {
					msyslog(LOG_ERR,
						"invalid port number "
						"%d, trap ignored", 
						curr_opt->value.i);
					err_flag = 1;
				}
				port_no = (u_short)curr_opt->value.i;
			}
			else if (T_Interface == curr_opt->attr) {
				addr_node = curr_opt->value.p;

				/* Resolve the interface address */
				ZERO_SOCK(&addr_sock);
				AF(&addr_sock) = (u_short)addr_node->type;

				if (getnetnum(addr_node->address,
					      &addr_sock, 1, t_UNK) != 1) {
					err_flag = 1;
					break;
				}

				localaddr = findinterface(&addr_sock);

				if (NULL == localaddr) {
					msyslog(LOG_ERR,
						"can't find interface "
						"with address %s",
						stoa(&addr_sock));
					err_flag = 1;
				}

				destroy_address_node(addr_node);
			}
			free_node(curr_opt);
		}

		/* Now process the trap for the specified interface
		 * and port number
		 */
		if (!err_flag) {
			ZERO_SOCK(&peeraddr);
			if (1 != getnetnum(curr_trap->addr->address,
					   &peeraddr, 1, t_UNK)) {
				err_flag = 1;
				break;
			}

			/* port is at same location for v4 and v6 */
			SET_PORT(&peeraddr, port_no ? port_no : TRAPPORT);

			if (NULL == localaddr) {
				AF(&peeraddr) = default_ai_family;
				localaddr = ANY_INTERFACE_CHOOSE(&peeraddr);
			}
			else
				AF(&peeraddr) = AF(&addr_sock);

			if (!ctlsettrap(&peeraddr, localaddr, 0,
					NTP_VERSION))
				msyslog(LOG_ERR,
					"can't set trap for %s",
					stoa(&peeraddr));
		}
		DESTROY_QUEUE(curr_trap->options);
		free_node(curr_trap);
	}
}

static void
config_fudge(void)
{
	struct addr_opts_node *curr_fudge;
	struct attr_val *curr_opt;
	sockaddr_u addr_sock;
	struct address_node *addr_node;
	struct refclockstat clock_stat;
	int err_flag;


	while (!empty(my_config.fudge)) {
		curr_fudge = (struct addr_opts_node *) dequeue(my_config.fudge);
		err_flag = 0;

		/* Get the reference clock address and
		 * ensure that it is sane
		 */
		addr_node = curr_fudge->addr;
		ZERO_SOCK(&addr_sock);
		if (getnetnum(addr_node->address, &addr_sock, 1, t_REF) != 1)
			err_flag = 1;

		if (!ISREFCLOCKADR(&addr_sock)) {
			msyslog(LOG_ERR,
				"%s is inappropriate address for the fudge command, line ignored",
				stoa(&addr_sock));
			err_flag = 1;
		}

		/* Parse all the options to the fudge command */
		memset(&clock_stat, 0, sizeof(clock_stat));
		while (!empty(curr_fudge->options)) {
			curr_opt = (struct attr_val *) dequeue(curr_fudge->options);

			/* The attribute field is used to store the flag.
			 * Set haveflags with it
			 */
			clock_stat.haveflags |= curr_opt->attr;
			switch (curr_opt->attr) {
			    case CLK_HAVETIME1:
				clock_stat.fudgetime1 = curr_opt->value.d;
				break;
			    case CLK_HAVETIME2:
				clock_stat.fudgetime2 = curr_opt->value.d;
				break;
			    case CLK_HAVEVAL1:
				clock_stat.fudgeval1 = curr_opt->value.i;
				break;
			    case CLK_HAVEVAL2:
				memcpy(&clock_stat.fudgeval2,
				       curr_opt->value.s,
				       min(strlen(curr_opt->value.s), 4));
				free(curr_opt->value.s);
				break;
			    case CLK_HAVEFLAG1:
				if (curr_opt->value.i)
					clock_stat.flags |= CLK_FLAG1;
				else
					clock_stat.flags &= ~CLK_FLAG1;
				break;
			    case CLK_HAVEFLAG2:
				if (curr_opt->value.i)
					clock_stat.flags |= CLK_FLAG2;
				else
					clock_stat.flags &= ~CLK_FLAG2;
				break;
			    case CLK_HAVEFLAG3:
				if (curr_opt->value.i)
					clock_stat.flags |= CLK_FLAG3;
				else
					clock_stat.flags &= ~CLK_FLAG3;
				break;
			    case CLK_HAVEFLAG4:
				if (curr_opt->value.i)
					clock_stat.flags |= CLK_FLAG4;
				else
					clock_stat.flags &= ~CLK_FLAG4;
				break;
			}

			free_node(curr_opt);
		}

#ifdef REFCLOCK
		if (!err_flag)
			refclock_control(&addr_sock, &clock_stat,
					 (struct refclockstat *)0);
#endif

		DESTROY_QUEUE(curr_fudge->options);
		free_node(curr_fudge);
	}
}

static void
config_vars(void)
{
	struct attr_val *curr_var;
	FILE *new_file;

	while (!empty(my_config.vars)) {
		curr_var = (struct attr_val *) dequeue(my_config.vars);
		/* Determine which variable to set and set it */
		switch (curr_var->attr) {
		    case T_Broadcastdelay:
			proto_config(PROTO_BROADDELAY, 0, curr_var->value.d, NULL);
			break;
		    case T_Calldelay:
			proto_config(PROTO_CALLDELAY, curr_var->value.i, 0, NULL);
			break;
		    case T_Tick:
			proto_config(PROTO_ADJ, 0, curr_var->value.d, NULL);
			break;
		    case T_Driftfile:
			if (!strcmp(curr_var->value.s, "\0")) {
				stats_drift_file = 0;
				msyslog(LOG_INFO, "config: driftfile disabled\n");
			} else {
				stats_config(STATS_FREQ_FILE, curr_var->value.s);
				free(curr_var->value.s);
			}
			break;
		    case T_WanderThreshold:
			wander_threshold = curr_var->value.d;
			break;
		    case T_Leapfile:
			stats_config(STATS_LEAP_FILE, curr_var->value.s);
			free(curr_var->value.s);
			break;
		    case T_Pidfile:
			stats_config(STATS_PID_FILE, curr_var->value.s);
			free(curr_var->value.s);
			break;
		    case T_Logfile:
			new_file = fopen(curr_var->value.s, "a");
			if (new_file != NULL) {
				NLOG(NLOG_SYSINFO) /* conditional if clause for conditional syslog */
				    msyslog(LOG_NOTICE, "logging to file %s", curr_var->value.s);
				if (syslog_file != NULL &&
				    fileno(syslog_file) != fileno(new_file))
					(void)fclose(syslog_file);

				syslog_file = new_file;
				syslogit = 0;
			}
			else
				msyslog(LOG_ERR,
					"Cannot open log file %s",
					curr_var->value.s);
			free(curr_var->value.s);
			break;
#ifdef OPENSSL
		    case T_Automax:
			sys_automax = curr_var->value.i;
			break;
#endif
		}
		free_node(curr_var);
	}
}

/* Define a function to check if a resolved address is sane.
 * If yes, return 1, else return 0;
 */
static int
is_sane_resolved_address(
	sockaddr_u *	peeraddr,
	int		hmode
	)
{
	if (!ISREFCLOCKADR(peeraddr) && ISBADADR(peeraddr)) {
		msyslog(LOG_ERR,
			"attempt to configure invalid address %s",
			stoa(peeraddr));
		return 0;
	}
	/*
	 * Shouldn't be able to specify multicast
	 * address for server/peer!
	 * and unicast address for manycastclient!
	 */
	if ((T_Server == hmode || T_Peer == hmode || T_Pool == hmode)
	    && IS_MCAST(peeraddr)) {
		msyslog(LOG_ERR,
			"attempt to configure invalid address %s",
			stoa(peeraddr));
		return 0;
	}
	if (T_Manycastclient == hmode && !IS_MCAST(peeraddr)) {
		msyslog(LOG_ERR,
			"attempt to configure invalid address %s",
			stoa(peeraddr));
		return 0;
	}

	if (IS_IPV6(peeraddr) && !ipv6_works)
		return 0;

	/* Ok, all tests succeeded, now we can return 1 */
	return 1;
}

static int
get_correct_host_mode(
	int hmode
	)
{
	switch (hmode) {
	    case T_Server:
	    case T_Pool:
	    case T_Manycastclient:
		return MODE_CLIENT;
		break;
	    case T_Peer:
		return MODE_ACTIVE;
		break;
	    case T_Broadcast:
		return MODE_BROADCAST;
		break;
	    default:
		fprintf(stderr, "Fatal error in client_type in ntp_config.y");
		exit(1);
		break;
	}
}

static void
config_peers(void)
{
	struct addrinfo *res, *res_bak;
	sockaddr_u peeraddr;
	struct peer_node *curr_peer;
	int hmode;
	int status;
	int no_needed;
	int i;

	while (!empty(my_config.peers)) {
		curr_peer = (struct peer_node *) dequeue(my_config.peers);

		/* Find the number of associations needed.
		 * If a pool coomand is specified, then sys_maxclock needed
		 * else, only one is needed
		 */
		no_needed = (curr_peer->host_mode == T_Pool) ? sys_maxclock : 1;

		/* Find the correct host-mode */
		hmode = get_correct_host_mode(curr_peer->host_mode);

		/* Attempt to resolve the address */
		ZERO_SOCK(&peeraddr);
		AF(&peeraddr) = (u_short)curr_peer->addr->type;

		status = get_multiple_netnums(curr_peer->addr->address, &peeraddr, &res, 0, t_UNK);

		/* I don't know why getnetnum would return -1.
		 * The old code had this test, so I guess it must be
		 * useful
		 */
		if (status == -1) {
			/* Do nothing, apparently we found an IPv6
			 * address and can't do anything about it */
		}
		/* Check if name resolution failed. If yes, store the
		 * peer information in a file for asynchronous
		 * resolution later
		 */
		else if (status != 1) {
			save_resolve(curr_peer->addr->address,
				     hmode,
				     curr_peer->peerversion,
				     curr_peer->minpoll,
				     curr_peer->maxpoll,
				     curr_peer->peerflags,
				     curr_peer->ttl,
				     curr_peer->peerkey,
				     (u_char *)"*");
		}
		/* Yippie!! Name resolution has succeeded!!!
		 * Now we can proceed to some more sanity checks on
		 * the resolved address before we start to configure
		 * the peer
		 */
		else {
			res_bak = res;

			/*
			 * Loop to configure the desired number of
			 * associations
			 */
			for (i = 0; (i < no_needed) && res; res =
			    res->ai_next) {
				++i;
				memcpy(&peeraddr, res->ai_addr,
				    res->ai_addrlen);
				if (is_sane_resolved_address(
					&peeraddr,
					curr_peer->host_mode))

					peer_config(&peeraddr,
					ANY_INTERFACE_CHOOSE(&peeraddr),
					    hmode,
					    curr_peer->peerversion,
					    curr_peer->minpoll,
					    curr_peer->maxpoll,
					    curr_peer->peerflags,
					    curr_peer->ttl,
					    curr_peer->peerkey,
					    (u_char *)"*");
			}
			freeaddrinfo(res_bak);
		}

		/* Ok, everything done. Free up peer node memory */
		destroy_address_node(curr_peer->addr);
		free_node(curr_peer);
	}
}

static void
config_unpeers(void)
{
	struct addrinfo *res, *res_bak;
	sockaddr_u peeraddr;
	struct unpeer_node *curr_unpeer;
	struct peer *peer;
	int status;
	int found;

	while (!empty(my_config.unpeers)) {
		curr_unpeer = (struct unpeer_node *) dequeue(my_config.unpeers);

		/* Attempt to resolve the address */
		ZERO_SOCK(&peeraddr);
		AF(&peeraddr) = (u_short)curr_unpeer->addr->type;

		status = get_multiple_netnums(curr_unpeer->addr->address, &peeraddr, &res, 0, t_UNK);

		/* I don't know why getnetnum would return -1.
		 * The old code had this test, so I guess it must be
		 * useful
		 */
		if (status == -1) {
			/* Do nothing, apparently we found an IPv6
			 * address and can't do anything about it */
		}
		/* Check if name resolution failed. If yes, throw
		 * up our hands.
		 */
		else if (status != 1) {
			/* Do nothing */
		}
		/* Yippie!! Name resolution has succeeded!!!
		 */
		else {
			res_bak = res;

			/*
			 * Loop through the addresses found
			 */
			while (res) {
				memcpy(&peeraddr, res->ai_addr, res->ai_addrlen);

				found = 0;
				peer = NULL;

				DPRINTF(1, ("searching for %s\n", stoa(&peeraddr)));

				while (!found) {
					peer = findexistingpeer(&peeraddr, peer, -1);
					if (!peer)
						break;
					if (peer->flags & FLAG_CONFIG)
						found = 1;
				}

				if (found) {
					peer_clear(peer, "GONE");
					unpeer(peer);
				}

				res = res->ai_next;
			}
			freeaddrinfo(res_bak);
		}

		/* Ok, everything done. Free up peer node memory */
		free(curr_unpeer->addr->address);
		free_node(curr_unpeer->addr);
		free_node(curr_unpeer);
	}
}

#ifdef SIM
static void
config_sim(void)
{
	int i;
	server_info *serv_info;
	struct attr_val *init_stmt;

	/* Check if a simulate block was found in the configuration code.
	 * If not, return an error and exit
	 */
	if (NULL == my_config.sim_details) {
		fprintf(stderr, "ERROR!! I couldn't find a \"simulate\" block for configuring the simulator.\n");
		fprintf(stderr, "\tCheck your configuration file.\n");
		exit(1);
	}

	/* Process the initialization statements
	 * -------------------------------------
	 */
	while (!empty(my_config.sim_details->init_opts)) {
		init_stmt = dequeue(my_config.sim_details->init_opts);

		switch(init_stmt->attr) {

		case T_Beep_Delay:
			simulation.beep_delay = init_stmt->value.d;
			break;

		case T_Sim_Duration:
			simulation.end_time = init_stmt->value.d;
			break;

		default:
			fprintf(stderr,
				"Unknown simulator init token %d\n",
				init_stmt->attr);
			exit(1);
		}
		free_node(init_stmt);
	}
	DESTROY_QUEUE(my_config.sim_details->init_opts);


	/* Process the server list
	 * -----------------------
	 */
	simulation.num_of_servers = 
		get_no_of_elements(my_config.sim_details->servers);
	simulation.servers = emalloc(simulation.num_of_servers 
				     * sizeof(server_info));

	for (i = 0;i < simulation.num_of_servers; i++) {
		serv_info = dequeue(my_config.sim_details->servers);
		if (NULL == serv_info) {
			fprintf(stderr, "Simulator server list is corrupt\n");
			exit(1);
		} else {
			memcpy(&simulation.servers[i], serv_info, sizeof(server_info));
			free_node(serv_info);
		}
	}
	DESTROY_QUEUE(my_config.sim_details->servers);

	/* Free the sim_node memory and set the sim_details as NULL */
	free_node(my_config.sim_details);
	my_config.sim_details = NULL;

	/* Create server associations */
	printf("Creating server associations\n");
	create_server_associations();
	fprintf(stderr,"\tServer associations successfully created!!\n");
}
#endif /* SIM */

/* Define two different config functions. One for the daemon and the other for
 * the simulator. The simulator ignores a lot of the standard ntpd configuration
 * options
 */

static void
config_ntpd(void)
{
	config_monitor();
	config_auth();
	config_tos();
	config_access();
	config_tinker();
	config_system_opts();
	config_logconfig();
	config_phone();
	config_setvar();
	config_ttl();
	config_trap();
	config_vars();
	config_other_modes();
	config_peers();
	config_unpeers();
	config_fudge();
	config_qos();
}

#ifdef SIM
static void
config_ntpdsim(void)
{
	printf("Configuring Simulator...\n");
	printf("Some ntpd-specific commands in the configuration file will be ignored.\n");

	config_tos();
	config_monitor();
	config_tinker();
	config_system_opts();
	config_logconfig();
	config_vars();
	config_sim();
}
#endif /* SIM */

void
config_remotely(void)
{
	input_from_file = 0;

	key_scanner = create_keyword_scanner(keyword_list);
	yyparse();
	delete_keyword_scanner(key_scanner);
	key_scanner = NULL;

	DPRINTF(1, ("Finished Parsing!!\n"));

	config_ntpd();

	input_from_file = 1;
}


/* ACTUAL getconfig code */

void
getconfig(
	int argc,
	char *argv[]
	)
{
	char line[MAXLINE];

#ifndef SYS_WINNT
	config_file = CONFIG_FILE;
#else
	temp = CONFIG_FILE;
	if (!ExpandEnvironmentStrings((LPCTSTR)temp, (LPTSTR)config_file_storage, (DWORD)sizeof(config_file_storage))) {
		msyslog(LOG_ERR, "ExpandEnvironmentStrings CONFIG_FILE failed: %m\n");
		exit(1);
	}
	config_file = config_file_storage;

	temp = ALT_CONFIG_FILE;
	if (!ExpandEnvironmentStrings((LPCTSTR)temp, (LPTSTR)alt_config_file_storage, (DWORD)sizeof(alt_config_file_storage))) {
		msyslog(LOG_ERR, "ExpandEnvironmentStrings ALT_CONFIG_FILE failed: %m\n");
		exit(1);
	}
	alt_config_file = alt_config_file_storage;

#endif /* SYS_WINNT */
	res_fp = NULL;
	ntp_syslogmask = NLOG_SYNCMASK; /* set more via logconfig */

	/*
	 * install a non default variable with this daemon version
	 */
	(void) sprintf(line, "daemon_version=\"%s\"", Version);
	set_sys_var(line, strlen(line)+1, RO);

	/*
	 * Say how we're setting the time of day
	 */
	(void) sprintf(line, "settimeofday=\"%s\"", set_tod_using);
	set_sys_var(line, strlen(line)+1, RO);

	/*
	 * Initialize the loop.
	 */
	loop_config(LOOP_DRIFTINIT, 0.);

	getCmdOpts(argc, argv);

	curr_include_level = 0;
	if (
		(fp[curr_include_level] = F_OPEN(FindConfig(config_file), "r")) == NULL
#ifdef HAVE_NETINFO
		/* If there is no config_file, try NetInfo. */
		&& check_netinfo && !(config_netinfo = get_netinfo_config())
#endif /* HAVE_NETINFO */
		) {
		fprintf(stderr, "getconfig: Couldn't open <%s>\n", FindConfig(config_file));
		msyslog(LOG_INFO, "getconfig: Couldn't open <%s>", FindConfig(config_file));
#ifdef SYS_WINNT
		/* Under WinNT try alternate_config_file name, first NTP.CONF, then NTP.INI */

		if ((fp[curr_include_level] = F_OPEN(FindConfig(alt_config_file), "r")) == NULL) {

			/*
			 * Broadcast clients can sometimes run without
			 * a configuration file.
			 */

			fprintf(stderr, "getconfig: Couldn't open <%s>\n", FindConfig(alt_config_file));
			msyslog(LOG_INFO, "getconfig: Couldn't open <%s>", FindConfig(alt_config_file));
			return;
		}
#else  /* not SYS_WINNT */
		return;
#endif /* not SYS_WINNT */
	}

	/*** BULK OF THE PARSER ***/
	ip_file = fp[curr_include_level];
	init_syntax_tree();
	key_scanner = create_keyword_scanner(keyword_list);
	yyparse();
	delete_keyword_scanner(key_scanner);
	key_scanner = NULL;

	DPRINTF(1, ("Finished Parsing!!\n"));

	/* The actual configuration done depends on whether we are configuring the
	 * simulator or the daemon. Perform a check and call the appropriate
	 * function as needed.
	 */

#ifndef SIM
	config_ntpd();
#else
	config_ntpdsim();
#endif

	while (curr_include_level != -1) {
		FCLOSE(fp[curr_include_level--]);
	}

#ifdef HAVE_NETINFO
	if (config_netinfo)
		free_netinfo_config(config_netinfo);
#endif /* HAVE_NETINFO */

	/*
	printf("getconfig: res_fp <%p> call_resolver: %d", res_fp, call_resolver);
	*/

	if (res_fp != NULL) {
		if (call_resolver) {
			/*
			 * Need name resolution
			 */
			do_resolve_internal();
		}
	}
}



/* FUNCTIONS COPIED FROM THE OLDER ntp_config.c
 * --------------------------------------------
 */


/*
 * get_pfxmatch - find value for prefixmatch
 * and update char * accordingly
 */
static unsigned long
get_pfxmatch(
	char ** s,
	struct masks *m
	)
{
	while (m->name) {
		if (strncmp(*s, m->name, strlen(m->name)) == 0) {
			*s += strlen(m->name);
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
static unsigned long
get_match(
	char *s,
	struct masks *m
	)
{
	while (m->name) {
		if (strcmp(s, m->name) == 0) {
			return m->mask;
		} else {
			m++;
		}
	}
	return 0;
}

/*
 * get_logmask - build bitmask for ntp_syslogmask
 */
static unsigned long
get_logmask(
	char *s
	)
{
	char *t;
	unsigned long offset;
	unsigned long mask;

	t = s;
	offset = get_pfxmatch(&t, logcfg_class);
	mask   = get_match(t, logcfg_item);

	if (mask)
		return mask << offset;
	else
		msyslog(LOG_ERR, "logconfig: illegal argument %s - ignored", s);

	return 0;
}


#ifdef HAVE_NETINFO

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

       	config = (struct netinfo_config_state *)malloc(sizeof(struct netinfo_config_state));
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
	       	for (; prop_index < (sizeof(keywords)/sizeof(keywords[0])); prop_index++)
	       	{
		       	ni_namelist namelist;
			struct keyword current_prop = keywords[prop_index];

			/*
			 * For each value associated in the property, we're going to return
			 * a separate line. We squirrel away the values in the config state
			 * so the next time through, we don't need to do this lookup.
			 */
		       	NI_INIT(&namelist);
	       		if (ni_lookupprop(config->domain, &config->config_dir, current_prop.text, &namelist) == NI_OK) {
				ni_index index;

				/* Found the property, but it has no values */
				if (namelist.ni_namelist_len == 0) continue;

				if (! (val_list = config->val_list = (char**)malloc(sizeof(char*) * (namelist.ni_namelist_len + 1))))
				{ msyslog(LOG_ERR, "out of memory while configuring"); break; }

				for (index = 0; index < namelist.ni_namelist_len; index++) {
					char *value = namelist.ni_namelist_val[index];

					if (! (val_list[index] = (char*)malloc(strlen(value)+1)))
					{ msyslog(LOG_ERR, "out of memory while configuring"); break; }

					strcpy(val_list[index], value);
				}
				val_list[index] = NULL;

				break;
			}
			ni_namelist_free(&namelist);
		}
		config->prop_index = prop_index;
	}

	/* No list; we're done here. */
       	if (!val_list) return CONFIG_UNKNOWN;

	/*
	 * We have a list of values for the current property.
	 * Iterate through them and return each in order.
	 */
	if (val_list[val_index])
	{
		int ntok = 1;
		int quoted = 0;
		char *tokens = val_list[val_index];

		msyslog(LOG_INFO, "%s %s", keywords[prop_index].text, val_list[val_index]);

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
				while (ISSPACE(*tokens)) tokens++;
				if (ISEOL(*tokens)) break;
			}
		}

		if (ntok == MAXTOKENS) {
			/* HMS: chomp it to lose the EOL? */
			msyslog(LOG_ERR,
				"gettokens_netinfo: too many tokens.  Ignoring: %s",
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
       	free(val_list);	val_list = config->val_list = NULL; val_index = config->val_index = 0;

	goto again;
}

#endif /* HAVE_NETINFO */

/*
 * getnetnum - return a net number (this is crude, but careful)
 *
 * returns 1 for success, and mysteriously, 0 or -1 for failure
 */
static int
getnetnum(
	const char *num,
	sockaddr_u *addr,
	int complain,
	enum gnn_type a_type
	)
{
	int retval;
	struct addrinfo *res;

	/* Get all the addresses that resolve to this name */
	retval = get_multiple_netnums(num, addr, &res, complain, a_type);

	if (retval != 1) {
		/* Name resolution failed */
		return retval;
	}

	memcpy(addr, res->ai_addr, res->ai_addrlen);

	DPRINTF(2, ("getnetnum given %s, got %s\n", num, stoa(addr)));

	freeaddrinfo(res);
	return 1;
}

/*
 * get_multiple_netnums
 *
 * returns 1 for success, and mysteriously, 0 or -1 for failure
 */
static int
get_multiple_netnums(
	const char *num,
	sockaddr_u *addr,
	struct addrinfo **res,
	int complain,
	enum gnn_type a_type
	)
{
	struct addrinfo hints;
	struct addrinfo *ptr;

	int retval;

	/* Get host address. Looking for UDP datagram connection */
	memset(&hints, 0, sizeof(hints));
	if (IS_IPV4(addr) || IS_IPV6(addr))
		hints.ai_family = AF(addr);
	else
		hints.ai_family = AF_UNSPEC;
	/*
	 * If we don't have an IPv6 stack, just look up IPv4 addresses
	 */
	if (!ipv6_works)
		hints.ai_family = AF_INET;

	hints.ai_socktype = SOCK_DGRAM;

	DPRINTF(4, ("getaddrinfo %s\n", num));

	retval = getaddrinfo(num, "ntp", &hints, &ptr);

	if (retval || (AF_INET6 == ptr->ai_family && !ipv6_works)) {
		if (complain)
			msyslog(LOG_ERR,
				"getaddrinfo: \"%s\" invalid host "
				"address, ignored",
				num);
		else
			DPRINTF(1, ("getaddrinfo: \"%s\" invalid host "
				    "address.\n",
				    num));

		if (!retval)
			return -1;
		else 
			return 0;
	}
	*res = ptr;

	return 1;
}



#if !defined(VMS) && !defined(SYS_WINNT)
/*
 * catchchild - receive the resolver's exit status
 */
static RETSIGTYPE
catchchild(
	int sig
	)
{
	/*
	 * We only start up one child, and if we're here
	 * it should have already exited.  Hence the following
	 * shouldn't hang.  If it does, please tell me.
	 */
#if !defined (SYS_WINNT) && !defined(SYS_VXWORKS)
	(void) wait(0);
#endif /* SYS_WINNT  && VXWORKS*/
}
#endif /* VMS */


/*
 * save_resolve - save configuration info into a file for later name resolution
 */
static void
save_resolve(
	char *name,
	int mode,
	int version,
	int minpoll,
	int maxpoll,
	u_int flags,
	int ttl,
	keyid_t keyid,
	u_char *keystr
	)
{
#ifndef SYS_VXWORKS
	if (res_fp == NULL) {
#ifndef SYS_WINNT
		strcpy(res_file, RES_TEMPFILE);
#else
		int len;

		/* no /tmp directory under NT */
		if (!GetTempPath(sizeof res_file, res_file)) {
			msyslog(LOG_ERR, "can not get temp dir: %m");
			exit(1);
		}
		
		len = strlen(res_file);
		if (sizeof res_file < len + sizeof "ntpdXXXXXX") {
			msyslog(LOG_ERR,
				"temporary directory path %s too long",
				res_file);
			exit(1);
		}

		memmove(res_file + len, "ntpdXXXXXX",
			sizeof "ntpdXXXXXX");
#endif /* SYS_WINNT */
#ifdef HAVE_MKSTEMP
		{
			int fd;

			res_fp = NULL;
			if ((fd = mkstemp(res_file)) != -1)
				res_fp = fdopen(fd, "r+");
		}
#else
		mktemp(res_file);
		res_fp = fopen(res_file, "w");
#endif
		if (res_fp == NULL) {
			msyslog(LOG_ERR, "open failed for %s: %m", res_file);
			return;
		}
	}
#ifdef DEBUG
	if (debug) {
		printf("resolving %s\n", name);
	}
#endif

	(void)fprintf(res_fp, "%s %d %d %d %d %d %d %u %s\n", name,
		      mode, version, minpoll, maxpoll, flags, ttl, keyid, keystr);
#ifdef DEBUG
	if (debug > 1)
		printf("config: %s %d %d %d %d %x %d %u %s\n", name, mode,
		       version, minpoll, maxpoll, flags, ttl, keyid, keystr);
#endif

#else  /* SYS_VXWORKS */
	/* save resolve info to a struct */
#endif /* SYS_VXWORKS */
}


/*
 * abort_resolve - terminate the resolver stuff and delete the file
 */
static void
abort_resolve(void)
{
	/*
	 * In an ideal world we would might reread the file and
	 * log the hosts which aren't getting configured.  Since
	 * this is too much work, however, just close and delete
	 * the temp file.
	 */
	if (res_fp != NULL)
		(void) fclose(res_fp);
	res_fp = NULL;

#ifndef SYS_VXWORKS		/* we don't open the file to begin with */
#if !defined(VMS)
	(void) unlink(res_file);
#else
	(void) delete(res_file);
#endif /* VMS */
#endif /* SYS_VXWORKS */
}


/*
 * do_resolve_internal - start up the resolver function (not program)
 */
/*
 * On VMS, this routine will simply refuse to resolve anything.
 *
 * Possible implementation: keep `res_file' in memory, do async
 * name resolution via QIO, update from within completion AST.
 * I'm unlikely to find the time for doing this, though. -wjm
 */
static void
do_resolve_internal(void)
{
	int i;

	if (res_fp == NULL) {
		/* belch */
		msyslog(LOG_ERR,
			"do_resolve_internal: Fatal: res_fp == NULL");
		exit(1);
	}

	/* we are done with this now */
	(void) fclose(res_fp);
	res_fp = NULL;

#if !defined(VMS) && !defined (SYS_VXWORKS)
	req_file = res_file;	/* set up pointer to res file */
#ifndef SYS_WINNT
	(void) signal_no_reset(SIGCHLD, catchchild);

#ifndef SYS_VXWORKS
	/* the parent process will write to the pipe
	 * in order to wake up to child process
	 * which may be waiting in a select() call
	 * on the read fd */
	if (pipe(resolver_pipe_fd) < 0) {
		msyslog(LOG_ERR,
			"unable to open resolver pipe");
		exit(1);
	}

	i = fork();
	/* Shouldn't the code below be re-ordered?
	 * I.e. first check if the fork() returned an error, then
	 * check whether we're parent or child.
	 *     Martin Burnicki
	 */
	if (i == 0) {
		/*
		 * this used to close everything
		 * I don't think this is necessary
		 */
		/*
		 * To the unknown commenter above:
		 * Well, I think it's better to clean up
		 * after oneself. I have had problems with
		 * refclock-io when intres was running - things
		 * where fine again when ntpintres was gone.
		 * So some systems react erratic at least.
		 *
		 *			Frank Kardel
		 *
		 * 94-11-16:
		 * Further debugging has proven that the above is
		 * absolutely harmful. The internal resolver
		 * is still in the SIGIO process group and the lingering
		 * async io information causes it to process requests from
		 * all file decriptor causing a race between the NTP daemon
		 * and the resolver. which then eats data when it wins 8-(.
		 * It is absolutly necessary to kill any IO associations
		 * shared with the NTP daemon.
		 *
		 * We also block SIGIO (currently no ports means to
		 * disable the signal handle for IO).
		 *
		 * Thanks to wgstuken@informatik.uni-erlangen.de to notice
		 * that it is the ntp-resolver child running into trouble.
		 *
		 * THUS:
		 */

		/*
		msyslog(LOG_INFO, "do_resolve_internal: pre-closelog");
		*/
		closelog();
		kill_asyncio(0);

		(void) signal_no_reset(SIGCHLD, SIG_DFL);

#ifdef DEBUG
		if (0)
			debug = 2;
#endif

		init_logging("ntpd_intres", 0);
		setup_logfile();
		/*
		msyslog(LOG_INFO, "do_resolve_internal: post-closelog");
		*/

		ntp_intres();

		/*
		 * If we got here, the intres code screwed up.
		 * Print something so we don't die without complaint
		 */
		msyslog(LOG_ERR, "call to ntp_intres lost");
		abort_resolve();
		exit(1);
	}
#else
	/* vxWorks spawns a thread... -casey */
	i = sp (ntp_intres);
	/*i = taskSpawn("ntp_intres",100,VX_FP_TASK,20000,ntp_intres);*/
#endif
	if (i == -1) {
		msyslog(LOG_ERR, "fork() failed, can't start ntp_intres: %m");
		(void) signal_no_reset(SIGCHLD, SIG_DFL);
		abort_resolve();
	}
	else {
		/* This is the parent process who will write to the pipe,
		 * so we close the read fd */
		close(resolver_pipe_fd[0]);
	}
#else /* SYS_WINNT */
	{
		/* NT's equivalent of fork() is _spawn(), but the start point
		 * of the new process is an executable filename rather than
		 * a function name as desired here.
		 */
		unsigned thread_id;
		uintptr_t res_thd_handle;

		fflush(stdout);
		ResolverEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (ResolverEventHandle == NULL) {
			msyslog(LOG_ERR, "Unable to create resolver event object, can't start ntp_intres");
			abort_resolve();
		}
		res_thd_handle = _beginthreadex(
			NULL,			/* no security attributes	*/
			0,			/* use default stack size	*/
			ntp_intres_thread,	/* thread function		*/
			NULL,			/* argument to thread function	*/
			0,			/* use default creation flags	*/
			&thread_id);		/* receives thread identifier	*/
		if (!res_thd_handle) {
			msyslog(LOG_ERR, "_beginthreadex ntp_intres_thread failed %m");
			CloseHandle(ResolverEventHandle);
			ResolverEventHandle = NULL;
			abort_resolve();
		}
	}
#endif /* SYS_WINNT */
#else /* VMS  VX_WORKS */
	msyslog(LOG_ERR,
		"Name resolution not implemented for VMS - use numeric addresses");
	abort_resolve();
#endif /* VMS VX_WORKS */
}
