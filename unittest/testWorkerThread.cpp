#include "WorkerThread.h"
#include "LinearTaskQueue.h"
#include "TaskBase.h"
#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>

using namespace tpool;
using namespace std;

namespace {
  struct EndTask : public TaskBase {
    virtual void DoRun()
    {
      throw runtime_error("EndTask");
    }
  };
}

TEST(WorkerThreadTestSuite, test_Ctor)
{
  {
    TaskQueueBase::Ptr q(new LinearTaskQueue);
    WorkerThread t(q);
    q->Push(TaskBase::Ptr(new EndTask));
  }
}
