// -*- mode: c++ -*-
#ifndef _TPOOL_FIXED_THREAD_POOL_H_
#define _TPOOL_FIXED_THREAD_POOL_H_

#include "WorkerThread.h"
#include "LinearTaskQueue.h"
#include "FunctorTask.h"
#include "EndTask.h"
#include <vector>
#include <cstdlib> // for size_t
#include <boost/foreach.hpp>

namespace tpool {
  template<class TaskQueue = LinearTaskQueue>
    class FixedThreadPool {
  public:
    FixedThreadPool(const size_t threadNum = 10)
      : m_taskQueue(new TaskQueue),
	m_threads(threadNum)
    {
      BOOST_FOREACH(WorkerThread::Ptr& t, m_threads)
	{
	  t.reset(new WorkerThread(m_taskQueue));
	}
    }

  ~FixedThreadPool();
  

    size_t GetThreadNum() const
    {
      return m_threads.size();
    }

    void AddTask(TaskBase::Ptr task)
    {
      m_taskQueue->Push(task);
    }

    template<typename Func>
    void AddTask(Func f);

  private:
    TaskQueueBase::Ptr m_taskQueue;
    std::vector<WorkerThread::Ptr> m_threads;
  };

  typedef FixedThreadPool<> LFixedThreadPool;

  // Implementation
  template<class TaskQueue>
    FixedThreadPool<TaskQueue>::~FixedThreadPool()
    {
      const size_t threadNum = m_threads.size();
      for (int i = 0; i < threadNum; ++i)
	{
	  m_taskQueue->Push(TaskBase::Ptr(new EndTask));
	}
    }

  template<class TaskQueue>
  template<typename Func>
  void FixedThreadPool<TaskQueue>::AddTask(Func f)
  {
    m_taskQueue->Push(MakeFunctorTask(f));
  }
}


#endif
