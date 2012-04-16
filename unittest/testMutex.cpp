#include "Thread.h"
#include "Mutex.h"
#include <iostream>
#include <gtest/gtest.h>
#include <cstdlib>
#include <stdexcept>
#include <unistd.h>

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

namespace {
  struct IncThreadFunc {
    sync::Mutex& mutex;
    int& counter;
    
    IncThreadFunc(sync::Mutex& m, int& i)
      : mutex(m), counter(i)
    {
    }
    
    void operator()()
    {
      for (int i = 0; i < 10; ++i)
	{
	  {
	    sync::MutexLocker l(mutex);
	    ++counter;
	  }
	  sleep(1);
	}
    }
  };

  struct WaitThreadFunc {
    int& wakeCount;
    int& counter;
    sync::Mutex& mutex;

    WaitThreadFunc(sync::Mutex& m, int& i, int& wc)
      : mutex(m), counter(i), wakeCount(wc)
    {
      wakeCount = -1;
    }

    struct GreaterThanFunc {
      int& counter;
      GreaterThanFunc(int& i)
	: counter(i)
      {
      }

      bool operator()()
      {
	return counter >= 5;
      }
    };
    
    void operator()()
    {
      sync::MutexWaitWhen(mutex,
			  (GreaterThanFunc(counter)));
      sync::MutexLocker l(mutex);
      wakeCount = counter; // wakeCount should > 0 now
    }
  };
}

TEST(MutexTestSuite, test_MutexWaitWhen)
{
  int counter = 0;
  int wakeCount = -1;
  sync::Mutex m;
  {
    Thread t1((WaitThreadFunc(m, counter, wakeCount)));
    Thread t2((IncThreadFunc(m, counter)));
  }

  ASSERT_GT(wakeCount, 0);
}
