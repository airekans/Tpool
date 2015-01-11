#include "WorkerThread.h"
#include "Thread.h"
#include "TaskQueueBase.h"
#include "TaskBase.h"
#include "EndTask.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <functional>

using namespace std;
using namespace tpool;
using namespace tpool::sync;
using namespace boost;

namespace {
struct NoOp {
    void operator()()
    {}
};
}  // namespace


WorkerThread::WorkerThread(TaskQueueBase::Ptr taskQueue)
{
    Init(taskQueue, NoOp());
}

WorkerThread::WorkerThread(TaskQueueBase::Ptr taskQueue,
        const Function& action)
{
    Init(taskQueue, action);
}

inline void WorkerThread::Init(TaskQueueBase::Ptr taskQueue,
        const Function& action)
{
    using boost::bind;

    m_taskQueue = taskQueue;

    // ensure that the thread is created successfully.
    while (true)
    {
        try
        {
            // check for the creation exception
            m_thread.reset(new CancelableThread(
                    bind(&WorkerThread::WorkFunction, this, _1), action));
            break;
        }
        catch (const std::exception& e)
        {
            ProcessError(e);
        }
    }
}

// dtor has to be defined for pimpl idiom
WorkerThread::~WorkerThread()
{}

void WorkerThread::Cancel()
{
    m_thread->Cancel();
}

void WorkerThread::CancelAsync()
{
    m_thread->CancelAsync();
}

void WorkerThread::CancelNow()
{
    {
        MutexLocker l(m_runningTaskGuard);
        if (m_runningTask)
        {
            m_runningTask->CancelAsync();
        }
    }
    Cancel();
}

void WorkerThread::ProcessError(const std::exception& e)
{
    cerr << "WorkerThread ctor" << endl;
    cerr << e.what() << endl;
    cerr << "Try again." << endl;
}

void WorkerThread::WorkFunction(const Function& checkFunc)
{
    while (true)
    {
        // 1. check cancel request
        checkFunc();

        // 2. fetch task from task queue
        GetTaskFromTaskQueue();

        // 2.5. check cancel request again
        checkFunc();

        // 3. perform the task
        if (m_runningTask)
        {
            if (dynamic_cast<EndTask*>(m_runningTask.get()) != NULL)
            {
                break; // stop the worker thread.
            }
            else
            {
                m_runningTask->Run();
            }
        }
        // 4. perform any post-task action
    }
}

void WorkerThread::GetTaskFromTaskQueue()
{
    MutexLocker l(m_runningTaskGuard);
    m_runningTask = m_taskQueue->Pop();
}

