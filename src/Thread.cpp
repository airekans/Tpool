#include "Thread.h"
#include <pthread.h>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace tpool;

void* Thread::thread_func(void* thisObj)
{
  Thread* obj = static_cast<Thread*>(thisObj);
  obj->Entry();
  return NULL;
}

Thread::~Thread()
{
  if (m_threadId.isValid)
    {
      pthread_join(m_threadId.id, NULL);
    }
}

void Thread::Run()
{
  if (m_threadId.isValid) // the thread has been started
    {
      return;
    }

  if (pthread_create(&(m_threadId.id), NULL,
		     thread_func, (void*) this) != 0)
    {
      cerr << "pthread_create failed" << endl;
      exit(1);
    }

  m_threadId.isValid = true;
}

void Thread::Stop()
{
  if (!m_threadId.isValid)
    {
      return;
    }

  pthread_cancel(m_threadId.id);
}

