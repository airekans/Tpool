#ifndef _TPOOL_MUTEX_H_
#define _TPOOL_MUTEX_H_

#include <pthread.h>
#include <boost/noncopyable.hpp>


namespace tpool {
  namespace sync {
    class MutexLocker;
    class MutexWaitLocker;
    class ConditionVariable;
    
    class Mutex : private boost::noncopyable {
      friend class MutexLocker;
      friend class MutexWaitLocker;
      friend class ConditionVariable;

    public:
      Mutex();
      ~Mutex();
      
    private:
      // These two functions can only called by MutexLocker
      void Lock();
      void Unlock();
      
      pthread_mutex_t m_mutex;
    };

    class MutexLocker : private boost::noncopyable {
    public:
      explicit MutexLocker(Mutex& m);
      ~MutexLocker();

    private:
      Mutex& m_mutex;
    };

    class MutexWaitLocker {
    public:
      template<typename ConditionFunc>
	MutexWaitLocker(Mutex& m, ConditionFunc f)
	: m_mutex(m)
	{
	  while (true)
	    {
	      m_mutex.Lock();
	      if (f())
		{
		  break;
		}
	      m_mutex.Unlock();
	    }
	}

      ~MutexWaitLocker();

    private:
      Mutex& m_mutex;
    };
  }
}


#endif
