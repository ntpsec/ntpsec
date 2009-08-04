#ifndef __NTP_CONFIG_H__
#define __NTP_CONFIG_H__

#include "ntp_data_structures.h"
#include "ntpsim.h"


/*
 * Configuration file name
 */
#ifndef CONFIG_FILE
# ifndef SYS_WINNT
#  define	CONFIG_FILE "/etc/ntp.conf"
# else /* SYS_WINNT */
#  define	CONFIG_FILE	"%windir%\\system32\\drivers\\etc\\ntp.conf"
#  define	ALT_CONFIG_FILE "%windir%\\ntp.conf"
#  define	NTP_KEYSDIR	"%windir%\\system32\\drivers\\etc"
# endif /* SYS_WINNT */
#endif /* not CONFIG_FILE */

#ifdef HAVE_IPTOS_SUPPORT
/* 
 * "qos" modified keywords 
 */
#define	CONF_QOS_LOWDELAY		1
#define CONF_QOS_THROUGHPUT		2
#define CONF_QOS_RELIABILITY	3
#define CONF_QOS_MINCOST		4

#ifdef 		IPTOS_PREC_INTERNETCONTROL
#define CONF_QOS_CS0			5
#define CONF_QOS_CS1			6
#define CONF_QOS_CS2			7
#define CONF_QOS_CS3			8
#define CONF_QOS_CS4			9
#define CONF_QOS_CS5			10
#define CONF_QOS_CS6			11
#define CONF_QOS_CS7			12
#endif		/* IPTOS_PREC_INTERNETCONTROL */

#endif	/* HAVE_IPTOS_SUPPORT */



/* Limits */
#define MAXLINE 1024


/* Structure for storing an attribute-value pair  */
struct attr_val {
    int attr;
    union val{
        double d;
        int i;
        char *s;
        void *p;
    } value;
    int type;
};

/* Structure for nodes on the syntax tree */
struct address_node {
    char *address;
    int type;
};

struct restrict_node {
    struct address_node *addr;
    struct address_node *mask;
    queue *flags;
    int line_no;
};

struct peer_node {
    int host_mode;
    struct address_node *addr;
    int minpoll;
    int maxpoll;
    int ttl;
    int peerversion;
    int peerkey;
    int peerflags;
    double bias;
};

struct unpeer_node {
    struct address_node *addr;
};

struct auth_node {
    int autokey;
    int control_key;
    queue *crypto_cmd_list;
    char *keys;
    char *keysdir;
    int request_key;
    int revoke;
    queue *trusted_key_list;
    char *ntp_signd_socket;
};

struct filegen_node {
    char *name;
    queue *options;
};

struct setvar_node {
    char *data;
    u_long len;
    u_short def;
};

struct addr_opts_node {
    struct address_node *addr;
    queue *options;
};

struct sim_node {
    queue *init_opts;
    queue *servers;
};

enum broadcastclienttype {
    SIMPLE = 1,
    NOVOLLEY
};

/* A fix for now to prevent the configuration from being consumed before
 * it could be ever dumped
 */
struct config_tree_list {
    s_list *peers;
    s_list *unpeers;

    /* Other Modes */
    int broadcastclient;
    s_list *manycastserver;
    s_list *multicastclient;

    s_list *orphan_cmds;

    /* Monitoring Configuration */
    s_list *stats_list;
    char *stats_dir;
    s_list *filegen_opts;

    /* Access Control Configuration */
    s_list *discard_opts;
    s_list *restrict_opts;

    s_list *fudge;
    s_list *tinker;
    s_list *enable_opts;
    s_list *disable_opts;
    struct auth_node auth;

    s_list *logconfig;
    s_list *qos;
    s_list *phone;
    s_list *setvar;
    s_list *ttl;
    s_list *trap;
    s_list *vars;

    struct sim_node *sim_details;
};


/* The syntax tree */
struct config_tree {
    queue *peers;
    queue *unpeers;

    /* Other Modes */
    int broadcastclient;
    queue *manycastserver;
    queue *multicastclient;

    queue *orphan_cmds;

    /* Monitoring Configuration */
    queue *stats_list;
    char *stats_dir;
    queue *filegen_opts;

    /* Access Control Configuration */
    queue *discard_opts;
    queue *restrict_opts;

    queue *fudge;
    queue *tinker;
    queue *enable_opts;
    queue *disable_opts;
    struct auth_node auth;

    queue *logconfig;
    queue *qos;
    queue *phone;
    queue *setvar;
    queue *ttl;
    queue *trap;
    queue *vars;

    struct sim_node *sim_details;
};


/* Structure for holding a remote configuration command */
struct REMOTE_CONFIG_INFO {
    char buffer[MAXLINE];
    char err_msg[MAXLINE];
    int pos;
    int err_pos;
    int no_errors;
};


struct peer_node *create_peer_node(int hmode, struct address_node *addr, queue *options);
struct unpeer_node *create_unpeer_node(struct address_node *addr);
struct address_node *create_address_node(char *addr, int type);
void destroy_address_node(struct address_node *my_node);
queue *enqueue_in_new_queue(void *my_node);
struct attr_val *create_attr_dval(int attr, double value);
struct attr_val *create_attr_ival(int attr, int value);
struct attr_val *create_attr_sval(int attr, char *s);
struct attr_val *create_attr_pval(int attr, void *s);
struct filegen_node *create_filegen_node(void **name, queue *options);
void **create_pval(void *val);
struct restrict_node *create_restrict_node(struct address_node *addr, struct address_node *mask, queue *flags, int line_no);
int *create_ival(int val);
struct addr_opts_node *create_addr_opts_node(struct address_node *addr, queue *options);
struct sim_node *create_sim_node(queue *init_opts, queue *servers);
struct setvar_node *create_setvar_node(char *var, char *val, u_short def);

script_info *create_sim_script_info(double duration, queue *script_queue);
server_info *create_sim_server(struct address_node *addr, double server_offset, queue *script);

extern struct REMOTE_CONFIG_INFO remote_config;
void config_remotely(void);

int dump_config_dumper(const char *filename);


#endif
