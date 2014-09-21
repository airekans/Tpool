#include "Timer.h"
#include "TestUtil.h"

#include <gtest/gtest.h>

using namespace tpool;
using namespace tpool::unittest;

namespace {
  class TimerTestSuite : public ::testing::Test
  {
  protected:
    unsigned counter;

    virtual void SetUp()
    {
      counter = 0;
    }
  };

  struct TestTimerTask : public TimerTask
  {
    unsigned& counter;

    TestTimerTask(unsigned& cnt)
    : counter(cnt)
    {}

    virtual void DoRun()
    {
      ++counter;
    }
  };

}

TEST_F(TimerTestSuite, test_Ctor)
{
  Timer timer;
}

TEST_F(TimerTestSuite, test_RunLater)
{
  {
    Timer timer;
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 200);
    ASSERT_EQ(0, counter);

    MilliSleep(500);
    ASSERT_EQ(1, counter);
  }
  ASSERT_EQ(1, counter);
}

TEST_F(TimerTestSuite, test_RunLater_and_cancel_task)
{
  {
    Timer timer;
    TimerTask::Ptr task(new TestTimerTask(counter));
    timer.RunLater(task, 200);
    ASSERT_EQ(0, counter);

    task->Cancel();
    ASSERT_EQ(0, counter);
  }
  ASSERT_EQ(0, counter);
}

TEST_F(TimerTestSuite, test_RunLater_and_async_cancel_task)
{
  {
    Timer timer;
    TimerTask::Ptr task(new TestTimerTask(counter));
    timer.RunLater(task, 200);
    ASSERT_EQ(0, counter);

    task->CancelAsync();
    ASSERT_EQ(0, counter);
  }
  ASSERT_EQ(0, counter);
}

TEST_F(TimerTestSuite, test_RunEvery)
{
  {
    Timer timer;
    TimerTask::Ptr task(new TestTimerTask(counter));
    timer.RunEvery(task, 500, true);
    MilliSleep(100);
    ASSERT_EQ(1, counter);

    MilliSleep(1100);
    ASSERT_EQ(3, counter);
  }
  ASSERT_EQ(3, counter);
}

