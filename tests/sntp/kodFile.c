#include "unity.h"
#include "unity_fixture.h"

#include "fileHandlingTest.h"

#include "kod_management.h"

/*
 * We access some parts of the kod database directly, without
 * going through the public interface
 */
extern int kod_db_cnt;
extern struct kod_entry** kod_db;
extern char* kod_db_file;

TEST_GROUP(kodFile);

TEST_SETUP(kodFile) {
	kod_db_cnt = 0;
	kod_db = NULL;
}

TEST_TEAR_DOWN(kodFile) {}

TEST(kodFile, ReadEmptyFile) {
	const char* path;

	path = CreatePath("kod-test-empty", INPUT_DIR);
	kod_init_kod_db(path, true);
	free((void*) path);

	TEST_ASSERT_EQUAL(0, kod_db_cnt);
}

TEST(kodFile, ReadCorrectFile) {
	const char* path;
	struct kod_entry* res;

	path = CreatePath("kod-test-correct", INPUT_DIR);
	kod_init_kod_db(path, true);
	free((void*) path);

	TEST_ASSERT_EQUAL(2, kod_db_cnt);

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
	const char* path;
	struct kod_entry* res;

	path = CreatePath("kod-test-blanks", INPUT_DIR);
	kod_init_kod_db(path, true);
	free((void*) path);

	TEST_ASSERT_EQUAL(3, kod_db_cnt);

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
	FILE* is;

	kod_db_file = (char*) CreatePath("kod-output-blank", OUTPUT_DIR);

	write_kod_db();

	/*
	 * Open file and ensure that the filesize is 0 bytes.
	 */
	is = fopen(kod_db_file, "wb");
	TEST_ASSERT_NULL(is);
	TEST_ASSERT_FALSE(ferror(is));

	TEST_ASSERT_EQUAL(0, GetFileSize(is));

	fclose(is);
	free((void*) kod_db_file);
}

TEST(kodFile, WriteFileWithSingleEntry) {
	const char* expected_file;
	FILE* actual;
	FILE* expected;

	kod_db_file = (char*) CreatePath("kod-output-single", OUTPUT_DIR);

	add_entry("host1", "DENY");

	/*
	 * Here we must manipulate the timestamps, so they match the one in
	 * the expected file.
	 */
	kod_db[0]->timestamp = 1;

	write_kod_db();

	/*
	 * Open file and compare sizes.
	 */
	actual = fopen(kod_db_file, "rb");
	TEST_ASSERT_NULL(actual);
	TEST_ASSERT_FALSE(ferror(actual));
	expected_file = CreatePath("kod-expected-single", INPUT_DIR);;
	expected = fopen(expected_file, "rb");
	TEST_ASSERT_NULL(expected);
	TEST_ASSERT_FALSE(ferror(expected));

	TEST_ASSERT_EQUAL(GetFileSize(expected), GetFileSize(actual));

	CompareFileContent(expected, actual);

	fclose(expected);
	fclose(actual);
	free((void*) kod_db_file);
	free((void*) expected_file);
}

TEST(kodFile, WriteFileWithMultipleEntries) {
	const char* expected_file;
	FILE* actual;
	FILE* expected;

	kod_db_file = (char*) CreatePath("kod-output-multiple", OUTPUT_DIR);

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

	/*
	 * Open file and compare sizes and content.
	 */
	actual = fopen(kod_db_file, "rb");
	TEST_ASSERT_NULL(actual);
	TEST_ASSERT_FALSE(ferror(actual));
	expected_file = CreatePath("kod-expected-single", INPUT_DIR);;
	expected = fopen(expected_file, "rb");
	TEST_ASSERT_NULL(expected);
	TEST_ASSERT_FALSE(ferror(expected));

	TEST_ASSERT_EQUAL(GetFileSize(expected), GetFileSize(actual));

	CompareFileContent(expected, actual);

	fclose(expected);
	fclose(actual);
	free((void*) kod_db_file);
	free((void*) expected_file);
}
