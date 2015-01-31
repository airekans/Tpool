// -*- mode: c++ -*-
#ifndef _TPOOL_THREAD_H_
#define _TPOOL_THREAD_H_

#include "ThisThread.h"
#include "Atomic.h"

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

    int GetThreadId() const { return m_threadId; }

private:
    template<class Func>
    static void* ThreadFunction(void* arg);

    void ProcessCreateError(const int error);
    static void ProcessException(const std::exception& e);
    static void ProcessUnknownException();

    template<typename Func>
    struct ThreadArgs {
        ThreadArgs(Atomic<int>* tid, const Func& f)
        : threadId(tid), func(f)
        {}

        Atomic<int>* threadId;
        Func func;
    };

    pthread_t m_threadData;
    Atomic<int> m_threadId;
    bool m_isStart;
};


// Implementation
template<class Func>
Thread::Thread(const Func& f)
: m_threadId(0), m_isStart(false)
{
    typedef ThreadArgs<Func> TArgs;

    std::auto_ptr<TArgs> fp(new TArgs(&m_threadId, f));

    int error =  pthread_create(&m_threadData, NULL,
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
    typedef ThreadArgs<Func> TArgs;

    std::auto_ptr<TArgs> fp(reinterpret_cast<TArgs*>(arg));
    *(fp->threadId) = Tid();

    try
    {
        (fp->func)(); // call the functor
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

}  // namespace tpool

#endif
