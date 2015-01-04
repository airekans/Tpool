#include "Timer.h"
#include "Atomic.h"
#include "Thread.h"
#include "TestUtil.h"

#include <gtest/gtest.h>

using namespace tpool;
using namespace tpool::unittest;

namespace {
  class TimerTestSuite : public ::testing::Test
  {
  protected:
    Atomic<unsigned> counter;

    TimerTestSuite()
    : counter(0)
    {}

    virtual void SetUp()
    {
      counter = 0;
    }
  };

  struct TestTimerTask : public TimerTask
  {
    Atomic<unsigned>& counter;

    TestTimerTask(Atomic<unsigned>& cnt)
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

TEST_F(TimerTestSuite, test_RunLater_with_same_task)
{
  {
    Timer timer;
    TimerTask::Ptr task(new TestTimerTask(counter));
    ASSERT_TRUE(timer.RunLater(task, 200));
    ASSERT_FALSE(timer.RunLater(task, 200));
    ASSERT_EQ(0, counter);

    MilliSleep(300);
    ASSERT_EQ(1, counter);
  }
  ASSERT_EQ(1, counter);
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

TEST_F(TimerTestSuite, test_RunEvery_with_same_task)
{
  {
    Timer timer;
    TimerTask::Ptr task(new TestTimerTask(counter));
    ASSERT_TRUE(timer.RunEvery(task, 500, true));
    ASSERT_FALSE(timer.RunEvery(task, 500, true));
    MilliSleep(100);
    ASSERT_EQ(1, counter);

    MilliSleep(1100);
    ASSERT_EQ(3, counter);
  }
  ASSERT_EQ(3, counter);
}

TEST_F(TimerTestSuite, test_RunEvery_and_not_run_now)
{
  {
    Timer timer;
    TimerTask::Ptr task(new TestTimerTask(counter));
    timer.RunEvery(task, 500, false);
    MilliSleep(100);
    ASSERT_EQ(0, counter);

    MilliSleep(1100);
    ASSERT_EQ(2, counter);
  }
  ASSERT_EQ(2, counter);
}

TEST_F(TimerTestSuite, test_RunEvery_and_cancel)
{
  {
    Timer timer;
    TimerTask::Ptr task(new TestTimerTask(counter));
    timer.RunEvery(task, 500, true);
    MilliSleep(100);
    ASSERT_EQ(1, counter);

    MilliSleep(500);
    ASSERT_EQ(2, counter);
    task->Cancel();

    MilliSleep(500);
    ASSERT_EQ(2, counter);
  }
  ASSERT_EQ(2, counter);
}

TEST_F(TimerTestSuite, test_Run_with_multiple_tasks)
{
  {
    Timer timer;
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 200);
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 100);
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 300);
    ASSERT_EQ(0, counter);

    MilliSleep(200);
    ASSERT_GT(counter, 0);

    MilliSleep(500);
    ASSERT_EQ(3, counter);
  }
  ASSERT_EQ(3, counter);
}

namespace {
  struct TestTimerFunc
  {
    Atomic<unsigned>& counter;

    TestTimerFunc(Atomic<unsigned>& cnt)
    : counter(cnt)
    {}

    void operator()()
    {
      ++counter;
    }
  };
}

TEST_F(TimerTestSuite, test_RunLater_with_functor)
{
  {
    Timer timer;
    timer.RunLater(TestTimerFunc(counter), 200);
    ASSERT_EQ(0, counter);

    MilliSleep(500);
    ASSERT_EQ(1, counter);
  }
  ASSERT_EQ(1, counter);
}

namespace {
  struct RunLaterThread {
    Timer& m_timer;
    Atomic<unsigned>& m_counter;
    TimeValue m_delay;

    RunLaterThread(Timer& timer, Atomic<unsigned>& counter,
        TimeValue delay)
    : m_timer(timer), m_counter(counter), m_delay(delay)
    {}

    void operator()()
    {
      m_timer.RunLater(TestTimerFunc(m_counter), m_delay);
    }
  };
}

TEST_F(TimerTestSuite, test_RunLater_with_multiple_threads)
{
  {
    Timer timer;

    // create 2 threads to do RunLater
    Thread thread1(RunLaterThread(timer, counter, 100));
    Thread thread2(RunLaterThread(timer, counter, 100));
    timer.RunLater(TestTimerFunc(counter), 100);

    MilliSleep(300);
    ASSERT_EQ(3, counter);
  }
  ASSERT_EQ(3, counter);
}

