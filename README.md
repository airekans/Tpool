Tpool
======

Tpool is C++ thread pool implementation based on POSIX pthread. It is designed with simplicity in mind.

Intro
======

Tpool is very easy to use. Suppose you want to write a web server and handle the incoming request in the thread pool,
then you can write the following code to accomplish this task:

```cpp
#include "FixedThreadPool.h"
#include <boost/shared_ptr.hpp>
#include <tcp/socket.h> // note that this is a fake header, for illustration purpose

using namespace tpool;
using boost::shared_ptr;

// The request handler class
class DaytimeTask : public TaskBase {
public:
    DaytimeTask(shared_ptr<tcp::socket> socket)
    : m_socket(socket)
    {}
    
    virtual void DoRun()
    {
        time_t now = time(0);
        const string daytimeString = ctime(&now);
        socket.Write(daytimeString);
    }

private:
    shared_ptr<tcp::socket> m_socket;
};

int main(int argc, char** argv)
{
    LFixedThreadPool threadPool; // create the thread pool, with 4 threads by default
    // create a socket listen on port 12345
    tcp::acceptor acceptor(io_service,
                           tcp::endpoint(tcp::v4(), 12345));
    
    while (true)
    {
        shared_ptr<tcp::socket> socket(new tcp::socket);
        acceptor.accept(*socket);
        // Add the task to thread pool
        threadPool.AddTask(TaskBase::Ptr(new DaytimeTask(socket)));
    }

    return 0;
}
```
