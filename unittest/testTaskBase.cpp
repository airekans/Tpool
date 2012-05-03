#include "TaskBase.h"
#include "Thread.h"
#include <gtest/gtest.h>
#include <unistd.h>

using namespace tpool;

namespace {
  struct FakeTask : public TaskBase {
    virtual void DoRun()
    {
      sleep(2);
    }
  };

  struct ThreadFunc {
    TaskBase& m_task;
    
    ThreadFunc(TaskBase& task)
      : m_task(task)
    {}
    
    void operator()()
    {
      m_task.Run();
    }
  };
}

TEST(TaskBase, test_GetState)
{
  FakeTask task;

  EXPECT_EQ(TaskBase::INIT, task.GetState());

  {
    Thread t((ThreadFunc(task)));
    sleep(1);
    EXPECT_EQ(TaskBase::RUNNING, task.GetState());
  }

  EXPECT_EQ(TaskBase::FINISHED, task.GetState());
}

namespace {
  struct CancelTask : public TaskBase {
    virtual void DoRun()
    {
      sleep(2);
      CheckCancellation();
    }
  };
}

TEST(TaskBase, test_Cancel)
{
  CancelTask task;

  EXPECT_EQ(TaskBase::INIT, task.GetState());

  {
    Thread t((ThreadFunc(task)));
    sleep(1);
    EXPECT_EQ(TaskBase::RUNNING, task.GetState());

    task.Cancel();
  }
  EXPECT_EQ(TaskBase::CANCELLED, task.GetState());
}
