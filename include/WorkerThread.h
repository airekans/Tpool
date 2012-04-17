#ifndef _TPOOL_WORKER_THREAD_H_
#define _TPOOL_WORKER_THREAD_H_

#include "TaskQueueBase.h"
#include <boost/shared_ptr.hpp>
#include <memory>

namespace tpool {
  class Thread;

  class WorkerThread {
  public:
    typedef std::auto_ptr<WorkerThread> Ptr;

    WorkerThread(TaskQueueBase::Ptr taskQueue);
    ~WorkerThread();

  private:
    TaskQueueBase::Ptr m_taskQueue;
    std::auto_ptr<Thread> m_thread;
  };
}

#endif
