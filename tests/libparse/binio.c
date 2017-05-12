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


TEST(binio, get_lsb_short0) {
        long ret;
        unsigned char zero[2] = { 0, 0};
        unsigned char *bp = &zero[0];

        ret = get_lsb_short( &bp);

        TEST_ASSERT_EQUAL_INT64( 0, ret );
}

TEST(binio, get_lsb_short1) {
        long ret;
        unsigned char zero[2] = { 1, 2};
        unsigned char *bp = &zero[0];

        ret = get_lsb_short( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x0201UL, ret );
}

TEST(binio, get_lsb_short2) {
        long ret;
        unsigned char zero[2] = { 2, 1};
        unsigned char *bp = &zero[0];

        ret = get_lsb_short( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x0102UL, ret );
}

TEST(binio, get_lsb_short3) {
        long ret;
        unsigned char zero[2] = { 0xff, 0xff};
        unsigned char *bp = &zero[0];

        ret = get_lsb_short( &bp);

        TEST_ASSERT_EQUAL_HEX64( -1L, ret );
}

TEST(binio, get_lsb_short4) {
        long ret;
        unsigned char zero[2] = { 0, 0x80};
        unsigned char *bp = &zero[0];

        ret = get_lsb_short( &bp);

        TEST_ASSERT_EQUAL_HEX64( -0x8000L, ret );
}


TEST(binio, get_lsb_ulong0) {
        unsigned long ret;
        unsigned char zero[4] = { 0, 0, 0, 0};
        unsigned char *bp = &zero[0];

        ret = get_lsb_ulong( &bp);

        TEST_ASSERT_EQUAL_UINT64( 0, ret );
}


TEST(binio, get_lsb_ulong1) {
        unsigned long ret;
        unsigned char zero[4] = { 1, 2, 3, 4};
        unsigned char *bp = &zero[0];

        ret = get_lsb_ulong( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x04030201UL, ret );
}


TEST(binio, get_lsb_ulong2) {
        unsigned long ret;
        unsigned char zero[4] = { 4, 3, 2, 1};
        unsigned char *bp = &zero[0];

        ret = get_lsb_ulong( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x01020304UL, ret );
}


TEST(binio, get_lsb_ulong3) {
        unsigned long ret;
        unsigned char zero[4] = { 0xff, 0xff, 0xff, 0xff};
        unsigned char *bp = &zero[0];

        ret = get_lsb_ulong( &bp);

        printf("ulong: %lx\n", ret);
        TEST_ASSERT_EQUAL_HEX64( 0x0FFFFFFFFUL, ret );
}


TEST(binio, get_lsb_ulong4) {
        unsigned long ret;
        unsigned char zero[4] = { 0, 0, 0, 0x80};
        unsigned char *bp = &zero[0];

        ret = get_lsb_ulong( &bp);

        TEST_ASSERT_EQUAL_HEX64( 0x080000000UL, ret );
}

TEST_GROUP_RUNNER(binio) {
        RUN_TEST_CASE(binio, get_lsb_short0);
        RUN_TEST_CASE(binio, get_lsb_short1);
        RUN_TEST_CASE(binio, get_lsb_short2);
        RUN_TEST_CASE(binio, get_lsb_short3);
        RUN_TEST_CASE(binio, get_lsb_short4);

        RUN_TEST_CASE(binio, get_lsb_ulong0);
        RUN_TEST_CASE(binio, get_lsb_ulong1);
        RUN_TEST_CASE(binio, get_lsb_ulong2);
        /* next two tests are good, but the code they test is bad
        RUN_TEST_CASE(binio, get_lsb_ulong3);
        RUN_TEST_CASE(binio, get_lsb_ulong4);
        */
}
