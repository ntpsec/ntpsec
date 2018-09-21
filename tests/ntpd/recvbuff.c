#include "config.h"
#include "ntp_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"
#include "recvbuff.h"


TEST_GROUP(recvbuff);

TEST_SETUP(recvbuff) {
	init_recvbuff(RECV_INIT);
}

TEST_TEAR_DOWN(recvbuff) {}


TEST(recvbuff, Initialization) {
	TEST_ASSERT_EQUAL(RECV_INIT, free_recvbuffs());
}

TEST(recvbuff, GetAndFree) {
	unsigned long initial = free_recvbuffs();
	recvbuf_t* buf = get_free_recv_buffer();

	TEST_ASSERT_EQUAL(initial-1, free_recvbuffs());
	freerecvbuf(buf);
	TEST_ASSERT_EQUAL(initial, free_recvbuffs());
}

TEST_GROUP_RUNNER(recvbuff) {
	RUN_TEST_CASE(recvbuff, Initialization);
	RUN_TEST_CASE(recvbuff, GetAndFree);
}
