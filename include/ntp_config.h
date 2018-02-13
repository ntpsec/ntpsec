#ifndef GUARD_NTP_CONFIG_H
#define GUARD_NTP_CONFIG_H

#include <sys/resource.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "ntp_syslog.h"
#include "ntp_fp.h"
#include "ntp.h"
#include "ntp_malloc.h"
#include "ntp_refclock.h"
#include "ntp_stdlib.h"
#include "ntp_machine.h"

/*
 * Configuration file name
 */
#ifndef CONFIG_FILE
#  define	CONFIG_FILE "/etc/ntp.conf"
#endif /* not CONFIG_FILE */
#define CONFIG_DIR	"ntp.d"

/* Limits */
#define MAXLINE 1024

/* Configuration sources */

#define CONF_SOURCE_FILE		0
#define CONF_SOURCE_NTPQ		1

/* list of servers from command line for config_peers() */
extern	int	cmdline_server_count;
extern	char **	cmdline_servers;

typedef struct int_range_tag {
	int	first;
	int	last;
} int_range;

/* Structure for storing an attribute-value pair  */
typedef struct attr_val_tag attr_val;
struct attr_val_tag {
	attr_val *	link;
	int		attr;
	int		type;	/* T_String, T_Integer, ... */
	union val {
		int		i;
		unsigned int	u;
		int_range	r;
		double		d;
		char *		s;
	} value;
};

typedef DECL_FIFO_ANCHOR(attr_val) attr_val_fifo;

/* Structure for nodes on the syntax tree */
typedef struct address_node_tag address_node;
struct address_node_tag {
	address_node *	link;
	char *		address;
	unsigned short	type;	/* family, AF_UNSPEC (0), AF_INET[6] */
};

typedef DECL_FIFO_ANCHOR(address_node) address_fifo;

typedef struct int_node_tag int_node;
struct int_node_tag {
	int_node *	link;
	int		i;
};

typedef DECL_FIFO_ANCHOR(int_node) int_fifo;

typedef struct string_node_tag string_node;
struct string_node_tag {
	string_node *	link;
	char *		s;
};

typedef DECL_FIFO_ANCHOR(string_node) string_fifo;

typedef struct restrict_node_tag restrict_node;
struct restrict_node_tag {
	restrict_node *	link;
	int		mode;	/* restrict or unrestrict? */
	address_node *	addr;
	address_node *	mask;
	int_fifo *	flags;
	int		line_no;
};

typedef DECL_FIFO_ANCHOR(restrict_node) restrict_fifo;

typedef struct peer_node_tag peer_node;
struct peer_node_tag {
	peer_node *	link;
	address_node *	addr;
	int		host_mode;
	struct peer_ctl	ctl;
	struct refclockstat clock_stat;
	char *		group;
};

typedef DECL_FIFO_ANCHOR(peer_node) peer_fifo;

typedef struct unpeer_node_tag unpeer_node;
struct unpeer_node_tag {
	unpeer_node *	link;
	associd_t	assocID;
	address_node *	addr;
};

typedef DECL_FIFO_ANCHOR(unpeer_node) unpeer_fifo;

typedef struct auth_node_tag auth_node;
struct auth_node_tag {
	int		control_key;
	char *		keys;
	attr_val_fifo *	trusted_key_list;
	char *		ntp_signd_socket;
};

typedef struct filegen_node_tag filegen_node;
struct filegen_node_tag {
	filegen_node *	link;
	int		filegen_token;
	attr_val_fifo *	options;
};

typedef DECL_FIFO_ANCHOR(filegen_node) filegen_fifo;

typedef struct setvar_node_tag setvar_node;
struct setvar_node_tag {
	setvar_node *	link;
	char *		var;
	char *		val;
	int		isdefault;
};

typedef DECL_FIFO_ANCHOR(setvar_node) setvar_fifo;

typedef struct nic_rule_node_tag nic_rule_node;
struct nic_rule_node_tag {
	nic_rule_node *	link;
	int		match_class;
	char *		if_name;	/* or numeric address */
	int		action;
};

typedef DECL_FIFO_ANCHOR(nic_rule_node) nic_rule_fifo;

