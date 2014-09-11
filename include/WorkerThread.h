// -*- mode: c++ -*-
#ifndef _TPOOL_WORKER_THREAD_H_
#define _TPOOL_WORKER_THREAD_H_

#include "TaskQueueBase.h"
#include "ConditionVariable.h"
#include "Thread.h"
#include "Atomic.h"
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <memory>
#include <exception>

namespace tpool {
  class WorkerThread : public boost::noncopyable {
  private:
    enum State {
      INIT,
      RUNNING,
      FINISHED,
    };
    
  public:
    typedef boost::shared_ptr<WorkerThread> Ptr;

    WorkerThread(TaskQueueBase::Ptr taskQueue);
    template <typename FinishAction>
    WorkerThread(TaskQueueBase::Ptr taskQueue, FinishAction action);
    ~WorkerThread();

    void Cancel();
    void CancelAsync();
    void CancelNow();

  private:
    template <typename FinishAction>
    void Init(TaskQueueBase::Ptr taskQueue, FinishAction action);
    void ProcessError(const std::exception& e);
    void CheckCancellation() const;
    void WorkFunction();
    template <typename FinishAction>
    void ThreadFunction(FinishAction action);
    bool IsFinished() const;
    bool DoIsFinished() const;
    void SetState(const State state);
    void DoSetState(const State state);
    void GetTaskFromTaskQueue();
    void NotifyFinished();
    
    TaskQueueBase::Ptr m_taskQueue;
    TaskBase::Ptr m_runningTask;
    mutable sync::Mutex m_runningTaskGuard;
    State m_state;
    mutable sync::MutexConditionVariable m_stateGuard;
    Atomic<bool> m_isRequestCancel;
    std::auto_ptr<Thread> m_thread; // Thread must be the last variable
  };


  // Implementation
  template <typename FinishAction>
  void WorkerThread::Init(TaskQueueBase::Ptr taskQueue, FinishAction action)
  {
    using boost::bind;
    
    m_taskQueue = taskQueue;

    // ensure that the thread is created successfully.
    while (true)
      {
	try
	  {
	    // check for the creation exception
	    m_thread.reset(new Thread(bind(&WorkerThread::
					   ThreadFunction<FinishAction>,
					   this, action)));
	    break;
	  }
	catch (const std::exception& e)
	  {
	    ProcessError(e);
	  }    
      }
  }

  template <typename FinishAction>
  WorkerThread::WorkerThread(TaskQueueBase::Ptr taskQueue,
			     FinishAction action)
    : m_state(INIT),
      m_isRequestCancel(false)
  {
    Init(taskQueue, action);
  }
  
  template <typename FinishAction>
  void WorkerThread::ThreadFunction(FinishAction action)
  {
    WorkFunction();
    action(); // WorkerThread finished.
    NotifyFinished();
  }
}

#endif
