#ifndef _TPOOL_THREAD_H_
#define _TPOOL_THREAD_H_

#include <pthread.h>
#include <memory>
#include <cstdlib>
#include <stdexcept>
#include <boost/noncopyable.hpp>


namespace tpool {
  
  class Thread : private boost::noncopyable {
  private:
    template<class Func>
      static void* ThreadFunction(void* arg)
      {
	std::auto_ptr<Func> fp(static_cast<Func*>(arg));

	try
	  {
	    (*fp)(); // call the functor
	  }
	catch (const std::exception& e)
	  {
	    ProcessException(e);
	  }
	
	return NULL;
      }

    static void ProcessException(const std::exception& e);

  public:
    template<class Func>
      explicit Thread(const Func& f)
      : m_isStart(false)
      {
	std::auto_ptr<Func> fp(new Func(f));

	if (pthread_create(&m_threadId, NULL,
		       ThreadFunction<Func>,
		       fp.get())
	    != 0)
	  {
	    // TODO: check different error code.
	    throw std::runtime_error("failed to create thread");
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
