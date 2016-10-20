/*
 * ntp_fp.h - definitions for NTP fixed/floating-point arithmetic
 */

#ifndef GUARD_NTP_FP_H
#define GUARD_NTP_FP_H

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
typedef struct {
	union {
		uint32_t Xl_ui;
		int32_t Xl_i;
	} Ul_i;
	uint32_t	l_uf;
} l_fp;

#define l_ui	Ul_i.Xl_ui		/* unsigned integral part */
#define	l_i	Ul_i.Xl_i		/* signed integral part */

static inline uint64_t lfp_to_uint64(const l_fp *lfp) {
  return
    (uint64_t)lfp->l_ui << 32 |
    (uint64_t)lfp->l_uf;
}

static inline void uint64_to_lfp(l_fp *lfp, uint64_t x) {
  lfp->l_ui = x >> 32;
  lfp->l_uf = x & 0xFFFFFFFFUL;
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

#define	NTOHL_MFP(ni, nf, hi, hf)				\
	do {							\
		(hi) = ntohl(ni);				\
		(hf) = ntohl(nf);				\
	} while (false)

#define	HTONL_MFP(hi, hf, ni, nf)				\
	do {							\
		(ni) = htonl(hi);				\
		(nf) = htonl(hf);				\
	} while (false)

#define HTONL_FP(h, n)						\
	HTONL_MFP((h)->l_ui, (h)->l_uf, (n)->l_ui, (n)->l_uf)

#define NTOHL_FP(n, h)						\
	NTOHL_MFP((n)->l_ui, (n)->l_uf, (h)->l_ui, (h)->l_uf)

/* Convert unsigned ts fraction to net order ts */
#define	HTONL_UF(uf, nts)					\
	do {							\
		(nts)->l_ui = 0;				\
		(nts)->l_uf = htonl(uf);			\
	} while (false)

/*
 * Conversions between the two fixed point types
 */
#define	MFPTOFP(x_i, x_f)	(((x_i) >= 0x00010000) ? 0x7fffffff : \
				(((x_i) <= -0x00010000) ? 0x80000000 : \
				(((x_i)<<16) | (((x_f)>>16)&0xffff))))
#define	LFPTOFP(v)		MFPTOFP((v)->l_i, (v)->l_uf)

#define UFPTOLFP(x, v) ((v)->l_ui = (u_fp)(x)>>16, (v)->l_uf = (x)<<16)
#define FPTOLFP(x, v)  (UFPTOLFP((x), (v)), (x) < 0 ? (v)->l_ui -= 0x10000 : 0)

#define MAXLFP(v) ((v)->l_ui = 0x7fffffffu, (v)->l_uf = 0xffffffffu)
#define MINLFP(v) ((v)->l_ui = 0x80000000u, (v)->l_uf = 0u)

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

#define	M_NEGM(r_i, r_f, a_i, a_f)	/* r = -a */ \
	do { \
		(r_f) = ~(a_f) + 1u; \
		(r_i) = ~(a_i) + ((r_f) == 0); \
	} while (false)

#define M_ADD(r_i, r_f, a_i, a_f)	/* r += a */ \
	do { \
		uint32_t add_t = (r_f); \
		(r_f) += (a_f); \
		(r_i) += (a_i) + ((uint32_t)(r_f) < add_t); \
	} while (false)

#define M_ADD3(r_o, r_i, r_f, a_o, a_i, a_f) /* r += a, three word */ \
	do { \
		uint32_t add_t, add_c; \
		add_t  = (r_f); \
		(r_f) += (a_f); \
		add_c  = ((uint32_t)(r_f) < add_t); \
		(r_i) += add_c; \
		add_c  = ((uint32_t)(r_i) < add_c); \
		add_t  = (r_i); \
		(r_i) += (a_i); \
		add_c |= ((uint32_t)(r_i) < add_t); \
		(r_o) += (a_o) + add_c; \
	} while (false)

#define M_SUB(r_i, r_f, a_i, a_f)	/* r -= a */ \
	do { \
		uint32_t sub_t = (r_f); \
		(r_f) -= (a_f); \
		(r_i) -= (a_i) + ((uint32_t)(r_f) > sub_t); \
	} while (false)

#define	M_RSHIFTU(v_i, v_f)		/* v >>= 1, v is unsigned */ \
	do { \
		(v_f) = ((uint32_t)(v_f) >> 1) | ((uint32_t)(v_i) << 31);	\
		(v_i) = ((uint32_t)(v_i) >> 1); \
	} while (false)

#define	M_RSHIFT(v_i, v_f)		/* v >>= 1, v is signed */ \
	do { \
		(v_f) = ((uint32_t)(v_f) >> 1) | ((uint32_t)(v_i) << 31);	\
		(v_i) = ((uint32_t)(v_i) >> 1) | ((uint32_t)(v_i) & 0x80000000);	\
	} while (false)

#define	M_LSHIFT(v_i, v_f)		/* v <<= 1 */ \
	do { \
		(v_i) = ((uint32_t)(v_i) << 1) | ((uint32_t)(v_f) >> 31);	\
		(v_f) = ((uint32_t)(v_f) << 1); \
	} while (false)

