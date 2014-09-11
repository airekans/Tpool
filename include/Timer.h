// -*- mode: c++ -*-
#ifndef _TPOOL_TIMER_H_
#define _TPOOL_TIMER_H_

#include "Thread.h"

#include <memory>
#include <boost/noncopyable.hpp>


namespace tpool {

  class TimerEvent {
  public:
    TimerEvent();
    ~TimerEvent();
  };


  // This timer implementation uses a thread to 
  // sleep until timeout
  class Timer : public boost::noncopyable {
  public:
    Timer();
    ~Timer();

  private:

    std::auto_ptr<Thread> m_thread;
  };
}

#endif  // _TPOOL_TIMER_H_


