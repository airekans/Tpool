// -*- mode: c++ -*-
#ifndef _TPOOL_WORKER_THREAD_H_
#define _TPOOL_WORKER_THREAD_H_

#include "TaskQueueBase.h"
#include "ConditionVariable.h"
#include <boost/shared_ptr.hpp>
#include <memory>

namespace tpool {
  class Thread;

  class WorkerThread {
  public:
    typedef boost::shared_ptr<WorkerThread> Ptr;

    WorkerThread(TaskQueueBase::Ptr taskQueue);
    ~WorkerThread();

    void Cancel();
    void CancelAsync();

  private:
    bool IsRequestCancel() const;
    void CheckCancellation() const;
    void ThreadFunction();
    
    TaskQueueBase::Ptr m_taskQueue;
    volatile bool m_isRequestCancel;
    mutable sync::MutexConditionVariable m_cancelCondition;
    std::auto_ptr<Thread> m_thread; // Thread must be the last variable
  };
}

#endif
