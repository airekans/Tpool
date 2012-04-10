#ifndef _TPOOL_THREAD_H_
#define _TPOOL_THREAD_H_

#include <pthread.h>

namespace tpool {

  class Thread {
  private:
    static void* thread_func(void* thisObj);

    struct Tid {
      bool isValid;
      pthread_t id;

    Tid() : isValid(false)
      {
      }
    };
   
  public:
    typedef Tid ThreadId;

    virtual ~Thread() {}

    ThreadId Run();
    void Stop();
    ThreadId GetId() const;

  protected:
    virtual void Entry() = 0;

  private:
    ThreadId m_threadId;

  };

}

#endif
