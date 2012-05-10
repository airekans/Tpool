#include "WorkerThread.h"
#include "Thread.h"
#include "TaskQueueBase.h"
#include "TaskBase.h"
#include "EndTask.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <boost/bind.hpp>

using namespace std;
using namespace tpool;
using namespace tpool::sync;
using namespace boost;

namespace {
  // This exception makes thread exit.
  class WorkerThreadExitException : public std::exception {
  public:
    explicit WorkerThreadExitException(const string& s)
      : m_exitMessage(s)
    {}

    virtual ~WorkerThreadExitException() throw()
    {}

    virtual const char* what() const throw()
    {
      return m_exitMessage.c_str();
    }

  private:
    string m_exitMessage;
  };

  struct NoOp {
    void operator()()
    {}
  };
}


WorkerThread::WorkerThread(TaskQueueBase::Ptr taskQueue)
  : m_isRequestCancel(false)
{
  Init(taskQueue, NoOp());
}

// dtor has to be defined for pimpl idiom
WorkerThread::~WorkerThread()
{
}

void WorkerThread::Cancel()
{
  m_isRequestCancel = true;

  ConditionWaitLocker l(m_cancelCondition,
			bind(&WorkerThread::IsRequestCancel, this));
}

void WorkerThread::CancelAsync()
{
  m_isRequestCancel = true;
}

void WorkerThread::ProcessError(const std::exception& e)
{
  cerr << "WorkerThread ctor" << endl;
  cerr << e.what() << endl;
  cerr << "Try again." << endl;
}

bool WorkerThread::IsRequestCancel() const
{
  return m_isRequestCancel;
}

void WorkerThread::CheckCancellation() const
{
  if (m_isRequestCancel)
    {
      throw WorkerThreadExitException("cancelled");
    }
}

void WorkerThread::WorkFunction()
{
  while (true)
    {
      try
	{
	  // 1. check cancel request
	  CheckCancellation();

	  // 2. fetch task from task queue
	  TaskBase::Ptr task = m_taskQueue->Pop();

	  // 3. perform the task
	  if (task)
	    {
	      if (dynamic_cast<EndTask*>(task.get()) != NULL)
		{
		  break; // stop the worker thread.
		}
	      else
		{
		  task->Run();
		}
	    }
	  // 4. perform any post-task action
	}
      catch (const WorkerThreadExitException&)
	{
	  ConditionNotifyLocker l(m_cancelCondition,
				  bind(&WorkerThread::IsRequestCancel,
				       this));
	  m_isRequestCancel = false;
	  // stop the worker thread.
	  break;
	}
      catch (...) // caught other exception
	{
	  // continue
	}
    }
}
