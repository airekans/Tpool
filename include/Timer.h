// -*- mode: c++ -*-
#ifndef _TPOOL_TIMER_H_
#define _TPOOL_TIMER_H_

#include "Thread.h"

#include "TaskBase.h"

#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>


namespace tpool {

  class TimerTask : public TaskBase {
  public:
	typedef boost::shared_ptr<TimerTask> Ptr;

  };

  // This timer implementation uses a thread to 
  // sleep until timeout
  class Timer : public boost::noncopyable {
  public:
    Timer();
    ~Timer();

    void RunAt(TimerTask::Ptr task, long delay);
    void RunLater(TimerTask::Ptr task, long delay);
    void RunEvery(TimerTask::Ptr task, long delay);

    template<typename Func>
    TimerTask::Ptr RunAt(Func func, long delay);

  private:
    std::auto_ptr<Thread> m_thread;
  };
}

#endif  // _TPOOL_TIMER_H_


