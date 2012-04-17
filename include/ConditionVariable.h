#ifndef _TPOOL_CONDITION_VARIABLE_H_
#define _TPOOL_CONDITION_VARIABLE_H_

#include "Mutex.h"
#include <pthread.h>
#include <boost/noncopyable.hpp>


namespace tpool {
  namespace sync {
    class ConditionWaitLocker;
    class ConditionNotifyLocker;
    class ConditionNotifyAllLocker;

    class ConditionVariable : private boost::noncopyable {
      friend class ConditionWaitLocker;
      friend class ConditionNotifyLocker;
      friend class ConditionNotifyAllLocker;
      
    public:
      explicit ConditionVariable(Mutex& m);
      ~ConditionVariable();

    private:
      void Notify();
      void NotifyAll();
      void Wait();
      void Lock();
      void Unlock();

      Mutex& m_mutex;
      pthread_cond_t m_cond;
    };

    class MutexConditionVariable :
      public Mutex, public ConditionVariable
    {
    public:
      MutexConditionVariable();
    };

    class ConditionWaitLocker : private boost::noncopyable {
    public:
      template<typename WaitConditionFunc>
      explicit ConditionWaitLocker(ConditionVariable& c,
			       WaitConditionFunc f)
	: m_conditionVariable(c)
      {
	m_conditionVariable.Lock();
	while (f())
	  {
	    m_conditionVariable.Wait();
	  }
      }
      
      ~ConditionWaitLocker()
      {
	m_conditionVariable.Unlock();
      }
      
    private:
      ConditionVariable& m_conditionVariable;
    };

    class ConditionNotifyLocker : private boost::noncopyable {
    public:
      template<typename NotifyConditionFunc>
      explicit ConditionNotifyLocker(ConditionVariable& c,
				     NotifyConditionFunc f)
	: m_conditionVariable(c)
      {
	m_conditionVariable.Lock();
	if (f())
	  {
	    m_conditionVariable.Notify();
	  }
	// The user code should set the condition to true
      }

      ~ConditionNotifyLocker()
      {
	m_conditionVariable.Unlock();
      }

    private:
      ConditionVariable& m_conditionVariable;
    };

    class ConditionNotifyAllLocker : private boost::noncopyable {
    public:
      template<typename NotifyConditionFunc>
      explicit ConditionNotifyAllLocker(ConditionVariable& c,
					NotifyConditionFunc f)
	: m_conditionVariable(c)
      {
	m_conditionVariable.Lock();
	if (f())
	  {
	    m_conditionVariable.NotifyAll();
	  }
      }

      ~ConditionNotifyAllLocker()
      {
	m_conditionVariable.Unlock();
      }

    private:
      ConditionVariable& m_conditionVariable;
    };
  }
}

#endif
