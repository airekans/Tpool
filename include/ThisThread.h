#ifndef _TPOOL_THISTHREAD_H_
#define _TPOOL_THISTHREAD_H_

#include "Likely.h"

namespace tpool {

extern __thread int t_cachedTid;
extern __thread char t_tidString[32];
extern __thread int t_tidStringLength;
extern __thread const char* t_threadName;
void CacheTid();

inline int Tid()
{
    if (TPOOL_UNLIKELY(t_cachedTid == 0))
    {
        CacheTid();
    }
    return t_cachedTid;
}

inline const char* TidString()
{
    return t_tidString;
}

inline int TidStringLength()
{
    return t_tidStringLength;
}

bool IsMainThread();


}  // namespace tpool


#endif /* _TPOOL_THISTHREAD_H_ */
