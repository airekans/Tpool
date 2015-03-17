#ifndef _TPOOL_THREADLOCAL_H_
#define _TPOOL_THREADLOCAL_H_

#include "Likely.h"

#include <boost/noncopyable.hpp>

#include <pthread.h>
#include <cassert>
#include <iostream>

namespace tpool {

template<typename T>
T* ThreadLocalAllocate(T*)
{
    return new T;
}

template<typename T>
void ThreadLocalDeallocate(T* obj)
{
    std::cout << "ThreadLocalDeallocate" << std::endl;
    delete obj;
}

template<typename T>
class ThreadLocal : public ::boost::noncopyable
{
public:
    ThreadLocal()
    {
        pthread_key_create(&m_thread_key, &ThreadLocal::DestroyThreadData);
    }

    ~ThreadLocal()
    {
        T* obj = reinterpret_cast<T*>(pthread_getspecific(m_thread_key));
        if (obj != NULL)
        {
            ThreadLocalDeallocate(obj);
        }

        pthread_key_delete(m_thread_key);
    }

    inline T& GetValue()
    {
        return *GetPointer();
    }

    inline T* GetPointer()
    {
        T* obj = reinterpret_cast<T*>(pthread_getspecific(m_thread_key));
        if (TPOOL_UNLIKELY(obj == NULL))
        {
            obj = ThreadLocalAllocate((T*) NULL);
            assert(obj != NULL);
            pthread_setspecific(m_thread_key, obj);
        }
        return obj;
    }

private:
    static void DestroyThreadData(void* p)
    {
        T* obj = reinterpret_cast<T*>(p);
        ThreadLocalDeallocate(obj);
    }


    pthread_key_t m_thread_key;
};

}  // namespace tpool


#endif /* _TPOOL_THREADLOCAL_H_ */
