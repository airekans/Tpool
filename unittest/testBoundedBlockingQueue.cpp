#include "BoundedBlockingQueue.h"
#include "Thread.h"
#include "TestUtil.h"

#include <gtest/gtest.h>
#include <cstdlib>
#include <vector>

using namespace std;
using namespace tpool;


TEST(BoundedBlockingQueueTestSuite, test_ctor)
{
    {
        BoundedBlockingQueue<int> b_queue(4);
        ASSERT_EQ(static_cast<size_t>(0), b_queue.Size());
        ASSERT_EQ(4, b_queue.MaxSize());
        ASSERT_TRUE(b_queue.IsEmpty());
        ASSERT_FALSE(b_queue.IsFull());
    }

    {
        queue<int> container;
        container.push(1);
        container.push(2);

        BoundedBlockingQueue<int> b_queue(4, container);
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
        container.push_back(3);
        container.push_back(4);

        BoundedBlockingQueue<int> b_queue(4, container);
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

TEST(BoundedBlockingQueueTestSuite, test_Push)
{
    int numbers[] = {5, 4, 3, 1, 6};
    size_t num_size = sizeof(numbers) / sizeof(int);

    BoundedBlockingQueue<int> b_queue(num_size);
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

namespace {

template<typename T>
struct PushThreadFunc {
    const vector<T>& elems;
    BoundedBlockingQueue<T>& blocking_queue;

    PushThreadFunc(const vector<T>& e, BoundedBlockingQueue<T>& q)
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

TEST(BoundedBlockingQueueTestSuite, test_blocking_Push)
{
    int numbers[] = {5, 4, 3, 1, 6};
    size_t num_size = sizeof(numbers) / sizeof(int);
    vector<int> num_vec(numbers, numbers + num_size);
    ASSERT_EQ(num_size, static_cast<size_t>(num_vec.size()));

    BoundedBlockingQueue<int> b_queue(num_size - 1);
    {
        Thread t1((PushThreadFunc<int>(num_vec, b_queue)));
        unittest::MilliSleep(500);
        ASSERT_TRUE(b_queue.IsFull());
        ASSERT_EQ(num_size - 1, b_queue.Size());

        ASSERT_EQ(numbers[0], b_queue.Pop());
    }

    ASSERT_TRUE(b_queue.IsFull());
    ASSERT_EQ(num_size - 1, b_queue.Size());

    for (size_t i = 0; i < num_size - 1; ++i)
    {
        ASSERT_EQ(numbers[i + 1], b_queue.Pop());
    }
}

TEST(BoundedBlockingQueueTestSuite, test_NonblockingPush)
{
    int numbers[] = {5, 4, 3, 1, 6};
    size_t num_size = sizeof(numbers) / sizeof(int);

    BoundedBlockingQueue<int> b_queue(num_size - 1);
    for (size_t i = 0; i < num_size - 1; ++i)
    {
        ASSERT_FALSE(b_queue.IsFull());
        ASSERT_TRUE(b_queue.NonblockingPush(numbers[i]));
    }
    ASSERT_TRUE(b_queue.IsFull());
    ASSERT_EQ(num_size - 1, b_queue.Size());

    ASSERT_FALSE(b_queue.NonblockingPush(numbers[num_size - 1]));
    ASSERT_TRUE(b_queue.IsFull());
    ASSERT_EQ(num_size - 1, b_queue.Size());
    for (size_t i = 0; i < num_size - 1; ++i)
    {
        ASSERT_EQ(numbers[i], b_queue.Pop());
    }
}

TEST(BoundedBlockingQueueTestSuite, test_NonblockingPop)
{
    int numbers[] = {5, 4, 3, 1, 6};
    size_t num_size = sizeof(numbers) / sizeof(int);

    BoundedBlockingQueue<int> b_queue(num_size);
    ASSERT_FALSE(b_queue.IsFull());
    ASSERT_TRUE(b_queue.NonblockingPush(numbers[0]));

    int tmp_num = 0;
    ASSERT_TRUE(b_queue.NonblockingPop(tmp_num));
    ASSERT_EQ(numbers[0], tmp_num);
    ASSERT_TRUE(b_queue.IsEmpty());

    ASSERT_FALSE(b_queue.NonblockingPop(tmp_num));
    ASSERT_TRUE(b_queue.IsEmpty());
}

TEST(BoundedBlockingQueueTestSuite, test_NonblockingFront)
{
    BoundedBlockingQueue<int> b_queue(4);
    ASSERT_FALSE(b_queue.IsFull());
    ASSERT_TRUE(b_queue.IsEmpty());

    int tmp_num = 0;
    ASSERT_FALSE(b_queue.NonblockingFront(tmp_num));
    ASSERT_FALSE(b_queue.IsFull());
    ASSERT_TRUE(b_queue.IsEmpty());
}


