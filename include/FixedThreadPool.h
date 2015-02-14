// -*- mode: c++ -*-
#ifndef _TPOOL_FIXED_THREAD_POOL_H_
#define _TPOOL_FIXED_THREAD_POOL_H_

#include "WorkerThread.h"
#include "LinearTaskQueue.h"
#include "FunctorTask.h"
#include "EndTask.h"
#include "ConditionVariable.h"
#include "Atomic.h"
#include "FunctorFutureTask.h"
#include "Timer.h"

#include <vector>
#include <cstdlib> // for size_t
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <functional>

namespace tpool {
  
  template<class TaskQueue = LinearTaskQueue>
  class FixedThreadPool : public boost::noncopyable {
  private:
    enum State {
      INIT,
      RUNNING,
      FINISHED,
    };

  public:
    FixedThreadPool(const size_t threadNum = 4);
    ~FixedThreadPool();
  

    size_t GetThreadNum() const;
    TaskBase::Ptr AddTask(TaskBase::Ptr task);

    template<typename Func>
    TaskBase::Ptr AddTask(Func f);

    template <typename T, typename Func>
    typename FutureTask<T>::Ptr AddFutureTask(Func f);

    TaskBase::Ptr AddTimerTask(TaskBase::Ptr task,
        TimeValue delay_in_ms);

    TaskBase::Ptr AddIntervalTask(TaskBase::Ptr task,
        TimeValue period_in_ms, bool is_run_now);

    template<typename Func>
    TaskBase::Ptr AddTimerTask(Func f, TimeValue delay_in_ms);

    template<typename Func>
    TaskBase::Ptr AddIntervalTask(Func f,
        TimeValue period_in_ms, bool is_run_now);
    
    void Stop();
    void StopAsync();
    void StopNow();
    
  private:
    TaskBase::Ptr DoAddTask(TaskBase::Ptr task);
    TaskBase::Ptr DoAddTimerTask(TaskBase::Ptr task,
        TimeValue delay_in_ms);
    TaskBase::Ptr DoAddIntervalTask(TaskBase::Ptr task,
        TimeValue period_in_ms, bool is_run_now);
    void NotifyWhenThreadsStop();
    bool IsFinished() const;
    bool DoIsFinished() const;
    void SetState(const State state);
    void DoSetState(const State state);

    struct ThreadPoolTimerTask;

    typedef std::vector<WorkerThread::Ptr> WorkerThreads;
    
    TaskQueueBase::Ptr m_taskQueue;
    sync::Mutex m_stoppedThreadNumGuard;
    size_t m_stoppedThreadNum;
    State m_state;
    mutable sync::MutexConditionVariable m_stateGuard;
    Atomic<bool> m_isRequestStop;
    WorkerThreads m_threads;
    sync::Mutex m_timerGuard;
    ::boost::scoped_ptr<Timer> m_timer;
  };

  typedef FixedThreadPool<> LFixedThreadPool;

  // Implementation
  template<class TaskQueue>
  FixedThreadPool<TaskQueue>::FixedThreadPool(const size_t threadNum)
    : m_taskQueue(new TaskQueue),
      m_stoppedThreadNum(0),
      m_state(INIT),
      m_isRequestStop(false),
      m_threads(threadNum)
  {
    using boost::bind;
    using boost::protect;

    BOOST_FOREACH(WorkerThread::Ptr& t, m_threads)
      {
	t.reset(new WorkerThread(m_taskQueue,
				 protect(bind(&FixedThreadPool::
					      NotifyWhenThreadsStop, this))
				 ));
      }

    SetState(RUNNING);
  }
  
  template<class TaskQueue>
  FixedThreadPool<TaskQueue>::~FixedThreadPool()
  {
    // keep other thread from pushing more tasks
    StopAsync();
  }

  template<class TaskQueue>
  size_t FixedThreadPool<TaskQueue>::GetThreadNum() const
  {
    return m_threads.size();
  }

  template<class TaskQueue>
  TaskBase::Ptr FixedThreadPool<TaskQueue>::AddTask(TaskBase::Ptr task)
  {
    return DoAddTask(task);
  }

