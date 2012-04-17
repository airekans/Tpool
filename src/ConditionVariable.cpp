#include "ConditionVariable.h"
#include "Mutex.h"

using namespace tpool::sync;


ConditionVariable::ConditionVariable(Mutex& m)
  : m_mutex(m)
{
  pthread_cond_init(&m_cond, NULL);
}

ConditionVariable::~ConditionVariable()
{
  pthread_cond_destroy(&m_cond);
}

void ConditionVariable::Notify()
{
  pthread_cond_signal(&m_cond);
}

void ConditionVariable::NotifyAll()
{
  pthread_cond_broadcast(&m_cond);
}

void ConditionVariable::Wait()
{
  pthread_cond_wait(&m_cond, &(m_mutex.m_mutex));
}

void ConditionVariable::Lock()
{
  m_mutex.Lock();
}

void ConditionVariable::Unlock()
{
  m_mutex.Unlock();
}
