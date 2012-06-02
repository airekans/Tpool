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

namespace {
  struct CancelFunction {
    TaskBase& m_task;

    CancelFunction(TaskBase& task)
      : m_task(task)
    {}

    void operator()()
    {
      m_task.Cancel();
    }
  };
}

TEST(TaskBase, test_multiple_Cancel_simultunuously)
{
  CancelTask task;

  EXPECT_EQ(TaskBase::INIT, task.GetState());

  {
    Thread t((ThreadFunc(task)));
    sleep(1);
    EXPECT_EQ(TaskBase::RUNNING, task.GetState());
    Thread t1((CancelFunction(task)));
    task.Cancel();
  }
  EXPECT_EQ(TaskBase::CANCELLED, task.GetState());
}

namespace {
  struct NotRunningTask : public TaskBase {
    bool& running;

    NotRunningTask(bool& r)
      : running(r)
    {}

    virtual void DoRun()
    {
      running = true;
    }
  };
}

TEST(TaskBase, test_Cancel_before_Run)
{
  bool running = false;

  NotRunningTask task(running);
  task.CancelAsync();
  task.Run();

  EXPECT_EQ(false, running);
  EXPECT_EQ(TaskBase::CANCELLED, task.GetState());
}
