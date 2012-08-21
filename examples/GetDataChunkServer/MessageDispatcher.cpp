#include "MessageDispatcher.h"
#include "HandleMessageTask.h"
#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <cassert>

using namespace std;
using namespace google::protobuf;
using tpool::TaskBase;

void MessageDispatcher::SetSocket(Socket& socket)
{
  m_socket = &socket;
}

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
      assert(m_socket != NULL);
      m_threadPool.AddTask(TaskBase::Ptr(new HandleMessageTask(handler->second,
							       message,
							       *m_socket)));
    }
  else
    {
      DefaultHandler(message);
    }
}


