#include "FunctorTask.h"
#include <gtest/gtest.h>
#include <boost/bind.hpp>

using namespace tpool;
using namespace boost;

namespace {
  int GLOBAL_COUNTER = 0;
  void Increment()
  {
    GLOBAL_COUNTER = 1;
  }
}

TEST(FunctorTask, construct_for_function_pointer)
{
  typedef void (*func)();
  
  GLOBAL_COUNTER = 0;
  FunctorTask<func> functorTask(&Increment);
  functorTask.DoRun();

  ASSERT_EQ(1, GLOBAL_COUNTER);
}

namespace {
  int GLOBAL_COUNTER_2 = 0;
  struct IncrementFunc {
    void operator()()
    {
      GLOBAL_COUNTER_2 = 1;
    }
  };
}

TEST(FunctorTask, construct_for_functor)
{
  GLOBAL_COUNTER_2 = 0;
  FunctorTask<IncrementFunc> functorTask((IncrementFunc()));
  functorTask.DoRun();

  ASSERT_EQ(1, GLOBAL_COUNTER_2);
}

namespace {
  int GLOBAL_COUNTER_3 = 0;
  void IncrementWith(int* i)
  {
    *i = 1;
  }
}

TEST(FunctorTask, construct_for_boost_bind)
{
  GLOBAL_COUNTER_3 = 0;
  TaskBase::Ptr task = MakeFunctorTask(bind(&IncrementWith, &GLOBAL_COUNTER_3));
  task->Run();

  ASSERT_EQ(1, GLOBAL_COUNTER_3);
}
