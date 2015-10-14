extern "C" {
#include "unity.h"
#include "unity_fixture.h"
}

TEST_GROUP(kodFile);

TEST_SETUP(kodFile) {}

TEST_TEAR_DOWN(kodFile) {}

#include "fileHandlingTest.h"

extern "C" {
#include "kod_management.h"

#include "ntp_stdlib.h" // For estrdup()
};

/*
 * We access some parts of the kod database directly, without
 * going through the public interface
 */
extern int kod_db_cnt;
extern kod_entry** kod_db;
extern char* kod_db_file;

class kodFileTest : public fileHandlingTest {
protected:
	virtual void SetUp() {
		kod_db_cnt = 0;
		kod_db = NULL;
	}

	virtual void TearDown() {
	}
};

TEST(kodFile, ReadEmptyFile) {
	kod_init_kod_db(CreatePath("kod-test-empty", INPUT_DIR).c_str(), true);

	TEST_ASSERT_EQUAL(0, kod_db_cnt);
}

TEST(kodFile, ReadCorrectFile) {
	kod_init_kod_db(CreatePath("kod-test-correct", INPUT_DIR).c_str(), true);

	TEST_ASSERT_EQUAL(2, kod_db_cnt);

	kod_entry* res;

	TEST_ASSERT_EQUAL(1, search_entry("192.0.2.5", &res));
	TEST_ASSERT_EQUAL_STRING("DENY", res->type);
	TEST_ASSERT_EQUAL_STRING("192.0.2.5", res->hostname);
	TEST_ASSERT_EQUAL(0x12345678, res->timestamp);

	TEST_ASSERT_EQUAL(1, search_entry("192.0.2.100", &res));
	TEST_ASSERT_EQUAL_STRING("RSTR", res->type);
	TEST_ASSERT_EQUAL_STRING("192.0.2.100", res->hostname);
	TEST_ASSERT_EQUAL(0xfff, res->timestamp);
}

TEST(kodFile, ReadFileWithBlankLines) {
	kod_init_kod_db(CreatePath("kod-test-blanks", INPUT_DIR).c_str(), true);

	TEST_ASSERT_EQUAL(3, kod_db_cnt);

	kod_entry* res;

	TEST_ASSERT_EQUAL(1, search_entry("192.0.2.5", &res));
	TEST_ASSERT_EQUAL_STRING("DENY", res->type);
	TEST_ASSERT_EQUAL_STRING("192.0.2.5", res->hostname);
	TEST_ASSERT_EQUAL(0x12345678, res->timestamp);

	TEST_ASSERT_EQUAL(1, search_entry("192.0.2.100", &res));
	TEST_ASSERT_EQUAL_STRING("RSTR", res->type);
	TEST_ASSERT_EQUAL_STRING("192.0.2.100", res->hostname);
	TEST_ASSERT_EQUAL(0xfff, res->timestamp);

	TEST_ASSERT_EQUAL(1, search_entry("example.com", &res));
	TEST_ASSERT_EQUAL_STRING("DENY", res->type);
	TEST_ASSERT_EQUAL_STRING("example.com", res->hostname);
	TEST_ASSERT_EQUAL(0xabcd, res->timestamp);
}

TEST(kodFile, WriteEmptyFile) {
	kod_db_file = estrdup(CreatePath("kod-output-blank", OUTPUT_DIR).c_str());

	write_kod_db();

	// Open file and ensure that the filesize is 0 bytes.
	std::ifstream is(kod_db_file, std::ios::binary);
	TEST_ASSERT_FALSE(is.fail());

	TEST_ASSERT_EQUAL(0, GetFileSize(is));

	is.close();
}

TEST(kodFile, WriteFileWithSingleEntry) {
	kod_db_file = estrdup(CreatePath("kod-output-single", OUTPUT_DIR).c_str());

	add_entry("host1", "DENY");

	/* Here we must manipulate the timestamps, so they match the one in
	 * the expected file.
	 */
	kod_db[0]->timestamp = 1;

	write_kod_db();

	// Open file and compare sizes.
	ifstream actual(kod_db_file, ios::binary);
	ifstream expected(CreatePath("kod-expected-single", INPUT_DIR).c_str());
	TEST_ASSERT_TRUE(actual.good());
	TEST_ASSERT_TRUE(expected.good());

	TEST_ASSERT_EQUAL(GetFileSize(expected), GetFileSize(actual));

	CompareFileContent(expected, actual);
}

TEST(kodFile, WriteFileWithMultipleEntries) {
	kod_db_file = estrdup(CreatePath("kod-output-multiple", OUTPUT_DIR).c_str());

	add_entry("example.com", "RATE");
	add_entry("192.0.2.1", "DENY");
	add_entry("192.0.2.5", "RSTR");

	/*
	 * Manipulate timestamps. This is a bit of a hack, ideally these
	 * tests should not care about the internal representation.
	 */
	kod_db[0]->timestamp = 0xabcd;
	kod_db[1]->timestamp = 0xabcd;
	kod_db[2]->timestamp = 0xabcd;

	write_kod_db();

	// Open file and compare sizes and content.
	ifstream actual(kod_db_file, ios::binary);
	ifstream expected(CreatePath("kod-expected-multiple", INPUT_DIR).c_str());
	TEST_ASSERT_TRUE(actual.good());
	TEST_ASSERT_TRUE(expected.good());

	TEST_ASSERT_EQUAL(GetFileSize(expected), GetFileSize(actual));

	CompareFileContent(expected, actual);
}
