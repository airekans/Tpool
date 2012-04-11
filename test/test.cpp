#include "Thread.h"
#include <iostream>
#include <gtest/gtest.h>

using namespace tpool;
using namespace std;

namespace {
  int GLOBAL_COUNTER = 0;
}

class TestThread : public Thread {
public:
  virtual void Entry()
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
    TestThread t;

    t.Run();

    for (int i = 0; i < 20; ++i)
      {
	++GLOBAL_COUNTER;
      }
  }
  ASSERT_EQ(40, GLOBAL_COUNTER);
}

