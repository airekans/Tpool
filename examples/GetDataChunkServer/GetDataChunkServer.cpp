#include "DataChunk.pb.h"
#include "MessageReader.h"
#include "MessageDispatcher.h"
#include "Socket.h"
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <google/protobuf/message.h>
#include <boost/typeof/typeof.hpp>
#include <sstream>

using namespace tpool;
using namespace std;
using boost::asio::ip::tcp;
using boost::shared_ptr;
using google::protobuf::Message;


Message* HandleSimpleDataChunkRequest(simple::GetSimpleDataChunkRequest* request)
{
  using simple::GetSimpleDataChunkRequest;
  using simple::SimpleDataChunkResponse;

  cout << "In HandleSimpleDataChunkRequest" << endl;

  const int responseNum = request->num();
  cout << "responseNum: " << responseNum << endl;
  
  SimpleDataChunkResponse* response = new SimpleDataChunkResponse;
  for (int i = 0; i < responseNum; ++i)
    {
      BOOST_AUTO(chunk, response->add_chunks());
      chunk->set_x((double) i);
      chunk->set_y((double) i);
    }

  ostringstream oss;
  response->SerializeToOstream(&oss);
  cout << "response length: " << oss.str().length() << endl;

  return response;
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
      
      Socket socket(boost::shared_ptr<tcp::socket>(new tcp::socket(io_service)));
      acceptor.accept(*(socket.GetImpl()));

      MessageDispatcher::GetInstance().SetSocket(socket);
      MessageDispatcher::GetInstance().
	SetMessageHandler<simple::GetSimpleDataChunkRequest>
	(boost::bind(&HandleSimpleDataChunkRequest, _1));
      
      MessageReader reader(socket);
      reader.Loop();
    }
  catch (std::exception& e)
    {
      cerr << e.what() << endl;
    }

  return 0;
}
