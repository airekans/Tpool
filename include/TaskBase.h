#ifndef _TPOOL_TASK_BASE_H_
#define _TPOOL_TASK_BASE_H_

#include <boost/shared_ptr.hpp>

namespace tpool {
  class TaskBase {
  public:
    typedef boost::shared_ptr<TaskBase> Ptr;

    ~TaskBase() {}
    
    virtual void Do() = 0;
  };
}

#endif
