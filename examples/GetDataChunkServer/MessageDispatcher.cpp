#include "MessageDispatcher.h"
#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

using namespace std;
using namespace google::protobuf;


MessageDispatcher& MessageDispatcher::GetInstance()
{
  static MessageDispatcher dispatcher;
  return dispatcher;
}

void MessageDispatcher::DefaultHandler(Message* message) const
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


