#include "TaskBase.h"

using namespace tpool;

TaskBase::TaskBase()
  : m_state(INIT)
{
  
}

void TaskBase::Run()
{
  m_state = RUNNING;
  DoRun();
  m_state = FINISHED;
}

TaskBase::State TaskBase::GetState() const
{
  return m_state;
}
