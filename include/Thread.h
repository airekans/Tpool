#ifndef _TPOOL_THREAD_H_
#define _TPOOL_THREAD_H_

#include <pthread.h>

namespace tpool {

  class Thread {
  private:
    static void* thread_func(void* thisObj);

    struct ThreadId {
      bool isValid;
      pthread_t id;

    ThreadId() : isValid(false)
      {
      }
    };
   
  public:

    virtual ~Thread();

    void Run();
    void Stop();

  protected:
    virtual void Entry() = 0;

  private:
    ThreadId m_threadId;

  };

}

#endif
