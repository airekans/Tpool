#ifndef _TPOOL_WORKER_THREAD_H_
#define _TPOOL_WORKER_THREAD_H_

#include "Thread.h"

namespace tpool {
  class WorkerThread : public Thread {
  public:

  protected:
    virtual void Entry();
  };
}

#endif
