#ifndef _TPOOL_THREAD_POOL_H_
#define _TPOOL_THREAD_POOL_H_

#include "WorkerThread.h"
#include "LinearTaskQueue.h"
#include "EndTask.h"
#include <vector>
#include <cstdlib> // for size_t
#include <boost/foreach.hpp>

namespace tpool {
  template<class TaskQueue = LinearTaskQueue>
    class ThreadPool {
  public:
    ThreadPool(const size_t threadNum = 10)
      : m_taskQueue(new TaskQueue),
	m_threads(threadNum)
    {
      BOOST_FOREACH(WorkerThread::Ptr& t, m_threads)
	{
	  t.reset(new WorkerThread(m_taskQueue));
	}
    }

  ~ThreadPool();
  

    size_t GetThreadNum() const
    {
      return m_threads.size();
    }

    void AddTask(TaskBase::Ptr task)
    {
      m_taskQueue->Push(task);
    }

  private:
    TaskQueueBase::Ptr m_taskQueue;
    std::vector<WorkerThread::Ptr> m_threads;
  };

  typedef ThreadPool<> LThreadPool;

  // Implementation
  template<class TaskQueue>
    ThreadPool<TaskQueue>::~ThreadPool()
    {
      const size_t threadNum = m_threads.size();
      for (int i = 0; i < threadNum; ++i)
	{
	  m_taskQueue->Push(TaskBase::Ptr(new EndTask));
	}
    }
}


#endif
