#include "fileHandlingTest.h"

extern "C" {
#include "log.h"
#include <fcntl.h>
};

class logFileTest : public fileHandlingTest {
};

#if 0	/* temporarily disable, needs rework */
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

TEST_F(logFileTest, WriteDebugMessage) {
	ActivateOption("-l", "/dev/null");

	std::string filename = CreatePath("log-output-debug", OUTPUT_DIR);

	// Create and/or truncate the file.
	FILE* debugFile = fopen(filename.c_str(), "w");

	fflush(stderr);

	/* 
	 * Copy stderr's original file handle, so we can restore
	 * it later on. Then redirect stderr to our debugFile.
	 */
	int orig = dup(fileno(stderr));
	dup2(fileno(debugFile), fileno(stderr));
	fclose(debugFile);

	debug_msg("This is a debug message");
	debug_msg("Another debug message");

	// Restore the original stderr handle.
	fflush(stderr);
	dup2(orig, fileno(stderr));
	close(orig);

	/*
	 * Compare the file size, since the contents differs in the
	 * timestamp.
	 */
	ifstream expected(CreatePath("log-expected-debug", INPUT_DIR).c_str());
	ifstream actual(filename.c_str());
	ASSERT_TRUE(expected.good());
	ASSERT_TRUE(actual.good());

	EXPECT_EQ(GetFileSize(expected), GetFileSize(actual));
}
#endif
