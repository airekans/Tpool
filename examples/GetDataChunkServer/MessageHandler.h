// -*- mode: c++ -*-
#ifndef _GET_DATA_CHUNK_SERVER_MESSAGE_HANDLER_H_
#define _GET_DATA_CHUNK_SERVER_MESSAGE_HANDLER_H_

#include <google/protobuf/message.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>


class MessageHandler
{
public:
  typedef boost::shared_ptr<MessageHandler> Ptr;
  
  MessageHandler()
  {}
  
  virtual ~MessageHandler()
  {}

  virtual void HandleMessage(google::protobuf::Message* message) = 0;
};

template <typename T>
class FunctorMessageHandler : public MessageHandler
{
public:
  typedef boost::function<void (T* message)> Handler;
  
  FunctorMessageHandler(const Handler& handler)
    : m_handler(handler)
  {}
  
  virtual ~FunctorMessageHandler()
  {}

  virtual void HandleMessage(google::protobuf::Message* message)
  {
    T* concreteMessage = dynamic_cast<T*>(message);
    assert(concreteMessage != NULL);
    m_handler(concreteMessage);
  }

private:
  Handler m_handler;
};

#endif
