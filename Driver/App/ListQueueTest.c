#include "unity_fixture.h"
#include "stm32f4xx.h"
#include "Queue_List.h"

#define ITEM_LIMIT 5

TEST_GROUP(ListQueueTest);

ps_Queue pQueue = NULL;

TEST_SETUP(ListQueueTest)
{
    pQueue = Create_Queue(sizeof(uint16_t), ITEM_LIMIT);
}

TEST_TEAR_DOWN(ListQueueTest)
{
    Delete_Queue(pQueue);
}

TEST(ListQueueTest, ReadAfterWrite)
{
    uint16_t Data = 0;
    uint16_t i = 0;
    
    for(i = 0; i < ITEM_LIMIT; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(Queue_Write(&Data, pQueue), SUCCESS);
        Data++;
    }
    
    for(i = 0; i < ITEM_LIMIT; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(Queue_Read(&Data, pQueue), SUCCESS);
        TEST_ASSERT_EQUAL_UINT16(Data, i);
    }
}

TEST(ListQueueTest, ReadMoreThanWrite)
{
    uint16_t Data = 0;
    uint16_t i = 0;
    
    for(i = 0; i < ITEM_LIMIT; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(Queue_Write(&Data, pQueue), SUCCESS);
        Data++;
    }
    
    for(i = 0; i < 5; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(Queue_Read(&Data, pQueue), SUCCESS);
        TEST_ASSERT_EQUAL_UINT16(Data, i);
    }
    
    TEST_ASSERT_EQUAL_UINT8(Queue_Read(&Data, pQueue), ERROR);
}

TEST(ListQueueTest, WriteMoreThanLimit)
{
    uint16_t Data = 0;
    uint16_t i = 0;
    
    for(i = 0; i < ITEM_LIMIT; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(Queue_Write(&Data, pQueue), SUCCESS);
        Data++;
    }
    
    TEST_ASSERT_EQUAL_UINT8(Queue_Write(&Data, pQueue), ERROR);
    
    for(i = 0; i < 5; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(Queue_Read(&Data, pQueue), SUCCESS);
        TEST_ASSERT_EQUAL_UINT16(Data, i);
    }
    
    TEST_ASSERT_EQUAL_UINT8(Queue_Read(&Data, pQueue), ERROR);
}
