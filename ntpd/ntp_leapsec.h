/*
 * ntp_leapsec.h - leap second processing for NTPD
 *
 * Written by Juergen Perlinger (perlinger@ntp.org) for the NTP project.
 * The contents of 'html/copyright.html' apply.
 * ----------------------------------------------------------------------
 * This is an attempt to get the leap second handling into a dedicated
 * module to make the somewhat convoluted logic testable.
 */

#ifndef NTP_LEAPSEC_H
#define NTP_LEAPSEC_H

/* these should probably go to libntp... */
extern vint64 strtouv64(const char * src, char ** endp, int base);
extern int    icmpv64(const vint64 * lhs, const vint64 * rhs);
extern int    ucmpv64(const vint64 * lhs, const vint64 * rhs);

/* function pointer types. Note that 'fprintf' and 'getc' can be casted
 * to the dumper resp. reader type, provided the auxiliary argument is a
 * valid FILE pointer in hat case.
 */
typedef void (*leapsec_dumper)(void*, const char *fmt, ...);
typedef int  (*leapsec_reader)(void*);

struct leap_table;
typedef struct leap_table leap_table_t;

/* Set/get electric mode
 * Electric mode is defined as the operation mode where the system clock
 * automagically manages the leap second, so we don't have to care about
 * stepping the clock. (This should be the case with most systems,
 * including the current implementation of the Win32 timekeeping.)
 *
 * The consequence of electric mode is that we do not 'see' the leap
 * second, and no client actions are needed when crossing the leap era
 * boundary.  In manual (aka non-electric) mode the clock will simply
 * step forward untill *we* (that is, this module) tells the client app
 * to step at the right time. This needs a slightly different type of
 * processing, so switching between those two modes should not be done
 * close to a leap second. The transition might be lost in that case.
 *
 * OTOH, this is a system characteristic, so it's expected to be set
 * properly somewhere after system start and retain the value.
 *
 * Simply querying the state or setting it to the same value as before
 * does not have any unwanted side effects.  You can query by giving a
 * negative value for the switch.
 */
extern int/*BOOL*/ leapsec_electric(int/*BOOL*/ on);


/* Query result for a leap second schedule
 * 'when' tells the transition point in full time scale, but only if
 *	'tai_diff' is not zero. 
 * 'dist' is the distance to the transition, in clock seconds.
 *	Only valid if 'tai_diff' not zero. To get the true (elapsed
 *	time) difference, add 'tai_diff' to that value.
 * 'tai_offs' is the CURRENT distance from clock (UTC) to TAI.
 * 'tai_diff' is the change in TAI offset after the next leap
 *	transition. Zero if nothing is pending.
 * 'warped' is set only once, when the the leap second occurred between
 *	two queries.
 * 'proximity' is a proximity warning. See definitions below. This might
 *	be more useful than an absolute difference to the leap second.
 * 'dynamic' != 0 if entry was requested by clock/peer
 */ 
struct leap_result {
	vint64	        when;
	u_int32         dist;
	short           tai_offs;
	short           tai_diff;
	short           warped;
	u_short         proximity;
	short		dynamic;
};
typedef struct leap_result leap_result_t;

struct leap_signature {
	u_int32	etime;	/* expiration time	*/
	u_int32 ttime;	/* transition time	*/
	short   taiof;	/* total offset to TAI	*/
};
typedef struct leap_signature leap_signature_t;


#define LSPROX_NOWARN	0	/* clear radar screen         */
#define LSPROX_SCHEDULE	1	/* less than 1 month to target*/
#define LSPROX_ANNOUNCE	2	/* less than 1 day to target  */
#define LSPROX_ALERT	3	/* less than 10 sec to target */

/* Get the current or alternate table ponter. Getting the alternate
 * pointer will automatically copy the primary table, so it can be
 * subsequently modified.
 */
extern leap_table_t *leapsec_get_table(int alternate);
/* Set the current leap table. Accepts only return values from
 * 'leapsec_get_table()', so it's hard to do something wrong. Returns
 * TRUE if the table was exchanged.
 */
extern int/*BOOL*/ leapsec_set_table(leap_table_t*);

/* Clear all leap second data. Use it for init & cleanup */
extern void leapsec_clear(leap_table_t*);

/* Check if a table is expired at a given point in time. 'when' is
 * subject to NTP era unfolding.
 */
extern int/*BOOL*/ leapsec_is_expired(leap_table_t*, u_int32 when, 
				      const time_t * pivot);

/* Load a leap second file. If 'blimit' is set, do not store (but
 * register with their TAI offset) leap entries before the build date.
 */
extern int/*BOOL*/ leapsec_load(leap_table_t*, leapsec_reader,
				void*, int blimit);


/* Dump the current leap table in readable format, using the provided
 * dump formatter function.
 */
extern void leapsec_dump(const leap_table_t*, leapsec_dumper func, void *farg);

/* Glue to integrate this easier into existing code */
extern int/*BOOL*/ leapsec_load_file(FILE*, int blimit);
extern void        leapsec_getsig(leap_signature_t * psig);
extern int/*BOOL*/ leapsec_expired(u_int32 when, const time_t * pivot);

/* Reset the current leap frame */
extern void leapsec_reset_frame(void);

/* Given a transition time, the TAI offset valid after that and an
 * expiration time, try to establish a system leap transition. Only
 * works if the existing table is extended. On success, updates the
 * signature data.
 */
extern int/*BOOL*/ leapsec_add_fix(u_int32 ttime, u_int32 etime, int total,
				   const time_t * pivot);

/* Take a time stamp and create a leap second frame for it. This will
 * schedule a leap second for the beginning of the next month, midnight
 * UTC. The 'insert' argument tells if a leap second is added (!=0) or
 * removed (==0). We do not handle multiple inserts (yet?)
 *
 * Returns 1 if the insert worked, 0 otherwise. (It's not possible to
 * insert a leap second into the current history -- only appending
 * towards the future is allowed!)
 *
 * 'ntp_now' is subject to era unfolding. The entry is marked
 * dynamic. The leap signature is NOT updated.
 */
extern int/*BOOL*/ leapsec_add_dyn(u_int32 ntp_now, int insert,
				   const time_t * pivot);

/* Take a time stamp and get the associated leap information. The time
 * stamp is subject to era unfolding around the pivot or the current
 * system time if pivot is NULL. Sets the information in '*qr' and
 * returns TRUE if a leap second era boundary was crossed between the
 * last and the current query. In that case, qr.warped contains the
 * required clock stepping. (Which is zero in electric mode...)
 */
extern int/*BOOL*/ leapsec_query(leap_result_t *qr, u_int32 ntpts,
				 const time_t * pivot);

#endif /* !defined(NTP_LEAPSEC_H) */
