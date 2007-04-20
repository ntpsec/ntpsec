#ifndef __NTP_CONFIG_H__
#define __NTP_CONFIG_H__

#include "ntp_data_structures.h"


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
};

struct auth_node {
    int autokey;
    int control_key;
    queue *crypto_cmd_list;
    char *keys;
    char *keysdir;
    int requested_key;
    int revoke;
    queue *trusted_key_list;
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

/* The syntax tree */
struct config_tree {
    queue *peers;

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
    queue *phone;
    queue *setvar;
    queue *ttl;
    queue *trap;
    queue *vars;

    struct sim_node *sim_details;
};

/* Structure to hold a filename, file pointer and positional info */
struct FILE_INFO {
    char *fname;               /* Path to the file */
    FILE *fd;                  /* File Descriptor */
    int line_no;               /* Line Number in the file being scanned */
    int col_no;                /* Column Number in the file being scanned */
    int prev_line_col_no;      /* Col No on the previous line when a '\n' is
                                  seen */
};

/* Structure for holding a remote configuration command */
struct REMOTE_CONFIG_INFO {
    char buffer[MAXLINE];
    char err_msg[MAXLINE];
    int pos;
    int err_pos;
    int no_errors;
};

extern struct REMOTE_CONFIG_INFO remote_config;
void config_remotely(void);

#endif