typedef struct addr_opts_node_tag addr_opts_node;
struct addr_opts_node_tag {
	addr_opts_node *link;
	address_node *	addr;
	attr_val_fifo *	options;
};

typedef DECL_FIFO_ANCHOR(addr_opts_node) addr_opts_fifo;

typedef struct sim_node_tag sim_node;
struct sim_node_tag {
	sim_node *		link;
	attr_val_fifo *		init_opts;
};

typedef DECL_FIFO_ANCHOR(sim_node) sim_fifo;

/* The syntax tree */
typedef struct config_tree_tag config_tree;
struct config_tree_tag {
	config_tree *	link;

	attr_val	source;
	time_t		timestamp;

	peer_fifo *	peers;
	unpeer_fifo *	unpeers;

	/* Other Modes */
	attr_val_fifo *	orphan_cmds;	/* s/b renamed tos_options */

	/* Monitoring Configuration */
	int_fifo *	stats_list;
	char *		stats_dir;
	filegen_fifo *	filegen_opts;

	/* Access Control Configuration */
	attr_val_fifo *	discard_opts;
	attr_val_fifo *	mru_opts;
	restrict_fifo *	restrict_opts;

	addr_opts_fifo *fudge;
	attr_val_fifo *	rlimit;
	attr_val_fifo *	tinker;
	attr_val_fifo *	enable_opts;
	attr_val_fifo *	disable_opts;

	auth_node	auth;

	attr_val_fifo *	logconfig;
	string_fifo *	phone;
	setvar_fifo *	setvar;
	attr_val_fifo *	vars;
	nic_rule_fifo *	nic_rules;
	int_fifo *	reset_counters;

	sim_fifo *	sim_details;
	int		mdnstries;
};
extern void init_readconfig(void);
extern void set_keys_file(char*);
extern void set_trustedkey(keyid_t);
extern int mdnstries;


/* Structure for holding a remote configuration command */
struct REMOTE_CONFIG_INFO {
	char buffer[MAXLINE];
	char err_msg[MAXLINE];
	int pos;
	int err_pos;
	int no_errors;
};


/* get text from T_ tokens */
const char * token_name(int token);

/* generic fifo routines for structs linked by 1st member */
void*	append_gen_fifo(void *fifo, void *entry);
void *	concat_gen_fifos(void *first, void *second);
#define APPEND_G_FIFO(pf, pe)		\
	((pf) = append_gen_fifo((pf), (pe)))
#define CONCAT_G_FIFOS(first, second)	\
	((first) = concat_gen_fifos((first), (second)))
#define HEAD_PFIFO(pf)			\
	(((pf) != NULL)			\
	      ? HEAD_FIFO(*(pf))	\
	      : NULL)

address_node *addr_from_typeunit(char *type, int unit);
peer_node *create_peer_node(int hmode, address_node *addr,
			    attr_val_fifo *options);
unpeer_node *create_unpeer_node(address_node *addr);
address_node *create_address_node(char *addr, int type);
void destroy_address_node(address_node *my_node);
attr_val *create_attr_dval(int attr, double value);
attr_val *create_attr_ival(int attr, int value);
attr_val *create_attr_uval(int attr, unsigned int value);
attr_val *create_attr_rangeval(int attr, int first, int last);
attr_val *create_attr_sval(int attr, const char *s);
filegen_node *create_filegen_node(int filegen_token,
				  attr_val_fifo *options);
string_node *create_string_node(char *str);
restrict_node *create_restrict_node(int mode, address_node *addr,
				    address_node *mask,
				    int_fifo *flags, int line_no);
int_node *create_int_node(int val);
addr_opts_node *create_addr_opts_node(address_node *addr,
				      attr_val_fifo *options);
setvar_node *create_setvar_node(char *var, char *val, int isdefault);
nic_rule_node *create_nic_rule_node(int match_class, char *if_name,
				    int action);

extern struct REMOTE_CONFIG_INFO remote_config;
void config_remotely(sockaddr_u *);

extern bool have_interface_option;
extern char *stats_drift_file;	/* name of the driftfile */


void ntp_rlimit(int, rlim_t, int, const char *);

#endif	/* GUARD_NTP_CONFIG_H */
