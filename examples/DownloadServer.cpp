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

const size_t MAX_LENGTH = 1024;

class DownloadTask : public TaskBase {
public:
  DownloadTask(shared_ptr<tcp::socket> socket)
    : m_socket(socket)
  {}

  virtual void DoRun()
  {
    cout << "Process Download Request." << endl;

    char data[MAX_LENGTH] = {0};

    while (true)
      {
	CheckCancellation();

	boost::system::error_code error;
	size_t length = m_socket->read_some(boost::asio::buffer(data), error);
	if (error == boost::asio::error::eof)
	  break; // Connection closed cleanly by peer.
	else if (error)
	  throw boost::system::system_error(error); // Some other error.

	ProcessData(data);
      }
  }

private:
  void ProcessData(char data[])
  {
    // Save the data to file. Here I ignore the data and
    // just output it to terminal
    cout << "received data" << endl;
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
			     tcp::endpoint(tcp::v4(), 29994));
      
      int cnt = 0;
      while (true)
	{
	  shared_ptr<tcp::socket> socket(new tcp::socket(io_service));
	  acceptor.accept(*socket);

	  // Add the task to thread pool
	  TaskBase::Ptr task(new DownloadTask(socket));
	  threadPool.AddTask(task);
	  cout << "Received Connection: " << ++cnt << endl;

	  // wait 5 seconds, then cancel the task
	  sleep(5);
	  if (TaskBase::FINISHED != task->GetState())
	    {
	      task->Cancel();
	      cout << "DownloadTask is cancelled." << endl;
	    }
	}
    }
  catch (std::exception& e)
    {
      cerr << e.what() << endl;
    }

  return 0;
}
