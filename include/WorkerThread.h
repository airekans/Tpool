#ifndef _TPOOL_WORKER_THREAD_H_
#define _TPOOL_WORKER_THREAD_H_

#include <boost/shared_ptr.hpp>
#include <memory>

namespace tpool {
  class TaskQueueBase;
  class Thread;

  class WorkerThread {
  public:
    WorkerThread(boost::shared_ptr<TaskQueueBase> taskQueue);

  private:
    boost::shared_ptr<TaskQueueBase> m_taskQueue;
    std::auto_ptr<Thread> m_thread;
  };
}

#endif
