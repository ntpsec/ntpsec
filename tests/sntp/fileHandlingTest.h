#ifndef GUARD_FILE_HANDLING_TEST_H
#define GUARD_FILE_HANDLING_TEST_H

#include "sntptest.h"

typedef enum {
	INPUT_DIR = 0,
	OUTPUT_DIR = 1
}  DirectoryType;

const char* CreatePath(const char* filename, DirectoryType argument);
ssize_t GetFileSize(FILE* file);
void CompareFileContent(FILE* expected, FILE* actual);
void ClearFile(const char* filename);

#endif // GUARD_FILE_HANDLING_TEST_H
