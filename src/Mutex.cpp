#include "Mutex.h"
#include <cstdlib>
#include <stdexcept>

using namespace tpool::sync;
using namespace std;

Mutex::Mutex()
{
  // TODO: check the return code.
  if (pthread_mutex_init(&m_mutex, NULL) != 0)
    {
      throw runtime_error("failed to initialize mutex");
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
