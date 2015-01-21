// -*- mode: c++ -*-
#ifndef _TPOOL_MUTEX_H_
#define _TPOOL_MUTEX_H_

#include <pthread.h>
#include <boost/noncopyable.hpp>

#include <cassert>


namespace tpool {
namespace sync {

class MutexLocker;
class MutexWaitLocker;
class ConditionVariable;

class Mutex : private boost::noncopyable {
    friend class MutexLocker;
    friend class MutexWaitLocker;
    friend class MutexTryLocker;  // for TryLock
    friend class ConditionVariable;

public:
    Mutex();
    ~Mutex();

private:
    // These two functions can only called by MutexLocker
    void Lock();
    void Unlock();
    bool TryLock();
    void Init();

    pthread_mutex_t m_mutex;
    volatile bool m_isInit;
};

class MutexLocker : private boost::noncopyable {
public:
    explicit MutexLocker(Mutex& m);
    ~MutexLocker();

private:
    Mutex& m_mutex;
};

class MutexWaitLocker : public boost::noncopyable {
public:
    template<typename ConditionFunc>
    MutexWaitLocker(Mutex& m, ConditionFunc f)
    : m_mutex(m)
      {
        while (true)
        {
            m_mutex.Lock();
            if (!f()) // wait when the condition is true
            {
                break;
            }
            m_mutex.Unlock();
        }
      }

    ~MutexWaitLocker();

private:
    Mutex& m_mutex;
};

// Usage:
// Mutex mutex;
// MutexTryLocker lock(mutex);
// while (!lock.TryLock())
// {
//     // do something else or wait a minute
// }
// // lock here
class MutexTryLocker : public boost::noncopyable {
public:
    MutexTryLocker(Mutex& mutex)
    : m_mutex(mutex), m_isLocked(false)
    {}

    ~MutexTryLocker()
    {
        if (m_isLocked)
        {
            m_mutex.Unlock();
        }
    }

    bool TryLock()
    {
        assert(!m_isLocked);
        m_isLocked = m_mutex.TryLock();
        return m_isLocked;
    }

private:
    Mutex& m_mutex;
    bool m_isLocked;
};

}  // namespace sync
}  // namespace tpool


#endif
