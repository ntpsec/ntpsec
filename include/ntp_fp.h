/*
 * ntp_fp.h - definitions for NTP fixed/floating-point arithmetic
 */

#ifndef GUARD_NTP_FP_H
#define GUARD_NTP_FP_H

#include <arpa/inet.h>

#include "ntp_types.h"

/*
 * NTP uses two fixed point formats.
 *
 * The first (l_fp) is the "long" format and is 64 bits wide in units
 * of 1/2^32 seconds (which is between 232 and 233 decimal
 * picoseconds).  The zero value signifies the zero date of the
 * current NTP era; era zero began on the date 1900-00-00T00:00:00 in
 * proleptic UTC (leap second correction was not introduced until
 * 1972).
 *
 * The long format is used for timestamps in the NTP packet header (in
 * network byte order).  It is defined in RFC 5905 in Section 6 (Data
 * Types). In the on-the-wire context, it is always unsigned.
 *
 * When it is convenient to compute in seconds, this type can
 * be interpreted as a fixed-point float with the radix point between
 * bits 31 and 32. This is why there are macros to extract the low and
 * high halves.
 *
 * Internally, this type is sometimes used for time offsets.  In that 
 * context it is interpreted as signed and can only express offsets
 * up to a half cycle. Offsets are normally much, much smaller than that;
 * for an offset to have a value even as large as 1 second would be 
 * highly unusual after ntpd initialization.
 *
 * Anyway, an l_fp looks like:
 *
 *    0			  1		      2			  3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |			       Integral Part			     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |			       Fractional Part			     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * NTP time stamps will overflow in 2036.  Until then we are in
 * NTP Epoch 0.  After that NTP timestamps will be in Epoch 1.  Negative
 * epochs can be used to represent time before Jan 1900.
 *
 * The epoch number is not explicit on the wire. It will seldom be an
 * issue: timestamp differences between two adjacent epochs are still
 * valid as long as the true time difference is less than half an
 * epoch.  In other words, you don't have to explicitly check for the
 * epoch of either timestamp if you assume that these are less than 68
 * years apart.
 */
typedef uint64_t l_fp;
#define lfpfrac(n)		((uint32_t)(n))
#define lfptosint(n)	        (( int64_t)((uint64_t)(n) << 32))
#define lfptouint(n)	        ((uint64_t)((uint64_t)(n) << 32))
#define lfpsint(n)		(( int32_t)((n) >> 32))
#define lfpuint(n)		((uint32_t)((n) >> 32))
#define bumplfpsint(n, i)       ((n) += lfptouint(i))
#define bumplfpuint(n, i)       ((n) += lfptouint(i))
#define setlfpfrac(n, v)        ((n)  = (lfptouint(lfpuint(n)) | lfpfrac(v)))
#define setlfpsint(n, v)        ((n)  = (lfptosint(v) | lfpfrac(n)))
#define setlfpuint(n, v)        ((n)  = (lfptouint(v) | lfpfrac(n)))

static inline l_fp lfpinit(int32_t sec, uint32_t frac)
{
    l_fp tmp = lfptouint(sec) | lfpfrac(frac);
    return tmp;
}

static inline l_fp lfpinit_u(uint32_t sec, uint32_t frac)
{
    l_fp tmp = lfptouint(sec) | lfpfrac(frac);
    return tmp;
}

/*
 * Fractional precision (of an l_fp) is actually the number of
 * bits in an int32_t/uint32_t.
 */
#define	FRACTION_PREC	(32)

/*
 * The second fixed point format is 32 bits, with the decimal between
 * bits 15 and 16.  There is a signed version (s_fp) and an unsigned
 * version (u_fp).  This is used to represent synchronizing distance
 * and synchronizing dispersion in the NTP packet header (again, in
 * network byte order) and internally to hold both distance and
 * dispersion values (in local byte order).  In network byte order
 * it looks like:
 *
 *    0			  1		      2			  3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |		  Integer Part	     |	   Fraction Part	     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */
typedef uint32_t u_fp;

/*
 * A unit second in fp format.	Actually 2**(half_the_bits_in_int32)
 */
#define	FP_SECOND	(0x10000)

