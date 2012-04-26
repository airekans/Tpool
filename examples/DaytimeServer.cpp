#include "FixedThreadPool.h"
#include "TaskBase.h"
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <ctime>
#include <string>
#include <cstdlib>

using namespace tpool;
using boost::asio::ip::tcp;
using boost::shared_ptr;
using namespace std;

class DaytimeTask : public TaskBase {
public:
  DaytimeTask(shared_ptr<tcp::socket> socket)
    : m_socket(socket)
  {}

  virtual void DoRun()
  {
    cout << "Process Daytime Request." << endl;

    sleep(1); // simulate the 1 second delay.
    const string daytimeString = GetDaytimeString();
    
    boost::system::error_code ignored_error;
    boost::asio::write(*m_socket, boost::asio::buffer(daytimeString),
		       boost::asio::transfer_all(), ignored_error);
  }

private:
  string GetDaytimeString() const
  {
    time_t now = time(0);
    return ctime(&now);
  }
  
  shared_ptr<tcp::socket> m_socket;
};


int main(int argc, char** argv)
{
  try
    {
      LFixedThreadPool threadPool;
      boost::asio::io_service io_service;
      tcp::acceptor acceptor(io_service,
			     tcp::endpoint(tcp::v4(), 1234));
      
      int cnt = 0;
      while (true)
	{
	  shared_ptr<tcp::socket> socket(new tcp::socket(io_service));
	  acceptor.accept(*socket);

	  // Add the task to thread pool
	  threadPool.AddTask(TaskBase::Ptr(new DaytimeTask(socket)));
	  cout << "Received Connection: " << ++cnt << endl;
	}
    }
  catch (std::exception& e)
    {
      cerr << e.what() << endl;
    }

  return 0;
}
