// -*- mode: c++ -*-
#ifndef _TPOOL_FUTURE_TASK_H_
#define _TPOOL_FUTURE_TASK_H_

#include "TaskBase.h"

namespace tpool {
  template <typename T>
  class FutureTask : public TaskBase {
  public:
    FutureTask();
    virtual ~FutureTask();

    T GetResult();
  };
}

#endif
