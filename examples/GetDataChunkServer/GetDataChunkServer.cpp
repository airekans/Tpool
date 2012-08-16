#include "DataChunk.pb.h"
#include "MessageReader.h"
#include "MessageDispatcher.h"
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


void HandleSimpleDataChunkRequest(Message* message,
				  shared_ptr<tcp::socket> socket)
{
  using simple::GetSimpleDataChunkRequest;
  using simple::SimpleDataChunkResponse;

  cout << "In HandleSimpleDataChunkRequest" << endl;

  GetSimpleDataChunkRequest* request =
    dynamic_cast<GetSimpleDataChunkRequest*>(message);

  if (request == NULL)
    {
      return;
    }

  const int responseNum = request->num();
  cout << "responseNum: " << responseNum << endl;
  
  SimpleDataChunkResponse response;
  for (int i = 0; i < responseNum; ++i)
    {
      BOOST_AUTO(chunk, response.add_chunks());
      chunk->set_x((double) i);
      chunk->set_y((double) i);
    }

  ostringstream oss;
  response.SerializeToOstream(&oss);
  cout << "response length: " << oss.str().length() << endl;

  boost::system::error_code ignored_error;
  boost::asio::write(*socket, boost::asio::buffer(oss.str()),
		     boost::asio::transfer_all(), ignored_error);
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
	(boost::bind(&HandleSimpleDataChunkRequest, _1, socket));
      
      MessageReader reader(socket);
      reader.Loop();
    }
  catch (std::exception& e)
    {
      cerr << e.what() << endl;
    }

  return 0;
}
