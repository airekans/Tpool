#include "FixedThreadPool.h"
#include <gtest/gtest.h>
#include <boost/bind.hpp>

using namespace tpool;
using namespace boost;

TEST(FixedThreadPoolTestSuite, test_Construction)
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
  struct IncTask : public TaskBase {
    int& counter;
    
    IncTask(int& i) : counter(i) {}

    virtual void Do()
    {
      counter = 1;
    }
  };
}

TEST(FixedThreadPoolTestSuite, test_AddTask)
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

TEST(FixedThreadPoolTestSuite, test_template_AddTask)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    threadPool.AddTask(bind(IncrementWith, &counter));
  }
  ASSERT_EQ(1, counter);
}
