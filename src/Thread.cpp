#include "Thread.h"
#include <iostream>


using namespace std;
using namespace tpool;

void Thread::ProcessException(const exception& e)
{
  cerr << e.what() << endl;
}

Thread::~Thread()
{
  if (m_isStart)
    {
      pthread_join(m_threadId, NULL);
    }
}


