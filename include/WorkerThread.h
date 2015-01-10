// -*- mode: c++ -*-
#ifndef _TPOOL_WORKER_THREAD_H_
#define _TPOOL_WORKER_THREAD_H_

#include "TaskQueueBase.h"
#include "ConditionVariable.h"
#include "CancelableThread.h"
#include "Atomic.h"

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

#include <memory>
#include <exception>

namespace tpool {

class WorkerThread : public boost::noncopyable {
private:
    enum State {
        INIT,
        RUNNING,
        FINISHED,
    };

public:
    typedef boost::shared_ptr<WorkerThread> Ptr;
    typedef CancelableThread::Function Function;

    WorkerThread(TaskQueueBase::Ptr taskQueue);
    WorkerThread(TaskQueueBase::Ptr taskQueue, const Function& action);
    ~WorkerThread();

    void Cancel();
    void CancelAsync();
    void CancelNow();

private:
    void Init(TaskQueueBase::Ptr taskQueue, const Function& action);
    void ProcessError(const std::exception& e);
    void WorkFunction(const Function& checkFunc);
    void GetTaskFromTaskQueue();

    TaskQueueBase::Ptr m_taskQueue;
    TaskBase::Ptr m_runningTask;
    mutable sync::Mutex m_runningTaskGuard;
    std::auto_ptr<CancelableThread> m_thread; // Thread must be the last variable
};


}  // namespace tpool

#endif
