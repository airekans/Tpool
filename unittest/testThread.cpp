#include "Thread.h"
#include <iostream>
#include <gtest/gtest.h>
#include <cstdlib>
#include <stdexcept>

using namespace std;
using namespace tpool;

namespace {
  int GLOBAL_COUNTER = 0;

  struct ThreadFunctor {
    void operator()()
    {
      for (int i = 0; i < 20; ++i)
	{
	  ++GLOBAL_COUNTER;
	}
    }
  };

}


TEST(BasicThreadTestSuite, test_lifecycle)
{
  {
    Thread t((ThreadFunctor()));
  }

  ASSERT_EQ(20, GLOBAL_COUNTER);
}

namespace {
  struct ThreadFuncThrowException {
    ThreadFuncThrowException(int& num)
      : m_num(num)
    {
    }

    void operator()()
    {
      m_num = 0;
      throw runtime_error("exit thread function");
      m_num = 1;
    }

    int& m_num;
  };
}

TEST(BasicThreadTestSuite, test_threadFuncThrowException)
{
  int i = 0;
  ThreadFuncThrowException f(i);

  {
    Thread t(f);
  }

  ASSERT_EQ(0, i);
}
