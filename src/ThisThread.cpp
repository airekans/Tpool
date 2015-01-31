#include "ThisThread.h"

#include <cstdio>

#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>


namespace {

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

}  // namespace

namespace tpool {

__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "unknown";


void CacheTid()
{
    if (t_cachedTid == 0)
    {
        t_cachedTid = gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
    }
}

bool IsMainThread()
{
    return Tid() == ::getpid();
}


}  // namespace tpool

