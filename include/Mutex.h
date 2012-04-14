#ifndef _TPOOL_MUTEX_H_
#define _TPOOL_MUTEX_H_

#include <pthread.h>

namespace tpool {
  namespace sync {
    class MutexLocker;
    
    class Mutex {
      friend class MutexLocker;
      
    public:
      Mutex();
      ~Mutex();
      
    private:
      Mutex(const Mutex&);
      Mutex& operator=(const Mutex&);

      // These two functions can only called by MutexLocker
      void Lock();
      void Unlock();
      
      pthread_mutex_t m_mutex;
    };

    class MutexLocker {
    public:
      MutexLocker(Mutex& m);
      ~MutexLocker();

    private:
      Mutex* m_mutex;
    };
  }
}


#endif
