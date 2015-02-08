#include "CyclicBarrier.h"
#include "Thread.h"
#include "TestUtil.h"

#include <boost/shared_ptr.hpp>

#include <gtest/gtest.h>

using namespace tpool;
using namespace std;

TEST(CyclicBarrierTestSuite, test_Ctor)
{
    sync::CyclicBarrier barrier(1);
}

TEST(CyclicBarrierTestSuite, test_single_wait)
{
    sync::CyclicBarrier barrier(1);
    barrier.Wait();
}

namespace {

struct IncThreadFunc {
    Atomic<int>& m_count;
    sync::CyclicBarrier& m_barrier;

    IncThreadFunc(Atomic<int>& count, sync::CyclicBarrier& barrier)
    : m_count(count), m_barrier(barrier)
    {}

    void operator()()
    {
        m_barrier.Wait();
        ++m_count;
    }
};

}

typedef ::boost::shared_ptr<Thread> ThreadPtr;

TEST(CyclicBarrierTestSuite, test_multiple_wait)
{
    const int times = 10;
    sync::CyclicBarrier barrier(times);
    Atomic<int> count(0);

    {
        vector<ThreadPtr> threads;
        for (int i = 0; i < times; ++i)
        {
            ThreadPtr t(new Thread((IncThreadFunc(count, barrier))));
            threads.push_back(t);
        }
    }

    ASSERT_EQ(times, count);
}


TEST(CyclicBarrierTestSuite, test_reset)
{
    sync::CyclicBarrier barrier(2);
    Atomic<int> count(0);

    {
        Thread t((IncThreadFunc(count, barrier)));
        unittest::MilliSleep(500);
        ASSERT_EQ(0, count);
        barrier.Reset();
    }

    ASSERT_EQ(1, count);
}

namespace {

struct CycleWaitFunc {
    Atomic<int>& m_count;
    sync::CyclicBarrier& m_barrier;
    const int m_times;

    CycleWaitFunc(Atomic<int>& count, sync::CyclicBarrier& barrier,
                  const int times)
    : m_count(count), m_barrier(barrier), m_times(times)
    {}

    void operator()()
    {
        for (int i = 0; i < m_times; ++i)
        {
            ++m_count;
            m_barrier.Wait();
        }
    }
};

}

TEST(CyclicBarrierTestSuite, test_multiple_wait_cycles)
{
    const int times = 10;
    sync::CyclicBarrier barrier(times);
    Atomic<int> count(0);

    {
        vector<ThreadPtr> threads;
        for (int i = 0; i < times - 1; ++i)
        {
            ThreadPtr t(new Thread((CycleWaitFunc(count, barrier, times))));
            threads.push_back(t);
        }

        for (int i = 0; i < times; ++i)
        {
            barrier.Wait();
            ASSERT_GE(count, (i + 1) * (times - 1));
        }
    }

    ASSERT_EQ(times * (times - 1), count);
}

