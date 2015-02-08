#ifndef _TPOOL_CYCLICBARRIER_H_
#define _TPOOL_CYCLICBARRIER_H_

#include "ConditionVariable.h"

#include <boost/noncopyable.hpp>

#include <cassert>

namespace tpool {
namespace sync {

class CyclicBarrier : private ::boost::noncopyable {
public:
    CyclicBarrier(const int count)
    : m_max_count(count), m_count(count), m_barrier_index(0)
    , m_reset_index(0)
    {
        assert(count > 0);
    }

    int Wait()
    {
        int ret = 0;
        MutexLocker lock(m_count_guard);
        --m_count;
        if (m_count == 0)
        {
            m_count_guard.NotifyAll();
            m_count = m_max_count;
            ++m_barrier_index;
        }
        else
        {
            const int current_barrier_index = m_barrier_index;
            const int current_reset_index = m_reset_index;
            while (current_barrier_index == m_barrier_index &&
                   current_reset_index == m_reset_index)
            {
                m_count_guard.Wait();
            }

            if (current_reset_index != m_reset_index)
            {
                ret = 1;
            }
        }

        return ret;
    }

    void Reset()
    {
        MutexLocker lock(m_count_guard);
        m_count_guard.NotifyAll();
        m_count = m_max_count;
        ++m_reset_index;
    }

private:
    const int m_max_count;
    int m_count;
    int m_barrier_index;
    int m_reset_index;
    MutexConditionVariable m_count_guard;
};

}  // namespace sync
}  // namespace tpool


#endif /* _TPOOL_CYCLICBARRIER_H_ */
