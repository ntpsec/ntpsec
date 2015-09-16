#include "tests_main.h"

const char *progname = "sntptest";

static const char** args_argv;
static int args_argc;

const char* tests_main_args(int arg)
{
	int a;
	int count = 0;
	for (a = 1; a < args_argc; ++a) {
		if (args_argv[a][0] != '-') {
			if (count == arg) {
				return args_argv[a];
			}
			++count;
		}
	}
	return NULL;
}

static void RunAllTests(void)
{
	RUN_TEST_GROUP(crypto);
	RUN_TEST_GROUP(keyFile);
}

int main(int argc, const char * argv[]) {

	init_lib();
	init_auth();

	args_argc = argc;
	args_argv = argv;

	return UnityMain(argc, argv, RunAllTests);
}
