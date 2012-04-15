#ifndef _TPOOL_LINEAR_TASK_QUEUE_H_
#define _TPOOL_LINEAR_TASK_QUEUE_H_

#include "TaskQueueBase.h"
#include "Mutex.h"
#include <queue>

namespace tpool {
  class LinearTaskQueue : public TaskQueueBase {
  public:
    virtual void Push(TaskBase::Ptr task);
    virtual TaskBase::Ptr Pop();

  private:
    std::queue<TaskBase::Ptr> m_tasks;
    sync::Mutex m_mutex;
  };
}

#endif
