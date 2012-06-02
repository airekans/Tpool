#include "WorkerThread.h"
#include "LinearTaskQueue.h"
#include "EndTask.h"
#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

using namespace tpool;
using namespace std;

TEST(WorkerThread, test_Ctor)
{
  {
    TaskQueueBase::Ptr q(new LinearTaskQueue);
    WorkerThread t(q);
    q->Push(TaskBase::Ptr(new EndTask));
  }
}

namespace {
  struct TestTask : public TaskBase {
    int& counter;

    TestTask(int& i)
      : counter(i)
    {}

    virtual void DoRun()
    {
      sleep(2);
      ++counter;
    }
  };
}

TEST(WorkerThread, test_Cancel)
{
  int counter = 0;
  TaskQueueBase::Ptr q(new LinearTaskQueue);
  {
    WorkerThread t(q);
    q->Push(TaskBase::Ptr(new TestTask(counter)));
    q->Push(TaskBase::Ptr(new TestTask(counter)));
    sleep(1);
    t.Cancel();
    // expect WorkerThread run only one task
    ASSERT_EQ(1, counter);
  }
  ASSERT_EQ(1, counter);
  ASSERT_EQ(1, q->Size());
}

TEST(WorkerThread, test_multiple_Cancel)
{
  int counter = 0;
  TaskQueueBase::Ptr q(new LinearTaskQueue);
  {
    WorkerThread t(q);
    q->Push(TaskBase::Ptr(new TestTask(counter)));
    q->Push(TaskBase::Ptr(new TestTask(counter)));
    sleep(1);
    t.Cancel();
    t.Cancel(); // should not block the execution.
    // expect WorkerThread run only one task
  }
  ASSERT_EQ(1, counter);
}

namespace {
  struct CancelFunction {
    WorkerThread& m_workerThread;

    CancelFunction(WorkerThread& workerThread)
      : m_workerThread(workerThread)
    {}

    void operator()()
    {
      m_workerThread.Cancel();
    }
  };
}

TEST(WorkerThread, test_multiple_Cancel_simultunuously)
{
  int counter = 0;
  TaskQueueBase::Ptr q(new LinearTaskQueue);
  {
    WorkerThread t(q);
    q->Push(TaskBase::Ptr(new TestTask(counter)));
    q->Push(TaskBase::Ptr(new TestTask(counter)));
    sleep(1);
    Thread t1((CancelFunction(t)));
    t.Cancel();
    // expect WorkerThread run only one task
  }
  ASSERT_EQ(1, counter);
}

TEST(WorkerThread, test_CancelAsync)
{
  int counter = 0;
  TaskQueueBase::Ptr q(new LinearTaskQueue);
  {
    WorkerThread t(q);
    q->Push(TaskBase::Ptr(new TestTask(counter)));
    q->Push(TaskBase::Ptr(new TestTask(counter)));
    sleep(1);
    t.CancelAsync();
    // expect WorkerThread finish zero task
    ASSERT_EQ(0, counter);
  }
}

namespace {
  struct FinishAction
  {
    int& counter;
    
    FinishAction(int& i)
      : counter(i)
    {}

    void operator()()
    {
      ++counter;
    }
  };
}

TEST(WorkerThread, test_ctor_with_FinishAction)
{
  int counter = 0;

  TaskQueueBase::Ptr q(new LinearTaskQueue);
  {
    WorkerThread t(q, FinishAction(counter));
    q->Push(TaskBase::Ptr(new TestTask(counter)));
    q->Push(TaskBase::Ptr(new EndTask));
  }
  ASSERT_EQ(2, counter);
}

namespace {
  struct LoopTask : TestTask {
    LoopTask(int& i)
      : TestTask(i)
    {}

    virtual void DoRun()
    {
      for (int i = 0; i < 2; ++i)
	{
	  TestTask::DoRun();
	  CheckCancellation();
	}
    }
  };
}

TEST(WorkerThread, test_CancelNow)
{
  int counter = 0;
  int finishFlag = 0;
  TaskQueueBase::Ptr q(new LinearTaskQueue);
  {
    WorkerThread t(q, FinishAction(finishFlag));
    q->Push(TaskBase::Ptr(new LoopTask(counter)));
    q->Push(TaskBase::Ptr(new LoopTask(counter)));
    sleep(1);
    t.CancelNow();
    EXPECT_EQ(1, counter);
    EXPECT_EQ(1, finishFlag);
  }
}

TEST(WorkerThread, test_multiple_CancelNow)
{
  int counter = 0;
  int finishFlag = 0;
  TaskQueueBase::Ptr q(new LinearTaskQueue);
  {
    WorkerThread t(q, FinishAction(finishFlag));
    q->Push(TaskBase::Ptr(new LoopTask(counter)));
    q->Push(TaskBase::Ptr(new LoopTask(counter)));
    sleep(1);
    t.CancelNow();
    t.CancelNow(); // should not block the execution.
    EXPECT_EQ(1, counter);
    EXPECT_EQ(1, finishFlag);
  }
}

namespace {
  struct SleepFinishAction : public FinishAction {
    SleepFinishAction(int& i)
      : FinishAction(i)
    {}
    
    void operator()()
    {
      sleep(1);
      FinishAction::operator()();
    }
  };
}

TEST(WorkerThread, test_FinishAcion_execute_before_Finished)
{
  int counter = 0;
  int finishFlag = 0;
  TaskQueueBase::Ptr q(new LinearTaskQueue);
  {
    WorkerThread t(q, FinishAction(finishFlag));
    q->Push(TaskBase::Ptr(new LoopTask(counter)));
    sleep(1);
    t.CancelNow();
    EXPECT_EQ(1, counter);
    EXPECT_EQ(1, finishFlag);
  }
}
