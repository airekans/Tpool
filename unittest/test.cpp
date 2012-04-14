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
  ThreadFunctor()
  {
    cout << "in functor" << endl;
  }
  
  void operator()()
  {
    for (int i = 0; i < 20; ++i)
      {
	++GLOBAL_COUNTER;
	cout << "thread " << i << endl;
      }
  }
};

TEST(BasicThreadTestSuite, test_lifecycle)
{
  {
    cout << "creating thread" << endl;
    ThreadFunctor f;
    Thread t(f);
    
    for (int i = 0; i < 20; ++i)
      {
	++GLOBAL_COUNTER;
	cout << "main " << i << endl;
      }

  }

  sleep(1);
  // ASSERT_EQ(40, GLOBAL_COUNTER);
}

