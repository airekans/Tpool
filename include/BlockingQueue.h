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

    BlockingQueue()
    : m_waiting_count(0)
    {}

    explicit BlockingQueue(const QueueImpl& container)
    : m_queue(container), m_waiting_count(0)
    {}

    explicit BlockingQueue(const QueueContainer& container)
    : m_queue(container), m_waiting_count(0)
    {}

    void Push(const ElemType& elem)
    {
        sync::MutexLocker lock(m_mutexCond);
        m_queue.push(elem);
        if (m_waiting_count > 0)
        {
            m_mutexCond.NotifyAll();
            m_waiting_count = 0;
        }
    }

    // Return copy here to avoid concurrent acess issue
    ElemType Pop()
    {
        ElemType elem;
        // wait until task queue is not empty
//        for (;;)
//        {
//            if (NonblockingPop(elem))
//            {
//                return elem;
//            }
//        }
        sync::MutexLocker lock(m_mutexCond);
        while (m_queue.empty())
        {
            ++m_waiting_count;
            m_mutexCond.Wait();
        }

        elem = m_queue.front();
        m_queue.pop();
        return elem;
    }

    // Use this version of Pop if copying object is expensive
    void Pop(ElemType& elem)
    {
        // wait until task queue is not empty
        sync::MutexLocker lock(m_mutexCond);
        while (m_queue.empty())
        {
            ++m_waiting_count;
            m_mutexCond.Wait();
        }

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
        sync::MutexLocker lock(m_mutexCond);
        while (m_queue.empty())
        {
            ++m_waiting_count;
            m_mutexCond.Wait();
        }
        return m_queue.front();
    }

    // Use this version of Front if copying object is expensive
    void Front(ElemType& elem) const
    {
        // wait until task queue is not empty
        sync::MutexLocker lock(m_mutexCond);
        while (m_queue.empty())
        {
            ++m_waiting_count;
            m_mutexCond.Wait();
        }

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
    mutable unsigned int m_waiting_count;
    mutable sync::MutexConditionVariable m_mutexCond;
};


}  // namespace tpool


#endif /* _TPOOL_BLOCKINGQUEUE_H_ */
