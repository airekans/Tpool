#include "WorkerThread.h"
#include "Thread.h"
#include "TaskQueueBase.h"
#include "TaskBase.h"
#include "EndTask.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <functional>

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
  : m_state(INIT),
    m_isRequestCancel(false)
{
  Init(taskQueue, NoOp());
}

// dtor has to be defined for pimpl idiom
WorkerThread::~WorkerThread()
{
}

void WorkerThread::Cancel()
{
  if (!m_isRequestCancel)
    {
      m_isRequestCancel = true;
      ConditionWaitLocker l(m_stateGuard,
			    bind(not1(mem_fun(&WorkerThread::
					      DoIsFinished)),
				 this));
    }
}

void WorkerThread::CancelAsync()
{
  m_isRequestCancel = true;
}

void WorkerThread::CancelNow()
{
  {
    MutexLocker l(m_runningTaskGuard);
    if (m_runningTask)
      {
	m_runningTask->CancelAsync();
      }
    CancelAsync();
  }
  ConditionWaitLocker l(m_stateGuard,
			bind(not1(mem_fun(&WorkerThread::
					  DoIsFinished)),
			     this));
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
  SetState(RUNNING);
  while (true)
    {
      try
	{
	  // 1. check cancel request
	  CheckCancellation();

	  // 2. fetch task from task queue
	  m_runningTask = m_taskQueue->Pop();

	  // 2.5. check cancel request again
	  CheckCancellation();
	  
	  // 3. perform the task
	  if (m_runningTask)
	    {
	      if (dynamic_cast<EndTask*>(m_runningTask.get()) != NULL)
		{
		  break; // stop the worker thread.
		}
	      else
		{
		  m_runningTask->Run();
		}
	    }
	  // 4. perform any post-task action
	}
      catch (const WorkerThreadExitException&)
	{
	  ConditionNotifyLocker l(m_stateGuard,
				  bind(&WorkerThread::IsRequestCancel,
				       this));
	  DoSetState(FINISHED);
	  // stop the worker thread.
	  break;
	}
      catch (...) // caught other exception
	{
	  // continue
	}
    }
}

bool WorkerThread::IsFinished() const
{
  MutexLocker l(m_stateGuard);
  return DoIsFinished();
}

bool WorkerThread::DoIsFinished() const
{
  return m_state == FINISHED;
}

void WorkerThread::SetState(const State state)
{
  MutexLocker l(m_stateGuard);
  DoSetState(state);
}

void WorkerThread::DoSetState(const State state)
{
  m_state = state;
}

void WorkerThread::GetTaskFromTaskQueue()
{
  MutexLocker l(m_runningTaskGuard);
  m_runningTask = m_taskQueue->Pop();
}
