#ifndef _TPOOL_END_TASK_H_
#define _TPOOL_END_TASK_H_

#include "TaskBase.h"

namespace tpool {
  class EndTask : public TaskBase {
  public:
    virtual void Do();
  };
}

#endif
