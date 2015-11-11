#ifndef GUARD_NTPDIGTEST_H
#define GUARD_NTPDIGTEST_H

#include "tests_main.h"

#include "ntp_stdlib.h"

#if 0
class ntpdigtest : public ntptest {
protected:
	ntpdigtest() {
		optionSaveState(&ntpdigOptions);
	}

	~ntpdigtest() {
		optionRestore(&ntpdigOptions);
	}

	void ActivateOption(const char* option, const char* argument) {
		const int ARGV_SIZE = 4;

		char* opts[ARGV_SIZE];

		opts[0] = estrdup("ntpdigopts");
		opts[1] = estrdup(option);
		opts[2] = estrdup(argument);
		opts[3] = estrdup("127.0.0.1");

		optionProcess(&ntpdigOptions, ARGV_SIZE, opts);
	}
};
#endif

#endif // GUARD_NTPDIGTEST_H
