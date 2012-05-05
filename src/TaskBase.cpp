#include "TaskBase.h"
#include <exception>
#include <string>

using namespace std;
using namespace tpool;
using namespace tpool::sync;

namespace {
  class TaskCancelException : public exception {
  public:
    explicit TaskCancelException(const string& s)
      : m_cancelMessage(s)
    {}

    virtual ~TaskCancelException() throw()
    {}

    virtual const char* what() const throw()
    {
      return m_cancelMessage.c_str();
    }

  private:
    string m_cancelMessage;
  };

  // "Stop" means state == FINISHED or state == CANCELLED
  bool IsStopState(TaskBase& task)
  {
    return task.GetState() == TaskBase::FINISHED ||
      task.GetState() == TaskBase::CANCELLED;
  }
}

TaskBase::TaskBase()
  : m_state(INIT), m_isRequestCancel(false)
{
  
}

void TaskBase::Run()
{
  m_state = RUNNING;
  try
    {
      CheckCancellation(); // check before running the task.
      DoRun();
      m_state = FINISHED;
    }
  catch (const TaskCancelException&)
    {
      m_state = CANCELLED;
    }
}

void TaskBase::Cancel()
{
  m_isRequestCancel = true;

  // wait until the task is cancelled.
}

void TaskBase::CancelAsync()
{
  m_isRequestCancel = true;
}

TaskBase::State TaskBase::GetState() const
{
  return m_state;
}

void TaskBase::CheckCancellation() const
{
  if (m_isRequestCancel)
    {
      throw TaskCancelException("cancel task");
    }
}
