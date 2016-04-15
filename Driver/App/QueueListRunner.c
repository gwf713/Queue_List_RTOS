#include "unity_fixture.h"

TEST_GROUP_RUNNER(ListQueueTest)
{
    RUN_TEST_CASE(ListQueueTest, ReadAfterWrite);
    RUN_TEST_CASE(ListQueueTest, ReadMoreThanWrite);
    RUN_TEST_CASE(ListQueueTest, WriteMoreThanLimit);
}

