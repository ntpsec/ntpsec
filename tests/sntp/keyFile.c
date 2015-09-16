#include "unity.h"
#include "unity_fixture.h"

#include "fileHandlingTest.h"

#include "crypto.h"

TEST_GROUP(keyFile);

TEST_SETUP(keyFile) {}

TEST_TEAR_DOWN(keyFile) {}

static bool CompareKeys(struct key* expected, struct key* actual) {
	if (expected->key_id != actual->key_id) {
		printf("Expected key_id: %d but was: %d",
		       expected->key_id, actual->key_id);
		return false;
	}
	if (expected->key_len != actual->key_len) {
		printf("Expected key_len: %d but was: %d",
		       expected->key_len, actual->key_len);
		return false;
	}
	if (strcmp(expected->type, actual->type) != 0) {
		printf("Expected key_type: %s but was: %s",
		       expected->type,  actual->type);
		return false;
	}
	if (memcmp(expected->key_seq, actual->key_seq, expected->key_len) != 0) {
		printf("Key seq mismatch!\n");
		return false;
	}
	return true;
}

static bool CompareKey(int key_id,
		       int key_len,
		       const char* type,
		       const char* key_seq,
		       struct key* actual) {
	struct key temp;

	temp.key_id = key_id;
	temp.key_len = key_len;
	strlcpy(temp.type, type, sizeof(temp.type));
	memcpy(temp.key_seq, key_seq, key_len);

	return CompareKeys(&temp, actual);
}

TEST(keyFile, ReadEmptyKeyFile) {
	const char* path;
	struct key* keys = NULL;
	path = CreatePath("key-test-empty", INPUT_DIR);
	TEST_ASSERT_EQUAL(0, auth_init(path, &keys));
	free((void*) path);
	TEST_ASSERT_TRUE(keys == NULL);
}

TEST(keyFile, ReadASCIIKeys) {
	const char* path;
	struct key* keys = NULL;
	struct key* result = NULL;
	path = CreatePath("key-test-ascii", INPUT_DIR);
	TEST_ASSERT_EQUAL(2, auth_init(path, &keys));
	free((void*) path);
	TEST_ASSERT_TRUE(keys != NULL);
	get_key(40, &result);
	TEST_ASSERT_TRUE(result != NULL);
	TEST_ASSERT_TRUE(CompareKey(40, 11, "MD5", "asciikeyTwo", result));
	result = NULL;
	get_key(50, &result);
	TEST_ASSERT_TRUE(result != NULL);
	TEST_ASSERT_TRUE(CompareKey(50, 11, "MD5", "asciikeyOne", result));
}

TEST(keyFile, ReadHexKeys) {
	const char* path;
	struct key* keys = NULL;
	struct key* result = NULL;
	char data[15];

	path = CreatePath("key-test-hex", INPUT_DIR);
	TEST_ASSERT_EQUAL(3, auth_init(path, &keys));
	free((void*) path);

	TEST_ASSERT_TRUE(keys != NULL);

	get_key(10, &result);
	TEST_ASSERT_TRUE(result != NULL);
	TEST_ASSERT_TRUE(CompareKey(10, 13, "MD5",
		 "\x01\x23\x45\x67\x89\xab\xcd\xef\x01\x23\x45\x67\x89", result));

	result = NULL;
	get_key(20, &result);
	TEST_ASSERT_TRUE(result != NULL);
	memset(data, 0x11, 15);
	TEST_ASSERT_TRUE(CompareKey(20, 15, "MD5", data, result));

	result = NULL;
	get_key(30, &result);
	TEST_ASSERT_TRUE(result != NULL);
	memset(data, 0x01, 13);
	TEST_ASSERT_TRUE(CompareKey(30, 13, "MD5", data, result));
}

TEST(keyFile, ReadKeyFileWithComments) {
	const char* path;
	struct key* keys = NULL;
	struct key* result = NULL;
	char data[15];

	path = CreatePath("key-test-comments", INPUT_DIR);
	TEST_ASSERT_EQUAL(2, auth_init(path, &keys));
	free((void*) path);

	TEST_ASSERT_TRUE(keys != NULL);

	get_key(10, &result);
	TEST_ASSERT_TRUE(result != NULL);
	memset(data, 0x01, 15);
	TEST_ASSERT_TRUE(CompareKey(10, 15, "MD5", data, result));

	result = NULL;
	get_key(34, &result);
	TEST_ASSERT_TRUE(result != NULL);
	TEST_ASSERT_TRUE(CompareKey(34, 3, "MD5", "xyz", result));
}

TEST(keyFile, ReadKeyFileWithInvalidHex) {
	const char* path;
	struct key* keys = NULL;
	struct key* result = NULL;
	char data[15];

	path = CreatePath("key-test-invalid-hex", INPUT_DIR);
	TEST_ASSERT_EQUAL(1, auth_init(path, &keys));
	free((void*) path);

	TEST_ASSERT_TRUE(keys != NULL);

	get_key(10, &result);
	TEST_ASSERT_TRUE(result != NULL);
	memset(data, 0x01, 15);
	TEST_ASSERT_TRUE(CompareKey(10, 15, "MD5", data, result));

	result = NULL;
	get_key(30, &result); /* Should not exist, and result should remain NULL. */
	TEST_ASSERT_TRUE(result == NULL);
}

TEST_GROUP_RUNNER(keyFile) {
	RUN_TEST_CASE(keyFile, ReadEmptyKeyFile);
	RUN_TEST_CASE(keyFile, ReadASCIIKeys);
	RUN_TEST_CASE(keyFile, ReadHexKeys);
	RUN_TEST_CASE(keyFile, ReadKeyFileWithComments);
	RUN_TEST_CASE(keyFile, ReadKeyFileWithInvalidHex);
}
