#include "Thread.h"
#include "ConditionVariable.h"

#include <gtest/gtest.h>

#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <cstdlib>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace tpool;

namespace {
int GLOBAL_COUNTER = 0;

struct ThreadFunctor {
    void operator()()
    {
        for (int i = 0; i < 20; ++i)
        {
            ++GLOBAL_COUNTER;
        }
    }
};

}


TEST(BasicThreadTestSuite, test_lifecycle)
{
    {
        Thread t((ThreadFunctor()));
    }

    ASSERT_EQ(20, GLOBAL_COUNTER);
}

namespace {
struct ThreadFuncThrowException {
    explicit ThreadFuncThrowException(int& num)
    : m_num(num)
    {
    }

    virtual ~ThreadFuncThrowException() {}

    void operator()()
    {
        m_num = 0;
        ThrowException();
        m_num = 1;
    }

    virtual void ThrowException()
    {
        throw runtime_error("exit thread function");
    }

    int& m_num;
};
}

TEST(BasicThreadTestSuite, test_threadFuncThrowException)
{
    int i = -1;
    ThreadFuncThrowException f(i);

    {
        Thread t(f);
    }

    ASSERT_EQ(0, i);
}

namespace {
class ThreadFuncThrowUnknownException : public ThreadFuncThrowException {
public:
    ThreadFuncThrowUnknownException(int& i)
    : ThreadFuncThrowException(i)
    {}

    virtual void ThrowException()
    {
        throw 1;
    }
};
}

TEST(BasicThreadTestSuite, test_threadFuncThrowUnknownException)
{
    int i = -1;
    ThreadFuncThrowUnknownException f(i);
    {
        Thread t(f);
    }

    ASSERT_EQ(0, i);
}

namespace {

struct RunThreadFunctor {
    sync::MutexConditionVariable& m_cond;
    bool& m_isStart;

    RunThreadFunctor(sync::MutexConditionVariable& cond,
                     bool& isStart)
    : m_cond(cond), m_isStart(isStart)
    {}

    void operator()()
    {
        sync::ConditionNotifyLocker lock(m_cond);
        m_isStart = true;
    }
};

bool IsNotStart(bool& isStart)
{
    return !isStart;
}

}

TEST(BasicThreadTestSuite, test_GetThreadId)
{
    bool isStart = false;
    ::tpool::sync::MutexConditionVariable cond;
    Thread t((RunThreadFunctor(cond, isStart)));

    ::tpool::sync::ConditionWaitLocker lock(cond,
            ::boost::bind(&IsNotStart, ::boost::ref(isStart)));
    ASSERT_GT(t.GetThreadId(), 0);
}


