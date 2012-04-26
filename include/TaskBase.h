// -*- mode: c++ -*-
#ifndef _TPOOL_TASK_BASE_H_
#define _TPOOL_TASK_BASE_H_

#include <boost/shared_ptr.hpp>

namespace tpool {
  class TaskBase {
  public:
    typedef boost::shared_ptr<TaskBase> Ptr;

    enum State {
      INIT,
      RUNNING,
      FINISHED,
      CANCELED,
    };

    TaskBase();
    ~TaskBase() {}
    
    void Run();
    void Cancel();
    State GetState() const;

  protected:
    void CheckCancellation() const;

  private:
    virtual void DoRun() = 0;

    volatile State m_state;
    volatile bool m_isRequestCancel;
  };
}

#endif
