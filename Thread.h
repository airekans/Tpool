#ifndef _TPOOL_THREAD_H_
#define _TPOOL_THREAD_H_

#include <pthread.h>
#include <iostream>
#include <cstdlib>

namespace tpool {

  class Thread {
  private:
    static void* thread_func(void* thisObj);
   
  public:
    virtual ~Thread() {}

    void Run();

  protected:
    virtual void Entry() = 0;

  };
}

#endif
