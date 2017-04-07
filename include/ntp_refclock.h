/*
 * ntp_refclock.h - definitions for reference clock support
 */

#ifndef GUARD_NTP_REFCLOCK_H
#define GUARD_NTP_REFCLOCK_H

#if defined(HAVE_SYS_MODEM_H)
#include <sys/modem.h>
#endif

#include "ntp_types.h"
#include "ntp_tty.h"
#include "recvbuff.h"

/*
 * Configuration flag values
 */
#define	CLK_HAVETIME1	0x1
#define	CLK_HAVETIME2	0x2
#define	CLK_HAVEVAL1	0x4
#define	CLK_HAVEVAL2	0x8

#define	CLK_FLAG1	0x1
#define	CLK_FLAG2	0x2
#define	CLK_FLAG3	0x4
#define	CLK_FLAG4	0x8
#define CLK_HOLDOVER	0x10	/* no corresponding HAVE_ flag */

#define	CLK_HAVEFLAG1	0x10
#define	CLK_HAVEFLAG2	0x20
#define	CLK_HAVEFLAG3	0x40
#define	CLK_HAVEFLAG4	0x80

/*
 * Structure for returning clock status
 */
struct refclockstat {
	uint8_t	flags;		/* clock flags */
	uint8_t	haveflags;	/* bit array of valid flags */
	u_short	lencode;	/* length of last timecode */
	const char *p_lastcode;	/* last timecode received */
	uint32_t	polls;		/* transmit polls */
	uint32_t	noresponse;	/* no response to poll */
	uint32_t	badformat;	/* bad format timecode received */
	uint32_t	baddata;	/* invalid data timecode received */
	uint32_t	timereset;	/* driver resets */
	const char *clockname;	/* refclockname */
	const char *clockdesc;	/* ASCII description */
	double	fudgetime1;	/* configure fudge time1 */
	double	fudgetime2;	/* configure fudge time2 */
	int32_t	fudgeval1;	/* configure fudge value1 */
	uint32_t	fudgeval2;	/* configure fudge value2 */
	uint8_t	currentstatus;	/* clock status */
	uint8_t	lastevent;	/* last exception event */
	uint8_t	leap;		/* leap bits */
	struct	ctl_var *kv_list; /* additional variables */
};

/*
 * Reference clock I/O structure.  Used to provide an interface between
 * the reference clock drivers and the I/O module.
 */
struct refclockio {
	struct	refclockio *next; /* link to next structure */
	void	(*clock_recv) (struct recvbuf *); /* completion routine */
	int 	(*io_input)   (struct recvbuf *); /* input routine -
				to avoid excessive buffer use
				due to small bursts
				of refclock input data */
	struct peer *srcclock;	/* refclock peer */
	size_t	datalen;	/* length of data */
	int	fd;		/* file descriptor */
	u_long	recvcount;	/* count of receive completions */
	bool	active;		/* true when in use */
};

/*
 * Structure for returning debugging info
 */
#define	NCLKBUGVALUES	16
#define	NCLKBUGTIMES	32

struct refclockbug {
	uint8_t	nvalues;	/* values following */
	uint8_t	ntimes;		/* times following */
	u_short	svalues;	/* values format sign array */
	uint32_t	stimes;		/* times format sign array */
	uint32_t	values[NCLKBUGVALUES]; /* real values */
	l_fp	times[NCLKBUGTIMES]; /* real times */
};

/*
 * Structure interface between the reference clock support
 * ntp_refclock.c and the driver utility routines
 */
#define MAXSTAGE	60	/* max median filter stages  */
#define NSTAGE		5	/* default median filter stages */
#define BMAX		128	/* max timecode length */
#define GMT		0	/* I hope nobody sees this */
#define MAXDIAL		60	/* max length of modem dial strings */

struct refclockproc {
	void *	unitptr;	/* pointer to unit structure */
	struct refclock * conf;	/* pointer to driver method table */
	struct refclockio io;	/* I/O handler structure */
	uint8_t	leap;		/* leap/synchronization code */
	uint8_t	currentstatus;	/* clock status */
	uint8_t	lastevent;	/* last exception event */
	const char *clockname;	/* clock name (tag for logging) */
	const char *clockdesc;	/* clock description */
	u_long	nextaction;	/* local activity timeout */
	void	(*action)(struct peer *); /* timeout callback */