#define	M_LSHIFT3(v_o, v_i, v_f)	/* v <<= 1, with overflow */ \
	do { \
		(v_o) = ((uint32_t)(v_o) << 1) | ((uint32_t)(v_i) >> 31);	\
		(v_i) = ((uint32_t)(v_i) << 1) | ((uint32_t)(v_f) >> 31);	\
		(v_f) = ((uint32_t)(v_f) << 1); \
	} while (false)

#define	M_ADDUF(r_i, r_f, uf)		/* r += uf, uf is uint32_t fraction */ \
	M_ADD((r_i), (r_f), 0, (uf))	/* let optimizer worry about it */

#define	M_SUBUF(r_i, r_f, uf)		/* r -= uf, uf is uint32_t fraction */ \
	M_SUB((r_i), (r_f), 0, (uf))	/* let optimizer worry about it */

#define	M_ADDF(r_i, r_f, f)		/* r += f, f is a int32_t fraction */ \
	do { \
		int32_t add_f = (int32_t)(f); \
		if (add_f >= 0) \
			M_ADD((r_i), (r_f), 0, (uint32)( add_f)); \
		else \
			M_SUB((r_i), (r_f), 0, (uint32)(-add_f)); \
	} while(0)

#define	M_ISNEG(v_i)			/* v < 0 */ \
	(((v_i) & 0x80000000) != 0)

#define	M_ISGT(a_i, a_f, b_i, b_f)	/* a > b signed */ \
	(((uint32_t)((a_i) ^ 0x80000000) > (uint32_t)((b_i) ^ 0x80000000)) || \
	  ((a_i) == (b_i) && ((uint32_t)(a_f)) > ((uint32_t)(b_f))))

#define	M_ISGTU(a_i, a_f, b_i, b_f)	/* a > b unsigned */ \
	(((uint32_t)(a_i)) > ((uint32_t)(b_i)) || \
	  ((a_i) == (b_i) && ((uint32_t)(a_f)) > ((uint32_t)(b_f))))

#define	M_ISHIS(a_i, a_f, b_i, b_f)	/* a >= b unsigned */ \
	(((uint32_t)(a_i)) > ((uint32_t)(b_i)) || \
	  ((a_i) == (b_i) && ((uint32_t)(a_f)) >= ((uint32_t)(b_f))))

#define	M_ISGEQ(a_i, a_f, b_i, b_f)	/* a >= b signed */ \
	(((uint32_t)((a_i) ^ 0x80000000) > (uint32_t)((b_i) ^ 0x80000000)) || \
	  ((a_i) == (b_i) && (uint32_t)(a_f) >= (uint32_t)(b_f)))

#define	M_ISEQU(a_i, a_f, b_i, b_f)	/* a == b unsigned */ \
	((uint32_t)(a_i) == (uint32_t)(b_i) && (uint32_t)(a_f) == (uint32_t)(b_f))

/*
 * Operations on the long fp format
 */
#define	L_ADD(r, a)	M_ADD((r)->l_ui, (r)->l_uf, (a)->l_ui, (a)->l_uf)
#define	L_SUB(r, a)	M_SUB((r)->l_ui, (r)->l_uf, (a)->l_ui, (a)->l_uf)
#define	L_NEG(v)	M_NEG((v)->l_ui, (v)->l_uf)
#define L_ADDUF(r, uf)	M_ADDUF((r)->l_ui, (r)->l_uf, (uf))
#define L_SUBUF(r, uf)	M_SUBUF((r)->l_ui, (r)->l_uf, (uf))
#define	L_ADDF(r, f)	M_ADDF((r)->l_ui, (r)->l_uf, (f))
#define	L_RSHIFT(v)	M_RSHIFT((v)->l_i, (v)->l_uf)
#define	L_RSHIFTU(v)	M_RSHIFTU((v)->l_ui, (v)->l_uf)
#define	L_LSHIFT(v)	M_LSHIFT((v)->l_ui, (v)->l_uf)
#define	L_CLR(v)	((v)->l_ui = (v)->l_uf = 0)

#define	L_ISNEG(v)	M_ISNEG((v)->l_ui)
#define L_ISZERO(v)	(((v)->l_ui | (v)->l_uf) == 0)
#define	L_ISGT(a, b)	M_ISGT((a)->l_i, (a)->l_uf, (b)->l_i, (b)->l_uf)
#define	L_ISGTU(a, b)	M_ISGTU((a)->l_ui, (a)->l_uf, (b)->l_ui, (b)->l_uf)
#define	L_ISHIS(a, b)	M_ISHIS((a)->l_ui, (a)->l_uf, (b)->l_ui, (b)->l_uf)
#define	L_ISGEQ(a, b)	M_ISGEQ((a)->l_ui, (a)->l_uf, (b)->l_ui, (b)->l_uf)
#define L_ISGEQU(a, b)  L_ISHIS(a, b)
#define	L_ISEQU(a, b)	M_ISEQU((a)->l_ui, (a)->l_uf, (b)->l_ui, (b)->l_uf)

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

