#ifndef SNTP_MAIN_H
#define SNTP_MAIN_H

#include <config.h>
#include <l_stdlib.h>
#include <ntp_fp.h>
#include <ntp.h>
#include <ntp_stdlib.h>
#include <ntp_unixtime.h>
#include <isc/result.h>
#include <isc/net.h>
#include <stdio.h>

#include <sntp-opts.h>

void set_li_vn_mode (struct pkt *spkt, char leap, char version, char mode); 
int sntp_main (int argc, char **argv);
int on_wire (struct addrinfo *host, struct addrinfo *bcastaddr);
int set_time (double offset);

#endif /* SNTP_MAIN_H */
