/*
 * ntpsim.h - Prototypes for ntpsim
 */

#ifndef __ntpsim_h
#define __ntpsim_h

#include <stdio.h>
#include <math.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "ntp_syslog.h"
#include "ntp_fp.h"
#include "ntp.h"
#include "ntpd.h"
#include "ntp_select.h"
#include "ntp_malloc.h"
#include "ntp_refclock.h"
#include "recvbuff.h"
#include "ntp_io.h"
#include "ntp_stdlib.h"

#define PI 3.1415926535

/*
 * ntpsim declarations
 */
 
typedef enum {
        BEEP, CLOCK, TIMER, PACKET
}funcTkn;

typedef struct
{
        double time;
        union {
                struct pkt evnt_pkt;
		struct recvbuf evnt_buf;
        }buffer;
#define ntp_pkt buffer.evnt_pkt
#define rcv_buf buffer.evnt_buf
        funcTkn function;
}Event;

typedef struct List
{
        Event event;
        struct List *next;
}*Queue;

typedef struct nde
{
	u_int32 tick;			/* tick - 1/freq */
	u_int32 tickadj;		/* tickadj - slew rate */
	double offset;			/* clock reading offset */
	double ferr;			/* systematic frequency offset*/
	double fnse;			/* random frequency noise */
	double ndly;			/* base delat to server */
	double nnse;			/* randon network noise */
        double time;			/* Correct Node Time */
	double clk_time;                /* Unsynchronised Clock Time */
	double ntp_time;		/* Synchronised NTP Time */
	double adj;			/* Time to be adjusted */
        Queue events;			/* Node Event Queue */
	struct recvbuf *rbuflist;	/* Node Receive Buffer */
}Node;

/* Function Prototypes */
int ntpsim(int argc, char *argv[]);
Event event(double, funcTkn);
Queue queue(Event, Queue );
Node node(void);
void ntpinit(int argc, char *argv[]);
void push(Event, Queue *);
Event pop(Queue *);
void run(Node *);
void ndbeep(Node *, Event);
void ndeclk(Node *, Event);
void ntptmr(Node *, Event);
void netpkt(Node *, Event);
int srvr_rply(Node *, struct sockaddr_in *, struct interface *, struct pkt *);
double guassian(double, double);
int node_clock(Node *, double);
int node_gettime(Node *, struct timeval *);
int node_adjtime(Node *, struct timeval *, struct timeval *);
int node_settime(Node *, struct timeval *);
int get_precision(int);
void abortsim(char *);

/* The global Node */
Node ntp_node;

#endif

