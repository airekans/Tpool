#include "FixedThreadPool.h"
#include "TaskBase.h"
#include "DataChunk.pb.h"
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


void ProcessMessagePackage(shared_ptr<tcp::socket> socket, LFixedThreadPool& tp,
			   const int packageLength)
{
  char packageBuffer[packageLength];
  boost::system::error_code error;
  const int length = socket->read_some(boost::asio::buffer(packageBuffer, packageLength), error);

  const char* bufferPtr = packageBuffer;
  const int messageNameLength = *(int*) bufferPtr; // should convert the number;
  const int messageLength = packageLength - messageNameLength;
  bufferPtr += sizeof(messageNameLength);

  const string MessageName = bufferPtr; // '\0' terminated string
  bufferPtr += messageNameLength;
}


void ReadCommandLoop(shared_ptr<tcp::socket> socket, LFixedThreadPool& tp)
{
  char lengthBuffer[sizeof(int)] = {0};
  boost::system::error_code error;

  size_t length = socket->read_some(boost::asio::buffer(lengthBuffer), error);
  const int packageLength = *(int*) lengthBuffer; // should convert the number;

  ProcessMessagePackage(socket, tp, packageLength);
}



int main(int argc, char** argv)
{
  const unsigned int SERVER_PORT = 29995;
  
  try
    {
      LFixedThreadPool threadPool;
      boost::asio::io_service io_service;
      tcp::acceptor acceptor(io_service,
			     tcp::endpoint(tcp::v4(), SERVER_PORT));
      cout << "Server starts listening port " << SERVER_PORT << endl;
      
      shared_ptr<tcp::socket> socket(new tcp::socket(io_service));
      acceptor.accept(*socket);

      
      
      // Add the task to thread pool
      TaskBase::Ptr task(new DownloadTask(socket));
      threadPool.AddTask(task);

      // wait 5 seconds, then cancel the task
      sleep(5);
      if (TaskBase::FINISHED != task->GetState())
	{
	  task->Cancel();
	  cout << "DownloadTask is cancelled." << endl;
	}
    }
  catch (std::exception& e)
    {
      cerr << e.what() << endl;
    }

  return 0;
}
