/* NTP simulator engine - Harish Nair */
/* University of Delaware, 09/11/2001 */

#include "ntpd.h"
#include "ntpsim.h"

/* Defines... */
#define SIM_TIME 86400		/* Simulation Time		*/
#define NET_DLY 1000            /* Base Propagation Delay       */
#define PROC_DLY 10             /* Base Processing Delay        */
#define SRVR_ACRY 10	        /* Server Accuracy		*/
#define BEEP_DLY 3600           /* Beep Delay                   */
#define TCK 10000               /* Tick = 1/frequency           */
#define TCKADJ 5                /* Tickadj                      */

#define watch(x) printf("%s!\n", (char *)x) /* for debug */

/* Function Pointers */
void (*funcPtr[]) (Node *, Event) = {
        &ndbeep, &ndeclk, &ntptmr, &netpkt
};

/* ntpsim function */
int ntpsim(int argc, char *argv[])
{
	srand48((unsigned int)time(NULL));
	ntp_node = node();
	ntpinit(argc, argv);
	run(&ntp_node);
	return(0);
}

/* returns an event */
Event event(double t, funcTkn f)
{
	Event e;
	e.time = t;
	e.function = f;
	return(e);
}

/* creates an event queue */
Queue queue(Event e, Queue q)
{
	Queue ret;
	if((ret=(Queue)malloc(sizeof(struct List))) == NULL)
                abortsim("queue-malloc");
	ret->event = e;
	ret->next = q;
	return(ret);
}

/* creates a node for simulation */
Node node()
{
	Node n;
	struct timeval tv;

        (void) gettimeofday(&tv, (struct timezone *)0);

	n.time = tv.tv_sec+tv.tv_usec/1e6;
	n.time = 0.0;
	n.ntp_time = n.time;
	n.clk_time = n.time;
	n.sim_time = SIM_TIME;
	n.adj = 0.0;
	n.tick = TCK;
	n.tickadj = TCKADJ;
	n.offset = 0;
	n.ferr = 0;
	n.fnse = 0;
	n.ndly = NET_DLY;
	n.pdly = PROC_DLY;
	n.bdly = BEEP_DLY;
	n.nnse1 = 0;
	n.nnse2 = 0;
	n.snse = SRVR_ACRY;
	n.events = NULL;
	n.rbuflist = (struct recvbuf *)0;
	push(event(0, BEEP), &(n.events));
	push(event(n.time+1.0, TIMER), &(n.events));
	return(n);
}

/* prepares node for ntp simulation */
void ntpinit(int argc, char *argv[])
{
#ifdef AUTOKEY
        u_int n;
        char hostname[MAX_HOSTLEN + 1];
#endif /* AUTOKEY */
	initializing = 1;
        init_auth();
        init_util();
        init_restrict();
        init_mon();
        init_timer();
        init_lib();
        init_random();
        init_request();
        init_control();
        init_peer();
        init_proto();
        init_io();
        init_loopfilter();
        mon_start(MON_OFF);
	getconfig(argc, argv);
#ifdef AUTOKEY
        gethostname(hostname, MAX_HOSTNAME);
        n = strlen(hostname) + 1;
        sys_hostname = emalloc(n);
        memcpy(sys_hostname, hostname, n);
#ifdef PUBKEY
        crypto_setup();
#endif /* PUBKEY */
#endif /* AUTOKEY */
        initializing = 0;
}

/* push an event into the event queue */
void push(Event e, Queue *qp)
{
	Queue *tmp = qp;
	while((*tmp!=NULL)&&((*tmp)->event.time<e.time)) tmp = &((*tmp)->next);
	*tmp = queue(e, (*tmp));
}

/* pop the first event from the event queue */
Event pop(Queue *qp)
{
	Event ret;
	Queue tmp;
	tmp = *qp;
	if(tmp == NULL) abortsim("pop - empty queue");
	ret = tmp->event;
	*qp = tmp->next;
	free(tmp);
	return(ret);
}

/* simulate a node */
void run(Node *n)
{
	Event e;
	double maxtime;

	maxtime = n->time + n->sim_time;
	while( n->time <= maxtime && n->events != NULL ) {
		e = pop(&(n->events));
		ndeclk(n, e);
		if(n->time>maxtime) break;
		funcPtr[e.function](n, e);
	}
}

void ndeclk(Node *n, Event e)
{
	node_clock(n, e.time);
}

/* simulates the ntp protocol */
void ntptmr(Node *n, Event e)
{
	struct recvbuf * rbuf = NULL;

	timer();

	while (n->rbuflist != (struct recvbuf *)0)
	{
		rbuf = n->rbuflist;
		n->rbuflist = rbuf->next;
		(rbuf->receiver)(rbuf);
		free(rbuf);
	}
	push(event(e.time+(1<<EVENT_TIMEOUT), TIMER), &(n->events));
}

