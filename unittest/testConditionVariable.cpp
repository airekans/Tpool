#include "ConditionVariable.h"

using namespace tpool;

namespace {
  int GLOBAL_COUNTER = 0;
  bool WAIT_CONDITION = true;

  ConditionVariable cond;
  
  struct WaitThreadFunc {
    struct {
      bool operator()()
      {
	return WAIT_CONDITION;
      }
    } waitCondition;
    
    void operator()()
    {
      sync::ConditionLocker l(cond);
      l.WaitWhen(waitCondition);

      GLOBAL_COUNTER = 1;
    }
  };
}

TEST(ConditionVariableTestSuite, test_signal)
{
  Thread t1();
}
