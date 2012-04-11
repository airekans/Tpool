#ifndef _TPOOL_WORKER_THREAD_H_
#define _TPOOL_WORKER_THREAD_H_

#include "Thread.h"
#include <boost/shared_ptr.hpp>

namespace tpool {
  class TaskQueueBase;

  class WorkerThread : public Thread {
  public:
    WorkerThread(boost::shared_ptr<TaskQueueBase> taskQueue);

  protected:
    virtual void Entry();

  private:
    boost::shared_ptr<TaskQueueBase> m_taskQueue;
  };
}

#endif
