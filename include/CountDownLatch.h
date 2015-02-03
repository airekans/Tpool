#ifndef _TPOOL_COUNTDOWNLATCH_H_
#define _TPOOL_COUNTDOWNLATCH_H_

#include "ConditionVariable.h"

#include <boost/noncopyable.hpp>

#include <cassert>

namespace tpool {
namespace sync {

class CountDownLatch : private ::boost::noncopyable {
public:
    explicit CountDownLatch(const int count)
    : m_count(count)
    {
        assert(count > 0);
    }

    void Wait()
    {
        MutexLocker lock(m_count_guard);
        while (m_count > 0)
        {
            m_count_guard.Wait();
        }
    }

    void CountDown()
    {
        MutexLocker lock(m_count_guard);
        --m_count;
        if (m_count == 0)
        {
            m_count_guard.NotifyAll();
        }
    }

    int GetCount() const
    {
        MutexLocker lock(m_count_guard);
        return m_count;
    }

private:
    int m_count;
    mutable MutexConditionVariable m_count_guard;
};

}  // namespace sync
}  // namespace tpool


#endif /* _TPOOL_COUNTDOWNLATCH_H_ */

