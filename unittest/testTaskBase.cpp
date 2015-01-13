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
    bool& m_running;

    NotRunningTask(bool& r)
      : m_running(r)
    {}

    virtual void DoRun()
    {
      m_running = true;
    }
  };
}

TEST(TaskBase, test_Cancel_before_Run)
{
  bool running = false;

  NotRunningTask task(running);
  task.CancelAsync();
  task.Run();

  EXPECT_FALSE(running);
  EXPECT_EQ(TaskBase::CANCELLED, task.GetState());
}

namespace {
  struct NotRunningTaskWithOnCancel : public NotRunningTask {
    bool& m_onCancelFlag;
    
    NotRunningTaskWithOnCancel(bool& onCancelFlag, bool& r)
      : NotRunningTask(r), m_onCancelFlag(onCancelFlag)
    {}

    virtual void OnCancel()
    {
      m_onCancelFlag = true;
    }
  };
}

TEST(TaskBase, test_OnCancel_before_Run)
{
  bool running = false;
  bool onCancelFlag = false;

  NotRunningTaskWithOnCancel task(onCancelFlag, running);
  task.CancelAsync();
  task.Run();

  EXPECT_FALSE(running);
  EXPECT_TRUE(onCancelFlag);
  EXPECT_EQ(TaskBase::CANCELLED, task.GetState());
}

namespace {
  struct RunningTaskWithOnCancel : public NotRunningTaskWithOnCancel {
    RunningTaskWithOnCancel(bool& onCancelFlag, bool& r)
      : NotRunningTaskWithOnCancel(onCancelFlag, r)
    {}

    virtual void DoRun()
    {
      NotRunningTaskWithOnCancel::DoRun();
      sleep(2);
      CheckCancellation();
    }      
  };
}

TEST(TaskBase, test_OnCancel_when_running)
{
  bool running = false;
  bool onCancelFlag = false;

  RunningTaskWithOnCancel task(onCancelFlag, running);
  {
    Thread t((ThreadFunc(task)));
    sleep(1);
    task.Cancel();
  }

  EXPECT_TRUE(running);
  EXPECT_TRUE(onCancelFlag);
  EXPECT_EQ(TaskBase::CANCELLED, task.GetState());
}

namespace {

  struct TaskWithOnCallCancel : public TaskBase {
    bool& m_onCancelFlag;

    TaskWithOnCallCancel(bool& onCancelFlag)
    : m_onCancelFlag(onCancelFlag)
    {}

    virtual void OnCallCancel()
    {
      m_onCancelFlag = true;
    }

    virtual void DoRun()
    {
      sleep(2);
      CheckCancellation();
    }
  };

}  // namespace

TEST(TaskBase, test_OnCallCancel_when_running)
{
  bool onCancelFlag = false;

  TaskWithOnCallCancel task(onCancelFlag);
  {
    Thread t((ThreadFunc(task)));
    sleep(1);
    task.Cancel();
  }

  EXPECT_TRUE(onCancelFlag);
  EXPECT_EQ(TaskBase::CANCELLED, task.GetState());
}

