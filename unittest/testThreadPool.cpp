#include "FixedThreadPool.h"
#include "TestUtil.h"

#include <gtest/gtest.h>
#include <boost/bind.hpp>
#include <unistd.h>

using namespace tpool;
using namespace tpool::sync;
using namespace boost;
using namespace std;
using namespace tpool::unittest;


TEST(FixedThreadPool, test_Construction)
{
  {
    LFixedThreadPool threadPool;
    EXPECT_EQ(static_cast<size_t>(4), threadPool.GetThreadNum());
  }
  
  {
    LFixedThreadPool threadPool(5);
    EXPECT_EQ(static_cast<size_t>(5), threadPool.GetThreadNum());
  }
}

namespace {
  Mutex GLOBAL_MUTEX;
  
  struct IncTask : public TaskBase {
    int& counter;
    
    IncTask(int& i) : counter(i) {}

    virtual void DoRun()
    {
      MutexLocker l(GLOBAL_MUTEX);
      ++counter;
    }
  };
}

TEST(FixedThreadPool, test_AddTask)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    threadPool.AddTask(TaskBase::Ptr(new IncTask(counter)));
  }
  ASSERT_EQ(1, counter);
}

namespace {
  void IncrementWith(int* i)
  {
    *i = 1;
  }
}

TEST(FixedThreadPool, test_template_AddTask)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    threadPool.AddTask(bind(IncrementWith, &counter));
  }
  ASSERT_EQ(1, counter);
}

namespace {
  struct SleepFunc {
    void operator()()
    {
      sleep(1);
    }
  };

  struct SleepAndIncTask : public IncTask {
    SleepAndIncTask(int& i)
      : IncTask(i)
    {}
    
    virtual void DoRun()
    {
      sleep(1);
      IncTask::DoRun();
    }
  };
}

TEST(FixedThreadPool, test_StopAsync)
{
  {
    LFixedThreadPool threadPool;
    threadPool.AddTask(SleepFunc());
    threadPool.StopAsync();
    EXPECT_FALSE(threadPool.AddTask(SleepFunc()));
  }

  int counter = 0;
  {
    LFixedThreadPool threadPool;
    threadPool.AddTask(TaskBase::Ptr(new SleepAndIncTask(counter)));
    threadPool.StopAsync();
    EXPECT_EQ(0, counter);
  }
  EXPECT_EQ(1, counter);
}

TEST(FixedThreadPool, test_Stop)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    threadPool.AddTask(TaskBase::Ptr(new SleepAndIncTask(counter)));
    EXPECT_EQ(0, counter);
    threadPool.Stop();
    EXPECT_EQ(1, counter);
    EXPECT_FALSE(threadPool.AddTask(TaskBase::Ptr(new IncTask(counter))));
  }
}

TEST(FixedThreadPool, test_Stop_when_TaskQueue_empty)
{
  {
    LFixedThreadPool threadPool;
    threadPool.Stop();
  }
}

namespace {
  struct StopFunction {
    LFixedThreadPool& m_threadPool;

    StopFunction(LFixedThreadPool& threadPool)
      : m_threadPool(threadPool)
    {}
    
    void operator()()
    {
      m_threadPool.Stop();
    }
  };
}

TEST(FixedThreadPool, test_multiple_Stop_simultinuously)
{
  {
    LFixedThreadPool threadPool;
    Thread t((StopFunction(threadPool)));
    threadPool.Stop();
  }
}

namespace {
  struct LoopSleepAndIncTask : public SleepAndIncTask {
    LoopSleepAndIncTask(int& i)
      : SleepAndIncTask(i)
    {}
    
    virtual void DoRun()
    {
      for (int i = 0; i < 2; ++i)
	{
	  SleepAndIncTask::DoRun();
	  sleep(1);
	  CheckCancellation();
	}
    }
  };
}

TEST(FixedThreadPool, test_StopNow)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    const size_t num = threadPool.GetThreadNum();
    for (size_t i = 0; i < num; ++i)
      {
	threadPool.AddTask(TaskBase::Ptr(new LoopSleepAndIncTask(counter)));
      }
    sleep(1);
    threadPool.StopNow();

    EXPECT_GE(static_cast<size_t>(counter), num);
    EXPECT_FALSE(threadPool.AddTask(TaskBase::Ptr(new IncTask(counter))));
  }
}

TEST(FixedThreadPool, test_StopNow_when_TaskQueue_empty)
{
  {
    LFixedThreadPool threadPool;
    threadPool.StopNow();
  }
}

TEST(FixedThreadPool, test_AddTimerTask)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    threadPool.AddTimerTask(TaskBase::Ptr(new IncTask(counter)), 300);
    ASSERT_EQ(0, counter);

    MilliSleep(100);
    ASSERT_EQ(0, counter);

    MilliSleep(300);
    ASSERT_EQ(1, counter);
  }
  ASSERT_EQ(1, counter);
}

TEST(FixedThreadPool, test_AddTimerTask_and_cancel)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    TaskBase::Ptr task(new IncTask(counter));
    threadPool.AddTimerTask(task, 200);
    ASSERT_EQ(0, counter);
    task->CancelAsync();

    MilliSleep(300);
    ASSERT_EQ(0, counter);
  }
  ASSERT_EQ(0, counter);

  {
    LFixedThreadPool threadPool;
    TaskBase::Ptr task(new IncTask(counter));
    task = threadPool.AddTimerTask(task, 200);
    ASSERT_EQ(0, counter);
    task->CancelAsync();

    MilliSleep(300);
    ASSERT_EQ(0, counter);
  }
  ASSERT_EQ(0, counter);
}

TEST(FixedThreadPool, test_AddIntervalTask)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    threadPool.AddIntervalTask(TaskBase::Ptr(new IncTask(counter)),
        200, false);
    ASSERT_EQ(0, counter);

    MilliSleep(100);
    ASSERT_EQ(0, counter);

    MilliSleep(200);
    ASSERT_EQ(1, counter);

    MilliSleep(200);
    ASSERT_EQ(2, counter);
  }
  MilliSleep(200);
  ASSERT_EQ(2, counter);
}

TEST(FixedThreadPool, test_AddIntervalTask_and_cancel)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    TaskBase::Ptr task(new IncTask(counter));
    threadPool.AddIntervalTask(task, 200, false);
    ASSERT_EQ(0, counter);
    task->CancelAsync();

    MilliSleep(300);
    ASSERT_EQ(0, counter);

    MilliSleep(200);
    ASSERT_EQ(0, counter);
  }
  ASSERT_EQ(0, counter);

  {
    LFixedThreadPool threadPool;
    TaskBase::Ptr task(new IncTask(counter));
    task = threadPool.AddIntervalTask(task, 200, false);
    ASSERT_EQ(0, counter);
    task->CancelAsync();

    MilliSleep(300);
    ASSERT_EQ(0, counter);

    MilliSleep(200);
    ASSERT_EQ(0, counter);
  }
  ASSERT_EQ(0, counter);
}


