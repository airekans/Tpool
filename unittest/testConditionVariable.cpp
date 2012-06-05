#include "ConditionVariable.h"
#include "Thread.h"
#include <gtest/gtest.h>
#include <unistd.h>

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

  struct NotifyFunc {
    bool operator()()
    {
      sleep(1);
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
      int tmp = 1;
    }
  }
  ASSERT_EQ(1, GLOBAL_COUNTER);
}
