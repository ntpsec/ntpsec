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
/* byte order for meinberg doubles */
// static offsets_t mbg_double = { 1, 0, 3, 2, 5, 4, 7, 6 };


/*
        unsigned char *p = (void *)&fp;
        printf("fp: %d %d %d %d %d %d %d %d\n",
	     p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
*/

TEST(ieee754io, test_zero64) {
        int ret;
        unsigned char one[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char *bp = &one[0];
        l_fp fp;

	ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

	TEST_ASSERT( IEEE_OK == ret);
	TEST_ASSERT_EQUAL_INT64( 0, (long)fp );
}

TEST(ieee754io, test_one64) {
        int ret;
        unsigned char one[8] = { 0x3f, 0xf0, 0, 0, 0, 0, 0, 0 };
	unsigned char *bp = &one[0];
        l_fp fp;

	ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

	TEST_ASSERT( IEEE_OK == ret);
	TEST_ASSERT_EQUAL_INT64( 1L << 32, (long)fp );
}

TEST(ieee754io, test_negone64) {
        int ret;
        unsigned char negone[8] = { 0xbf, 0xf0, 0, 0, 0, 0, 0, 0 };
	unsigned char *bp = &negone[0];
        l_fp fp;

	ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

	TEST_ASSERT( IEEE_OK == ret);
	TEST_ASSERT_EQUAL_INT64( -(1L << 32), (long)fp );
}

TEST(ieee754io, test_nan64) {
        int ret;
        unsigned char buf[8] = { 0x7f, 0xF0, 0, 0, 0, 0, 0, 0 };
	unsigned char *bp = &buf[0];
        l_fp fp;

        /* +INF */
	ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

	TEST_ASSERT( IEEE_POSINFINITY == ret);
        /* not IEEE754, but check for 0 anyway */
	TEST_ASSERT_EQUAL_INT64( 0, (long)fp );

        /* -INF */
        buf[0] = 0xff;
	bp = &buf[0];
	ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        printf("\n%d\n", ret);
	TEST_ASSERT( IEEE_NEGINFINITY == ret);
        /* not IEEE754, but check for 0 anyway */
	TEST_ASSERT_EQUAL_INT64( 0, (long)fp );

        /* NAN */
        buf[0] = 0x7f;
        buf[1] = 0xf8;
	bp = &buf[0];
	ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        printf("\n%d\n", ret);
	TEST_ASSERT( IEEE_NAN == ret);
        /* not IEEE754, but check for 0 anyway */
	TEST_ASSERT_EQUAL_INT64( 0, (long)fp );
}

TEST_GROUP_RUNNER(ieee754io) {
	RUN_TEST_CASE(ieee754io, test_zero64);
	RUN_TEST_CASE(ieee754io, test_one64);
	RUN_TEST_CASE(ieee754io, test_negone64);
	RUN_TEST_CASE(ieee754io, test_nan64);
}
