#include "Timer.h"

#include <boost/bind.hpp>

#include <cassert>
#include <iostream>

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

tpool::Timer::TimerQueue::TimerQueue()
: m_queue(TimerQueue::CompareTimerTask)
{}

unsigned tpool::Timer::TimerQueue::GetSize() const
{
  return static_cast<unsigned>(m_queue.size());
}

tpool::Timer::Timer()
{
  using boost::bind;

  // ensure that the thread is created successfully.
  while (true)
  {
    try
    {
      // check for the creation exception
      m_thread.reset(new Thread(bind(&Timer::ThreadFunction, this)));
      break;
    }
    catch (const std::exception& e)
    {
      ProcessError(e);
    }
  }
}

tpool::Timer::~Timer()
{
}

void tpool::Timer::ThreadFunction()
{
  TimeValue wait_time = 0;
  while (true)
  {
    // lock here
    while (m_timer_queue.IsEmpty())
    {
      // cond_wait here
    }

    TimerTask::Ptr task = m_timer_queue.GetMin();
    // unlock here
    if (task->IsCancelled())
    {
      // lock here
      (void) m_timer_queue.PopMin();
      // unlock here
      continue;
    }

    TimeValue now = GetCurrentTime();
    TimeValue deadline = task->GetDeadline();
    if (deadline <= now)
    {
      task->Run();
      // lock here
      (void) m_timer_queue.PopMin();
      // unlock here
    }
    else
    {
      // lock
      // sleep the delay here
      // unlock
    }
  }
}

void tpool::Timer::ProcessError(const std::exception& e)
{
  cerr << "Timer ctor" << endl;
  cerr << e.what() << endl;
  cerr << "Try again." << endl;
}

bool tpool::Timer::TimerQueue::CompareTimerTask(
    TimerTask::Ptr a, TimerTask::Ptr b)
{
  return a->GetDeadline() < b->GetDeadline();
}



