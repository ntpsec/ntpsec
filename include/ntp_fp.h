/*
 * ntp_fp.h - definitions for NTP fixed/floating-point arithmetic
 */

#ifndef GUARD_NTP_FP_H
#define GUARD_NTP_FP_H

#include <arpa/inet.h>

#include "ntp_types.h"

/*
 * NTP uses two fixed point formats.  The first (l_fp) is the "long"
 * format and is 64 bits wide with the decimal between bits 31 and 32.
 * This is used for time stamps in the NTP packet header (in network
 * byte order) and for internal computations of offsets (in local host
 * byte order). We use the same structure for both signed and unsigned
 * values, which is a big hack but saves rewriting all the operators
 * twice. Just to confuse this, we also sometimes just carry the
 * fractional part in calculations, in both signed and unsigned forms.
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
 */
typedef uint64_t l_fp;
#define LOW32	0x00000000ffffffffUL
#define HIGH32	0xffffffff00000000UL
#define BUMP	0x0000000100000000UL
#define lfpfrac(n)		((uint32_t)((n) & LOW32))
#define setlfpfrac(n, v)	(n) = (((n) & HIGH32) | ((v) & LOW32))
#define lfpsint(n)		(int32_t)(((n) & HIGH32) >> 32)
#define setlfpsint(n, v)	(n) = (int64_t)((((int64_t)(v)) << 32) | ((n) & LOW32))
#define bumplfpsint(n, i)	(n) += (i)*BUMP
#define lfpuint(n)		(uint32_t)(((n) & HIGH32) >> 32)
#define setlfpuint(n, v)	(n) = (uint64_t)((((uint64_t)(v)) << 32) | ((n) & LOW32))
#define bumplfpuint(n, i)	(n) += (i)*BUMP

static inline l_fp lfpinit(int32_t hi, uint32_t lo)
{
    l_fp tmp = 0;
    setlfpsint(tmp, hi);
    setlfpfrac(tmp, lo);
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
typedef int32_t s_fp;
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

static inline l_fp_w htonl_fp(l_fp lfp) {
    l_fp_w lfpw;
    lfpw.l_ui = htonl(lfpuint(lfp));
    lfpw.l_uf = htonl(lfpfrac(lfp));
    return lfpw;
}

static inline l_fp ntohl_fp(l_fp_w lfpw) {
    return lfpinit(ntohl(lfpw.l_ui), ntohl(lfpw.l_uf));
}

/* Convert unsigned ts fraction to net order ts */
#define	HTONL_UF(uf, nts)	*nts = lfpint(0, htonl(uf))

/*
 * Conversions between the two fixed point types
 */
#define	MFPTOFP(x_i, x_f)	(((x_i) >= 0x00010000) ? 0x7fffffff : \
				(((x_i) <= -0x00010000) ? 0x80000000 : \
				(((x_i)<<16) | (((x_f)>>16)&0xffff))))
#define	LFPTOFP(v)		MFPTOFP(lfpsint(*v), lfprac(*v))

#define UFPTOLFP(x, v) (*v = lfpinit((u_fp)(x)>>16, (x)<<16))
#define FPTOLFP(x, v)  (UFPTOLFP((x), (v)), (x) < 0 ? setlfpuint(*v, getlfpuint(*v) - 0x10000) : 0)

#define MAXLFP(v) *v = lfpinit(0x7fffffffu, 0xffffffffu)
#define MINLFP(v) *v = lfpinit(0x80000000u, 0u)

/*
 * Primitive operations on long fixed point values.  If these are
 * reminiscent of assembler op codes it's only because some may
 * be replaced by inline assembler for particular machines someday.
 * These are the (kind of inefficient) run-anywhere versions.
 */
#define	M_NEG(v_i, v_f)		/* v = -v */ \
	do { \
		(v_f) = ~(v_f) + 1u; \
		(v_i) = ~(v_i) + ((v_f) == 0); \
	} while (false)

#define	M_ISNEG(v_i)			/* v < 0 */ \
	(((v_i) & 0x80000000) != 0)

