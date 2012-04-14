#include "Thread.h"
#include <pthread.h>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace tpool;

Thread::~Thread()
{
}

void Thread::Run()
{
  // if (m_threadId.isValid) // the thread has been started
  //   {
  //     return;
  //   }

  // if (pthread_create(&(m_threadId.id), NULL,
  // 		     thread_func, (void*) this) != 0)
  //   {
  //     cerr << "pthread_create failed" << endl;
  //     exit(1);
  //   }

  // m_threadId.isValid = true;
}

void Thread::Stop()
{
  // if (!m_threadId.isValid)
  //   {
  //     return;
  //   }

  // pthread_cancel(m_threadId.id);
}

