// -*- mode: c++ -*-
#ifndef _TPOOL_TIMER_H_
#define _TPOOL_TIMER_H_

#include "Thread.h"

#include "TaskBase.h"

#include <memory>
#include <queue>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>


namespace tpool {
  typedef unsigned long long TimeValue;

  class Timer;

  /// Subclass should override the DoRun function
  class TimerTask : public TaskBase {
    friend class Timer;

  public:
    typedef boost::shared_ptr<TimerTask> Ptr;

    TimerTask();

  private:
    TimeValue GetDeadline() const;
    void SetDeadline(const TimeValue deadline);
    bool IsIntervalTask() const;
    void SetIsIntervalTask(const bool is_interval_task);

    TimeValue m_deadline;
    bool m_is_interval_task;
  };

  // This timer implementation uses a thread to 
  // sleep until timeout
  class Timer : public boost::noncopyable {
  public:
    Timer();
    ~Timer();

    /// RunLater will run the task after delay_in_ms.
    void RunLater(TimerTask::Ptr task, TimeValue delay_in_ms);

    /// RunEvery will run the task every interval_in_ms.
    /// If is_run_now is true, the task will be run immediately
    void RunEvery(TimerTask::Ptr task, TimeValue interval_in_ms,
            bool is_run_now);

    template<typename Func>
    TimerTask::Ptr RunLater(Func func, TimeValue delay_in_ms);

    template<typename Func>
    TimerTask::Ptr RunEvery(Func func, TimeValue interval_in_ms,
            bool is_run_now);

  private:
    void ThreadFunction();

    class TimerQueue {
    public:
      TimerTask::Ptr GetMin() const;
      TimerTask::Ptr PopMin();
      void PushTask(TimerTask::Ptr task);
      bool IsEmpty() const;
      unsigned GetSize() const;

    private:
      ::std::priority_queue<TimerTask::Ptr> m_queue;
    };

    TimerQueue m_timer_queue;
    ::std::auto_ptr<Thread> m_thread;
  };
}

#endif  // _TPOOL_TIMER_H_


