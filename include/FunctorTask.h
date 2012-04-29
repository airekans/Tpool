#ifndef _TPOOL_FUNCTOR_TASK_H_
#define _TPOOL_FUNCTOR_TASK_H_

#include "TaskBase.h"

namespace tpool {
  template<typename Func>
  class FunctorTask : public TaskBase {
  public:
    FunctorTask(Func f);

    virtual void Do();
    
  private:
    Func m_functor;
  };

  /// helper function
  template<typename Func>
  TaskBase::Ptr MakeFunctorTask(Func f)
  {
    return TaskBase::Ptr(new FunctorTask<Func>(f));
  }

  // Implementation
  template<typename Func>
  FunctorTask<Func>::FunctorTask(Func f)
    : m_functor(f)
  {}

  template<typename Func>
  void FunctorTask<Func>::Do()
  {
    m_functor();
  }
}

#endif
