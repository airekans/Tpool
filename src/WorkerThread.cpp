#include "WorkerThread.h"
#include "Thread.h"
#include "TaskQueueBase.h"
#include "TaskBase.h"
#include <stdexcept>
#include <iostream>

using namespace std;
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
	      task->Run();
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

  // ensure that the thread is created successfully.
  while (true)
    {
      try
	{
	  // check for the creation exception
	  m_thread.reset(new Thread(f));
	  break;
	}
      catch (const runtime_error& e)
	{
	  cerr << "WorkerThread ctor" << endl;
	  cerr << e.what() << endl;
	  cerr << "Try again." << endl;
	}    
    }
}

// dtor has to be defined for pimpl idiom
WorkerThread::~WorkerThread()
{
}
