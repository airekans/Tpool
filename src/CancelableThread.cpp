#include "CancelableThread.h"

#include <boost/bind.hpp>

#include <cassert>
#include <string>
#include <iostream>


using namespace std;
using namespace tpool;

namespace {
    // This exception makes thread exit.
    class CancelableThreadExitException : public std::exception {
    public:
        explicit CancelableThreadExitException(const string& s)
        : m_exitMessage(s)
        {}

        virtual ~CancelableThreadExitException() throw()
        {}

        virtual const char* what() const throw()
        {
            return m_exitMessage.c_str();
        }

    private:
        string m_exitMessage;
    };
}  // namespace


tpool::CancelableThread::CancelableThread(WorkFunction workFunction)
: m_state(INIT), m_isRequestCancel(false), m_workFunction(workFunction)
{
    Init();
}

tpool::CancelableThread::CancelableThread(
    WorkFunction workFunction, Function finishAction)
: m_state(INIT), m_isRequestCancel(false), m_workFunction(workFunction),
  m_finishAction(finishAction)
{
    Init();
}

tpool::CancelableThread::~CancelableThread()
{
}

void tpool::CancelableThread::Cancel()
{
    CancelAsync();
    sync::ConditionWaitLocker l(m_stateGuard,
        ::boost::bind(not1(mem_fun(&CancelableThread::DoIsFinished)),
                      this));
}

void tpool::CancelableThread::CancelAsync()
{
    m_isRequestCancel = true;
}

bool tpool::CancelableThread::IsRequestCancel() const
{
    return m_isRequestCancel;
}

void tpool::CancelableThread::Init()
{
    using boost::bind;

    assert(m_workFunction);
    // ensure that the thread is created successfully.
    while (true)
    {
        try
        {
            // check for the creation exception
            m_thread.reset(new Thread(bind(
                &CancelableThread::ThreadFunction, this)));
            break;
        }
        catch (const std::exception& e)
        {
            ProcessError(e);
        }
    }
}

void tpool::CancelableThread::CheckCancellation() const
{
    if (m_isRequestCancel)
    {
        throw CancelableThreadExitException("cancelled");
    }
}

void tpool::CancelableThread::ProcessError(const std::exception& e)
{
    cerr << "CancelableThread ctor" << endl;
    cerr << e.what() << endl;
    cerr << "Try again." << endl;
}

bool tpool::CancelableThread::IsFinished() const
{
    sync::MutexLocker l(m_stateGuard);
    return DoIsFinished();
}

bool tpool::CancelableThread::DoIsFinished() const
{
    return m_state == FINISHED;
}

void tpool::CancelableThread::SetState(const State state)
{
    sync::MutexLocker l(m_stateGuard);
    DoSetState(state);
}

void tpool::CancelableThread::DoSetState(const State state)
{
    m_state = state;
}

void tpool::CancelableThread::NotifyFinished()
{
    sync::ConditionNotifyAllLocker l(m_stateGuard,
        ::boost::bind(&Atomic<bool>::GetData, &m_isRequestCancel));
    DoSetState(FINISHED);
}

void tpool::CancelableThread::ThreadFunction()
{
    SetState(RUNNING);
    try
    {
        CheckCancellation();
        m_workFunction(
            ::boost::bind(&CancelableThread::CheckCancellation, this));
    }
    catch (const CancelableThreadExitException&)
    {
        // stop the worker thread.
    }
    catch (...) // caught other exception
    {}

    if (m_finishAction)
    {
        m_finishAction();
    }
    NotifyFinished();
}

