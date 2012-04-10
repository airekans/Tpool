#ifndef _TPOOL_THREAD_H_
#define _TPOOL_THREAD_H_

#include <pthread.h>
#include <iostream>
#include <cstdlib>

namespace tpool {

  namespace __impl {

    template<class ObjClass>
      void* thread_func(void* obj)
      {
	ObjClass* o = static_cast<ObjClass*>(obj);
	o->Entry();
      }

  } // implementation details


  template<class ChildClass>
    class Thread {
  public:
    virtual ~Thread() {}

    void Run()
    {
      using std::cerr;
      using std::endl;
      using __impl::thread_func;

      pthread_t threadId;

      if (pthread_create(&threadId, NULL,
			 thread_func<ChildClass>, (void*) this) != 0)
	{
	  cerr << "pthread_create failed" << endl;
	  exit(1);
	}
    }
    virtual void Entry() = 0;

  };
}

#endif
