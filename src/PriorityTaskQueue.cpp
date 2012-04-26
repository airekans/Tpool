#include "PriorityTaskQueue.h"
#include <boost/typeof/typeof.hpp>

using namespace tpool;


PriorityTaskQueue::PriorityTaskQueue(const size_t numOfPriorities)
  : m_tasks(numOfPriorities)
{
}

void PriorityTaskQueue::Push(TaskBase::Ptr task)
{
  
}

TaskBase::Ptr PriorityTaskQueue::Pop()
{
  for (BOOST_AUTO(slotIt, m_tasks.begin());
       slotIt != m_tasks.end();
       ++slotIt)
    {
      if (!(slotIt->empty()))
	{
	  TaskBase::Ptr task = slotIt->front();
	  slotIt->pop();
	  return task;
	}
    }
  // No task, wait for pushing tasks
  return TaskBase::Ptr();
}

size_t PriorityTaskQueue::Size() const
{
  size_t totalSize = 0;
  for (BOOST_AUTO(slotIt, m_tasks.begin());
       slotIt != m_tasks.end();
       ++slotIt)
    {
      totalSize += slotIt->size();
    }
  return totalSize;
}
