#include "config.h"
#include "ntp_stdlib.h"
#include "ntp_fp.h"
#include "ieee754io.h"
#include "timespecops.h"

#include "unity.h"
#include "unity_fixture.h"

/*
 * Tests for libparse/ieee754io.c, in -lparse
 *
 * It would be nice to use C floats and doubles here, but
 * the byte order is not the same as trimble or meinberg,
 * and C floats and doubles need not be IEEE 754
 */

TEST_GROUP(ieee754io);

TEST_SETUP(ieee754io) {}

TEST_TEAR_DOWN(ieee754io) {}

static offsets_t native_off = { 0, 1, 2, 3, 4, 5, 6, 7 };
/* byte order for meinberg doubles */
static offsets_t mbg_double = { 1, 0, 3, 2, 5, 4, 7, 6 };


/*
        unsigned char *p = (void *)&fp;
        printf("fp: %d %d %d %d %d %d %d %d\n",
             p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
*/

TEST(ieee754io, test_zero32) {
        int ret;
        unsigned char one[4] = { 0, 0, 0, 0};
        unsigned char *bp = &one[0];
        l_fp fp;

        ret = fetch_ieee754( &bp, IEEE_SINGLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_INT64( 0, (int64_t)fp );
}

TEST(ieee754io, test_one32) {
        int ret;
        unsigned char one[4] = { 0x3f, 0x80, 0, 0};
        unsigned char *bp = &one[0];
        l_fp fp;

        ret = fetch_ieee754( &bp, IEEE_SINGLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_INT64( 1LL << 32, (int64_t)fp );
}

TEST(ieee754io, test_negone32) {
        int ret;
        unsigned char negone[4] = { 0xbf, 0x80, 0, 0};
        unsigned char *bp = &negone[0];
        l_fp fp;

        ret = fetch_ieee754( &bp, IEEE_SINGLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_INT64( -(1LL << 32), (int64_t)fp );
}

TEST(ieee754io, test_nan32) {
        int ret;
        unsigned char buf[4] = { 0x7f, 0x80, 0, 0};
        unsigned char *bp = &buf[0];
        l_fp fp;

        /* +INF */
        ret = fetch_ieee754( &bp, IEEE_SINGLE, &fp, native_off);

        TEST_ASSERT( IEEE_POSINFINITY == ret);
        /* not IEEE754, but check for 0 anyway */
        TEST_ASSERT_EQUAL_INT64( 0, (int64_t)fp );

        /* -INF */
        buf[0] = 0xff;
        bp = &buf[0];
        ret = fetch_ieee754( &bp, IEEE_SINGLE, &fp, native_off);

        TEST_ASSERT( IEEE_NEGINFINITY == ret);
        /* not IEEE754, but check for 0 anyway */
        TEST_ASSERT_EQUAL_INT64( 0, (int64_t)fp );

        /* NAN */
        buf[0] = 0x7f;
        buf[1] = 0xf8;
        bp = &buf[0];
        ret = fetch_ieee754( &bp, IEEE_SINGLE, &fp, native_off);

        TEST_ASSERT( IEEE_NAN == ret);
        /* not IEEE754, but check for 0 anyway */
        TEST_ASSERT_EQUAL_INT64( 0, (int64_t)fp );
}

TEST(ieee754io, test_max32) {
        int ret;
        /* not enough precision in a float to get precision of l_fp */
        unsigned char buf[4] = { 0x4e, 0xff, 255, 255};
        unsigned char buf_n[4] = { 0xce, 0xff, 255, 255};
        unsigned char *bp = &buf[0];
        l_fp fp;

        /* max that fits */
        ret = fetch_ieee754( &bp, IEEE_SINGLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_INT64( 0x7FFFFF8000000000ULL, fp );

        /* negative max that fits */
        bp = &buf_n[0];
        ret = fetch_ieee754( &bp, IEEE_SINGLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_UINT64( 0x8000008000000000ULL, fp );
}

TEST(ieee754io, test_order32) {
        int ret;
        /* not enough precision in a double to get max l_fp */
        unsigned char buf[4] = { 0x3e, 127, 1, 2};
        /* not enough precision in a double to get negative max l_fp */
        unsigned char buf_n[4] = { 127, 0x3e, 2, 1};
        unsigned char *bp = &buf[0];
        l_fp fp;

        /* normal linear order */
        ret = fetch_ieee754( &bp, IEEE_SINGLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_UINT64((uint64_t)0x3FC04080ULL,
                                 (uint64_t)fp);

        /* meinberg order */
        bp = &buf_n[0];
        ret = fetch_ieee754( &bp, IEEE_SINGLE, &fp, mbg_double);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_UINT64((uint64_t)0x3FC04080ULL,
                                 (uint64_t)fp);
}

TEST(ieee754io, test_small32) {
        int ret;
        /* small number, one LSB of l_fp */
        unsigned char buf[4] = { 0x2F, 255, 255, 255};
        unsigned char *bp = &buf[0];
        l_fp fp;

        ret = fetch_ieee754( &bp, IEEE_SINGLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_INT64( 1, fp );
}


TEST(ieee754io, test_zero64) {
        int ret;
        unsigned char one[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        unsigned char *bp = &one[0];
        l_fp fp;

        ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_INT64( 0, (int64_t)fp );
}

TEST(ieee754io, test_one64) {
        int ret;
        unsigned char one[8] = { 0x3f, 0xf0, 0, 0, 0, 0, 0, 0 };
        unsigned char *bp = &one[0];
        l_fp fp;

        ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_INT64( 1LL << 32, (int64_t)fp );
}

TEST(ieee754io, test_negone64) {
        int ret;
        unsigned char negone[8] = { 0xbf, 0xf0, 0, 0, 0, 0, 0, 0 };
        unsigned char *bp = &negone[0];
        l_fp fp;

        ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_INT64( -(1LL << 32), (int64_t)fp );
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
        TEST_ASSERT_EQUAL_INT64( 0, (int64_t)fp );

        /* -INF */
        buf[0] = 0xff;
        bp = &buf[0];
        ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        TEST_ASSERT( IEEE_NEGINFINITY == ret);
        /* not IEEE754, but check for 0 anyway */
        TEST_ASSERT_EQUAL_INT64( 0, (int64_t)fp );

        /* +OVERFLOW */
        buf[0] = 0x41;
        buf[1] = 0xff;
        bp = &buf[0];
        ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        TEST_ASSERT( IEEE_POSOVERFLOW == ret);
        /* not IEEE754, but check for 0 anyway */
        TEST_ASSERT_EQUAL_INT64( 0, (int64_t)fp );

        /* -OVERFLOW */
        buf[0] = 0xC1;
        buf[1] = 0xff;
        bp = &buf[0];
        ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        TEST_ASSERT( IEEE_NEGOVERFLOW == ret);
        /* not IEEE754, but check for 0 anyway */
        TEST_ASSERT_EQUAL_INT64( 0, (int64_t)fp );

        /* NAN */
        buf[0] = 0x7f;
        buf[1] = 0xf8;
        bp = &buf[0];
        ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        TEST_ASSERT( IEEE_NAN == ret);
        /* not IEEE754, but check for 0 anyway */
        TEST_ASSERT_EQUAL_INT64( 0, (int64_t)fp );
}

TEST(ieee754io, test_max64) {
        int ret;
        /* not enough precision in a double to get max l_fp */
        unsigned char buf[8] = { 65, 239, 255, 255, 255, 255, 255, 255 };
        /* not enough precision in a double to get negative max l_fp */
        unsigned char buf_n[8] = { 65, 239, 255, 255, 255, 255, 255, 254 };
        unsigned char *bp = &buf[0];
        l_fp fp;

        /* max that fits */
        ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_UINT64( (uint64_t)0xFFFFFFFFFFFFF800ULL, fp );

        /* negative max that fits */
        bp = &buf_n[0];
        ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_UINT64( (uint64_t)0xFFFFFFFFFFFFF000ULL, fp );
}

TEST(ieee754io, test_order64) {
        int ret;
        /* not enough precision in a double to get max l_fp */
        unsigned char buf[8] = { 65, 239, 1, 2, 3, 4, 5, 6 };
        /* not enough precision in a double to get negative max l_fp */
        unsigned char buf_n[8] = { 239, 65, 2, 1, 4, 3, 6, 5 };
        unsigned char *bp = &buf[0];
        l_fp fp;

        /* normal linear order */
        ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_UINT64( (uint64_t)0xF808101820283000ULL, fp );

        /* meinberg order */
        bp = &buf_n[0];
        ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, mbg_double);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_UINT64( (uint64_t)0xF808101820283000ULL, fp );
}

TEST(ieee754io, test_small64) {
        int ret;
        /* small number, one LSB of l_fp  */
        unsigned char buf[8] = { 0x3d, 255, 255, 255, 255, 255, 255, 255};
        unsigned char *bp = &buf[0];
        l_fp fp;

        ret = fetch_ieee754( &bp, IEEE_DOUBLE, &fp, native_off);

        TEST_ASSERT( IEEE_OK == ret);
        TEST_ASSERT_EQUAL_INT64( 1, fp );
}

TEST_GROUP_RUNNER(ieee754io) {
        RUN_TEST_CASE(ieee754io, test_zero32);
        RUN_TEST_CASE(ieee754io, test_one32);
        RUN_TEST_CASE(ieee754io, test_negone32);
        RUN_TEST_CASE(ieee754io, test_small32);
        RUN_TEST_CASE(ieee754io, test_nan32);
        RUN_TEST_CASE(ieee754io, test_max32);
        RUN_TEST_CASE(ieee754io, test_order32);

        RUN_TEST_CASE(ieee754io, test_zero64);
        RUN_TEST_CASE(ieee754io, test_one64);
        RUN_TEST_CASE(ieee754io, test_negone64);
        RUN_TEST_CASE(ieee754io, test_small64);
        RUN_TEST_CASE(ieee754io, test_nan64);
        RUN_TEST_CASE(ieee754io, test_max64);
        RUN_TEST_CASE(ieee754io, test_order64);
}
