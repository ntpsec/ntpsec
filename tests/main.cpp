#include "main.h"

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);

	// Some tests makes use of an extra parameter passed to the tests
	// executable. Save this param as a static member of the base class.
	if (argc > 1) {
		ntptest::SetExtraParam(argv[1]);
	}
	
	return RUN_ALL_TESTS();
}

std::string ntptest::m_param = "";

void ntptest::SetExtraParam(const char* param)
{
	m_param = param;
}
