#ifndef _TPOOL_BLOCKINGQUEUE_H_
#define _TPOOL_BLOCKINGQUEUE_H_

#include "ConditionVariable.h"

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>

#include <queue>

namespace tpool {

template<typename T, typename QueueImpl = std::queue<T> >
class BlockingQueue : private ::boost::noncopyable {
private:
    typedef typename QueueImpl::container_type QueueContainer;

public:
    typedef T ElemType;
    typedef QueueImpl QueueImplType;

    BlockingQueue() {}

    explicit BlockingQueue(const QueueImpl& container)
    : m_queue(container)
    {}

    explicit BlockingQueue(const QueueContainer& container)
    : m_queue(container)
    {}


    void Push(const ElemType& elem)
    {
        sync::ConditionNotifyAllLocker l(m_mutexCond,
                boost::bind(&QueueImpl::empty, &m_queue));
        m_queue.push(elem);
    }

    // Return copy here to avoid concurrent acess issue
    ElemType Pop()
    {
        // wait until task queue is not empty
        sync::ConditionWaitLocker l(m_mutexCond,
                boost::bind(&QueueImpl::empty, &m_queue));

        ElemType elem = m_queue.front();
        m_queue.pop();
        return elem;
    }

    // Use this version of Pop if copying object is expensive
    void Pop(ElemType& elem)
    {
        // wait until task queue is not empty
        sync::ConditionWaitLocker l(m_mutexCond,
                boost::bind(&QueueImpl::empty, &m_queue));

        elem = m_queue.front();
        m_queue.pop();
    }

    bool NonblockingPop(ElemType& elem)
    {
        sync::MutexLocker l(m_mutexCond);
        if (m_queue.empty())
        {
            return false;
        }
        else
        {
            elem = m_queue.front();
            m_queue.pop();
            return true;
        }
    }

    ElemType Front() const
    {
        // wait until task queue is not empty
        sync::ConditionWaitLocker l(m_mutexCond,
                boost::bind(&QueueImpl::empty, &m_queue));
        return m_queue.front();
    }

    // Use this version of Front if copying object is expensive
    void Front(ElemType& elem) const
    {
        // wait until task queue is not empty
        sync::ConditionWaitLocker l(m_mutexCond,
                boost::bind(&QueueImpl::empty, &m_queue));

        elem = m_queue.front();
    }

    bool NonblockingFront(ElemType& elem) const
    {
        sync::MutexLocker l(m_mutexCond);
        if (m_queue.empty())
        {
            return false;
        }
        else
        {
            elem = m_queue.front();
            return true;
        }
    }

    size_t Size() const
    {
        sync::MutexLocker l(m_mutexCond);
        return m_queue.size();
    }

    bool IsEmpty() const
    {
        sync::MutexLocker l(m_mutexCond);
        return m_queue.empty();
    }

private:
    QueueImpl m_queue;
    mutable sync::MutexConditionVariable m_mutexCond;
};


}  // namespace tpool


#endif /* _TPOOL_BLOCKINGQUEUE_H_ */
