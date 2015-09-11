#ifndef GUARD_TESTS_MAIN_H
#define GUARD_TESTS_MAIN_H

#include "config.h"

#include <string>
#include <vector>

#include <gtest/gtest.h>

extern "C" {
#include "ntp_stdlib.h"
}

class ntptest : public ::testing::Test {
public:
	static void SetExtraParams(int start, int count, char** argv);
protected:
	static std::vector<std::string> m_params;
};

#endif // GUARD_TESTS_MAIN_H
