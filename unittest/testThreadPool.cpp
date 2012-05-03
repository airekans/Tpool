#include "FixedThreadPool.h"
#include <gtest/gtest.h>
#include <boost/bind.hpp>
#include <unistd.h>

using namespace tpool;
using namespace boost;
using namespace std;


TEST(FixedThreadPool, test_Construction)
{
  {
    LFixedThreadPool threadPool;
    EXPECT_EQ(10, threadPool.GetThreadNum());
  }
  
  {
    LFixedThreadPool threadPool(5);
    EXPECT_EQ(5, threadPool.GetThreadNum());
  }
}

namespace {
  sync::Mutex GLOBAL_MUTEX;
  
  struct IncTask : public TaskBase {
    int& counter;
    
    IncTask(int& i) : counter(i) {}

    virtual void DoRun()
    {
      sync::MutexLocker l(GLOBAL_MUTEX);
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

    EXPECT_GT(counter, num);
    EXPECT_FALSE(threadPool.AddTask(TaskBase::Ptr(new IncTask(counter))));
  }
}