TEST_F(TimerTestSuite, test_RunEvery_with_functor)
{
  {
    Timer timer;
    timer.RunEvery(TestTimerFunc(counter), 500, false);
    ASSERT_EQ(0, counter);

    MilliSleep(600);
    ASSERT_EQ(1, counter);

    MilliSleep(500);
    ASSERT_EQ(2, counter);
  }
  ASSERT_EQ(2, counter);
}

namespace {
  struct RunEveryThread {
    Timer& m_timer;
    Atomic<unsigned>& m_counter;
    TimeValue m_delay;
    bool m_is_run_now;

    RunEveryThread(Timer& timer, Atomic<unsigned>& counter,
        TimeValue delay, bool is_run_now)
    : m_timer(timer), m_counter(counter), m_delay(delay),
      m_is_run_now(is_run_now)
    {}

    void operator()()
    {
      m_timer.RunEvery(TestTimerFunc(m_counter), m_delay,
          m_is_run_now);
    }
  };
}

TEST_F(TimerTestSuite, test_RunEvery_with_multiple_threads)
{
  {
    Timer timer;

    // create 2 threads to do RunEvery
    Thread thread1(RunEveryThread(timer, counter, 200, true));
    Thread thread2(RunEveryThread(timer, counter, 200, true));
    timer.RunEvery(TestTimerFunc(counter), 200, true);

    MilliSleep(100);
    ASSERT_EQ(3, counter);

    MilliSleep(200);
    ASSERT_EQ(6, counter);
  }
  ASSERT_EQ(6, counter);
}

TEST_F(TimerTestSuite, test_StopAsync)
{
  {
    Timer timer;
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 500);
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 100);
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 1000);
    ASSERT_EQ(0, counter);

    MilliSleep(200);
    ASSERT_EQ(1, counter);

    timer.StopAsync();
  }
  ASSERT_EQ(1, counter);
}

TEST_F(TimerTestSuite, test_RunLater_after_StopAsync)
{
  {
    Timer timer;
    TimerTask::Ptr task(new TestTimerTask(counter));
    ASSERT_TRUE(timer.RunLater(task, 100));
    ASSERT_EQ(0, counter);

    MilliSleep(300);
    ASSERT_EQ(1, counter);

    timer.StopAsync();
    TimerTask::Ptr task1(new TestTimerTask(counter));
    ASSERT_FALSE(timer.RunLater(task1, 100));
    ASSERT_EQ(1, counter);

    MilliSleep(300);
    ASSERT_EQ(1, counter);
  }
  ASSERT_EQ(1, counter);
}

TEST_F(TimerTestSuite, test_Stop)
{
  {
    Timer timer;
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 500);
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 100);
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 1000);
    ASSERT_EQ(0, counter);

    MilliSleep(200);
    ASSERT_EQ(1, counter);

    timer.Stop();

    MilliSleep(500);
    ASSERT_EQ(1, counter);
  }
  ASSERT_EQ(1, counter);
}

namespace {
  struct TimerStopThread {
    Timer& m_timer;

    TimerStopThread(Timer& timer)
    : m_timer(timer)
    {}

    void operator()()
    {
      m_timer.Stop();
    }
  };
}

TEST_F(TimerTestSuite, test_Stop_with_multiple_threads)
{
  {
    Timer timer;
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 500);
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 100);
    timer.RunLater(TimerTask::Ptr(new TestTimerTask(counter)), 1000);
    ASSERT_EQ(0, counter);

    MilliSleep(200);
    ASSERT_EQ(1, counter);

    Thread thread1((TimerStopThread(timer)));
    Thread thread2((TimerStopThread(timer)));
    timer.Stop();

    MilliSleep(500);
    ASSERT_EQ(1, counter);
  }
  ASSERT_EQ(1, counter);
}

TEST_F(TimerTestSuite, test_RunLater_after_Stop)
{
  {
    Timer timer;
    TimerTask::Ptr task(new TestTimerTask(counter));
    ASSERT_TRUE(timer.RunLater(task, 100));
    ASSERT_EQ(0, counter);

    MilliSleep(300);
    ASSERT_EQ(1, counter);

    timer.Stop();
    TimerTask::Ptr task1(new TestTimerTask(counter));
    ASSERT_FALSE(timer.RunLater(task1, 100));
    ASSERT_EQ(1, counter);

    MilliSleep(300);
    ASSERT_EQ(1, counter);
  }
  ASSERT_EQ(1, counter);
}
