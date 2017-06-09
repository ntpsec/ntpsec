#include "config.h"
#include "ntp_stdlib.h"
#include "ntp_fp.h"
#include "binio.h"

#include "unity.h"
#include "unity_fixture.h"

/*
 * Tests for libparse/binio.c, in -lparse
 *
 */

TEST_GROUP(binio);

TEST_SETUP(binio) {}

TEST_TEAR_DOWN(binio) {}

/* LSB int tests */

TEST(binio, get_lsb_int160) {
        long ret;
        unsigned char zero[2] = { 0, 0};
        unsigned char *bp = &zero[0];

        ret = get_lsb_int16( &bp);

        TEST_ASSERT_EQUAL_INT64( 0, ret );
}

TEST(binio, get_lsb_int161) {
        long ret;
        unsigned char zero[2] = { 1, 2};
        unsigned char *bp = &zero[0];

        ret = get_lsb_int16( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x0201UL, ret );
}

TEST(binio, get_lsb_int162) {
        long ret;
        unsigned char zero[2] = { 2, 1};
        unsigned char *bp = &zero[0];

        ret = get_lsb_int16( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x0102UL, ret );
}

TEST(binio, get_lsb_int163) {
        long ret;
        unsigned char zero[2] = { 0xff, 0xff};
        unsigned char *bp = &zero[0];

        ret = get_lsb_int16( &bp);

        TEST_ASSERT_EQUAL_HEX64( -1L, ret );
}

TEST(binio, get_lsb_int164) {
        long ret;
        unsigned char zero[2] = { 0, 0x80};
        unsigned char *bp = &zero[0];

        ret = get_lsb_int16( &bp);

        TEST_ASSERT_EQUAL_HEX64( -0x8000L, ret );
}


TEST(binio, get_lsb_int320) {
        int32_t ret;
        unsigned char zero[4] = { 0, 0, 0, 0};
        unsigned char *bp = &zero[0];

        ret = get_lsb_int32( &bp);

        TEST_ASSERT_EQUAL_INT32( 0, ret );
}


TEST(binio, get_lsb_int321) {
        int32_t ret;
        unsigned char zero[4] = { 1, 2, 3, 4};
        unsigned char *bp = &zero[0];

        ret = get_lsb_int32( &bp);

        TEST_ASSERT_EQUAL_HEX32( 0x04030201UL, ret );
}


TEST(binio, get_lsb_int322) {
        int32_t ret;
        unsigned char zero[4] = { 4, 3, 2, 1};
        unsigned char *bp = &zero[0];

        ret = get_lsb_int32( &bp);

        TEST_ASSERT_EQUAL_HEX32( 0x01020304UL, ret );
}


TEST(binio, get_lsb_int323) {
        int32_t ret;
        unsigned char zero[4] = { 0xff, 0xff, 0xff, 0xff};
        unsigned char *bp = &zero[0];

        ret = get_lsb_int32( &bp);

        TEST_ASSERT_EQUAL_HEX32( 0x0FFFFFFFFUL, ret );
}


TEST(binio, get_lsb_int324) {
        int32_t ret;
        unsigned char zero[4] = { 0, 0, 0, 0x80};
        unsigned char *bp = &zero[0];

        ret = get_lsb_int32( &bp);

        TEST_ASSERT_EQUAL_HEX32( 0x080000000UL, ret );
}


TEST(binio, put_lsb_uint160) {
        unsigned char ret[2] = { 0, 0};
        unsigned char expect[2] = { 0, 0};
        unsigned char *bp = &ret[0];

        put_lsb_uint16( &bp, 0);

        TEST_ASSERT_EQUAL_HEX8_ARRAY( expect, ret, 2 );
}

TEST(binio, put_lsb_uint161) {
        unsigned char ret[2] = { 0, 0};
        unsigned char expect[2] = { 1, 0};
        unsigned char *bp = &ret[0];

        put_lsb_uint16( &bp, 1);

        TEST_ASSERT_EQUAL_HEX8_ARRAY( expect, ret, 2 );
}

TEST(binio, put_lsb_uint162) {
        unsigned char ret[2] = { 0, 0};
        unsigned char expect[2] = { 0, 1};
        unsigned char *bp = &ret[0];

        put_lsb_uint16( &bp, 256);

        TEST_ASSERT_EQUAL_HEX8_ARRAY( expect, ret, 2 );
}

TEST(binio, put_lsb_uint163) {
        unsigned char ret[2] = { 0, 0};
        unsigned char expect[2] = { 0xff, 0xff };
        unsigned char *bp = &ret[0];

        put_lsb_uint16( &bp, 0xffff);

        TEST_ASSERT_EQUAL_HEX8_ARRAY( expect, ret, 2 );
}

