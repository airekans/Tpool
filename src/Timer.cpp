#include "Timer.h"

#include <boost/bind.hpp>

#include <cassert>
#include <iostream>

#include <sys/time.h>

using namespace std;
using namespace tpool;


TimeValue tpool::GetCurrentTime()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    TimeValue now_in_ms = now.tv_sec;
    now_in_ms = now_in_ms * 1000 + now.tv_usec / 1000;
    return now_in_ms;
}

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

/// precondition: the mutex associated with this queue
///    should be locked
void tpool::Timer::TimerQueue::PushTask(TimerTask::Ptr task)
{
    m_queue.push(task);
    // there is only one thread(the timer thread)
    // waiting for this condition, so it's okay
    // to use notify.
    m_cond.Notify();
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

void tpool::Timer::TimerQueue::Wait()
{
    m_cond.Wait();
}

bool tpool::Timer::TimerQueue::TimedWait(TimeValue delay)
{
    return m_cond.TimedWait(delay);
}

/// precondition: the mutex associated with this queue
///    should be locked
void tpool::Timer::TimerQueue::Clear()
{
    while (!m_queue.empty())
    {
        m_queue.pop();
    }
    m_cond.Notify();
}

bool tpool::Timer::TimerQueue::CompareTimerTask(
        TimerTask::Ptr a, TimerTask::Ptr b)
{
    return a->GetDeadline() > b->GetDeadline();
}


/// Timer
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
            m_thread.reset(new CancelableThread(
                bind(&Timer::ThreadFunction, this, _1)));
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
    Stop();
}

void tpool::Timer::ThreadFunction(const Function& checkFunc)
{
    while (true)
    {
        bool is_fired = false;
        TimerTask::Ptr task;

        {
            sync::MutexLocker lock(m_queue_guard);
            while (m_timer_queue.IsEmpty())
            {
                checkFunc();
                m_timer_queue.Wait();
            }
            checkFunc();

            task = m_timer_queue.GetMin();
            const TimeValue now = GetCurrentTime();
            const TimeValue deadline = task->GetDeadline();
            if (task->IsCancelled())
            {
                is_fired = true;
                (void) m_timer_queue.PopMin();
            }
            else if (deadline <= now)
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

bool tpool::Timer::RunLater(TimerTask::Ptr task, TimeValue delay_in_ms)
{
    return DoRunLater(task, delay_in_ms);
}

bool tpool::Timer::RunEvery(TimerTask::Ptr task, TimeValue interval_in_ms,
        bool is_run_now)
{
    return DoRunEvery(task, interval_in_ms, is_run_now);
}

bool tpool::Timer::DoRunLater(TimerTask::Ptr task, TimeValue delay_in_ms)
{
    const TimeValue now = GetCurrentTime();
    const TimeValue task_deadline = now + delay_in_ms;

    if (m_thread->IsRequestCancel())
    {
        return false;
    }

    // A task having been put in the timer cannot be put again
    if (task->GetDeadline() != 0)
    {
        return false;
    }

    task->SetDeadline(task_deadline);
    sync::MutexLocker lock(m_queue_guard);
    m_timer_queue.PushTask(task);
    return true;
}

bool tpool::Timer::DoRunEvery(TimerTask::Ptr task, TimeValue interval_in_ms,
        bool is_run_now)
{
    const TimeValue now = GetCurrentTime();

    if (m_thread->IsRequestCancel())
    {
        return false;
    }

    // A task having been put in the timer cannot be put again
    if (task->GetDeadline() != 0)
    {
        return false;
    }

    if (is_run_now)
    {
        task->SetDeadline(now);
    }
    else
    {
        task->SetDeadline(now + interval_in_ms);
    }
    task->SetInterval(interval_in_ms);
    sync::MutexLocker lock(m_queue_guard);
    m_timer_queue.PushTask(task);
    return true;
}

void tpool::Timer::StopAsync()
{
    m_thread->CancelAsync();
    sync::MutexLocker lock(m_queue_guard);
    m_timer_queue.Clear();
}

void tpool::Timer::Stop()
{
    StopAsync();
    m_thread->Cancel();
}

void tpool::Timer::ProcessError(const std::exception& e)
{
    cerr << "Timer ctor" << endl;
    cerr << e.what() << endl;
    cerr << "Try again." << endl;
}




