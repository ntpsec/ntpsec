#ifndef GUARD_NTP_IO_H
#define GUARD_NTP_IO_H

#include "ntp_workimpl.h"

/*
 * POSIX says use <fnctl.h> to get O_* symbols and
 * SEEK_SET symbol form <unistd.h>.
 */
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <netinet/in.h>

#include <isc/netaddr.h>

/*
 * NIC rule match types
 */
typedef enum {
	MATCH_ALL,
	MATCH_IPV4,
	MATCH_IPV6,
	MATCH_WILDCARD,
	MATCH_IFNAME,
	MATCH_IFADDR
} nic_rule_match;

/*
 * NIC rule actions
 */
typedef enum {
	ACTION_LISTEN,
	ACTION_IGNORE,
	ACTION_DROP
} nic_rule_action;


extern int	qos;
SOCKET		move_fd(SOCKET fd);
extern bool	is_ip_address(const char *, u_short, sockaddr_u *);
extern void	sau_from_netaddr(sockaddr_u *, const isc_netaddr_t *);
extern void	add_nic_rule(nic_rule_match match_type,
			     const char *if_name, int prefixlen,
			     nic_rule_action action);
extern	void	maintain_activefds(int fd, int closing);
extern void	make_socket_nonblocking( SOCKET fd );
extern SOCKET	move_fd( SOCKET fd );

#endif	/* GUARD_NTP_IO_H */