	char	a_lastcode[BMAX]; /* last timecode received */
	int	lencode;	/* length of last timecode */

	int	year;		/* year of eternity */
	int	day;		/* day of year */
	int	hour;		/* hour of day */
	int	minute;		/* minute of hour */
	int	second;		/* second of minute */
	long	nsec;		/* nanosecond of second */
	uint32_t	yearstart;	/* beginning of year */
	int	coderecv;	/* put pointer */
	int	codeproc;	/* get pointer */
	l_fp	lastref;	/* reference timestamp */
	l_fp	lastrec;	/* receive timestamp */
	double	offset;		/* mean offset */
	double	disp;		/* sample dispersion */
	double	jitter;		/* jitter (mean squares) */
	double	filter[MAXSTAGE]; /* median filter */

	/*
	 * Configuration data
	 */
	double	fudgetime1;	/* fudge time1 */
	double	fudgetime2;	/* fudge time2 */
	uint8_t	stratum;	/* server stratum */
	uint32_t	refid;		/* reference identifier */
	uint8_t	sloppyclockflag; /* driver options */

	/*
	 * Status tallies
 	 */
	u_long	timestarted;	/* time we started this */
	u_long	polls;		/* polls sent */
	u_long	noreply;	/* no replies to polls */
	u_long	badformat;	/* bad format reply */
	u_long	baddata;	/* bad data reply */
};

/*
 * Structure interface between the reference clock support
 * ntp_refclock.c and particular clock drivers. This must agree with the
 * structure defined in the driver.
 */

struct refclock {
	const char *basename;
	bool (*clock_start)	(int, struct peer *);
	void (*clock_shutdown)	(int, struct peer *);
	void (*clock_poll)	(int, struct peer *);
	void (*clock_control)	(int, const struct refclockstat *,
				 struct refclockstat *, struct peer *);
	void (*clock_init)	(void);
	void (*clock_timer)	(int, struct peer *);
};

/*
 * Function prototypes
 */
extern	bool	io_addclock	(struct refclockio *);
extern	void	io_closeclock	(struct refclockio *);

#ifdef REFCLOCK
extern	bool	refclock_newpeer (uint8_t, int, struct peer *);
extern	void	refclock_unpeer (struct peer *);
extern	void	refclock_receive (struct peer *);
extern	void	init_refclock	(void);
extern	void	refclock_control(sockaddr_u *,
				 const struct refclockstat *,
				 struct refclockstat *);
extern	int	refclock_open	(char *, u_int, u_int);
extern	bool	refclock_setup	(int, u_int, u_int);
extern	void	refclock_timer	(struct peer *);
extern	void	refclock_transmit(struct peer *);
extern 	bool	refclock_process(struct refclockproc *);
extern 	bool	refclock_process_f(struct refclockproc *, double);
extern 	void	refclock_process_offset(struct refclockproc *, l_fp,
					l_fp, double);
extern	void	refclock_report	(struct peer *, int);
extern	char	*refclock_name	(const struct peer *);
extern	int	refclock_gtlin	(struct recvbuf *, char *, int, l_fp *);
extern	size_t	refclock_gtraw	(struct recvbuf *, char *, size_t, l_fp *);
extern	bool	indicate_refclock_packet(struct refclockio *,
					 struct recvbuf *);
extern	void	process_refclock_packet(struct recvbuf *);
extern struct   refclock refclock_arbiter;
extern struct   refclock refclock_gpsdjson;
extern struct	refclock refclock_hpgps;
extern struct   refclock refclock_jjy;
extern struct   refclock refclock_local;
extern struct   refclock refclock_modem;
extern struct   refclock refclock_neoclock4x;
extern struct   refclock refclock_nmea;
extern struct   refclock refclock_parse;
extern struct   refclock refclock_shm;
extern struct   refclock refclock_spectracom;
extern struct   refclock refclock_trimble;
extern struct   refclock refclock_true;
extern struct   refclock refclock_zyfer;

#endif /* REFCLOCK */

#endif /* GUARD_NTP_REFCLOCK_H */
