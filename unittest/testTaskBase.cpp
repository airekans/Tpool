#include "TaskBase.h"
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
}

TEST(TaskBase, test_GetState)
{
  FakeTask task;

  EXPECT_EQ(TaskBase::INIT, task.GetState());

  
}
