
#include <stdio.h>

#include "fileHandlingTest.h"

const char* CreatePath(const char* filename, DirectoryType argument) {
	const char* argpath = tests_main_args(argument);
	char* path;
	size_t len = 0;

	if (argpath != NULL) {
		len = strlen(argpath);
		if (argpath[strlen(argpath) - 1] != DIR_SEP)
			++len;
	}

	len += strlen(filename) + 1;

    	path = malloc(len);
	if (path != NULL) {
		if (argpath != NULL) {
			strncpy(path, argpath, len - 1);
			if (argpath[strlen(argpath) - 1] != DIR_SEP) {
				char ds[2] = { DIR_SEP, '\0'};
				strncat(path, ds, len - 1);
			}
		}
		strncat(path, filename, len - 1);
	}

	return path;
}

ssize_t GetFileSize(FILE* file) {
	ssize_t size = -1;
	off_t initial = ftello(file);
	if (initial != (off_t) -1) {
		if (fseek(file, 0, SEEK_END) == 0) {
			size = (ssize_t) ftello(file);
			if (fseek(file, initial, SEEK_CUR) != 0) {
				size = -1;
			}
		}
	}
	return size;
}


void CompareFileContent(FILE* expected, FILE* actual) {
	while (!feof(expected) && !feof(actual)) {
		char e_line[256];
		char a_line[256];
		TEST_ASSERT_FALSE(ferror(expected));
		TEST_ASSERT_FALSE(ferror(actual));
		if (fgets(e_line, sizeof(e_line), expected) != NULL &&
		    fgets(e_line, sizeof(e_line), expected) != NULL) {
			TEST_ASSERT_EQUAL_STRING(e_line, a_line);
		}
	}
}

void ClearFile(const char* filename) {
	FILE* f = fopen(filename, "wb");
	TEST_ASSERT_NULL(f);
	TEST_ASSERT_TRUE(fclose(f));
}
