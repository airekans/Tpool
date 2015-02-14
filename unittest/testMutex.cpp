#include "Thread.h"
#include "Mutex.h"
#include "TestUtil.h"

#include <iostream>
#include <gtest/gtest.h>
#include <cstdlib>
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace tpool;
using namespace tpool::sync;

namespace {
int GLOBAL_COUNTER = 0;
Mutex g_mutex;

struct ThreadFunctor {
    ThreadFunctor(Mutex& mut)
    : m(mut)
    {

    }

    void operator()()
    {
        for (int i = 0; i < 20; ++i)
        {
            MutexLocker l(m);
            ++GLOBAL_COUNTER;
        }
    }

    Mutex& m;
};

}


TEST(MutexTestSuite, test_MutexLocker)
{
    {
        Thread t1((ThreadFunctor(g_mutex)));
        Thread t2((ThreadFunctor(g_mutex)));

        for (int i = 0; i < 20; ++i)
        {
            MutexLocker l(g_mutex);
            ++GLOBAL_COUNTER;
        }
    }

    ASSERT_EQ(60, GLOBAL_COUNTER);
}

namespace {
struct IncThreadFunc {
    Mutex& mutex;
    int& counter;

    IncThreadFunc(Mutex& m, int& i)
    : mutex(m), counter(i)
    {
    }

    void operator()()
    {
        for (int i = 0; i < 2; ++i)
        {
            {
                MutexLocker l(mutex);
                ++counter;
            }
            sleep(1);
        }
    }
};

struct WaitThreadFunc {
    int& wakeCount;
    int& counter;
    Mutex& mutex;

    WaitThreadFunc(Mutex& m, int& i, int& wc)
    : wakeCount(wc), counter(i), mutex(m)
    {
        wakeCount = -1;
    }

    struct GreaterThanFunc {
        int& counter;
        GreaterThanFunc(int& i)
        : counter(i)
        {
        }

        bool operator()()
        {
            return counter < 1;
        }
    };

    void operator()()
    {
        MutexWaitLocker lock(mutex,
                (GreaterThanFunc(counter)));
        cout << "wait end: counter " << counter << endl;
        wakeCount = counter; // wakeCount should > 0 now
    }
};
}

TEST(MutexTestSuite, test_MutexWaitLocker)
{
    int counter = 0;
    int wakeCount = -1;
    Mutex m;
    {
        Thread t1((WaitThreadFunc(m, counter, wakeCount)));
        Thread t2((IncThreadFunc(m, counter)));
    }

    ASSERT_GT(wakeCount, 0);
}

namespace {

struct TryLockThreadFunc {
    int& counter;
    int times;
    Mutex& mutex;

    TryLockThreadFunc(Mutex& m, int& cnt, int n)
    : counter(cnt), times(n), mutex(m)
    {}

    void operator()()
    {
        for (int i = 0; i < times; ++i)
        {
            MutexTryLocker lock(mutex);
            while (!lock.TryLock())
            {
                ::tpool::unittest::MilliSleep(100);
            }
            ++counter;
        }
    }
};

}

TEST(MutexTestSuite, test_MutexTryLocker)
{
    int counter = 0;
    Mutex m;
    {
        Thread t1((IncThreadFunc(m, counter)));
        Thread t2((TryLockThreadFunc(m, counter, 10)));
        Thread t3((TryLockThreadFunc(m, counter, 20)));
    }

    ASSERT_EQ(32, counter);
}

