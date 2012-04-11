#include "WorkerThread.h"

using namespace tpool;
using namespace boost;

WorkerThread::WorkerThread(shared_ptr<TaskQueueBase> taskQueue)
{
  m_taskQueue = taskQueue;
}

void WorkerThread::Entry()
{
  while (true)
    {
      // 1. fetch task from task queue
      shared<TaskBase> task = m_taskQueue->Pop();

      // 2. perform the task
      if (task)
	{
	  task->Do();
	}
      // 3. perform any post-task action
    }
}