/*
 * Byte order conversions
 */
#define	HTONS_FP(x)	(htonl(x))
#define	NTOHS_FP(x)	(ntohl(x))

typedef struct {
         uint32_t        l_ui;
         uint32_t        l_uf;
} l_fp_w;

/*
 * Generate the wire-format version (that is, big-endian all the way down)
 * of a timestamp expressed as a 64-bit scalar.
 */
static inline l_fp_w htonl_fp(l_fp lfp) {
    l_fp_w lfpw;
    lfpw.l_ui = htonl(lfpuint(lfp));
    lfpw.l_uf = htonl(lfpfrac(lfp));
    return lfpw;
}

static inline l_fp ntohl_fp(l_fp_w lfpw) {
    return lfpinit_u(ntohl(lfpw.l_ui), ntohl(lfpw.l_uf));
}

#define	M_ISNEG(v_i)			/* v < 0 */ \
	(((v_i) & 0x80000000) != 0)

/*
 * Operations on the long fp format.  The only reason these aren't
 * native operations is to be independent of whether the l_fp
 * type is signed or unsigned.
 */
#define	L_NEG(v)	(v) = (l_fp)(-(int64_t)(v))
#define	L_ISNEG(v)	M_ISNEG(lfpuint(v))
#define	L_ISGT(a, b)	((int64_t)(a) > (int64_t)(b))
#define	L_ISGTU(a, b)	((a) > (b))

/*
 * scaling to 32bit FP format
 * double to u_fp conversion
 */
#define FP_SCALE(r)	(ldexp((double)(r),  16))
#define FP_UNSCALE(r)	(ldexp((double)(r), -16))
#define DTOUFP(r)	((u_fp)FP_SCALE(r))

/*
 * l_fp/double conversions
 */
#define FRAC		4294967296.0 		/* 2^32 as a double */

#include <math.h>	/* ldexp() */

static inline l_fp dtolfp(double d)
/* double to l_fp
 * assumes signed l_fp, i.e. a time offset
 * undefined return if d in NaN
 */
{
/* Please check issue 264 before changing this to use llround
 * https://gitlab.com/NTPsec/ntpsec/issues/264
 * llround is broken on NetBSD, 2017-May-05
 */
	return (l_fp)(int64_t)(ldexp(d, 32));
}

static inline double lfptod(l_fp r)
/* l_fp to double
 * assumes signed l_fp, i.e. a time offset
 */
{
	return ldexp((double)((int64_t)r), -32);
}

/*
 * Prototypes
 */
extern	char *	dolfptoa	(l_fp, bool, short, bool);
extern	char *	mfptoa		(l_fp, short);
extern	char *	mfptoms		(l_fp, short);

extern	bool	atolfp		(const char *, l_fp *);
extern	bool	hextolfp	(const char *, l_fp *);
extern	bool	mstolfp		(const char *, l_fp *);
extern	char *	prettydate	(const l_fp);
extern	char *	gmprettydate	(const l_fp);
extern	char *	rfc3339date	(const l_fp);
extern	char *	rfc3339time     (time_t);

extern	void	set_sys_fuzz	(double);
extern  void	get_ostime	(struct timespec *tsp);
extern	void	get_systime	(l_fp *);
extern	bool	step_systime	(double, int (*settime)(struct timespec *));
extern	bool	adj_systime	(double, int (*adjtime)(const struct timeval *, struct timeval *));

#define	lfptoa(fpv, ndec)	mfptoa((fpv), (ndec))
#define	lfptoms(fpv, ndec)	mfptoms((fpv), (ndec))

#define	ulfptoa(fpv, ndec)	dolfptoa((fpv), false, (ndec), false)
#define	ulfptoms(fpv, ndec)	dolfptoa((fpv), false, (ndec), true)
#define	umfptoa(lfp, ndec)	dolfptoa((lfp), false, (ndec), false)

/*
 * Optional callback from libntp step_systime() to ntpd.  Optional
*  because other libntp clients like ntpdate don't use it.
 */
typedef void (*time_stepped_callback)(void);
extern time_stepped_callback	step_callback;

#endif /* GUARD_NTP_FP_H */