TEST(binio, put_lsb_uint164) {
        unsigned char ret[2] = { 0, 0};
        unsigned char expect[2] = { 1, 0x80};
        unsigned char *bp = &ret[0];

        put_lsb_uint16( &bp, 0x8001);

        TEST_ASSERT_EQUAL_HEX8_ARRAY( expect, ret, 2 );
}


/* LSB unsigned int tests */

TEST(binio, get_lsb_uint160) {
        long ret;
        unsigned char zero[2] = { 0, 0};
        unsigned char *bp = &zero[0];

        ret = get_lsb_uint16( &bp);

        TEST_ASSERT_EQUAL_INT64( 0, ret );
}

TEST(binio, get_lsb_uint161) {
        long ret;
        unsigned char zero[2] = { 1, 2};
        unsigned char *bp = &zero[0];

        ret = get_lsb_uint16( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x0201UL, ret );
}

TEST(binio, get_lsb_uint162) {
        long ret;
        unsigned char zero[2] = { 2, 1};
        unsigned char *bp = &zero[0];

        ret = get_lsb_uint16( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x0102UL, ret );
}

TEST(binio, get_lsb_uint163) {
        long ret;
        unsigned char zero[2] = { 0xff, 0xff};
        unsigned char *bp = &zero[0];

        ret = get_lsb_uint16( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x0ffffUL, ret );
}

TEST(binio, get_lsb_uint164) {
        long ret;
        unsigned char zero[2] = { 0, 0x80};
        unsigned char *bp = &zero[0];

        ret = get_lsb_uint16( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x8000L, ret );
}


TEST(binio, get_lsb_uint320) {
        uint32_t ret;
        unsigned char zero[4] = { 0, 0, 0, 0};
        unsigned char *bp = &zero[0];

        ret = get_lsb_uint32( &bp);

        TEST_ASSERT_EQUAL_UINT32( 0, ret );
}


TEST(binio, get_lsb_uint321) {
        uint32_t ret;
        unsigned char zero[4] = { 1, 2, 3, 4};
        unsigned char *bp = &zero[0];

        ret = get_lsb_uint32( &bp);

        TEST_ASSERT_EQUAL_HEX32( 0x04030201UL, ret );
}


TEST(binio, get_lsb_uint322) {
        uint32_t ret;
        unsigned char zero[4] = { 4, 3, 2, 1};
        unsigned char *bp = &zero[0];

        ret = get_lsb_uint32( &bp);

        TEST_ASSERT_EQUAL_HEX32( 0x01020304UL, ret );
}


TEST(binio, get_lsb_uint323) {
        uint32_t ret;
        unsigned char zero[4] = { 0xff, 0xff, 0xff, 0xff};
        unsigned char *bp = &zero[0];

        ret = get_lsb_uint32( &bp);

        TEST_ASSERT_EQUAL_HEX32( 0x0FFFFFFFFUL, ret );
}


TEST(binio, get_lsb_uint324) {
        uint32_t ret;
        unsigned char zero[4] = { 0, 0, 0, 0x80};
        unsigned char *bp = &zero[0];

        ret = get_lsb_uint32( &bp);

        TEST_ASSERT_EQUAL_HEX32( 0x080000000UL, ret );
}

/* MSB tests */

TEST(binio, get_msb_short0) {
        long ret;
        unsigned char zero[2] = { 0, 0};
        unsigned char *bp = &zero[0];

        ret = get_msb_short( &bp);

        TEST_ASSERT_EQUAL_INT64( 0, ret );
}

TEST(binio, get_msb_short1) {
        long ret;
        unsigned char zero[2] = { 2, 1};
        unsigned char *bp = &zero[0];

        ret = get_msb_short( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x0201UL, ret );
}

TEST(binio, get_msb_short2) {
        long ret;
        unsigned char zero[2] = { 1, 2};
        unsigned char *bp = &zero[0];

        ret = get_msb_short( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x0102UL, ret );
}

TEST(binio, get_msb_short3) {
        long ret;
        unsigned char zero[2] = { 0xff, 0xff};
        unsigned char *bp = &zero[0];

        ret = get_msb_short( &bp);

        TEST_ASSERT_EQUAL_HEX64( -1L, ret );
}

TEST(binio, get_msb_short4) {
        long ret;
        unsigned char zero[2] = { 0x80, 0};
        unsigned char *bp = &zero[0];

        ret = get_msb_short( &bp);

        TEST_ASSERT_EQUAL_HEX64( -0x8000L, ret );
}

TEST(binio, getmsb_short0) {
        int  ret;
        unsigned char zero[2] = { 0, 0};

        ret = getmsb_short( zero);

        TEST_ASSERT_EQUAL_INT( 0, ret );
}

TEST(binio, getmsb_short1) {
        int ret;
        unsigned char zero[2] = { 2, 1};

        ret = getmsb_short( zero);

        TEST_ASSERT_EQUAL_INT( 0x0201L, ret );
}

