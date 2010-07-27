#include "sntptest.h"

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

class kodFileTest : public sntptest {
protected:
	std::string CreatePath(const char* filename) {
		std::string path = m_param;

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
};

TEST_F(kodFileTest, ReadEmptyFile) {
	kod_init_kod_db(CreatePath("kod-test-empty").c_str());

	EXPECT_EQ(0, kod_db_cnt);
}

TEST_F(kodFileTest, ReadCorrectFile) {
	kod_init_kod_db(CreatePath("kod-test-correct").c_str());
	
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
	kod_init_kod_db(CreatePath("kod-test-blanks").c_str());

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
