#ifndef GUARD_NTP_TESTS_LFPTEST_H
#define GUARD_NTP_TESTS_LFPTEST_H

#include "ntp_fp.h"


#define Frac10(x) ((unsigned long long)(ldexp(lfpfrac(x), -32)*1E10))

static bool IsEqual(const l_fp *expected, const l_fp *actual) {
	if (*expected == *actual) {
		return true;
	}
	printf("Expected: 0x%8x.%08x (%u.%010llu) but was: 0x%8x.%08x (%u.%010llu)\n",
		lfpuint(*expected), lfpfrac(*expected), \
		lfpuint(*expected), Frac10(*expected), \
		lfpuint(*actual), lfpfrac(*actual), \
		lfpuint(*actual), Frac10(*actual));
	return false;
}

static const uint32_t HALF = 2147483648U;           // (1 << 31)
static const uint32_t HALF_PROMILLE_UP = 2147484;   // slightly more than 0.0005
static const uint32_t HALF_PROMILLE_DOWN = 2147483; // slightly less than 0.0005
static const uint32_t QUARTER = 1073741824U;        // (1 << 30)
static const uint32_t QUARTER_PROMILLE_APPRX = 1073742U;

#endif /* GUARD_NTP_TESTS_LFPTEST_H */

