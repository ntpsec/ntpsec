#ifndef NTP_TESTS_LFPTEST_H
#define NTP_TESTS_LFPTEST_H

#include "libntptest.h"

extern "C" {
#include "ntp_fp.h"
};

class lfptest : public libntptest {
protected:
	::testing::AssertionResult IsEqual(const l_fp &expected, const l_fp &actual) {
		if (L_ISEQU(&expected, &actual)) {
			return ::testing::AssertionSuccess();
		} else {
			return ::testing::AssertionFailure()
				<< " expected: " << lfptoa(&expected, FRACTION_PREC)
				<< " but was: " << lfptoa(&actual, FRACTION_PREC);
		}
	}
};

#endif /* NTP_TESTS_LFPTEST_H */