/*
 * Use 64 bit integers if available.
 * XSCALE generates bad code for these, at least with GCC 3.3.5.
 */
#if !(defined(__arm__) && defined(__XSCALE__) && defined(__GNUC__))

#include <math.h>	/* ldexp() */

#define M_DTOLFP(d, r_ui, r_uf)		/* double to l_fp */	\
	do {							\
		double	d_tmp;					\
		uint64_t	q_tmp;					\
		int	M_isneg;					\
								\
		d_tmp = (d);					\
		M_isneg = (d_tmp < 0.);				\
		if (M_isneg) {					\
			d_tmp = -d_tmp;				\
		}						\
		q_tmp = (uint64_t)ldexp(d_tmp, 32);		\
		if (M_isneg) {					\
			q_tmp = ~q_tmp + 1;			\
		}						\
		(r_uf) = (uint32_t)q_tmp;			\
		(r_ui) = (uint32_t)(q_tmp >> 32);		\
	} while (false)

#define M_LFPTOD(r_ui, r_uf, d) 	/* l_fp to double */	\
	do {							\
		double	d_tmp;					\
		uint64_t	q_tmp;					\
		int	M_isneg;				\
								\
		q_tmp = ((uint64_t)(r_ui) << 32) + (r_uf);	\
		M_isneg = M_ISNEG(r_ui);			\
		if (M_isneg) {					\
			q_tmp = ~q_tmp + 1;			\
		}						\
		d_tmp = ldexp((double)q_tmp, -32);		\
		if (M_isneg) {					\
			d_tmp = -d_tmp;				\
		}						\
		(d) = d_tmp;					\
	} while (false)

#else /* use only 32 bit unsigned values */

#define M_DTOLFP(d, r_ui, r_uf) 		/* double to l_fp */ \
	do { \
		double d_tmp; \
		if ((d_tmp = (d)) < 0) { \
			(r_ui) = (uint32_t)(-d_tmp); \
			(r_uf) = (uint32_t)(-(d_tmp + (double)(r_ui)) * FRAC); \
			M_NEG((r_ui), (r_uf)); \
		} else { \
			(r_ui) = (uint32_t)d_tmp; \
			(r_uf) = (uint32_t)((d_tmp - (double)(r_ui)) * FRAC); \
		} \
	} while (0)
#define M_LFPTOD(r_ui, r_uf, d) 		/* l_fp to double */ \
	do { \
		uint32_t l_thi, l_tlo; \
		l_thi = (r_ui); l_tlo = (r_uf); \
		if (M_ISNEG(l_thi)) { \
			M_NEG(l_thi, l_tlo); \
			(d) = -((double)l_thi + (double)l_tlo / FRAC); \
		} else { \
			(d) = (double)l_thi + (double)l_tlo / FRAC; \
		} \
	} while (0)
#endif

#define DTOLFP(d, v) 	M_DTOLFP((d), (v)->l_ui, (v)->l_uf)
#define LFPTOD(v, d) 	M_LFPTOD((v)->l_ui, (v)->l_uf, (d))

/*
 * Prototypes
 */
extern	char *	dofptoa		(u_fp, bool, short, bool);
extern	char *	dolfptoa	(uint32_t, uint32_t, bool, short, bool);

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
extern  void	mfp_mul		(int32_t *, uint32_t *, int32_t, uint32_t, int32_t, uint32_t);

extern	void	set_sys_fuzz	(double);
extern  void	get_ostime	(struct timespec *tsp);
extern	void	normalize_time	(struct timespec, long, l_fp *);
extern	void	get_systime	(l_fp *);
extern	bool	step_systime	(double, int (*settime)(struct timespec *));
extern	bool	adj_systime	(double, int (*adjtime)(const struct timeval *, struct timeval *));

#define	lfptoa(fpv, ndec)	mfptoa((fpv)->l_ui, (fpv)->l_uf, (ndec))
#define	lfptoms(fpv, ndec)	mfptoms((fpv)->l_ui, (fpv)->l_uf, (ndec))

#define	ufptoa(fpv, ndec)	dofptoa((fpv), false, (ndec), false)
#define	ufptoms(fpv, ndec)	dofptoa((fpv), false, (ndec), true)
#define	ulfptoa(fpv, ndec)	dolfptoa((fpv)->l_ui, (fpv)->l_uf, 0, (ndec), 0)
#define	ulfptoms(fpv, ndec)	dolfptoa((fpv)->l_ui, (fpv)->l_uf, 0, (ndec), 1)
#define	umfptoa(fpi, fpf, ndec) dolfptoa((fpi), (fpf), 0, (ndec), 0)

/*
 * Optional callback from libntp step_systime() to ntpd.  Optional
*  because other libntp clients like ntpdate don't use it.
 */
typedef void (*time_stepped_callback)(void);
extern time_stepped_callback	step_callback;

#endif /* GUARD_NTP_FP_H */
