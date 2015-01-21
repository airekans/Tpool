#include "Mutex.h"
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <cassert>

using namespace tpool::sync;
using namespace std;

Mutex::Mutex()
: m_isInit(false)
{
    Init();
}

Mutex::~Mutex()
{
    // TODO: check whether the mutex is in unlocked state.
    if (!m_isInit)
    {
        cerr << "WARNING: mutex is uninitialized." << endl;
    }
    else
    {
        pthread_mutex_destroy(&m_mutex);
    }
}

void Mutex::Lock()
{
    if (!m_isInit)
    {
        cerr << "WARNING: mutex is uninitialized." << endl;
    }
    else
    {
        pthread_mutex_lock(&m_mutex);
    }
}

void Mutex::Unlock()
{
    if (!m_isInit)
    {
        cerr << "WARNING: mutex is uninitialized." << endl;
    }
    else
    {
        pthread_mutex_unlock(&m_mutex);
    }
}

bool Mutex::TryLock()
{
    if (!m_isInit)
    {
        cerr << "WARNING: mutex is uninitialized." << endl;
        return false;
    }
    else
    {
        int res = pthread_mutex_trylock(&m_mutex);
        assert(res != EINVAL);
        return (res == 0);
    }
}


void Mutex::Init()
{
    if (!m_isInit)
    {
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw runtime_error("failed to initialize mutex");
        }
        else
        {
            m_isInit = true;
        }
    }
}

MutexLocker::MutexLocker(Mutex& m)
: m_mutex(m)
{
    m_mutex.Lock();
}

MutexLocker::~MutexLocker()
{
    m_mutex.Unlock();
}

MutexWaitLocker::~MutexWaitLocker()
{
    m_mutex.Unlock();
}
