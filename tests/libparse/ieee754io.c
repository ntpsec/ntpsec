#include "config.h"
#include "ntp_stdlib.h"
#include "ntp_fp.h"
#include "ieee754io.h"
#include "timespecops.h"

#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(ieee754io);

TEST_SETUP(ieee754io) {}

TEST_TEAR_DOWN(ieee754io) {}

static offsets_t native_off = { 0, 1, 2, 3, 4, 5, 6, 7 };

TEST(ieee754io, test_one) {
        int ret;
        unsigned char one[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char *bp = &one[0];
        l_fp fp;

        printf("hello\n");
	ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp,
            native_off);

        printf("hello\n");
        printf("%ld\n", (long)fp);

	TEST_ASSERT( IEEE_OK == ret);
	TEST_ASSERT_EQUAL_INT64( 0, (long)fp );
}

TEST_GROUP_RUNNER(ieee754io) {
	RUN_TEST_CASE(ieee754io, test_one);
}
