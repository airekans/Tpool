#ifndef _TPOOL_LINEAR_TASK_QUEUE_H_
#define _TPOOL_LINEAR_TASK_QUEUE_H_

#include "TaskQueueBase.h"

namespace tpool {
  class LinearTaskQueue : public TaskQueueBase {
  public:
    virtual void Push(boost::shared_ptr<TaskBase> task);
    virtual boost::shared_ptr<TaskBase> Pop();
  };
}

#endif
