#include "Thread.h"
#include "Mutex.h"
#include <iostream>
#include <gtest/gtest.h>
#include <cstdlib>
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace tpool;
using namespace tpool::sync;

namespace {
  int GLOBAL_COUNTER = 0;
  Mutex mutex;
  
  struct ThreadFunctor {
    ThreadFunctor(Mutex& mutex)
      : m(mutex)
    {
      
    }
    
    void operator()()
    {
      for (int i = 0; i < 20; ++i)
	{
	  MutexLocker l(m);
	  ++GLOBAL_COUNTER;
	}
    }

    Mutex& m;
  };

}


TEST(MutexTestSuite, test_MutexLocker)
{
  {
    Thread t1((ThreadFunctor(mutex)));
    Thread t2((ThreadFunctor(mutex)));

    for (int i = 0; i < 20; ++i)
      {
	MutexLocker l(mutex);
	  ++GLOBAL_COUNTER;
      }
  }

  ASSERT_EQ(60, GLOBAL_COUNTER);
}

namespace {
  struct IncThreadFunc {
    Mutex& mutex;
    int& counter;
    
    IncThreadFunc(Mutex& m, int& i)
      : mutex(m), counter(i)
    {
    }
    
    void operator()()
    {
      for (int i = 0; i < 2; ++i)
	{
	  {
	    MutexLocker l(mutex);
	    ++counter;
	  }
	  sleep(1);
	}
    }
  };

  struct WaitThreadFunc {
    int& wakeCount;
    int& counter;
    Mutex& mutex;

    WaitThreadFunc(Mutex& m, int& i, int& wc)
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
	return counter < 1;
      }
    };
    
    void operator()()
    {
      MutexWaitLocker(mutex,
			  (GreaterThanFunc(counter)));
      cout << "wait end: counter " << counter << endl;
      wakeCount = counter; // wakeCount should > 0 now
    }
  };
}

TEST(MutexTestSuite, test_MutexWaitLocker)
{
  int counter = 0;
  int wakeCount = -1;
  Mutex m;
  {
    Thread t1((WaitThreadFunc(m, counter, wakeCount)));
    Thread t2((IncThreadFunc(m, counter)));
  }

  ASSERT_GT(wakeCount, 0);
}
