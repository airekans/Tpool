#include "Mutex.h"
#include <cstdlib>

using namespace tpool::sync;

Mutex::Mutex()
{
  // TODO: check the return code.
  if (pthread_mutex_init(&m_mutex, NULL) != 0)
    {
      exit(1);
    }
}

Mutex::~Mutex()
{
  // TODO: check whether the mutex is in unlocked state.
  pthread_mutex_destroy(&m_mutex);
}

MutexLocker::MutexLocker(Mutex& m)
{
  m_mutex = &m;
  pthread_mutex_lock(&m.m_mutex);
}

MutexLocker::~MutexLocker()
{
  pthread_mutex_unlock(&(m_mutex->m_mutex));
}
