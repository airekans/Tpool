// -*- mode: c++ -*-
#ifndef _TPOOL_TASK_BASE_H_
#define _TPOOL_TASK_BASE_H_

#include "ConditionVariable.h"
#include "Atomic.h"
#include <boost/shared_ptr.hpp>

namespace tpool {
  class TaskBase {
  public:
    typedef boost::shared_ptr<TaskBase> Ptr;

    enum State {
      INIT,
      RUNNING,
      FINISHED,
      CANCELLED,
    };

    TaskBase();
    virtual ~TaskBase() {}
    
    void Run();
    void Cancel();
    void CancelAsync();
    
    State GetState() const;
    bool IsRunning() const;
    bool IsFinished() const;
    bool IsCancelled() const;
    bool IsRequestCancel() const;
    bool IsStopped() const;

  protected:
    void CheckCancellation() const;
    virtual void OnCancel();

  private:
    virtual void DoRun() = 0;
    void SetState(const State state);
    bool IsStopState() const;

    State m_state;
    mutable sync::Mutex m_stateGuard;
    Atomic<bool> m_isRequestCancel;
    sync::ConditionVariable m_cancelCondition;
  };
}

#endif
