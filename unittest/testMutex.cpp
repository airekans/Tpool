#include "Thread.h"
#include "Mutex.h"
#include <iostream>
#include <gtest/gtest.h>
#include <cstdlib>
#include <stdexcept>

using namespace std;
using namespace tpool;

namespace {
  int GLOBAL_COUNTER = 0;
  sync::Mutex mutex;
  
  struct ThreadFunctor {
    ThreadFunctor(sync::Mutex& mutex)
      : m(mutex)
    {
      
    }
    
    void operator()()
    {
      for (int i = 0; i < 20; ++i)
	{
	  sync::MutexLocker l(m);
	  ++GLOBAL_COUNTER;
	}
    }

    sync::Mutex& m;
  };

}


TEST(MutexTestSuite, test_MutexLocker)
{
  {
    Thread t1((ThreadFunctor(mutex)));
    Thread t2((ThreadFunctor(mutex)));

    for (int i = 0; i < 20; ++i)
      {
	sync::MutexLocker l(mutex);
	  ++GLOBAL_COUNTER;
      }
  }

  ASSERT_EQ(60, GLOBAL_COUNTER);
}
