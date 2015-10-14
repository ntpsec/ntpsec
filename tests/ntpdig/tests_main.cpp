#include "tests_main.h"

const char *progname = "ntpdigtest";

static void RunAllTests(void) {
	RUN_TEST_GROUP(crypto);
	RUN_TEST_GROUP(keyFile);
}

int main(int argc, const char * argv[]) {

	init_lib();
	init_auth();

	return UnityMain(argc, argv, RunAllTests);
}
