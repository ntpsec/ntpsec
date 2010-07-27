#include "sntptest.h"

extern "C" {
#include "kod_management.h"

#include "ntp_stdlib.h" // For estrdup()
};

#include <fstream>
using std::ifstream;
using std::ios;
using std::string;

/*
 * We access some parts of the kod database directly, without
 * going through the public interface
 */
extern int kod_db_cnt;
extern kod_entry** kod_db;
extern char* kod_db_file;

class kodFileTest : public sntptest {
protected:
	enum DirectoryType {
		INPUT_DIR = 0,
		OUTPUT_DIR = 1
	};

	std::string CreatePath(const char* filename, DirectoryType argument) {
		std::string path;

		if (m_params.size() >= argument + 1) {
			path = m_params[argument];
		}

		if (path[path.size()-1] != DIR_SEP && !path.empty()) {
			path.append(1, DIR_SEP);
		}
		path.append(filename);

		return path;
	}

	virtual void SetUp() {
		kod_db_cnt = 0;
		kod_db = NULL;
	}

	virtual void TearDown() {
		/* 
		 * When calling kod_init_kod_db(), it will place an exit handler
		 * that runs write_kod_db() when the test executable stops running.
		 * To prevent write_kod_db() from messing up our pre-written input
		 * files, kod_db_file is set to /dev/null at the end of every test.
		 */

		kod_db_file = estrdup("/dev/null");
	}

	int GetFileSize(ifstream& file) {
		int initial = file.tellg();

		file.seekg(0, std::ios::end);
		int length = file.tellg();
		file.seekg(initial);

		return length;
	}

	void CompareFileContent(ifstream& expected, ifstream& actual) {
		int currentLine = 1;
		while (actual.good() && expected.good()) {
			string actualLine, expectedLine;
			getline(actual, actualLine);
			getline(expected, expectedLine);
			
			EXPECT_EQ(expectedLine, actualLine) << "Comparision failed on line " << currentLine;
			currentLine++;
		}
	}
};

TEST_F(kodFileTest, ReadEmptyFile) {
	kod_init_kod_db(CreatePath("kod-test-empty", INPUT_DIR).c_str());

	EXPECT_EQ(0, kod_db_cnt);
}

TEST_F(kodFileTest, ReadCorrectFile) {
	kod_init_kod_db(CreatePath("kod-test-correct", INPUT_DIR).c_str());
	
	EXPECT_EQ(2, kod_db_cnt);

	kod_entry* res;

	ASSERT_EQ(1, search_entry("192.0.2.5", &res));
	EXPECT_STREQ("DENY", res->type);
	EXPECT_STREQ("192.0.2.5", res->hostname);
	EXPECT_EQ(0x1234567890, res->timestamp);

	ASSERT_EQ(1, search_entry("192.0.2.100", &res));
	EXPECT_STREQ("RSTR", res->type);
	EXPECT_STREQ("192.0.2.100", res->hostname);
	EXPECT_EQ(0xfff, res->timestamp);
}

TEST_F(kodFileTest, ReadFileWithBlankLines) {
	kod_init_kod_db(CreatePath("kod-test-blanks", INPUT_DIR).c_str());

	EXPECT_EQ(3, kod_db_cnt);

	kod_entry* res;

	ASSERT_EQ(1, search_entry("192.0.2.5", &res));
	EXPECT_STREQ("DENY", res->type);
	EXPECT_STREQ("192.0.2.5", res->hostname);
	EXPECT_EQ(0x1234567890, res->timestamp);

	ASSERT_EQ(1, search_entry("192.0.2.100", &res));
	EXPECT_STREQ("RSTR", res->type);
	EXPECT_STREQ("192.0.2.100", res->hostname);
	EXPECT_EQ(0xfff, res->timestamp);

	ASSERT_EQ(1, search_entry("example.com", &res));
	EXPECT_STREQ("DENY", res->type);
	EXPECT_STREQ("example.com", res->hostname);
	EXPECT_EQ(0xabcd, res->timestamp);
}

TEST_F(kodFileTest, WriteEmptyFile) {
	kod_db_file = estrdup(CreatePath("kod-output-blank", OUTPUT_DIR).c_str());

	write_kod_db();

	// Open file and ensure that the filesize is 0 bytes.
	std::ifstream is(kod_db_file, std::ios::binary);
	ASSERT_FALSE(is.fail());
	
	EXPECT_EQ(0, GetFileSize(is));

	is.close();
}

TEST_F(kodFileTest, WriteFileWithSingleEntry) {
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
	ASSERT_TRUE(actual.good());
	ASSERT_TRUE(expected.good());

	ASSERT_EQ(GetFileSize(expected), GetFileSize(actual));

	CompareFileContent(expected, actual);
}

TEST_F(kodFileTest, WriteFileWithMultipleEntries) {
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
	ASSERT_TRUE(actual.good());
	ASSERT_TRUE(expected.good());
	
	ASSERT_EQ(GetFileSize(expected), GetFileSize(actual));

	CompareFileContent(expected, actual);
}
