#include "FutureTask.h"
#include <gtest/gtest.h>

using namespace tpool;

namespace {
  struct TestFutureTask: public FutureTask<int>
  {
    virtual int Call()
    {
      return 1;
    }
  };
}

TEST(FutureTask, test_Ctor)
{
  TestFutureTask task;
}
