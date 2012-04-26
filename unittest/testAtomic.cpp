#include "Atomic.h"
#include "Thread.h"
#include <gtest/gtest.h>
#include <string>

using namespace std;
using namespace tpool;

TEST(Atomic, test_Ctor)
{
  Atomic<int> i(1);

  ASSERT_EQ(1, (int)i);
}

TEST(Atomic, test_copy_ctor)
{
  Atomic<int> i1(1);
  Atomic<int> i2(i1);

  ASSERT_EQ(1, (int)i1);
  ASSERT_EQ(1, (int)i2);
}

TEST(Atomic, test_assignment_T)
{
  Atomic<int> i(0);
  i = 1;

  ASSERT_EQ(1, (int)i);
}

TEST(Atomic, test_assignment_Atomic)
{
  Atomic<int> i1(0);
  Atomic<int> i2(1);
  i1 = i2;

  ASSERT_EQ(1, i1);
  ASSERT_EQ(1, i2);
}

TEST(Atomic, test_self_assignment)
{
  Atomic<string> s("abc");
  s = s;

  ASSERT_EQ("abc", (string)s);
}

namespace {
  struct IncCounter {
    int counter;

    IncCounter() : counter(0) {}

    IncCounter& operator=(const IncCounter& i)
    {
      if (this != &i)
	{
	  ++counter;
	}
      return *this;
    }
  };
  
  struct ThreadFunc {
    Atomic<IncCounter>& m_data;

    ThreadFunc(Atomic<IncCounter>& atom)
      : m_data(atom)
    {}

    void operator()()
    {
      m_data = IncCounter();
    }
  };
}

TEST(Atomic, test_concurrent_set)
{
  Atomic<IncCounter> i((IncCounter()));
  {
    Thread t((ThreadFunc(i)));
    i = IncCounter();
  }

  ASSERT_EQ(2, ((IncCounter)i).counter);
}
