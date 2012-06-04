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

namespace {
  LFixedThreadPool& GetThreadPool()
  {
    static LFixedThreadPool threadPool;
    return threadPool;
  }
}


void ProcessMessagePackage(shared_ptr<tcp::socket> socket,
			   const long packageLength)
{
  using boost::asio::detail::socket_ops::network_to_host_long;

  char packageBuffer[packageLength];
  boost::system::error_code error;
  const int length = socket->read_some(boost::asio::buffer(packageBuffer, packageLength), error);

  cout << "received buffer contents: ";
  for (int i = 0; i < length; ++i)
    {
      cout << packageBuffer[i];
    }
  cout << endl;
  
  const char* bufferPtr = packageBuffer;
  const long messageNameLengthNetwork = *(long*) bufferPtr;
  const long messageNameLength = network_to_host_long(messageNameLengthNetwork);

  cout << "messageName Length: " << messageNameLength << endl;
  
  const int messageLength = packageLength - sizeof(messageNameLength) - messageNameLength;
  bufferPtr += sizeof(messageNameLength);

  const string messageName = bufferPtr; // '\0' terminated string
  bufferPtr += messageNameLength;

  cout << "messageName: " << messageName << endl;
}


void ReadCommandLoop(shared_ptr<tcp::socket> socket)
{
  using boost::asio::detail::socket_ops::network_to_host_long;

  char lengthBuffer[sizeof(long)] = {0};
  boost::system::error_code error;

  while (true)
    {
      size_t length = socket->read_some(boost::asio::buffer(lengthBuffer), error);
      const long packageLengthNetwork = *(long*) lengthBuffer;
      const long packageLength = network_to_host_long(packageLengthNetwork);

      cout << "received " << packageLength << " bytes" << endl;

      ProcessMessagePackage(socket, packageLength);
    }
}



int main(int argc, char** argv)
{
  const unsigned int SERVER_PORT = 29995;
  
  try
    {
      boost::asio::io_service io_service;
      tcp::acceptor acceptor(io_service,
			     tcp::endpoint(tcp::v4(), SERVER_PORT));
      cout << "Server starts listening port " << SERVER_PORT << endl;
      
      shared_ptr<tcp::socket> socket(new tcp::socket(io_service));
      acceptor.accept(*socket);

      ReadCommandLoop(socket);
    }
  catch (std::exception& e)
    {
      cerr << e.what() << endl;
    }

  return 0;
}
