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
} funcTkn;

typedef struct {
        double time;
        union {
                struct pkt evnt_pkt;
		struct recvbuf evnt_buf;
        } buffer;
#define ntp_pkt buffer.evnt_pkt
#define rcv_buf buffer.evnt_buf
        funcTkn function;
} Event;

typedef struct List {
        Event event;
        struct List *next;
} *Queue;

typedef struct nde {
        double	time;			/* simulation time */
	double	sim_time;		/* end simulation time */
	double	ntp_time;		/* client disciplined time */
	double	adj;			/* remaining time correction */
	double	slew;			/* correction slew rate */

	double	clk_time;		/* server time */
	double	ferr;			/* frequency errort */
	double	fnse;			/* random walk noise */
	double	ndly;			/* network delay */
	double	snse;			/* phase noise */
	double	pdly;			/* processing delay */
	double	bdly;			/* beep interval */

	double	last_time;		/* last clock read time */
        Queue	events;			/* Node Event Queue */
	struct recvbuf *rbuflist;	/* Node Receive Buffer */
} Node;

/*
 * Function prototypes
 */
int	ntpsim		(int argc, char *argv[]);
Event	event		(double, funcTkn);
Queue	queue		(Event, Queue );
Node	node		(void);
void	push		(Event, Queue *);
Event	pop		(Queue *);
void	ndbeep		(Node *, Event);
void	ndeclk		(Node *, Event);
void	ntptmr		(Node *, Event);
void	netpkt		(Node *, Event);
int	srvr_rply	(Node *, struct sockaddr_storage *,
			    struct interface *, struct pkt *);
double	gauss		(double, double);
double	poisson		(double, double);
int	node_clock	(Node *, double);
void	abortsim	(char *);

/*
 * The global Node
 */
Node ntp_node;

#endif

