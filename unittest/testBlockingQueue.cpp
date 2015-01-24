#include "BlockingQueue.h"
#include "Thread.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <vector>

using namespace std;
using namespace tpool;
using namespace tpool::sync;
using namespace boost;


TEST(BlockingQueueTestSuite, test_ctor)
{
    {
        BlockingQueue<int> b_queue;
        ASSERT_EQ(static_cast<size_t>(0), b_queue.Size());
        ASSERT_TRUE(b_queue.IsEmpty());
    }

    {
        queue<int> container;
        container.push(1);
        container.push(2);

        BlockingQueue<int> b_queue(container);
        ASSERT_EQ(static_cast<size_t>(container.size()), b_queue.Size());
        size_t queue_size = b_queue.Size();
        for (size_t i = 0; i < queue_size; ++i)
        {
            ASSERT_EQ(container.front(), b_queue.Front());
            container.pop();
            b_queue.Pop();
        }
    }

    {
        queue<int>::container_type container;
        container.push_back(1);
        container.push_back(2);

        BlockingQueue<int> b_queue(container);
        ASSERT_EQ(static_cast<size_t>(container.size()), b_queue.Size());
        size_t queue_size = b_queue.Size();
        for (size_t i = 0; i < queue_size; ++i)
        {
            ASSERT_EQ(container.front(), b_queue.Front());
            container.pop_front();
            b_queue.Pop();
        }
    }
}

TEST(BlockingQueueTestSuite, test_Push)
{
    int numbers[] = {5, 4, 3, 1, 6};
    size_t num_size = sizeof(numbers) / sizeof(int);

    BlockingQueue<int> b_queue;
    for (size_t i = 0; i < num_size; ++i)
    {
        b_queue.Push(numbers[i]);
    }

    ASSERT_EQ(num_size, b_queue.Size());

    for (size_t i = 0; i < num_size; ++i)
    {
        ASSERT_EQ(numbers[i], b_queue.Pop());
    }
}

TEST(BlockingQueueTestSuite, test_Pop)
{
    int numbers[] = {5, 4, 3, 1, 6};
    size_t num_size = sizeof(numbers) / sizeof(int);

    BlockingQueue<int> b_queue;
    for (size_t i = 0; i < num_size; ++i)
    {
        b_queue.Push(numbers[i]);
    }

    ASSERT_EQ(num_size, b_queue.Size());

    for (size_t i = 0; i < num_size; ++i)
    {
        int num = 0;
        b_queue.Pop(num);
        ASSERT_EQ(numbers[i], num);
    }
}

TEST(BlockingQueueTestSuite, test_NonblockingPop)
{
    int numbers[] = {5, 4, 3, 1, 6};
    size_t num_size = sizeof(numbers) / sizeof(int);

    BlockingQueue<int> b_queue;
    int tmp_num = 0;
    ASSERT_FALSE(b_queue.NonblockingPop(tmp_num));

    for (size_t i = 0; i < num_size; ++i)
    {
        b_queue.Push(numbers[i]);
    }

    ASSERT_EQ(num_size, b_queue.Size());

    for (size_t i = 0; i < num_size; ++i)
    {
        int num = 0;
        ASSERT_TRUE(b_queue.NonblockingPop(num));
        ASSERT_EQ(numbers[i], num);
    }

    ASSERT_FALSE(b_queue.NonblockingPop(tmp_num));
}


TEST(BlockingQueueTestSuite, test_Front)
{
    int numbers[] = {5, 4, 3, 1, 6};
    size_t num_size = sizeof(numbers) / sizeof(int);

    BlockingQueue<int> b_queue;
    for (size_t i = 0; i < num_size; ++i)
    {
        b_queue.Push(numbers[i]);
        ASSERT_EQ(numbers[0], b_queue.Front());

        int num = 0;
        b_queue.Front(num);
        ASSERT_EQ(numbers[0], num);
    }
}

TEST(BlockingQueueTestSuite, test_NonblockingFront)
{
    int numbers[] = {5, 4, 3, 1, 6};
    size_t num_size = sizeof(numbers) / sizeof(int);

    BlockingQueue<int> b_queue;
    int tmp_num = 0;
    ASSERT_FALSE(b_queue.NonblockingFront(tmp_num));

    for (size_t i = 0; i < num_size; ++i)
    {
        b_queue.Push(numbers[i]);
        ASSERT_TRUE(b_queue.NonblockingFront(tmp_num));
        ASSERT_EQ(numbers[0], tmp_num);
    }
}

namespace {

template<typename T>
struct PushThreadFunc {
    const vector<T>& elems;
    BlockingQueue<T>& blocking_queue;

    PushThreadFunc(const vector<T>& e, BlockingQueue<T>& q)
    : elems(e), blocking_queue(q)
    {}

    virtual ~PushThreadFunc() {}

    virtual void DoRun()
    {
        for (size_t i = 0; i < elems.size(); ++i)
        {
            blocking_queue.Push(elems[i]);
        }
    }

    void operator()()
    {
        DoRun();
    }
};

}

TEST(BlockingQueueTestSuite, test_concurrent_Push)
{
    int num_array[] = {1, 4, 5, 3, 7, 8};
    size_t array_size = sizeof(num_array) / sizeof(int);
    vector<int> numbers(num_array, num_array + array_size);
    ASSERT_EQ(array_size, static_cast<size_t>(numbers.size()));

    BlockingQueue<int> b_queue;
    {
        Thread t1((PushThreadFunc<int>(numbers, b_queue)));
        Thread t2((PushThreadFunc<int>(numbers, b_queue)));
        Thread t3((PushThreadFunc<int>(numbers, b_queue)));
    }

    ASSERT_EQ(array_size * 3, b_queue.Size());
    int array_sum = 0;
    for (size_t i = 0; i < array_size; ++i)
    {
        array_sum += numbers[i];
    }

    int queue_sum = 0;
    for (size_t i = 0; i < array_size * 3; ++i)
    {
        queue_sum += b_queue.Pop();
    }
    ASSERT_EQ(array_sum * 3, queue_sum);
}

namespace {

template<typename T>
struct PopThreadFunc {
    size_t pop_count;
    BlockingQueue<T>& blocking_queue;

    PopThreadFunc(size_t cnt, BlockingQueue<T>& q)
    : pop_count(cnt), blocking_queue(q)
    {}

    void operator()()
    {
        for (size_t i = 0; i < pop_count; ++i)
        {
            (void) blocking_queue.Front();
            (void) blocking_queue.Pop();
        }
    }

};

}

TEST(BlockingQueueTestSuite, test_concurrent_Push_Pop)
{
    int num_array[] = {1, 4, 5, 3, 7, 8};
    size_t array_size = sizeof(num_array) / sizeof(int);
    vector<int> numbers(num_array, num_array + array_size);
    ASSERT_EQ(array_size, static_cast<size_t>(numbers.size()));

    BlockingQueue<int> b_queue;
    {
        Thread t1((PushThreadFunc<int>(numbers, b_queue)));
        Thread t2((PushThreadFunc<int>(numbers, b_queue)));
        Thread t3((PopThreadFunc<int>(array_size * 2, b_queue)));
    }
}

