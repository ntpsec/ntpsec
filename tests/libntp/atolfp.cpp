#include "libntptest.h"

extern "C" {
#include "ntp_fp.h"
#include "ntp_stdlib.h"
};

class atolfpTest : public libntptest {
};

TEST_F(atolfpTest, PositiveInteger) {
	const char *str = "500";
	l_fp expected = {500,0};
}
