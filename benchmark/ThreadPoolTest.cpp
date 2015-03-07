#include "FixedThreadPool.h"
#include "Atomic.h"

#include <cstdlib>
#include <iostream>

using namespace tpool;
using namespace std;


namespace {

class IncTask : public ::tpool::TaskBase
{
public:
    virtual void DoRun()
    {}
};

}

int main(int argc, char** argv)
{
    int times = 1000000;
    if (argc > 1)
    {
        times = atoi(argv[1]);
        if (times <= 0)
        {
            times = 1000000;
        }
    }

    {
        LFixedThreadPool thread_pool(4);
        TaskBase::Ptr task;
        for (int i = 0; i < times; ++i)
        {
            task.reset(new IncTask);
            thread_pool.AddTask(task);
        }
    }

    cout << "result: " << times << endl;

    return 0;
}


