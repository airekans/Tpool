#include "MessageReader.h"
#include "MessageDispatcher.h"
#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

void MessageReader::Loop()
{
  using boost::asio::detail::socket_ops::network_to_host_long;

  char lengthBuffer[sizeof(long)] = {0};

  while (true)
    {
      size_t length = m_socket.Read(boost::asio::buffer(lengthBuffer));
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
  const int length = m_socket.Read(boost::asio::buffer(packageBuffer, packageLength));

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

