#ifndef _TPOOL_TASK_BASE_H_
#define _TPOOL_TASK_BASE_H_

namespace tpool {
  class TaskBase {
  public:
    virtual void Do() = 0;
  };
}

#endif
