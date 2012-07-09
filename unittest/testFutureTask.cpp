#include "FutureTask.h"
#include <gtest/gtest.h>

using namespace tpool;

namespace {
  struct TestFutureTask: public FutureTask<int>
  {
    virtual ReturnType Call()
    {
      return 1;
    }
  };
}

TEST(FutureTask, test_Ctor)
{
  TestFutureTask task;
}

namespace {
  struct IntFutureTask : public FutureTask<int*>
  {
    virtual ReturnType Call()
    {
      return new int(1);
    }
  };
}

TEST(FutureTask, test_Call)
{
  IntFutureTask task;

  task.Run();

  int *result = task.GetResult();
  ASSERT_NE((int *)NULL, result);
  ASSERT_EQ(1, *result);
}

TEST(FutureTask, test_TryGetResult)
{
  TestFutureTask task;

  EXPECT_THROW(task.TryGetResult(), ResultNotReadyException);
}
