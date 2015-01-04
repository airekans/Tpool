// -*- mode: c++ -*-
#ifndef _TPOOL_CANCELABLE_THREAD_H_
#define _TPOOL_CANCELABLE_THREAD_H_

#include "Thread.h"
#include "ConditionVariable.h"
#include "Mutex.h"
#include "Atomic.h"

#include <memory>
#include <exception>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace tpool {

  // This class can not be derived.
    class CancelableThread : private ::boost::noncopyable {
    private:
        typedef ::boost::function<void()> Function;
        typedef ::boost::function<void(Function)> WorkFunction;

    public:
        CancelableThread(WorkFunction workFunction);
        CancelableThread(WorkFunction workFunction, Function finishAction);
        ~CancelableThread();

        void Cancel();
        void CancelAsync();

    protected:
        enum State {
          INIT,
          RUNNING,
          FINISHED,
        };

    private:
        void Init();
        void CheckCancellation() const;
        void ProcessError(const std::exception& e);
        bool IsFinished() const;
        bool DoIsFinished() const;
        void SetState(const State state);
        void DoSetState(const State state);
        void GetTaskFromTaskQueue();
        void NotifyFinished();
        void ThreadFunction();

    private:
        State m_state;
        mutable sync::MutexConditionVariable m_stateGuard;
        Atomic<bool> m_isRequestCancel;
        WorkFunction m_workFunction;
        Function m_finishAction;
        std::auto_ptr<Thread> m_thread; // Thread must be the last variable
    };

}  // namespace tpool

#endif  // _TPOOL_CANCELABLE_THREAD_H_