/* Inserts received packet into n->rbuflist */
void netpkt(Node *n, Event e)
{
	
	struct recvbuf * rbuf = (struct recvbuf *)0;
	struct recvbuf * obuf = n->rbuflist;

	if((rbuf=(struct recvbuf *)malloc(sizeof(struct recvbuf)))==NULL)
		abortsim("ntprcv-malloc");
	memcpy(rbuf, &(e.rcv_buf), sizeof(struct recvbuf));
	rbuf->receiver = receive;
	rbuf->next = (struct recvbuf *)0;
	if(obuf == (struct recvbuf *)0) n->rbuflist = rbuf;
	else {
		while (obuf->next != (struct recvbuf *)0) obuf = obuf->next;
		obuf->next = rbuf;
	}
}

/* simulates a reply from server - called from sendpkt in ntp_io.c*/
int srvr_rply(Node *n, struct sockaddr_storage *dest,
		struct interface *inter, struct pkt *rpkt)
{
	double  srvr_time = (guassian(n->time*1e6, n->snse))/1e6;
	struct  pkt xpkt;
	struct recvbuf rbuf;

	s_char	srvr_precision = (s_char)get_precision(100);
	double  srvr_rootdelay = 0.0;
	double  srvr_rootdispersion = 0.0;
	u_int32 srvr_refid = inet_addr("127.1.1.0");
	l_fp    srvr_reftime;
	l_fp	xmt_ts;
	l_fp	recv_time;
	double  prop_delay = 0.0;
	double	proc_delay = 0.0;
	Event   xvnt;

	srvr_reftime.l_i = 0;
        srvr_reftime.l_f = 0;
	xpkt.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOWARNING, NTP_VERSION,
					MODE_SERVER);
	xpkt.stratum = STRATUM_TO_PKT(((u_char)1));
	xpkt.refid = srvr_refid;
	xpkt.ppoll = rpkt->ppoll;
        xpkt.precision = srvr_precision;
        xpkt.rootdelay = HTONS_FP(DTOFP(srvr_rootdelay));
        xpkt.rootdispersion =
            HTONS_FP(DTOUFP(srvr_rootdispersion));
        HTONL_FP(&srvr_reftime, &xpkt.reftime);
        xpkt.org = rpkt->xmt;
	recv_time.l_i = (int)srvr_time;
	prop_delay = guassian(n->ndly, n->nnse1);
	recv_time.l_f = (srvr_time-(int)srvr_time)*1e6+prop_delay;
	if(recv_time.l_f >= 1e6) {
		recv_time.l_i++;
		recv_time.l_f -= 1e6;
	}
        HTONL_FP(&recv_time, &xpkt.rec);
	xmt_ts.l_i = (int)srvr_time;
	proc_delay = guassian(n->pdly, sqrt(n->pdly/1000));
        xmt_ts.l_f = (srvr_time-(int)srvr_time)*1e6+prop_delay+proc_delay;
	if(xmt_ts.l_f >= 1e6) {
                xmt_ts.l_i++; 
                xmt_ts.l_f -= 1e6; 
        } 
        HTONL_FP(&xmt_ts, &xpkt.xmt);
	rbuf.receiver = receive;
        get_systime(&(rbuf.recv_time));
        rbuf.recv_length = LEN_PKT_NOMAC;
        rbuf.recv_pkt = xpkt;
        memcpy(&rbuf.srcadr, dest, sizeof(struct sockaddr_storage));
        memcpy(&rbuf.recv_srcadr, dest, sizeof(struct sockaddr_storage));
        if((rbuf.dstadr =
            (struct interface *)malloc(sizeof(struct interface))) == NULL)
                abortsim("server-malloc");
        memcpy(rbuf.dstadr, inter, sizeof(struct interface));
        rbuf.next = (struct recvbuf *)0;
	prop_delay += guassian(n->ndly, n->nnse2);
	xvnt = event((srvr_time*1e6+prop_delay+proc_delay)/1e6, PACKET);
	xvnt.rcv_buf = rbuf;
	push(xvnt, &(n->events));
	return(0);
}

void ndbeep(Node *n, Event e)
{
	static int first_time = 1;
	char *dash = "-----------------";

	if(n->bdly>0) {
		if(first_time) {
			printf("\t%4c    T    %4c\t%4c  T+ERR  %3c\t%5cT+ERR+NTP\n", ' ', ' ', ' ', ' ',' ');
			printf("\t%s\t%s\t%s\n", dash, dash, dash);
			first_time = 0;
			push(event(n->bdly, BEEP), &(n->events));  
        		push(event(n->sim_time, BEEP), &(n->events));
			printf("\t%16.6f\t%16.6f\t%16.6f\n",
                        	n->time, n->clk_time, n->ntp_time);
			return;
		}
		printf("\t%16.6f\t%16.6f\t%16.6f\n",
			n->time, n->clk_time, n->ntp_time);
		push(event(e.time+n->bdly, BEEP), &(n->events));
	}
}

/* aborts the simulator */
void abortsim(char *errmsg)
{
        perror(errmsg);
        exit(1);
}

