#include "FunctorFutureTask.h"
#include <gtest/gtest.h>
#include <string>

using namespace tpool;
using namespace std;

namespace {
  struct TestFunctor
  {
    string operator()()
    {
      return "test";
    }
  };
}

TEST(FunctorFutureTask, test_Ctor)
{
  FunctorFutureTask<string, TestFunctor> task((TestFunctor()));
}
