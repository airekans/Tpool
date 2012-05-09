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
