#include "Thread.h"
#include <iostream>
#include <gtest/gtest.h>
#include <cstdlib>

using namespace std;
using namespace tpool;

namespace {
  int GLOBAL_COUNTER = 0;
}

struct ThreadFunctor {
  void operator()()
  {
    for (int i = 0; i < 20; ++i)
      {
	++GLOBAL_COUNTER;
      }
  }
};

TEST(BasicThreadTestSuite, test_lifecycle)
{
  {
    Thread t((ThreadFunctor()));
  }

  ASSERT_EQ(20, GLOBAL_COUNTER);
}

