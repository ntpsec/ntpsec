#include "fileHandlingTest.h"

extern "C" {
#include "log.h"
};

class logFileTest : public fileHandlingTest {
protected:
	void ClearFile(const std::string& filename) {
		std::ofstream clear(filename.c_str(), ios::trunc);
		ASSERT_TRUE(clear.good());
		clear.close();
	}
};

TEST_F(logFileTest, WriteLogFile) {
	std::string filename = CreatePath("log-output-log", OUTPUT_DIR);

	ClearFile(filename);

	init_log(filename.c_str());
	
	log_msg("Test log message", 0);
	log_msg("Test message 2", 0);

	/*
	 * Compare the file size, since the contents differs in the
	 * timestamp.
	 */
	ifstream expected(CreatePath("log-expected-log", INPUT_DIR).c_str());
	ifstream actual(CreatePath("log-output-log", OUTPUT_DIR).c_str());
	ASSERT_TRUE(expected.good());
	ASSERT_TRUE(actual.good());

	EXPECT_EQ(GetFileSize(expected), GetFileSize(actual));
}

