#include "ConditionVariable.h"
#include "Thread.h"
#include <gtest/gtest.h>
#include <unistd.h>

using namespace tpool;

namespace {
  int GLOBAL_COUNTER = 0;
  bool WAIT_CONDITION = true;

  sync::Mutex m;
  sync::ConditionVariable cond(m);
  
  struct WaitThreadFunc {
    sync::ConditionVariable& condition;

    WaitThreadFunc(sync::ConditionVariable& c)
      : condition(c)
    {}
    
    struct {
      bool operator()()
      {
	return WAIT_CONDITION;
      }
    } waitCondition;
    
    void operator()()
    {
      sync::ConditionWaitLocker(condition, waitCondition);
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
      sync::ConditionNotifyLocker l(cond, NotifyFunc());
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
      sync::ConditionNotifyAllLocker l(cond, NotifyFunc());
      WAIT_CONDITION = false;
      GLOBAL_COUNTER = 0;
    }
  }
  ASSERT_EQ(2, GLOBAL_COUNTER);
}

TEST(ConditionVariableTestSuite, test_MutexCondition_wait)
{
  WAIT_CONDITION = true;
  sync::MutexConditionVariable condition;
  {
    Thread t((WaitThreadFunc(condition)));
    {
      sync::ConditionNotifyLocker l(condition, NotifyFunc());
      WAIT_CONDITION = false;
      GLOBAL_COUNTER = 0;
    }
    {
      // ensure condition can be used both as Mutex
      // and ConditionVariable
      sync::MutexLocker l(condition);
      int tmp = 1;
    }
  }
  ASSERT_EQ(1, GLOBAL_COUNTER);
}
