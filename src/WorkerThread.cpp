#include "WorkerThread.h"
#include "Thread.h"
#include "TaskQueueBase.h"
#include "TaskBase.h"
#include <iostream>

using namespace tpool;
using namespace boost;
using namespace std;

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
	  cout << "prepare to get pop" << endl;

	  // 1. fetch task from task queue
	  TaskBase::Ptr task = taskQueue->Pop();

	  cout << "Got pop" << endl;

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

WorkerThread::~WorkerThread()
{
  cout << "~WorkerThread" << endl;
}
