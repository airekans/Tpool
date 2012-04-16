#ifndef _TPOOL_TASK_QUEUE_BASE_H_
#define _TPOOL_TASK_QUEUE_BASE_H_

#include "TaskBase.h"
#include <boost/shared_ptr.hpp>
#include <cstdlib> // for size_t

namespace tpool {
  class TaskQueueBase {
  public:
    typedef boost::shared_ptr<TaskQueueBase> Ptr;
    
    virtual ~TaskQueueBase() {}
    
    virtual void Push(TaskBase::Ptr task) = 0;
    virtual TaskBase::Ptr Pop() = 0;
    virtual size_t Size() const = 0;
  };
}

#endif
