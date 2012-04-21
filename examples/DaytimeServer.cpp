#include "ThreadPool.h"
#include "TaskBase.h"
#include <exception>
#include <iostream>
#include <boost/asio.hpp>

using namespace tpool;
using boost::asio::ip::tcp;
using boost::shared_ptr;
using namespace std;

class DaytimeTask : public TaskBase {
public:
  DaytimeTask(shared_ptr<tcp::socket> socket)
    : m_socket(socket)
  {}

  virtual void Do()
  {

    
  }

private:
  shared_ptr<tcp::socket> m_socket;
};


int main(int argc, char** argv)
{
  try
    {
      LThreadPool threadPool;
      boost::asio::io_service io_service;
      tcp::acceptor acceptor(io_service,
			     tcp::endpoint(tcp::v4(), 1234));

      while (true)
	{
	  shared_ptr<tcp::socket> socket(new tcp::socket(io_service));
	  acceptor.accept(*socket);

	  // Add the task to thread pool
	  threadPool.AddTask(TaskBase::Ptr(new DaytimeTask(socket)));
	}
    }
  catch (std::exception& e)
    {
      cerr << e.what() << endl;
    }

  return 0;
}
