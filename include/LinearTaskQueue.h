#ifndef _TPOOL_LINEAR_TASK_QUEUE_H_
#define _TPOOL_LINEAR_TASK_QUEUE_H_

#include "TaskQueueBase.h"
#include "ConditionVariable.h"
#include <queue>

namespace tpool {
  class LinearTaskQueue : public TaskQueueBase {
  public:
    virtual void Push(TaskBase::Ptr task);
    virtual TaskBase::Ptr Pop();
    virtual size_t Size() const;

  private:
    typedef std::queue<TaskBase::Ptr> TaskQueueImpl;
    TaskQueueImpl m_tasks;
    mutable sync::Mutex m_mutex;
  };
}

#endif
