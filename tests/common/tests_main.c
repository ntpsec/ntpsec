#include "tests_main.h"

const char *progname = "ntpdigtest";

static const char** args_argv;
static int args_argc;

/* XXX: This needs to have named arguments and error
   out if one is not supplied */
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

#ifdef TEST_NTPDIG
	RUN_TEST_GROUP(crypto);
	RUN_TEST_GROUP(keyFile);
//	RUN_TEST_GROUP(kodDatabase);
//	RUN_TEST_GROUP(kodFile);
//	RUN_TEST_GROUP(packetHandling);
#endif

#ifdef TEST_LIBNTP_SSL
	RUN_TEST_GROUP(ssl_init);
#endif

}

int main(int argc, const char * argv[]) {

	init_lib();
	init_auth();

	args_argc = argc;
	args_argv = argv;

	return UnityMain(argc, argv, RunAllTests);
}
