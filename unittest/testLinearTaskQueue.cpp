#include "LinearTaskQueue.h"
#include "Thread.h"
#include "TaskBase.h"
#include <gtest/gtest.h>
#include <cstdlib>

using namespace tpool;

namespace {
  struct EmptyTask : public TaskBase {
    void Do() {}
  };
  
  struct PushThreadFunc {
    const int num;
    LinearTaskQueue& taskQueue;
    
    PushThreadFunc(LinearTaskQueue& q, const int n)
      : taskQueue(q), num(n)
    {}

    void operator()()
    {
      for (int i = 0; i < num; ++i)
	{
	  taskQueue.Push(TaskBase::Ptr(new EmptyTask));
	}
    }
  };
}

TEST(LinearTaskQueueTestSuite, test_Push)
{
  LinearTaskQueue taskQueue;
  {
    Thread t1((PushThreadFunc(taskQueue, 20)));
    Thread t2((PushThreadFunc(taskQueue, 50)));
  }

  ASSERT_EQ(70, taskQueue.Size());
}

namespace {
  struct PopThreadFunc {
    LinearTaskQueue& taskQueue;
    
    PopThreadFunc(LinearTaskQueue& q)
      : taskQueue(q)
    {
    }

    void operator()()
    {
      TaskBase::Ptr b = taskQueue.Pop();
    }
  };
}

TEST(LinearTaskQueueTestSuite, test_PopWait)
{
  LinearTaskQueue taskQueue;
  {
    Thread popThread((PopThreadFunc(taskQueue)));
    sleep(1);
    Thread pushThread((PushThreadFunc(taskQueue, 10)));
  }

  ASSERT_EQ(9, taskQueue.Size());
}
