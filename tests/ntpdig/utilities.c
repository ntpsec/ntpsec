#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(utilities);

TEST_SETUP(utilities) {}

TEST_TEAR_DOWN(utilities) {}

#include "ntpdigtest.h"
#include "file_handling.h"

#include "main.h"
#include "utilities.h"

#include "sockaddrtest.h"

const char * Version = "stub unit test Version string";


struct addrinfo CreateAddrinfo(sockaddr_u* sock) {
	struct addrinfo a;
	memset(&a, '\0', sizeof(struct addrinfo));
	a.ai_family = sock->sa.sa_family;
	a.ai_addrlen = SIZEOF_SOCKADDR(a.ai_family);
	a.ai_addr = &sock->sa;
	return a;
}


bool outputFileOpened;
FILE* outputFile;

//	debugUtilitiesTest() : outputFileOpened(false) {}

void InitDebugTest(const char *filename) {
	// Clear the contents of the current file.
	// Open the output file
	outputFile = fopen(filename, "w+");
	TEST_ASSERT_TRUE(outputFile != NULL);
	outputFileOpened = true;
}

// Closes outputFile, and compare contents.
void FinishDebugTest(const char *expected,
					 const char *actual) {
	FILE* fp_a;
	FILE* fp_e;

	if (outputFileOpened) {
		fclose(outputFile);
	}

	fp_a = fopen(actual, "rb");
	fp_e = fopen(expected, "rb");

	if (fp_a != NULL && fp_e != NULL)
	    CompareFileContent(fp_e, fp_a);
	else
	    fprintf(stderr,
		    "FinishDebugTest: file pointer unexpectedly null.\n");

	if (fp_a)
	    fclose(fp_a);
	if (fp_e)
	    fclose(fp_e);

	free((void*) expected);
	free((void*) actual);
}


/*
 * These tests are essentially a copy of the tests for socktoa()
 * in libntp. If ntpdig switches to using that functions, these
 * tests can be removed.
 */

TEST(utilities, IPv4Address) {
	const char* ADDR = "192.0.2.10";

	sockaddr_u input = CreateSockaddr4(ADDR, 123);
	struct addrinfo inputA = CreateAddrinfo(&input);

	/* coverity[leaked_storage] */
	TEST_ASSERT_EQUAL_STRING(ADDR, ss_to_str(&input));
	/* coverity[leaked_storage] */
	TEST_ASSERT_EQUAL_STRING(ADDR, addrinfo_to_str(&inputA));
	/* coverity[leaked_storage] */
}

TEST(utilities, IPv6Address) {
	const struct in6_addr address = {{{
						0x20, 0x01, 0x0d, 0xb8,
						0x85, 0xa3, 0x08, 0xd3,
						0x13, 0x19, 0x8a, 0x2e,
						0x03, 0x70, 0x73, 0x34
					}}};
	const char *expected = "2001:db8:85a3:8d3:1319:8a2e:370:7334";
	sockaddr_u	input;
	struct addrinfo	inputA;

	memset(&input, 0, sizeof(input));
	input.sa6.sin6_family = AF_INET6;
	input.sa6.sin6_addr = address;
	/* coverity[leaked_storage] */
	TEST_ASSERT_EQUAL_STRING(expected, ss_to_str(&input));

	inputA = CreateAddrinfo(&input);
	/* coverity[leaked_storage] */
TEST_ASSERT_EQUAL_STRING(expected, addrinfo_to_str(&inputA));
}

TEST(utilities, SetLiVnMode1) {
	struct pkt expected;
	expected.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOWARNING,
					     NTP_VERSION,
					     MODE_SERVER);

	struct pkt actual;
	set_li_vn_mode(&actual, LEAP_NOWARNING, NTP_VERSION,
				   MODE_SERVER);

	TEST_ASSERT_EQUAL(expected.li_vn_mode, actual.li_vn_mode);
}

TEST(utilities, SetLiVnMode2) {
	struct pkt expected;
	expected.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOTINSYNC,
										 NTP_OLDVERSION,
										 MODE_BROADCAST);

	struct pkt actual;
	set_li_vn_mode(&actual, LEAP_NOTINSYNC, NTP_OLDVERSION,
				   MODE_BROADCAST);

	TEST_ASSERT_EQUAL(expected.li_vn_mode, actual.li_vn_mode);
}

/* Debug utilities tests */

TEST(utilities, DebugPktOutput) {
	const char *filename = CreatePath("debug-output-pkt", OUTPUT_DIR);
	InitDebugTest(filename);

	struct pkt testpkt;
	memset(&testpkt, 0, sizeof(struct pkt));
	testpkt.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOWARNING,
										NTP_VERSION,
										MODE_SERVER);

	l_fp test;
	test.l_ui = 8;
	test.l_uf = 2147483647; // Lots of ones.
	HTONL_FP(&test, &testpkt.xmt);

	pkt_output(&testpkt, LEN_PKT_NOMAC, true, outputFile);

	FinishDebugTest(CreatePath("debug-input-pkt", INPUT_DIR), filename);
}

TEST(utilities, DebugLfpOutputBinaryFormat) {
	const char *filename = CreatePath("debug-output-lfp-bin", OUTPUT_DIR);
	InitDebugTest(filename);

	l_fp test;
	test.l_ui = 63;  // 00000000 00000000 00000000 00111111
	test.l_uf = 127; // 00000000 00000000 00000000 01111111

	l_fp network;
	HTONL_FP(&test, &network);

	l_fp_output_bin(&network, outputFile);

	FinishDebugTest(CreatePath("debug-input-lfp-bin", INPUT_DIR), filename);
}

TEST(utilities, DebugLfpOutputDecimalFormat) {
	const char *filename = CreatePath("debug-output-lfp-dec", OUTPUT_DIR);
	InitDebugTest(filename);

	l_fp test;
	test.l_ui = 6310; // 0x000018A6
	test.l_uf = 308502; // 0x00004B516

	l_fp network;
	HTONL_FP(&test, &network);

	l_fp_output_dec(&network, outputFile);

	FinishDebugTest(CreatePath("debug-input-lfp-dec", INPUT_DIR), filename);
}

TEST_GROUP_RUNNER(utilities) {
	RUN_TEST_CASE(utilities, IPv4Address);
	RUN_TEST_CASE(utilities, IPv6Address);
	RUN_TEST_CASE(utilities, SetLiVnMode1);
	RUN_TEST_CASE(utilities, SetLiVnMode2);
	RUN_TEST_CASE(utilities, DebugPktOutput);
	RUN_TEST_CASE(utilities, DebugLfpOutputBinaryFormat);
	RUN_TEST_CASE(utilities, DebugLfpOutputDecimalFormat);
}
