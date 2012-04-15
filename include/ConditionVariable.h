#ifndef _TPOOL_CONDITION_VARIABLE_H_
#define _TPOOL_CONDITION_VARIABLE_H_

#include <pthread.h>
#include <boost/noncopyable.hpp>


namespace tpool {
  namespace sync {
    class Mutex;
    
    class ConditionVariable : private boost::noncopyable {
    public:
      explicit ConditionVariable(Mutex& m);
      ~ConditionVariable();

      void Notify();
      void NotifyAll();
      void Wait();

    private:
      Mutex* m_mutex;
      pthread_cond_t m_cond;
    };

    class ConditionLocker : private boost::noncopyable {
    public:
      explicit ConditionLocker(ConditionVariable& c);
      ~ConditionLocker();
      
      void NotifyWhen();
      void NotifyAllWhen();
      void WaitWhen();

    private:
      ConditionVariable* m_conditionVariable;
    };
  }
}

#endif
