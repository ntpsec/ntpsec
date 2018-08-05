

#include "tests_main.h"
const char *progname = "ntpsectest";

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
	syslogit = false;
	termlogit = false;

#ifdef TEST_NTPDIG
	RUN_TEST_GROUP(crypto);
	RUN_TEST_GROUP(keyFile);
	RUN_TEST_GROUP(kodDatabase);
	RUN_TEST_GROUP(kodFile);
	RUN_TEST_GROUP(packetHandling);
	RUN_TEST_GROUP(packetProcessing);
	RUN_TEST_GROUP(utilities);
#endif

#ifdef TEST_LIBNTP
	RUN_TEST_GROUP(authkeys);
	RUN_TEST_GROUP(calendar);
	RUN_TEST_GROUP(clocktime);
	RUN_TEST_GROUP(endian);
	RUN_TEST_GROUP(decodenetnum);
	RUN_TEST_GROUP(dolfptoa);
	RUN_TEST_GROUP(hextolfp);
	RUN_TEST_GROUP(lfpfunc);
	RUN_TEST_GROUP(lfptostr);
	RUN_TEST_GROUP(macencrypt);
	RUN_TEST_GROUP(msyslog);
	RUN_TEST_GROUP(netof6);
	RUN_TEST_GROUP(numtoa);
	RUN_TEST_GROUP(prettydate);
	RUN_TEST_GROUP(random);
	RUN_TEST_GROUP(refidsmear);
	RUN_TEST_GROUP(socktoa);
	RUN_TEST_GROUP(statestr);
	RUN_TEST_GROUP(strtolfp);
	RUN_TEST_GROUP(timespecops);
	RUN_TEST_GROUP(vi64ops);
	RUN_TEST_GROUP(ymd2yd);
#endif

#ifdef TEST_LIBPARSE
	RUN_TEST_GROUP(binio);
	RUN_TEST_GROUP(gpstolfp);
	RUN_TEST_GROUP(ieee754io);
#endif

#ifdef TEST_NTPD
	RUN_TEST_GROUP(leapsec);
	RUN_TEST_GROUP(hackrestrict);
	RUN_TEST_GROUP(recvbuff);
#endif

}


int main(int argc, const char * argv[]) {

	ssl_init();
	auth_init();
	init_network();

	args_argc = argc;
	args_argv = argv;

	return UnityMain(argc, argv, RunAllTests);
}
