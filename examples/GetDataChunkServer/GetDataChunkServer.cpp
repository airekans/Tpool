#include "DataChunk.pb.h"
#include "MessageReader.h"
#include "MessageDispatcher.h"
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <google/protobuf/message.h>

using namespace tpool;
using namespace std;
using boost::asio::ip::tcp;
using boost::shared_ptr;
using google::protobuf::Message;


void HandleSimpleDataChunkRequest(Message* message)
{
  cout << "In HandleSimpleDataChunkRequest" << endl;
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

      MessageDispatcher::GetInstance().
	SetMessageHandler<simple::GetSimpleDataChunkRequest>
	(MessageDispatcher::MessageHandler(boost::bind(&HandleSimpleDataChunkRequest, _1)));
      
      MessageReader reader(socket);
      reader.Loop();
    }
  catch (std::exception& e)
    {
      cerr << e.what() << endl;
    }

  return 0;
}
