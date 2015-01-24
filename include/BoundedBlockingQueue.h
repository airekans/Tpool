#ifndef _TPOOL_BOUNDEDBLOCKINGQUEUE_H_
#define _TPOOL_BOUNDEDBLOCKINGQUEUE_H_

#include "Mutex.h"
#include "ConditionVariable.h"

#include <boost/noncopyable.hpp>

#include <queue>

namespace tpool {

template<typename T, typename QueueImpl = std::queue<T> >
class BoundedBlockingQueue : private boost::noncopyable {
private:
    typedef BoundedBlockingQueue<T, QueueImpl> Self;

public:
    typedef T ElemType;
    typedef QueueImpl QueueImplType;

    explicit BoundedBlockingQueue(size_t max_size)
    : m_max_size(max_size), m_queue_empty_cond(m_queue_guard)
    , m_queue_full_cond(m_queue_guard)
    {}

    void Push(const T& elem)
    {
        sync::ConditionWaitLocker l(m_queue_full_cond,
                boost::bind(&Self::IsFullWithLockHeld, this));
        if (m_queue.empty())
        {
            m_queue_empty_cond.NotifyAll();
        }
        // queue is not full
        m_queue.push(elem);
    }

    bool NonblockingPush(const T& elem)
    {
        sync::MutexLocker lock(m_queue_guard);
        if (m_queue.size() == m_max_size)
        {
            return false;
        }
        if (m_queue.empty())
        {
            m_queue_empty_cond.NotifyAll();
        }
        m_queue.push(elem);
        return true;
    }

    T Pop()
    {
        T elem;
        Pop(elem);
        return elem;
    }

    void Pop(T& elem)
    {
        // wait until task queue is not empty
        sync::ConditionWaitLocker l(m_queue_empty_cond,
                boost::bind(&QueueImpl::empty, &m_queue));

        if (m_queue.size() == m_max_size)
        {
            m_queue_full_cond.NotifyAll();
        }

        elem = m_queue.front();
        m_queue.pop();
    }

    bool NonblockingPop(T& elem)
    {
        sync::MutexLocker lock(m_queue_guard);
        if (m_queue.empty())
        {
            return false;
        }
        else
        {
            if (m_queue.size() == m_max_size)
            {
                m_queue_full_cond.NotifyAll();
            }

            elem = m_queue.front();
            m_queue.pop();
            return true;
        }
    }

    T Front() const
    {
        // wait until task queue is not empty
        sync::ConditionWaitLocker l(m_queue_empty_cond,
                boost::bind(&QueueImpl::empty, &m_queue));
        return m_queue.front();
    }

    void Front(T& elem) const
    {
        sync::ConditionWaitLocker l(m_queue_empty_cond,
                boost::bind(&QueueImpl::empty, &m_queue));
        elem = m_queue.front();
    }

    bool NonblockingFront(T& elem) const
    {
        sync::MutexLocker lock(m_queue_guard);
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
        sync::MutexLocker lock(m_queue_guard);
        return m_queue.size();
    }

    bool IsFull() const
    {
        sync::MutexLocker lock(m_queue_guard);
        return m_queue.size() == m_max_size;
    }

    bool IsEmpty() const
    {
        sync::MutexLocker lock(m_queue_guard);
        return m_queue.empty();
    }

private:
    bool IsFullWithLockHeld() const
    {
        return m_queue.size() == m_max_size;
    }

    const size_t m_max_size;
    QueueImpl m_queue;
    mutable sync::Mutex m_queue_guard;
    mutable sync::ConditionVariable m_queue_empty_cond;
    mutable sync::ConditionVariable m_queue_full_cond;
};

}  // namespace tpool

#endif /* _TPOOL_BOUNDEDBLOCKINGQUEUE_H_ */
