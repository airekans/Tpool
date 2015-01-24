#include "LinearTaskQueue.h"
#include <boost/bind.hpp>
#include <functional>

using namespace tpool;


void LinearTaskQueue::Push(TaskBase::Ptr task)
{
    m_tasks.Push(task);
}

TaskBase::Ptr LinearTaskQueue::Pop()
{
    return m_tasks.Pop();
}

size_t LinearTaskQueue::Size() const
{
    return m_tasks.Size();
}

