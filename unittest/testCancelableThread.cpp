#include "CancelableThread.h"

#include <boost/function.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

using namespace std;

typedef ::boost::function<void()> CancelFunction;

namespace {

void NoOpWork(CancelFunction)
{}

void NoOp()
{}

class IncNumWork
{
    int& m_num;
public:
    IncNumWork(int& num)
    : m_num(num)
    {}

    void operator() (::tpool::CancelableThread::Function)
    {
        ++m_num;
    }
};

}

TEST(CancelableThread, test_Ctor)
{
    {
        ::tpool::CancelableThread thread((&NoOpWork));
    }

    {
        ::tpool::CancelableThread thread(&NoOpWork, &NoOp);
    }
}

TEST(CancelableThread, test_normal_usage)
{
    int num = 0;
    {
        ::tpool::CancelableThread thread((IncNumWork(num)));
    }

    ASSERT_EQ(1, num);
}

namespace {

struct TestWork
{
    int& counter;

    TestWork(int& i)
    : counter(i)
    {}

    void operator() (::tpool::CancelableThread::Function cancelFunction)
    {
        sleep(2);
        cancelFunction();
        ++counter;
    }
};

}


TEST(CancelableThread, test_Cancel)
{
    int counter = 0;
    {
        ::tpool::CancelableThread t((TestWork(counter)));
        sleep(1);
        t.Cancel();
        ASSERT_EQ(0, counter);
    }
    ASSERT_EQ(0, counter);
}

TEST(CancelableThread, test_multiple_Cancel)
{
    int counter = 0;
    {
        ::tpool::CancelableThread t((TestWork(counter)));
        sleep(1);
        t.Cancel();
        t.Cancel(); // should not block the execution.
    }
    ASSERT_EQ(0, counter);
}

TEST(CancelableThread, test_CancelAsync)
{
    int counter = 0;
    {
        ::tpool::CancelableThread t((TestWork(counter)));
        sleep(1);
        t.CancelAsync();
        ASSERT_EQ(0, counter);
    }
    ASSERT_EQ(0, counter);
}

namespace {

struct FinishAction
{
    int& counter;

    FinishAction(int& i)
    : counter(i)
    {}

    void operator()()
    {
        ++counter;
    }
};

}

TEST(CancelableThread, test_ctor_with_FinishAction)
{
    int counter = 0;
    int finishCounter = 0;
    {
        ::tpool::CancelableThread t((TestWork(counter)),
                                    FinishAction(finishCounter));
    }
    ASSERT_EQ(1, counter);
    ASSERT_EQ(1, finishCounter);
}

TEST(CancelableThread, test_FinishAcion_execute_before_Finished)
{
    int counter = 0;
    int finishFlag = 0;
    {
        ::tpool::CancelableThread t((TestWork(counter)),
                                    FinishAction(finishFlag));
        sleep(1);
        t.Cancel();
        EXPECT_EQ(0, counter);
        EXPECT_EQ(1, finishFlag);
    }
    EXPECT_EQ(0, counter);
    EXPECT_EQ(1, finishFlag);
}


