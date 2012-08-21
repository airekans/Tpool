// -*- mode: c++ -*-
#ifndef _GET_DATA_CHUNK_SERVER_MESSAGE_DISPATCHER_H_
#define _GET_DATA_CHUNK_SERVER_MESSAGE_DISPATCHER_H_

#include "FixedThreadPool.h"
#include "DataChunk.pb.h"
#include "MessageHandler.h"
#include <map>
#include <boost/function.hpp>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>


class MessageDispatcher {
public:
  void Dispatch(google::protobuf::Message* message);
  template <typename T>
  void SetMessageHandler(const typename FunctorMessageHandler<T>::Handler& messageHandler);

  static MessageDispatcher& GetInstance();
  
private:
  void DefaultHandler(google::protobuf::Message* message) const;
  
  typedef std::map<const google::protobuf::Descriptor*,
		   MessageHandler::Ptr> HandlerMap;
  
  HandlerMap m_messageHandlers;
  tpool::LFixedThreadPool m_threadPool;
};

template <typename T>
void MessageDispatcher::SetMessageHandler(const typename FunctorMessageHandler<T>::Handler& handler)
{
  MessageHandler::Ptr messageHandler(new FunctorMessageHandler<T>(handler));
  m_messageHandlers[T::default_instance().GetDescriptor()] = messageHandler;
}

#endif