  template<class TaskQueue>
  template<typename Func>
  TaskBase::Ptr FixedThreadPool<TaskQueue>::AddTask(Func f)
  {
    return DoAddTask(MakeFunctorTask(f));
  }

  template<class TaskQueue>
  template<typename T, typename Func>
  typename FutureTask<T>::Ptr FixedThreadPool<TaskQueue>::AddFutureTask(Func f)
  {
    typename FutureTask<T>::Ptr task(MakeFunctorFutureTask<T>(f).get());
    DoAddTask(TaskBase::Ptr(task.get()));
    return task;
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::Stop()
  {
    using boost::bind;
    using std::mem_fun;
    using std::not1;

    StopAsync();

    // wait until all worker threads stop
    sync::ConditionWaitLocker l(m_stateGuard,
				bind(not1(mem_fun(&FixedThreadPool::
						  DoIsFinished)),
				     this));

    bool isTimerStarted = false;
    {
      sync::MutexLocker lock(m_timerGuard);
      isTimerStarted = static_cast<bool>(m_timer);
    }

    if (isTimerStarted)
    {
      m_timer->Stop(); // wait timer thread to stop
    }
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::StopAsync()
  {
    if (m_isRequestStop.CompareAndSet(false, true))
      {
        // Stop the timer first
        bool isTimerStarted = false;
        {
          sync::MutexLocker lock(m_timerGuard);
          isTimerStarted = static_cast<bool>(m_timer);
        }

        if (isTimerStarted)
        {
          m_timer->StopAsync();
        }

        // NOTE: there may be some tasks pushed after these EndTasks,
        //     because m_isRequestStop is locked alone, and if a thread
        //     calling AddTask before this StopAsync and stop running,
        //     then StopAsync is run, push EndTasks and the thread is
        //     running again, then the tasks will be pushed after the
        //     EndTasks.
	const size_t threadNum = m_threads.size();
	for (size_t i = 0; i < threadNum; ++i)
	  {
	    m_taskQueue->Push(TaskBase::Ptr(new EndTask));
	  }
      }
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::StopNow()
  {
    StopAsync();

    BOOST_FOREACH(WorkerThread::Ptr& t, m_threads)
      {
	t->CancelAsync();
      }
    
    BOOST_FOREACH(WorkerThread::Ptr& t, m_threads)
      {
	t->CancelNow();
      }

    bool isTimerStarted = false;
    {
      sync::MutexLocker lock(m_timerGuard);
      isTimerStarted = static_cast<bool>(m_timer);
    }

    // NOTE: there may be a case that when a thread calling
    //    AddTimerTask and stop running, then this thread calling
    //    StopNow, then the timer thread runs again.
    //    In this case, the thread will be created and put a timer task
    //    but the task will not be put in the thread pool task queue.
    if (isTimerStarted)
    {
      m_timer->Stop(); // wait timer thread to stop
    }
  }

  template<class TaskQueue>
  TaskBase::Ptr FixedThreadPool<TaskQueue>::DoAddTask(TaskBase::Ptr task)
  {
    if (m_isRequestStop)
    {
      return TaskBase::Ptr();
    }
    else if (!task)
    {
      return task;
    }
    
    m_taskQueue->Push(task);
    return task;
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::NotifyWhenThreadsStop()
  {
    using boost::bind;

    size_t stoppedThreadNum = 0;
    {
      sync::MutexLocker l(m_stoppedThreadNumGuard);
      stoppedThreadNum = ++m_stoppedThreadNum;
    }

    if (stoppedThreadNum >= m_threads.size())
      {
    	sync::ConditionNotifyAllLocker l(m_stateGuard,
    				      bind(&Atomic<bool>::GetData,
    					   &m_isRequestStop));
	DoSetState(FINISHED);
      }
  }

  template<class TaskQueue>
  bool FixedThreadPool<TaskQueue>::IsFinished() const
  {
    sync::MutexLocker l(m_stateGuard);
    return DoIsFinished();
  }

  template<class TaskQueue>
  bool FixedThreadPool<TaskQueue>::DoIsFinished() const
  {
    return m_state == FINISHED;
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::SetState(const State state)
  {
    sync::MutexLocker l(m_stateGuard);
    DoSetState(state);
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::DoSetState(const State state)
  {
    m_state = state;
  }

  template<class TaskQueue>
  struct FixedThreadPool<TaskQueue>::ThreadPoolTimerTask :
    public TimerTask
  {
    typedef FixedThreadPool<TaskQueue> ThreadPool;

    ThreadPoolTimerTask(ThreadPool& thread_pool, TaskBase::Ptr& task)
    : m_thread_pool(thread_pool), m_task(task)
    {}

    virtual void DoRun()
    {
      if (m_task->IsRequestCancel())
      {
        m_task->Run(); // to make the task to cancelled state.
        CancelAsync();
        CheckCancellation();
        return;
      }
      // TODO: check isRequestedStop here.

      // If the task cannot be added, it means the
      // thread pool has been stopped.
      if (!m_thread_pool.AddTask(m_task))
      {
        CancelAsync();
        CheckCancellation();
      }
    }

    ThreadPool& m_thread_pool;
    TaskBase::Ptr m_task;
  };


  template<class TaskQueue>
  inline TaskBase::Ptr
  FixedThreadPool<TaskQueue>::AddTimerTask(TaskBase::Ptr task,
      TimeValue delay_in_ms)
  {
    return DoAddTimerTask(task, delay_in_ms);
  }

  template<class TaskQueue>
  inline TaskBase::Ptr
  FixedThreadPool<TaskQueue>::AddIntervalTask(TaskBase::Ptr task,
      TimeValue period_in_ms, bool is_run_now)
  {
    return DoAddIntervalTask(task, period_in_ms, is_run_now);
  }

  template<class TaskQueue>
  template<typename Func>
  inline TaskBase::Ptr
  FixedThreadPool<TaskQueue>::AddTimerTask(Func f,
      TimeValue delay_in_ms)
  {
    TaskBase::Ptr task(MakeFunctorTask(f));
    return DoAddTimerTask(task, delay_in_ms);
  }

  template<class TaskQueue>
  template<typename Func>
  inline TaskBase::Ptr
  FixedThreadPool<TaskQueue>::AddIntervalTask(Func f,
      TimeValue period_in_ms, bool is_run_now)
  {
    TaskBase::Ptr task(MakeFunctorTask(f));
    return DoAddIntervalTask(task, period_in_ms, is_run_now);
  }

  template<class TaskQueue>
  inline TaskBase::Ptr
  FixedThreadPool<TaskQueue>::DoAddTimerTask(TaskBase::Ptr task,
      TimeValue delay_in_ms)
  {
    if (m_isRequestStop)
    {
      return TaskBase::Ptr();
    }
    else if (!task)
    {
      return task;
    }

    TimerTask::Ptr timer_task(new ThreadPoolTimerTask(*this, task));

    {
      sync::MutexLocker lock(m_timerGuard);
      if (!m_timer)
      {
        m_timer.reset(new Timer);
      }
    }
    if (m_timer->RunLater(timer_task, delay_in_ms))
    {
      return task;
    }
    else
    {
      return TaskBase::Ptr();
    }
  }

  template<class TaskQueue>
  inline TaskBase::Ptr
  FixedThreadPool<TaskQueue>::DoAddIntervalTask(TaskBase::Ptr task,
      TimeValue period_in_ms, bool is_run_now)
  {
    if (m_isRequestStop)
    {
      return TaskBase::Ptr();
    }
    else if (!task)
    {
      return task;
    }

    TimerTask::Ptr timer_task(new ThreadPoolTimerTask(*this, task));
    {
      sync::MutexLocker lock(m_timerGuard);
      if (!m_timer)
      {
        m_timer.reset(new Timer);
      }
    }
    if (m_timer->RunEvery(timer_task, period_in_ms, is_run_now))
    {
      return task;
    }
    else
    {
      return TaskBase::Ptr();
    }
  }
}


#endif
