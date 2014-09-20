#include "ConditionVariable.h"
#include "Thread.h"
#include <gtest/gtest.h>
#include <unistd.h>
#include <sys/select.h>

using namespace tpool;
using namespace tpool::sync;

namespace {
  int GLOBAL_COUNTER = 0;
  bool WAIT_CONDITION = true;

  Mutex m;
  ConditionVariable cond(m);
  
  struct WaitThreadFunc {
    ConditionVariable& condition;

    WaitThreadFunc(ConditionVariable& c)
      : condition(c)
    {}
    
    struct WaitCondFunc{
      bool operator()()
      {
	return WAIT_CONDITION;
      }
    };
    
    void operator()()
    {
      ConditionWaitLocker l(condition, WaitCondFunc());
      ++GLOBAL_COUNTER;
    }
  };

  void MilliSleep(TimeValue sleep_period)
  {
    struct timeval timeout;
    timeout.tv_sec = sleep_period / 1000;
    timeout.tv_usec = (sleep_period % 1000) * 1000;
    (void) select(0, NULL, NULL, NULL, &timeout);
  }

  struct NotifyFunc {
    TimeValue sleep_time;

    NotifyFunc(TimeValue sleep_t = 1000)
    : sleep_time(sleep_t)
    {}

    bool operator()()
    {
      MilliSleep(sleep_time);
      return true;
    }
  };
}

TEST(ConditionVariableTestSuite, test_wait_and_notify)
{
  WAIT_CONDITION = true;
  {
    Thread t((WaitThreadFunc(cond)));
    {
      ConditionNotifyLocker l(cond, NotifyFunc());
      WAIT_CONDITION = false;
      GLOBAL_COUNTER = 0;
    }
  }
  ASSERT_EQ(1, GLOBAL_COUNTER);
}

TEST(ConditionVariableTestSuite, test_wait_and_notify_all)
{
  WAIT_CONDITION = true;
  {
    Thread t1((WaitThreadFunc(cond)));
    Thread t2((WaitThreadFunc(cond)));
    {
      ConditionNotifyAllLocker l(cond, NotifyFunc());
      WAIT_CONDITION = false;
      GLOBAL_COUNTER = 0;
    }
  }
  ASSERT_EQ(2, GLOBAL_COUNTER);
}

namespace {
  bool TrueFunc()
  {
    return true;
  }
}

TEST(ConditionVariableTestSuite, test_timed_wait_expired)
{
  ConditionWaitLocker l(cond);
  ASSERT_FALSE(l.TimedWait(&TrueFunc, 200));  // sleep 200 ms
}

namespace {
  struct TimedWaitThreadFunc {
    ConditionVariable& condition;
    TimeValue wait_time;
    bool time_wait_result;

    TimedWaitThreadFunc(ConditionVariable& c, TimeValue t)
      : condition(c), wait_time(t), time_wait_result(false)
    {}

    struct WaitCondFunc{
      bool operator()()
      {
	return WAIT_CONDITION;
      }
    };

    void operator()()
    {
      ConditionWaitLocker l(condition);
      time_wait_result = l.TimedWait(WaitCondFunc(), wait_time);
      ++GLOBAL_COUNTER;
    }
  };
}

TEST(ConditionVariableTestSuite, test_notify_timed_wait)
{
  WAIT_CONDITION = true;
  {
    Thread t((TimedWaitThreadFunc(cond, 1000)));
    {
      ConditionNotifyLocker l(cond, NotifyFunc(500));
      WAIT_CONDITION = false;
      GLOBAL_COUNTER = 0;
    }
  }
  ASSERT_EQ(1, GLOBAL_COUNTER);
}

TEST(ConditionVariableTestSuite, test_MutexCondition_wait)
{
  WAIT_CONDITION = true;
  MutexConditionVariable condition;
  {
    Thread t((WaitThreadFunc(condition)));
    {
      ConditionNotifyLocker l(condition, NotifyFunc());
      WAIT_CONDITION = false;
      GLOBAL_COUNTER = 0;
    }
    {
      // ensure condition can be used both as Mutex
      // and ConditionVariable
      MutexLocker l(condition);
    }
  }
  ASSERT_EQ(1, GLOBAL_COUNTER);
}
