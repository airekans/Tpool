// -*- mode: c++ -*-
#ifndef _TPOOL_FIXED_THREAD_POOL_H_
#define _TPOOL_FIXED_THREAD_POOL_H_

#include "WorkerThread.h"
#include "LinearTaskQueue.h"
#include "FunctorTask.h"
#include "EndTask.h"
#include "ConditionVariable.h"
#include "Atomic.h"
#include <vector>
#include <cstdlib> // for size_t
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <functional>

namespace tpool {
  
  template<class TaskQueue = LinearTaskQueue>
  class FixedThreadPool {
  private:
    enum State {
      INIT,
      RUNNING,
      FINISHED,
    };

  public:
    FixedThreadPool(const size_t threadNum = 10);
    ~FixedThreadPool();
  

    size_t GetThreadNum() const;
    TaskBase::Ptr AddTask(TaskBase::Ptr task);

    template<typename Func>
    TaskBase::Ptr AddTask(Func f);

    void Stop();
    void StopAsync();
    void StopNow();
    
  private:
    TaskBase::Ptr DoAddTask(TaskBase::Ptr task);
    void NotifyWhenThreadsStop();
    bool IsFinished() const;
    bool DoIsFinished() const;
    void SetState(const State state);
    void DoSetState(const State state);

    typedef std::vector<WorkerThread::Ptr> WorkerThreads;
    
    TaskQueueBase::Ptr m_taskQueue;
    sync::Mutex m_stoppedThreadNumGuard;
    size_t m_stoppedThreadNum;
    State m_state;
    mutable sync::MutexConditionVariable m_stateGuard;
    Atomic<bool> m_isRequestStop;
    WorkerThreads m_threads;
  };

  typedef FixedThreadPool<> LFixedThreadPool;

  // Implementation
  template<class TaskQueue>
  FixedThreadPool<TaskQueue>::FixedThreadPool(const size_t threadNum)
    : m_taskQueue(new TaskQueue),
      m_threads(threadNum),
      m_stoppedThreadNum(0),
      m_state(INIT),
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

    SetState(RUNNING);
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
  TaskBase::Ptr FixedThreadPool<TaskQueue>::AddTask(TaskBase::Ptr task)
  {
    return DoAddTask(task);
  }

  template<class TaskQueue>
  template<typename Func>
  TaskBase::Ptr FixedThreadPool<TaskQueue>::AddTask(Func f)
  {
    return DoAddTask(MakeFunctorTask(f));
  }

  // TODO: finish this function
  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::Stop()
  {
    using boost::bind;
    using std::mem_fun;
    using std::not1;

    if (!m_isRequestStop)
      {
	StopAsync();

	// wait until all worker threads stop
	sync::ConditionWaitLocker l(m_stateGuard,
				    bind(not1(mem_fun(&FixedThreadPool::
						      DoIsFinished)),
					 this));
      }
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::StopAsync()
  {
    if (!m_isRequestStop)
      {
	m_isRequestStop = true;
	const size_t threadNum = m_threads.size();
	for (size_t i = 0; i < threadNum; ++i)
	  {
	    m_taskQueue->Push(TaskBase::Ptr(new EndTask));
	  }
      }
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::StopNow()
  {
    StopAsync();

    BOOST_FOREACH(WorkerThread::Ptr& t, m_threads)
      {
	t->CancelAsync();
      }
    
    BOOST_FOREACH(WorkerThread::Ptr& t, m_threads)
      {
	t->CancelNow();
      }
  }

  template<class TaskQueue>
  TaskBase::Ptr FixedThreadPool<TaskQueue>::DoAddTask(TaskBase::Ptr task)
  {
    if (m_isRequestStop)
      {
	return TaskBase::Ptr();
      }
    
    m_taskQueue->Push(task);
    return task;
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::NotifyWhenThreadsStop()
  {
    using boost::bind;

    {
      sync::MutexLocker l(m_stoppedThreadNumGuard);
      ++m_stoppedThreadNum;
    }

    if (m_stoppedThreadNum >= m_threads.size())
      {
    	sync::ConditionNotifyLocker l(m_stateGuard,
    				      bind(&Atomic<bool>::GetData,
    					   &m_isRequestStop));
	DoSetState(FINISHED);
      }
  }

  template<class TaskQueue>
  bool FixedThreadPool<TaskQueue>::IsFinished() const
  {
    sync::MutexLocker l(m_stateGuard);
    return DoIsFinished();
  }

  template<class TaskQueue>
  bool FixedThreadPool<TaskQueue>::DoIsFinished() const
  {
    return m_state == FINISHED;
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::SetState(const State state)
  {
    sync::MutexLocker l(m_stateGuard);
    DoSetState(state);
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::DoSetState(const State state)
  {
    m_state = state;
  }
}


#endif
