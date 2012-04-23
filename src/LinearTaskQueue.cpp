#include "LinearTaskQueue.h"
#include <boost/bind.hpp>
#include <functional>

using namespace tpool;
using namespace tpool::sync;
using namespace boost;


void LinearTaskQueue::Push(TaskBase::Ptr task)
{
  ConditionNotifyAllLocker l(m_mutexCond,
			     bind(&TaskQueueImpl::empty, &m_tasks));
  m_tasks.push(task);
}

TaskBase::Ptr LinearTaskQueue::Pop()
{
  // wait until task queue is not empty
  ConditionWaitLocker l(m_mutexCond,
			bind(&TaskQueueImpl::empty, &m_tasks));

  TaskBase::Ptr task = m_tasks.front();
  m_tasks.pop();
  return task;
}

size_t LinearTaskQueue::Size() const
{
  MutexLocker l(m_mutexCond);
  return m_tasks.size();
}

