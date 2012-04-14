#ifndef _TPOOL_THREAD_H_
#define _TPOOL_THREAD_H_

#include <pthread.h>
#include <memory>
#include <iostream>

namespace tpool {
  
  class Thread {
  private:
    template<class Func>
      static void* thread_func(void* arg)
      {
	std::auto_ptr<Func> fp(static_cast<Func*>(arg));
	(*fp)(); // call the functor
	
	return NULL;
      }

    struct ThreadId {
      bool isValid;
      pthread_t id;

    ThreadId() : isValid(false)
      {
      }
    };
   
  public:
    template<class Func>
      explicit Thread(Func& f)
      {
	std::auto_ptr<Func> fp(new Func());

	pthread_t tid;
	pthread_create(&tid, NULL,
		       thread_func<Func>,
		       fp.get());
	fp.release();
      }

    virtual ~Thread();

    void Run();
    void Stop();

  private:
    ThreadId m_threadId;

  };

}

#endif
