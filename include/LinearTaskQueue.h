#ifndef _TPOOL_LINEAR_TASK_QUEUE_H_
#define _TPOOL_LINEAR_TASK_QUEUE_H_

#include "TaskQueueBase.h"
#include "BlockingQueue.h"
#include "ConditionVariable.h"
#include <queue>

namespace tpool {

class LinearTaskQueue : public TaskQueueBase {
public:
    virtual void Push(TaskBase::Ptr task);
    virtual TaskBase::Ptr Pop();
    virtual bool NonblockingPop(TaskBase::Ptr& task);
    virtual size_t Size() const;

private:
    BlockingQueue<TaskBase::Ptr> m_tasks;
};

}  // namespace tpool

#endif
