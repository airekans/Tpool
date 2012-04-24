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
      ThrowException();
      m_num = 1;
    }

    virtual void ThrowException()
    {
      throw runtime_error("exit thread function");
    }
    
    int& m_num;
  };
}

TEST(BasicThreadTestSuite, test_threadFuncThrowException)
{
  int i = -1;
  ThreadFuncThrowException f(i);

  {
    Thread t(f);
  }

  ASSERT_EQ(0, i);
}

namespace {
  class ThreadFuncThrowUnknownException : public ThreadFuncThrowException {
  public:
    ThreadFuncThrowUnknownException(int& i)
      : ThreadFuncThrowException(i)
    {}

    virtual void ThrowException()
    {
      throw 1;
    }
  };
}

TEST(BasicThreadTestSuite, test_threadFuncThrowUnknownException)
{
  int i = -1;
  ThreadFuncThrowUnknownException f(i);
  {
    Thread t(f);
  }

  ASSERT_EQ(0, i);
}
