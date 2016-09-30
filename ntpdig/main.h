#ifndef GUARD_MAIN_H
#define GUARD_MAIN_H

#include <stdio.h>
#include <isc/net.h>

#include "ntp.h"
#include "ntp_stdlib.h"
#include "ntp_debug.h"
#include "timespecops.h"

#include "crypto.h"

void	set_li_vn_mode(struct pkt *spkt, char leap, char version, char mode); 
extern int ntpdig_main(int argc, char **argv, const char *);
int	generate_pkt(struct pkt *x_pkt, const struct timespec *tv_xmt,
				  int key_id, struct key *pkt_key);
int	handle_pkt(int rpktl, struct pkt *rpkt, sockaddr_u *host,
		   const char *hostname);
void	offset_calculation(struct pkt *rpkt, int rpktl,
			   struct timespec *tv_dst, double *offset,
			   double *precision, double *root_dispersion);
int	set_time(double offset);

#endif /* GUARD_MAIN_H */
