#include "FutureTask.h"
#include "Thread.h"
#include "Atomic.h"

#include <boost/scoped_ptr.hpp>

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

  ::boost::scoped_ptr<int> result(task.GetResult());
  ASSERT_NE((int *)NULL, result.get());
  ASSERT_EQ(1, *result);
}

namespace {
  struct TestIncFutureTask : public FutureTask<int>
  {
    Atomic<int>& data;
    
    TestIncFutureTask(Atomic<int>& i)
      : data(i)
    {}

    virtual int Call()
    {
      sleep(2);
      return ++data;
    }
  };

  struct TestIncFunctor
  {
    TestIncFutureTask& task;

    TestIncFunctor(TestIncFutureTask& t)
      : task(t)
    {}
    
    void operator()()
    {
      task.Run();
    }
  };
}

TEST(FutureTask, test_GetResultBeforeCallDone)
{
  Atomic<int> data(0);
  TestIncFutureTask task(data);

  {
    Thread t((TestIncFunctor(task)));
    sleep(1);
    EXPECT_EQ(0, data);
  }

  EXPECT_EQ(1, data);
  EXPECT_EQ(1, task.GetResult());
}

TEST(FutureTask, test_TryGetResultBeforeCall)
{
  TestFutureTask task;

  EXPECT_THROW(task.TryGetResult(), ResultNotReadyException);
}

TEST(FutureTask, test_TryGetResultAfterCallDone)
{
  TestFutureTask task;

  task.Run();

  EXPECT_NO_THROW(task.TryGetResult());
  EXPECT_EQ(1, task.TryGetResult());
}
