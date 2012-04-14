#include "Thread.h"

using namespace std;
using namespace tpool;

Thread::~Thread()
{
  if (m_isStart)
    {
      pthread_join(m_threadId, NULL);
    }
}


