#include <gtest/gtest.h>

#include "config.h"

#include <string>

class ntptest : public ::testing::Test {
public:
	static void SetExtraParam(const char* param);
protected:
	static std::string m_param;
};
