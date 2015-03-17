#ifndef _TPOOL_PRIORITY_TASK_QUEUE_H_
#define _TPOOL_PRIORITY_TASK_QUEUE_H_

#include "TaskQueueBase.h"
#include "TaskBase.h"
#include <vector>
#include <queue>

namespace tpool {
  class PriorityTaskQueue : TaskQueueBase {
  public:
    PriorityTaskQueue(const size_t numOfPriorities = 5);

    virtual void Push(TaskBase::Ptr task);
    virtual TaskBase::Ptr Pop();
    virtual bool NonblockingPop(TaskBase::Ptr& task) { return false; }
    virtual size_t Size() const;

  private:
    typedef std::queue<TaskBase::Ptr> PrioritySlot;
    typedef std::vector<PrioritySlot> TaskQueueImpl;
    TaskQueueImpl m_tasks;
  };
}

#endif
