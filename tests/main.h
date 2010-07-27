#include <gtest/gtest.h>

#include "config.h"

#include <string>
#include <vector>

class ntptest : public ::testing::Test {
public:
	static void SetExtraParams(int start, int count, char** argv);
protected:
	static std::vector<std::string> m_params;
};
