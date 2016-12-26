#ifndef GUARD_NTP_TESTS_LFPTEST_H
#define GUARD_NTP_TESTS_LFPTEST_H

#include "ntp_fp.h"

static bool IsEqual(const l_fp *expected, const l_fp *actual) {
	if (L_ISEQU(expected, actual)) {
		return true;
	} else {
		printf("Expected: %s (%d.%d) but was: %s (%d.%d)\n", lfptoa(*expected, FRACTION_PREC), lfpuint(*expected), lfpfrac(*expected), lfptoa(*actual, FRACTION_PREC), lfpuint(*actual), lfpfrac(*actual));
		return false;
	}
}

static const int32_t HALF = -2147483647L - 1L;
static const int32_t HALF_PROMILLE_UP = 2147484; // slightly more than 0.0005
static const int32_t HALF_PROMILLE_DOWN = 2147483; // slightly less than 0.0005
static const int32_t QUARTER = 1073741824L;
static const int32_t QUARTER_PROMILLE_APPRX = 1073742L;

#endif /* GUARD_NTP_TESTS_LFPTEST_H */

