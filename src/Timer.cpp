#include "Timer.h"

#include <boost/bind.hpp>

#include <cassert>
#include <iostream>

using namespace std;
using namespace tpool;


tpool::TimerTask::TimerTask()
: m_deadline(0), m_interval(0)
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
  return m_interval > 0;
}

TimeValue tpool::TimerTask::GetInterval() const
{
  return m_interval;
}

void tpool::TimerTask::SetInterval(const TimeValue interval)
{
  m_interval = interval;
}

/// Timer::TimerQueue
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

void
tpool::Timer::TimerQueue::PopMinAndPush()
{
  assert(!m_queue.empty());
  // the deadline in the min_task should be changed
  TimerTask::Ptr min_task = m_queue.top();
  m_queue.pop();
  m_queue.push(min_task);
}


void tpool::Timer::TimerQueue::PushTask(TimerTask::Ptr task)
{
  m_queue.push(task);
}

bool tpool::Timer::TimerQueue::IsEmpty() const
{
  return m_queue.empty();
}

tpool::Timer::TimerQueue::TimerQueue(sync::Mutex& m)
: m_cond(m), m_queue(TimerQueue::CompareTimerTask)
{}

unsigned tpool::Timer::TimerQueue::GetSize() const
{
  return static_cast<unsigned>(m_queue.size());
}

tpool::Timer::Timer()
: m_timer_queue(m_queue_guard)
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
    bool is_fired = false;
    TimerTask::Ptr task;

    {
      sync::MutexLocker lock(m_queue_guard);
      while (m_timer_queue.IsEmpty())
      {
        m_timer_queue.Wait();
      }

      task = m_timer_queue.GetMin();
      if (task->IsCancelled())
      {
        (void) m_timer_queue.PopMin();
        continue;
        // mutex unlock here
      }

      const TimeValue now = GetCurrentTime();
      const TimeValue deadline = task->GetDeadline();
      if (deadline <= now)
      {
        is_fired = true;
        if (task->IsIntervalTask())
        {
          task->SetDeadline(now + task->GetInterval());
          m_timer_queue.PopMinAndPush();
        }
        else
        {
          (void) m_timer_queue.PopMin();
        }
      }
      else
      {
        const TimeValue delay = deadline - now;
        (void) m_timer_queue.TimedWait(delay);
      }
    } // mutex unlock

    if (is_fired)
    {
      task->Run();
    }
  }
}

void tpool::Timer::ProcessError(const std::exception& e)
{
  cerr << "Timer ctor" << endl;
  cerr << e.what() << endl;
  cerr << "Try again." << endl;
}

void tpool::Timer::TimerQueue::Wait()
{
  m_cond.Wait();
}

bool tpool::Timer::TimerQueue::TimedWait(TimeValue delay)
{
  return m_cond.TimedWait(delay);
}

bool tpool::Timer::TimerQueue::CompareTimerTask(
    TimerTask::Ptr a, TimerTask::Ptr b)
{
  return a->GetDeadline() < b->GetDeadline();
}




