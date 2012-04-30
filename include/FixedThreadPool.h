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
#include <boost/bind/protect.hpp>

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
    void NotifyWhenThreadsStop();

    typedef std::vector<WorkerThread::Ptr> WorkerThreads;
    
    TaskQueueBase::Ptr m_taskQueue;
    size_t m_stoppedThreadNum;
    volatile bool m_isRequestStop;
    mutable sync::MutexConditionVariable m_stopCondition;
    WorkerThreads m_threads;
  };

  typedef FixedThreadPool<> LFixedThreadPool;

  // Implementation
  template<class TaskQueue>
  FixedThreadPool<TaskQueue>::FixedThreadPool(const size_t threadNum)
    : m_taskQueue(new TaskQueue),
      m_threads(threadNum),
      m_stoppedThreadNum(0),
      m_isRequestStop(false)
  {
    using boost::bind;
    using boost::protect;

    BOOST_FOREACH(WorkerThread::Ptr& t, m_threads)
      {
	t.reset(new WorkerThread(m_taskQueue,
				 protect(bind(&FixedThreadPool::
					      NotifyWhenThreadsStop, this))
				 ));
      }
  }
  
  template<class TaskQueue>
  FixedThreadPool<TaskQueue>::~FixedThreadPool()
  {
    // keep other thread from pushing more tasks
    StopAsync();
    
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

    StopAsync();

    // wait until all worker threads stop
    sync::ConditionWaitLocker l(m_stopCondition,
				bind(&FixedThreadPool::IsRequestStop, this));
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::StopAsync()
  {
    m_isRequestStop = true;
    const size_t threadNum = m_threads.size();
    for (size_t i = 0; i < threadNum; ++i)
      {
	m_taskQueue->Push(TaskBase::Ptr(new EndTask));
      }
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

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::NotifyWhenThreadsStop()
  {
    using boost::bind;

    if (++m_stoppedThreadNum >= m_threads.size())
      {
    	sync::ConditionNotifyLocker l(m_stopCondition,
    				      bind(&FixedThreadPool::IsRequestStop,
    					   this));
    	m_isRequestStop = false;
      }
  }


}


#endif
