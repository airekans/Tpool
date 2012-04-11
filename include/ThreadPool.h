#ifndef _TPOOL_THREAD_POOL_H_
#define _TPOOL_THREAD_POOL_H_

#include "WorkerThread.h"
#include <vector>

namespace tpool {
  template<class TaskQueue>
    class ThreadPool {
  public:

  private:
    TaskQueue m_taskQueue;
    std::vector<WorkerThread> m_threads;
  };
}


#endif
