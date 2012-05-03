#include "TaskBase.h"
#include <exception>
#include <string>

using namespace std;
using namespace tpool;

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
