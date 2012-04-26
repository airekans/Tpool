#include "LinearTaskQueue.h"
#include "Thread.h"
#include "TaskBase.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <vector>

using namespace std;
using namespace tpool;
using namespace boost;

namespace {
  struct EmptyTask : public TaskBase {
    void DoRun() {}
  };

  struct PushThreadFunc {
    const int num;
    LinearTaskQueue& taskQueue;
    
    PushThreadFunc(LinearTaskQueue& q, const int n)
      : taskQueue(q), num(n)
    {}

    virtual void DoRun()
    {
      for (int i = 0; i < num; ++i)
	{
	  taskQueue.Push(TaskBase::Ptr(new EmptyTask));
	}
    }
    
    void operator()()
    {
      DoRun();
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

namespace {
  struct ThreadSafeInt {
    int i;
    sync::Mutex mutex;

    ThreadSafeInt() : i(0) {}
    ThreadSafeInt& operator++()
    {
      sync::MutexLocker l(mutex);
      ++i;
      return *this;
    }
  };
  
  struct IncTask : public TaskBase {
    ThreadSafeInt& counter;

    IncTask(ThreadSafeInt& i)
      : counter(i)
    {}

    void DoRun()
    {
      ++counter;
    }
  };

  struct PushIncTaskThreadFunc : public PushThreadFunc {
    ThreadSafeInt& counter;

    PushIncTaskThreadFunc(LinearTaskQueue& q, int n, ThreadSafeInt& i)
      : PushThreadFunc(q, n), counter(i)
    {}

    virtual void DoRun()
    {
      for (int i = 0; i < num; ++i)
	{
	  taskQueue.Push(TaskBase::Ptr(new IncTask(counter)));
	}
    }
  };

  struct RunTaskThreadFunc {
    LinearTaskQueue& taskQueue;

    RunTaskThreadFunc(LinearTaskQueue& q)
      : taskQueue(q)
    {}

    void operator()()
    {
      TaskBase::Ptr t = taskQueue.Pop();
      t->Run();
    }
  };
}

TEST(LinearTaskQueueTestSuite, test_NormalUsage)
{
  LinearTaskQueue taskQueue;
  ThreadSafeInt counter;
  {
    typedef shared_ptr<Thread> ThreadPtr;
    vector<ThreadPtr> workers;

    for (int i = 0; i < 10; ++i)
      {
	workers.push_back(ThreadPtr(
	  new Thread((RunTaskThreadFunc(taskQueue)))));
      }
    Thread t2((PushIncTaskThreadFunc(taskQueue, 10, counter)));
  }

  ASSERT_EQ(10, counter.i);
}
