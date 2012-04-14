#ifndef _TPOOL_THREAD_H_
#define _TPOOL_THREAD_H_

#include <pthread.h>
#include <memory>
#include <cstdlib>

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

  public:
    template<class Func>
      explicit Thread(Func& f)
      : m_isStart(false)
      {
	std::auto_ptr<Func> fp(new Func(f));

	if (pthread_create(&m_threadId, NULL,
		       thread_func<Func>,
		       fp.get())
	    != 0)
	  {
	    exit(1);
	  }
	
	fp.release();
	m_isStart = true;
      }

    ~Thread();

  private:
    pthread_t m_threadId;
    bool m_isStart;

  };

}

#endif
