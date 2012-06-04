#include "FixedThreadPool.h"
#include "TaskBase.h"
#include "DataChunk.pb.h"
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/bind/protect.hpp>
#include <boost/system/error_code.hpp>
#include <ctime>
#include <string>
#include <cstdlib>
#include <sstream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <map>
#include <boost/function.hpp>

using namespace tpool;
using boost::asio::ip::tcp;
using boost::shared_ptr;
using boost::function;
using namespace std;
using google::protobuf::Message;
using google::protobuf::Descriptor;

const size_t MAX_LENGTH = 1024;

namespace {
  LFixedThreadPool& GetThreadPool()
  {
    static LFixedThreadPool threadPool;
    return threadPool;
  }
}

class MessageDispatcher {
public:
  typedef function<void (Message* message)> MessageHandler;
  
  void Dispatch(Message* message);
  template <typename T>
  void SetMessageHandler(MessageHandler messageHandler);

  static MessageDispatcher& GetInstance()
  {
    static MessageDispatcher dispatcher;
    return dispatcher;
  }
  
private:
  void DefaultHandler(Message* message) const
  {
    using simple::GetSimpleDataChunkRequest;
    
    cout << "default handler" << endl;
    GetSimpleDataChunkRequest* request =
      dynamic_cast<GetSimpleDataChunkRequest*>(message);
    if (request != NULL)
      {
	cout << "Request: " << request->num() << endl;
      }
  }
  
  typedef map<const Descriptor*, MessageHandler> HandlerMap;
  
  HandlerMap m_messageHandlers;
  LFixedThreadPool m_threadPool;
};

class MessageReader {
public:
  MessageReader(shared_ptr<tcp::socket> socket)
    : m_socket(socket)
  {}

  void Loop();
  
private:
  void ProcessMessagePackage(const long packageLength);
  
  shared_ptr<tcp::socket> m_socket;
};

// Imlementation
void MessageDispatcher::Dispatch(Message* message)
{
  HandlerMap::const_iterator handler = m_messageHandlers.find(message->GetDescriptor());
  if (handler != m_messageHandlers.end())
    {
      m_threadPool.AddTask(boost::protect(boost::bind(handler->second, message)));
    }
  else
    {
      DefaultHandler(message);
    }
}

template <typename T>
void MessageDispatcher::SetMessageHandler(MessageHandler messageHandler)
{
  m_messageHandlers[T::default_instance().GetDescriptor()] = messageHandler;
}

void MessageReader::Loop()
{
  using boost::asio::detail::socket_ops::network_to_host_long;

  char lengthBuffer[sizeof(long)] = {0};
  boost::system::error_code error;

  while (true)
    {
      size_t length = m_socket->read_some(boost::asio::buffer(lengthBuffer), error);
      const long packageLengthNetwork = *(long*) lengthBuffer;
      const long packageLength = network_to_host_long(packageLengthNetwork);

      cout << "received " << packageLength << " bytes" << endl;

      ProcessMessagePackage(packageLength);
    }
}

void MessageReader::ProcessMessagePackage(const long packageLength)
{
  using boost::asio::detail::socket_ops::network_to_host_long;

  char packageBuffer[packageLength];
  boost::system::error_code error;
  const int length = m_socket->read_some(boost::asio::buffer(packageBuffer, packageLength), error);

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
  cout << "messageLength: " << messageLength << endl;

  const google::protobuf::Descriptor* descriptor =
    google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(messageName);
  const google::protobuf::Message* prototype =
    google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
  google::protobuf::Message* message = prototype->New();

  message->ParseFromArray(bufferPtr, messageLength);

  MessageDispatcher::GetInstance().Dispatch(message);
}

void HandleSimpleDataChunkRequest(const int, Message* message)
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
	(MessageDispatcher::MessageHandler(boost::bind(&HandleSimpleDataChunkRequest, 0, _1)));
      
      MessageReader reader(socket);
      reader.Loop();
    }
  catch (std::exception& e)
    {
      cerr << e.what() << endl;
    }

  return 0;
}