/*
 * Operations on the long fp format.  The only reason these aren't
 * native operations is to be independent of whether the l_fp
 * type is signed ot unsigned.
 */
#define	L_NEG(v)	(*v) = (-1 * (int64_t)(*v))
#define L_ADDUF(r, uf)	(*r) = ((*r) + (uf))
#define L_SUBUF(r, uf)	(*r) = ((*r) - (uf))
#define	L_ADDF(r, f)	(*r) = ((int64_t)(*r) + (int64_t)(uf))
#define	L_ISNEG(v)	M_ISNEG(lfpuint(*v))
#define	L_ISGT(a, b)	((int64_t)(*a) > (int64_t)(*b))
#define	L_ISGTU(a, b)	((*a) > (*b))
#define	L_ISGEQ(a, b)	((int64_t)(*a) >= (int64_t)(*b))
#define L_ISGEQU(a, b)  ((*a) >= (*b))

/*
 * s_fp/double and u_fp/double conversions
 */
#define FRIC		65536.0			/* 2^16 as a double */
#define DTOFP(r)	((s_fp)((r) * FRIC))
#define DTOUFP(r)	((u_fp)((r) * FRIC))
#define FPTOD(r)	((double)(r) / FRIC)

/*
 * l_fp/double conversions
 */
#define FRAC		4294967296.0 		/* 2^32 as a double */

#include <math.h>	/* ldexp() */

static inline l_fp dtolfp(double d)
/* double to l_fp */
{
	double	d_tmp;
	uint64_t	q_tmp;
	int	M_isneg;

	d_tmp = (d);
	M_isneg = (d_tmp < 0.);
	if (M_isneg) {
		d_tmp = -d_tmp;
	}
	q_tmp = (uint64_t)ldexp(d_tmp, 32);
	if (M_isneg) {
		q_tmp = ~q_tmp + 1;
	}
	return lfpinit((uint32_t)(q_tmp >> 32) , (uint32_t)q_tmp);
}

static inline double lfptod(l_fp r)
/* l_fp to double */
{
	double	d;
	uint64_t	q_tmp;
	int	M_isneg;

	q_tmp = ((uint64_t)lfpuint(r) << 32) + lfpfrac(r);
	M_isneg = M_ISNEG(lfpuint(r));
	if (M_isneg) {
		q_tmp = ~q_tmp + 1;
	}
	d = ldexp((double)q_tmp, -32);
	if (M_isneg) {
		d = -d;
	}
	return d;
}

/*
 * Prototypes
 */
extern	char *	dofptoa		(u_fp, bool, short, bool);
extern	char *	dolfptoa	(l_fp, bool, short, bool);
extern	char *	mfptoa		(l_fp, short);
extern	char *	mfptoms		(l_fp, short);

extern	bool	atolfp		(const char *, l_fp *);
extern	char *	fptoa		(s_fp, short);
extern	char *	fptoms		(s_fp, short);
extern	bool	hextolfp	(const char *, l_fp *);
extern  void	gpstolfp	(int, int, unsigned long, l_fp *);
extern	bool	mstolfp		(const char *, l_fp *);
extern	char *	prettydate	(l_fp *);
extern	char *	gmprettydate	(l_fp *);
extern	char *	gmprettydate	(l_fp *);
extern	char *	rfc3339date	(l_fp *);
extern  l_fp	mfp_mul		(l_fp, int32_t, uint32_t);

extern	void	set_sys_fuzz	(double);
extern  void	get_ostime	(struct timespec *tsp);
extern	void	normalize_time	(struct timespec, long, l_fp *);
extern	void	get_systime	(l_fp *);
extern	bool	step_systime	(double, int (*settime)(struct timespec *));
extern	bool	adj_systime	(double, int (*adjtime)(const struct timeval *, struct timeval *));

#define	lfptoa(fpv, ndec)	mfptoa((fpv), (ndec))
#define	lfptoms(fpv, ndec)	mfptoms((fpv), (ndec))

#define	ufptoa(fpv, ndec)	dofptoa((fpv), false, (ndec), false)
#define	ufptoms(fpv, ndec)	dofptoa((fpv), false, (ndec), true)
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
