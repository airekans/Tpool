#include "WorkerThread.h"
#include "Thread.h"
#include "TaskQueueBase.h"
#include "TaskBase.h"

using namespace tpool;
using namespace boost;

namespace {
  struct ThreadFunc {
    TaskQueueBase::Ptr taskQueue;

    ThreadFunc(TaskQueueBase::Ptr q)
    {
      taskQueue = q;
    }
    
    ThreadFunc(const ThreadFunc& f)
    {
      taskQueue = f.taskQueue;
    }

    void operator()()
    {
      while (true)
	{
	  // 1. fetch task from task queue
	  TaskBase::Ptr task = taskQueue->Pop();

	  // 2. perform the task
	  if (task)
	    {
	      task->Do();
	    }
	  // 3. perform any post-task action
	}
    }
  };
  
}


WorkerThread::WorkerThread(TaskQueueBase::Ptr taskQueue)
{
  m_taskQueue = taskQueue;

  ThreadFunc f(taskQueue);

  m_thread.reset(new Thread(f));
}

// dtor has to be defined for pimpl idiom
WorkerThread::~WorkerThread()
{
}
