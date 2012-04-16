#ifndef _TPOOL_MUTEX_H_
#define _TPOOL_MUTEX_H_

#include <pthread.h>
#include <boost/noncopyable.hpp>


namespace tpool {
  namespace sync {
    class MutexLocker;
    class ConditionVariable;
    
    class Mutex : private boost::noncopyable {
      friend class MutexLocker;
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
      Mutex* m_mutex;
    };

    template<typename ConditionFunc>
      void MutexWaitWhen(Mutex& m, ConditionFunc f)
      {
	while (true)
	  {
	    MutexLocker l(m);
	    if (f())
	      {
		return;
	      }
	  }
      }
  }
}


#endif
