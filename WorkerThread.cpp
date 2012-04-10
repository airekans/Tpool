#include "WorkerThread.h"

using namespace tpool;

void WorkerThread::Entry()
{
  while (true)
    {
      // 1. fetch task from task queue
      // 2. perform the task
      // 3. perform any post-task action
    }
}
