#include "Timer.h"

#include <cassert>

using namespace std;
using namespace tpool;


tpool::TimerTask::TimerTask()
: m_deadline(0), m_is_interval_task(false)
{}

TimeValue tpool::TimerTask::GetDeadline() const
{
  return m_deadline;
}

void tpool::TimerTask::SetDeadline(const TimeValue deadline)
{
  m_deadline = deadline;
}

bool tpool::TimerTask::IsIntervalTask() const
{
  return m_deadline;
}

void tpool::TimerTask::SetIsIntervalTask(const bool is_interval_task)
{
  m_is_interval_task = is_interval_task;
}

TimerTask::Ptr tpool::Timer::TimerQueue::GetMin() const
{
  assert(!m_queue.empty());
  return m_queue.top();
}

TimerTask::Ptr tpool::Timer::TimerQueue::PopMin()
{
  assert(!m_queue.empty());
  TimerTask::Ptr min_task = m_queue.top();
  m_queue.pop();
  return min_task;
}

void tpool::Timer::TimerQueue::PushTask(TimerTask::Ptr task)
{
  m_queue.push(task);
}

bool tpool::Timer::TimerQueue::IsEmpty() const
{
  return m_queue.empty();
}

unsigned tpool::Timer::TimerQueue::GetSize() const
{
  return static_cast<unsigned>(m_queue.size());
}



