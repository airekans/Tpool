#include "LinearTaskQueue.h"

using namespace tpool;
using namespace tpool::sync;
using namespace boost;

void LinearTaskQueue::Push(TaskBase::Ptr task)
{
  MutexLocker l(m_mutex);
  m_tasks.push(task);
}

TaskBase::Ptr LinearTaskQueue::Pop()
{
  MutexLocker l(m_mutex);
  TaskBase::Ptr task = m_tasks.front();
  m_tasks.pop();
  return task;
}