TEST(binio, getmsb_short2) {
        int ret;
        unsigned char zero[2] = { 1, 2};

        ret = getmsb_short( zero);

        TEST_ASSERT_EQUAL_INT( 0x0102L, ret );
}

TEST(binio, getmsb_short3) {
        int ret;
        unsigned char zero[2] = { 0xff, 0xff};

        ret = getmsb_short( zero);

        TEST_ASSERT_EQUAL_INT( -1, ret );
}

TEST(binio, getmsb_short4) {
        int ret;
        unsigned char zero[2] = { 0x80, 0};

        ret = getmsb_short( zero);

        TEST_ASSERT_EQUAL_INT( -0x8000L, ret );
}

TEST(binio, get_msb_ushort0) {
        uint32_t  ret;
        unsigned char zero[2] = { 0, 0};

        ret = get_msb_ushort( zero);

        TEST_ASSERT_EQUAL_HEX32( 0, ret );
}

TEST(binio, get_msb_ushort1) {
        uint32_t ret;
        unsigned char zero[2] = { 2, 1};

        ret = get_msb_ushort( zero);

        TEST_ASSERT_EQUAL_HEX32( 0x0201UL, ret );
}

TEST(binio, get_msb_ushort2) {
        uint32_t ret;
        unsigned char zero[2] = { 1, 2};

        ret = get_msb_ushort( zero);

        TEST_ASSERT_EQUAL_HEX32( 0x0102UL, ret );
}

TEST(binio, get_msb_ushort3) {
        uint32_t ret;
        unsigned char zero[2] = { 0xff, 0xff};

        ret = get_msb_ushort( zero);

        TEST_ASSERT_EQUAL_HEX32( 0x0FFFFUL, ret );
}

TEST(binio, get_msb_ushort4) {
        uint32_t ret;
        unsigned char zero[2] = { 0x80, 0};

        ret = get_msb_ushort( zero);

        TEST_ASSERT_EQUAL_HEX32( 0x8000UL, ret );
}

TEST_GROUP_RUNNER(binio) {
        /* LSB int tests */
        RUN_TEST_CASE(binio, get_lsb_int160);
        RUN_TEST_CASE(binio, get_lsb_int161);
        RUN_TEST_CASE(binio, get_lsb_int162);
        RUN_TEST_CASE(binio, get_lsb_int163);
        RUN_TEST_CASE(binio, get_lsb_int164);

        RUN_TEST_CASE(binio, get_lsb_int320);
        RUN_TEST_CASE(binio, get_lsb_int321);
        RUN_TEST_CASE(binio, get_lsb_int322);
        RUN_TEST_CASE(binio, get_lsb_int323);
        RUN_TEST_CASE(binio, get_lsb_int324);

        RUN_TEST_CASE(binio, put_lsb_uint160);
        RUN_TEST_CASE(binio, put_lsb_uint161);
        RUN_TEST_CASE(binio, put_lsb_uint162);
        RUN_TEST_CASE(binio, put_lsb_uint163);
        RUN_TEST_CASE(binio, put_lsb_uint164);

        /* LSB unsigned int tests */
        RUN_TEST_CASE(binio, get_lsb_uint160);
        RUN_TEST_CASE(binio, get_lsb_uint161);
        RUN_TEST_CASE(binio, get_lsb_uint162);
        RUN_TEST_CASE(binio, get_lsb_uint163);
        RUN_TEST_CASE(binio, get_lsb_uint164);

        RUN_TEST_CASE(binio, get_lsb_uint320);
        RUN_TEST_CASE(binio, get_lsb_uint321);
        RUN_TEST_CASE(binio, get_lsb_uint322);
        RUN_TEST_CASE(binio, get_lsb_uint323);
        RUN_TEST_CASE(binio, get_lsb_uint324);

        /* MSB tests */
        RUN_TEST_CASE(binio, get_msb_short0);
        RUN_TEST_CASE(binio, get_msb_short1);
        RUN_TEST_CASE(binio, get_msb_short2);
        RUN_TEST_CASE(binio, get_msb_short3);
        RUN_TEST_CASE(binio, get_msb_short4);

        RUN_TEST_CASE(binio, getmsb_short0);
        RUN_TEST_CASE(binio, getmsb_short1);
        RUN_TEST_CASE(binio, getmsb_short2);
        RUN_TEST_CASE(binio, getmsb_short3);
        RUN_TEST_CASE(binio, getmsb_short4);

        RUN_TEST_CASE(binio, get_msb_ushort0);
        RUN_TEST_CASE(binio, get_msb_ushort1);
        RUN_TEST_CASE(binio, get_msb_ushort2);
        RUN_TEST_CASE(binio, get_msb_ushort3);
        RUN_TEST_CASE(binio, get_msb_ushort4);

}
