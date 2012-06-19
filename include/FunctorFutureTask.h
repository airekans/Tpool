// -*- mode: c++ -*-
#ifndef _FUNCTOR_FUTURE_TASK_H_
#define _FUNCTOR_FUTURE_TASK_H_

#include "FutureTask.h"

namespace tpool {
  template <typename T, typename Func>
  class FunctorFutureTask : public FutureTask<T> {
  public:
    FunctorFutureTask(Func f);
    
    virtual T Call();

  private:
    Func m_functor;
  };

  // Helper function
  template <typename T, typename Func>
  FunctorFutureTask<T, Func> MakeFunctorFutureTask(Func f)
  {
    return MakeFunctorFutureTask<T, Func>(f);
  }
  

  // Implementation
  template <typename T, typename Func>
  FunctorFutureTask<T, Func>::FunctorFutureTask(Func f)
    : m_functor(f)
  {}

  template <typename T, typename Func>
  T FunctorFutureTask<T, Func>::Call()
  {
    return m_functor();
  }
}

#endif
