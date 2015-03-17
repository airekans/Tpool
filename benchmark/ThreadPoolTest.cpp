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
    int thread_num = 4;
    if (argc > 1)
    {
        times = atoi(argv[1]);
        if (times <= 0)
        {
            times = 1000000;
        }
    }
    if (argc > 2)
    {
        thread_num = atoi(argv[2]);
        if (thread_num <= 0)
        {
            thread_num = 4;
        }
    }


    {
        LFixedThreadPool thread_pool(thread_num);
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


