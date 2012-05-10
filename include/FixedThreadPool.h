// -*- mode: c++ -*-
#ifndef _TPOOL_FIXED_THREAD_POOL_H_
#define _TPOOL_FIXED_THREAD_POOL_H_

#include "WorkerThread.h"
#include "LinearTaskQueue.h"
#include "FunctorTask.h"
#include "EndTask.h"
#include "ConditionVariable.h"
#include <vector>
#include <cstdlib> // for size_t
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

namespace tpool {
  template<class TaskQueue = LinearTaskQueue>
  class FixedThreadPool {
  public:
    FixedThreadPool(const size_t threadNum = 10);
    ~FixedThreadPool();
  

    size_t GetThreadNum() const;
    bool AddTask(TaskBase::Ptr task);

    template<typename Func>
    bool AddTask(Func f);

    void Stop();
    void StopAsync();
    void StopNow();
    
  private:
    bool DoAddTask(TaskBase::Ptr task);
    bool IsRequestStop() const;

    TaskQueueBase::Ptr m_taskQueue;
    std::vector<WorkerThread::Ptr> m_threads;
    volatile bool m_isRequestStop;
    mutable sync::MutexConditionVariable m_stopCondition;
  };

  typedef FixedThreadPool<> LFixedThreadPool;

  // Implementation
  template<class TaskQueue>
  FixedThreadPool<TaskQueue>::FixedThreadPool(const size_t threadNum)
    : m_taskQueue(new TaskQueue),
      m_threads(threadNum),
      m_isRequestStop(false)
  {
    BOOST_FOREACH(WorkerThread::Ptr& t, m_threads)
      {
	t.reset(new WorkerThread(m_taskQueue));
      }
  }
  
  template<class TaskQueue>
  FixedThreadPool<TaskQueue>::~FixedThreadPool()
  {
    // keep other thread from pushing more tasks
    StopAsync();
    const size_t threadNum = m_threads.size();
    for (int i = 0; i < threadNum; ++i)
      {
	m_taskQueue->Push(TaskBase::Ptr(new EndTask));
      }
  }

  template<class TaskQueue>
  size_t FixedThreadPool<TaskQueue>::GetThreadNum() const
  {
    return m_threads.size();
  }

  template<class TaskQueue>
  bool FixedThreadPool<TaskQueue>::AddTask(TaskBase::Ptr task)
  {
    return DoAddTask(task);
  }

  template<class TaskQueue>
  template<typename Func>
  bool FixedThreadPool<TaskQueue>::AddTask(Func f)
  {
    return DoAddTask(MakeFunctorTask(f));
  }

  // TODO: finish this function
  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::Stop()
  {
    using boost::bind;

    m_isRequestStop = true;

    // wait until all worker threads stop
    sync::ConditionWaitLocker l(m_stopCondition,
				bind(&FixedThreadPool::IsRequestStop, this));
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::StopAsync()
  {
    m_isRequestStop = true;
  }

  template<class TaskQueue>
  bool FixedThreadPool<TaskQueue>::DoAddTask(TaskBase::Ptr task)
  {
    if (m_isRequestStop)
      {
	return false;
      }
    
    m_taskQueue->Push(task);
    return true;
  }

  template<class TaskQueue>
  bool FixedThreadPool<TaskQueue>::IsRequestStop() const
  {
    return m_isRequestStop;
  }
}


#endif
