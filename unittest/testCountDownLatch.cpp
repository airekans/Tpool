#include "CountDownLatch.h"
#include "Thread.h"
#include "Atomic.h"

#include <boost/shared_ptr.hpp>

#include <gtest/gtest.h>

#include <vector>

using namespace tpool;
using namespace std;

TEST(CountDownLatchTestSuite, test_ctor)
{
    sync::CountDownLatch latch(1);
}

namespace {

struct IncThreadFunc {
    Atomic<int>& m_count;
    sync::CountDownLatch& m_latch;

    IncThreadFunc(Atomic<int>& count, sync::CountDownLatch& latch)
    : m_count(count), m_latch(latch)
    {}

    void operator()()
    {
        m_latch.Wait();
        ++m_count;
    }
};

}

TEST(CountDownLatchTestSuite, test_wait)
{
    sync::CountDownLatch latch(1);
    Atomic<int> count(0);
    ASSERT_EQ(0, count);
    {
        Thread t((IncThreadFunc(count, latch)));
        ASSERT_EQ(0, count);
        latch.CountDown();
    }
    ASSERT_EQ(1, count);
}

typedef ::boost::shared_ptr<Thread> ThreadPtr;

TEST(CountDownLatchTestSuite, test_multiple_wait)
{
    sync::CountDownLatch latch(1);
    Atomic<int> count(0);
    ASSERT_EQ(0, count);

    const int times = 10;
    {
        vector<ThreadPtr> threads;
        for (int i = 0; i < times; ++i)
        {
            ThreadPtr t(new Thread((IncThreadFunc(count, latch))));
            threads.push_back(t);
        }
        ASSERT_EQ(0, count);
        latch.CountDown();
    }
    ASSERT_EQ(times, count);
}

namespace {

struct CountDownFunc {
    Atomic<int>& m_count;
    sync::CountDownLatch& m_latch;

    CountDownFunc(Atomic<int>& count, sync::CountDownLatch& latch)
    : m_count(count), m_latch(latch)
    {}

    void operator()()
    {
        ++m_count;
        m_latch.CountDown();
    }
};

}

TEST(CountDownLatchTestSuite, test_countdown)
{
    sync::CountDownLatch latch(1);
    Atomic<int> count(0);
    ASSERT_EQ(0, count);
    {
        Thread t((CountDownFunc(count, latch)));
        latch.Wait();
        ASSERT_EQ(1, count);
    }
    ASSERT_EQ(1, count);
}

TEST(CountDownLatchTestSuite, test_multiple_countdown)
{
    const int times = 10;
    sync::CountDownLatch latch(times);
    Atomic<int> count(0);
    ASSERT_EQ(0, count);

    {
        vector<ThreadPtr> threads;
        for (int i = 0; i < times; ++i)
        {
            ThreadPtr t(new Thread((CountDownFunc(count, latch))));
            threads.push_back(t);
        }
        latch.Wait();
        ASSERT_EQ(times, count);
    }
    ASSERT_EQ(times, count);
}

TEST(CountDownLatchTestSuite, test_wait_after_fire)
{
    sync::CountDownLatch latch(1);
    Atomic<int> count(0);
    ASSERT_EQ(0, count);
    {
        Thread t((IncThreadFunc(count, latch)));
        ASSERT_EQ(0, count);
        latch.CountDown();
    }
    ASSERT_EQ(1, count);

    latch.Wait();  // should not block
    ASSERT_EQ(1, count);
    latch.Wait();  // should not block
    ASSERT_EQ(1, count);
}

