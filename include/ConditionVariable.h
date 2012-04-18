#ifndef _TPOOL_CONDITION_VARIABLE_H_
#define _TPOOL_CONDITION_VARIABLE_H_

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

    private:
      Mutex* m_mutex;
    };

    class ConditionLocker : private boost::noncopyable {
    public:
      
    };
  }
}

#endif
