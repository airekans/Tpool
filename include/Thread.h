// -*- mode: c++ -*-
#ifndef _TPOOL_THREAD_H_
#define _TPOOL_THREAD_H_

#include <pthread.h>
#include <memory>
#include <cstdlib>
#include <stdexcept>
#include <boost/noncopyable.hpp>


namespace tpool {
  
  class Thread : private boost::noncopyable {
  public:
    template<class Func>
    explicit Thread(const Func& f);

    ~Thread();

  private:
    template<class Func>
    static void* ThreadFunction(void* arg);

    void ProcessCreateError(const int error);
    static void ProcessException(const std::exception& e);
    static void ProcessUnknownException();
    

    pthread_t m_threadId;
    bool m_isStart;
  };


  // Implementation
  template<class Func>
  Thread::Thread(const Func& f)
    : m_isStart(false)
  {
    std::auto_ptr<Func> fp(new Func(f));

    int error =  pthread_create(&m_threadId, NULL,
				ThreadFunction<Func>, fp.get());
    if (error != 0)
      {
	ProcessCreateError(error);
      }
	
    fp.release();
    m_isStart = true;
  }

  template<class Func>
  void* Thread::ThreadFunction(void* arg)
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
    catch (...)
      {
	ProcessUnknownException();
      }
	
    return NULL;
  }
}

#endif
