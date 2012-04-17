#include "LinearTaskQueue.h"
#include <boost/bind.hpp>
#include <functional>
#include <iostream>

using namespace tpool;
using namespace tpool::sync;
using namespace boost;
using namespace std;

void LinearTaskQueue::Push(TaskBase::Ptr task)
{
  MutexLocker l(m_mutex);
  m_tasks.push(task);
}

TaskBase::Ptr LinearTaskQueue::Pop()
{
  cout << "Pop()" << endl;
  
  // wait until task queue is not empty
  MutexWaitLocker l(m_mutex,
		    bind(not1(mem_fun(&TaskQueueImpl::empty)),
			 &m_tasks));
  cout << "queue not empty" << endl;

  TaskBase::Ptr task = m_tasks.front();
  m_tasks.pop();
  return task;
}

size_t LinearTaskQueue::Size() const
{
  MutexLocker l(m_mutex);
  return m_tasks.size();
}
