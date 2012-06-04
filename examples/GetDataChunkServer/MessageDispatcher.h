#ifndef _GET_DATA_CHUNK_SERVER_MESSAGE_DISPATCHER_H_
#define _GET_DATA_CHUNK_SERVER_MESSAGE_DISPATCHER_H_

#include "FixedThreadPool.h"
#include "DataChunk.pb.h"
#include <map>
#include <boost/function.hpp>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>


class MessageDispatcher {
public:
  typedef boost::function<void (google::protobuf::Message* message)> MessageHandler;
  
  void Dispatch(google::protobuf::Message* message);
  template <typename T>
  void SetMessageHandler(MessageHandler messageHandler);

  static MessageDispatcher& GetInstance()
  {
    static MessageDispatcher dispatcher;
    return dispatcher;
  }
  
private:
  void DefaultHandler(google::protobuf::Message* message) const;
  
  typedef std::map<const google::protobuf::Descriptor*, MessageHandler> HandlerMap;
  
  HandlerMap m_messageHandlers;
  tpool::LFixedThreadPool m_threadPool;
};


#endif
