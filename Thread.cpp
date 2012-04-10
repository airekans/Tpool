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


void Thread::Run()
{
  pthread_t threadId;

  if (pthread_create(&threadId, NULL,
		     thread_func, (void*) this) != 0)
    {
      cerr << "pthread_create failed" << endl;
      exit(1);
    }
}

