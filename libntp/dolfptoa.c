/*
 * dolfptoa - do the grunge work of converting an l_fp number to decimal
 *
 * Warning: this conversion is lossy in the low-order bits of the fractional
 * part.  It's good enough for statistics and logging, but do not expect
 * it to round-trip through atolfp(). 1444359386.1798776096, for example, may
 * dump as ...095 or ...097.
 */
#include "config.h"
#include <stdio.h>
#include <string.h>

#include "ntp_fp.h"
#include "lib_strbuf.h"
#include "ntp_stdlib.h"

char *
dolfptoa(
	l_fp lfp,
	bool neg,
	short ndec,
	bool msec
	)
{
	uint32_t fpi = lfpuint(lfp);
	uint32_t fpv = lfpfrac(lfp);
	uint8_t *cp, *cpend, *cpdec;
	int dec;
	uint8_t cbuf[24];
	char *buf, *bp;

	/*
	 * Get a string buffer before starting
	 */
	buf = lib_getbuf();

	/*
	 * Zero the character buffer
	 */
	ZERO(cbuf);

	/*
	 * Work on the integral part. This should work reasonable on
	 * all machines with 32 bit arithmetic. Please note that 32 bits
	 * can *always* be represented with at most 10 decimal digits,
	 * including a possible rounding from the fractional part.
	 */
	cp = cpend = cpdec = &cbuf[10];
	for (dec = cp - cbuf; dec > 0 && fpi != 0; dec--) {
		/* can add another digit */
		uint32_t digit;

		digit  = fpi;
		fpi   /= 10U;
		/*
		 * This should be able to be replaced by [digit -= fpi * 10].
		 * It is being left as is at the moment for subtle bug avoidance.
		 */
		digit -= (fpi << 3) + (fpi << 1); /* i*10 */
		*--cp  = (uint8_t)digit;
	}

	/*
	 * Done that, now deal with the problem of the fraction.  First
	 * determine the number of decimal places.
	 */
	dec = ndec;
	if (dec < 0)
		dec = 0;
	if (msec) {
		dec   += 3;
		cpdec += 3;
	}
	if (dec > (long)sizeof(cbuf) - (cpend - cbuf))
		dec = (long)sizeof(cbuf) - (cpend - cbuf);

	/*
	 * If there's a fraction to deal with, do so.
	 */
	for (/*NOP*/;  dec > 0 && fpv != 0;  dec--)  {
		uint32_t digit, tmph, tmpl;

		/* FIXME - get rid of this ugly kludge! */
#define M_ADD(r_i, r_f, a_i, a_f)	/* r += a */ \
		do { \
			uint32_t add_t = (r_f); \
			(r_f) += (a_f); \
			(r_i) += (a_i) + ((uint32_t)(r_f) < add_t); \
		} while (false)

#define	M_LSHIFT(v_i, v_f)		/* v <<= 1 */ \
		do { \
			(v_i) = ((uint32_t)(v_i) << 1) | ((uint32_t)(v_f) >> 31);	\
			(v_f) = ((uint32_t)(v_f) << 1); \
		} while (false)

		/*
		 * The scheme here is to multiply the fraction
		 * (0.1234...) by ten.  This moves a junk of BCD into
		 * the units part.  record that and iterate.
		 * multiply by shift/add in two dwords.
		 */
		digit = 0;
		M_LSHIFT(digit, fpv);
		tmph = digit;
		tmpl = fpv;
		M_LSHIFT(digit, fpv);
		M_LSHIFT(digit, fpv);
		M_ADD(digit, fpv, tmph, tmpl);
#undef M_ADD
#undef M_LSHIFT
		*cpend++ = (uint8_t)digit;
	}

	/* decide whether to round or simply extend by zeros */
	if (dec > 0) {
		/* only '0' digits left -- just reposition end */
		cpend += dec;
	} else {
		/* some bits remain in 'fpv'; do round */
		uint8_t *tp    = cpend;
		int     carry = ((fpv & 0x80000000) != 0);

		for (dec = tp - cbuf;  carry && dec > 0;  dec--) {
			*--tp += 1;
			if (*tp == 10)
				*tp = 0;
			else
				carry = false;
		}

		if (tp < cp) /* rounding from 999 to 1000 or similar? */
			cp = tp;
	}

	/*
	 * We've now got the fraction in cbuf[], with cp pointing at
	 * the first character, cpend pointing past the last, and
	 * cpdec pointing at the first character past the decimal.
	 * Remove leading zeros, then format the number into the
	 * buffer.
	 */
	while (cp < cpdec && *cp == 0)
		cp++;
	if (cp >= cpdec)
		cp = cpdec - 1;

	bp = buf;
	if (neg)
		*bp++ = '-';
	while (cp < cpend) {
		if (cp == cpdec)
			*bp++ = '.';
		*bp++ = (char)(*cp++) + '0';
	}
	*bp = '\0';

	/*
	 * Done!
	 */
	return buf;
}


char *
mfptoa(
	l_fp	lfp,
	short	ndec
	)
{
	bool	isneg = L_ISNEG(lfp);

	if (isneg) {
		L_NEG(lfp);
	}

	return dolfptoa(lfp, isneg, ndec, false);
}


char *
mfptoms(
	l_fp	lfp,
	short	ndec
	)
{
	bool	isneg = L_ISNEG(lfp);

	if (isneg) {
		L_NEG(lfp);
	}

	return dolfptoa(lfp, isneg, ndec, true);
}


