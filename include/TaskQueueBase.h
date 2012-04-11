#ifndef _TPOOL_TASK_QUEUE_BASE_H_
#define _TPOOL_TASK_QUEUE_BASE_H_

#include "TaskBase.h"
#include <boost/shared_ptr.hpp>

namespace tpool {
  class TaskQueueBase {
  public:
    virtual void Push(boost::shared_ptr<TaskBase> task) = 0;
    virtual boost::shared_ptr<TaskBase> Pop() = 0;
  };
}

#endif
